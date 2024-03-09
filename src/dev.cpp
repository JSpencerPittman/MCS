#include <iostream>
#include <opencv2/opencv.hpp>
#include "stream_protocol.h"

using namespace std;

int main() {
    cv::Mat sample = cv::imread("sample.jpg");

    std::vector<streamprotocol::ImagePacket> vImagePackets;
    streamprotocol::PackifyImage(sample, vImagePackets);

    std::vector<unsigned char> vEncodedImage;
    streamprotocol::EncodeImagePacket(vImagePackets[0], vEncodedImage);

    std::cout << vEncodedImage.size() << std::endl;

    streamprotocol::ImagePacket stPack;
    bool bRes = streamprotocol::DecodeImagePacket(vEncodedImage, stPack);

    cout << stPack.unPacketNum << endl;
    cout << stPack.unPacketsPerImage << endl;
    cout << stPack.unPayloadSize << endl;

    
}