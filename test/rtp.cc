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