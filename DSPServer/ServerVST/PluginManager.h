#pragma once
#include "pluginterfaces/vst2.x/aeffectx.h"
#include <stdint.h>
#include <string>
#include <map>
typedef std::map<std::string, int> ParameterMap;

typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);

struct MIDIMessageInfoStruct;

class PluginManager
{
protected:
    void* module;
    PluginEntryProc mainProc;
    AEffect* plugin;
    float** inputs;
    float** outputs;
    int blockSize;

    struct MyVstEvents
    {
        VstInt32 numEvents;
        VstIntPtr alwaysZero;
        VstEvent* events[1000];
    } vstEvents;
    VstMidiEvent midiEvents[1000];

public:
    PluginManager();
    ~PluginManager();

    void setSampleRate(float sampleRateHz);
    void setTempo(double bpm);

    bool load(const char* pathToPluginDll);
    bool open();

    bool openCustomGui();
    void updateGui();

    void checkProperties();
    void setupParamLookup(ParameterMap &pmap);

    void setProgram(int index);
    void setBlockSize(int blockSize, float** buffers);

    float getParameter(int index);
    void setParameter(int index, float value);

    void processMidi(MIDIMessageInfoStruct* pMidi, int nMessages, int nFrames);

    void resume();
    void processBlock();
    void suspend();

    void close();
    void unload();
};
