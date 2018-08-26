#include "MyDSP.h"
#include "TRACE.h"
#include <string.h>
#define HAS_PREFIX(string, prefix) (strncmp(string, prefix, strlen(prefix)) == 0)
#include <stdio.h>

MyDSP::MyDSP()
    : nMidiMsgs(0)
{
    synth.init(44100.0);
}

MyDSP::~MyDSP()
{
    synth.deinit();
}

void MyDSP::acceptMidi(MIDIMessageInfoStruct* pMidi, int nMessages)
{
    memcpy(midiData, pMidi, nMessages * sizeof(MIDIMessageInfoStruct));
    nMidiMsgs = nMessages;
    pMm = midiData;
}

void MyDSP::acceptParamChanges(ParamMessageStruct* pMsgs, int nMessages)
{
}

void MyDSP::render(float** buffers, int nFrames)
{
    float* outBuffers[] = { buffers[2], buffers[3] };

    // clear output buffers
    memset(outBuffers[0], 0, nFrames * sizeof(float));
    memset(outBuffers[1], 0, nFrames * sizeof(float));

    for (int frame = 0; frame < nFrames; frame += CHUNKSIZE)
    {
        // if buffer not a multiple of CHUNKSIZE, last chunk might be short
        int chunkSize = nFrames - frame;
        if (chunkSize > CHUNKSIZE) chunkSize = CHUNKSIZE;

        // process pending MIDI events
        while (nMidiMsgs > 0 && frame >= pMm->startFrame)
        {
            processMidiEvent(pMm);
            pMm++;
            nMidiMsgs--;
        }

        // render sound
        synth.render(2, chunkSize, outBuffers);

        // advance buffer pointers
        outBuffers[0] += chunkSize;
        outBuffers[1] += chunkSize;
    }

    // process any pending MIDI events left over from last chunk
    while (nMidiMsgs > 0)
    {
        processMidiEvent(pMm);
        pMm++;
        nMidiMsgs--;
    }
}

void MyDSP::processMidiEvent(MIDIMessageInfoStruct* event)
{
    unsigned data1 = event->data1;
    unsigned data2 = event->data2;
    float value;

    switch (event->status & 0xF0)
    {
    case 0x90:  // note on
        if (data2 > 0) synth.playNote(data1, data2, NOTE_HZ(data1));
        else synth.stopNote(data1, false);
        break;

    case 0x80:  // note off
        synth.stopNote(data1, false);
        break;

    case 0xB0:  // control change
        switch (data1)
        {
        case 1: // mod wheel
            value = data2 / 127.0;
            TRACE("vibrato depth %f\n", value);
            synth.vibratoDepth = 0.5f * value;
            break;

        default:
            break;
        }
        break;

    case 0xE0:  // pitch bend
        value = (int((data2 << 7) | (data1 & 0x7F)) - 8192) / 8192.0f;  // range -1 .. +1
        synth.pitchOffset = 2.0f * value;
        break;
    }
}
