//
//  VoiceManager.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "VoiceManager.hpp"

namespace AudioKitCore {
    
    VoiceManager::VoiceManager()
    : disabled(false)
    , nCurrentPolyphony(0)
    , eventCounter(0)
    , doRenderPrep(0)
    {
    }
    
    VoiceManager::~VoiceManager()
    {
        voice.clear();
    }
    
    int VoiceManager::init(VoicePointerArray voiceArray, int polyphony,
                           RenderPrepCallback renderPrepCallback, void* callbackPtr)
    {
        voice = voiceArray;
        setPolyphony(polyphony);
        for (int i = 0; i < nCurrentPolyphony; i++)
        {
            voice[i]->event = 0;
            voice[i]->noteNumber = -1;
        }

        doRenderPrep = renderPrepCallback;
        cbPtr = callbackPtr;

        eventCounter = 0;
        return 0;   // no error
    }
    
    void VoiceManager::deinit()
    {
    }

    bool VoiceManager::setPolyphony(int polyphony)
    {
        int nVoices = polyphony;
        if (nVoices < 1) nVoices = 1;
        if (nVoices > (int)voice.size()) nVoices = (int)voice.size();
        nCurrentPolyphony = nVoices;
        return nVoices == polyphony;    // return true only if we got exactly what was requested
    }

    void VoiceManager::playNote(unsigned noteNumber, unsigned velocity, float noteHz)
    {
        eventCounter++;
        pedalLogic.keyDownAction(noteNumber);
        play(noteNumber, velocity, noteHz);
    }
    
    void VoiceManager::stopNote(unsigned noteNumber, bool immediate)
    {
        eventCounter++;
        if (immediate || pedalLogic.keyUpAction(noteNumber))
            stop(noteNumber, immediate);
    }
    
    void VoiceManager::sustainPedal(bool down)
    {
        eventCounter++;
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

    VoiceBase* VoiceManager::voicePlayingNote(unsigned noteNumber)
    {
        for (int i=0; i < nCurrentPolyphony; i++)
        {
            VoiceBase* pVoice = voice[i];
            if (pVoice->noteNumber == (int)noteNumber) return pVoice;
        }
        return 0;
    }
    
    void VoiceManager::play(unsigned noteNumber, unsigned velocity, float noteHz)
    {
        //printf("playNote nn=%d vel=%d %.2f Hz\n", noteNumber, velocity, noteHz);

        // is any voice already playing this note?
        VoiceBase* pVoice = voicePlayingNote(noteNumber);
        if (pVoice)
        {
            // re-start the note
            //pVoice->restart(eventCounter, velocity / 127.0f);
            pVoice->start(eventCounter, noteNumber, velocity, noteHz, velocity / 127.0f);
            //printf("Restart note %d as %d\n", noteNumber, pVoice->noteNumber);
            return;
        }
        
        // find a free voice (with noteNumber < 0) to play the note
        for (int i=0; i < nCurrentPolyphony; i++)
        {
            pVoice = voice[i];
            if (pVoice->noteNumber < 0)
            {
                // found a free voice: assign it to play this note
                pVoice->start(eventCounter, noteNumber, velocity, noteHz, velocity / 127.0f);
                //printf("Play note %d (%.2f Hz) vel %d\n", noteNumber, noteHz, velocity);
                return;
            }
        }
        
        // all oscillators in use: find "stalest" voice to steal
        unsigned greatestDiffOfAll = 0;
        VoiceBase* pStalestVoiceOfAll = 0;
        unsigned greatestDiffInRelease = 0;
        VoiceBase* pStalestVoiceInRelease = 0;
        for (int i=0; i < nCurrentPolyphony; i++)
        {
            pVoice = voice[i];
            unsigned diff = eventCounter - pVoice->event;
            if (pVoice->isReleasing())
            {
                if (diff > greatestDiffInRelease)
                {
                    greatestDiffInRelease = diff;
                    pStalestVoiceInRelease = pVoice;
                }
            }
            if (diff > greatestDiffOfAll)
            {
                greatestDiffOfAll = diff;
                pStalestVoiceOfAll = pVoice;
            }
        }

        if (pStalestVoiceInRelease != 0)
        {
            // We have a stalest note in its release phase: restart that one
            pStalestVoiceInRelease->restart(eventCounter, noteNumber, velocity, noteHz, velocity / 127.0f);
        }
        else
        {
            // No notes in release phase: restart the "stalest" one we could find
            pStalestVoiceOfAll->restart(eventCounter, noteNumber, velocity, noteHz, velocity / 127.0f);
        }
    }
    
    void VoiceManager::stop(unsigned noteNumber, bool immediate)
    {
        //printf("stopNote nn=%d %s\n", noteNumber, immediate ? "immediate" : "release");
        VoiceBase* pVoice = voicePlayingNote(noteNumber);
        if (pVoice == 0) return;
        //printf("stopNote pVoice is %p\n", pVoice);
        
        if (immediate)
        {
            pVoice->stop(eventCounter);
            //printf("Stop note %d immediate\n", noteNumber);
        }
        else
        {
            pVoice->release(eventCounter);
            //printf("Stop note %d release\n", noteNumber);
        }
    }

    void VoiceManager::stopAll(void)
    {
        for (int i = 0; i < nCurrentPolyphony; i++)
            voice[i]->stop(eventCounter);
    }
    
    void VoiceManager::render(unsigned sampleCount, float *outBuffers[])
    {
        if (disabled || !doRenderPrep) return;

        float* pOutLeft = outBuffers[0];
        float* pOutRight = outBuffers[1];
        
        if (doRenderPrep) doRenderPrep(cbPtr);

        for (int i=0; i < nCurrentPolyphony; i++)
        {
            VoiceBase* pVoice = voice[i];
            int nn = pVoice->noteNumber;
            if (nn >= 0)
            {
                if (pVoice->doModulation() || pVoice->getSamples(sampleCount, pOutLeft, pOutRight))
                {
                    pVoice->stop(eventCounter);
                }
            }
        }
    }
}
