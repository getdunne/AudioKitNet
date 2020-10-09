#pragma once
#include <stdint.h>
#include <thread>
#include "MySocketLib.h"
#include "MyDSP.h"
#include "Protocol.h"

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
    bool isRunning() { if (bRunning) pDSP->idle(); return bRunning; }

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
