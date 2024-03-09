#include <iostream>
#include "udp.h"

#define IP_ADDR "127.0.0.1"
#define PORT 8000

using namespace std;

int main() {
    cout << "Starting Client..." << endl;

    udp::Socket servSock(IP_ADDR, PORT);
    udp::Socket cliSock;

    std::cout << "START " << udp::StartSocket(cliSock) << std::endl;

    std::string sSend = "Hello server!";
    std::cout << "SENT " << udp::Send(cliSock, servSock, sSend) << std::endl;;

    udp::CloseSocket(cliSock);
}