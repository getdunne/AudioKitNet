#include "DSP_Server.h"
#include "TRACE.h"
#include <thread>

// Set true to enable processing incoming data, false for testing just network I/O
#define PROCESS_DATA true


DSP_Server::DSP_Server(MyDSP* pdsp)
    : pDSP(pdsp)
    , bCancel(false)
    , bRunning(true)
    , bEnabled(true)
    , pThread(0)
{
    TRACE("Initialize Winsock\n");
    if (!MySocketLib::Initialize()) return;

    SetAddrPort(DEFAULT_ADDR, DEFAULT_PORT);
}

DSP_Server::~DSP_Server()
{
    Stop();

    TRACE("Cleaning up\n");
    MySocketLib::Cleanup();
}

void DSP_Server::ThreadProc(DSP_Server *self)
{
    int iResult = 0;
    int iSendResult = 0;

    while (!(self->bCancel))
    {
        // Await client connection
        TRACE("Wait for new client connection\n");
        if (!self->serverSocket.Accept(self->clientSocket))
        {
            TRACE("CWorkThread_Network::ThreadProc: stopping work thread\n");
            self->bRunning = false;
            break;
        }

        // If CWorkThread_Network::Stop() was called, we're done
        if (self->bCancel)
        {
            // normal completion
            TRACE("Normal completion\n");
            break;
        }

        // Get connect header
        TRACE("Got client connection\n");
        // Get header
        ConnectHeader hdr;
        int byteCount = self->clientSocket.Recv((char*)&hdr, sizeof(ConnectHeader));
        if (byteCount <= 0)
        {
            TRACE("Error receiving ConnectHeader...\n");
            self->clientSocket.Disconnect();
            break;
        }
        TRACE("sample rate %d, block size %d\n", hdr.sampleRateHz, hdr.maxSamplesPerBlock);
        self->pDSP->setSampleRate(float(hdr.sampleRateHz));

        // Receive and process data until the peer shuts down the connection
        if (!self->ClientLoop())
        {
            TRACE("Network error: disconnect and start over\n");
            self->clientSocket.Disconnect();
            //self->bCancel = true;
        }
    }
    self->bRunning = false;
}

bool DSP_Server::Start()
{
    Stop();
    TRACE("Starting work thread on %s:%d\n", szAddr, nPort);
    if (serverSocket.Connect(szAddr, nPort))
    {
        pThread = new std::thread(ThreadProc, this);
        if (!pThread) return false;

        bCancel = false;
        bRunning = true;
        return true;
    }
    return false;
}

void DSP_Server::Stop()
{
    if (pThread)
    {
        bCancel = true;

        TRACE("Killing Work thread\n");
        if (strcmp(szAddr, ALL_PORTS_ADDR) == 0)
            clientSocket.Connect(LOOPBACK_ADDR, nPort, true);
        else
            clientSocket.Connect(szAddr, nPort, true);

        pThread->join();

        delete pThread;
        pThread = 0;
        serverSocket.Disconnect();
    }
}

bool DSP_Server::SetAddrPort(const char* pszAddr, int portNum)
{
#if 0
    // no need to disconnect if no actual change
    if (bRunning && strncmp(pszAddr, szAddr, sizeof(szAddr)) == 0 && nPort == portNum) return true;
#endif

    bool running = bRunning;
    if (running) Stop();
    strncpy(szAddr, pszAddr, sizeof(szAddr));
    nPort = portNum;
    return Start();
}

#define MS_PER_SAMPLE (1.0f/44.1f)
#define SAMPLE_INDEX_TO_MS(si) (DWORD)(si * MS_PER_SAMPLE + 0.5f)

//static SYSTEMTIME systime;
//static long now, then;

bool DSP_Server::ClientLoop()
{
    // Receive until the peer shuts down the connection
    while (!bCancel)
    {
        // Get header
        SendDataHeader hdr;
        int byteCount = clientSocket.Recv((char*)&hdr, sizeof(SendDataHeader));
        if (byteCount < 0) return false;
        if (byteCount == 0)
        {
            TRACE("Connection closing...\n");
            break;
        }
        bool sampleDataPresent = (hdr.frameCount & UINT16_BIT15_MASK) != 0;
        hdr.frameCount &= UINT16_LOW15BITS_MASK;
        //GetSystemTime(&systime);
        //then = systime.wMinute * 60000 + systime.wSecond * 1000 + systime.wMilliseconds;

        //TRACE("Header (%d bytes) %d %d %d\n", sizeof(hdr), hdr.frameCount, hdr.midiCount, hdr.timeStamp);
        int sampleBytes = hdr.frameCount * CHANNELS * DATASIZE;

        if (hdr.midiCount > 0)
        {
            // get midi data
            int byteCount = ReceiveAndProcessMIDI(hdr.midiCount);
            if (byteCount < 0) return false;
            if (byteCount == 0)
            {
                TRACE("Connection closing...\n");
                break;
            }
        }

        if (hdr.paramCount > 0)
        {
            // midi data follow header
            int byteCount = ReceiveAndProcessParamChanges(hdr.paramCount);
            if (byteCount < 0) return false;
            if (byteCount == 0)
            {
                TRACE("Connection closing...\n");
                break;
            }
        }

        if (sampleDataPresent)
        {
            // filter input sample data follow header
            int byteCount = ReceiveSamples(hdr.frameCount);
            if (byteCount < 0) return false;
            if (byteCount == 0)
            {
                TRACE("Connection closing...\n");
                break;
            }
        }
        else
        {
            // zero receive buffer again, so we don't interpret MIDI etc. as samples,
            // should the user load a filter VST (or no VST at all) instead of a synth VST
            memset(recvbuf, 0, sizeof(recvbuf));
        }

        // Process the data and send back result
        ProcessSamples(hdr.frameCount, hdr.timeStamp);
        if (!SendSamples(hdr.frameCount)) return false;
    }

    return true;
}

int DSP_Server::ReceiveAndProcessMIDI(int nMessageCount)
{
    //TRACE("ReceiveAndProcessMIDI: %d\n", nMessageCount);
    int byteCount = clientSocket.Recv(recvbuf, nMessageCount * sizeof(MIDIMessageInfoStruct));
    if (byteCount > 0)
    {
        pDSP->acceptMidi((MIDIMessageInfoStruct*)(recvbuf), nMessageCount);
    }
    return byteCount;
}

int DSP_Server::ReceiveAndProcessParamChanges(int nMessageCount)
{
    //TRACE("ReceiveAndProcessParamChanges %d:", nMessageCount);
    int byteCount = clientSocket.Recv(recvbuf, nMessageCount * sizeof(ParamMessageStruct));
    //TRACE(" byteCount %d\n", byteCount);
    if (byteCount > 0)
    {
        pDSP->acceptParamChanges((ParamMessageStruct*)(recvbuf), nMessageCount);
    }
    return byteCount;
}

int DSP_Server::ReceiveSamples(int nFrameCount)
{
    float* pInLeft = (float*)recvbuf;
    float* pInRight = pInLeft + nFrameCount;
    int byteCount = 0;  // value to return

    // get header for processed sample data
    SampleDataHeader shdr;
    byteCount = clientSocket.Recv(&shdr, sizeof(shdr));
    if (byteCount != sizeof(shdr))
    {
        TRACE("1 Expected %d bytes, received %d\n", sizeof(shdr), byteCount);
        return byteCount;
    }

    int blockBytes = nFrameCount * sizeof(float);
    byteCount = clientSocket.Recv(pInLeft, blockBytes);
    if (byteCount != blockBytes)
    {
        TRACE("2 Expected %d bytes, received %d\n", blockBytes, byteCount);
        return byteCount;
    }
    byteCount = clientSocket.Recv(pInRight, blockBytes);
    if (byteCount != blockBytes)
    {
        TRACE("3 Expected %d bytes, received %d\n", blockBytes, byteCount);
        return byteCount;
    }

    return 1;   // return any value >0 to indicate success
}

void DSP_Server::ProcessSamples(int nFrameCount, uint32_t timeStamp)
{
    //TRACE("ProcessSamples: %d\n", nFrameCount);
    float* pInLeft = (float*)recvbuf;
    float* pInRight = pInLeft + nFrameCount;
    float* pOutLeft = (float*)sendbuf;
    float* pOutRight = pOutLeft + nFrameCount;

    if (PROCESS_DATA && bEnabled)
    {
        float* buffers[4] = { pInLeft, pInRight, pOutLeft, pOutRight };
        pDSP->render(buffers, nFrameCount);
    }
    else
    {
        memcpy(pOutLeft, pInLeft, nFrameCount * sizeof(float));
        memcpy(pOutRight, pInRight, nFrameCount * sizeof(float));
    }
}

bool DSP_Server::SendSamples(int nFrameCount)
{
    float* pOutLeft = (float*)sendbuf;
    float* pOutRight = pOutLeft + nFrameCount;
    char* pData; int byteCount, bytesToSend;

    SampleDataHeader shdr;
    int blockBytes = nFrameCount * sizeof(float);
    shdr.mainByteCount = blockBytes;
    shdr.corrByteCount = 0xFFFF;

    bytesToSend = 0;
    pData = sendData;
    byteCount = sizeof(SampleDataHeader);
    memcpy(pData, &shdr, byteCount); pData += byteCount; bytesToSend += byteCount;
    byteCount = blockBytes;
    memcpy(pData, pOutLeft, byteCount); pData += byteCount; bytesToSend += byteCount;
    memcpy(pData, pOutRight, byteCount); pData += byteCount; bytesToSend += byteCount;

    return clientSocket.Send(sendData, bytesToSend);
}
