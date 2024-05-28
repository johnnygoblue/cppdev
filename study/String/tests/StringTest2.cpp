#include <gtest/gtest.h>
#include "String.h"

// Test the default constructor
TEST(StringTest, DefaultConstructor) {
    String s;
    EXPECT_STREQ(s.c_str(), "");
    EXPECT_EQ(s.size(), 0);
}

// Test the constructor with C-string argument
TEST(StringTest, CStrConstructor) {
    String s("Hello");
    EXPECT_STREQ(s.c_str(), "Hello");
    EXPECT_EQ(s.size(), 5);
}

// Test the copy constructor
TEST(StringTest, CopyConstructor) {
    String s1("Hello");
    String s2(s1);
    EXPECT_STREQ(s2.c_str(), "Hello");
    EXPECT_EQ(s2.size(), 5);
}

// Test the move constructor
TEST(StringTest, MoveConstructor) {
    String s1("Hello");
    String s2(std::move(s1));
    EXPECT_STREQ(s2.c_str(), "Hello");
    EXPECT_EQ(s2.size(), 5);
    EXPECT_STREQ(s1.c_str(), "");
}

// Test the copy assignment operator
TEST(StringTest, CopyAssignment) {
    String s1("Hello");
    String s2 = s1;
    EXPECT_STREQ(s2.c_str(), "Hello");
    EXPECT_EQ(s2.size(), 5);
}

// Test the move assignment operator
TEST(StringTest, MoveAssignment) {
    String s1("Hello");
    String s2 = std::move(s1);
    EXPECT_STREQ(s2.c_str(), "Hello");
    EXPECT_EQ(s2.size(), 5);
    EXPECT_STREQ(s1.c_str(), "");
}

// Test concatenation
TEST(StringTest, Concatenation) {
    String s1("Hello");
    s1 += " World";
    EXPECT_STREQ(s1.c_str(), "Hello World");
    EXPECT_EQ(s1.size(), 11);
}

// Test clear function
TEST(StringTest, Clear) {
    String s("Hello");
    s.clear();
    EXPECT_STREQ(s.c_str(), "");
    EXPECT_EQ(s.size(), 0);
}

// Test operator[]
TEST(StringTest, IndexOperator) {
    String s("Hello");
    EXPECT_EQ(s[1], 'e');
    s[1] = 'a';
    EXPECT_EQ(s[1], 'a');
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
