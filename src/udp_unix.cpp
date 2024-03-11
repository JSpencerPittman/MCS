#include "udp_unix.h"

namespace udp {

    UDPUnixSocket::UDPUnixSocket(const std::string& ipAddr, uint16_t port, bool receiver):
        UDPSocket(ipAddr, port, receiver) {

        memset(&m_stSenderAddress, 0, sizeof(sockaddr_in));
        memset(&m_stRecieverAddress, 0, sizeof(sockaddr_in));


        m_stRecieverAddress.sin_family = AF_INET;
        m_stRecieverAddress.sin_port = htons(port);
        m_stRecieverAddress.sin_addr.s_addr = inet_addr(ipAddr.c_str());
    }

    bool UDPUnixSocket::Start() {
        int nFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        if(nFileDescriptor < 0) return false;
        else m_nFileDescriptor = nFileDescriptor;

        if(m_bReceiver) {
            int nResult = bind(m_nFileDescriptor, (const sockaddr*) &m_stRecieverAddress, sizeof(sockaddr));
            if(nResult < 0) return false;
        }

        return true;
    }

    bool UDPUnixSocket::Close() {
        return close(m_nFileDescriptor) >= 0;
    }

    bool UDPUnixSocket::Send(std::vector<unsigned char>& vMessage) {
         int nResult = sendto(m_nFileDescriptor,
               vMessage.data(),
               vMessage.size(),
               0,
               (const sockaddr *) &m_stRecieverAddress,
               sizeof(m_stRecieverAddress));
        return nResult >= 0 ? true : false;
    }

    bool UDPUnixSocket::Receive(std::vector<unsigned char>& vMessage, uint64_t unBufferSize) {
        unsigned char* pBuffer = new unsigned char[unBufferSize];
        socklen_t siAddrLen = sizeof(m_stSenderAddress);
        int nMessageLength = recvfrom(m_nFileDescriptor, 
                pBuffer, 
                unBufferSize, 
                MSG_WAITALL, 
                (sockaddr *) &m_stSenderAddress, 
                &siAddrLen);
        vMessage = std::vector<unsigned char>(pBuffer, pBuffer + nMessageLength);
        return nMessageLength >= 0;
    }
};