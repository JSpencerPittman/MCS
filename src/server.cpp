#include <iostream>
#include "udp.h"

#define IP_ADDR "127.0.0.1"
#define PORT 8000

using namespace std;

int main() {
    cout << "Starting Client..." << endl;

    udp::Socket servSock(IP_ADDR, PORT);
    udp::Socket cliSock;

     std::cout << "START " << udp::StartSocket(servSock) << std::endl;
     std::cout << "BIND " << udp::BindSocket(servSock) << std::endl;

    std::string sMessage;
    udp::Receive(cliSock, servSock, sMessage, 1024);

    std::cout << "RECIEVED: " << sMessage << std::endl;

    udp::CloseSocket(servSock);
}