#ifndef MOCKALLOCATOR_H
#define MOCKALLOCATOR_H

#include "StringAllocator.h"
#include <gmock/gmock.h>

class MockAllocator : public StringAllocator {
public:
    MOCK_METHOD(char*, allocate, (std::size_t size), (override));
    MOCK_METHOD(void, deallocate, (char* ptr), (override));
};

#endif // MOCKALLOCATOR_H
