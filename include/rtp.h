#ifndef RTP_H
#define RTP_H

#include <cstdint>
#include <chrono>
#include <vector>

#include "util.hpp"

namespace rtp {

    const unsigned int FIXEDHEADER_SIZE = 12;
    const unsigned int BITS_PER_BYTE = 8;
    const unsigned int BYTES_PER_WORD = 4;

    typedef bool Bit;
    typedef uint8_t Byte;
    typedef uint16_t Half;
    typedef uint32_t Word;
    typedef std::vector<Bit> BitArray;
    typedef std::vector<Byte> ByteArray;
    typedef std::vector<Word> WordArray;

    class RTPPacket {
    public:
        RTPPacket();

        /* --- Generic Parameters --- */
        Byte Version() const { return m_unVersion; }
        void SetVersion(Byte unVersion) { m_unVersion = unVersion; }

        Byte PayloadType() const { return m_unPayloadType; }
        void SetPayloadType(Byte unPayloadType) { m_unPayloadType = unPayloadType; }

        bool Marker() const { return m_bMarker; }
        void Mark() { m_bMarker = true; }
        void Unmark() { m_bMarker = false; }
       
        /* --- Synchronization --- */
        Half SequenceNumber() const { return m_unSequenceNumber; }
        Word Timestamp() const { return m_unTimestamp; }
        Word SSRC() const { return m_unSSRC; }

        void SetSequenceNumber(Half unSequenceNumber) { m_unSequenceNumber = unSequenceNumber; }
        void SetTimestamp(Word unTimestamp) { m_unTimestamp = unTimestamp; }
        void SetSSRC(Word unSSRC) { m_unSSRC = unSSRC; }

        /* --- Contributors --- */
        Byte CC() const { return m_unCC; }
        WordArray Contributors() const { return m_vContributors; }
        void AddContributor(Word unContributor);
        void AddContributors(WordArray vContributors);

        /* --- Extension --- */
        Bit HasExtension() const { return m_bHasExtension; }
        Half ExtensionProfile() const { return m_unProfile;  };
        Half ExtensionWC() const { return m_unExtWordCnt;  }
        WordArray Extension() const { return m_vExtension; };

        void SetExtension(Half unProfile, const WordArray& vExtension);
        void SetExtension(const WordArray& vExtension);
        void SetProfile(Half unProfile) { m_unProfile = unProfile; };

        /* --- Padding --- */
        bool HasPadding() const { return m_bHasPadding; }
        Byte PaddingFactor() const { return m_unPaddingFactor; }

        void SetPadding(Byte unPacketSize);

        /* --- Serialization --- */
        ByteArray Serialize() const;

    private:
        /* --- Serialization --- */
        ByteArray SerializeFixedHeader() const;
        ByteArray SerializeContributions() const;
        ByteArray SerializeExtension() const;
        ByteArray GeneratePadding(Word unPacketSize) const;

        /* --- Generic Parameters --- */
        Byte m_unVersion;
        Bit m_bMarker;
        Byte m_unPayloadType;

        /* --- Synchronization --- */
        Half m_unSequenceNumber;
        Word m_unTimestamp;
        Word m_unSSRC;

        /* --- Contributors --- */
        Byte m_unCC;
        WordArray m_vContributors;

        /* --- Extension --- */
        Bit m_bHasExtension;
        Half m_unProfile;
        Half m_unExtWordCnt;
        WordArray m_vExtension;

        /* --- Padding --- */
        Bit m_bHasPadding;
        Byte m_unPaddingFactor;

        /* --- Payload --- */
        ByteArray m_vPayload;
    };


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


    struct RTPPacketST {
        RTPHeaderFixed* pHeaderFixed;
        RTPHeaderContributors* pHeaderContributors;
        RTPHeaderExtension* pHeaderExtension;
        util::ByteArray vPayload;
    };

    void SerializePacket(std::vector<unsigned char>& vSerializedPacket, const RTPPacket& stPacket);
    void DeserializePacket(RTPPacket& stPacket, const std::vector<unsigned char>& vSerializedPacket);

};

#endif