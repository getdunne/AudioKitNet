//
//  BiquadFilterModel.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#pragma once
#include "kiss_fftr.h"
#include <math.h>
#include <complex>

namespace AudioKitCore
{

    // This "model" of a traditional "RBJ Biquad" filter is adapted from Vinnie Falco's
    // "Collection of Useful C++ Classes for Digital Signal Processing"
    // See https://github.com/vinniefalco/DSPFilters

    class BiquadFilterModel
    {
    protected:
        static constexpr double doublePi = 3.1415926535897932384626433832795028841971;

        template <typename Ty, typename To>
        inline std::complex<Ty> addmul(const std::complex<Ty>& c, Ty v, const std::complex<To>& c1)
        {
            return std::complex <Ty>(c.real() + v * c1.real(), c.imag() + v * c1.imag());
        }

        typedef std::complex<double> complex_t;

        double a0, a1, a2, b0, b1, b2;

    public:
        // normalizedCutoffFrequency varies from 0 to 0.5 (Nyquist)
        // q varies from about 0.1 to maybe 10.0 or a bit more
        void setNormalizedCutoffAndQ(double normalizedCutoffFrequency, double q);

        // alternate "synth-friendly" setup: typically, resdB will be in range -20 to +20 dB
        void setCutoffAndResonance(double samplingRateHz, double cutoffHz, double resdB)
        {
            setNormalizedCutoffAndQ(cutoffHz / samplingRateHz, pow(10.0, -0.5 * resdB));
        }

        // normalizedFrequency varies from 0 to 0.5 (Nyquist)
        double gainAtNormalizedFrequency(double normalizedFrequency);
    };

}
