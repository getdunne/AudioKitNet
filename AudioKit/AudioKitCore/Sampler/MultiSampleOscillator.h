//
//  MultiSampleOscillator.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#pragma once
#include "SampleOscillator.hpp"
#include "MultiSample.hpp"

namespace AudioKitCore
{

    class MultiSampleOscillator : public SampleOscillator
    {
    public:
        MultiSampleOscillator();
        ~MultiSampleOscillator();

        void init(MultiSample* multiSample, double sampleRate);
        void setLoopThruRelease(bool value) { loopThruRelease = value; }

        void start(unsigned noteNumber, unsigned velocity, float noteFrequency);
        void restart(unsigned velocity);
        void restart();
        void release();
        void stop() { sampleBuffer = 0; }

        // return true if we run out of samples
        inline bool getSample(float* output, float gain)
        {
            return SampleOscillator::getSample(sampleBuffer, output, gain);
        }

        // return true if we run out of samples
        inline bool getSamplePair(float* leftOutput, float* rightOutput, float gain)
        {
            return SampleOscillator::getSamplePair(sampleBuffer, leftOutput, rightOutput, gain);
        }

    protected:
        MultiSample* multiSample;
        SampleBuffer* sampleBuffer;
        double sampleRate;
        bool loopThruRelease;   // if true, sample continue looping thru note release phase
        unsigned noteNum;
        float noteHz;
    };

}
