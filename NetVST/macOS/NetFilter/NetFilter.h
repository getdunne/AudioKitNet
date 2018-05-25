#include "AUEffectBase.h"
#include "NetFilterVersion.h"
#include "MySocketConn.hpp"

#if AU_DEBUG_DISPATCHER
	#include "AUDebugDispatcher.h"
#endif


#ifndef __NetFilter_h__
#define __NetFilter_h__

#pragma pack(push, 1)

typedef struct
{
    UInt16 frameCount;	// set high bit to indicate that sample data follow
    UInt16 midiCount;
	UInt16 paramCount;
	UInt16 padding;
    UInt32 timeStamp;
} SendDataHeader;

typedef struct
{
    UInt8	status;
    UInt8	channel;
    UInt8	data1;
    UInt8	data2;
    UInt32	startFrame;
} MIDIMessageInfoStruct;

typedef struct
{
	UInt16 effectIndex;
	UInt16 paramIndex;
	float  paramValue;
} ParamMessageStruct;

typedef struct
{
	UInt16 mainByteCount;	// or total bytecount if uncompressed
	UInt16 corrByteCount;	// 0 = lossy compression, 0xFFFF = uncompressed
} SampleDataHeader;

#pragma pack(pop)

#define MAXMIDIMSGS 500
#define MAXPARAMS 50


// parameters
enum {
    kParam_IP1=0,
    kParam_IP2,
    kParam_IP3,
    kParam_IP4,
    kParam_PortNum,
    kParam_On,
    
    kNumberOfParameters
};

// Define constants to identify factory presets.
enum Preset {
    kPreset_Default = 0,
    
    kNumberOfPresets
};

class WavPackEncoder;
class WavPackDecoder;

class NetFilter : public AUEffectBase
{
protected:
    MySocketConn server;
    UInt32 timeStamp;
	SendDataHeader sendHdr;
	MIDIMessageInfoStruct midiData[MAXMIDIMSGS];
	ParamMessageStruct paramData[MAXPARAMS];
	char* pSendData;
	bool noPacketSentYet;	// true right after connection, until 1st packet sent
	int prevBlockbytes;		// remembers size of last sample block (Logic Pro changes it)
	
public:
	NetFilter(AudioUnit component);
	virtual ~NetFilter ();
	
    virtual OSStatus Initialize();
    virtual void Cleanup();
	virtual OSStatus Version() { return kNetFilterVersion; }
	
 	virtual	bool SupportsTail () { return true; }
    
	virtual ComponentResult		GetPropertyInfo(AudioUnitPropertyID		inID,
												AudioUnitScope			inScope,
												AudioUnitElement		inElement,
												UInt32 &			outDataSize,
												Boolean	&			outWritable );
	
	virtual ComponentResult		GetProperty(AudioUnitPropertyID inID,
											AudioUnitScope 		inScope,
											AudioUnitElement 	inElement,
											void *			outData);
	
    virtual OSStatus    GetParameterInfo(AudioUnitScope     inScope,
										 AudioUnitParameterID   inParameterID,
										 AudioUnitParameterInfo &outParameterInfo);
    
    virtual OSStatus 	SetParameter(AudioUnitParameterID       inID,
                                     AudioUnitScope             inScope,
                                     AudioUnitElement           inElement,
                                     AudioUnitParameterValue    inValue,
                                     UInt32                     inBufferOffsetInFrames);
    
	virtual OSStatus Render(AudioUnitRenderActionFlags &    ioActionFlags,
							const AudioTimeStamp &          inTimeStamp,
							UInt32                          inNumberFrames);
	
	virtual OSStatus ProcessBufferLists(AudioUnitRenderActionFlags &ioActionFlags,
										const AudioBufferList &inBuffer,
										AudioBufferList &outBuffer,
										UInt32 inFramesToProcess);

    virtual OSStatus   HandleMidiEvent( UInt8	status,
									   UInt8	channel,
									   UInt8	data1,
									   UInt8	data2,
									   UInt32	inStartFrame);
    virtual OSStatus HandleNonNoteEvent( UInt8	status,
										UInt8	channel,
										UInt8	data1,
										UInt8	data2,
										UInt32	inStartFrame);
};

#endif