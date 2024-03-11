#include <iostream>
#include <opencv2/opencv.hpp>
#include "udp.h"
#include "stream_protocol.h"

#define IP_ADDR "127.0.0.1"
#define PORT 8000

using namespace std;

int main() {
    cout << "Starting Client..." << endl;

    udp::Socket client(IP_ADDR, PORT, false);

    std::cout << "START " << udp::StartSocket(client) << std::endl;

    cv::Mat sample = cv::imread("C:/Users/Jason/Work/URT/MCS/sample.jpg");
    std::vector<streamprotocol::ImagePacket> packets;
    streamprotocol::PackifyImage(sample, packets);

    std::vector<unsigned char> buffer;
    for(streamprotocol::ImagePacket& packet : packets) {
        streamprotocol::EncodeImagePacket(packet, buffer);
        std::cout << "BUFFSIZE: " << packet.unPayloadSize << std::endl;
        std::cout << "SENT " << udp::Send(client, buffer) << std::endl;
    }

    udp::CloseSocket(client);
}