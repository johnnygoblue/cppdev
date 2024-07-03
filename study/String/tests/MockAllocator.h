#ifndef MOCKALLOCATOR_H
#define MOCKALLOCATOR_H

#include "StringAllocator.h"
#include <gmock/gmock.h>

class MockAllocator : public MyAllocator {
public:
    MockAllocator() = default;
    MOCK_METHOD(char*, allocate, (std::size_t size), (const override));
    MOCK_METHOD(void, deallocate, (char* ptr, std::size_t size), (const override));
    MockAllocator(const MockAllocator&) = default; // just so String's ctor works
    MockAllocator& operator=(const MockAllocator&) = delete;
    ~MockAllocator() = default;
};

#endif // MOCKALLOCATOR_H
