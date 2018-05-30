#include <stdio.h>
#include <stdlib.h>
#include <WS2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

#include "MySocketLib.h"
#include "TRACE.h"

#if 1
int xp_inet_pton(int af, const char *src, void *dst);
const char *xp_inet_ntop(int af, const void *src, char *dst, socklen_t size);
#define inet_pton xp_inet_pton
#define inet_ntop xp_inet_ntop
#endif

bool MySocketLib::Initialize()
{
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        TRACE("WSAStartup failed with error: %d\n", err);
        return false;
    }
    return true;
}

void MySocketLib::Cleanup()
{
    WSACleanup();
}

MySocket::MySocket()
    : connected(false)
{
}

MySocket::~MySocket()
{
    if (connected) Disconnect();
}
    
void MySocket::Disconnect (void)
{
    if (connected)
    {
        // ensure no more send() or recv() calls will be made
        connected = false;
        TRACE("Disconnecting\n");

        // allow pending operations to complete, then close
        shutdown(m_socket, SD_BOTH);
        closesocket(m_socket);
    }
}

bool MySocket::Send (void* pData, int byteCount)
{
    if (!connected) return true;
    long result = byteCount;
    
    for (int bc=0; bc < byteCount; )
    {
        result = send(m_socket, (const char*)pData, byteCount - bc, 0);
        if (result <= 0)
        {
            TRACE("MySocket::Send error %ld\n", WSAGetLastError());
            Disconnect();
            return false;
        }
        bc += result;
        pData = (char*)pData + result;
    }
    return true;
}

int MySocket::Recv (void* pData, int byteCount)
{
    if (!connected) return 0;
    long result;
    
    for (int bc=0; bc < byteCount; )
    {
        result = recv(m_socket, (char*)pData, byteCount - bc, 0);
        if (result < 0)
        {
            TRACE("MySocket::Recv error %d\n", WSAGetLastError());
            Disconnect();
            return result;
        }
        else if (result == 0)
        {
            TRACE("MySocket::Connection dropped\n");
            Disconnect();
            return 0;
        }
        bc += result;
        pData = (char*)pData + result;
    }
    return byteCount;
}

bool MyClientSocket::Connect (const char* serverAddr, int portNumber, bool force)
{
    if (!force && connected) return true;
    
    m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket < 0)
    {
        TRACE("cannot create socket\n");
        return false;
    }
    
    struct sockaddr_in sa;
    memset((char*)&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons((u_short)portNumber);
    inet_pton(AF_INET, serverAddr, &sa.sin_addr);
    
    TRACE("trying to connect\n");
    if (-1 == connect(m_socket, (struct sockaddr *) &sa, sizeof(sa)))
    {
        TRACE("connect error\n");
        connected = false;
        return false;
    }

    TRACE("Connected\n");
    connected = true;
    return true;
}

bool MyServerSocket::Connect (const char* serverAddr, int portNumber)
{
    if (connected) return false;

    m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket < 0)
    {
        TRACE("cannot create socket");
        return false;
    }
    
    struct sockaddr_in sa;
    memset((char*)&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons((u_short)portNumber);
    inet_pton(AF_INET, serverAddr, &sa.sin_addr);

    if (bind(m_socket, (struct sockaddr*)&sa, sizeof(sa)) < 0)
    {
        TRACE("bind failed");
        Disconnect();
        return false;
    }

    if (listen(m_socket, 1) < 0)
    {
        TRACE("listen failed");
        Disconnect();
        return false;
    }

    TRACE("Connected\n");
    connected = true;
    return true;
}

bool MyServerSocket::Accept (MyClientSocket& client)
{
    SOCKET s = accept(m_socket, NULL, NULL);
    if (s == INVALID_SOCKET)
    {
        TRACE("accept failed");
        return false;
    }

    client.m_socket = s;
    client.connected = true;
    return true;
}
