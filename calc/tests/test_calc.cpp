#include <iostream>
#include <set>
#include <gtest/gtest.h>
#include "calc.h"

template<typename T>
bool areVectorsEqualUnordered(const std::vector<T>& vec1, const std::vector<T>& vec2) {
    std::multiset<T> multiset1(vec1.begin(), vec1.end());
    std::multiset<T> multiset2(vec2.begin(), vec2.end());
    return multiset1 == multiset2;
}

TEST(CalcTest, getNextNthEntryHasKey) {
    std::string haystack = "Try to find the needle in a haystack";
    std::string needle = "needle";
    EXPECT_EQ(getNextNthEntry(haystack, needle, 1), std::optional<std::string>("in"));
    EXPECT_EQ(getNextNthEntry(haystack, needle, 2), std::optional<std::string>("a"));
    EXPECT_EQ(getNextNthEntry(haystack, needle, 3), std::optional<std::string>("haystack"));
    EXPECT_EQ(getNextNthEntry(haystack, needle, 4), std::nullopt);
    EXPECT_EQ(getNextNthEntry(haystack, needle, 5), std::nullopt);
    EXPECT_EQ(getNextNthEntry(haystack, "haystack", 1), std::nullopt);
}

TEST(CalcTest, getNextNthEntryNoKey) {
    std::string haystack = "Can we stop trying to find the non-existent ?";
    EXPECT_EQ(getNextNthEntry(haystack, "yes", 1), std::nullopt);
    EXPECT_EQ(getNextNthEntry(haystack, "no", 1), std::nullopt);
    EXPECT_EQ(getNextNthEntry(haystack, "an", 1), std::nullopt);
}

TEST(CalcTest, getNextNthEntryEdge1) {
    std::string edge1 = "small std::string";
    EXPECT_EQ(getNextNthEntry(edge1, "small", 1), std::optional("std::string"));
    EXPECT_EQ(getNextNthEntry(edge1, "small", 2), std::nullopt);
}

TEST(CalcTest, getNextNthEntryEdge2) {
    std::string edge2 = "small std::string";
    EXPECT_EQ(getNextNthEntry(edge2, "std::string", 1), std::nullopt);
}

TEST(CalcTest, genCheckPointsEven) {
    std::string t1 = "09:00:00";
    std::string t2 = "10:00:00";
    std::vector<std::string> tps = {"09:00:00", "09:10:00", "09:20:00", "09:30:00", "09:40:00", "09:50:00", "10:00:00"};
    EXPECT_TRUE(areVectorsEqualUnordered(genCheckPoints(t1, t2, 600), tps));
}

TEST(CalcTest, genCheckPointsOdd) {
    std::string t1 = "09:00:00";
    std::string t2 = "10:00:00";
    std::vector<std::string> tps = {"09:00:00", "09:11:40", "09:23:20", "09:35:00", "09:46:40", "09:58:20"};
    EXPECT_TRUE(areVectorsEqualUnordered(genCheckPoints(t1, t2, 700), tps));
}

TEST(CalcTest, genCheckPointsEdge1) {
    std::string t1 = "08:59:59";
    std::string t2 = "09:00:03";
    std::vector<std::string> tps = {"08:59:59", "09:00:03"};
    EXPECT_TRUE(areVectorsEqualUnordered(genCheckPoints(t1, t2, 4), tps));
}

TEST(CalcTest, genCheckPointsEdge2) {
    std::string t1 = "08:59:59";
    std::string t2 = "09:00:03";
    std::vector<std::string> tps = {"08:59:59"};
    EXPECT_TRUE(areVectorsEqualUnordered(genCheckPoints(t1, t2, 5), tps));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
