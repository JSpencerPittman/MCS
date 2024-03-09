#ifndef STREAM_PROTOCOL_H
#define STREAM_PROTOCOL_H

#include <cstdint>
#include <algorithm>
#include <opencv2/opencv.hpp>

#define UDP_PAYLOAD_LIMIT 64000

namespace streamprotocol {

    class ImageReconstructor {
    public:
        ImageReconstructor();

    private:
    };

    struct ImagePacket {
        uint16_t unPacketNum;
        uint16_t unPacketsPerImage;

        std::vector<unsigned char> vPayload;
        uint16_t unPayloadSize;
    };

    void PackifyImage(const cv::Mat& cvImage, std::vector<ImagePacket>& vImagePacketArray);

    void EncodeImagePacket(ImagePacket& pImagePacket, std::vector<unsigned char>& vEncodedImage);
    bool DecodeImagePacket(std::vector<unsigned char>& vEncodedImage, ImagePacket& pImagePacket);

};

#endif