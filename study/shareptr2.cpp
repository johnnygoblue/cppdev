#include <string>
#include <fstream>
#include <memory>
#include <cstdio>

class FileDeleter
{
	private:
		std::string filename;
	public:
		FileDeleter(const std::string& fn)
			: filename(fn) {}
		void operator() (std::ofstream* fp) {
			delete fp; // close file
			std::remove(filename.c_str()); // delete file
		}
};

int main()
{
	// create and open temp file
	// here we initialize a shared_ptr with a newly created output file. The passed FileDeleter
	// ensures that this file gets closed and deleted with standard C function remove(), provieded
	// in <cstdio> when the last copy of this shared pointer loses the ownership of this output
	// stream. Because remove() needs a filename, we pass this as an argument to the constructor
	// of FileDeletor.
	std::shared_ptr<std::ofstream> fp(new std::ofstream("tmpfile.txt"),
			FileDeleter("tmpfile.txt"));
	// ...
	return 0;
}
