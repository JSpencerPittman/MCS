#include <iostream>
#include <opencv2/opencv.hpp>
#include "stream_protocol.h"

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
    cout << "Starting Client..." << endl;

    UDPPlatformSocket client(IP_ADDR, PORT, false);

    std::cout << "START " << client.Start() << std::endl;

    cv::Mat sample = cv::imread("sample.jpg");
    std::vector<streamprotocol::ImagePacket> packets;
    streamprotocol::PackifyImage(sample, packets);

    std::vector<unsigned char> buffer;
    for(streamprotocol::ImagePacket& packet : packets) {
        streamprotocol::EncodeImagePacket(packet, buffer);
        std::cout << "BUFFSIZE: " << packet.unPayloadSize << std::endl;
        std::cout << "SENT " << client.Send(buffer) << std::endl;
    }

    client.Close();
}