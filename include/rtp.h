#ifndef RTP_H
#define RTP_H

#include <cstdint>
#include <chrono>
#include <vector>

#include "util.hpp"

namespace rtp {

    struct RTPHeaderFixed {
        uint8_t unVersion;
        bool bHasPadding;
        uint8_t unPaddingFactor;
        bool bHasExtension;
        uint8_t unCC;
        bool bMarker;
        uint8_t unPayloadType;
        uint16_t unSequenceNumber;
        uint32_t unTimestamp;
        uint32_t unSSRC;
    };

    struct RTPHeaderContributors {
        uint8_t unCC;
        uint32_t* pCSRC;
    };

    struct RTPHeaderExtension {
        uint16_t unProfile;
        uint16_t unWordCount;
        uint32_t* pExtension;
    };


    struct RTPPacket {
        RTPHeaderFixed* pHeaderFixed;
        RTPHeaderContributors* pHeaderContributors;
        RTPHeaderExtension* pHeaderExtension;
        util::ByteArray vPayload;
    };

    void SerializePacket(std::vector<unsigned char>& vSerializedPacket, const RTPPacket& stPacket);
    void DeserializePacket(RTPPacket& stPacket, const std::vector<unsigned char>& vSerializedPacket);

};

#endif