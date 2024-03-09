#include <iostream>
#include "udp.h"

#define IP_ADDR "127.0.0.1"
#define PORT 8000

using namespace std;

int main() {
    cout << "Starting Client..." << endl;

    udp::Socket sock(IP_ADDR, PORT, true);

     std::cout << "START " << udp::StartSocket(sock) << std::endl;
     std::cout << "BIND " << udp::BindSocket(sock) << std::endl;

    std::string sMessage;
    udp::Receive(sock, sMessage, 1024);

    std::cout << "RECIEVED: " << sMessage << std::endl;

    udp::CloseSocket(sock);
}