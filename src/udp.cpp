#include "udp.h"

#pragma comment(lib, "Ws2_32.lib")

namespace udp {

    Socket::Socket(const std::string& ipAddr, uint16_t port, bool reciever) {
        sIPAddr = ipAddr;
        unPort = port;
        bReceiver = reciever;

        memset(&stSenderAddress, 0, sizeof(sockaddr_in));
        memset(&stRecieverAddress, 0, sizeof(sockaddr_in));

        stRecieverAddress.sin_family = AF_INET;
        stRecieverAddress.sin_port = htons(port);
        stRecieverAddress.sin_addr.s_addr = inet_addr(ipAddr.c_str());
    }

    bool StartSocket(Socket& stSocket) {
        WSADATA wsaData;

        int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed: %d\n", iResult);
            return 1;
        }
           
        int nFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if(nFileDescriptor < 0) return false;
        else stSocket.nFileDescriptor = nFileDescriptor;

        if(stSocket.bReceiver) {
            int nResult = bind(stSocket.nFileDescriptor, (const sockaddr*) &stSocket.stRecieverAddress, sizeof(sockaddr));
            if(nResult < 0) return false;
        }

        return true;
    }

    void CloseSocket(Socket& stSocket) {
        closesocket(stSocket.nFileDescriptor);
    }

    bool Send(Socket& stSocket, std::vector<unsigned char>& sMessage) {
        int nResult = sendto(stSocket.nFileDescriptor,
               (const char*) sMessage.data(),
               sMessage.size(),
               0,
               (const sockaddr *) &stSocket.stRecieverAddress,
               sizeof(stSocket.stRecieverAddress));
        return nResult >= 0 ? true : false;
    }

    void Receive(Socket& stSocket, std::vector<unsigned char>& sMessage, uint64_t unBufferSize) {
        unsigned char* pBuffer = new unsigned char[unBufferSize];
        int siAddrLen = sizeof(stSocket.stSenderAddress);
        
        int nMessageLength = recvfrom(stSocket.nFileDescriptor, 
                (char*) pBuffer, 
                unBufferSize, 
                0, 
                (sockaddr *) &stSocket.stSenderAddress, 
                &siAddrLen);

        sMessage = std::vector<unsigned char>(pBuffer, pBuffer + nMessageLength);
        delete [] pBuffer;
    }

};