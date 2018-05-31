//
//  VoiceManager.hpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "VoiceBase.hpp"
#include "SustainPedalLogic.hpp"
#include <vector>

#define MIDI_NOTENUMBERS 128    // MIDI offers 128 distinct note numbers
#define CHUNKSIZE 16            // process samples in "chunks" this size

namespace AudioKitCore
{

    typedef std::vector<VoiceBase*> VoicePointerArray;

    class VoiceManager
    {
    public:
        typedef void (*RenderPrepCallback)(void*);

        VoiceManager();
        ~VoiceManager();
        
        int init(VoicePointerArray voiceArray, int polyphony, RenderPrepCallback renderPrepCallback, void* callbackPtr);
        void deinit();

        bool setPolyphony(int polyphony);
        
        void playNote(unsigned noteNumber, unsigned velocity, float noteHz);
        void stopNote(unsigned noteNumber, bool immediate);
        void sustainPedal(bool down);
        void stopAll(void);

        void enable() { disabled = false; }
        void disable() { disabled = true; }
        
        void render(unsigned sampleCount, float *outBuffers[]);
        
    protected:
        bool disabled;
        int nCurrentPolyphony;
        VoicePointerArray voice;
        VoiceBase* voiceAllocatedToKey[MIDI_NOTENUMBERS];

        // "event" counter for voice-stealing (reallocation)
        unsigned eventCounter;

        // sustain pedal management
        SustainPedalLogic pedalLogic;

        // actually start/stop notes
        void play(unsigned noteNumber, unsigned velocity, float noteHz);
        void stop(unsigned noteNumber, bool immediate);

        // return first voice playing given note, or null if there is none
        VoiceBase* voicePlayingNote(unsigned noteNumber);

        // pointer to client-supplied function called just before rendering each block
        // Note this will be called on the audio rendering thread.
        RenderPrepCallback doRenderPrep;
        void* cbPtr;
    };
}

