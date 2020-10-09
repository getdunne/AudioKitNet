#pragma once
#include <stdint.h>
#include "PluginManager.h"
#include "Protocol.h"

class MyDSP
{
public:
    MyDSP();
    ~MyDSP();

    void acceptMidi(MIDIMessageInfoStruct* pMidi, int nMessages);
    void acceptParamChanges(ParamMessageStruct* pMsgs, int nMessages);

    bool command(char* cmd);

    void setSampleRate(float sampleRateHz);
    void render(float** buffers, int nFrames);
    void idle();

protected:
    PluginManager plugin;

    MIDIMessageInfoStruct midiData[1000], *pMm;
    int nMidiMsgs;
    ParamMessageStruct paramData[1000], *pPm;
    int nParamMsgs;

    ParameterMap pmap;

    void appendMidiEvent(MIDIMessageInfoStruct* event);
    void appendMidiProgramChange(int progNumber);
    void appendMidiBankSelect(int bankNumber);
};
