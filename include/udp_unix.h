#ifndef UDP_UNIX_H
#define UDP_UNIX_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "udp.hpp"

namespace udp {

    class UDPUnixSocket : UDPSocket<sockaddr_in> {
        public:
            UDPUnixSocket(const std::string& ipAddr, uint16_t port, bool receiver);

            bool Start() override;
            bool Close() override;

            bool Send(std::vector<unsigned char>& vMessage) override;
            bool Receive(std::vector<unsigned char>& vMessage, uint64_t unBufferSize) override;
    };

};

#endif