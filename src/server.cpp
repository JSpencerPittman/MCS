#include <iostream>
#include <stream_protocol.h>

#ifdef _WIN32
#include "udp_windows.h"
typedef udp::UDPWindowSocket UDPPlatformSocket;
#else
#include "udp_unix.h"
typedef udp::UDPUnixSocket UDPPlatformSocket;
#endif

#define IP_ADDR "127.0.0.1"
#define PORT 8000

using namespace std;

int main() {
    cout << "Starting Server..." << endl;

    UDPPlatformSocket server(IP_ADDR, PORT, true);

    std::cout << "START " << server.Start() << std::endl;

    std::vector<unsigned char> buffer;
    streamprotocol::ImageReconstructor reconstruct;
    streamprotocol::ImagePacket packet;
    while (true) {
        server.Receive(buffer, 10000);
        streamprotocol::DecodeImagePacket(buffer, packet);
        reconstruct.Submit(packet);
        std::cout << "RECEIVED: " << buffer.size() << std::endl;
        std::cout << "READY: " << reconstruct.Ready() << std::endl;
        if(reconstruct.Ready()) break;
    }

	cv::Mat frame;
	bool result = reconstruct.Reconstruct(frame);
	std::cout << "RESULT: " << result << std::endl;

	cv::imwrite("output.jpg", frame);

    server.Close();
}