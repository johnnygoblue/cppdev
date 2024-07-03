#ifndef STRING_H
#define STRING_H

#include <iostream>
#include <cstring>
#include <stdexcept>
#include <optional>
#include <memory>

#include "StringAllocator.h"

// The below code is to help determine if T is of type char array
// In template deduction, const char* is deduced as char[N] instead
// of the intended type const char*
template<typename T>
struct is_char_array : std::false_type {};

template<typename T, std::size_t N>
struct is_char_array<T[N]> : std::is_same<T, char> {};

template<typename Allocator = DefaultStringAllocator>
class String {
public:
    using allocator_type = Allocator;

    // Default constructor
    String(const Allocator& allocator_ = Allocator()) :
        data(&a_null_byte), length(0), allocation(1), allocator(allocator_) {
            ++number;
            total_allocation += allocation;
    }

    //// Templated constructor is an attempt to combine two constructors to avoid
    //// code duplication (WIP)
    //template<typename T>
    //String(const T&& input, const Allocator& allocator_ = Allocator()) :
    //    data(&a_null_byte), length(0), allocation(1), allocator(allocator_) {
    //    if constexpr (is_char_array<T>::value) {
    //        if (messages_wanted)
    //            std::cout << "(CStringConstructor) String(const char* cstr_) called with: " << (input ? input : "") << std::endl;
    //        initialize(input);
    //    } else if constexpr (std::is_same_v<T, String<Allocator>>) {
    //        if (messages_wanted)
    //            std::cout << "(CopyConstructor) String(const String& original) called with: " << input.data << std::endl;
    //        initialize(input.data);
    //    }
    //    ++number;
    //    total_allocation += allocation;
    //}

    String(const char* cstr_, const Allocator& allocator_ = Allocator());

    // The copy constructor initializes this String with the original's data,
    // and gets minimum allocation.
    String(const String& original, const Allocator& allocator_ = Allocator());

    // Move constructor - take original's data, and set the original String
    // member variables to the empty state (do not initialize "this" String and swap).
    String(String&& original) noexcept;

    // deallocate C-string memory
    ~String() noexcept;

    // Assignment operators
    // Left-hand side gets a copy of rhs data and gets minimum allocation.
    // This operator uses the copy-swap idiom for assignment.
    String& operator=(const String& rhs);

    // This operator creates a temporary String object from the rhs C-string, and swaps the contents
    String& operator=(const char* rhs);

    // Move assignment - simply swaps contents with rhs without any copying
    String& operator=(String&& rhs) noexcept;

    // Accessors
    // Return a pointer to the internal C-string
    const char* c_str() const { return data ? data : &a_null_byte; }

    // Return size (length) of internal C-string in this String
    int size() const { return length; }

    // Return current allocation for this String
    int get_allocation() const { return allocation; }

    // Return a reference to character i in the string.
    // Throw exception if 0 <= i < size is false.
    char& operator[](int i);
    const char& operator[](int i) const; // const version for const Strings

    // Modifiers
    // Set to an empty string with minimum allocation by create/swap with an empty string.
    void clear();

    /* These concatenation operators add the rhs string data to the lhs object.
    They do not create any temporary String objects. They either directly copy the rhs data
    into the lhs space if it is big enough to hold the rhs, or allocate new space
    and copy the old lhs data into it followed by the rhs data. The lhs object retains the
    final memory allocation. If the rhs is a null byte or an empty C-string or String,
    no change is made to lhs String. */
    String& operator+=(char rhs);
    String& operator+=(const char* rhs);
    String& operator+=(const String& rhs);

    /* Swap the contents of this String with another one.
    The member variable values are interchanged, along with the
    pointers to the allocated C-strings, but the two C-strings
    are neither copied nor modified. No memory allocation/deallocation is done. */
    void swap(String& other) noexcept;

    /* Monitoring functions - not part of a normal implementation */
    /* used here for demonstration and testing purposes. */

    // Return the total number of Strings in existence
    static int get_number() { return number; }

    // Return total bytes allocated for all Strings in existence
    static int get_total_allocation() { return total_allocation; }

    // Call with true to cause ctor, assignment, and dtor messages to be output.
    // These messages are output from each function before it does anything else.
    static void set_messages_wanted(bool messages_wanted_) { messages_wanted = messages_wanted_; }

private:
    char* data;                             // pointer to the internal C-string
    int length;                             // length of the internal C-string (size)
    int allocation;                         // total allocation including the null terminator

    // abstract allocator for testsing purposes, we store by value to avoid the
    // nasty dangling reference issues, but since GMock object isn't copy-able
    // we're outta luck on that aspect
    allocator_type allocator;

    static char a_null_byte; // to hold a null byte for empty string representation

    /* Variables for monitoring functions - not part of a normal implementation. */
    /* But used here for demonstration and testing purposes. */
    static int number;              // counts number of String objects in existence
    static int total_allocation;    // counts total amount of memory allocated
    static bool messages_wanted;    // whether to output constructor/destructor/operator= messages, initially false

    // internal method to allocate more bytes, used during concatenation
    void ensure_allocation(int additional_length);
    // internal method to allocate for constructors
    void initialize(const char* cstr);
    char& get_char_at (int i) const; // private method that performs bounds checking
};

template<typename Allocator>
char String<Allocator>::a_null_byte = '\0';
template<typename Allocator>
int String<Allocator>::number = 0;
template<typename Allocator>
int String<Allocator>::total_allocation = 0;
template<typename Allocator>
bool String<Allocator>::messages_wanted = false;
// non-member overloaded operators

/* Concatenate a String with another String.
 If one of the arguments is a C-string, the String constructor will automatically create
 a temporary String for it to match this function (inefficient, but instructive).
 This automatic behavior would be disabled if the String constructor was declared "explicit".
 This function constructs a copy of the lhs in a local String variable,
 then concatenates the rhs to it with operator +=, and returns it. */
template<typename Allocator>
String<Allocator> operator+(const String<Allocator>& lhs, const String<Allocator>& rhs);

// Input and output operators
// The output operator writes the contents of the String to the stream
template<typename Allocator>
std::ostream& operator<<(std::ostream& os, const String<Allocator>& str);

/* The input operator clears the supplied String, then starts reading the stream.
It skips initial whitespace, then copies characters into
the supplied str until whitespace is encountered again. The terminating
whitespace remains in the input stream, analogous to how string input normally works.
str is expanded as needed, and retains the final allocation.
If the input stream fails, str contains whatever characters were read. */
template<typename Allocator>
std::istream& operator>>(std::istream& is, String<Allocator>& str);

// C-String constructor
template<typename Allocator>
String<Allocator>::String(const char* cstr_, const Allocator& allocator_) :
    data(nullptr), length(0), allocation(1), allocator(allocator_) {
    if (messages_wanted)
        std::cout << "(CStringConstructor) String(const char* cstr_) called with: " << (cstr_ ? cstr_ : "") << std::endl;

    initialize(cstr_);

    ++number;
    total_allocation += allocation;
}

// Copy constructor
template<typename Allocator>
String<Allocator>::String(const String<Allocator>& original, const Allocator& allocator_) :
    data(nullptr), length(0), allocation(1), allocator(allocator_) {
    if (messages_wanted)
        std::cout << "(CopyConstructor) String(const String& original) called with: " << original.data << std::endl;

    initialize(original.data);

    ++number;
    total_allocation += allocation;
}

// Move constructor
template<typename Allocator>
String<Allocator>::String(String<Allocator>&& original) noexcept :
    data(original.data), length(original.length), allocation(original.allocation), allocator(Allocator()) {
    if (messages_wanted)
        std::cout << "(MoveConstructor) String(String&& original) called with: " << original.data << std::endl;

    original.data = nullptr;
    original.length = 0;
    original.allocation = 0;
    ++number;
}

// C-String copy assignment
template<typename Allocator>
String<Allocator>& String<Allocator>::operator=(const char* rhs) {
    if (messages_wanted)
        std::cout << "(CStringAssignment) operator=(const char* rhs) called with: " << (rhs ? rhs : "") << std::endl;

    String temp(rhs); // Strong exception guarantee
    swap(temp);
    return *this;
}

// Copy assignment
template<typename Allocator>
String<Allocator>& String<Allocator>::operator=(const String<Allocator>& rhs) {
    if (this != &rhs) {
        if (messages_wanted)
            std::cout << "(CopyAssignment) operator=(const String& rhs) called with: " << rhs.data << std::endl;

        String temp(rhs); // Strong exception guarantee
        swap(temp);
    }
    return *this;
}

// Move assignment
template<typename Allocator>
String<Allocator>& String<Allocator>::operator=(String<Allocator>&& rhs) noexcept {
    if (this != &rhs) {
        if (messages_wanted)
            std::cout << "(MoveAssignment) operator=(String&& rhs) called with: " << rhs.data << std::endl;

        swap(rhs);
    }
    return *this;
}

// Destructor
template<typename Allocator>
String<Allocator>::~String<Allocator>() noexcept {
    if (messages_wanted)
        std::cout << "(Destructor) ~String() called with: " << data << std::endl;

    if (length) {
        allocator.deallocate(data, allocation);
        total_allocation -= allocation;
    }

    --number;
}

// Subscript/Indexing operator
template<typename Allocator>
char& String<Allocator>::operator[](int i) {
    return const_cast<char&>(get_char_at(i));
}

// Subscript/Indexing operator
template<typename Allocator>
const char& String<Allocator>::operator[](int i) const {
    if (messages_wanted)
        std::cout << "(ConstReturnIndexOperator) called" << std::endl;
    return get_char_at(i);
}

template<typename Allocator>
char& String<Allocator>::get_char_at(int i) const {
    if (i < 0 || i >= length) {
        throw std::runtime_error("Out of bounds");
    }
    return data[i];
}

template<typename Allocator>
void String<Allocator>::ensure_allocation(int additional_length) {
    if (length + additional_length >= allocation) {
        int new_allocation = 2 * (length + additional_length + 1);
        char* new_data = nullptr;
        try {
            new_data = allocator.allocate(new_allocation);
            std::strcpy(new_data, data);
        } catch (...) {
            allocator.deallocate(new_data, new_allocation);
            throw std::runtime_error("Bad alloc");
        }
        if (length) {
            allocator.deallocate(data, allocation);
        }
        data = new_data;
        allocation = new_allocation;
        total_allocation += allocation - (length + 1);
    }
}

template<typename Allocator>
void String<Allocator>::initialize(const char* cstr) {
    if (cstr) {
        length = std::strlen(cstr);
        allocation = length + 1;
        try {
            data = allocator.allocate(allocation);
            std::strcpy(data, cstr);
        } catch (...) {
            allocator.deallocate(data, allocation);
            throw std::runtime_error("Bad alloc");
        }
    }
}

// Set to null state
template<typename Allocator>
void String<Allocator>::clear() {
    if (messages_wanted)
        std::cout << "clear() called" << std::endl;

    String temp;
    swap(temp);
}

// Concatenate single character
template<typename Allocator>
String<Allocator>& String<Allocator>::operator+=(char rhs) {
    if (messages_wanted)
        std::cout << "operator+=(char rhs) called with: " << rhs << std::endl;

    ensure_allocation(1);

    data[length] = rhs;
    data[++length] = '\0';
    return *this;
}

// Concatentate C-String
template<typename Allocator>
String<Allocator>& String<Allocator>::operator+=(const char* rhs) {
    if (messages_wanted)
        std::cout << "operator+=(const char* rhs) called with: " << (rhs ? rhs : "") << std::endl;

    if (rhs && *rhs) {
        int rhs_length = std::strlen(rhs);
        ensure_allocation(rhs_length);
        std::strcat(data, rhs);
        length += rhs_length;
    }
    return *this;
}

// Concatenate String
template<typename Allocator>
String<Allocator>& String<Allocator>::operator+=(const String<Allocator>& rhs) {
    if (messages_wanted)
        std::cout << "operator+=(const String& rhs) called with: " << rhs.data << std::endl;

    if (rhs.length > 0) {
        ensure_allocation(rhs.length);

        std::strcat(data, rhs.data);
        length += rhs.length;
    }
    return *this;
}

// Implement our own swap
template<typename Allocator>
void String<Allocator>::swap(String<Allocator>& other) noexcept {
    if (messages_wanted)
        std::cout << "swap(String& other) called" << std::endl;

    std::swap(data, other.data);
    std::swap(length, other.length);
    std::swap(allocation, other.allocation);
}

template<typename Allocator>
bool operator==(const String<Allocator>& lhs, const String<Allocator>& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

template<typename Allocator>
bool operator!=(const String<Allocator>& lhs, const String<Allocator>& rhs) {
    return !(lhs == rhs);
}

template<typename Allocator>
bool operator<(const String<Allocator>& lhs, const String<Allocator>& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

template<typename Allocator>
bool operator>(const String<Allocator>& lhs, const String<Allocator>& rhs) {
    return rhs < lhs;
}

template<typename Allocator>
String<Allocator> operator+(const String<Allocator>& lhs, const String<Allocator>& rhs) {
    String result(lhs);
    result += rhs;
    return result;
}


template<typename Allocator>
std::ostream& operator<<(std::ostream& os, const String<Allocator>& str) {
    return os << str.c_str();
}

template<typename Allocator>
std::istream& operator>>(std::istream& is, String<Allocator>& str) {
    str.clear();
    char ch;
    while (is.get(ch) && std::isspace(ch)) {
        // Skip initial whitespace
    }

    if (is) {
        do {
            str += ch;
        } while (is.get(ch) && !std::isspace(ch));
    }

    return is;
}
#endif // STRING_H
