#pragma once
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Synth.hpp"

typedef struct
{
    uint8_t     status;
    uint8_t     channel;
    uint8_t     data1;
    uint8_t     data2;
    uint32_t    startFrame;
} MIDIMessageInfoStruct;

typedef struct
{
    uint16_t    effectIndex;
    uint16_t    paramIndex;
    float       paramValue;
} ParamMessageStruct;

#define NOTE_HZ(midiNoteNumber) ( 440.0f * pow(2.0f, ((midiNoteNumber) - 69.0f)/12.0f) )


class MyDSP
{
public:
    MyDSP();
    ~MyDSP();

    void acceptMidi(MIDIMessageInfoStruct* pMidi, int nMessages);
    void acceptParamChanges(ParamMessageStruct* pMsgs, int nMessages);

    bool command(char* cmd) { return synth.command(cmd); }

    void render(float** buffers, int nFrames);

protected:
    AudioKitCore::Synth synth;

    MIDIMessageInfoStruct midiData[1000], *pMm;
    int nMidiMsgs;

    void processMidiEvent(MIDIMessageInfoStruct* event);
};
