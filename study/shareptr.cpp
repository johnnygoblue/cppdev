#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

int main()
{
	// This way of creation is fast and safe because it uses one instead of two allocations:
	// one for the object and one for the shared data the shared pointer uses to control the
	// object
	//shared_ptr<string> pNico = make_shared<string>("nico");
	shared_ptr<string> pJutta = make_shared<string>("jutta");

	// the following ways are OK but not the most optimal
	shared_ptr<string> pNico(new string("nico"), [](string *p) {
			cout << "delete " << *p << endl;
			delete p;
			});
	//shared_ptr<string> pJutta{new string("jutta")};

	// capitalize person names
	(*pNico)[0] = 'N';
	pJutta->replace(0, 1, "J");

	// put them multiple times in a container
	vector<shared_ptr<string>> whoMadeCoffee;
	whoMadeCoffee.push_back(pJutta);
	whoMadeCoffee.push_back(pJutta);
	whoMadeCoffee.push_back(pNico);
	whoMadeCoffee.push_back(pJutta);
	whoMadeCoffee.push_back(pNico);

	// print all elements
	for (auto ptr : whoMadeCoffee) {
		cout << *ptr << " ";
	}
	cout << endl;

	// overwrite a name again
	*pNico = "Nicolai";

	// print all elements again
	for (auto ptr : whoMadeCoffee) {
		cout << *ptr << " ";
	}
	cout << endl;

	// print some internal data
	cout << "use_count: " << whoMadeCoffee[0].use_count() << endl;

	// resizes so no owner of pNico is existent hence pNico calls delete
	whoMadeCoffee.resize(2);
	return 0;
}
