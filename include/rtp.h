#ifndef RTP_H
#define RTP_H

#include <cstdint>
#include <chrono>

namespace rtp {

    typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> RTPTimestamp;

    struct RTPHeader {
        uint8_t unVersion;
        bool bHasPadding;
        bool bHasExtension;
        uint8_t unCC;
        bool bMarket;
        uint8_t unPayloadType;
        uint16_t unSequenceNumber;
        RTPTimestamp tmTimestamp;
        uint32_t unSSRC;
    };

};

#endif