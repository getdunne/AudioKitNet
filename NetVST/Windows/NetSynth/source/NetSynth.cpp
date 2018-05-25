#include "NetSynth.h"
#include "TRACE.h"


AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new NetSynth (audioMaster);
}

NetSynth::NetSynth (audioMasterCallback audioMaster)
    : NetVST (audioMaster, 1)	// 1 program
{
	if (audioMaster)
	{
		setNumInputs(0);
		setNumOutputs(2);
		canProcessReplacing();
		isSynth();
		setUniqueID('NetS');
	}
	suspend ();
}

NetSynth::~NetSynth ()
{
}

bool NetSynth::getOutputProperties (VstInt32 index, VstPinProperties* properties)
{
	if (index < 2)
	{
		vst_strncpy (properties->label, "Vstx ", 63);
		char temp[11] = {0};
		int2string (index + 1, temp, 10);
		vst_strncat (properties->label, temp, 63);

		properties->flags = kVstPinIsActive;
		if (index < 2)
			properties->flags |= kVstPinIsStereo;	// make channel 1+2 stereo
		return true;
	}
	return false;
}

bool NetSynth::getEffectName (char* name)
{
	vst_strncpy (name, "NetSynth", kVstMaxEffectNameLen);
	return true;
}

bool NetSynth::getVendorString (char* text)
{
	vst_strncpy (text, "Shane Dunne & Associates", kVstMaxVendorStrLen);
	return true;
}

bool NetSynth::getProductString (char* text)
{
	vst_strncpy (text, "NetSynth", kVstMaxProductStrLen);
	return true;
}

VstInt32 NetSynth::getVendorVersion ()
{ 
	return 1000; 
}

VstInt32 NetSynth::canDo (char* text)
{
    if (!strcmp (text, "hasEditor"))
        return 1;
	if (!strcmp (text, "receiveVstEvents"))
		return 1;
	if (!strcmp (text, "receiveVstMidiEvent"))
		return 1;
	return -1;	// explicitly can't do; 0 => don't know
}

VstInt32 NetSynth::getNumMidiInputChannels ()
{
	return 1; // we are monotimbral
}

VstInt32 NetSynth::getNumMidiOutputChannels ()
{
	return 0; // no MIDI output back to Host app
}

void NetSynth::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* pOutLeft = outputs[0];
    float* pOutRight = outputs[1];
    int blockBytes = sampleFrames * DATASIZE;

    if (!Connected())
    {
        // if not connected, output silence
        memset(pOutLeft, 0, blockBytes);
        memset(pOutRight, 0, blockBytes);
        return;
    }

    int bytesToSend = 0;
	char* pData = sendDataBuffer;
	int byteCount;

    // send header
    SendDataHeader hdr;
    hdr.frameCount = sampleFrames;
    hdr.midiCount = midiCount;
    hdr.paramCount = paramCount;
    hdr.timeStamp = 0;
    byteCount = sizeof(SendDataHeader);
	memcpy(pData, &hdr, byteCount); pData += byteCount; bytesToSend += byteCount;

    // send MIDI data
    if (midiCount > 0)
    {
	    byteCount = midiCount * sizeof(MIDIMessageInfoStruct);
	    memcpy(pData, midiData, byteCount); pData += byteCount; bytesToSend += byteCount;
        midiCount = 0;
    }

    // send Param updates
    if (paramCount > 0)
    {
	    byteCount = paramCount * sizeof(ParamMessageStruct);
	    memcpy(pData, paramData, byteCount); pData += byteCount; bytesToSend += byteCount;
        paramCount = 0;
    }
    
    if (!Send(sendDataBuffer, bytesToSend)) return;

    // Synth does not send sample data

    if (prevBlockBytes == 0)
    {
        // pretend we got a packet from remote synth, output silence
        memset(pOutLeft, 0, blockBytes);
        memset(pOutRight, 0, blockBytes);

        // only do this the very first time after connecting
        prevBlockBytes = blockBytes;
    }
    else
    {
        // get header for returned sample data
        SampleDataHeader shdr;
        if (Recv(&shdr, sizeof(shdr)) != sizeof(shdr)) return;
        if (Recv(pOutLeft, prevBlockBytes) != prevBlockBytes) return;
        if (Recv(pOutRight, prevBlockBytes) != prevBlockBytes) return;
        prevBlockBytes = blockBytes;
    }
}
