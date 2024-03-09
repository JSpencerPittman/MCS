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

     std::cout << "START " << udp::StartSocket(socket) << std::endl;

    std::string sSend = "Hello server!";
    udp::Send(socket, sSend);

    udp::CloseSocket(socket);
}