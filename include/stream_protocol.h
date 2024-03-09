#ifndef STREAM_PROTOCOL_H
#define STREAM_PROTOCOL_H

#include <cstdint>

namespace streamprotocol {

    class ImageReconstructor {
    public:
        ImageReconstructor();

    private:
    };

    struct ImagePacket {
        char* aPayload;
        uint16_t unPacketNum;
        uint16_t unPacketsPerImage;
    };

    bool PackifyImage();
    bool ParseImagePacket();

};

#endif