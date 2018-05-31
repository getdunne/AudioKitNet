//
//  Sampler.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "Sampler.hpp"
#include <math.h>

namespace AudioKitCore {
    
    Sampler::Sampler()
    : sampleRate(44100.0f)    // sensible guess
    , isFilterEnabled(false)
    , masterVolume(1.0f)
    , pitchOffset(0.0f)
    , vibratoDepth(0.0f)
    , cutoffMultiple(4.0f)
    , cutoffEnvelopeStrength(20.0f)
    , linearResonance(1.0f)
    , loopThruRelease(false)
    , stoppingAllVoices(false)
    {
        for (int i=0; i < MAX_POLYPHONY; i++)
        {
            voice[i].adsrEnvelope.pParams = &adsrEnvelopeParameters;
            voice[i].filterEnvelope.pParams = &filterEnvelopeParameters;
        }
    }
    
    Sampler::~Sampler()
    {
    }
    
    int Sampler::init(double sampleRate)
    {
        MultiSample::init();

        sampleRate = (float)sampleRate;
        adsrEnvelopeParameters.updateSampleRate((float)(sampleRate/CHUNKSIZE));
        filterEnvelopeParameters.updateSampleRate((float)(sampleRate/CHUNKSIZE));
        vibratoLFO.sinusoid();
        vibratoLFO.init(sampleRate/CHUNKSIZE, 5.0f);

        for (int i=0; i<MAX_POLYPHONY; i++) voice[i].init(sampleRate);

        return 0;   // no error
    }
    
    void Sampler::deinit()
    {
        MultiSample::deinit();
    }
    
    SamplerVoice* Sampler::voicePlayingNote(unsigned int noteNumber)
    {
        for (int i=0; i < MAX_POLYPHONY; i++)
        {
            SamplerVoice* pVoice = &voice[i];
            if (pVoice->noteNumber == noteNumber) return pVoice;
        }
        return 0;
    }

    void Sampler::playNote(unsigned noteNumber, unsigned velocity, float noteFrequency)
    {
        pedalLogic.keyDownAction(noteNumber);
        //if (pedalLogic.keyDownAction(noteNumber))
        //    stop(noteNumber, false);
        play(noteNumber, velocity, noteFrequency);
    }
    
    void Sampler::stopNote(unsigned noteNumber, bool immediate)
    {
        if (immediate || pedalLogic.keyUpAction(noteNumber))
            stop(noteNumber, immediate);
    }
    
    void Sampler::sustainPedal(bool down)
    {
        if (down) pedalLogic.pedalDown();
        else {
            for (int nn=0; nn < MIDI_NOTENUMBERS; nn++)
            {
                if (pedalLogic.isNoteSustaining(nn))
                    stop(nn, false);
            }
            pedalLogic.pedalUp();
        }
    }
    
    void Sampler::play(unsigned noteNumber, unsigned velocity, float noteFrequency)
    {
        if (stoppingAllVoices) return;

        //printf("playNote nn=%d vel=%d %.2f Hz\n", noteNumber, velocity, noteFrequency);
        // sanity check: ensure we are initialized with at least one buffer
        if (!isKeyMapValid || sampleBufferList.size() == 0) return;
        
        // is any voice already playing this note?
        SamplerVoice* pVoice = voicePlayingNote(noteNumber);
        if (pVoice)
        {
            // re-start the note
            pVoice->restart(velocity / 127.0f, lookupSample(noteNumber, velocity));
            //printf("Restart note %d as %d\n", noteNumber, pVoice->noteNumber);
            return;
        }
        
        // find a free voice (with noteNumber < 0) to play the note
        for (int i=0; i < MAX_POLYPHONY; i++)
        {
            SamplerVoice* pVoice = &voice[i];
            if (pVoice->noteNumber < 0)
            {
                // found a free voice: assign it to play this note
                KeyMappedSampleBuffer* pBuf = lookupSample(noteNumber, velocity);
                if (pBuf == 0) return;  // don't crash if someone forgets to build map
                pVoice->start(noteNumber, sampleRate, noteFrequency, velocity / 127.0f, pBuf);
                //printf("Play note %d (%.2f Hz) vel %d as %d (%.2f Hz, voice %d pBuf %p)\n",
                //       noteNumber, noteFrequency, velocity, pBuf->noteNumber, pBuf->noteFrequency, i, pBuf);
                return;
            }
        }
        
        // all oscillators in use; do nothing
        //printf("All oscillators in use!\n");
    }
    
    void Sampler::stop(unsigned noteNumber, bool immediate)
    {
        //printf("stopNote nn=%d %s\n", noteNumber, immediate ? "immediate" : "release");
        SamplerVoice* pVoice = voicePlayingNote(noteNumber);
        if (pVoice == 0) return;
        //printf("stopNote pVoice is %p\n", pVoice);
        
        if (immediate)
        {
            pVoice->stop();
            //printf("Stop note %d immediate\n", noteNumber);
        }
        else
        {
            pVoice->release(loopThruRelease);
            //printf("Stop note %d release\n", noteNumber);
        }
    }

    void Sampler::stopAllVoices()
    {
        // Lock out starting any new notes, and tell Render() to stop all active notes
        stoppingAllVoices = true;

        // Wait until Render() has killed all active notes
        bool noteStillSounding = true;
        while (noteStillSounding)
        {
            noteStillSounding = false;
            for (int i=0; i < MAX_POLYPHONY; i++)
                if (voice[i].noteNumber >= 0) noteStillSounding = true;
        }
    }

    void Sampler::restartVoices()
    {
        // Allow starting new notes again
        stoppingAllVoices = false;
    }
    
    void Sampler::render(unsigned channelCount, unsigned sampleCount, float *outBuffers[])
    {
        float* pOutLeft = outBuffers[0];
        float* pOutRight = outBuffers[1];
        
        float pitchDev = this->pitchOffset + vibratoDepth * vibratoLFO.getSample();
        float cutoffMul = isFilterEnabled ? cutoffMultiple : -1.0f;
        
        SamplerVoice* pVoice = &voice[0];
        for (int i=0; i < MAX_POLYPHONY; i++, pVoice++)
        {
            int nn = pVoice->noteNumber;
            if (nn >= 0)
            {
                if (stoppingAllVoices ||
                    pVoice->prepToGetSamples(masterVolume, pitchDev, cutoffMul, cutoffEnvelopeStrength, linearResonance) ||
                    pVoice->getSamples(sampleCount, pOutLeft, pOutRight))
                {
                    stopNote(nn, true);
                }
            }
        }
    }
}
