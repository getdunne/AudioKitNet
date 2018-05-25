//
//  MultiSample.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "MultiSample.hpp"
#include <math.h>

namespace AudioKitCore {
    
    MultiSample::MultiSample()
    : isKeyMapValid(false)
    {
    }
    
    MultiSample::~MultiSample()
    {
    }
    
    int MultiSample::init()
    {
        return 0;   // no error
    }
    
    void MultiSample::deinit()
    {
        isKeyMapValid = false;
        for (KeyMappedSampleBuffer* pBuf : sampleBufferList) delete pBuf;
        sampleBufferList.clear();
        for (int i=0; i < MIDI_NOTENUMBERS; i++) keyMap[i].clear();
    }
    
    void MultiSample::loadSampleData(AKSampleDataDescriptor& sdd)
    {
        KeyMappedSampleBuffer* pBuf = new KeyMappedSampleBuffer();
        pBuf->minimumNoteNumber = sdd.sampleDescriptor.minimumNoteNumber;
        pBuf->maximumNoteNumber = sdd.sampleDescriptor.maximumNoteNumber;
        pBuf->minimumVelocity = sdd.sampleDescriptor.minimumVelocity;
        pBuf->maximumVelocity = sdd.sampleDescriptor.maximumVelocity;
        sampleBufferList.push_back(pBuf);
        
        pBuf->init(sdd.sampleRate, sdd.channelCount, sdd.sampleCount);
        float* pData = sdd.data;
        if (sdd.isInterleaved) for (int i=0; i < sdd.sampleCount; i++)
        {
            pBuf->setData(i, *pData++);
            if (sdd.channelCount > 1) pBuf->setData(sdd.sampleCount + i, *pData++);
        }
        else for (int i=0; i < sdd.channelCount * sdd.sampleCount; i++)
        {
            pBuf->setData(i, *pData++);
        }
        pBuf->noteNumber = sdd.sampleDescriptor.noteNumber;
        pBuf->noteFrequency = sdd.sampleDescriptor.noteFrequency;
        
        if (sdd.sampleDescriptor.startPoint > 0.0f) pBuf->startPoint = sdd.sampleDescriptor.startPoint;
        if (sdd.sampleDescriptor.endPoint > 0.0f)   pBuf->endPoint = sdd.sampleDescriptor.endPoint;
        
        pBuf->isLooping = sdd.sampleDescriptor.isLooping;
        if (pBuf->isLooping)
        {
            // loopStartPoint, loopEndPoint are usually sample indices, but values 0.0-1.0
            // are interpreted as fractions of the total sample length.
            if (sdd.sampleDescriptor.loopStartPoint > 1.0f) pBuf->loopStartPoint = sdd.sampleDescriptor.loopStartPoint;
            else pBuf->loopStartPoint = pBuf->endPoint * sdd.sampleDescriptor.loopStartPoint;
            if (sdd.sampleDescriptor.loopEndPoint > 1.0f) pBuf->loopEndPoint = sdd.sampleDescriptor.loopEndPoint;
            else pBuf->loopEndPoint = pBuf->endPoint * sdd.sampleDescriptor.loopEndPoint;
        }
    }
    
    // re-compute keyMap[] so every MIDI note number is automatically mapped to the sample buffer
    // closest in pitch
    void MultiSample::buildSimpleKeyMap()
    {
        // clear out the old mapping entirely
        isKeyMapValid = false;
        for (int i=0; i < MIDI_NOTENUMBERS; i++) keyMap[i].clear();
        
        for (int nn=0; nn < MIDI_NOTENUMBERS; nn++)
        {
            // scan loaded samples to find the minimum distance to note nn
            int minDistance = MIDI_NOTENUMBERS;
            for (KeyMappedSampleBuffer* pBuf : sampleBufferList)
            {
                int distance = abs(pBuf->noteNumber - nn);
                if (distance < minDistance)
                {
                    minDistance = distance;
                }
            }
            
            // scan again to add only samples at this distance to the list for note nn
            for (KeyMappedSampleBuffer* pBuf : sampleBufferList)
            {
                int distance = abs(pBuf->noteNumber - nn);
                if (distance == minDistance)
                {
                    keyMap[nn].push_back(pBuf);
                }
            }
        }
        isKeyMapValid = true;
    }
    
    // rebuild keyMap based on explicit mapping data in samples
    void MultiSample::buildKeyMap(void)
    {
        // clear out the old mapping entirely
        isKeyMapValid = false;
        for (int i=0; i < MIDI_NOTENUMBERS; i++) keyMap[i].clear();

        for (int nn=0; nn < MIDI_NOTENUMBERS; nn++)
        {
            for (KeyMappedSampleBuffer* pBuf : sampleBufferList)
            {
                if (nn >= pBuf->minimumNoteNumber && nn <= pBuf->maximumNoteNumber)
                    keyMap[nn].push_back(pBuf);
            }
        }
        isKeyMapValid = true;
    }

    KeyMappedSampleBuffer* MultiSample::lookupSample(unsigned noteNumber, unsigned velocity)
    {
        // common case: only one sample mapped to this note - return it immediately
        if (keyMap[noteNumber].size() == 1) return keyMap[noteNumber].front();

        // search samples mapped to this note for best choice based on velocity
        for (KeyMappedSampleBuffer* pBuf : keyMap[noteNumber])
        {
            // if sample does not have velocity range, accept it trivially
            if (pBuf->minimumVelocity < 0 || pBuf->maximumVelocity < 0) return pBuf;

            // otherwise (common case), accept based on velocity
            if ((int)velocity >= pBuf->minimumVelocity && (int)velocity <= pBuf->maximumVelocity) return pBuf;
        }

        // return nil if no samples mapped to note (or sample velocities are invalid)
        return 0;
    }

}
