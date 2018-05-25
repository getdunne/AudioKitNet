#include "NetFilter.h"
#include "TRACE.h"

AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new NetFilter (audioMaster);
}

NetFilter::NetFilter (audioMasterCallback audioMaster)
    : NetVST (audioMaster, 1)	// 1 program
{
	if (audioMaster)
    {
	    setNumInputs (2);		// stereo in
	    setNumOutputs (2);		// stereo out
	    setUniqueID ('NetF');	// identify
	    canProcessReplacing ();	// supports replacing output
    }

    resume();
}

NetFilter::~NetFilter ()
{
}

bool NetFilter::getEffectName (char* name)
{
	vst_strncpy (name, "NetFilter", kVstMaxEffectNameLen);
	return true;
}

bool NetFilter::getProductString (char* text)
{
	vst_strncpy (text, "NetFilter", kVstMaxProductStrLen);
	return true;
}

bool NetFilter::getVendorString (char* text)
{
	vst_strncpy (text, "Shane Dunne & Associates", kVstMaxVendorStrLen);
	return true;
}

VstInt32 NetFilter::getVendorVersion ()
{ 
	return 1000; 
}

VstInt32 NetFilter::canDo (char* text)
{
    if (!strcmp (text, "hasEditor"))
        return 1;
	if (!strcmp (text, "receiveVstEvents"))
		return 1;
	if (!strcmp (text, "receiveVstMidiEvent"))
		return 1;
	return -1;	// explicitly can't do; 0 => don't know
}

void NetFilter::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* pInLeft = inputs[0];
    float* pInRight = inputs[1];
    float* pOutLeft = outputs[0];
    float* pOutRight = outputs[1];
    int blockBytes = sampleFrames * DATASIZE;

    if (!Connected())
    {
        // if not connected, just pass input to output
        memcpy(pOutLeft, pInLeft, blockBytes);
        memcpy(pOutRight, pInRight, blockBytes);
        return;
    }

    int bytesToSend = 0;
	char* pData = sendDataBuffer;
	int byteCount;

    // send header
    SendDataHeader hdr;
    hdr.frameCount = sampleFrames | UINT16_BIT15_MASK;  // indicate that samples follow
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

    // send sample data
    SampleDataHeader shdr;
    shdr.mainByteCount = blockBytes;
    shdr.corrByteCount = 0xFFFF;

    byteCount = sizeof(SampleDataHeader);
    memcpy(pData, &shdr, byteCount); pData += byteCount; bytesToSend += byteCount;
    byteCount = blockBytes;
    memcpy(pData, pInLeft, byteCount); pData += byteCount; bytesToSend += byteCount;
    memcpy(pData, pInRight, byteCount); pData += byteCount; bytesToSend += byteCount;

    if (!Send(sendDataBuffer, bytesToSend)) return;

    if (prevBlockBytes == 0)
    {
        // pretend we got a packet from remote effect (copy input to output)
        memcpy(pOutLeft, pInLeft, blockBytes);
        memcpy(pOutRight, pInRight, blockBytes);

        // only do this the very first time after connecting
        prevBlockBytes = blockBytes;
    }
    else
    {
        // get header for processed sample data
        if (Recv(&shdr, sizeof(shdr)) != sizeof(shdr)) return;
        if (Recv(pOutLeft, prevBlockBytes) != prevBlockBytes) return;
        if (Recv(pOutRight, prevBlockBytes) != prevBlockBytes) return;
        prevBlockBytes = blockBytes;
    }
}
