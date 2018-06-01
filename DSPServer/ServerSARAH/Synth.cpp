//
//  Synth.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "Synth.hpp"
#include "FunctionTable.hpp"
#include <math.h>
#include <vector>
#include "wavpack.h"
#include <iostream>
#include <stdio.h>

namespace AudioKitCore {
    
    Synth::Synth()
    : pitchOffset(0.0f)
    , vibratoDepth(0.0f)
    {
        buildSetGetMaps();

        modParams.masterVol = 1.0f;
        modParams.phaseDeltaMul = 1.0f;

        for (int i=0; i < MAX_POLYPHONY; i++)
        {
            voice[i].ampEG.pParams = &ampEGParams;
            voice[i].filterEG.pParams = &filterEGParams;
        }
    }
    
    Synth::~Synth()
    {
    }
    
    int Synth::init(double sampleRate)
    {
        ampEGParams.updateSampleRate((float)(sampleRate/CHUNKSIZE));
        filterEGParams.updateSampleRate((float)(sampleRate/CHUNKSIZE));

        vibratoLFO.init(sampleRate/CHUNKSIZE);
        vibratoLFO.sinusoid();
        vibratoLFO.setFrequency(5.0f);

        voiceParams.osc1.pitchOffset = 0.0f;
        voiceParams.osc1.mixLevel = 0.7f;

        voiceParams.osc2.pitchOffset = -12.0f;
        voiceParams.osc2.mixLevel = 0.6f;

        ampEGParams.setAttackTimeSeconds(0.1f);
        ampEGParams.setDecayTimeSeconds(0.1f);
        ampEGParams.sustainFraction = 0.8f;
        ampEGParams.setReleaseTimeSeconds(0.5f);

        filterEGParams.setAttackTimeSeconds(2.0f);
        filterEGParams.setDecayTimeSeconds(2.0f);
        filterEGParams.sustainFraction = 0.1f;
        filterEGParams.setReleaseTimeSeconds(2.0f);

        VoicePointerArray vpa;
        for (int i = 0; i < MAX_POLYPHONY; i++)
        {
            voice[i].init(sampleRate, &voiceParams, &modParams);
            vpa.push_back(&voice[i]);
        }
        voiceManager.init(vpa, MAX_POLYPHONY, &renderPrepCallback, this);

        return 0;   // no error
    }

    void Synth::updateOsc1()
    {
        for (int i = 0; i < MAX_POLYPHONY; i++)
        {
            voice[i].updateOsc1();
        }
    }
    
    void Synth::updateOsc2()
    {
        for (int i = 0; i < MAX_POLYPHONY; i++)
        {
            voice[i].updateOsc2();
        }
    }

    void Synth::updateFilters()
    {
        for (int i = 0; i < MAX_POLYPHONY; i++)
        {
            voice[i].updateFilter();
        }
    }

    void Synth::deinit()
    {
    }

    void Synth::playNote(unsigned noteNumber, unsigned velocity, float noteHz)
    {
        voiceManager.playNote(noteNumber, velocity, noteHz);
    }
    
    void Synth::stopNote(unsigned noteNumber, bool immediate)
    {
        voiceManager.stopNote(noteNumber, immediate);
    }
    
    void Synth::sustainPedal(bool down)
    {
        voiceManager.sustainPedal(down);
    }

    void Synth::renderPrepCallback(void* thisPtr)
    {
        Synth& self = *((Synth*)thisPtr);
        float pitchDev = self.pitchOffset + self.vibratoDepth * self.vibratoLFO.getSample();
        self.modParams.phaseDeltaMul = powf(2.0f, pitchDev / 12.0f);
    }
    
    void Synth::render(unsigned channelCount, unsigned sampleCount, float *outBuffers[])
    {
        voiceManager.render(sampleCount, outBuffers);
    }

    void Synth::buildSetGetMaps()
    {
        get["osc1PitchOffset"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc1.pitchOffset); };
        get["osc1MixLevel"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc1.mixLevel); };
        set["osc1PitchOffset"] = [this](char* in) { voiceParams.osc1.pitchOffset = atof(in); updateOsc1(); };
        set["osc1MixLevel"] = [this](char* in) { voiceParams.osc1.mixLevel = atof(in); };

        get["osc2PitchOffset"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc2.pitchOffset); };
        get["osc2MixLevel"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc2.mixLevel); };
        set["osc2PitchOffset"] = [this](char* in) { voiceParams.osc2.pitchOffset = atof(in); updateOsc2(); };
        set["osc2MixLevel"] = [this](char* in) { voiceParams.osc2.mixLevel = atof(in); };

        get["masterVol"] = [this](char* out) { sprintf(out, "%g", modParams.masterVol); };
        set["masterVol"] = [this](char* in) { modParams.masterVol = atof(in); };

        get["ampAttack"] = [this](char* out) { sprintf(out, "%f", ampEGParams.getAttackTimeSeconds()); };
        get["ampDecay"] = [this](char* out) { sprintf(out, "%f", ampEGParams.getDecayTimeSeconds()); };
        get["ampSustain"] = [this](char* out) { sprintf(out, "%f", ampEGParams.sustainFraction); };
        get["ampRelease"] = [this](char* out) { sprintf(out, "%f", ampEGParams.getReleaseTimeSeconds()); };
        set["ampAttack"] = [this](char* in) { ampEGParams.setAttackTimeSeconds(atof(in)); };
        set["ampDecay"] = [this](char* in) { ampEGParams.setDecayTimeSeconds(atof(in)); };
        set["ampSustain"] = [this](char* in) { ampEGParams.sustainFraction = atof(in); };
        set["ampRelease"] = [this](char* in) { ampEGParams.setReleaseTimeSeconds(atof(in)); };

        get["fltAttack"] = [this](char* out) { sprintf(out, "%f", filterEGParams.getAttackTimeSeconds()); };
        get["fltDecay"] = [this](char* out) { sprintf(out, "%f", filterEGParams.getDecayTimeSeconds()); };
        get["fltSustain"] = [this](char* out) { sprintf(out, "%f", filterEGParams.sustainFraction); };
        get["fltRelease"] = [this](char* out) { sprintf(out, "%f", filterEGParams.getReleaseTimeSeconds()); };
        set["fltAttack"] = [this](char* in) { filterEGParams.setAttackTimeSeconds(atof(in)); };
        set["fltDecay"] = [this](char* in) { filterEGParams.setDecayTimeSeconds(atof(in)); };
        set["fltSustain"] = [this](char* in) { filterEGParams.sustainFraction = atof(in); };
        set["fltRelease"] = [this](char* in) { filterEGParams.setReleaseTimeSeconds(atof(in)); };

        get["fltCutoff"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc1.cutoffMultiple); };
        set["fltCutoff"] = [this](char* in)
        { voiceParams.osc1.cutoffMultiple = voiceParams.osc2.cutoffMultiple = atof(in); updateFilters(); };
        get["fltEgStrength"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc1.cutoffEgStrength); };
        set["fltEgStrength"] = [this](char* in)
        { voiceParams.osc1.cutoffEgStrength = voiceParams.osc2.cutoffEgStrength = atof(in); };
        get["fltResonance"] = [this](char* out) { sprintf(out, "%g", -20.0f * log10(voiceParams.osc1.filterQ)); };
        set["fltResonance"] = [this](char* in)
        { voiceParams.osc1.filterQ = voiceParams.osc2.filterQ = pow(10.0, -0.05 * atof(in)); };
    }

    bool Synth::command(char* cmd)
    {
        char *pEqual = strchr(cmd, '=');
        char* arg = pEqual + 1;
        *pEqual = 0;
        std::string name(cmd);
        *pEqual = '=';

        if (*arg == '?') get[name](arg);
        else set[name](arg);

        return true;
    }

}
