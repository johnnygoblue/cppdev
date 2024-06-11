/* Ordered_list is a linked-list class template  with iterators similar to 
the Standard Library std::list class. Each list node contains an object of the type 
specified in the first template parameter, T.

This is an ordered list in that the nodes are automatically kept in order. The type of 
the ordering function is a second template parameter, OF (for Ordering Function), 
whose default value is a type given by a small template for a function object class 
that orders two objects of type T using that type's less-than operator. Thus the default 
is to order objects from smallest to largest using their defined less-than relationship.
Another template provides a function object class that orders two pointers to objects 
of type T by dereferencing the pointers and applying T's less-than operator.
For example:

    // Thing objects in order by Thing::operator<.
 	Ordered_list<Thing, Less_than_ref> ol_things;
 
	// the same as above, by default
	Ordered_list<Thing> ol_things;	
    
    // Thing pointers in order by Thing::operator< applied to dereferenced pointers.
    Ordered_list<Thing*, Less_than_ptrs> ol_things;

    // Thing pointers in order by a custom ordering.
 	Ordered_list<Thing*, My_ordering_class> ol_things;
	
The only way to add to the list is with the insert function, which  automatically puts 
the new item in the proper place in the list using the ordering function to determine 
the point of insertion. If a matching object is already in the list (as determined by
the ordering function) the insertion fails - the list is not modified, and false is 
returned by the insert function.

The iterators encapsulate a pointer to the list nodes, and are a public class nested 
within the Ordered_list class, and would be declared e.g. as 
Ordered_list<Thing*, Less_than_ptr>::Iterator;
Operators ++, *, and -> are overloaded for iterators similar to std::list<>::iterator.

This container supports const operations; if the container is declared as a const
object, then the compiler will automatically apply the const versions of the begin(),
end(), and find() functions, which will return const_Iterators. A const_Iterator can not
be used to modify the data to which it points. const_Iterators are also a public class
nested within the Ordered_list class.

Copy constructor and assignment operators are defined, so that Ordered_lists can 
be used like built-in types. Move construction and assignment operators are also defined, 
consistent with C++11 container library practice.

The operations on Ordered_list provide exception safety in the form of the basic 
exception guarantee - if an attempt to create or modify an Ordered_list fails and 
an exception is thrown, no memory is leaked and the exception is propagated out to 
the client code to catch. In addition, the relevant operations also provide the strong 
exception guarantee - if an attempt to modify an Ordered_list fails, it is left in its
original state, and the exception is propagated out to the client code to catch. 
Finally, many operations also provide the no-throw guarantee and are specified 
with noexcept. See comments on individual member functions for specifics.
 
To find an object in the list that matches a supplied "probe" object, the ordering 
function is used to determine equality. That is, the find functions assume that
if both (x < y) and (y < x) are false, then x == y. This allows both insert and find 
operations to be done with only the less-than relation.
 
When an object is inserted in the list using the copy version of insert, 
a list node is constructed that contains a copy of the supplied object, 
so objects stored in the list must have accessible and properly defined copy constructors. 
The move version of the insert function will attempt to move construct 
the supplied object in the new list node instead of copying it. 
A move constructor for the object must be available for this to happen.

When a node is removed from the list with erase(), it is destroyed, and so a class-type object
contained in the node must have an accessible and properly defined destructor function.  
When the list is destroyed, or cleared with the clear() function, all of the list nodes 
are destroyed.

Unless the container is declared to be const, this class does not protect the list items 
from being modified. If a list item is modified in a way that changes where it should 
appear in the list, the list will become disordered and list items may become un-findable 
or new items will be inserted incorrectly - the effects are undefined, although a specific 
implementation might behave in a predictable manner. It is user's responsibility to ensure 
that items are not changed in a way that affects the validity of the ordering in the list.

If the user declares a list of pointers to objects, the user is responsible for allocating 
and deallocating the pointed-to objects. Note especially that if the Ordered_list is 
deallocated or cleared, or a single node is erased from the list, the pointed-to data 
is NOT deallocated. In short, the Ordered_list does not attempt to manage the user's objects.

If any operations are attempted that are erroneous (e.g. erasing a non-existent node), the
results are undefined. 

This module includes some function templates for applying functions to items in the container,
using iterators to specify the range of items to apply the function to.

All Ordered_list constructors and the destructor increment/decrement g_Ordered_list_count.
The list Node constructors and destructor increment/decrement g_Ordered_list_Node_count.
*/
#ifndef ORDERED_LIST_H
#define ORDERED_LIST_H

#include <utility>
#include <cassert>
#include <iterator>
#include <algorithm>

extern int g_Ordered_list_count;
extern int g_Ordered_list_Node_count;

// a simple class for error exceptions - msg points to a C-string error message
struct Error {
    Error(const char* msg_ = "") : msg(msg_) {}
    const char* const msg;
};

// These Function Object Class templates make it simple to use a class's less-than operator
// for the ordering function in declaring an Ordered_list container.
// These declare operator() as a const member function because the function does
// not modify the state of the function object, meaning that it can be used in
// a const member function of the Ordered_list class.

// Compare two objects (passed by const&) using T's operator<
template<typename T>
struct Less_than_ref {
    bool operator() (const T& t1, const T& t2) const {return t1 < t2;}
};

// Compare two pointers (T is a pointer type) using *T's operator<
template<typename T>
struct Less_than_ptr {
	bool operator()(const T p1, const T p2) const {return *p1 < *p2;}
};

// Forward declaration of the Ordered_list
template<typename T, typename OF>
class Ordered_list;

// Node is a template class for a node in an Ordered_list. Because it is an implementation detail of Ordered_list,
// it has no public interface - all of its members are private, but it declares class Ordered_list<T, OF> as a friend.
// T is the type of the objects in the list - the data item in the list node. See Stroustrup's The C++ Programming Language 23.4.6.3
template<typename T>
class Node {
private:
    // delcare the client class as a friend - note the template parameter names are different here
    template<typename A, typename B> friend class Ordered_list;

    Node(const T& new_datum, Node* new_prev, Node* new_next) :
	datum(new_datum), prev(new_prev), next(new_next)
	{ g_Ordered_list_Node_count++; }

    // Move construct a node using rvalue reference to the new datum
    // and move constructs the datum member variable from it, using its move constructor
    Node(T&& new_datum, Node* new_prev, Node* new_next) :
	datum(std::move(new_datum)), prev(new_prev), next(new_next)
	{ g_Ordered_list_Node_count++; }

    Node(const Node& original) :
	datum(original.datum), prev(original.prev), next(original.next)
        { g_Ordered_list_Node_count++; }

    // The following functions should not be needed and so are deleted
    Node(Node&& original) = delete;
    Node& operator= (const Node& rhs) = delete;
    Node& operator= (Node&& rhs) = delete;
    
    // only defined to support allocation counting
    ~Node()
        { g_Ordered_list_Node_count--; }

    T datum;
    Node* prev; 
    Node* next;
};

// T is the type of the objects in the list - the data item in the list node
// OF is the ordering function object type, defaulting to Less_than_ref for T
template<typename T, typename OF = Less_than_ref<T> >
class Ordered_list {
public:
    // Default constructor creates an empty container that has an ordering function of type OF
    Ordered_list();
    // Copy construct this list from another list by copying its data    
    Ordered_list(const Ordered_list& original);
    // Move construct this list from another list by taking its data, leaving the original in an empty state (like when default constructed)
    Ordered_list(Ordered_list&& original) noexcept;
    // Copy assign this list with a copy of another list, using copy-swap idiom.
    // Basic and strong exception guarantee:
    // If an exception is thrown during the copy, no meomory is leaked, and lhs is unchanged.
    Ordered_list& operator= (const Ordered_list& rhs);
    // Move assignment operator simply swaps the current content with the rhs.
    // Since no type T data is copied, no exceptions are possible, so the no-throw guarantee is made.
    Ordered_list& operator= (Ordered_list&& rhs) noexcept;
    // deallocate all the nodes in this list
    ~Ordered_list();
    // Delete the nodes in the list, if any, and initialize it.
    void clear() noexcept;
    // Return the number of nodes in the list
    int size() const { return node_count; }
    // Return true if the list is empty
    bool empty() const { return node_count == 0; }

    // Iterator is a public nested class within Ordered_list.
    // An iterator object designates a Node by encapsulating a pointer to the Node,
    // and provides Standard Library-stype operators for using, manipulating,
    // and comparing Iterators. This class is nested inside Ordered_list<> as a public member; refer to as e.g. Ordered_list<int, My_of>::Iterator
    class Iterator {
    public:
        Iterator() : node_ptr(nullptr) {} // default initialize to nullptr
        // Overloaded dereferencing operator
        // The * operator returns a reference to the datum in the pointed-to node.
        T& operator* () const { assert(node_ptr); return node_ptr->datum; }
        // The -> operator simply returns the address of the datain the pointed-to node.
        // The compiler reapplies the -> operator with the returned pointer.
        T* operator-> () const { assert(node_ptr); return &(node_ptr->datum); }
        // Prefix ++ operator moves the iterator forward to point to the next node
        Iterator& operator++ () {
            assert(node_ptr);
            node_ptr = node_ptr->next;
            return *this;
        }
        // Postfix ++ operator saves the current address for the pointed-to node,
        // moves this iterator to point to the next node, and returns
        // an iterator pointing to the node at the saved address
        Iterator operator++ (int) {
            Iterator temp(*this);
            ++(*this);
            return temp;
        }
        // Iterators are equal if they point to the same node
        bool operator== (Iterator rhs) const { return node_ptr == rhs.node_ptr; }
        bool operator!= (Iterator rhs) const { return node_ptr != rhs.node_ptr; }

    private:
        /* *** define a private constructor for Iterator that takes a Node<T>* param.
        Ordered_list can use this to create Iterators conveniently initialized to point to a Node.
        It is private because the client code can't and shouldn't be using it - it isn't even supposed to know about the Node objects. */
        Iterator(Node<T>* ptr) : node_ptr(ptr) {}
        Node<T>* node_ptr;
        friend class Ordered_list<T, OF>;
    };

    // const_Iterator is a public nested class within Ordered_list.
    // It behaves identically to an Iterator except that it cannot be used to modify
    // the pointed-to data, as shown by its dereferencing operators returning const.
    class const_Iterator {
    public:
        const_Iterator() : node_ptr(nullptr) {} // defualt initialize to nullptr
        // A conversion constructor: construct a const_Iterator from an Iterator
        const_Iterator(Iterator original) : node_ptr(original.node_ptr) {}
        // Overloaded dereferencing operators
        // The * operator returns a const reference to the datum in the pointed-to node.
        const T& operator* () const { assert(node_ptr); return node_ptr->datum; }
        // The -> operator returns the address of the data in the pointed-to node as const.
        const T* operator-> () const { assert(node_ptr); return &(node_ptr->datum); }
        // prefix ++ operator moves the iterator forward to point to the next node
        // and returns this iterator
        const_Iterator& operator++ () {
            assert(node_ptr);
            node_ptr = node_ptr->next;
            return *this;
        }
        // postfix ++ operator saves the current address for the pointed-to node,
        // moves this iterator to point to the next node, and returns
        // an iterator pointing to the node at the saved address.
        const_Iterator operator++ (int) {
            const_Iterator temp(*this);
            ++(*this);
            return temp;
        }
        // const_Iterators are equal if they point to the same node.
        bool operator== (const_Iterator rhs) const { return node_ptr == rhs.node_ptr; }
        bool operator!= (const_Iterator rhs) const { return node_ptr != rhs.node_ptr; }

    private:
        // Ditto from Iterator's private method
        const_Iterator(const Node<T>* ptr) : node_ptr(ptr) {}
        const Node<T>* node_ptr;
        friend class Ordered_list<T, OF>;
    };

    /* Supply begin and end iterators for a non-const container */
    // Return an iterator pointing to the first node.
    Iterator begin() { return Iterator(head); }
    // Return an iterator pointing to "past the end".
    Iterator end() { return Iterator(nullptr); }
    // Return a const_Iterator pointing to the first node.
    // If the list is empty, the Iterator points to "past the end"
    const_Iterator cbegin() { return const_Iterator(head); }
    const_Iterator cend() { return const_Iterator(nullptr); }
    const_Iterator cbegin() const { return const_Iterator(head); }
    // Return an iterator pointing to "past the end"
    const_Iterator cend() const { return const_Iterator(nullptr); }
    /* Supply begin and end iterators for a const container */
    const_Iterator begin() const { return const_Iterator(head); }
    const_Iterator end() const { return const_Iterator(nullptr); }

    bool insert(const T& new_datum);
    bool insert(T&& new_datum);
    Iterator find(const T& probe_datum) noexcept;
    const_Iterator find(const T& probe_datum) const noexcept;
    // Delete the specified node, specified by a const_Iterator because it only
    // designates the location of the to-be-removed data. This function can be called with an Iterator
    // which will be simply converted to a const_Iterator by the conversion function above.
    // Caller is responsible for any required deletion of any pointed-to data beforehand.
    // Do not attempt to dereference the iterator after calling this function - it
    // is invalid after this function executes. The results are undefined if the
    // Iterator does not point to an actual node, or the list is empty.
    void erase(const_Iterator it) noexcept;
    // Interchange the member variable values of this list with the other list;
    // Only the pointers and size are interchanged;
    // no allocation or deallocation of list Nodes is done.
    // Thus the no-throw guarantee can be provided.
    // This function can only be called with another list of the same type as
    // this list, which means that the ordering function objects must be identical, and
    // so do not need to be swapped.
    void swap(Ordered_list& other) noexcept;

private:
    OF ordering_fo;
    Node<T>* head;
    Node<T>* tail;
    int node_count;

    void init();
    bool insert_node(Node<T>* new_node) {
        if (!head) {
            head = tail = new_node;
        } else {
            Node<T>* current = head;
            Node<T>* prev = nullptr;
            while (current && ordering_fo(current->datum, new_node->datum)) {
                prev = current;
                current = current->next;
            }
                // if a < b are b < a are both false then a must be equal to b
            if (current && !ordering_fo(new_node->datum, current->datum)) {
                delete new_node;
                return false;
            }
            new_node->next = current;
            new_node->prev = prev;
            if (prev) {
                prev->next = new_node;
            } else {
                head = new_node;
            }
            if (current) {
                current->prev = new_node;
            } else {
                tail = new_node;
            }
        }
        node_count++;
        return true;
    }
    // Create a new node and append it to this list, ignoring the ordering which is guaranteed
    void push_back(const T& value) {
        // Allocate new node outside of modifying the list
        Node<T>* newNode = nullptr;
        try {
            newNode = new Node<T>(value, nullptr, nullptr); // This can throw an exception
        } catch (...) {
            delete newNode;
            throw; // Re-throw the caught exception
        }
        // New node is successfully allocated, now append it to the list
        if (!head) {
            head = newNode;
        } else {
            Node<T>* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
        ++node_count;
    }
};

// Ordered_list class member function definitions

template<typename T, typename OF>
Ordered_list<T, OF>::Ordered_list() : head(nullptr), tail(nullptr), node_count(0), ordering_fo(OF()) {
    g_Ordered_list_count++;
}

// Since the invariants are kept in original, we don't need to determine the ordering (insert) 
// when copying over the nodes, simply appending the nodes one by one will do the job.
// The use of RAII approach ensures strong exception guarantee (automatically clean up if the buildup
// encounters an exception)
template<typename T, typename OF>
Ordered_list<T, OF>::Ordered_list(const Ordered_list& original) : head(nullptr), tail(nullptr), node_count(0), ordering_fo(original.ordering_fo) {
    try {
        Ordered_list temp; // temporary list
        Node<T>* current = original.head;
        while (current) {
            temp.push_back(current->datum);
            current = current->next;
        }
        this->swap(temp); // swap the contents after temp is successfully constructed
        ++g_Ordered_list_count;
    } catch (...) {
        // If an exception is thrown during the building of temp, the destructor is called 
        // when temp is out of scope hence ensuring no memory leak is possible. Therefore,
        // we don't explicitly call temp.clear() here.
        throw;
    }
}

// Move constructor
template<typename T, typename OF>
Ordered_list<T, OF>::Ordered_list(Ordered_list&& original) noexcept : head(nullptr), tail(nullptr), node_count(0), ordering_fo(OF()) {
    swap(original);
    ++g_Ordered_list_count;    
}

// Employee the copy-swap idiom and reuse code from copy constructor
template<typename T, typename OF>
Ordered_list<T, OF>& Ordered_list<T, OF>::operator=(const Ordered_list& rhs) {
    if (this != &rhs) {
        try {
            Ordered_list copy(rhs);
            swap(copy);
        } catch (...) {
            throw; // rethrow
        }
    }
    return *this;
}

template<typename T, typename OF>
Ordered_list<T, OF>& Ordered_list<T, OF>::operator=(Ordered_list&& rhs) noexcept {
    if (this != &rhs) {
        clear();
        swap(rhs);
    }
    return *this;
}

template<typename T, typename OF>
Ordered_list<T, OF>::~Ordered_list() {
	clear();
	g_Ordered_list_count--;
}

template<typename T, typename OF>
void Ordered_list<T, OF>::clear() noexcept {
    while (head) {
        Node<T>* temp = head;
        head = head->next;
        delete temp;
    }
    head = tail = nullptr;
    node_count = 0;
}

template<typename T, typename OF>
bool Ordered_list<T, OF>::insert(const T& new_datum) {
    Node<T>* new_node = nullptr;
    try {
        new_node = new Node<T>(new_datum, nullptr, nullptr);
        return insert_node(new_node);
    } catch (const std::exception& e) {
        delete new_node;
        //std::cout << "\nOops except occur" << std::endl;
        throw Error(e.what());
    } catch (...) {
        delete new_node;
        throw Error("Unknown error occurred");
    }
}
template<typename T, typename OF>
bool Ordered_list<T, OF>::insert(T&& new_datum) {
    Node<T>* new_node = nullptr;
    try {
        new_node = new Node<T>(new_datum, nullptr, nullptr);
        return insert_node(new_node);
    } catch (const std::exception& e) {
        delete new_node;
        //std::cout << "\nOops except occur" << std::endl;
        throw Error(e.what());
    } catch (...) {
        delete new_node;
        throw Error("Unknown error occurred"); // rethrow
    }
}

template<typename T, typename OF>
typename Ordered_list<T, OF>::Iterator Ordered_list<T, OF>::find(const T& probe_datum) noexcept {
    Node<T>* current = head;
    while (current) {
        if (!ordering_fo(current->datum, probe_datum) && !ordering_fo(probe_datum, current->datum)) {
            return Iterator(current);
        }
        current = current->next;
    }
    return end();
}

template<typename T, typename OF>
typename Ordered_list<T, OF>::const_Iterator Ordered_list<T, OF>::find(const T& probe_datum) const noexcept {
    const Node<T>* current = head;
    while (current) {
        if (!ordering_fo(current->datum, probe_datum) && !ordering_fo(probe_datum, current->datum)) {
            return const_Iterator(current);
        }
        current = current->next;
    }
    return cend();
}

template<typename T, typename OF>
void Ordered_list<T, OF>::erase(const_Iterator it) noexcept {
    const Node<T>* node_to_delete = it.node_ptr;
    if (!node_to_delete) return;

    if (node_to_delete->prev) {
        node_to_delete->prev->next = node_to_delete->next;
    } else {
        head = node_to_delete->next;
    }

    if (node_to_delete->next) {
        node_to_delete->next->prev = node_to_delete->prev;
    } else {
        tail = node_to_delete->prev;
    }

    delete node_to_delete;
    node_count--;
}

template<typename T, typename OF>
void Ordered_list<T, OF>::swap(Ordered_list& other) noexcept {
    std::swap(head, other.head);
    std::swap(tail, other.tail);
    std::swap(node_count, other.node_count);
}

template<typename IT, typename F>
void apply(IT first, IT last, F function) {
    for (; first != last; ++first)
        function(*first);
}

// These function templates are given two iterators, usually .begin() and .end(),
// and apply a function to each item (dereferenced iterator) in the list.
//  The templates work for both Iterators and const_Iterators.
//  Note that the function must be an ordinary function; these templates will not
//  instantiate if the function is a member function of some class. However, you
//  can often use a simple ordinary function with parameters that calls the
//  member function using the parameter values.
template<typename IT, typename F, typename A>
void apply_arg(IT first, IT last, F function, A arg) {
    for (; first != last; ++first)
        function(*first, arg);
}

// this function templates accept the second argument by reference - useful for streams.
template<typename IT, typename F, typename A>
void apply_arg_ref(IT first, IT last, F function, A& arg) {
    for (; first != last; ++first)
        function(*first, arg);
}

// the function must return true/false; apply the function until true is returned,
// then return true; otherwise return false.
template<typename IT, typename F>
bool apply_if(IT first, IT last, F function) {
    for (; first != last; ++first)
        if (function(*first))
            return true;
    return false;
}

// this function works like apply_if, with a fourth parameter used as the second argument for the function
template<typename IT, typename F, typename A>
bool apply_if_arg(IT first, IT last, F function, A arg) {
    for (; first != last; ++first)
        if (function(*first, arg))
            return true;
    return false;
}

template<typename T, typename OF>
void Ordered_list<T, OF>::init() {
    head = nullptr;
    tail = nullptr;
    node_count = 0;
}

#endif // ORDERED_LIST_H
