#ifndef RTP_H
#define RTP_H

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <cmath>

namespace rtp {

    const unsigned int FIXEDHEADER_SIZE = 12;
    const unsigned int BITS_PER_BYTE = 8;
    const unsigned int BYTES_PER_WORD = 4;
    const unsigned int BYTES_PER_HALF = 2;

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

        /* --- Payload --- */
        void LoadPayload(const ByteArray& vPayloadBytes) { m_vPayload = vPayloadBytes; };
        ByteArray RetreivePayload() const { return m_vPayload; };

        /* --- Padding --- */
        bool HasPadding() const { return m_bHasPadding; }
        Byte PaddingFactor() const { return m_unPaddingFactor; }

        void SetPadding(Byte unPacketSize);

        /* --- Serialization --- */
        ByteArray Serialize() const;
        static RTPPacket Deserialize(ByteArray vSerializedBytes);

    private:
        /* --- Serialization --- */
        ByteArray SerializeFixedHeader() const;
        ByteArray SerializeContributions() const;
        ByteArray SerializeExtension() const;
        ByteArray GeneratePadding(Word unPacketSize) const;

        void DeserializeFixedHeader(const ByteArray& vSerializedBytes);
        void DeserializeContributions(const ByteArray& vSerializedBytes);
        void DeserializeExtension(const ByteArray& vSerializedBytes);

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

    namespace conv {

        template<typename T>
        inline ByteArray IntegerToBytes(T tInteger, bool bBigEndian = false) {
            size_t siNumOfBytes = sizeof(tInteger);
            ByteArray vBytes;
            vBytes.reserve(siNumOfBytes);

            for (size_t siIdx = 0; siIdx < siNumOfBytes; ++siIdx) {
                vBytes.emplace_back(tInteger & 0xFF);
                tInteger = (unsigned int)tInteger >> 8;
            }

            if (bBigEndian)
                vBytes = ByteArray(vBytes.rbegin(), vBytes.rend());

            return vBytes;
        }

        template<typename T>
        inline BitArray IntegerToBits(T tInteger, size_t siFillTo = 0) {
            BitArray vBits;
            if (siFillTo) vBits.reserve(siFillTo);

            while (tInteger) {
                vBits.emplace_back(tInteger & 1);
                tInteger = tInteger >> 1;
            }

            if (vBits.size() < siFillTo)
                vBits.resize(siFillTo, false);

            vBits = BitArray(vBits.rbegin(), vBits.rend());

            return vBits;
        }


        inline ByteArray BitsToBytes(const BitArray& vBits) {
            size_t siPadBits = (8 - (vBits.size() % 8)) % 8;
            size_t siTotalBytes = (vBits.size() + siPadBits) / 8;

            ByteArray vBytes;
            vBytes.reserve(siTotalBytes);

            for (size_t siByteIdx = 0; siByteIdx < siTotalBytes; ++siByteIdx) {
                Byte unByte = 0;
                for (size_t siBitIdx = 0; siBitIdx < 8; ++siBitIdx) {
                    size_t siBitNum = (siByteIdx * BITS_PER_BYTE) + siBitIdx;

                    if (siBitNum < siPadBits) continue;
                    else siBitNum -= siPadBits;
                    
                    if (vBits[siBitNum]) unByte += std::pow(2, 7 - siBitIdx);
                    
                }
                vBytes.emplace_back(unByte);
            }

            return vBytes;
        }

        template <typename T>
        inline T BytesToInteger(ByteArray vBytes) {
            T tInteger = 0;
            size_t siNumBytes = vBytes.size();

            for (size_t siIdx = 0; siIdx < siNumBytes; ++siIdx)
                tInteger += ((T)vBytes[siIdx]) << (BITS_PER_BYTE * (siNumBytes - siIdx - 1));

            return tInteger;
        }

        inline BitArray ByteToBits(Byte unByte) {
            BitArray vBits;
            vBits.reserve(BITS_PER_BYTE);

            while (unByte) {
                vBits.emplace_back(unByte & 1);
                unByte = unByte >> 1;
            }

            if (vBits.size() < BITS_PER_BYTE)
                vBits.resize(8, 0);

            vBits = BitArray(vBits.rbegin(), vBits.rend());

            return vBits;
        }

        inline BitArray BytesToBits(const ByteArray& vBytes) {
            BitArray vBits;
            vBits.reserve(BITS_PER_BYTE * vBytes.size());

            for (const Byte& unByte : vBytes) {
                BitArray vConvBits = ByteToBits(unByte);
                vBits.insert(vBits.begin(), vConvBits.begin(), vConvBits.end());
            }

            return vBits;
        }

    };

};

#endif