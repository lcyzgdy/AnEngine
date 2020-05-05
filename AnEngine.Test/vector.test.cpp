#include <iostream>
#include <algorithm>
#include <vector>
using std::cout;
using std::endl;
using std::vector;

class A
{
public:
	A()
	{
		cout << "1" << endl;
	}
	A(const A& a)
	{
		cout << "2" << endl;
	}
	A(A&& a)
	{
		cout << "3" << endl;
	}
};

int main()
{
	vector<A> v;
	A a;
	// A b;
	// v.emplace_back(A());
	v.push_back(A());
	return 0;
}