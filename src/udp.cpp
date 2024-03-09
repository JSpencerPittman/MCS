#include "udp.h"

namespace udp {

    bool StartSocket(Socket& stSocket) {
        int nFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if(nFileDescriptor < 0) return false;
        else stSocket.nFileDescriptor = nFileDescriptor;
        return true;
    }

    bool BindSocket(Socket& stSocket) {
        sockaddr_in stSockAddrIP4;
        
        memset(&stSockAddrIP4, 0, sizeof(sockaddr_in));
        
        stSockAddrIP4.sin_family = AF_INET;
        stSockAddrIP4.sin_port = htons(stSocket.unPort);
        stSockAddrIP4.sin_addr.s_addr = inet_addr(stSocket.sIPAddr.c_str());
        stSocket.stSocketAddress = stSockAddrIP4;

        int nResult = bind(stSocket.nFileDescriptor, (const sockaddr*) &stSockAddrIP4, sizeof(sockaddr));
        if(nResult < 0) return false;

        return true;
    }

    void CloseSocket(Socket& stSocket) {
        close(stSocket.nFileDescriptor);
    }

    void Send(Socket& stReceiver, std::string& sMessage) {
        sendto(stReceiver.nFileDescriptor,
               sMessage.c_str(),
               sMessage.size(),
               MSG_CONFIRM,
               (const sockaddr *) &stReceiver.stSocketAddress,
               sizeof(stReceiver.stSocketAddress));
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