#include <memory>

// Purpose of class unique_ptr
// Functions often operate in the following way:
// 1. Acquire some resources.
// 2. Perform some operations.
// 3. Free the acquired resources.
// A typical example of using pointers in this way is the use of new and delete to create and destroy an object:
// void f()
// {
//		classA* ptr = new classA;
//		...
//		delete ptr;
// }
// It's a source of trouble. One obvious problem is that the deletion of the object might be forgotten, especially
// if you have return statements inside the function. There also is a less obvious danger that an exception might
// occur. Such an exception would exit the function immediately without calling the delete statement at the end of
// the function. The result would be a memory leak or, more generally, a resource leak.
//
// Avoiding such as resouce leak usually requires that a function catch all exceptions. For example:
// void f()
// {
//		classA* ptr = new classA;	// create an object explicitly
//		try {
//		...							// perform some operations
//		}
//		catch(...) {				// for any exception
//			delete ptr;				// clean up
//			throw;					// rethrow the exception
//		}
//
//		delete ptr;					// clean up on normal end
//	}
//
//	To handle the deletion of this object properly in the event of an exception, the code gets more complicated and redundant.
//	If a second object is handled in this way, or if more than one catch clause is used, the problem gets worse. This is bad
//	programming style and should be avoided because it is complex and error prone.
//
//	A unique_ptr has much the same interface as an ordinary pointer except for pointer arithmetic, such as ++.
//	Note that class unique_ptr<> does not allow you to initialize an object with an ordinary pointer by using the assignment
//	syntax. Thus, you must initialize the unique_ptr directly, by using its value:
//	std::unique_ptr<int> up = new int; // ERROR
//	std::unique_ptr<int> up (new int); // OK
//
// A unique_ptr does not have to own an object, so it can be empty.
// std::unique_ptr<std::string> up;
// up = nullptr; // can also assign the nullptr
// up.reset();   // or call reset;
//
// std::string* sp = up.release(); // up loses ownership
//
// if (up) { // check whether a unique_ptr owns an object by calling operator bool()
//		std::cout << *up << std::endl;
// }
//
// A unique_ptr provides the semantics of exclusive ownership, however it's up to the programmer
// to ensure that no two unique pointers are initialized by the same pointer.
// std::string* sp = new std::string("hello");
// std::unique_ptr<std::string> up1(sp);
// std::unique_ptr<std::string> up2(sp); // ERROR: up1 and up2 own same data
//
// We can't copy or assign a unique pointer if we use the ordinary copy semantics. However, we can
// use the move semantics provided since C++11. In that case, the constructor or assignment
// operator transfers the ownership to another unique pointer.
//
// Consider, for example, the following use of the copy constructor:
// std::unique_ptr<ClassA> up1(new ClassA); // initialize a unique_ptr with a new object
// std::unique_ptr<ClassA> up2(up1); // ERROR: not possible
// std::unique_ptr<ClassA> up3(std::move(up1)); // OK
//
// The assignment operator behaves similarly:
// std::unique_ptr<ClassA> up1(new ClassA);
// std::unique_ptr<ClassA> up2;
// up2 = up1 // ERROR: not possible
// up2 = std::move(up1); // assign the unique_ptr: transfers ownership from up1 to up2
//
