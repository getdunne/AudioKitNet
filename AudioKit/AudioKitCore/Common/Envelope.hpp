//
//  Envelope.hpp
//  ExtendingAudioKit
//
//  Created by Shane Dunne on 2018-04-06.
//  Copyright © 2018 Shane Dunne & Associates. All rights reserved.
//

#pragma once
#include "LinearRamper.hpp"
#include "FunctionTable.hpp"

namespace AudioKitCore
{

    struct EnvelopeSegmentParams
    {
        float initialLevel;             // where this segment starts
        float finalLevel;               // where it ends
        float seconds;                  // how long it takes to get there
    };

    struct EnvelopeParams
    {
        float sampleRateHz;

        int nSegments;                  // number of segments
        EnvelopeSegmentParams* pSeg;    // points to an array of nSegments elements

        int attackSegmentIndex;         // start() begins at this segment
        int sustainSegmentIndex;        // index of first sustain segment (-1 if none)
        int releaseSegmentIndex;        // release() jumps to this segment

        EnvelopeParams();
        void init(float newSampleRateHz, int nSegs, EnvelopeSegmentParams* pSegParams,
                  int susSegIndex=-1, int attackSegIndex=0, int releaseSegIndex=-1);
        void updateSampleRate(float newSampleRateHz);
    };

    struct Envelope
    {
        EnvelopeParams *pParams;
        LinearRamper ramper;
        int currentSegmentIndex;

        void init(EnvelopeParams *pParameters);

        void start();       // begin attack segment
        void restart();     // go to segment 0
        void release();     // go to release segment
        void reset();       // reset to idle state
        bool isIdle() { return currentSegmentIndex < 0; }
        bool isReleasing() { return currentSegmentIndex >= pParams->releaseSegmentIndex; }

        float getSample();
    };
}
