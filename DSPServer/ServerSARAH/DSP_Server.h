#pragma once
#include <stdint.h>
#include <thread>
#include "MySocketLib.h"
#include "MyDSP.h"

#define MAXFRAMES 2048
#define CHANNELS 2
#define DATASIZE sizeof(float)
#define BUFLEN (MAXFRAMES * CHANNELS * DATASIZE)
#define ALL_PORTS_ADDR "0.0.0.0"    // listen on all available interfaces
#define LOOPBACK_ADDR "127.0.0.1"   // listen on loopback interface only
#define DEFAULT_ADDR ALL_PORTS_ADDR
#define DEFAULT_PORT 27016

#pragma pack(push, 1)

#define UINT16_BIT15_MASK       0x8000
#define UINT16_LOW15BITS_MASK   0x7FFF

typedef struct
{
    uint16_t    frameCount;
    uint16_t    midiCount;
    uint16_t    paramCount;
    uint16_t    padding;
    uint32_t    timeStamp;
} SendDataHeader;

// moved to MyDSP.h
//typedef struct
//{
//    uint8_t     status;
//    uint8_t     channel;
//    uint8_t     data1;
//    uint8_t     data2;
//    uint32_t    startFrame;
//} MIDIMessageInfoStruct;

//typedef struct
//{
//    uint16_t    effectIndex;
//    uint16_t    paramIndex;
//    float       paramValue;
//} ParamMessageStruct;

typedef struct
{
    uint16_t    mainByteCount;      // or total bytecount if uncompressed
    uint16_t    corrByteCount;      // 0 = lossy compression, 0xFFFF = uncompressed
} SampleDataHeader;

#pragma pack(pop)

class DSP_Server
{
protected:
    MyDSP* pDSP;
    bool volatile bCancel;                      // cancel (stop) flag
    bool volatile bRunning;                     // true while running
    bool bEnabled;                              // if false, thread should essentially do nothing

public:
    DSP_Server(MyDSP* pdsp);
    ~DSP_Server();
    void Enable() { bEnabled = true; }
    void Disable() { bEnabled = false; }

    void Stop();
    bool Start();
    bool isRunning() { return bRunning; }

    bool command(char* cmd) { return pDSP->command(cmd); }

    bool SetAddrPort(const char* pszAddr, int portNum);

protected:
    char szAddr[16];        // socket IP address to listen on
    int nPort;              // socket port number to listen on
    MyServerSocket serverSocket;
    MyClientSocket clientSocket;

    char recvbuf[BUFLEN], sendbuf[BUFLEN];
    char sendData[sizeof(SampleDataHeader) + BUFLEN];

    // Helpers for thread procedure
    bool ClientLoop();
    int ReceiveAndProcessMIDI(int nMessageCount);  // return bytecount, or 0 if client disconnected, <0 for error
    int ReceiveAndProcessParamChanges(int nMessageCount);  // return bytecount, or 0 on disconnect, <0 on errr
    int ReceiveSamples(int nFrameCount);           // return bytecount, or 0 if client disconnected, <0 for error
    void ProcessSamples(int nFrameCount, uint32_t timeStamp);
    bool SendSamples(int nFrameCount);   // return false only if socket send failed

    std::thread *pThread;
    static void ThreadProc(DSP_Server *self);
};
