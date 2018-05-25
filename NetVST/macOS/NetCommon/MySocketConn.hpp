#ifndef MySocketConn_hpp
#define MySocketConn_hpp

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class MySocketConn
{
    int m_socket;
    bool connected;
    
public:
    MySocketConn();
    ~MySocketConn();
    
    bool isConnected (void) { return connected; }
    bool Connect (const char* serverAddr, int portNumber);
    void Disconnect (void);
    bool Send (void* pData, int byteCount);
    bool Recv (void* pData, int byteCount);
};

#endif /* MySocketConn_hpp */
