#include "stream_protocol.h"

namespace streamprotocol {

    ImageReconstructor::ImageReconstructor(): m_bFirst(true), m_unReceivedCnt(0) {}

    bool ImageReconstructor::Submit(const ImagePacket& stPacket) {
        if(m_bFirst) {
            m_bFirst = false;
            m_unNumOfPacketsPerImage = stPacket.unPacketsPerImage;

            m_pReceived = new bool[m_unNumOfPacketsPerImage];
            std::fill(m_pReceived, m_pReceived+m_unNumOfPacketsPerImage, false);

            m_vPayloads.resize(m_unNumOfPacketsPerImage);
        }

        uint16_t unPacketNum = stPacket.unPacketNum;

        if(unPacketNum >= stPacket.unPacketsPerImage)
            return false;
        if(m_pReceived[unPacketNum] && stPacket.unPayloadSize != (uint16_t) m_vPayloads[unPacketNum].size())
            return false;

        if(m_pReceived[unPacketNum]) {
            std::copy(stPacket.vPayload.begin(), stPacket.vPayload.end(), m_vPayloads[unPacketNum].begin());
        } else {
            m_vPayloads[unPacketNum] = stPacket.vPayload;
            m_pReceived[unPacketNum] = true;
            ++m_unReceivedCnt;
        }

        return true;
    }

    bool ImageReconstructor::Ready() const {
        return m_unReceivedCnt == m_unNumOfPacketsPerImage;
    }

    bool ImageReconstructor::Reconstruct(cv::Mat& cvReconsructImg) {
        if(!Ready()) return false;

        std::vector<unsigned char> vJoined;

        for(std::vector<unsigned char>& vPayload : m_vPayloads) 
            vJoined.insert(vJoined.end(), vPayload.begin(), vPayload.end());

        cv::imdecode(cv::Mat(vJoined), cv::IMREAD_COLOR, &cvReconsructImg);
        return !cvReconsructImg.empty();
    }
    
    void PackifyImage(const cv::Mat& cvImage, std::vector<ImagePacket>& vImagePacketArray) {
        // Compress image.
        std::vector<int> vParans = {cv::IMWRITE_JPEG_QUALITY, SP_JPEG_QUALITY}; 
        std::vector<uchar> vBuffer;
        cv::imencode(SP_IMAGE_FORMAT, cvImage, vBuffer, vParans);

        // Split the image into separate chunks
        uint8_t unNumOfChunks = ceil((double)vBuffer.size() / (double)SP_PAYLOAD_LIMIT);

        vImagePacketArray.assign(unNumOfChunks, ImagePacket());

        for(uint32_t unIdx = 0; unIdx < unNumOfChunks; ++unIdx) {
            // Find range of data belonging to this chunk.
            uint32_t unStartIdx = unIdx * SP_PAYLOAD_LIMIT;
            uint32_t unEndIdx = std::min(unStartIdx + SP_PAYLOAD_LIMIT, (unsigned int)vBuffer.size());

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
        vEncodedImage.assign(SP_HEADER_SIZE+stImagePacket.unPayloadSize, 0);
        
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

    bool DecodeImagePacket(std::vector<unsigned char>& vEncodedImage, ImagePacket& stImagePacket) {
        if(vEncodedImage.size() <= SP_HEADER_SIZE) return false;
        std::string sIdentifier(vEncodedImage.begin(), vEncodedImage.begin() + SP_HEADER_FIELD_SIZE);

        if(sIdentifier.find_first_of("STREAM") == std::string::npos) return false;

        std::string sPacketNum(vEncodedImage.begin() + SP_HEADER_FIELD_SIZE, vEncodedImage.begin() + 2 * SP_HEADER_FIELD_SIZE);
        std::string sPacketsPerImage(vEncodedImage.begin()+ 2 * SP_HEADER_FIELD_SIZE, vEncodedImage.begin() + 3 * SP_HEADER_FIELD_SIZE);
        std::string sPayloadSize(vEncodedImage.begin() + 3 * SP_HEADER_FIELD_SIZE, vEncodedImage.begin() + SP_HEADER_SIZE);

        stImagePacket.unPacketNum = (uint16_t) std::stoi(sPacketNum);
        stImagePacket.unPacketsPerImage = (uint16_t) std::stoi(sPacketsPerImage);
        stImagePacket.unPayloadSize = (uint16_t) std::stoi(sPayloadSize);
        
        stImagePacket.vPayload = std::vector<unsigned char>(vEncodedImage.begin()+SP_HEADER_SIZE, vEncodedImage.end());

        return true;
    }

};