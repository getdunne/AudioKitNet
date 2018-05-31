//
//  VoiceBase.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#pragma once
#include "VoiceBase.hpp"

namespace AudioKitCore
{

    struct VoiceBase
    {
        void* pTimbreParams;    // pointer to a block of shared parameters
        void* pModParams;       // pointer to a block of modulation values
        unsigned event;         // last "event number" associated with this voice
        int noteNumber;         // MIDI note number, or -1 if not playing any note
        float noteHz;           // note frequency in Hz
        float noteVol;          // fraction 0.0 - 1.0, based on MIDI velocity
        
        // temporary holding variables
        int newNoteNumber;  // holds new note number while damping note before restarting
        int newVelocity;    // same for new velocity
        float newNoteVol;   // same for new note volume
        float tempGain;     // product of global volume, note volume, and amp EG

        VoiceBase() : pTimbreParams(0), pModParams(0), event(0), noteNumber(-1) {}

        void init(double sampleRate, void* pTimbreParameters, void* pModParameters);

        virtual void start(unsigned evt, unsigned noteNum, unsigned velocity, float freqHz, float volume);
        virtual void restart(unsigned evt, unsigned noteNum, unsigned noteVel, float freqHz, float volume);
        virtual void release(unsigned evt);
        virtual bool isReleasing(void) = 0;
        virtual void stop(unsigned evt);
        
        // both of these return true if amp envelope is finished
        virtual bool doModulation(void) = 0;
        virtual bool getSamples(int nSamples, float* pOutLeft, float* pOutRight) = 0;
    };

}
