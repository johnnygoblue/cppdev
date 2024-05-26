#include <iostream>
#include <set>
#include <gtest/gtest.h>
#include "calc.h"

template<typename T>
bool areVectorsEqualUnordered(const vector<T>& vec1, const vector<T>& vec2) {
    std::multiset<T> multiset1(vec1.begin(), vec1.end());
    std::multiset<T> multiset2(vec2.begin(), vec2.end());
    return multiset1 == multiset2;
}

TEST(CalcTest, ExampleTest) {
    int result = exampleFunction(1, 2);
    EXPECT_EQ(result, 3);
}

TEST(CalcTest, getNextNthEntryHasKey) {
    string haystack = "Try to find the needle in a haystack";
    string needle = "needle";
    EXPECT_EQ(getNextNthEntry(haystack, needle, 1), std::optional<string>("in"));
    EXPECT_EQ(getNextNthEntry(haystack, needle, 2), std::optional<string>("a"));
    EXPECT_EQ(getNextNthEntry(haystack, needle, 3), std::optional<string>("haystack"));
    EXPECT_EQ(getNextNthEntry(haystack, needle, 4), std::nullopt);
    EXPECT_EQ(getNextNthEntry(haystack, needle, 5), std::nullopt);
    EXPECT_EQ(getNextNthEntry(haystack, "haystack", 1), std::nullopt);
}

TEST(CalcTest, getNextNthEntryNoKey) {
    string haystack = "Can we stop trying to find the non-existent ?";
    EXPECT_EQ(getNextNthEntry(haystack, "yes", 1), std::nullopt);
    EXPECT_EQ(getNextNthEntry(haystack, "no", 1), std::nullopt);
    EXPECT_EQ(getNextNthEntry(haystack, "an", 1), std::nullopt);
}

TEST(CalcTest, getNextNthEntryEdge1) {
    string edge1 = "small string";
    EXPECT_EQ(getNextNthEntry(edge1, "small", 1), std::optional("string"));
    EXPECT_EQ(getNextNthEntry(edge1, "small", 2), std::nullopt);
}

TEST(CalcTest, getNextNthEntryEdge2) {
    string edge2 = "small string";
    EXPECT_EQ(getNextNthEntry(edge2, "string", 1), std::nullopt);
}

TEST(CalcTest, genCheckPointsEven) {
    string t1 = "09:00:00";
    string t2 = "10:00:00";
    vector<string> tps = {"09:00:00", "09:10:00", "09:20:00", "09:30:00", "09:40:00", "09:50:00", "10:00:00"};
    EXPECT_TRUE(areVectorsEqualUnordered(genCheckPoints(t1, t2, 600), tps));
}

TEST(CalcTest, genCheckPointsOdd) {
    string t1 = "09:00:00";
    string t2 = "10:00:00";
    vector<string> tps = {"09:00:00", "09:11:40", "09:23:20", "09:35:00", "09:46:40", "09:58:20"};
    EXPECT_TRUE(areVectorsEqualUnordered(genCheckPoints(t1, t2, 700), tps));
}

TEST(CalcTest, genCheckPointsEdge1) {
    string t1 = "08:59:59";
    string t2 = "09:00:03";
    vector<string> tps = {"08:59:59", "09:00:03"};
    EXPECT_TRUE(areVectorsEqualUnordered(genCheckPoints(t1, t2, 4), tps));
}

TEST(CalcTest, genCheckPointsEdge2) {
    string t1 = "08:59:59";
    string t2 = "09:00:03";
    vector<string> tps = {"08:59:59"};
    EXPECT_TRUE(areVectorsEqualUnordered(genCheckPoints(t1, t2, 5), tps));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
