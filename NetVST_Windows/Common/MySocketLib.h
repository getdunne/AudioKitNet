#pragma once
#include <WinSock2.h>

class MySocketLib
{
public:
    static bool Initialize();
    static void Cleanup();
};

class MySocket
{
protected:
    friend class MyServerSocket;
    SOCKET m_socket;
    bool connected;
    
public:
    MySocket();
    ~MySocket();
    
    bool isConnected (void) { return connected; }

    void Disconnect (void);
    bool Send (void* pData, int byteCount);

    // Returns bytecount on success, 0 if connection closed from other end,
    // or some negative number indicating an error code on any error.
    int Recv (void* pData, int byteCount);
};

class MyClientSocket : public MySocket
{
public:
    MyClientSocket() : MySocket() {}

    bool Connect (const char* serverAddr, int portNumber, bool force=false);
};

class MyServerSocket : public MySocket
{
protected:
    MyClientSocket client;

public:
    MyServerSocket() : MySocket() {}

   bool Connect (const char* serverAddr, int portNumber);
   bool Accept (MyClientSocket& client);
};
