#ifndef UDP_H
#define UDP_H

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include <WinSock2.h>

#include "udp.hpp"

namespace udp {

    class UDPWindowSocket : UDPSocket<sockaddr_in> {
    public:
        UDPWindowSocket(const std::string& ipAddr, uint16_t port, bool receiver);

        bool Start() override;
        bool Close() override;

        bool Send(std::vector<unsigned char>& vMessage) override;
        bool Receive(std::vector<unsigned char>& vMessage, uint64_t unBufferSize) override;
    }

};

#endif