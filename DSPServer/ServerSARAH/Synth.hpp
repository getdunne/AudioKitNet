//
//  Synth.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "SynthVoice.hpp"
#include "VoiceManager.hpp"
#include "SustainPedalLogic.hpp"

#include <list>
#include <map>
#include <functional>

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

    protected:
        // dispatch tables used by command()
        std::map<std::string, std::function<void(char*)> > get;
        std::map<std::string, std::function<void(char*)> > set;
        void buildSetGetMaps();
        
        // objects shared by all voices
        FunctionTableOscillator vibratoLFO;

        // simple parameters
        SynthVoiceParams voiceParams;
        ADSREnvelopeParams ampEGParams;
        ADSREnvelopeParams filterEGParams;

        void updateOsc1();
        void updateOsc2();
        void updateFilters();

        // modulation parameters
        SynthModParams modParams;
        float pitchOffset, vibratoDepth;

        // render-prep callback
        static void renderPrepCallback(void* thisPtr);
    };
}

