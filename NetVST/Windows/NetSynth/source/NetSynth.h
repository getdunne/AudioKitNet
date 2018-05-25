#pragma once
#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "NetVST.h"

class NetSynth : public NetVST
{
public:
	NetSynth (audioMasterCallback audioMaster);
	~NetSynth ();

	virtual bool getOutputProperties (VstInt32 index, VstPinProperties* properties);
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
	virtual VstInt32 canDo (char* text);
	virtual VstInt32 getNumMidiInputChannels ();
	virtual VstInt32 getNumMidiOutputChannels ();

	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);

protected:
#define MAX_SEND_BYTES (sizeof(SendDataHeader)\
    + MAX_MIDI_EVENTS*sizeof(MIDIMessageInfoStruct)\
    + MAX_PARAM_EVENTS*sizeof(ParamMessageStruct))
    char sendDataBuffer[MAX_SEND_BYTES];
};
