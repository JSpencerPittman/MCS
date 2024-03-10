#include "stream_protocol.h"

namespace streamprotocol {

    ImageReconstructor::ImageReconstructor(): m_bFirst(true), m_unReceivedCnt(0) {}

    void ImageReconstructor::Submit(const ImagePacket& pPacket) {
        if(m_bFirst) {
            m_bFirst = false;
            m_unNumOfPacketsPerImage = pPacket.unPacketsPerImage;

            m_pReceived = new bool[m_unNumOfPacketsPerImage];
            std::fill(m_pReceived, m_pReceived+m_unNumOfPacketsPerImage, false);

            m_lPayloads.resize(m_unNumOfPacketsPerImage);
        }

        uint16_t unPacketNum = pPacket.unPacketNum;
        if(m_pReceived[unPacketNum]) {
            std::copy(pPacket.vPayload.begin(), pPacket.vPayload.end(), m_lPayloads[unPacketNum].begin());
        } else {
            m_lPayloads[unPacketNum] = pPacket.vPayload;
            m_pReceived[unPacketNum] = true;
            ++m_unReceivedCnt;
        }
    }

    bool ImageReconstructor::Ready() const {
        return m_unReceivedCnt == m_unNumOfPacketsPerImage;
    }

    bool ImageReconstructor::Reconstruct(cv::Mat& cvReconsructImg) {
        if(!Ready()) return false;

        std::vector<unsigned char> vJoined;

        for(std::vector<unsigned char>& vPayload : m_lPayloads) 
            vJoined.insert(vJoined.end(), vPayload.begin(), vPayload.end());

        cv::imdecode(cv::Mat(vJoined), cv::IMREAD_COLOR, &cvReconsructImg);
        return true;
    }
    
    void PackifyImage(const cv::Mat& cvImage, std::vector<ImagePacket>& vImagePacketArray) {
        // Compress image.
        std::vector<int> vParans = {cv::IMWRITE_JPEG_QUALITY, 90}; 
        std::vector<uchar> vBuffer;
        cv::imencode(".jpg", cvImage, vBuffer, vParans);

        // Split the image into separate chunks
        uint8_t unNumOfChunks = ceil((double)vBuffer.size() / (double)UDP_PAYLOAD_LIMIT);

        vImagePacketArray.assign(unNumOfChunks, ImagePacket());

        for(uint32_t unIdx = 0; unIdx < unNumOfChunks; ++unIdx) {
            // Find range of data belonging to this chunk.
            uint32_t unStartIdx = unIdx * UDP_PAYLOAD_LIMIT;
            uint32_t unEndIdx = std::min(unStartIdx + UDP_PAYLOAD_LIMIT, (unsigned int)vBuffer.size());

            // Determine size of this chunks payload.
            uint32_t unChunkSize = unEndIdx - unStartIdx;

            // Construct packet.
            vImagePacketArray[unIdx].unPacketNum = unIdx;
            vImagePacketArray[unIdx].unPacketsPerImage = unNumOfChunks;
            vImagePacketArray[unIdx].vPayload.assign(unChunkSize, 0);
            vImagePacketArray[unIdx].unPayloadSize = unChunkSize;

            std::copy(vBuffer.begin() + unStartIdx, vBuffer.begin() + unEndIdx, vImagePacketArray[unIdx].vPayload.begin());
        }
    }

    void EncodeImagePacket(ImagePacket& stImagePacket, std::vector<unsigned char>& vEncodedImage) {
        vEncodedImage.assign(40+stImagePacket.unPayloadSize, 0);
        
        std::vector<std::string> vHeader = {
            "STREAM",
            std::to_string(stImagePacket.unPacketNum),
            std::to_string(stImagePacket.unPacketsPerImage),
            std::to_string(stImagePacket.unPayloadSize)
        };

        for(size_t siIdx = 0; siIdx < vHeader.size(); ++siIdx) {
            copy(vHeader[siIdx].begin(), vHeader[siIdx].end(), vEncodedImage.begin() + 10 * siIdx);
        }

        copy(stImagePacket.vPayload.begin(), stImagePacket.vPayload.end(), vEncodedImage.begin() + 40);
    }

    bool DecodeImagePacket(std::vector<unsigned char>& vEncodedImage, ImagePacket& pImagePacket) {
        if(vEncodedImage.size() <= 40) return false;
        std::string sIdentifier(vEncodedImage.begin(), vEncodedImage.begin()+10);
        if(sIdentifier.find_first_of("STREAM") == std::string::npos) return false;
        std::string sPacketNum(vEncodedImage.begin()+10, vEncodedImage.begin()+20);
        std::string sPacketsPerImage(vEncodedImage.begin()+20, vEncodedImage.begin()+30);
        std::string sPayloadSize(vEncodedImage.begin()+30, vEncodedImage.begin()+40);

        std::vector<unsigned char> vPayload(vEncodedImage.begin()+80, vEncodedImage.end());

        pImagePacket.unPacketNum = (uint16_t) std::stoi(sPacketNum);
        pImagePacket.unPacketsPerImage = (uint16_t) std::stoi(sPacketsPerImage);
        pImagePacket.unPayloadSize = (uint16_t) std::stoi(sPayloadSize);
        
        pImagePacket.vPayload = std::vector<unsigned char>(vEncodedImage.begin()+40, vEncodedImage.end());
        return true;
    }

};