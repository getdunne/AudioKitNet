#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "MySocketConn.hpp"

#define MAX_CONNECT_SECONDS 5	// wait at most this long for connect() before giving up

MySocketConn::MySocketConn()
    : connected(false)
{
}

MySocketConn::~MySocketConn()
{
    if (connected) Disconnect();
}
    
bool MySocketConn::Connect (const char* serverAddr, int portNumber)
{
    if (connected) return true;
    
    struct sockaddr_in server;
    struct hostent *sp;
    
    m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    memset((char *) &server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons((u_short)portNumber);
    sp = gethostbyname(serverAddr);
    if (!sp)
    {
        herror("gethostbyname");
        return false;
    }
    memcpy(&server.sin_addr, sp->h_addr, sp->h_length);
    
    printf("trying to connect\n");
	fcntl(m_socket, F_SETFL, O_NONBLOCK); // set non-blocking
	int rc = connect(m_socket, (struct sockaddr *) &server, sizeof(struct sockaddr_in));
    if ((rc == -1) && (errno != EINPROGRESS))
	{
		herror("connect");
		goto failure;
	}
	else if (rc == 0) goto success; // immediate success
	
	printf("in progress...\n");
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(m_socket, &fdset);
    struct timeval tv;
    tv.tv_sec = MAX_CONNECT_SECONDS;
    tv.tv_usec = 0;

	rc = select(m_socket + 1, NULL, &fdset, NULL, &tv);
	printf("select returned %d\n", rc);
    if (rc == 1)
    {
        int so_error;
        socklen_t len = sizeof(so_error);
        getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &so_error, &len);
		printf("getsockopt returned so_error = %d\n", so_error);
        if (so_error == 0) goto success;
    }
	printf("timeout\n");
	
failure:
	close(m_socket);
	connected = false;
	return false;
	
success:	
	int opts = fcntl(m_socket, F_GETFL, 0);
	fcntl(m_socket, F_SETFL, opts & (~O_NONBLOCK)); // set back to blocking
	printf("Connected\n");
	connected = true;
	return true;
}

void MySocketConn::Disconnect (void)
{
	if (!connected) return;
	
    printf("Disconnecting\n");
    if (m_socket != -1) close(m_socket);
    connected = false;
}

bool MySocketConn::Send (void* pData, int byteCount)
{
    if (!connected) return true;
    long result;
    
    for (int bc=0; bc < byteCount; )
    {
        result = send(m_socket, pData, byteCount - bc, 0);
        if (result <= 0)
        {
            printf("MySocketConn::Send error %d\n", errno);
            Disconnect();
            return false;
        }
        bc += result;
        pData = (char*)pData + result;
    }
    return true;
}

bool MySocketConn::Recv (void* pData, int byteCount)
{
    if (!connected) return true;
    long result = byteCount;

    for (int bc=0; bc < byteCount; )
    {
        result = recv(m_socket, pData, byteCount - bc, 0);
        if (result <= 0)
        {
            printf("MySocketConn::Recv error %d\n", errno);
            Disconnect();
            return false;
        }
        bc += result;
        pData = (char*)pData + result;
    }
    return true;
}

