//
//  VoiceBase.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "VoiceBase.hpp"
#include <stdio.h>

namespace AudioKitCore
{
    void VoiceBase::init(double sampleRate, void* pTimbreParameters, void* pModParameters)
    {
        pTimbreParams = pTimbreParameters;
        pModParams = pModParameters;
        event = 0;
        noteNumber = -1;
    }

    void VoiceBase::start(unsigned evt, unsigned noteNum, unsigned velocity, float freqHz, float volume)
    {
        event = evt;
        noteVol = volume;
        noteHz = freqHz;
        noteNumber = noteNum;
    }
    
    void VoiceBase::restart(unsigned evt, unsigned noteNum, unsigned noteVel, float freqHz, float volume)
    {
        event = evt;
        newNoteNumber = noteNum;
        newVelocity = noteVel;
        newNoteVol = volume;
        noteHz = freqHz;
    }

    void VoiceBase::release(unsigned evt)
    {
        event = evt;
    }
    
    void VoiceBase::stop(unsigned evt)
    {
        event = evt;
        noteNumber = -1;
    }

}
