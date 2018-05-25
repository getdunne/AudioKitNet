#include "NetWrapper.h"
#include "TRACE.h"
#include <windows.h>

typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);
static VstIntPtr VSTCALLBACK HostCallback (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);

// Uncomment at most one. If all are commented out, a file dialog will be presented
//#define PLUGIN_PATH "C:\\VST_OLD\\Phadiz(P)\\Phadiz(P).dll";
//#define PLUGIN_PATH "C:\\VST32\\String Studio VS-2.dll";
#define PLUGIN_PATH "C:\\TubeOhm-T-FM\\T-FM1-48.dll";

#define RUN_LOCALLY
const char serverAddr[] = "127.0.0.1";
const int serverPort = 27016;

AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
#ifdef PLUGIN_PATH
    char pluginPath[] = PLUGIN_PATH;
#else
    char pluginPath[MAX_PATH];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrDefExt = "dll";
    ofn.lpstrFile = pluginPath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Choose plugin DLL";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    GetOpenFileName(&ofn);
    if (!GetOpenFileName(&ofn)) return NULL;
#endif

    // load wrapped plugin DLL
    HMODULE pModule = LoadLibrary(pluginPath);
	if (!pModule)
	{
		TRACE("Failed to load VST Plugin library!\n");
		return NULL;
	}

    // get DLL main entry point
	PluginEntryProc pluginMainProc = 0;
	pluginMainProc = (PluginEntryProc)GetProcAddress((HMODULE)pModule, "VSTPluginMain");
	if (!pluginMainProc)
		pluginMainProc = (PluginEntryProc)GetProcAddress((HMODULE)pModule, "main");
	if (!pluginMainProc)
	{
		TRACE("VST Plugin main entry not found!\n");
		return NULL;
	}

	// call main entry point to create plugin, passing in our callback
	AEffect* effect = pluginMainProc(HostCallback);
	if (!effect)
	{
		TRACE("Failed to create effect instance!\n");
		return NULL;
	}

    // create wrapper object
	return new NetWrapper(pModule, effect, audioMaster, effect->numPrograms, effect->numParams);
}

//#define FAKE_TIMEINFO

#ifdef FAKE_TIMEINFO
// VSTTimeInfo structure and some related scale factors
static VstTimeInfo timeInfo;
static double secondsPerSample, beatsPerSecond, qtrPerBeat, qtrPerBar;
#endif

VstIntPtr VSTCALLBACK HostCallback (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	VstIntPtr result = 0;
    NetWrapper* pWrapper = effect==0 ? 0 : (NetWrapper*)effect->user;
    VstInt32 flags;

	switch (opcode)
	{
    case audioMasterAutomate:
        //TRACE("Set param %d to value %f\n", index, opt);
#ifndef RUN_LOCALLY
        if (pWrapper) pWrapper->StuffParamMsg(index, opt);
#endif
        break;

	case audioMasterVersion:
		result = kVstVersion;
		break;

    case audioMasterIdle:
        // ignore idle callbacks
        break;

    case audioMasterGetTime:
        flags = (VstInt32)value;
#ifdef FAKE_TIMEINFO
	    //TRACE("PLUG> HostCallback audioMasterGetTime\n index = %d, flags = %04X\n", index, flags);
        timeInfo.flags = kVstTransportPlaying | kVstPpqPosValid | kVstTempoValid | kVstBarsValid | kVstTimeSigValid;
        result = ToVstPtr<VstTimeInfo>(&timeInfo);
#else
        result = ToVstPtr<VstTimeInfo>(pWrapper->getTimeInfo(flags));
#endif
        break;

    default:
	    TRACE("PLUG> HostCallback (opcode %d)\n index = %d, value = %p, ptr = %p, opt = %f\n", opcode, index, FromVstPtr<void> (value), ptr, opt);
        break;
	}

	return result;
}


NetWrapperEditor::NetWrapperEditor (AEffect* effect, AudioEffect* wrapper)
    : AEffEditor(wrapper)
    , m_pEffect(effect)
{
}

bool NetWrapperEditor::getRect (ERect** rect)
{
    return m_pEffect->dispatcher(m_pEffect, effEditGetRect, 0, 0, rect, 0.0f);
}

bool NetWrapperEditor::open (void* ptr)
{
    systemWindow = ptr;
    return m_pEffect->dispatcher(m_pEffect, effEditOpen, 0, 0, ptr, 0.0f);
}

void NetWrapperEditor::close ()
{
    m_pEffect->dispatcher(m_pEffect, effEditClose, 0, 0, 0, 0.0f);
    systemWindow = 0;
}

void NetWrapperEditor::idle ()
{
    m_pEffect->dispatcher(m_pEffect, effEditIdle, 0, 0, 0, 0.0f);
}


NetWrapper::NetWrapper (void* pModule, AEffect* effect, audioMasterCallback audioMaster, VstInt32 numPrograms, VstInt32 numParams)
    : AudioEffectX (audioMaster, numPrograms, numParams)
    , m_pModule(pModule)
    , m_pEffect(effect)
    , midiCount(0)
    , paramCount(0)
{
	if (audioMaster)
    {
	    setNumInputs(effect->numInputs);
	    setNumOutputs(effect->numOutputs);
	    setUniqueID (effect->uniqueID);

	    canProcessReplacing();	// supports replacing output
    }
    effect->dispatcher (effect, effMainsChanged, 0, 1, 0, 0);
    effect->user = this;

	if ((effect->flags & effFlagsHasEditor) == 0)
	{
		TRACE("This plug does not have an editor!\n");
	}
    else
    {
        m_pEditor = new NetWrapperEditor(effect, this);
	    setEditor(m_pEditor);
    }

#ifdef FAKE_TIMEINFO
    // initialize time info
    memset(&timeInfo, 0, sizeof(VstTimeInfo));
    timeInfo.sampleRate = 44100.0;
    timeInfo.tempo = 180.0;
    timeInfo.timeSigNumerator = 4;
    timeInfo.timeSigDenominator = 4;
    secondsPerSample = 1.0 / timeInfo.sampleRate;
    beatsPerSecond = timeInfo.tempo / 60.0;
    qtrPerBeat = 4.0 / timeInfo.timeSigDenominator;
    qtrPerBar = qtrPerBeat * timeInfo.timeSigNumerator;
#endif

#ifndef RUN_LOCALLY
    MySocketLib::Initialize();
    Connect(serverAddr, serverPort);
#endif
}

NetWrapper::~NetWrapper ()
{
#ifndef RUN_LOCALLY
    if (Connected()) Disconnect();
    MySocketLib::Cleanup();
#endif

    m_pEffect->dispatcher(m_pEffect, effClose, 0, 0, 0, 0.0f);
    if (m_pModule) FreeLibrary((HMODULE)m_pModule);
}

#if 0
VstIntPtr NetWrapper::dispatcher (VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
    if (opcode != effProcessEvents && opcode != effEditIdle)
        TRACE("dispatcher: opcode %d\n", opcode);
    return AudioEffectX::dispatcher(opcode, index, value, ptr, opt);
    //return m_pEffect->dispatcher(m_pEffect, opcode, index, value, ptr, opt);
}
#endif

void NetWrapper::Connect (const char* ipAddr, int portNum)
{
    if (!server.Connect(ipAddr, portNum))
        TRACE("Unable to connect to server at %s:%d\n", ipAddr, portNum);
    else prevBlockBytes = 0;
}

bool NetWrapper::getEffectName (char* name)
{
    m_pEffect->dispatcher(m_pEffect, effGetEffectName, 0, 0, name, 0.0f);
	return true;
}

bool NetWrapper::getProductString (char* text)
{
    m_pEffect->dispatcher(m_pEffect, effGetProductString, 0, 0, text, 0.0f);
	return true;
}

bool NetWrapper::getVendorString (char* text)
{
    m_pEffect->dispatcher(m_pEffect, effGetVendorString, 0, 0, text, 0.0f);
	return true;
}

VstInt32 NetWrapper::getVendorVersion ()
{ 
	return m_pEffect->version;
}

VstInt32 NetWrapper::canDo (char* text)
{
	if (!strcmp (text, "receiveVstEvents"))
		return 1;
	if (!strcmp (text, "receiveVstMidiEvent"))
		return 1;
	return -1;	// explicitly can't do; 0 => don't know
}

VstInt32 NetWrapper::getNumMidiInputChannels ()
{
	return 1; // we are monophonic
}

VstInt32 NetWrapper::getNumMidiOutputChannels ()
{
	return 0; // no MIDI output back to Host app
}

void NetWrapper::getParameterName (VstInt32 index, char* label)
{
    m_pEffect->dispatcher(m_pEffect, effGetParamName, index, 0, label, 0);
}

void NetWrapper::getParameterDisplay (VstInt32 index, char* text)
{
    m_pEffect->dispatcher(m_pEffect, effGetParamDisplay, index, 0, text, 0);
}

void NetWrapper::StuffParamMsg (VstInt32 index, float value)
{
    if (Connected() && paramCount < MAX_PARAM_EVENTS-1)
    {
        paramData[paramCount].effectIndex = 0;
        paramData[paramCount].paramIndex = (UINT16)index;
        paramData[paramCount].paramValue = value;
        paramCount++;
    }
}

void NetWrapper::setParameter (VstInt32 index, float value)
{
#ifndef RUN_LOCALLY
    StuffParamMsg(index, value);
#endif

    m_pEffect->setParameter(m_pEffect, index, value);
}

float NetWrapper::getParameter (VstInt32 index)
{
    return m_pEffect->getParameter(m_pEffect, index);
}

void NetWrapper::setProgram (VstInt32 program)
{
     curProgram = program;
     TRACE("NetWrapper::setProgram %d\n", program);
     m_pEffect->dispatcher(m_pEffect, effSetProgram, 0, program, 0, 0.0f);
     StuffParamMsg(0xFFFF, (float)program);
}

void NetWrapper::getProgramName (char* name)
{
    m_pEffect->dispatcher(m_pEffect, effGetProgramName, 0, 0, name, 0);
}

VstInt32 NetWrapper::processEvents (VstEvents* ev)
{
#ifndef RUN_LOCALLY
	if (Connected()) for (VstInt32 i = 0; i < ev->numEvents; i++)
	{
		if ((ev->events[i])->type != kVstMidiType)
			continue;

		VstMidiEvent* event = (VstMidiEvent*)ev->events[i];

        char* md = event->midiData;
        midiData[midiCount].channel = md[0] & 0x0F;
        midiData[midiCount].status = md[0] & 0xF0;
        midiData[midiCount].data1 = md[1];
        midiData[midiCount].data2 = md[2];

        if (midiCount < MAX_MIDI_EVENTS-1)
            midiData[midiCount++].startFrame = event->deltaFrames;
	}
#endif

    return m_pEffect->dispatcher(m_pEffect, effProcessEvents, 0, 0, ev, 0.0f);
}

void NetWrapper::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
#ifdef RUN_LOCALLY
    m_pEffect->processReplacing(m_pEffect, inputs, outputs, sampleFrames);
#else
    float* pOutLeft = outputs[0];
    float* pOutRight = outputs[1];
    int blockBytes = sampleFrames * DATASIZE;

    if (!Connected())
    {
        // if not connected, output silence
        memset(pOutLeft, 0, blockBytes);
        memset(pOutRight, 0, blockBytes);
        return;
    }

    // send header
    SendDataHeader hdr;
    hdr.frameCount = sampleFrames;
    hdr.midiCount = midiCount;
    hdr.paramCount = paramCount;
    hdr.timeStamp = 0;
    if (!Send((char*)&hdr, sizeof(hdr))) return;

    // send MIDI data
    if (midiCount > 0)
    {
        if (!Send((char*)midiData, midiCount * sizeof(MIDIMessageInfoStruct))) return;
        midiCount = 0;
    }

    // send Param updates
    if (paramCount > 0)
    {
        //TRACE("Sending %d param events: ", paramCount);
        bool sr = Send((char*)paramData, paramCount * sizeof(ParamMessageStruct));
        //TRACE(sr ? "OK\n" : "FAIL\n");
        paramCount = 0;
    }

    // Synth does not send sample data

    if (prevBlockBytes == 0)
    {
        // pretend we got a packet from remote synth, output silence
        memset(pOutLeft, 0, blockBytes);
        memset(pOutRight, 0, blockBytes);

        // only do this the very first time after connecting
        prevBlockBytes = blockBytes;
    }
    else
    {
        // get header for returned sample data
        SampleDataHeader shdr;
        if (Recv(&shdr, sizeof(shdr)) != sizeof(shdr)) return;
        //TRACE("SHDR %d %d\n", shdr.mainByteCount, shdr.corrByteCount);

        if (Recv(pOutLeft, prevBlockBytes) != prevBlockBytes) return;
        if (Recv(pOutRight, prevBlockBytes) != prevBlockBytes) return;
        prevBlockBytes = blockBytes;
    }
#endif

#ifdef FAKE_TIMEINFO
    // update time info
    timeInfo.samplePos += sampleFrames;
    double ppqPerBuffer = beatsPerSecond * qtrPerBeat * secondsPerSample * sampleFrames;
    timeInfo.ppqPos += ppqPerBuffer;
    int beatStartPpq = (int)(timeInfo.ppqPos);
    int barStartBar = (int)(beatStartPpq / qtrPerBar);
    timeInfo.barStartPos = barStartBar * qtrPerBar;
#endif
}
