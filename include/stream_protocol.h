#ifndef STREAM_PROTOCOL_H
#define STREAM_PROTOCOL_H

#include <cstdint>
#include <algorithm>
#include <opencv2/opencv.hpp>

#define SP_PAYLOAD_LIMIT 8000
#define SP_JPEG_QUALITY 90
#define SP_HEADER_SIZE 40
#define SP_HEADER_FIELD_SIZE 10
#define SP_IMAGE_FORMAT ".jpg"

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

            bool Submit(const ImagePacket& stPacket);
            bool Ready() const;
            bool Reconstruct(cv::Mat& cvReconsructImg);


        private:
            bool m_bFirst;
            uint16_t m_unNumOfPacketsPerImage;
            bool* m_pReceived;
            uint16_t m_unReceivedCnt;
            std::vector<std::vector<unsigned char>> m_vPayloads;
    };

    void PackifyImage(const cv::Mat& cvImage, std::vector<ImagePacket>& vImagePacketArray);

    void EncodeImagePacket(ImagePacket& stImagePacket, std::vector<unsigned char>& vEncodedImage);
    bool DecodeImagePacket(std::vector<unsigned char>& vEncodedImage, ImagePacket& stImagePacket);

};

#endif