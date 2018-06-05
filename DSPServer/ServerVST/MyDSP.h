#pragma once
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "PluginManager.h"

class MyDSP
{
public:
    MyDSP();
    ~MyDSP();

    void acceptMidi(MIDIMessageInfoStruct* pMidi, int nMessages);
    void acceptParamChanges(ParamMessageStruct* pMsgs, int nMessages);

    bool command(char* cmd);

    void render(float** buffers, int nFrames);

protected:
    PluginManager plugin;

    MIDIMessageInfoStruct midiData[1000], *pMm;
    int nMidiMsgs;
    ParamMessageStruct paramData[1000], *pPm;
    int nParamMsgs;

    ParameterMap pmap;

    void processParamEvent(ParamMessageStruct* event);
};
