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
        int tableLength = 1 << sizePowerOfTwo;
        FunctionTableOscillator::init(sampleRate, tableLength);
        FourierFilter::init(sizePowerOfTwo);
        //filterCutoffMultiple = float(FunctionTable::nTableSize / 2);
        filterCutoffMultiple = float(FunctionTable::nTableSize / 4);
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
        BiquadFilterModel::setNormalizedCutoffAndQ(double(filterCutoffMultiple / FunctionTable::nTableSize), filterQ);
        FourierFilter::getFilteredWaveform(FunctionTable::pWaveTable);
    }

    void SarahOscillator::setCutoffMultiple(float cutoffMultiple)
    {
        filterCutoffMultiple = cutoffMultiple;
        recomputeFilteredWaveform();
    }

    void SarahOscillator::setQ(float newQ)
    {
        filterQ = newQ;
        recomputeFilteredWaveform();
    }

    void SarahOscillator::setCutoffMultipleAndQ(float cutoffMultiple, float newQ)
    {
        filterCutoffMultiple = cutoffMultiple;
        filterQ = newQ;
        recomputeFilteredWaveform();
    }

}
