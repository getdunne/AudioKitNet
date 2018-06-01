//
//  SarahOscillator.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "SarahOscillator.hpp"

namespace AudioKitCore
{

    void SarahOscillator::init(int sizePowerOfTwo, double sampleRate)
    {
        FunctionTableOscillator::init(sampleRate);
        FourierFilter::init(sizePowerOfTwo);
        filterCutoffHz = float(0.49 * sampleRate);
        filterQ = 1.0f;
    }

    void SarahOscillator::deinit()
    {
        FourierFilter::deinit();
        FunctionTable::deinit();
    }

    void SarahOscillator::prepare()
    {
        FourierFilter::doInitialForwardTransform(FunctionTable::pWaveTable);
    }

    void SarahOscillator::recomputeFilteredWaveform()
    {
        // TODO: consider keeping track of previous cutoff/Q values and doing nothing if differences are below some threshold
        BiquadFilterModel::setNormalizedCutoffAndQ(double(filterCutoffHz) / FunctionTableOscillator::sampleRateHz, filterQ);
        FourierFilter::getFilteredWaveform(FunctionTable::pWaveTable);
    }

    void SarahOscillator::setCutoffHz(float cutoffHz)
    {
        filterCutoffHz = cutoffHz;
        recomputeFilteredWaveform();
    }

    void SarahOscillator::setQ(float newQ)
    {
        filterQ = newQ;
        recomputeFilteredWaveform();
    }

    void SarahOscillator::setCutoffHzAndQ(float cutoffHz, float newQ)
    {
        filterCutoffHz = cutoffHz;
        filterQ = newQ;
        recomputeFilteredWaveform();
    }

}
