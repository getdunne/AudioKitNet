//
//  MultiSample.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//
#pragma once
#include "AKSampler_Typedefs.h"
#include "SampleBuffer.hpp"
#include <list>

#define MIDI_NOTENUMBERS 128    // MIDI offers 128 distinct note numbers

namespace AudioKitCore
{
    
    class MultiSample
    {
    public:
        MultiSample();
        ~MultiSample();
        
        int init();             // returns system error code, nonzero only if a problem occurs
        void deinit();          // call this to un-load all samples and clear the keymap

        // call to load samples
        void loadSampleData(AKSampleDataDescriptor& sdd);
        
        // after loading samples, call one of these to build the key map
        void buildKeyMap(void);         // use this when you have full key mapping data (min/max note, vel)
        void buildSimpleKeyMap(void);   // or this when you don't
        
        // call this to look up the correct sample for given (note, velocity) pair
        KeyMappedSampleBuffer* lookupSample(unsigned noteNumber, unsigned velocity);

    protected:
        // list of (pointers to) all loaded samples
        std::list<KeyMappedSampleBuffer*> sampleBufferList;
        
        // maps MIDI note numbers to "closest" samples (all velocity layers)
        std::list<KeyMappedSampleBuffer*> keyMap[MIDI_NOTENUMBERS];
        bool isKeyMapValid;
    };

}
