#ifndef UDP_H
#define UDP_H

#include <cstdint>
#include <vector>
#include <string>

namespace udp {

    template <typename SockAddrType>
    class UdpSocket {
        public:
            UdpSocket(const std::string& ipAddr, uint16_t port, bool reciever);
            
            virtual bool Start() = 0;
            virtual bool Close() = 0;

            virtual bool Send(std::vector<unsigned char>& sMessage) = 0;
            virtual bool Recieve(std::vector<unsigned char>& sMessage, uint64_t unBufferSize) = 0;

        protected:
            uint16_t m_unPort;
            std::string m_sIPAddr;
            bool m_bReceiver;
            int m_nFileDescriptor;

            SockAddrType m_stSenderAddress;
            SockAddrType m_stRecieverAddress;

        private:
    };

};

#endif