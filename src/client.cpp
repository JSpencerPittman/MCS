#include <iostream>
#include "udp.h"

#define IP_ADDR "127.0.0.1"
#define PORT 8000

using namespace std;

int main() {
    cout << "Starting Client..." << endl;

    udp::Socket client(IP_ADDR, PORT, false);

    std::cout << "START " << udp::StartSocket(client) << std::endl;

    std::string sMessage = "Hello server!";
    std::vector<unsigned char> sSend(sMessage.begin(), sMessage.end());
    std::cout << "SENT " << udp::Send(client, sSend) << std::endl;

    udp::CloseSocket(client);
}