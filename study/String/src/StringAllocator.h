#ifndef STRINGALLOCATOR_H
#define STRINGALLOCATOR_H

// Define an abstract base class so that we can force exception during memory allocation and
// test whether appropriate exceptions are thrown when such exceptions do happen.
class StringAllocator {
public:
	// ensure destructor of derived class is called when an object is deleted through a pointer
	// to the base class.
    virtual ~StringAllocator() = default;
    virtual char* allocate(std::size_t size) = 0;
    virtual void deallocate(char* ptr) = 0;
};

class DefaultStringAllocator : public StringAllocator {
public:
    char* allocate(std::size_t size) override {
        return new char[size];
    }
    void deallocate(char* ptr) override {
        delete[] ptr;
    }
};

#endif // STRINGALLOCATOR_H
