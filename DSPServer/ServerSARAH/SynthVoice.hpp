//
//  SynthVoice.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#pragma once
#include <math.h>

#include "VoiceBase.hpp"
#include "SarahOscillator.hpp"
#include "ADSREnvelope.hpp"
#include "Envelope.hpp"

namespace AudioKitCore
{

    struct SynthOscParams
    {
        float pitchOffset;      // semitones, relative to MIDI note
        float mixLevel;         // fraction
        float cutoffMultiple;
        float cutoffEgStrength;
        float filterQ;
    };

    struct SynthVoiceParams
    {
        SynthOscParams osc1, osc2;
    };

    struct SynthModParams
    {
        float masterVol;
        float phaseDeltaMul;
    };

    struct SynthVoice : public VoiceBase
    {
        SarahOscillator osc1, osc2;
        ADSREnvelope ampEG, filterEG;

        SynthVoice() : VoiceBase() {}

        void init(double sampleRate, SynthVoiceParams* pTimbreParameters, SynthModParams* pModParameters);
        void updateOsc1();
        void updateOsc2();
        void updateFilter();

        virtual void start(unsigned evt, unsigned noteNum, unsigned velocity, float freqHz, float volume);
        virtual void restart(unsigned evt, unsigned velocity, float volume);
        virtual void restart(unsigned evt, unsigned noteNum, unsigned velocity, float freqHz, float volume);
        virtual void release(unsigned evt);
        virtual bool isReleasing(void) { return ampEG.isReleasing(); }
        virtual void stop(unsigned evt);
        
        // return true if amp envelope is finished
        virtual bool doModulation(void);
        virtual bool getSamples(int nSamples, float* pOutLeft, float* pOutRight);
    };

}
