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
        modParams.cutoffMultiple = 4.0f;
        modParams.cutoffEgStrength = 20.0f;
        modParams.resLinear = 1.0f;
        modParams.phaseDeltaMul = 1.0f;

        for (int i=0; i < MAX_POLYPHONY; i++)
        {
            voice[i].ampEG.pParams = &ampEGParams;
            voice[i].filterEG.pParams = &filterEGParams;
        }
    }
    
    Synth::~Synth()
    {
        multiSample1.deinit();
        multiSample2.deinit();
    }
    
    int Synth::init(double sampleRate)
    {
        FunctionTable waveForm;
        int length = 1 << WaveStack::maxBits;
        waveForm.init(length);
        waveForm.sawtooth(0.5f);
        waveStack1.initStack(waveForm.pWaveTable);
        waveStack2.initStack(waveForm.pWaveTable);

        ampEGParams.updateSampleRate((float)(sampleRate/CHUNKSIZE));
        filterEGParams.updateSampleRate((float)(sampleRate/CHUNKSIZE));

        vibratoLFO.waveTable.sinusoid();
        vibratoLFO.init(sampleRate/CHUNKSIZE, 5.0f);

        voiceParams.osc1.phases = 5;
        voiceParams.osc1.freqSpread = 25.0f;
        voiceParams.osc1.panSpread = 0.95f;
        voiceParams.osc1.pitchOffset = 0.0f;
        voiceParams.osc1.mixLevel = 0.7f;

        voiceParams.osc2.phases = 1;
        voiceParams.osc2.freqSpread = 15.0f;
        voiceParams.osc2.panSpread = 1.0f;
        voiceParams.osc2.pitchOffset = -12.0f;
        voiceParams.osc2.mixLevel = 0.6f;

        voiceParams.smp1.mixLevel = 1.0f;
        voiceParams.smp2.mixLevel = 0.0f;

        ampEGParams.setAttackTimeSeconds(0.1f);
        ampEGParams.setDecayTimeSeconds(0.1f);
        ampEGParams.sustainFraction = 0.8f;
        ampEGParams.setReleaseTimeSeconds(0.5f);

        filterEGParams.setAttackTimeSeconds(2.0f);
        filterEGParams.setDecayTimeSeconds(2.0f);
        filterEGParams.sustainFraction = 0.1f;
        filterEGParams.setReleaseTimeSeconds(2.0f);

        voiceParams.filterStages = 2;

        VoicePointerArray vpa;
        for (int i = 0; i < MAX_POLYPHONY; i++)
        {
            voice[i].init(sampleRate, &multiSample1, &multiSample2,
                          &waveStack1, &waveStack2, &voiceParams, &modParams);
            vpa.push_back(&voice[i]);
        }
        voiceManager.init(vpa, MAX_POLYPHONY, &renderPrepCallback, this);

        loadSfz("D:\\Desktop\\Sounds\\MM Dreams SFZ", "BASS Ambi Acoustik.sfz", 0);
        loadSfz("D:\\Desktop\\Sounds\\MM Pads SFZ", "PAD-Pastoral Warmth for Sad Days.sfz", 1);

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

    void Synth::updateFilters(int stages)
    {
        for (int i = 0; i < MAX_POLYPHONY; i++)
        {
            voice[i].updateFilter(stages);
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

    bool Synth::loadCompressedSampleFile(AKSampleFileDescriptor& sfd, int multiSampleIndex)
    {
        char errMsg[100];
        WavpackContext* wpc = WavpackOpenFileInput(sfd.path, errMsg, OPEN_2CH_MAX, 0);
        if (wpc == 0)
        {
            std::cout << sfd.path << ": " << errMsg;
            return false;
        }

        AKSampleDataDescriptor sdd;
        sdd.sampleDescriptor = sfd.sampleDescriptor;
        sdd.sampleRate = (float)WavpackGetSampleRate(wpc);
        sdd.channelCount = WavpackGetReducedChannels(wpc);
        sdd.sampleCount = WavpackGetNumSamples(wpc);
        sdd.isInterleaved = sdd.channelCount > 1;
        sdd.data = new float[sdd.channelCount * sdd.sampleCount];

        // There are cases where loop end may be off by one
        if (sdd.sampleDescriptor.loopEndPoint > (float)(sdd.sampleCount - 1))
            sdd.sampleDescriptor.loopEndPoint = (float)(sdd.sampleCount - 1);

        int mode = WavpackGetMode(wpc);
        WavpackUnpackSamples(wpc, (int32_t*)sdd.data, sdd.sampleCount);
        if ((mode & MODE_FLOAT) == 0)
        {
            // convert samples to floating-point
            int bps = WavpackGetBitsPerSample(wpc);
            float scale = 1.0f / (1 << (bps - 1));
            float* pf = sdd.data;
            int32_t* pi = (int32_t*)pf;
            for (int i = 0; i < (sdd.sampleCount * sdd.channelCount); i++)
                *pf++ = scale * *pi++;
        }
        WavpackCloseFile(wpc);

        switch (multiSampleIndex)
        {
        case 0:
            multiSample1.loadSampleData(sdd);
            break;
        case 1:
            multiSample2.loadSampleData(sdd);
            break;
        }

        delete[] sdd.data;
        return true;
    }

    #define HAS_PREFIX(string, prefix) (strncmp(string, prefix, strlen(prefix)) == 0)
    #define NOTE_HZ(midiNoteNumber) ( 440.0f * pow(2.0f, ((midiNoteNumber) - 69.0f)/12.0f) )

    bool Synth::loadSfz(const char* folderPath, const char* sfzFileName, int multiSampleIndex)
    {
        stopAllVoices();
        switch (multiSampleIndex)
        {
        case 0:
            multiSample1.deinit();
            break;
        case 1:
            multiSample2.deinit();
            break;
        }

        char buf[1000];
        sprintf(buf, "%s/%s", folderPath, sfzFileName);

        FILE* pfile = fopen(buf, "r");
        if (!pfile) return false;

        int lokey, hikey, pitch, lovel, hivel;
        bool bLoop;
        float fLoopStart, fLoopEnd;
        char sampleFileName[100];
        char *p, *pp;

        while (fgets(buf, sizeof(buf), pfile))
        {
            p = buf;
            while (*p != 0 && isspace(*p)) p++;

            pp = strrchr(p, '\n');
            if (pp) *pp = 0;

            if (HAS_PREFIX(p, "<group>"))
            {
                p += 7;
                lokey = 0;
                hikey = 127;
                pitch = 60;

                pp = strstr(p, "lokey");
                if (pp)
                {
                    pp = strchr(pp, '=');
                    if (pp) pp++;
                    if (pp) lokey = atoi(pp);
                }

                pp = strstr(p, "hikey");
                if (pp)
                {
                    pp = strchr(pp, '=');
                    if (pp) pp++;
                    if (pp) hikey = atoi(pp);
                }

                pp = strstr(p, "pitch_keycenter");
                if (pp)
                {
                    pp = strchr(pp, '=');
                    if (pp) pp++;
                    if (pp) pitch = atoi(pp);
                }
            }
            else if (HAS_PREFIX(p, "<region>"))
            {
                p += 8;
                lovel = 0;
                hivel = 127;
                sampleFileName[0] = 0;
                bLoop = false;
                fLoopStart = 0.0f;
                fLoopEnd = 0.0f;

                pp = strstr(p, "lovel");
                if (pp)
                {
                    pp = strchr(pp, '=');
                    if (pp) pp++;
                    if (pp) lovel = atoi(pp);
                }

                pp = strstr(p, "hivel");
                if (pp)
                {
                    pp = strchr(pp, '=');
                    if (pp) pp++;
                    if (pp) hivel = atoi(pp);
                }

                pp = strstr(p, "loop_mode");
                if (pp)
                {
                    bLoop = true;
                }

                pp = strstr(p, "loop_start");
                if (pp)
                {
                    pp = strchr(pp, '=');
                    if (pp) pp++;
                    if (pp) fLoopStart = (float)atof(pp);
                }

                pp = strstr(p, "loop_end");
                if (pp)
                {
                    pp = strchr(pp, '=');
                    if (pp) pp++;
                    if (pp) fLoopEnd = (float)atof(pp);
                }

                pp = strstr(p, "sample");
                if (pp)
                {
                    pp = strchr(pp, '=');
                    if (pp) pp++;
                    while (*pp != 0 && isspace(*pp)) pp++;
                    strcpy(sampleFileName, pp);
                }

                sprintf(buf, "%s/%s", folderPath, sampleFileName);
                AKSampleFileDescriptor sfd;
                sfd.path = buf;
                sfd.sampleDescriptor.isLooping = bLoop;
                sfd.sampleDescriptor.loopStartPoint = 0.0;
                sfd.sampleDescriptor.loopStartPoint = fLoopStart;
                sfd.sampleDescriptor.loopEndPoint = fLoopEnd;
                sfd.sampleDescriptor.endPoint = 0.0f;
                sfd.sampleDescriptor.noteNumber = pitch;
                sfd.sampleDescriptor.noteFrequency = NOTE_HZ(sfd.sampleDescriptor.noteNumber);
                sfd.sampleDescriptor.minimumNoteNumber = lokey;
                sfd.sampleDescriptor.maximumNoteNumber = hikey;
                sfd.sampleDescriptor.minimumVelocity = lovel;
                sfd.sampleDescriptor.maximumVelocity = hivel;

                char* p = strrchr(sampleFileName, '.');
                strcpy(p, ".wv");
                sprintf(buf, "%s/%s", folderPath, sampleFileName);
                loadCompressedSampleFile(sfd, multiSampleIndex);
            }
        }
        fclose(pfile);

        switch (multiSampleIndex)
        {
        case 0:
            multiSample1.buildKeyMap();
            break;
        case 1:
            multiSample2.buildKeyMap();
            break;
        }
        restartVoices();

        return true;
    }

    void Synth::buildSetGetMaps()
    {
        get["osc1Phases"] = [this](char* out) { sprintf(out, "%d", voiceParams.osc1.phases); };
        get["osc1FreqSpread"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc1.freqSpread); };
        get["osc1PanSpread"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc1.panSpread); };
        get["osc1PitchOffset"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc1.pitchOffset); };
        get["osc1MixLevel"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc1.mixLevel); };
        set["osc1Phases"] = [this](char* in) { voiceParams.osc1.phases = atoi(in); updateOsc1(); };
        set["osc1FreqSpread"] = [this](char* in) { voiceParams.osc1.freqSpread = atof(in); updateOsc1(); };
        set["osc1PanSpread"] = [this](char* in) { voiceParams.osc1.panSpread = atof(in); updateOsc1(); };
        set["osc1PitchOffset"] = [this](char* in) { voiceParams.osc1.pitchOffset = atof(in); updateOsc1(); };
        set["osc1MixLevel"] = [this](char* in) { voiceParams.osc1.mixLevel = atof(in); };

        get["osc2Phases"] = [this](char* out) { sprintf(out, "%d", voiceParams.osc2.phases); };
        get["osc2FreqSpread"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc2.freqSpread); };
        get["osc2PanSpread"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc2.panSpread); };
        get["osc2PitchOffset"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc2.pitchOffset); };
        get["osc2MixLevel"] = [this](char* out) { sprintf(out, "%g", voiceParams.osc2.mixLevel); };
        set["osc2Phases"] = [this](char* in) { voiceParams.osc2.phases = atoi(in); updateOsc2(); };
        set["osc2FreqSpread"] = [this](char* in) { voiceParams.osc2.freqSpread = atof(in); updateOsc2(); };
        set["osc2PanSpread"] = [this](char* in) { voiceParams.osc2.panSpread = atof(in); updateOsc2(); };
        set["osc2PitchOffset"] = [this](char* in) { voiceParams.osc2.pitchOffset = atof(in); updateOsc2(); };
        set["osc2MixLevel"] = [this](char* in) { voiceParams.osc2.mixLevel = atof(in); };

        get["smp1MixLevel"] = [this](char* out) { sprintf(out, "%g", voiceParams.smp1.mixLevel); };
        get["smp2MixLevel"] = [this](char* out) { sprintf(out, "%g", voiceParams.smp2.mixLevel); };
        set["smp1MixLevel"] = [this](char* in) { voiceParams.smp1.mixLevel = atof(in); };
        set["smp2MixLevel"] = [this](char* in) { voiceParams.smp2.mixLevel = atof(in); };

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

        get["fltStages"] = [this](char* out) { sprintf(out, "%d", voiceParams.filterStages); };
        set["fltStages"] = [this](char* in) { voiceParams.filterStages = atoi(in); updateFilters(voiceParams.filterStages); };
        get["fltCutoff"] = [this](char* out) { sprintf(out, "%g", modParams.cutoffMultiple); };
        set["fltCutoff"] = [this](char* in) { modParams.cutoffMultiple = atof(in); };
        get["fltEgStrength"] = [this](char* out) { sprintf(out, "%g", modParams.cutoffEgStrength); };
        set["fltEgStrength"] = [this](char* in) { modParams.cutoffEgStrength = atof(in); ; };
        get["fltResonance"] = [this](char* out) { sprintf(out, "%g", -20.0f * log10(modParams.resLinear)); };
        set["fltResonance"] = [this](char* in) { modParams.resLinear = pow(10.0, -0.05 * atof(in)); };
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
