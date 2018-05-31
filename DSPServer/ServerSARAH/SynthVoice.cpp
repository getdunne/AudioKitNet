//
//  SynthVoice.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "SynthVoice.hpp"
#include <stdio.h>

namespace AudioKitCore
{
    void SynthVoice::init(double sampleRate, MultiSample* pMultiSample1, MultiSample* pMultiSample2,
                            WaveStack *pOsc1Stack, WaveStack* pOsc2Stack,
                            SynthVoiceParams *pTimbreParameters, SynthModParams* pModParameters)
    {
        VoiceBase::init(sampleRate, pTimbreParameters, pModParameters);

        smp1.init(pMultiSample1, sampleRate);
        smp2.init(pMultiSample2, sampleRate);

        osc1.init(sampleRate, pOsc1Stack);
        osc1.setPhases(pTimbreParameters->osc1.phases);
        osc1.setFreqSpread(pTimbreParameters->osc1.freqSpread);
        osc1.setPanSpread(pTimbreParameters->osc1.panSpread);

        osc2.init(sampleRate, pOsc2Stack);
        osc2.setPhases(pTimbreParameters->osc2.phases);
        osc2.setFreqSpread(pTimbreParameters->osc2.freqSpread);
        osc2.setPanSpread(pTimbreParameters->osc2.panSpread);

        fltL.init(sampleRate);
        fltR.init(sampleRate);
        fltL.setStages(pTimbreParameters->filterStages);
        fltR.setStages(pTimbreParameters->filterStages);

        ampEG.init();
        filterEG.init();
    }

    void SynthVoice::updateOsc1()
    {
        if (pTimbreParams == 0) return;
        SynthVoiceParams *pParams = (SynthVoiceParams*)pTimbreParams;
        osc1.setPhases(pParams->osc1.phases);
        osc1.setFreqSpread(pParams->osc1.freqSpread);
        osc1.setPanSpread(pParams->osc1.panSpread);
    }

    void SynthVoice::updateOsc2()
    {
        if (pTimbreParams == 0) return;
        SynthVoiceParams *pParams = (SynthVoiceParams*)pTimbreParams;
        osc2.setPhases(pParams->osc2.phases);
        osc2.setFreqSpread(pParams->osc2.freqSpread);
        osc2.setPanSpread(pParams->osc2.panSpread);
    }

    void SynthVoice::updateFilter(int stages)
    {
        fltL.setStages(stages);
        fltR.setStages(stages);
    }

    void SynthVoice::start(unsigned evt, unsigned noteNum, unsigned velocity, float freqHz, float volume)
    {
        smp1.start(noteNum, velocity, freqHz);
        smp2.start(noteNum, velocity, freqHz);

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
        smp1.restart(velocity);
        smp2.restart(velocity);

        ampEG.restart();
        VoiceBase::restart(evt, this->noteNumber, velocity, this->noteHz, volume);
    }
    
    void SynthVoice::restart(unsigned evt, unsigned noteNum, unsigned velocity, float freqHz, float volume)
    {
        smp1.start(noteNum, velocity, freqHz);
        smp2.start(noteNum, velocity, freqHz);

        ampEG.restart();
        VoiceBase::restart(evt, noteNum, velocity, freqHz, volume);
    }

    void SynthVoice::release(unsigned evt)
    {
        smp1.release();
        smp2.release();
        ampEG.release();
        filterEG.release();
        VoiceBase::release(evt);
    }
    
    void SynthVoice::stop(unsigned evt)
    {
        // For stop, call base-class version first, so note stops right away
        VoiceBase::stop(evt);

        smp1.stop();
        smp2.stop();

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
                    smp1.start(newNoteNumber, newVelocity, noteHz);
                    smp2.start(newNoteNumber, newVelocity, noteHz);
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

#if 0
        // pumping effect using multi-segment EG
        float pump = pumpEG.getSample();
        double cutoffHz = noteHz * (1.0f + cutoffMultiple + cutoffEgStrength * pump);
#else
        // standard ADSR EG
        double cutoffHz = noteHz * (1.0f + pMod->cutoffMultiple + pMod->cutoffEgStrength * filterEG.getSample());
#endif
        fltL.setParams(cutoffHz, pMod->resLinear);
        fltR.setParams(cutoffHz, pMod->resLinear);

        osc1.phaseDeltaMul = pMod->phaseDeltaMul;
        osc2.phaseDeltaMul = pMod->phaseDeltaMul;
        smp1.multiplier = pMod->phaseDeltaMul;
        smp2.multiplier = pMod->phaseDeltaMul;

        return false;
    }
    
    bool SynthVoice::getSamples(int nSamples, float* pOutLeft, float* pOutRight)
    {
        SynthVoiceParams* pParams = (SynthVoiceParams*)pTimbreParams;

        for (int i=0; i < nSamples; i++)
        {
            float leftSample = 0.0f;
            float rightSample = 0.0f;
            osc1.getSamples(&leftSample, &rightSample, pParams->osc1.mixLevel);
            osc2.getSamples(&leftSample, &rightSample, pParams->osc2.mixLevel);
            smp1.getSamplePair(&leftSample, &rightSample, pParams->smp1.mixLevel);
            smp2.getSamplePair(&leftSample, &rightSample, pParams->smp2.mixLevel);

            if (pParams->filterStages == 0)
            {
                *pOutLeft++ += tempGain * leftSample;
                *pOutRight++ += tempGain * rightSample;
            }
            else
            {
                *pOutLeft++ += fltL.process(tempGain * leftSample);
                *pOutRight++ += fltR.process(tempGain * rightSample);
            }
        }
        return false;
    }

}
