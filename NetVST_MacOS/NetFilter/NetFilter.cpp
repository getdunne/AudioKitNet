#include "NetFilter.h"

#define DEBUG_PRINT 1

// Define parameter name strings here.
static CFStringRef kParameterIP1Name = CFSTR("IP_1");
static CFStringRef kParameterIP2Name = CFSTR("IP_2");
static CFStringRef kParameterIP3Name = CFSTR("IP_3");
static CFStringRef kParameterIP4Name = CFSTR("IP_4");
static CFStringRef kParameterPortNumName = CFSTR("Port#");
static CFStringRef kParameterOnName = CFSTR("On/Off");

#define DEFAULT_IP1 127
#define DEFAULT_IP2 0
#define DEFAULT_IP3 0
#define DEFAULT_IP4 1
#define DEFAULT_PORT 27016

#define MAX_SAMPLE_PAIRS_PER_BLOCK 2048
#define SAMPLE_RATE 44100

COMPONENT_ENTRY(NetFilter)

// Define the presets.
static AUPreset kPresets[kNumberOfPresets] = {
    { kPreset_Default, CFSTR("Factory Default") },
    // { kPreset_One, CFSTR("Preset One") },
    // { kPreset_Two, CFSTR("Preset Two") },
};

NetFilter::NetFilter(AudioUnit component)
	: AUEffectBase(component)
{
	pSendData = (char*)malloc(sizeof(SendDataHeader)
							  + MAXMIDIMSGS*sizeof(MIDIMessageInfoStruct)
							  + MAXPARAMS*sizeof(ParamMessageStruct)
							  + sizeof(SampleDataHeader)
							  + MAX_SAMPLE_PAIRS_PER_BLOCK*2*sizeof(float));
	sendHdr.midiCount = 0;
	sendHdr.paramCount = 0;
	
	CreateElements();
	Globals()->UseIndexedParameters(kNumberOfParameters);
	
    SetParameter(kParam_IP1, kAudioUnitScope_Global, 0, DEFAULT_IP1, 0);
    SetParameter(kParam_IP2, kAudioUnitScope_Global, 0, DEFAULT_IP2, 0);
    SetParameter(kParam_IP3, kAudioUnitScope_Global, 0, DEFAULT_IP3, 0);
    SetParameter(kParam_IP4, kAudioUnitScope_Global, 0, DEFAULT_IP4, 0);
    SetParameter(kParam_PortNum, kAudioUnitScope_Global, 0, DEFAULT_PORT, 0);
    SetParameter(kParam_On, kAudioUnitScope_Global, 0, 0, 0);
    
    SetAFactoryPresetAsCurrent(kPresets[kPreset_Default]);
	
#if AU_DEBUG_DISPATCHER
	mDebugDispatcher = new AUDebugDispatcher (this);
#endif
	
}

NetFilter::~NetFilter()
{
    Cleanup();
	free(pSendData);

#if AU_DEBUG_DISPATCHER
	delete mDebugDispatcher;
#endif
}

OSStatus NetFilter::Initialize()
{
    OSStatus stat = AUEffectBase::Initialize();
    return stat;
}

void NetFilter::Cleanup()
{
    server.Disconnect();
    AUEffectBase::Cleanup();
}

ComponentResult NetFilter::GetPropertyInfo (AudioUnitPropertyID	inID,
										   AudioUnitScope		inScope,
										   AudioUnitElement	inElement,
										   UInt32 &		outDataSize,
										   Boolean &		outWritable)
{
	if (inScope == kAudioUnitScope_Global) {
		switch (inID) {
			case kAudioUnitProperty_CocoaUI:
				outWritable = false;
				outDataSize = sizeof (AudioUnitCocoaViewInfo);
				return noErr;
				
		}
	}
	
	return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}

ComponentResult	NetFilter::GetProperty(	AudioUnitPropertyID inID,
									  AudioUnitScope 		inScope,
									  AudioUnitElement 	inElement,
									  void *			outData )
{
	if (inScope == kAudioUnitScope_Global) {
		switch (inID) {
			case kAudioUnitProperty_CocoaUI:
			{
				// Look for a resource in the main bundle by name and type.
				CFBundleRef bundle = CFBundleGetBundleWithIdentifier( CFSTR("com.getdunne.NetFilter") );
				
				if (bundle == NULL) {
					printf("Could not find bundle specified for GUI resources\n");
					return fnfErr;
				}
                
				CFURLRef bundleURL = CFBundleCopyResourceURL( bundle, 
															 CFSTR("NetFilterUI"),
															 CFSTR("bundle"), 
															 NULL);
                
                if (bundleURL == NULL) {
					printf("Could not create resource URL for GUI\n");
					return fnfErr;
				}
                
				CFStringRef className = CFSTR("NetFilter_CocoaViewFactory");
				AudioUnitCocoaViewInfo cocoaInfo = { bundleURL, { className }};
				*((AudioUnitCocoaViewInfo *)outData) = cocoaInfo;
				
				return noErr;
			}
		}
	}
	
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}

enum { kAudioUnitParameterFlag_OmitFromPresets = (1L << 13) };

OSStatus NetFilter::GetParameterInfo(AudioUnitScope          inScope,
									   AudioUnitParameterID    inParameterID,
									   AudioUnitParameterInfo  &outParameterInfo)
{
    if (inScope != kAudioUnitScope_Global) return kAudioUnitErr_InvalidParameter;
    
    outParameterInfo.flags = kAudioUnitParameterFlag_IsWritable | kAudioUnitParameterFlag_IsReadable;
    
    switch(inParameterID)
    {
        case kParam_IP1:
            AUBase::FillInParameterName (outParameterInfo, kParameterIP1Name, false);
            outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
            outParameterInfo.minValue = 0;
            outParameterInfo.maxValue = 255;
            outParameterInfo.defaultValue = DEFAULT_IP1;
            break;
        case kParam_IP2:
            AUBase::FillInParameterName (outParameterInfo, kParameterIP2Name, false);
            outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
            outParameterInfo.minValue = 0;
            outParameterInfo.maxValue = 255;
            outParameterInfo.defaultValue = DEFAULT_IP2;
            break;
        case kParam_IP3:
            AUBase::FillInParameterName (outParameterInfo, kParameterIP3Name, false);
            outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
            outParameterInfo.minValue = 0;
            outParameterInfo.maxValue = 255;
            outParameterInfo.defaultValue = DEFAULT_IP3;
            break;
        case kParam_IP4:
            AUBase::FillInParameterName (outParameterInfo, kParameterIP4Name, false);
            outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
            outParameterInfo.minValue = 0;
            outParameterInfo.maxValue = 255;
            outParameterInfo.defaultValue = DEFAULT_IP3;
            break;
        case kParam_PortNum:
            AUBase::FillInParameterName (outParameterInfo, kParameterPortNumName, false);
            outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
            outParameterInfo.minValue = DEFAULT_PORT;
            outParameterInfo.maxValue = DEFAULT_PORT + 32;
            outParameterInfo.defaultValue = DEFAULT_PORT;
            break;
        case kParam_On:
            AUBase::FillInParameterName (outParameterInfo, kParameterOnName, false);
            outParameterInfo.unit = kAudioUnitParameterUnit_Boolean;
            outParameterInfo.minValue = 0;
            outParameterInfo.maxValue = 1;
            outParameterInfo.defaultValue = 0;
            outParameterInfo.flags |= kAudioUnitParameterFlag_OmitFromPresets;
            break;
        default:
            return kAudioUnitErr_InvalidParameter;
    }
    
    return noErr;
}

OSStatus NetFilter::SetParameter(AudioUnitParameterID      inID,
								   AudioUnitScope            inScope,
								   AudioUnitElement          inElement,
								   AudioUnitParameterValue   inValue,
								   UInt32                    inBufferOffsetInFrames)
{
    if (inScope != kAudioUnitScope_Global) return kAudioUnitErr_InvalidParameter;
    printf("SetParameter %d to %f\n", (int)inID, inValue);
    
    switch (inID)
    {
        case kParam_IP1:
        case kParam_IP2:
        case kParam_IP3:
        case kParam_IP4:
        case kParam_PortNum:
            break;
        case kParam_On:
            if (!server.isConnected() && inValue != 0)
            {
                char ipAddr[16];
                int ip1 = Globals()->GetParameter(kParam_IP1);
                int ip2 = Globals()->GetParameter(kParam_IP2);
                int ip3 = Globals()->GetParameter(kParam_IP3);
                int ip4 = Globals()->GetParameter(kParam_IP4);
                sprintf(ipAddr, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
                int portNum = Globals()->GetParameter(kParam_PortNum);
                printf("Connecting to %s:%d\n", ipAddr, portNum);
                if (!server.Connect(ipAddr, portNum)) inValue = 0;
				else noPacketSentYet = true;
            }
            else if (server.isConnected() && inValue == 0) server.Disconnect();
            break;
        default:
            return kAudioUnitErr_InvalidParameter;
    }
    
    return AUBase::SetParameter(inID, inScope, inElement, inValue, inBufferOffsetInFrames);
}

#include <sys/time.h>

// Only override Render() to stash the time stamp
OSStatus NetFilter::Render(AudioUnitRenderActionFlags &    ioActionFlags,
                             const AudioTimeStamp &          inTimeStamp,
                             UInt32                          inNumberFrames)
{
    timeStamp = inTimeStamp.mSampleTime;
    return AUEffectBase::Render(ioActionFlags, inTimeStamp, inNumberFrames);
}

OSStatus NetFilter::ProcessBufferLists(AudioUnitRenderActionFlags &ioActionFlags,
                                         const AudioBufferList &inBuffer,
                                         AudioBufferList &outBuffer,
                                         UInt32 inFramesToProcess)
{
    float *pInLeft = (Float32 *)inBuffer.mBuffers[0].mData;
    float *pInRight = (Float32 *)inBuffer.mBuffers[1].mData;
    float *pOutLeft = (Float32 *)outBuffer.mBuffers[0].mData;
    float *pOutRight = (Float32 *)outBuffer.mBuffers[1].mData;
	int blockBytes = inFramesToProcess * sizeof(float);
	
	UInt32 frameTime;
	int bytesToSend;
	char* pData;
	int byteCount;

	if (!server.isConnected()) goto justCopy;
	
	// update sendHdr and midiData[]
	sendHdr.frameCount = inFramesToProcess | 0x8000;	// set high bit to indicate sample data follow
	frameTime = sendHdr.timeStamp = timeStamp;
	for (int i=0; i < sendHdr.midiCount; i++) midiData[i].startFrame += frameTime;
	
	// send header, then MIDI data, then param data, then sample data
	bytesToSend = 0;
	pData = pSendData;
	byteCount = sizeof(SendDataHeader);
	memcpy(pData, &sendHdr, byteCount); pData += byteCount; bytesToSend += byteCount;
	byteCount = sendHdr.midiCount * sizeof(MIDIMessageInfoStruct);
	memcpy(pData, midiData, byteCount); pData += byteCount; bytesToSend += byteCount;
	byteCount = sendHdr.paramCount * sizeof(ParamMessageStruct);
	memcpy(pData, paramData, byteCount); pData += byteCount; bytesToSend += byteCount;
	
	// send sample data
	SampleDataHeader shdr;
	shdr.mainByteCount = blockBytes;
	shdr.corrByteCount = 0xFFFF;
	byteCount = sizeof(shdr);
	memcpy(pData, &shdr, byteCount); pData += byteCount; bytesToSend += byteCount;
	byteCount = blockBytes;
	memcpy(pData, pInLeft, byteCount); pData += byteCount; bytesToSend += byteCount;
	memcpy(pData, pInRight, byteCount); pData += byteCount; bytesToSend += byteCount;

	// Send all assembled data in one big chunk (avoids packet buffering delays)
	if (!server.Send(pSendData, bytesToSend)) goto justCopy;
	
	if (noPacketSentYet)
	{
		// first time only: pretend we got a (silent) packet from remote synth
		noPacketSentYet = false;
		prevBlockbytes = blockBytes;
		goto justCopy;
	}

	// get header for processed sample data
	if (!server.Recv(&shdr, (int)sizeof(shdr))) goto justCopy;
	if (!server.Recv(pOutLeft, prevBlockbytes)) goto justCopy;
	if (!server.Recv(pOutRight, prevBlockbytes)) goto justCopy;
	prevBlockbytes = blockBytes;
	
done:
    sendHdr.midiCount = 0;
	sendHdr.paramCount = 0;
    return noErr;
	
justCopy:
	// just pass input to output
	memcpy(pOutLeft, pInLeft, blockBytes);
	memcpy(pOutRight, pInRight, blockBytes);
    goto done;
}

OSStatus NetFilter::HandleMidiEvent(UInt8 status, UInt8 channel, UInt8	data1, UInt8 data2, UInt32 inStartFrame)
{
    //printf("Note: %d %d %d %d %d\n", status, channel, data1, data2, inStartFrame);
    if (server.isConnected())
    {
        MIDIMessageInfoStruct midiMsg = {status, channel, data1, data2, inStartFrame};
        if (sendHdr.midiCount < MAXMIDIMSGS-1)
            midiData[sendHdr.midiCount++] = midiMsg;
    }
    return noErr;
}

OSStatus NetFilter::HandleNonNoteEvent(UInt8 status, UInt8 channel, UInt8	data1, UInt8 data2, UInt32 inStartFrame)
{
    printf("NonNote: %d %d %d %d %d\n", status, channel, data1, data2, (int)inStartFrame);
    return noErr;
}
