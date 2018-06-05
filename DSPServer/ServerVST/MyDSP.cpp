#include "MyDSP.h"
#include "TRACE.h"
#include <string.h>
#include <windows.h>

static const char* pluginDllPath = "C:\\VST_OLD\\Phadiz(P)\\Phadiz(P).dll";
//static const char* pluginDllPath = "C:\\VstPlugins\\Synth1\\Synth1 VST.dll";
//static const char* pluginDllPath = "C:\\VST\\OBXd\\obxd14windows\\Obxd.dll";
//static const char* pluginDllPath = "C:\\Program Files (x86)\\Vstplugins\\Phasewave\\PHASEW-V1-22.dll";
//static const char* pluginDllPath = "C:\\VST\\Tranzistow\\Tranzistow32.dll";

MyDSP::MyDSP()
    : nMidiMsgs(0)
    , nParamMsgs(0)
{
    TRACE("Preparing to load plugin %s\n", pluginDllPath);

    if (!plugin.load(pluginDllPath))
    {
        TRACE("Error loading plugin %s\n", pluginDllPath);
        return;
    }

    if (!plugin.open())
    {
        TRACE("Unable to open plugin %s\n", pluginDllPath);
        return;
    }

    TRACE("Plugin loaded and opened\n");

    // testing only
    //plugin.checkProperties();
    //plugin.setupParamLookup(pmap);

    plugin.resume();
}

MyDSP::~MyDSP()
{
    plugin.suspend();
    plugin.close();
}

void MyDSP::acceptMidi(MIDIMessageInfoStruct* pMidi, int nMessages)
{
    memcpy(midiData, pMidi, nMessages * sizeof(MIDIMessageInfoStruct));
    nMidiMsgs = nMessages;
    pMm = midiData;
}

void MyDSP::appendMidiEvent(MIDIMessageInfoStruct* event)
{
    memcpy(midiData + nMidiMsgs, event, sizeof(MIDIMessageInfoStruct));
    nMidiMsgs++;
}

void MyDSP::acceptParamChanges(ParamMessageStruct* pMsgs, int nMessages)
{
    memcpy(paramData, pMsgs, nMessages * sizeof(ParamMessageStruct));
    nParamMsgs = nMessages;
    pPm = paramData;
}

void MyDSP::render(float** buffers, int nFrames)
{
    // clear output buffers
    memset(buffers[2], 0, nFrames * sizeof(float));
    memset(buffers[3], 0, nFrames * sizeof(float));

    // ensure plugin is set up for this block size
    plugin.setBlockSize(nFrames, buffers);

    // process parameter changes for this buffer period
    while (nParamMsgs > 0)
    {
        plugin.setParameter((int)pPm->paramIndex, pPm->paramValue);
        pPm++;
        nParamMsgs--;
    }
    // process MIDI events
    plugin.processMidi(midiData, nMidiMsgs);
    nMidiMsgs = 0;

    // render sound
    plugin.processBlock();
}

void MyDSP::appendMidiProgramChange(int progNumber)
{
    MIDIMessageInfoStruct midi;
    midi.status = 0xC0;                 // C0 = program change
    midi.channel = 0;
    midi.data1 = 0x7F & progNumber;     // max 7-bit number
    midi.data2 = 0;
    midi.startFrame = 511;
    appendMidiEvent(&midi);
}

void MyDSP::appendMidiBankSelect(int bankNumber)
{
    MIDIMessageInfoStruct midi;
    midi.status = 0xB0;                 // C0 = control change
    midi.channel = 0;
    midi.data1 = 32;                    // CC#32 = bank select LSB
    midi.data2 = 0x7F & bankNumber;     // lower 7 bits
    midi.startFrame = 511;              // last frame
    appendMidiEvent(&midi);

    midi.data1 = 0;                     // CC#0 = bank select MSB
    midi.data2 = 0x7F & (bankNumber >> 7); // upper 7 bits
    appendMidiEvent(&midi);
}

#define HAS_PREFIX(string, prefix) (strncmp(string, prefix, strlen(prefix)) == 0)
#define USE_MIDI_PROG_CHANGE

bool MyDSP::command(char* cmd)
{
    if (HAS_PREFIX(cmd, "edit"))
    {
        plugin.openCustomGui();
        return false;
    }
    if (HAS_PREFIX(cmd, "prog="))
    {
#ifdef USE_MIDI_PROG_CHANGE
        appendMidiProgramChange(atoi(cmd + 5));
#else
        int progIndex = atoi(cmd + 5);
        plugin.setProgram(progIndex);
#endif
        return false;
    }
    else if (HAS_PREFIX(cmd, "bank="))
    {
        int bank = atoi(cmd + 5);
        if (bank > 127) appendMidiBankSelect(bank);
        else appendMidiBankSelect(bank);
        return false;
    }
    else if (HAS_PREFIX(cmd, "tempo="))
    {
        plugin.setTempo(atof(cmd + 6));
        return false;
    }
    else if (HAS_PREFIX(cmd, "p"))
    {
        char *pEqual = strchr(cmd, '=');
        char* arg = pEqual + 1;
        *pEqual = 0;
        int paramIndex = atoi(cmd + 1);
        *pEqual = '=';

        if (*arg == '?') sprintf(arg, "%g", plugin.getParameter(paramIndex));
        else plugin.setParameter(paramIndex, atof(arg));
        return true;
    }
    else
    {
        char *pEqual = strchr(cmd, '=');
        if (!pEqual) return false;
        char* arg = pEqual + 1;
        *pEqual = 0;
        std::string paramName(cmd);
        *pEqual = '=';

        ParameterMap::iterator it = pmap.find(paramName);
        if (it != pmap.end())
        {
            int paramIndex = it->second;
            if (*arg == '?') sprintf(arg, "%g", plugin.getParameter(paramIndex));
            else plugin.setParameter(paramIndex, atof(arg));
            return true;
        }
    }
    return false;
}
