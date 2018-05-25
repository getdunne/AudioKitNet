#pragma once
#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "MySocketLib.h"

#pragma pack(push, 1)

#define UINT16_BIT15_MASK       0x8000
#define UINT16_LOW15BITS_MASK   0x7FFF

typedef struct
{
	UINT16	frameCount; // set high bit to indicate "input sample data follow"
	UINT16	midiCount;
    UINT16  paramCount;
    UINT16  padding;
	UINT32	timeStamp;
} SendDataHeader;

typedef struct
{
	UINT8	status;
	UINT8	channel;
	UINT8	data1;
	UINT8	data2;
	UINT32	startFrame;
} MIDIMessageInfoStruct;

typedef struct
{
    UINT16  effectIndex;
    UINT16  paramIndex;
    FLOAT   paramValue;
} ParamMessageStruct;

typedef struct
{
    UINT16  mainByteCount;      // or total bytecount if uncompressed
    UINT16  corrByteCount;      // 0 = lossy compression, 0xFFFF = uncompressed
} SampleDataHeader;

#pragma pack(pop)

#define CHANNELS 2
#define DATASIZE sizeof(float)
#define MAX_MIDI_EVENTS 500
#define MAX_PARAM_EVENTS 50
#define MAX_EFFECT_INDEX 31
#define MAX_PARAM_INDEX 500
#define MAX_SAMPLE_PAIRS 2048

enum
{
    // Parameters
    kOn = 0,
    kIP1,
    kIP2,
    kIP3,
    kIP4,
    kPort,

	kNumParams
};

class NetVST : public AudioEffectX
{
public:
	NetVST (audioMasterCallback audioMaster, VstInt32 numPrograms);
	~NetVST ();

	virtual void getParameterName (VstInt32 index, char* text);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual VstInt32 processEvents (VstEvents* events);

protected:
    bool Connected (void) { return server.isConnected(); }
    bool Send (void* pData, int byteCount) { return server.Send(pData, byteCount); }
    int Recv (void* pData, int byteCount) { return server.Recv(pData, byteCount); }

    float fOn;
    float fIP1, fIP2, fIP3, fIP4;
    float fPort;
    float fEffIdx, fParamIdx, fParamValue;

    void DisplayOnOff (float f, char* text);
    void DisplayIndex (float f, int max, char* text);
    void DisplayFloat (float f, char* text);
    void DisplayPort (float f, char* text);

    MyClientSocket server;
    void Disconnect (void);
    void Connect (void);

    int prevBlockBytes;     // remembers block size for response to previous packet

    MIDIMessageInfoStruct midiData[MAX_MIDI_EVENTS];
    int midiCount;

    ParamMessageStruct paramData[MAX_PARAM_EVENTS];
    int paramCount;
};
