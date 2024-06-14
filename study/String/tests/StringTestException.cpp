#include <gtest/gtest.h>
#include "MockAllocator.h"
#include "String.h"

TEST(StringTest, ConstructorThrowsOnAllocationFailure) {
    MockAllocator mockAllocator;
	// Set up the mock to throw an exception when allocate is called
    EXPECT_CALL(mockAllocator, allocate(::testing::_)).WillOnce(::testing::Throw(std::bad_alloc()));
	// Test the string constructor with the mock allocator
    EXPECT_THROW(String str("test", &mockAllocator), String_exception);
}

TEST(StringTest, CopyConstructorThrowsOnAllocationFailure) {
    String original("test");
    MockAllocator mockAllocator;
    EXPECT_CALL(mockAllocator, allocate(::testing::_)).WillOnce(::testing::Throw(std::bad_alloc()));

    EXPECT_THROW(String copy(original, &mockAllocator), String_exception);
}

TEST(StringTest, DeallocationIsCalled) {
	char dummy[5] = "test";
	MockAllocator mockAllocator;
	EXPECT_CALL(mockAllocator, allocate(::testing::_)).WillOnce(::testing::Return(dummy));
	EXPECT_CALL(mockAllocator, deallocate(dummy)).Times(1);
	{
		String str("test", &mockAllocator);
	}
}
