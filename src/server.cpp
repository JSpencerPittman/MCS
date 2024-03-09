#include <iostream>
#include "udp.h"

#define IP_ADDR "127.0.0.1"
#define PORT 8000

using namespace std;

int main() {
    cout << "Starting Client" << endl;

    udp::Socket socket;
    socket.sIPAddr = IP_ADDR;
    socket.unPort = PORT;

    udp::Socket cli;
    memset(&cli.stSocketAddress, 0, sizeof(cli.stSocketAddress));

    std::cout << "START " << udp::StartSocket(socket) << std::endl;
    std::cout << "BIND " << udp::BindSocket(socket) << std::endl;

    std::string sReceived;
    udp::Receive(cli, socket, sReceived, 1024);

    std::cout << sReceived << std::endl;

    udp::CloseSocket(socket);
}