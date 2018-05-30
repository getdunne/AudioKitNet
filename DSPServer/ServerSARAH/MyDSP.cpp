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
    unsigned noteNumber = event->data1;
    unsigned velocity = event->data2;

    switch (event->status & 0xF0)
    {
    case 0x90:  // note on
        if (velocity > 0) synth.playNote(noteNumber, velocity, NOTE_HZ(noteNumber));
        else synth.stopNote(noteNumber, false);
        break;

    case 0x80:  // note off
        synth.stopNote(noteNumber, false);
        break;
    }
}
