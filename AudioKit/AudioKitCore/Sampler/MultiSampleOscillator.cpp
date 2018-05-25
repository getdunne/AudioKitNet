//
//  MultiSampleOscillator.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "MultiSampleOscillator.h"

namespace AudioKitCore
{

    MultiSampleOscillator::MultiSampleOscillator()
        : multiSample(0)
        , sampleBuffer(0)
        , loopThruRelease(false)
    {
    }

    MultiSampleOscillator::~MultiSampleOscillator()
    {
    }

    void MultiSampleOscillator::init(MultiSample* multiSample, double sampleRate)
    {
        this->multiSample = multiSample;
        this->sampleRate = sampleRate;
    }

    void MultiSampleOscillator::start(unsigned noteNumber, unsigned velocity, float noteFrequency)
    {
        if (multiSample == 0) return;
        noteNum = noteNumber;
        noteHz = noteFrequency;
        sampleBuffer = multiSample->lookupSample(noteNumber, velocity);
        if (sampleBuffer)
        {
            indexPoint = sampleBuffer->startPoint;
            increment = (sampleBuffer->sampleRate / sampleRate) * (noteFrequency / sampleBuffer->noteFrequency);
            multiplier = 1.0;
            isLooping = sampleBuffer->isLooping;
        }
    }

    void MultiSampleOscillator::restart(unsigned velocity)
    {
        start(noteNum, velocity, noteHz);
    }

    void MultiSampleOscillator::restart()
    {
        if (sampleBuffer)
        {
            indexPoint = sampleBuffer->startPoint;
            isLooping = sampleBuffer->isLooping;
        }
    }

    void MultiSampleOscillator::release()
    {
        if (!loopThruRelease) isLooping = false;
    }
}
