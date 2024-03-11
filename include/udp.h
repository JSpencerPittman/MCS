#ifndef UDP_H
#define UDP_H

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include <WinSock2.h>

#include <iostream>

namespace udp {


    struct Socket {
        Socket(const std::string& ipAddr, uint16_t port, bool reciever);

        uint16_t unPort;
        std::string sIPAddr;
        bool bReceiver;
        int nFileDescriptor;

        sockaddr_in stSenderAddress;
        sockaddr_in stRecieverAddress;
    }; 

    bool StartSocket(Socket& stSocket);
    void CloseSocket(Socket& stSocket);

    bool Send(Socket& stSocket, std::vector<unsigned char>& sMessage);
    void Receive(Socket& stSocket, std::vector<unsigned char>& sMessage, uint64_t unBufferSize);
};

#endif