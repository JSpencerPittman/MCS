#include "udp.h"

namespace udp {

    Socket::Socket() {}

    Socket::Socket(const std::string& ipAddr, uint16_t port) {
        sIPAddr = ipAddr;
        unPort = port;
        
        sockaddr_in stSockAddrIP4;
        memset(&stSockAddrIP4, 0, sizeof(sockaddr_in));

        stSocketAddress.sin_family = AF_INET;
        stSocketAddress.sin_port = htons(port);
        stSocketAddress.sin_addr.s_addr = inet_addr(ipAddr.c_str());
    }

    bool StartSocket(Socket& stSocket) {
        int nFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if(nFileDescriptor < 0) return false;
        else stSocket.nFileDescriptor = nFileDescriptor;
        return true;
    }

    bool BindSocket(Socket& stSocket) {
        int nResult = bind(stSocket.nFileDescriptor, (const sockaddr*) &stSocket.stSocketAddress, sizeof(sockaddr));
        if(nResult < 0) return false;

        return true;
    }

    void CloseSocket(Socket& stSocket) {
        close(stSocket.nFileDescriptor);
    }

    bool Send(Socket& stSender, Socket& stReceiver, std::string& sMessage) {
        int nResult = sendto(stSender.nFileDescriptor,
               sMessage.c_str(),
               sMessage.size(),
               MSG_CONFIRM,
               (const sockaddr *) &stReceiver.stSocketAddress,
               sizeof(stReceiver.stSocketAddress));
        return nResult >= 0 ? true : false;
    }

    void Receive(Socket& stSender, Socket& stReceiver, std::string& sMessage, uint64_t unBufferSize) {
        char aBuffer[unBufferSize];
        socklen_t siAddrLen = sizeof(stSender.stSocketAddress);
        int nMessageLength = recvfrom(stReceiver.nFileDescriptor, 
                (char *) aBuffer, 
                unBufferSize, 
                MSG_WAITALL, 
                (sockaddr *) &stSender.stSocketAddress, 
                &siAddrLen);
        aBuffer[nMessageLength] = '\0';
        sMessage = std::string(aBuffer);
    }


};