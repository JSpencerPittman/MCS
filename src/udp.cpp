#include "udp.h"

namespace udp {

    Socket::Socket(const std::string& ipAddr, uint16_t port, bool reciever) {
        sIPAddr = ipAddr;
        unPort = port;
        bReceiver = reciever;
        
        // sockaddr_in stSockAddrIP4;
        // memset(&stSockAddrIP4, 0, sizeof(sockaddr_in));

        memset(&stSenderAddress, 0, sizeof(sockaddr_in));
        memset(&stRecieverAddress, 0, sizeof(sockaddr_in));

        stRecieverAddress.sin_family = AF_INET;
        stRecieverAddress.sin_port = htons(port);
        stRecieverAddress.sin_addr.s_addr = inet_addr(ipAddr.c_str());
    }

    bool StartSocket(Socket& stSocket) {
        int nFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if(nFileDescriptor < 0) return false;
        else stSocket.nFileDescriptor = nFileDescriptor;
        return true;
    }

    bool BindSocket(Socket& stSocket) {
        int nResult = bind(stSocket.nFileDescriptor, (const sockaddr*) &stSocket.stRecieverAddress, sizeof(sockaddr));
        if(nResult < 0) return false;

        return true;
    }

    void CloseSocket(Socket& stSocket) {
        close(stSocket.nFileDescriptor);
    }

    bool Send(Socket& stSocket, std::string& sMessage) {
        int nResult = sendto(stSocket.nFileDescriptor,
               sMessage.c_str(),
               sMessage.size(),
               MSG_CONFIRM,
               (const sockaddr *) &stSocket.stRecieverAddress,
               sizeof(stSocket.stRecieverAddress));
        return nResult >= 0 ? true : false;
    }

    void Receive(Socket& stSocket, std::string& sMessage, uint64_t unBufferSize) {
        char aBuffer[unBufferSize];
        socklen_t siAddrLen = sizeof(stSocket.stSenderAddress);
        int nMessageLength = recvfrom(stSocket.nFileDescriptor, 
                (char *) aBuffer, 
                unBufferSize, 
                MSG_WAITALL, 
                (sockaddr *) &stSocket.stSenderAddress, 
                &siAddrLen);
        aBuffer[nMessageLength] = '\0';
        sMessage = std::string(aBuffer);
    }

};