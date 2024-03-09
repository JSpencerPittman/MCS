#ifndef UDP_H
#define UDP_H

#include <cstdint>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace udp {

    struct Socket {
        uint16_t unPort;
        std::string sIPAddr;
        int nFileDescriptor;

        sockaddr_in stSocketAddress;
    }; 

    bool StartSocket(Socket& stSocket);
    bool BindSocket(Socket& stSocket);
    void CloseSocket(Socket& stSocket);

    void Send(Socket& stReceiver, std::string& sMessage);
    void Receive(Socket& stSender, Socket& stReceiver, std::string& sMessage, uint64_t unBufferSize);

};

#endif