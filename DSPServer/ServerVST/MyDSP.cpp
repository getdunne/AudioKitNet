#include "MyDSP.h"
#include "TRACE.h"
#include <string.h>
#include <windows.h>

static const char* pluginDllPath = "C:\\VST_OLD\\Phadiz(P)\\Phadiz(P).dll";
//static const char* pluginDllPath = "C:\\VstPlugins\\Synth1\\Synth1 VST.dll";
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
        processParamEvent(pPm);
        pPm++;
        nParamMsgs--;
    }
    // process MIDI
    plugin.processMidi(midiData, nMidiMsgs);
    nMidiMsgs = 0;

    // render sound
    plugin.processBlock();
}

void MyDSP::processParamEvent(ParamMessageStruct* event)
{
    plugin.setParameter((int)event->paramIndex, event->paramValue);
}

#define HAS_PREFIX(string, prefix) (strncmp(string, prefix, strlen(prefix)) == 0)

bool MyDSP::command(char* cmd)
{
    if (HAS_PREFIX(cmd, "prog="))
    {
        int progIndex = atoi(cmd + 5);
        plugin.setProgram(progIndex);
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
