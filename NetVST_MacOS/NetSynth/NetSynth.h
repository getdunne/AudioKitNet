#include "NetSynthVersion.h"
#include "AUInstrumentBase.h"
#include "MySocketConn.hpp"
#include <vector>

#pragma pack(push, 1)

typedef struct
{
    UInt16 frameCount;
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

class NetSynth : public AUMonotimbralInstrumentBase
{
protected:
    MySocketConn server;
	SendDataHeader sendHdr;
	MIDIMessageInfoStruct midiData[MAXMIDIMSGS];
	ParamMessageStruct paramData[MAXPARAMS];
	char* pSendData;
	bool noPacketSentYet;	// true right after connection, until 1st packet sent
	int prevBlockBytes;		// remembers size of last sample block (Logic Pro changes it)
    
public:
	NetSynth(ComponentInstance inComponentInstance);
	virtual ~NetSynth();
				
    virtual OSStatus   Initialize();
    virtual void       Cleanup();
    virtual OSStatus   Version() { return kNetSynthVersion; }
	
#if 0
    virtual Float64     GetLatency() {return 0.1;}
    virtual Float64     GetTailTime() {return 0.5;}
#endif
    virtual	bool        SupportsTail () { return true; }
	
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
    
    virtual	OSStatus   Render(AudioUnitRenderActionFlags &ioActionFlags,
                              const AudioTimeStamp &     inTimeStamp,
                              UInt32                     nFrames);
    
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
