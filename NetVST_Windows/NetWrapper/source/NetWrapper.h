#pragma once
#include "public.sdk/source/vst2.x/aeffeditor.h"
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
#define MAX_PARAM_EVENTS 100

class NetWrapperEditor : public AEffEditor
{
private:
    AEffect* m_pEffect;

public:
    NetWrapperEditor (AEffect* effect, AudioEffect* wrapper);
    virtual ~NetWrapperEditor () {}

	virtual bool getRect (ERect** rect);
	virtual bool open (void* ptr);
	virtual void close ();
	virtual bool isOpen () { return systemWindow != 0; }
	virtual void idle ();
};

class NetWrapper : public AudioEffectX
{
private:
    void* m_pModule;
    AEffect* m_pEffect;
    NetWrapperEditor* m_pEditor;

public:
	NetWrapper (void* pModule, AEffect* effect, audioMasterCallback audioMaster, VstInt32 numPrograms, VstInt32 numParams);
	~NetWrapper ();

    //virtual VstIntPtr dispatcher (VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();

	virtual VstInt32 canDo (char* text);
	virtual VstInt32 getNumMidiInputChannels ();
	virtual VstInt32 getNumMidiOutputChannels ();

	virtual void getParameterName (VstInt32 index, char* text);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);

    virtual void setProgram (VstInt32 program);
    virtual void getProgramName (char* name);

	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual VstInt32 processEvents (VstEvents* events);

    // host-callback helpers
    void StuffParamMsg (VstInt32 index, float value);

protected:
    MyClientSocket server;
    MIDIMessageInfoStruct midiData[MAX_MIDI_EVENTS];
    int midiCount;
    ParamMessageStruct paramData[MAX_PARAM_EVENTS];
    int paramCount;
    int prevBlockBytes;

    void Connect (const char* ipAddr, int portNum);
    void Disconnect (void) { server.Disconnect(); }
    bool Connected (void) { return server.isConnected(); }
    bool Send (void* pData, int byteCount) { return server.Send(pData, byteCount); }
    int Recv (void* pData, int byteCount) { return server.Recv(pData, byteCount); }
};
