//
//  Envelope.cpp
//  ExtendingAudioKit
//
//  Created by Shane Dunne on 2018-04-06.
//  Copyright © 2018 Shane Dunne & Associates. All rights reserved.
//

#include "Envelope.hpp"

namespace AudioKitCore
{

    EnvelopeParams::EnvelopeParams()
    : sampleRateHz(44100.0f) // a guess, will be overridden later by a call to init(,,,,)
    , nSegments(0)
    , pSeg(0)
    {
    }

    void EnvelopeParams::init(float newSampleRateHz, int nSegs, EnvelopeSegmentParams* pSegParams,
                              int susSegIndex, int attackSegIndex, int releaseSegIndex)
    {
        sampleRateHz = newSampleRateHz;
        nSegments = nSegs;
        pSeg = pSegParams;
        sustainSegmentIndex = susSegIndex;
        attackSegmentIndex = attackSegIndex;
        releaseSegmentIndex = (releaseSegIndex < 0) ? nSegs - 1 : releaseSegIndex;
    }

    void EnvelopeParams::updateSampleRate(float newSampleRateHz)
    {
        sampleRateHz = newSampleRateHz;
    }

    void Envelope::init(EnvelopeParams *pParameters)
    {
        pParams = pParameters;
        reset();
    }

    void Envelope::reset()
    {
        currentSegmentIndex = -1;   // no segment active; we're idle
        ramper.init(pParams->pSeg[pParams->attackSegmentIndex].initialLevel);
    }

    void Envelope::start()
    {
        int asi = pParams->attackSegmentIndex;
        currentSegmentIndex = asi;
        float initialLevel = pParams->pSeg[asi].initialLevel;
        float finalLevel = pParams->pSeg[asi].finalLevel;
        float normalizedInterval = pParams->pSeg[asi].seconds * pParams->sampleRateHz;
        ramper.init(initialLevel, finalLevel, normalizedInterval);
    }

    void Envelope::release()
    {
        int rsi = pParams->releaseSegmentIndex;
        currentSegmentIndex = rsi;
        float finalLevel = pParams->pSeg[rsi].finalLevel;
        float normalizedInterval = pParams->pSeg[rsi].seconds * pParams->sampleRateHz;
        ramper.reinit(finalLevel, normalizedInterval);
    }

    void Envelope::restart()
    {
        // segment 0 may be defined as a quick note-dampening segment before attack segment
        currentSegmentIndex = 0;
        float finalLevel = pParams->pSeg[0].finalLevel;
        float normalizedInterval = pParams->pSeg[0].seconds * pParams->sampleRateHz;
        ramper.reinit(finalLevel, normalizedInterval);
    }

    float Envelope::getSample()
    {
        // idle state?
        if (currentSegmentIndex < 0) return pParams->pSeg[pParams->attackSegmentIndex].initialLevel;

        // within a segment?
        if (ramper.isRamping()) return float(ramper.getNextValue());

        // end of last segment?
        if (currentSegmentIndex == (pParams->nSegments - 1))
        {
            float finalLevel = pParams->pSeg[currentSegmentIndex].finalLevel;
            reset();
            return finalLevel;
        }

        // end of last segment of looped sustain region?
        if (pParams->sustainSegmentIndex >= 0 && currentSegmentIndex == (pParams->releaseSegmentIndex - 1))
        {
            int ssi = pParams->sustainSegmentIndex;
            currentSegmentIndex = ssi;
            float initialLevel = pParams->pSeg[ssi].initialLevel;
            float finalLevel = pParams->pSeg[ssi].finalLevel;
            float normalizedInterval = pParams->pSeg[ssi].seconds * pParams->sampleRateHz;
            ramper.init(initialLevel, finalLevel, normalizedInterval);
            return initialLevel;
        }

        // advance to next segment
        currentSegmentIndex++;
        float initialLevel = pParams->pSeg[currentSegmentIndex].initialLevel;
        float finalLevel = pParams->pSeg[currentSegmentIndex].finalLevel;
        float normalizedInterval = pParams->pSeg[currentSegmentIndex].seconds * pParams->sampleRateHz;
        ramper.init(initialLevel, finalLevel, normalizedInterval);
        return initialLevel;
    }
}
