#include "String.h"
#include <gtest/gtest.h>

// Test fixture for String tests
class StringTest : public ::testing::Test {
protected:
    void SetUp() override {
        String<>::set_messages_wanted(true);  // Disable messages for cleaner test output
    }
};

// Test default constructor
TEST_F(StringTest, DefaultConstructor) {
    String s;
    EXPECT_STREQ(s.c_str(), "");
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.get_allocation(), 1);  // Null terminator
}

// Test default constructor with std::allocator
TEST_F(StringTest, DefaultConstructorWithStdAlloc) {
    std::allocator<char> stdAllocator;
    String<std::allocator<char>> s(stdAllocator);
    EXPECT_STREQ(s.c_str(), "");
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.get_allocation(), 1);  // Only null terminator
}

// Test constructor with C-string
TEST_F(StringTest, CStringConstructor) {
    const char* test_str = "Hello";
    String s(test_str);
    EXPECT_STREQ(s.c_str(), test_str);
    EXPECT_EQ(s.size(), 5);
    EXPECT_EQ(s.get_allocation(), 6);  // Length + null terminator
}

// Test constructor with empty C-string
TEST_F(StringTest, EmptyCStringConstructor) {
    const char* test_str = "";
    String s(test_str);
    EXPECT_STREQ(s.c_str(), test_str);
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.get_allocation(), 1);  // Length + null terminator
}

// Test copy constructor
TEST_F(StringTest, CopyConstructor) {
    String s1("Hello");
    String s2(s1);
    EXPECT_STREQ(s2.c_str(), "Hello");
    EXPECT_EQ(s2.size(), 5);
    EXPECT_EQ(s2.get_allocation(), 6);  // Length + null terminator
}

// Test copy constructor from empty
TEST_F(StringTest, CopyConstructorFromEmpty) {
    String s1("");
    String s2(s1);
    EXPECT_STREQ(s2.c_str(), "");
    EXPECT_EQ(s2.size(), 0);
    EXPECT_EQ(s2.get_allocation(), 1);  // Length + null terminator
}

// Test move constructor
TEST_F(StringTest, MoveConstructor) {
    String s1("Hello");
    String s2(std::move(s1));
    EXPECT_STREQ(s2.c_str(), "Hello");
    EXPECT_EQ(s2.size(), 5);
    EXPECT_EQ(s2.get_allocation(), 6);  // Length + null terminator
    EXPECT_EQ(s1.size(), 0);
    EXPECT_STREQ(s1.c_str(), "");  // s1 should be empty after move
}

// Test assignment operator with String
TEST_F(StringTest, CopyAssignment) {
    String s1("Hello");
    String s2;
    s2 = s1;
    EXPECT_STREQ(s2.c_str(), "Hello");
    EXPECT_EQ(s2.size(), 5);
    EXPECT_EQ(s2.get_allocation(), 6);  // Length + null terminator
}

// Test assignment operator with C-string
TEST_F(StringTest, CStringAssignment) {
    String s;
    s = "Hello";
    EXPECT_STREQ(s.c_str(), "Hello");
    EXPECT_EQ(s.size(), 5);
    EXPECT_EQ(s.get_allocation(), 6);  // Length + null terminator
}

// Test move assignment
TEST_F(StringTest, MoveAssignment) {
    String s1("Hello");
    String s2;
    s2 = std::move(s1);
    EXPECT_STREQ(s2.c_str(), "Hello");
    EXPECT_EQ(s2.size(), 5);
    EXPECT_EQ(s2.get_allocation(), 6);  // Length + null terminator
    EXPECT_STREQ(s1.c_str(), "");  // s1 should be empty after move
}

// Test operator[]
TEST_F(StringTest, OperatorSquareBrackets) {
    String s("Hello");
    EXPECT_EQ(s[0], 'H');
    EXPECT_EQ(s[1], 'e');
    EXPECT_EQ(s[2], 'l');
    EXPECT_EQ(s[3], 'l');
    EXPECT_EQ(s[4], 'o');
    EXPECT_THROW(s[5], std::exception);  // Out of bounds
}

// Test clear
TEST_F(StringTest, Clear) {
    String s("Hello");
    s.clear();
    EXPECT_STREQ(s.c_str(), "");
    EXPECT_EQ(s.size(), 0);
    EXPECT_EQ(s.get_allocation(), 1);  // Null terminator
}

// Test concatenation with char
TEST_F(StringTest, ConcatenateChar) {
    String s("Hello");
    s += '!';
    EXPECT_STREQ(s.c_str(), "Hello!");
    EXPECT_EQ(s.size(), 6);
    EXPECT_EQ(s.get_allocation(), 14);  // Doubling rule applied
}

// Test concatenation with C-string
TEST_F(StringTest, ConcatenateCString) {
    String s("Hello");
    s += " World";
    EXPECT_STREQ(s.c_str(), "Hello World");
    EXPECT_EQ(s.size(), 11);
    EXPECT_EQ(s.get_allocation(), 24);  // Doubling rule applied
}

// Test concatenation with String
TEST_F(StringTest, ConcatenateString) {
    String s1("Hello");
    String s2(" World");
    s1 += s2;
    EXPECT_STREQ(s1.c_str(), "Hello World");
    EXPECT_EQ(s1.size(), 11);
    EXPECT_EQ(s1.get_allocation(), 24);  // Doubling rule applied
}

// Test swap
TEST_F(StringTest, Swap) {
    String s1("Hello");
    String s2("World");
    s1.swap(s2);
    EXPECT_STREQ(s1.c_str(), "World");
    EXPECT_STREQ(s2.c_str(), "Hello");
}

// Test comparison operators
TEST_F(StringTest, ComparisonOperators) {
    String s1("Hello");
    String s2("World");
    String s3("Hello");
    EXPECT_TRUE(s1 == s3);
    EXPECT_FALSE(s1 == s2);
    EXPECT_TRUE(s1 != s2);
    EXPECT_FALSE(s1 != s3);
    EXPECT_TRUE(s1 < s2);
    EXPECT_FALSE(s2 < s1);
    EXPECT_TRUE(s2 > s1);
    EXPECT_FALSE(s1 > s2);
}

// Test concatenation operator+
TEST_F(StringTest, ConcatenateOperatorPlus) {
    String s1("Hello");
    String s2(" World");
    String s3 = s1 + s2;
    EXPECT_STREQ(s3.c_str(), "Hello World");
    EXPECT_EQ(s3.size(), 11);
}

// Test stream input
TEST_F(StringTest, StreamInput) {
    String s;
    std::istringstream iss("Hello World");
    iss >> s;
    EXPECT_STREQ(s.c_str(), "Hello");
    EXPECT_EQ(s.size(), 5);
}

// Test stream output
TEST_F(StringTest, StreamOutput) {
    String s("Hello");
    std::ostringstream oss;
    oss << s;
    EXPECT_EQ(oss.str(), "Hello");
}

// Run all the tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
