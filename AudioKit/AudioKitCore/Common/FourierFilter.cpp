//
//  FourierFilter.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//
#include "FourierFilter.hpp"

namespace AudioKitCore
{
    FourierFilter::FourierFilter() : fftLength(0)
    {
    }

    void FourierFilter::init(int sizePowerOfTwo)
    {
        // allocate required memory
        fftLength = 1 << sizePowerOfTwo;
        kiss_fftr_cfg fwd = kiss_fftr_alloc(fftLength, 0, 0, 0);
        kiss_fftr_cfg inv = kiss_fftr_alloc(fftLength, 1, 0, 0);
        spectrum = new kiss_fft_cpx[fftLength / 2 + 1];
        scaledSpectrum = new kiss_fft_cpx[fftLength / 2 + 1];
    }

    void FourierFilter::deinit()
    {
        if (fftLength)
        {
            delete[] scaledSpectrum;
            delete[] spectrum;
            kiss_fftr_free(inv);
            kiss_fftr_free(fwd);
        }
    }

    void FourierFilter::doInitialForwardTransform(float* inputWaveform)
    {
        // perform initial forward FFT to get spectrum
        kiss_fftr(fwd, inputWaveform, spectrum);
    }

    void FourierFilter::getFilteredWaveform(float* outputWaveform)
    {
        // adjust filter gain curve by this much
        double scaleFactor = 1.0 / (fftLength / 2);

        // compute scaledSpectrum according to current simulated filter gain curve
        for (int h = 0; h <= fftLength / 2; h++)
        {
            float gain = float(scaleFactor * gainAtNormalizedFrequency((double)h / fftLength));
            scaledSpectrum[h].r = spectrum[h].r * gain;
            scaledSpectrum[h].i = spectrum[h].r * gain;
        }

        // perform inverse FFT to get filtered waveform
        kiss_fftri(inv, scaledSpectrum, outputWaveform);
    }
}
