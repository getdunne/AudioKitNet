//
//  Synth.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "SynthVoice.hpp"
#include "VoiceManager.hpp"
#include "WaveStack.hpp"
#include "SustainPedalLogic.hpp"
#include "MultiSample.hpp"

#include <list>

#define MAX_POLYPHONY 32
#define MIDI_NOTENUMBERS 128    // MIDI offers 128 distinct note numbers
#define CHUNKSIZE 16            // process samples in "chunks" this size

namespace AudioKitCore
{
    
    class Synth
    {
    public:
        Synth();
        ~Synth();
        
        int init(double sampleRate);    // returns system error code, nonzero only if a problem occurs
        void deinit();                  // call this to un-load all samples and clear the keymap
        
        void playNote(unsigned noteNumber, unsigned velocity, float noteHz);
        void stopNote(unsigned noteNumber, bool immediate);
        void sustainPedal(bool down);

        bool command(char* cmd);
        
        void render(unsigned channelCount, unsigned sampleCount, float *outBuffers[]);

        // call before/after loading/unloading samples, to ensure none are in use
        void stopAllVoices() { voiceManager.disable(); }
        void restartVoices() { voiceManager.enable();  }

    protected:
        // array of voice resources, and a voice manager
        SynthVoice voice[MAX_POLYPHONY];
        VoiceManager voiceManager;

        // objects shared by all voices
    public:
        MultiSample multiSample1, multiSample2;
        bool loadCompressedSampleFile(AKSampleFileDescriptor& sfd, int multiSampleIndex);
        bool loadSfz(const char* folderPath, const char* sfzFileName, int multiSampleIndex);

    protected:
        WaveStack waveStack1, waveStack2;
        FunctionTableOscillator vibratoLFO;
        
        // simple parameters
        SynthVoiceParams voiceParams;
        ADSREnvelopeParams ampEGParams;
        ADSREnvelopeParams filterEGParams;

        void updateOsc1();
        void updateOsc2();
        void updateFilters(int stages);

        // modulation parameters
        SynthModParams modParams;
        float pitchOffset, vibratoDepth;

        // render-prep callback
        static void renderPrepCallback(void* thisPtr);
    };
}

