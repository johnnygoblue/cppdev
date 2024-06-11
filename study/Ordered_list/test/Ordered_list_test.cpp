#include <iostream>
#include <string>
#include <cmath>
#include <gtest/gtest.h>

#include "Ordered_list.h"

// Define these to test the correct setup and teardown of Ordered_list class
int g_Ordered_list_count = 0;
int g_Ordered_list_Node_count = 0;

// Custom ordering function for integers
struct IntOrder {
    bool operator()(int a, int b) const {
        return a < b;
    }
};

// Custom ordering function for strings
struct StringOrder {
    bool operator()(const std::string& a, const std::string& b) const {
        return a < b;
    }
};

// Test fixture for Ordered_list
template<typename T, typename OF>
class Ordered_listTest : public ::testing::Test {
protected:
    Ordered_list<T, OF> list;
};

using IntOrderedListTest = Ordered_listTest<int, IntOrder>;
using StringOrderedListTest = Ordered_listTest<std::string, StringOrder> ;

// Test default constructor
TEST_F(IntOrderedListTest, DefaultConstructor) {
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(g_Ordered_list_count, 1);
    EXPECT_EQ(g_Ordered_list_Node_count, 0);
}

// Test insert function
TEST_F(IntOrderedListTest, Insert) {
    EXPECT_TRUE(list.insert(3));
    EXPECT_EQ(g_Ordered_list_Node_count, 1);
    EXPECT_TRUE(list.insert(1));
    EXPECT_EQ(g_Ordered_list_Node_count, 2);
    EXPECT_TRUE(list.insert(2));
    EXPECT_EQ(g_Ordered_list_Node_count, 3);
    EXPECT_FALSE(list.insert(2)); // Duplicate insert should return false
    EXPECT_EQ(g_Ordered_list_Node_count, 3);
    EXPECT_EQ(list.size(), 3);

    auto it = list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
}

// Test copy constructor
TEST_F(IntOrderedListTest, CopyConstructor) {
    list.insert(3);
    list.insert(1);
    list.insert(2);

    Ordered_list<int, IntOrder> copy_list(list);
    EXPECT_EQ(copy_list.size(), 3);
    EXPECT_EQ(g_Ordered_list_count, 2);
    EXPECT_EQ(g_Ordered_list_Node_count, 6);

    auto it = copy_list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
}

// Test assignment operator
TEST_F(IntOrderedListTest, AssignmentOperator) {
    EXPECT_EQ(g_Ordered_list_count, 1);
    list.insert(3);
    list.insert(1);
    list.insert(2);
    Ordered_list<int, IntOrder> assign_list;
    assign_list = list;
    EXPECT_EQ(assign_list.size(), 3);
    auto it = assign_list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    // Ensure the original list is unchanged
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(g_Ordered_list_count, 2);
    EXPECT_EQ(g_Ordered_list_Node_count, 6);
}

// Test move constructor
TEST_F(IntOrderedListTest, MoveConstructor) {
    list.insert(3);
    list.insert(1);
    list.insert(2);
    EXPECT_EQ(g_Ordered_list_count, 1);
    EXPECT_EQ(g_Ordered_list_Node_count, 3);
    Ordered_list<int, IntOrder> move_list(std::move(list));
    // though move ctor technically shouldn't increment total objects allocated
    // as we will see later that if we don't increment obj count during move ctor
    // there'll be a problem in bookkeeping
    EXPECT_EQ(g_Ordered_list_count, 2); 
    EXPECT_EQ(g_Ordered_list_Node_count, 3);
    
    EXPECT_EQ(move_list.size(), 3);
    EXPECT_TRUE(list.empty()); // Original list should be empty

    auto it = move_list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
}

// Test move assignment operator
TEST_F(IntOrderedListTest, MoveAssignmentOperator) {
    list.insert(3);
    list.insert(1);
    list.insert(2);

    EXPECT_EQ(g_Ordered_list_count, 1);
    EXPECT_EQ(g_Ordered_list_Node_count, 3);
    Ordered_list<int, IntOrder> move_assign_list;
    move_assign_list = std::move(list);
    // ownership of resources are merely transferred
    EXPECT_EQ(g_Ordered_list_count, 2);
    EXPECT_EQ(g_Ordered_list_Node_count, 3);
    EXPECT_EQ(move_assign_list.size(), 3);
    EXPECT_TRUE(list.empty()); // Original list should be empty

    auto it = move_assign_list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
}

// Test find function
TEST_F(IntOrderedListTest, Find) {
    list.insert(3);
    list.insert(1);
    list.insert(2);

    auto it = list.find(2);
    EXPECT_NE(it, list.end());
    EXPECT_EQ(*it, 2);

    it = list.find(4);
    EXPECT_EQ(it, list.end());
}

// Test erase function
TEST_F(IntOrderedListTest, Erase) {
    list.insert(3);
    list.insert(1);
    list.insert(2);
    EXPECT_EQ(g_Ordered_list_Node_count, 3);

    auto it = list.find(2);
    list.erase(it);
    EXPECT_EQ(list.size(), 2);

    it = list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(g_Ordered_list_Node_count, 2);

    it = list.find(1);
    list.erase(it);
    EXPECT_EQ(list.size(), 1);
    it = list.begin();
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(g_Ordered_list_Node_count, 1);

    it = list.find(3);
    list.erase(it);
    EXPECT_EQ(list.size(), 0);
    it = list.begin();
    EXPECT_EQ(it, list.end());
    EXPECT_EQ(g_Ordered_list_Node_count, 0);
}

// Test apply_arg
TEST_F(IntOrderedListTest, ApplyArg) {
    list.insert(1);
    list.insert(2);
    list.insert(3);

    auto print_value = [](int val, int increment) {
        std::cout << (val + increment) << " ";
    };

    apply_arg(list.begin(), list.end(), print_value, 10); // Should print 11 12 13
}

// Test apply_arg_ref
TEST_F(IntOrderedListTest, ApplyArgRef) {
    list.insert(1);
    list.insert(2);
    list.insert(3);

    auto sum_values = [](int val, int& sum) {
        sum += val;
    };

    int sum = 0;
    apply_arg_ref(list.begin(), list.end(), sum_values, sum);
    EXPECT_EQ(sum, 6);
}

// Test apply_arg_ref
TEST_F(IntOrderedListTest, ApplyArgRefWithStream) {
    list.insert(1);
    list.insert(2);
    list.insert(3);
    auto output_square = [](int val, std::ostream& os) {
        os << pow(val, 2) << " ";
    };
    apply_arg_ref(list.begin(), list.end(), output_square, std::cout); // should print 1, 4, 9
    EXPECT_TRUE(true);
}

// Test apply_if
TEST_F(IntOrderedListTest, ApplyIf) {
    list.insert(1);
    list.insert(2);
    list.insert(3);

    auto is_even = [](int val) {
        if (val % 2 == 0) {
            std::cout << val << " ";
            return true;
        }
        return false;
    };

    apply_if(list.begin(), list.end(), is_even); // Should print 2
}

// Test apply_if_arg
TEST_F(IntOrderedListTest, ApplyIfArg) {
    list.insert(1);
    list.insert(2);
    list.insert(3);

    auto print_has_greater_than = [](int val, int threshold) {
        if (val > threshold) {
            std::cout << val << " ";
            return true;
        }
        return false;
    };

    apply_if_arg(list.begin(), list.end(), print_has_greater_than, 1); // Should print 2
}

// Test for string Ordered_list
TEST_F(StringOrderedListTest, StringInsertFindErase) {
    list.insert("banana");
    list.insert("apple");
    list.insert("cherry");

    EXPECT_EQ(list.size(), 3);

    auto it = list.begin();
    EXPECT_EQ(*it, "apple");
    ++it;
    EXPECT_EQ(*it, "banana");
    ++it;
    EXPECT_EQ(*it, "cherry");

    it = list.find("banana");
    EXPECT_NE(it, list.end());
    EXPECT_EQ(*it, "banana");

    list.erase(it);
    EXPECT_EQ(list.size(), 2);
    it = list.begin();
    EXPECT_EQ(*it, "apple");
    ++it;
    EXPECT_EQ(*it, "cherry");
}

//TEST_F(IntOrderedListTest, const_Iterator) {
//    auto const_it = list.cbegin();
//    EXPECT_TRUE(std::is_const_v<decltype(const_it)>);
//}
// Test for const_Itrator
//TEST_F(IntOrderedListTest, ConstList) {
//    const_list.insert(2);
//    const_list.insert(1);
//    const_list.insert(3);
//    auto const_it = const_list.begin();
//    EXPECT_TRUE(std::is_const_v<decltype(const_list)>);
//}
