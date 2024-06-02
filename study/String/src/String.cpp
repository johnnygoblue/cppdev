#include "String.h"
#include <iostream>
#include <cstring>

// Static member initialization
char String::a_null_byte = '\0';
int String::number = 0;
int String::total_allocation = 0;
bool String::messages_wanted = false;

String::String(const char* cstr_) : data(nullptr), length(0), allocation(0) {
    if (messages_wanted)
        std::cout << "(CStringConstructor) String(const char* cstr_) called with: " << (cstr_ ? cstr_ : "") << std::endl;

    if (cstr_) {
        length = std::strlen(cstr_);
        allocation = length + 1;
        data = new char[allocation];
        std::strcpy(data, cstr_);
    } else {
        data = &a_null_byte;
    }

    ++number;
    total_allocation += allocation;
}

String::String(const String& original) : data(nullptr), length(0), allocation(0) {
    if (messages_wanted)
        std::cout << "(CopyConstructor) String(const String& original) called with: " << original.data << std::endl;

    length = original.length;
    allocation = length + 1;
    data = new char[allocation];
    std::strcpy(data, original.data);

    ++number;
    total_allocation += allocation;
}

String::String(String&& original) noexcept : data(original.data), length(original.length), allocation(original.allocation) {
    if (messages_wanted)
        std::cout << "(MoveConstructor) String(String&& original) called with: " << original.data << std::endl;

    original.data = &a_null_byte;
    original.length = 0;
    original.allocation = 0;

    ++number;
}

String::~String() noexcept {
    if (messages_wanted)
        std::cout << "(Destructor) ~String() called with: " << data << std::endl;

    if (data != &a_null_byte) {
        delete[] data;
        total_allocation -= allocation;
    }

    --number;
}

String& String::operator=(const String& rhs) {
    if (this != &rhs) {
        if (messages_wanted)
            std::cout << "(CopyAssignment) operator=(const String& rhs) called with: " << rhs.data << std::endl;

        String temp(rhs);
        swap(temp);
    }
    return *this;
}

String& String::operator=(const char* rhs) {
    if (messages_wanted)
        std::cout << "(CStringAssignment) operator=(const char* rhs) called with: " << (rhs ? rhs : "") << std::endl;

    String temp(rhs);
    swap(temp);
    return *this;
}

String& String::operator=(String&& rhs) noexcept {
    if (this != &rhs) {
        if (messages_wanted)
            std::cout << "(MoveAssignment) operator=(String&& rhs) called with: " << rhs.data << std::endl;

        swap(rhs);
    }
    return *this;
}

char& String::operator[](int i) {
    return const_cast<char&>(get_char_at(i));
}

const char& String::operator[](int i) const {
	if (messages_wanted)
		std::cout << "(ConstReturnIndexOperator) called" << std::endl;
    return get_char_at(i);
}

char& String::get_char_at(int i) const {
	if (i < 0 || i >= length) {
		throw String_exception("Index out of bounds");
	}
	return data[i];
}

void String::clear() {
    if (messages_wanted)
        std::cout << "clear() called" << std::endl;

    String temp;
    swap(temp);
}

String& String::operator+=(char rhs) {
    if (messages_wanted)
        std::cout << "operator+=(char rhs) called with: " << rhs << std::endl;

    if (length + 1 >= allocation) {
        int new_allocation = 2 * (length + 2);
        char* new_data = new char[new_allocation];
        std::strcpy(new_data, data);
        if (data != &a_null_byte) {
            delete[] data;
        }
        data = new_data;
        allocation = new_allocation;
        total_allocation += allocation - (length + 1);
    }

    data[length] = rhs;
    data[++length] = '\0';
    return *this;
}

String& String::operator+=(const char* rhs) {
    if (messages_wanted)
        std::cout << "operator+=(const char* rhs) called with: " << (rhs ? rhs : "") << std::endl;

    if (rhs && *rhs) {
        int rhs_length = std::strlen(rhs);
        if (length + rhs_length >= allocation) {
            int new_allocation = 2 * (length + rhs_length + 1);
            char* new_data = new char[new_allocation];
            std::strcpy(new_data, data);
            if (data != &a_null_byte) {
                delete[] data;
            }
            data = new_data;
            allocation = new_allocation;
            total_allocation += allocation - (length + 1);
        }

        std::strcat(data, rhs);
        length += rhs_length;
    }
    return *this;
}

String& String::operator+=(const String& rhs) {
    if (messages_wanted)
        std::cout << "operator+=(const String& rhs) called with: " << rhs.data << std::endl;

    if (rhs.length > 0) {
        if (length + rhs.length >= allocation) {
            int new_allocation = 2 * (length + rhs.length + 1);
            char* new_data = new char[new_allocation];
            std::strcpy(new_data, data);
            if (data != &a_null_byte) {
                delete[] data;
            }
            data = new_data;
            allocation = new_allocation;
            total_allocation += allocation - (length + 1);
        }

        std::strcat(data, rhs.data);
        length += rhs.length;
    }
    return *this;
}

void String::swap(String& other) noexcept {
    if (messages_wanted)
        std::cout << "swap(String& other) called" << std::endl;

    std::swap(data, other.data);
    std::swap(length, other.length);
    std::swap(allocation, other.allocation);
}

bool operator==(const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

bool operator!=(const String& lhs, const String& rhs) {
    return !(lhs == rhs);
}

bool operator<(const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

bool operator>(const String& lhs, const String& rhs) {
    return rhs < lhs;
}

String operator+(const String& lhs, const String& rhs) {
    String result(lhs);
    result += rhs;
    return result;
}

std::ostream& operator<<(std::ostream& os, const String& str) {
    return os << str.c_str();
}

std::istream& operator>>(std::istream& is, String& str) {
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
