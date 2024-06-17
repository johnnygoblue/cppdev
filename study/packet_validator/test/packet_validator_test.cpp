#include <gtest/gtest.h>
#include "packet_validator.h"

class PacketValidatorTest : public ::testing::Test {
protected:
    packet_validator::config cfg{10, 2, 50};
    packet_validator validator{cfg};
};

TEST_F(PacketValidatorTest, TestOpenConnection) {
    packet_validator::time_point now = 1000000000; // 1 second

    EXPECT_TRUE(validator.handle_packet(now, "127.0.0.1:127.0.0.2:O"));
    EXPECT_FALSE(validator.handle_packet(now, "127.0.0.1:127.0.0.2:O")); // duplicate open
    EXPECT_TRUE(validator.handle_packet(now, "127.0.0.1:127.0.0.3:O"));
    EXPECT_FALSE(validator.handle_packet(now, "127.0.0.1:127.0.0.4:O")); // exceeds connection limit
}

TEST_F(PacketValidatorTest, TestDataTransfer) {
    packet_validator::time_point now = 1000000000; // 1 second

    EXPECT_TRUE(validator.handle_packet(now, "127.0.0.1:127.0.0.2:O"));
    EXPECT_TRUE(validator.handle_packet(now, "127.0.0.2:127.0.0.1:A"));
    EXPECT_TRUE(validator.handle_packet(now, "127.0.0.1:127.0.0.2:D:abcde"));
    EXPECT_FALSE(validator.handle_packet(now, "127.0.0.1:127.0.0.2:D:abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz")); // exceeds byte limit
}

TEST_F(PacketValidatorTest, TestCloseConnection) {
    packet_validator::time_point now = 1000000000; // 1 second

    EXPECT_TRUE(validator.handle_packet(now, "127.0.0.1:127.0.0.2:O"));
    EXPECT_TRUE(validator.handle_packet(now, "127.0.0.1:127.0.0.2:C"));
    EXPECT_FALSE(validator.handle_packet(now, "127.0.0.1:127.0.0.2:D:abcde")); // closed connection
}

TEST_F(PacketValidatorTest, TestTimeouts) {
    packet_validator::time_point now = 1000000000; // 1 second

    EXPECT_TRUE(validator.handle_packet(now, "127.0.0.1:127.0.0.2:O"));
    EXPECT_TRUE(validator.handle_packet(now, "127.0.0.1:127.0.0.2:D:abcde"));
    EXPECT_EQ(validator.handle_timeouts(now + 11 * 1000000000LL), 1); // timeout after 11 seconds
    EXPECT_FALSE(validator.handle_packet(now + 12 * 1000000000LL, "127.0.0.1:127.0.0.2:D:abcde")); // connection should be timed out
}
