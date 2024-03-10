#ifndef STREAM_PROTOCOL_H
#define STREAM_PROTOCOL_H

#include <cstdint>
#include <algorithm>
#include <opencv2/opencv.hpp>

#define UDP_PAYLOAD_LIMIT 64000

namespace streamprotocol {

    struct ImagePacket {
        uint16_t unPacketNum;
        uint16_t unPacketsPerImage;

        std::vector<unsigned char> vPayload;
        uint16_t unPayloadSize;
    };

    class ImageReconstructor {
        public:
            ImageReconstructor();

            void Submit(const ImagePacket& pPacket);
            bool Ready() const;
            bool Reconstruct(cv::Mat& cvReconsructImg);


        private:
            bool m_bFirst;
            uint16_t m_unNumOfPacketsPerImage;
            bool* m_pReceived;
            uint16_t m_unReceivedCnt;
            std::vector<std::vector<unsigned char>> m_lPayloads;
    };

    void PackifyImage(const cv::Mat& cvImage, std::vector<ImagePacket>& vImagePacketArray);

    void EncodeImagePacket(ImagePacket& pImagePacket, std::vector<unsigned char>& vEncodedImage);
    bool DecodeImagePacket(std::vector<unsigned char>& vEncodedImage, ImagePacket& pImagePacket);

};

#endif