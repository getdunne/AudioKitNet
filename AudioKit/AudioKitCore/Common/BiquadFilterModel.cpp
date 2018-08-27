//
//  BiquadFilterModel.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "BiquadFilterModel.hpp"

namespace AudioKitCore
{

    void BiquadFilterModel::setNormalizedCutoffAndQ(double normalizedCutoffFrequency, double q)
    {
        double w0 = 2 * doublePi * normalizedCutoffFrequency;
        double cs = cos(w0);
        double sn = sin(w0);
        double AL = sn / (2 * q);

        a0 = 1 + AL;
        a1 = -2 * cs;
        a2 = 1 - AL;

        b0 = (1 - cs) / 2;
        b1 = 1 - cs;
        b2 = (1 - cs) / 2;
    }

    double BiquadFilterModel::gainAtNormalizedFrequency(double normalizedFrequency)
    {
        if (normalizedFrequency == 0.0) return 0.0;

        double w = 2 * doublePi * normalizedFrequency;
        complex_t czn1 = std::polar(1., -w);
        complex_t czn2 = std::polar(1., -2 * w);
        complex_t ch(1);
        complex_t cbot(1);

        complex_t ct(b0 / a0);
        complex_t cb(1);
        ct = addmul(ct, b1 / a0, czn1);
        ct = addmul(ct, b2 / a0, czn2);
        cb = addmul(cb, a1 / a0, czn1);
        cb = addmul(cb, a2 / a0, czn2);
        ch *= ct;
        cbot *= cb;

        return sqrt(std::norm(ch / cbot));
    }
}
