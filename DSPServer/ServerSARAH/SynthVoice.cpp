//
//  SynthVoice.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "SynthVoice.hpp"
#include <stdio.h>
#include "TRACE.h"

namespace AudioKitCore
{
    void SynthVoice::init(double sampleRate, SynthVoiceParams *pTimbreParameters, SynthModParams* pModParameters)
    {
        VoiceBase::init(sampleRate, pTimbreParameters, pModParameters);

        osc1.init(11, sampleRate);
        osc1.sawtooth();
        osc1.prepare();
        osc1.setQ(pTimbreParameters->osc1.filterQ);

        osc2.init(11, sampleRate);
        osc2.sawtooth();
        osc2.prepare();
        osc2.setQ(pTimbreParameters->osc2.filterQ);

        ampEG.init();
        filterEG.init();
    }

    void SynthVoice::updateOsc1()
    {
    }

    void SynthVoice::updateOsc2()
    {
    }

    void SynthVoice::updateFilter()
    {
        if (pTimbreParams == 0) return;
        SynthVoiceParams *pParams = (SynthVoiceParams*)pTimbreParams;
        osc1.setCutoffMultipleAndQ(pParams->osc1.cutoffMultiple, pParams->osc1.filterQ);
        osc2.setCutoffMultipleAndQ(pParams->osc2.cutoffMultiple, pParams->osc2.filterQ);
    }

    void SynthVoice::start(unsigned evt, unsigned noteNum, unsigned velocity, float freqHz, float volume)
    {
        SynthVoiceParams *pParams = (SynthVoiceParams*)pTimbreParams;
        osc1.setFrequency(float(freqHz * pow(2.0f, pParams->osc1.pitchOffset / 12.0f)));
        osc2.setFrequency(float(freqHz * pow(2.0f, pParams->osc2.pitchOffset / 12.0f)));
        ampEG.start();
        filterEG.start();

        // Call base-class version last, after rest of setup
        VoiceBase::start(evt, noteNum, velocity, freqHz, volume);
    }
    
    void SynthVoice::restart(unsigned evt, unsigned velocity, float volume)
    {
        ampEG.restart();
        VoiceBase::restart(evt, this->noteNumber, velocity, this->noteHz, volume);
    }
    
    void SynthVoice::restart(unsigned evt, unsigned noteNum, unsigned velocity, float freqHz, float volume)
    {
        ampEG.restart();
        VoiceBase::restart(evt, noteNum, velocity, freqHz, volume);
    }

    void SynthVoice::release(unsigned evt)
    {
        ampEG.release();
        filterEG.release();
        VoiceBase::release(evt);
    }
    
    void SynthVoice::stop(unsigned evt)
    {
        // For stop, call base-class version first, so note stops right away
        VoiceBase::stop(evt);

        ampEG.reset();
        filterEG.reset();
    }
    
    bool SynthVoice::doModulation(void)
    {
        if (ampEG.isIdle()) return true;

        SynthModParams* pMod = (SynthModParams*)pModParams;
        SynthVoiceParams* pParams = (SynthVoiceParams*)pTimbreParams;

        if (ampEG.isPreStarting())
        {
            float ampeg = ampEG.getSample();
            tempGain = pMod->masterVol * noteVol * ampeg;
            if (!ampEG.isPreStarting())
            {
                noteVol = newNoteVol;
                tempGain = pMod->masterVol * noteVol * ampeg;

                if (newNoteNumber >= 0)
                {
                    // restarting a "stolen" voice with a new note number
                    osc1.setFrequency(float(noteHz * pow(2.0f, pParams->osc1.pitchOffset / 12.0f)));
                    osc2.setFrequency(float(noteHz * pow(2.0f, pParams->osc2.pitchOffset / 12.0f)));
                    noteNumber = newNoteNumber;
                }
                ampEG.start();
                filterEG.start();
            }
        }
        else
            tempGain = pMod->masterVol * noteVol * ampEG.getSample();

        float feg = filterEG.getSample();
        osc1.setCutoffMultipleAndQ(pParams->osc1.cutoffMultiple + pParams->osc1.cutoffEgStrength * feg,
                                   pParams->osc1.filterQ);
        osc2.setCutoffMultipleAndQ(pParams->osc2.cutoffMultiple + pParams->osc2.cutoffEgStrength * feg,
                                   pParams->osc2.filterQ);

        return false;
    }
    
    bool SynthVoice::getSamples(int nSamples, float* pOutLeft, float* pOutRight)
    {
        SynthVoiceParams* pParams = (SynthVoiceParams*)pTimbreParams;

        for (int i=0; i < nSamples; i++)
        {
            float leftSample = 0.0f;
            float rightSample = 0.0f;
            osc1.getSamples(&leftSample, &rightSample, tempGain * pParams->osc1.mixLevel);
            osc2.getSamples(&leftSample, &rightSample, tempGain * pParams->osc2.mixLevel);
            *pOutLeft++ += leftSample;
            *pOutRight++ += rightSample;
        }
        return false;
    }

}
