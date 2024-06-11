#include <iostream>
#include <type_traits>
#include <gtest/gtest.h>

#include "Ordered_list.h"

// This test suite tests the const version of the Ordered_list

int g_Ordered_list_count = 0;
int g_Ordered_list_Node_count = 0;

struct IntOrder {
    bool operator()(int a, int b) const {
        return a < b;
    }
};

template<typename T, typename OF>
class Ordered_listTest : public ::testing::Test {
protected:
    Ordered_list<T, OF> list;
};

using IntOrderedListTest = Ordered_listTest<int, IntOrder>;

TEST_F(IntOrderedListTest, DefaultConstructor) {
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
}

TEST_F(IntOrderedListTest, InsertConstOrderedList) {
    EXPECT_TRUE(list.insert(3));
    EXPECT_TRUE(list.insert(1));
    EXPECT_TRUE(list.insert(2));
    const Ordered_list<int, IntOrder> const_list = list;
    auto const_it = const_list.begin(); // const Iterator
    // *const_it = 5;  // doesn't compile
    const_it = const_list.end();
    // std::cout << *const_it << std::endl;; // compiles but assertion failure at runtime
    const_it = const_list.cbegin();
    // *const_it = 5; // doesn't compile
    auto const_it2 = list.cbegin(); // const Iterator
    // *const_it2 = 1; // doesn't compile
    auto it = list.begin();
    *it = 1; // this line compiles
    it = list.end();
    // std::cout << *it << std::endl; // compiles but assertion failure at runtime
}
