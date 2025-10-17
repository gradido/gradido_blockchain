#include "gradido_blockchain/data/Timestamp.h"
#include "gtest/gtest.h"

using gradido::data::Timestamp;

TEST(Timestamp, toString)
{
	Timestamp ts1(1758460711, 74638000);
	EXPECT_EQ(ts1.toString(), "1758460711.074638000");
	Timestamp ts2(1758460695, 806137000);
	EXPECT_EQ(ts2.toString(), "1758460695.806137000");
}
