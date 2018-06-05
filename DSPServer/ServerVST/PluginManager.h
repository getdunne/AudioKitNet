#pragma once
#include "pluginterfaces/vst2.x/aeffectx.h"
#include <stdint.h>
#include <map>
typedef std::map<std::string, int> ParameterMap;

// Moved these here from MyDSP.h
// eventually they need a header file of their own
typedef struct
{
    uint8_t     status;
    uint8_t     channel;
    uint8_t     data1;
    uint8_t     data2;
    uint32_t    startFrame;
} MIDIMessageInfoStruct;

typedef struct
{
    uint16_t    effectIndex;
    uint16_t    paramIndex;
    float       paramValue;
} ParamMessageStruct;


typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);

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

    bool load(const char* pathToPluginDll);
    bool open();

    bool openCustomGui();

    void checkProperties();
    void setupParamLookup(ParameterMap &pmap);

    void setProgram(int index);
    void setBlockSize(int blockSize, float** buffers);

    float getParameter(int index);
    void setParameter(int index, float value);

    void processMidi(MIDIMessageInfoStruct* pMidi, int nMessages);

    void resume();
    void processBlock();
    void suspend();

    void close();
};
