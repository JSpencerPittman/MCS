#include <gtest/gtest.h>
#include <vector>

#include <iostream>

#include "rtp.h"

void ValidateByteArray(const rtp::ByteArray& ba1, const rtp::ByteArray& ba2) {
	ASSERT_EQ(ba1.size(), ba2.size());
	for (size_t siIdx = 0; siIdx < ba1.size(); ++siIdx) {
		EXPECT_EQ(ba1[siIdx], ba2[siIdx]);
	}
}

TEST(PacketSerialize, SerializeBasic) {
	rtp::RTPPacket packet;

	packet.SetVersion(2);
	packet.SetPayloadType(96);
	packet.Unmark();

	packet.SetSequenceNumber(7);
	packet.SetTimestamp(2);
	packet.SetSSRC(3);

	rtp::ByteArray payload{ 10,9,8,7 };
	packet.LoadPayload(payload);

	rtp::ByteArray serialized = packet.Serialize();

	rtp::ByteArray goal{ 0b10000000, 96, 0, 7,
						 0, 0, 0, 2,
						 0, 0, 0, 3,
						 10, 9, 8, 7 };

	ValidateByteArray(serialized, goal);
}

TEST(PacketSerialize, SerializeExtensive) {
	rtp::RTPPacket packet;

	packet.SetVersion(2);
	packet.SetPayloadType(96);
	packet.Mark();

	packet.SetSequenceNumber(7);
	packet.SetTimestamp(2);
	packet.SetSSRC(3);

	packet.AddContributor(17);

	packet.SetExtension(14, { 12, 13 });

	packet.SetPadding(4);

	rtp::ByteArray payload{ 10,9,8,7 };
	packet.LoadPayload(payload);

	rtp::ByteArray serialized = packet.Serialize();

	rtp::ByteArray goal{ 0b10110001, 0b11100000, 0, 7,
						 0, 0, 0, 2,
						 0, 0, 0, 3,
						 0, 0, 0, 17,
						 0, 14, 0, 2,
						 0, 0, 0, 12,
						 0, 0, 0, 13,
						 10, 9, 8, 7,
						 0, 0, 0, 4};

	ValidateByteArray(serialized, goal);
}


TEST(PacketDeserialize, Deserialize) {
	rtp::ByteArray received{ 0b10110001, 0b11100000, 0, 7,
						 0, 0, 0, 2,
						 0, 0, 0, 3,
						 0, 0, 0, 17,
						 0, 14, 0, 2,
						 0, 0, 0, 12,
						 0, 0, 0, 13,
						 10, 9, 8, 7,
						 0, 0, 0, 4 };

	rtp::RTPPacket packet = rtp::RTPPacket::Deserialize(received);

	EXPECT_EQ(packet.Version(), 2);
	EXPECT_EQ(packet.PayloadType(), 96);
	EXPECT_EQ(packet.Marker(), true);

	EXPECT_EQ(packet.SequenceNumber(), 7);
	EXPECT_EQ(packet.Timestamp(), 2);
	EXPECT_EQ(packet.SSRC(), 3);

	EXPECT_EQ(packet.CC(), 1);
	EXPECT_EQ(packet.Contributors()[0], 17);

	EXPECT_EQ(packet.HasExtension(), true);
	EXPECT_EQ(packet.ExtensionProfile(), 14);
	EXPECT_EQ(packet.ExtensionWC(), 2);
	EXPECT_EQ(packet.Extension()[0], 12);
	EXPECT_EQ(packet.Extension()[1], 13);

	rtp::ByteArray tgt{ 10,9,8,7 };
	rtp::ByteArray actual = packet.RetreivePayload();
	ValidateByteArray(tgt, actual);

	EXPECT_EQ(packet.HasPadding(), true);
}