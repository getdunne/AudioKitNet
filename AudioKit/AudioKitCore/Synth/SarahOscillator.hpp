//
//  SarahOscillator.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#pragma once
#include "FunctionTable.hpp"
#include "FourierFilter.hpp"

namespace AudioKitCore
{

    class SarahOscillator : public FunctionTableOscillator, protected FourierFilter
    {
    protected:
        // simulated filter settings
        float filterCutoffMultiple;
        float filterQ;

        // performance variables
        float phaseDeltaMul;            // phaseDelta multiplier for pitchbend, vibrato

        // helper
        void recomputeFilteredWaveform();

    public:
        // init() just handles allocation
        void init(int sizePowerOfTwo, double sampleRate);
        void deinit();

        // after setting up base waveform, call prepare() to compute initial spectrum
        void prepare();

        // setter for phaseDeltaMul
        void setPhaseDeltaMul(float pdm) { phaseDeltaMul = pdm; }

        // update filter parameters: each of these triggers an inverse DFT computation
        void setCutoffMultiple(float cutoffMultiple);
        void setQ(float newQ);
        void setCutoffMultipleAndQ(float cutoffMultiple, float newQ);

        // call getSample(), getSamples() (inherited from FunctionTableOscillator) as needed
        inline void getSamples(float* pLeft, float* pRight, float gain)
        {
            float sample = FunctionTableOscillator::getSample() * gain;
            *pLeft++ += sample;
            *pRight++ += sample;
        }
    };

}
