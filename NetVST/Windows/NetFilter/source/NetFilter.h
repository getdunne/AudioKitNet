#pragma once

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "NetVST.h"

//-------------------------------------------------------------------------------------------------------
class NetFilter : public NetVST
{
public:
	NetFilter (audioMasterCallback audioMaster);
	~NetFilter ();

	// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
    virtual VstInt32 canDo (char* text);

protected:
#define MAX_SEND_BYTES (sizeof(SendDataHeader)\
    + MAX_MIDI_EVENTS*sizeof(MIDIMessageInfoStruct)\
    + MAX_PARAM_EVENTS*sizeof(ParamMessageStruct))\
    + sizeof(SampleDataHeader)\
    + MAX_SAMPLE_PAIRS*CHANNELS*DATASIZE
    char sendDataBuffer[MAX_SEND_BYTES];
};
