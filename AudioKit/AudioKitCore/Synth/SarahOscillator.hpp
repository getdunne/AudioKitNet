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
        float filterCutoffHz;
        float filterQ;

        // helper
        void recomputeFilteredWaveform();

    public:
        // init() just handles allocation
        void init(int sizePowerOfTwo, double sampleRate);
        void deinit();

        // after setting up base waveform, call prepare() to compute initial spectrum
        void prepare();

        // update filter parameters: each of these triggers an inverse DFT computation
        void setCutoffHz(float cutoffHz);
        void setQ(float newQ);
        void setCutoffHzAndQ(float cutoffHz, float newQ);

        // call getSample(), getSamples() (inherited from FunctionTableOscillator) as needed
    };

}
