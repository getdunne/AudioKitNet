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
#include "EnsembleOscillator.hpp"
#include "ADSREnvelope.hpp"
#include "Envelope.hpp"
#include "MultiStageFilter.hpp"
#include "MultiSampleOscillator.h"

namespace AudioKitCore
{

    struct SynthOscParams
    {
        int phases;             // 1 to 10, or 0 to disable oscillator
        float freqSpread;       // cents
        float panSpread;        // fraction 0 = no spread, 1 = max spread
        float pitchOffset;      // semitones, relative to MIDI note
        float mixLevel;         // fraction
    };

    struct SynthSampleParams
    {
        float mixLevel;
    };

    struct SynthVoiceParams
    {
        SynthOscParams osc1, osc2;
        SynthSampleParams smp1, smp2;
        int filterStages;       // 1 to 4, or 0 to disable filter
    };

    struct SynthModParams
    {
        float masterVol;
        float phaseDeltaMul;
        float cutoffMultiple;
        float cutoffEgStrength;
        float resLinear;
    };

    struct SynthVoice : public VoiceBase
    {
        MultiSampleOscillator smp1, smp2;
        EnsembleOscillator osc1, osc2;
        MultiStageFilter fltL, fltR;            // two filters (left/right)
        ADSREnvelope ampEG, filterEG;

        SynthVoice() : VoiceBase() {}

        void init(double sampleRate, MultiSample* pMultiSample1, MultiSample* pMultiSample2,
                  WaveStack* pOsc1Stack, WaveStack* pOsc2Stack,
                  SynthVoiceParams* pTimbreParameters, SynthModParams* pModParameters);
        void updateOsc1();
        void updateOsc2();
        void updateFilter(int stages);

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
