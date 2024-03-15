#include "udp_windows.h"

#pragma comment(lib, "Ws2_32.lib")

namespace udp {

    UDPWindowSocket::UDPWindowSocket(const std::string& ipAddr, uint16_t port, bool receiver):
        UDPSocket(ipAddr, port, receiver) {

        memset(&m_stSenderAddress, 0, sizeof(sockaddr_in));
        memset(&m_stRecieverAddress, 0, sizeof(sockaddr_in));


        m_stRecieverAddress.sin_family = AF_INET;
        m_stRecieverAddress.sin_port = htons(port);
        m_stRecieverAddress.sin_addr.s_addr = inet_addr(ipAddr.c_str());
    }

    bool UDPWindowSocket::Start() {
        WSADATA wsaData;

        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed: %d\n", iResult);
            return false;
        }

        int nFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if (nFileDescriptor < 0) return false;
        else m_nFileDescriptor = nFileDescriptor;

        if (m_bReceiver) {
            int nResult = bind(m_nFileDescriptor, (const sockaddr*)&m_stRecieverAddress, sizeof(sockaddr));
            if (nResult < 0) return false;
        }

        return true;
    }

    bool UDPWindowSocket::Close() {
        return closesocket(m_nFileDescriptor) >= 0;
    }

    bool UDPWindowSocket::Send(std::vector<unsigned char>& vMessage) {
        int nResult = sendto(m_nFileDescriptor,
            (const char*) vMessage.data(),
            vMessage.size(),
            0,
            (const sockaddr*)&m_stRecieverAddress,
            sizeof(m_stRecieverAddress));
        return nResult >= 0 ? true : false;
    }

    bool UDPWindowSocket::Receive(std::vector<unsigned char>& vMessage, uint64_t unBufferSize) {
        unsigned char* pBuffer = new unsigned char[unBufferSize];
        int nAddrLen = sizeof(m_stSenderAddress);
        int nMessageLength = recvfrom(m_nFileDescriptor,
            (char *) pBuffer,
            unBufferSize,
            0,
            (sockaddr*)&m_stSenderAddress,
            &nAddrLen);

        vMessage = std::vector<unsigned char>(pBuffer, pBuffer + nMessageLength);
        delete[] pBuffer;
        
        return nMessageLength >= 0;
    }

};