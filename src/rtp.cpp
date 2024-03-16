#include "rtp.h"

#define RTP_FIXEDHEADER_SIZE 12
#define BYTE_SIZE 8

namespace rtp {

    void SerializePacket(std::vector<unsigned char>& vSerializedPacket, const RTPPacket& stPacket) {
        unsigned int unPacketSize = 0;

        /* --- Fixed Header --- */
        RTPHeaderFixed* pFixed = stPacket.pHeaderFixed;

        if(pFixed == nullptr)
            throw std::invalid_argument("RTP packet has no fixed header.");

        util::ByteArray byteFixed;
        byteFixed.reserve(RTP_FIXEDHEADER_SIZE);
        
        util::BinArray binAgg;
        binAgg.reserve(2 * BYTE_SIZE);

        util::BinArray binVersion = util::Binarize(pFixed->unVersion, 2);
        std::copy(binVersion.begin(), binVersion.end(), binAgg.end());

        binAgg.push_back(pFixed->bHasPadding);
        binAgg.push_back(pFixed->bHasExtension);

        util::BinArray binCC = util::Binarize(pFixed->unCC, 4);
        std::copy(binCC.begin(), binCC.end(), binAgg.end());

        binAgg.push_back(pFixed->bMarker);

        util::BinArray binPayloadType = util::Binarize(pFixed->unPayloadType, 7);
        std::copy(binPayloadType.begin(), binPayloadType.end(), binAgg.end());

        util::ByteArray byteParams = util::Bytify(binAgg);
        std::copy(binAgg.begin(), binAgg.end(), byteFixed.begin());

        util::ByteArray byteTimestamp = util::SerializeInteger(pFixed->unTimestamp, true);
        std::copy(byteTimestamp.begin(), byteTimestamp.end(), byteFixed.begin());

        util::ByteArray byteSSRC = util::SerializeInteger(pFixed->unCC, true);
        std::copy(byteSSRC.begin(), byteSSRC.end(), byteFixed.begin());

        unPacketSize += RTP_FIXEDHEADER_SIZE;

        /* --- Contributors --- */
        RTPHeaderContributors* pContrib = stPacket.pHeaderContributors;

        bool bContrib = pContrib != nullptr && pContrib->unCC;
        util::ByteArray byteContribs;

        if(bContrib) {
            if(pContrib == nullptr)
                throw std::invalid_argument("Contribution was enabled, but no contributors provided.");

            byteContribs.reserve(4 * pContrib->unCC);
            
            for(unsigned int unIdx = 0; unIdx < pContrib->unCC; ++unIdx) {
                util::ByteArray byteContrib = util::SerializeInteger(pContrib->pCSRC[unIdx], true);
                std::copy(byteContrib.begin(), byteContrib.end(), byteContribs.end());
            }
        }

        unPacketSize += byteContribs.size();

        /* --- Extension -- */
        RTPHeaderExtension* pExtension = stPacket.pHeaderExtension;

        bool bExtension = pFixed->bHasExtension;
        util::ByteArray byteExtension;

        if(bExtension) {
            if(pExtension == nullptr)
                throw std::invalid_argument("Extension was enabled, but no extension provided.");
            
            byteExtension.reserve(4 + 4 * pExtension->unWordCount);

            util::ByteArray byteProfile = util::SerializeInteger(pExtension->unProfile, true);
            std::copy(byteProfile.begin(), byteProfile.end(), byteExtension.end());

            util::ByteArray byteExtLength = util::SerializeInteger(pExtension->unWordCount, true);
            std::copy(byteExtLength.begin(), byteExtLength.end(), byteExtension.end());
            
            for(unsigned int unIdx = 0; unIdx < pExtension->unWordCount; ++unIdx) {
                util::ByteArray byteExtWord = util::SerializeInteger(pExtension->pExtension[unIdx], true);
                std::copy(byteExtWord.begin(), byteExtWord.end(), byteExtension.end());
            }
        }

        unPacketSize += byteExtension.size();

        /* --- Payload --- */
        unPacketSize += stPacket.vPayload.size();

        /* -- Padding --- */
        bool bPadding = stPacket.pHeaderFixed->bHasPadding;
        util::ByteArray bytePadding;

        if(bPadding) {
            uint8_t unPaddingFactor = stPacket.pHeaderFixed->unPaddingFactor;
            uint8_t unPadSize = std::max((uint8_t) (unPaddingFactor - (unPacketSize % unPaddingFactor)), unPaddingFactor);

            bytePadding.resize(unPadSize, 0);
            bytePadding[unPadSize-1] = unPadSize;
        }

        unPacketSize += bytePadding.size();

        /* -- Aggregate Packet & Payload --- */
        vSerializedPacket.resize(unPacketSize);

        std::copy(byteFixed.begin(), byteFixed.end(), vSerializedPacket.end());

        if(bContrib)
            std::copy(byteContribs.begin(), byteContribs.end(), vSerializedPacket.end());

        if(bExtension)
            std::copy(byteExtension.begin(), byteExtension.end(), vSerializedPacket.end());

        std::copy(stPacket.vPayload.begin(), stPacket.vPayload.end(), vSerializedPacket.end());
        
        if(bPadding)
            std::copy(bytePadding.begin(), bytePadding.end(), vSerializedPacket.end());
    }

};