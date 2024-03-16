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

};