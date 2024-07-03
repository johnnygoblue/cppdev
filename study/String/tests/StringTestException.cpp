#include <gtest/gtest.h>
#include "MockAllocator.h"
#include "String.h"

//// Test fixture for String tests
//class StringTestException : public ::testing::Test {
//protected:
//    void SetUp() override {
//        String<>::set_messages_wanted(true);  // Disable messages for cleaner test output
//    }
//};
//TEST_F(StringTestException, MockAllocatorBehavior) {
//    MockAllocator mockAllocator;
//
//    EXPECT_CALL(mockAllocator, allocate(::testing::_))
//        .WillOnce(::testing::Return(reinterpret_cast<char*>(0x1)));
//    EXPECT_CALL(mockAllocator, deallocate(reinterpret_cast<char*>(0x1), ::testing::_))
//        .Times(1);
//    String<MockAllocator> str("Test", mockAllocator);
//    str.clear();
//}
//
//TEST_F(StringTestException, ConstructorThrowsOnAllocationFailure) {
//    MockAllocator mockAllocator;
//
//	// Set up the mock to throw an exception when allocate is called
//    EXPECT_CALL(mockAllocator, allocate(::testing::_))
//        .WillOnce(::testing::Throw(std::bad_alloc()));
//
//    // Ensure no other calls to allocate return nullptr unexpectedly
//    EXPECT_CALL(mockAllocator, allocate(::testing::_))
//        .WillRepeatedly(::testing::Return(reinterpret_cast<char*>(0x1)));
//
//    EXPECT_CALL(mockAllocator, deallocate(::testing::_, ::testing::_))
//        .Times(::testing::AnyNumber());
//
//	// Test the string constructor with the mock allocator
//    EXPECT_THROW(String<MockAllocator> str("This will fail", mockAllocator), std::bad_alloc);
//}
//
//int main(int argc, char **argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}
