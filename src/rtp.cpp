#include "rtp.h"

namespace rtp {

    RTPPacket::RTPPacket() : 
        m_unVersion(2), m_unPayloadType(0),
        m_bMarker(false), m_unSequenceNumber(0),
        m_unTimestamp(0), m_unSSRC(0),
        m_unCC(0), m_bHasExtension(false),
        m_unProfile(0), m_unExtWordCnt(0),
        m_bHasPadding(false), m_unPaddingFactor(0) {}

    void RTPPacket::AddContributor(Word unContributor) {
        ++m_unCC;
        m_vContributors.push_back(unContributor);
    }

    void RTPPacket::AddContributors(WordArray vContributors) {
        m_unCC += vContributors.size();
        m_vContributors.insert(m_vContributors.begin(), vContributors.begin(), vContributors.end());
    }

    void RTPPacket::SetExtension(Half unProfile, const WordArray& vExtension) {
        m_bHasExtension = true;
        m_unProfile = unProfile;
        m_unExtWordCnt = vExtension.size();
        m_vExtension = vExtension;
    }

    void RTPPacket::SetExtension(const WordArray& vExtension) {
        m_bHasExtension = true;
        m_unExtWordCnt = vExtension.size();
        m_vExtension = vExtension;
    }

    void RTPPacket::SetPadding(Byte unPaddingFactor) {
        m_bHasPadding = true;
        m_unPaddingFactor = unPaddingFactor;
    }

    ByteArray RTPPacket::Serialize() const {
        ByteArray vFixedBytes = SerializeFixedHeader();
        ByteArray vContribBytes = SerializeContributions();
        ByteArray vExtensionBytes = SerializeExtension();
        
        Word unPacketSize = vFixedBytes.size() + vContribBytes.size() + vExtensionBytes.size() + m_vPayload.size();
        ByteArray vPaddingBytes = GeneratePadding(unPacketSize);
        unPacketSize += vPaddingBytes.size();

        ByteArray vSerializedBytes;
        vSerializedBytes.reserve(unPacketSize);

        vSerializedBytes.insert(vSerializedBytes.end(), vFixedBytes.begin(), vFixedBytes.end());
        vSerializedBytes.insert(vSerializedBytes.end(), vContribBytes.begin(), vContribBytes.end());
        vSerializedBytes.insert(vSerializedBytes.end(), vExtensionBytes.begin(), vExtensionBytes.end());
        vSerializedBytes.insert(vSerializedBytes.end(), m_vPayload.begin(), m_vPayload.end());
        vSerializedBytes.insert(vSerializedBytes.end(), vPaddingBytes.begin(), vPaddingBytes.end());

        return vSerializedBytes;
    }

    RTPPacket RTPPacket::Deserialize(ByteArray vSerializedBytes) {
        if (vSerializedBytes.size() < FIXEDHEADER_SIZE)
            throw std::invalid_argument("Serialized packet does not have a readable header.");

        RTPPacket pktDeserialized;

        pktDeserialized.DeserializeFixedHeader(vSerializedBytes);
        pktDeserialized.DeserializeContributions(vSerializedBytes);
        pktDeserialized.DeserializeExtension(vSerializedBytes);
     
        size_t siRemainingBytes = vSerializedBytes.size();
        siRemainingBytes -= FIXEDHEADER_SIZE + pktDeserialized.m_unCC * BYTES_PER_WORD;
        if (pktDeserialized.m_bHasExtension)
            siRemainingBytes -= (1 + pktDeserialized.m_unExtWordCnt) * BYTES_PER_WORD;

        size_t siStartByte = FIXEDHEADER_SIZE;
        siStartByte += pktDeserialized.m_unCC * BYTES_PER_WORD;
        siStartByte += pktDeserialized.m_bHasExtension ? (1 + pktDeserialized.m_unExtWordCnt) * BYTES_PER_WORD : 0;
    
        Byte unNumPadBytes = 0;
        if (pktDeserialized.m_bHasPadding)
            unNumPadBytes = vSerializedBytes[vSerializedBytes.size() - 1];

        if (vSerializedBytes.size() - siStartByte - unNumPadBytes < 0)
            throw std::invalid_argument("Serialized packet does not have a readable payload.");

        ByteArray::const_iterator itrPayStart = vSerializedBytes.cbegin() + siStartByte;
        ByteArray::const_iterator itrPayEnd = vSerializedBytes.cend() - unNumPadBytes;
        pktDeserialized.m_vPayload = ByteArray(itrPayStart, itrPayEnd);

        return pktDeserialized;
    }

    ByteArray RTPPacket::SerializeFixedHeader() const {
        ByteArray vFixedBytes;
        vFixedBytes.reserve(FIXEDHEADER_SIZE);

        BitArray vAggBits;
        vAggBits.reserve(2 * BITS_PER_BYTE);

        BitArray vVersionBits = conv::IntegerToBits(m_unVersion, 2);
        vAggBits.insert(vAggBits.end(), vVersionBits.begin(), vVersionBits.end());

        vAggBits.push_back(m_bHasExtension);
        vAggBits.push_back(m_bHasExtension);

        BitArray vCCBits = conv::IntegerToBits(m_unCC, 4);
        vAggBits.insert(vAggBits.end(), vCCBits.begin(), vCCBits.end());

        vAggBits.push_back(m_bMarker);

        BitArray vPayloadTypeBits = conv::IntegerToBits(m_unPayloadType, 7);
        vAggBits.insert(vAggBits.end(), vPayloadTypeBits.begin(), vPayloadTypeBits.end());

        ByteArray vParamBytes = conv::BitsToBytes(vAggBits);
        vFixedBytes.insert(vFixedBytes.end(), vParamBytes.begin(), vParamBytes.end());

        ByteArray vSeqNumBytes = conv::IntegerToBytes(m_unSequenceNumber, true);
        vFixedBytes.insert(vFixedBytes.end(), vSeqNumBytes.begin(), vSeqNumBytes.end());

        ByteArray vTimestampBytes = conv::IntegerToBytes(m_unTimestamp, true);
        vFixedBytes.insert(vFixedBytes.end(), vTimestampBytes.begin(), vTimestampBytes.end());

        ByteArray vSSRCBytes = conv::IntegerToBytes(m_unSSRC, true);
        vFixedBytes.insert(vFixedBytes.end(), vSSRCBytes.begin(), vSSRCBytes.end());

        return vFixedBytes;
    };

    ByteArray RTPPacket::SerializeContributions() const {
        ByteArray vContribBytes;
        vContribBytes.reserve(BYTES_PER_WORD * m_unCC);

        for (Word unContributor : m_vContributors) {
            ByteArray vCBytes = conv::IntegerToBytes(unContributor, true);
            vContribBytes.insert(vContribBytes.end(), vCBytes.begin(), vCBytes.end());
        }

        return vContribBytes;
    }

    ByteArray RTPPacket::SerializeExtension() const {
        if (!m_bHasExtension) return {};

        ByteArray vExtensionBytes;
        vExtensionBytes.reserve(BYTES_PER_WORD + m_unExtWordCnt * BYTES_PER_WORD);

        ByteArray vProfileBytes = conv::IntegerToBytes(m_unProfile, true);
        vExtensionBytes.insert(vExtensionBytes.end(), vProfileBytes.begin(), vProfileBytes.end());

        ByteArray vWordCntBytes = conv::IntegerToBytes(m_unExtWordCnt, true);
        vExtensionBytes.insert(vExtensionBytes.end(), vWordCntBytes.begin(), vWordCntBytes.end());

        for (Word unExtWord : m_vExtension) {
            ByteArray vExtWordBytes = conv::IntegerToBytes(unExtWord, true);
            vExtensionBytes.insert(vExtensionBytes.end(), vExtWordBytes.begin(), vExtWordBytes.end());
        }

        return vExtensionBytes;
    }

    ByteArray RTPPacket::GeneratePadding(Word unPacketSize) const {
        if (!m_bHasExtension) return {};
        
        Byte unNumOfBytes = m_unPaddingFactor - (unPacketSize % m_unPaddingFactor);
        if (unNumOfBytes == 0) unNumOfBytes = m_unPaddingFactor;

        ByteArray vPaddingBytes;
        vPaddingBytes.resize(unNumOfBytes, 0);
        vPaddingBytes[unNumOfBytes - 1] = unNumOfBytes;

        return vPaddingBytes;
    }

    void RTPPacket::DeserializeFixedHeader(const ByteArray& vSerializedBytes) {
        // General Parameters
        m_unVersion = (vSerializedBytes[0] & 0b11000000) >> 6;
        m_bHasPadding = vSerializedBytes[0] & 0b00100000;
        m_bHasExtension = vSerializedBytes[0] & 0b00010000;
        m_unCC = vSerializedBytes[0] & 0b00001111;
        m_bMarker = vSerializedBytes[1] & 0b10000000;
        m_unPayloadType = vSerializedBytes[1] & 0b01111111;

        // Sequence number
        ByteArray vSeqNumBytes(vSerializedBytes.begin() + 2, vSerializedBytes.begin() + 4);
        m_unSequenceNumber = conv::BytesToInteger<Half>(vSeqNumBytes);

        // Timestamp
        ByteArray vTimestampBytes(vSerializedBytes.begin() + 4, vSerializedBytes.begin() + 8);
        m_unTimestamp = conv::BytesToInteger<Word>(vTimestampBytes);

        // SSRC identifier
        ByteArray vSSRCBytes(vSerializedBytes.begin() + 8, vSerializedBytes.begin()+12);
        m_unSSRC = conv::BytesToInteger<Word>(vSSRCBytes);

        if (m_unVersion != 2)
            throw std::invalid_argument("This library supports RTP version 2 only.");
    }

    void RTPPacket::DeserializeContributions(const ByteArray& vSerializedBytes) {
        if (m_unCC == 0) return;

        if (vSerializedBytes.size() < FIXEDHEADER_SIZE + m_unCC * BYTES_PER_WORD) 
            throw std::invalid_argument("Serialized packet does not have contributors.");

        ByteArray::const_iterator itrBytes = vSerializedBytes.begin() + FIXEDHEADER_SIZE;
        for (size_t siIdx = 0; siIdx < m_unCC; ++siIdx) {
            Word unContrib = conv::BytesToInteger<Word>({ itrBytes, itrBytes + BYTES_PER_WORD });
            m_vContributors.emplace_back(unContrib);
            itrBytes += BYTES_PER_WORD;
        }
    }

    void RTPPacket::DeserializeExtension(const ByteArray& vSerializedBytes) {
        if (!m_bHasExtension) return;

        size_t siRemainingBytes = vSerializedBytes.size() - FIXEDHEADER_SIZE;
        siRemainingBytes -= m_unCC * BYTES_PER_WORD;

        if (siRemainingBytes < BYTES_PER_WORD)
            throw std::invalid_argument("Serialized packet does not have extension.");

        ByteArray::const_iterator itrByte = vSerializedBytes.begin() + FIXEDHEADER_SIZE + m_unCC * BYTES_PER_WORD;

        // Profile
        m_unProfile = conv::BytesToInteger<Half>({ itrByte, itrByte + BYTES_PER_HALF });
        itrByte += BYTES_PER_HALF;

        // Extension length
        m_unExtWordCnt = conv::BytesToInteger<Half>({ itrByte, itrByte + BYTES_PER_HALF });
        itrByte += BYTES_PER_HALF;

        // Extension words
        siRemainingBytes -= BYTES_PER_WORD;
        if (siRemainingBytes < m_unExtWordCnt * BYTES_PER_WORD)
            throw std::invalid_argument("Serialized packet does not have extension.");

        for (size_t siIdx = 0; siIdx < m_unExtWordCnt; ++siIdx) {
            Word unExtWord = conv::BytesToInteger<Word>({ itrByte, itrByte + BYTES_PER_WORD });
            m_vExtension.emplace_back(unExtWord);
            itrByte += BYTES_PER_WORD;
        }
    }
};