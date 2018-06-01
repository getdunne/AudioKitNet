//
//  FourierFilter.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#pragma once
#include "kiss_fftr.h"
#include "BiquadFilterModel.hpp"

namespace AudioKitCore
{

    // A FourierFilter operates on real (not complex) single-cycle sampled waveforms with power-of-two sizes.
    // It will compute and store the DFT (spectrum) of a supplied waveform, and later output filtered
    // versions of that waveform based on supplied "cutoff frequency" and "Q" parameters. This is done by
    // scaling the spectrum coefficients according to a Biquad filter model, and computing the inverse DFT.

    class FourierFilter : public BiquadFilterModel
    {
    protected:
        int fftLength;
        kiss_fftr_cfg fwd, inv;
        kiss_fft_cpx *spectrum, *scaledSpectrum;

    public:
        FourierFilter();
        ~FourierFilter() { deinit(); }

        // init() basically just handles allocation
        void init(int sizePowerOfTwo);
        void deinit();

        // call this to load a waveform to be filtered
        void doInitialForwardTransform(float* inputWaveform);

        // call setNormalizedCutoffAndQ() or setCutoffAndResonance(), then call this
        void getFilteredWaveform(float* outputWaveform);
    };

}
