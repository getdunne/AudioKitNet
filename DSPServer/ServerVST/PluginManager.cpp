#include "PluginManager.h"
#include <Windows.h>
#include "TRACE.h"

static const VstInt32 kBlockSize = 512;
static const float kSampleRate = 44100.0f;


static VstTimeInfo timeInfo;

static VstIntPtr VSTCALLBACK HostCallback(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
    VstIntPtr result = 0;
    VstInt32 flags;

    // Filter idle calls...
    bool filtered = false;
    if (opcode == audioMasterIdle)
    {
        static bool wasIdle = false;
        if (wasIdle)
            filtered = true;
        else
        {
            TRACE("(Future idle calls will not be displayed!)\n");
            wasIdle = true;
        }
    }

    //if (!filtered) TRACE("PLUG> HostCallback (opcode %d)\n index = %d, value = %p, ptr = %p, opt = %f\n", opcode, index, FromVstPtr<void>(value), ptr, opt);

    switch (opcode)
    {
    case audioMasterVersion:
        result = kVstVersion;
        break;

    case audioMasterCanDo:
        //TRACE(" --> CanDo: %s\n", (char*)ptr);
        break;

    case audioMasterGetTime:
        flags = (VstInt32)value;
        //TRACE("PLUG> HostCallback audioMasterGetTime\n index = %d, flags = %04X\n", index, flags);
        timeInfo.sampleRate = double(kSampleRate);
        timeInfo.flags = 0;//kVstTransportPlaying | kVstPpqPosValid | kVstTempoValid | kVstBarsValid | kVstTimeSigValid;
        result = ToVstPtr<VstTimeInfo>(&timeInfo);
        break;

    }

    return result;
}

struct MyDLGTEMPLATE : DLGTEMPLATE
{
    WORD ext[3];
    MyDLGTEMPLATE()
    {
        memset(this, 0, sizeof(*this));
    };
};

INT_PTR CALLBACK EditorProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    AEffect* plugin = (AEffect*)lParam;

    switch (msg)
    {
    case WM_INITDIALOG:
        SetWindowText(hwnd, "VST Editor");
        SetTimer(hwnd, 1, 20, 0);

        if (plugin)
        {
            plugin->dispatcher(plugin, effEditOpen, 0, 0, hwnd, 0);

            ERect* eRect = 0;
            plugin->dispatcher(plugin, effEditGetRect, 0, 0, &eRect, 0);
            if (eRect)
            {
                int width = eRect->right - eRect->left;
                int height = eRect->bottom - eRect->top;
                if (width < 100)
                    width = 100;
                if (height < 100)
                    height = 100;

                RECT wRect;
                SetRect(&wRect, 0, 0, width, height);
                AdjustWindowRectEx(&wRect, GetWindowLong(hwnd, GWL_STYLE), FALSE, GetWindowLong(hwnd, GWL_EXSTYLE));
                width = wRect.right - wRect.left;
                height = wRect.bottom - wRect.top;

                SetWindowPos(hwnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
            }
        }
        break;

    case WM_TIMER:
        if (plugin) plugin->dispatcher(plugin, effEditIdle, 0, 0, 0, 0);
        break;

    case WM_CLOSE:
        KillTimer(hwnd, 1);
        if (plugin) plugin->dispatcher(plugin, effEditClose, 0, 0, 0, 0);
        EndDialog(hwnd, IDOK);
     	break;
    }

    return 0;
}


PluginManager::PluginManager()
    : module(0), mainProc(0), plugin(0)
    , inputs(0), outputs(0), blockSize(0)
{
}

PluginManager::~PluginManager()
{
    if (module)
    {
        if (inputs)
        {
            for (VstInt32 i = 2; i < plugin->numInputs; i++) delete[] inputs[i];
            if (plugin->numInputs > 0) delete[] inputs;
        }

        if (outputs)
        {
            for (VstInt32 i = 2; i < plugin->numOutputs; i++) delete[] outputs[i];
            delete[] outputs;
        }

        FreeLibrary((HMODULE)module);
    }
}

bool PluginManager::load(const char* pathToPluginDll)
{
    module = LoadLibrary(pathToPluginDll);
    if (module == 0) return false;

    mainProc = (PluginEntryProc)GetProcAddress((HMODULE)module, "VSTPluginMain");
    if (!mainProc) mainProc = (PluginEntryProc)GetProcAddress((HMODULE)module, "main");
    return mainProc != 0;
}

bool PluginManager::open()
{
    if (module == 0) return false;

    // instantiate the plugin
    plugin = mainProc(HostCallback);
    if (plugin == 0) return false;

    // preliminary setup
    plugin->dispatcher(plugin, effOpen, 0, 0, 0, 0);
    plugin->dispatcher(plugin, effSetSampleRate, 0, 0, 0, kSampleRate);

    // allocate inputs, outputs pointer arrays, but make the actual pointers null
    if (plugin->numInputs > 0)
    {
        inputs = new float*[plugin->numInputs];
        memset(inputs, 0, sizeof(float) * plugin->numInputs);
    }
    if (plugin->numOutputs > 0)
    {
        outputs = new float*[plugin->numOutputs];
        memset(outputs, 0, sizeof(float) * plugin->numOutputs);
    }

    return true;
}

// Open the plugin's custom GUI as a MODAL dialog. This only practical for occasional peeps at the GUI,
// because it blocks the main thread. However, the GUI is fully functional and the render thread is
// still going, so you can use the GUI interactively while it's open.
bool PluginManager::openCustomGui()
{
    if ((plugin->flags & effFlagsHasEditor) == 0)
    {
        TRACE("Plugin does not have an editor!\n");
        return false;
    }

    MyDLGTEMPLATE t;
    t.style = WS_POPUPWINDOW | WS_DLGFRAME | DS_MODALFRAME | DS_CENTER;
    t.cx = 100;
    t.cy = 100;
    DialogBoxIndirectParam(GetModuleHandle(0), &t, 0, (DLGPROC)EditorProc, (LPARAM)plugin);

    return true;
}

void PluginManager::checkProperties()
{
    if (module == 0 || plugin == 0) return;

    char effectName[256] = { 0 };
    char vendorString[256] = { 0 };
    char productString[256] = { 0 };

    plugin->dispatcher(plugin, effGetEffectName, 0, 0, effectName, 0);
    plugin->dispatcher(plugin, effGetVendorString, 0, 0, vendorString, 0);
    plugin->dispatcher(plugin, effGetProductString, 0, 0, productString, 0);

    printf("Name = %s\nVendor = %s\nProduct = %s\n\n", effectName, vendorString, productString);
    printf("numPrograms = %d\nnumParams = %d\nnumInputs = %d\nnumOutputs = %d\n\n",
        plugin->numPrograms, plugin->numParams, plugin->numInputs, plugin->numOutputs);

    // Programs
    for (VstInt32 progIndex = 0; progIndex < plugin->numPrograms; progIndex++)
    {
        char progName[256] = { 0 };
        if (!plugin->dispatcher(plugin, effGetProgramNameIndexed, progIndex, 0, progName, 0))
        {
            plugin->dispatcher(plugin, effSetProgram, 0, progIndex, 0, 0); // Note: old program not restored here!
            plugin->dispatcher(plugin, effGetProgramName, 0, 0, progName, 0);
        }
        printf("Program %03d: %s\n", progIndex, progName);
    }
    printf("\n");

    // Parameters
    for (VstInt32 paramIndex = 0; paramIndex < plugin->numParams; paramIndex++)
    {
        char paramName[256] = { 0 };
        char paramLabel[256] = { 0 };
        char paramDisplay[256] = { 0 };

        plugin->dispatcher(plugin, effGetParamName, paramIndex, 0, paramName, 0);
        plugin->dispatcher(plugin, effGetParamLabel, paramIndex, 0, paramLabel, 0);
        plugin->dispatcher(plugin, effGetParamDisplay, paramIndex, 0, paramDisplay, 0);
        float value = plugin->getParameter(plugin, paramIndex);

        printf("Param %03d: %s [%s %s] (normalized = %f)\n", paramIndex, paramName, paramDisplay, paramLabel, value);
    }

    // Can-dos
    static const char* canDos[] =
    {
        "receiveVstEvents",
        "receiveVstMidiEvent",
        "midiProgramNames"
    };

    for (VstInt32 canDoIndex = 0; canDoIndex < sizeof(canDos) / sizeof(canDos[0]); canDoIndex++)
    {
        printf("Can do %s... ", canDos[canDoIndex]);
        VstInt32 result = (VstInt32)plugin->dispatcher(plugin, effCanDo, 0, 0, (void*)canDos[canDoIndex], 0);
        switch (result)
        {
        case 0: printf("don't know"); break;
        case 1: printf("yes"); break;
        case -1: printf("definitely not"); break;
        default: printf("?????");
        }
        printf("\n");
    }

    printf("\n");
}

// Construct a std::map from parameter names to indices. This is only useful if the names are unique.
void PluginManager::setupParamLookup(ParameterMap &pmap)
{
    for (VstInt32 paramIndex = 0; paramIndex < plugin->numParams; paramIndex++)
    {
        char paramName[256] = { 0 };
        plugin->dispatcher(plugin, effGetParamName, paramIndex, 0, paramName, 0);
        pmap[paramName] = paramIndex;
    }
}

float PluginManager::getParameter(int index)
{
    if (module == 0 || plugin == 0) return 0.0f;

    return plugin->getParameter(plugin, (VstInt32)index);
}

void PluginManager::setParameter(int index, float value)
{
    if (module == 0 || plugin == 0) return;

    plugin->setParameter(plugin, (VstInt32)index, value);
}

void PluginManager::setProgram(int index)
{
    if (module == 0 || plugin == 0) return;

    plugin->dispatcher(plugin, effBeginSetProgram, 0, 0, 0, 0);
    plugin->dispatcher(plugin, effSetProgram, 0, index, 0, 0);
    plugin->dispatcher(plugin, effEndSetProgram, 0, 0, 0, 0);
}

void PluginManager::setBlockSize(int blockSize, float** buffers)
{
    if (module == 0) return;

    if (this->blockSize == blockSize) return;
    this->blockSize = blockSize;

    if (plugin->numInputs >= 1) inputs[0] = buffers[0];
    if (plugin->numInputs >= 2) inputs[1] = buffers[1];
    if (plugin->numOutputs >= 1) outputs[0] = buffers[2];
    if (plugin->numOutputs >= 2) outputs[1] = buffers[3];

    for (VstInt32 i = 2; i < plugin->numInputs; i++)
    {
        if (inputs[i]) delete[] inputs[i];
        inputs[i] = new float[blockSize];
        memset(inputs[i], 0, blockSize * sizeof(float));
    }

    for (VstInt32 i = 2; i < plugin->numOutputs; i++)
    {
        if (outputs[i]) delete[] outputs[i];
        outputs[i] = new float[blockSize];
        memset(outputs[i], 0, blockSize * sizeof(float));
    }
}

void PluginManager::processMidi(MIDIMessageInfoStruct* pMidi, int nMessages)
{
    if (module == 0 || plugin == 0) return;

    vstEvents.numEvents = nMessages;
    vstEvents.alwaysZero = 0;

    for (int i = 0; i < nMessages; i++, pMidi++)
    {
        VstMidiEvent* ev = &midiEvents[i];
        memset(ev, 0, sizeof(VstMidiEvent));
        ev->type = kVstMidiType;
        ev->byteSize = sizeof(VstMidiEvent);
        ev->deltaFrames = pMidi->startFrame;
        ev->midiData[0] = pMidi->channel | pMidi->status;
        ev->midiData[1] = pMidi->data1;
        ev->midiData[2] = pMidi->data2;
        vstEvents.events[i] = (VstEvent*)ev;
    }

    plugin->dispatcher(plugin, effProcessEvents, 0, 0, &vstEvents, 0.0f);
}

void PluginManager::processBlock()
{
    if (module == 0 || plugin == 0) return;

    plugin->processReplacing(plugin, inputs, outputs, blockSize);
}

void PluginManager::resume()
{
    if (module == 0 || plugin == 0) return;

    plugin->dispatcher(plugin, effMainsChanged, 0, 1, 0, 0);
}

void PluginManager::suspend()
{
    if (module == 0 || plugin == 0) return;

    plugin->dispatcher(plugin, effMainsChanged, 0, 0, 0, 0);
}

void PluginManager::close()
{
    if (module == 0 || plugin == 0) return;

    plugin->dispatcher(plugin, effClose, 0, 0, 0, 0);
}

