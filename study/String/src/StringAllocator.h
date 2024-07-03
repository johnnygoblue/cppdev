#ifndef STRINGALLOCATOR_H
#define STRINGALLOCATOR_H

// Define an abstract base class so that we can force exception during memory allocation and
// test whether appropriate exceptions are thrown when such exceptions do happen.
class MyAllocator {
public:
    using value_type = char;  // fixed type, no need to templatize it
	// ensure destructor of derived class is called when an object is deleted through a pointer
	// to the base class.
    virtual ~MyAllocator() = default;
    virtual char* allocate(std::size_t size) const = 0;
    virtual void deallocate(char* ptr, std::size_t size) const = 0;
};

class DefaultStringAllocator : public MyAllocator {
public:
    DefaultStringAllocator() = default;
    char* allocate(std::size_t size) const override {
        return new char[size];
    }

    void deallocate(char* ptr, std::size_t size) const override {
        ::operator delete(ptr, size * sizeof(char));
    }
};

#endif // STRINGALLOCATOR_H
