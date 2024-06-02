#ifndef STRING_H
#define STRING_H

#include <iostream>
#include <cstring>
#include <stdexcept>
#include <optional>

/* Simple exception class for reporting String errors */
struct String_exception {
    String_exception(const char* msg_) : msg(msg_) {}
    const char* msg;
};

class String {
public:
    // Default initialization is to contain an empty string with no allocation.
    // If a non-empty C-string is supplied, this String gets minimum allocation.
    String(const char* cstr_ = "");

    // The copy constructor initializes this String with the original's data,
    // and gets minimum allocation.
    String(const String& original);

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
    const char* c_str() const { return data; }

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
    char* data;           // pointer to the internal C-string
    int length;           // length of the internal C-string (size)
    int allocation;       // total allocation including the null terminator

    static char a_null_byte; // to hold a null byte for empty string representation

    /* Variables for monitoring functions - not part of a normal implementation. */
    /* But used here for demonstration and testing purposes. */
    static int number;              // counts number of String objects in existence
    static int total_allocation;    // counts total amount of memory allocated
    static bool messages_wanted;    // whether to output constructor/destructor/operator= messages, initially false
	char& get_char_at (int i) const; // private method that performs bounds checking

};

// non-member overloaded operators

// compare lhs and rhs strings; constructor will convert a C-string literal to a String.
// comparison is based on std::strcmp result compared to 0
bool operator==(const String& lhs, const String& rhs);
bool operator!=(const String& lhs, const String& rhs);
bool operator<(const String& lhs, const String& rhs);
bool operator>(const String& lhs, const String& rhs);

/* Concatenate a String with another String.
 If one of the arguments is a C-string, the String constructor will automatically create
 a temporary String for it to match this function (inefficient, but instructive).
 This automatic behavior would be disabled if the String constructor was declared "explicit".
 This function constructs a copy of the lhs in a local String variable,
 then concatenates the rhs to it with operator +=, and returns it. */
String operator+(const String& lhs, const String& rhs);

// Input and output operators
// The output operator writes the contents of the String to the stream
std::ostream& operator<<(std::ostream& os, const String& str);

/* The input operator clears the supplied String, then starts reading the stream.
It skips initial whitespace, then copies characters into
the supplied str until whitespace is encountered again. The terminating
whitespace remains in the input stream, analogous to how string input normally works.
str is expanded as needed, and retains the final allocation.
If the input stream fails, str contains whatever characters were read. */
std::istream& operator>>(std::istream& is, String& str);

#endif // STRING_H

