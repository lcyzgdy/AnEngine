#include <iostream>

#include "Promise.h"

using namespace std;
using namespace AnEngine;

int main()
{
	Promise a([]()->void
	{
		cout << 1 << endl;
	});
	a.Then([]()->void
	{
		cout << 2 << endl;
	}).Then([]()->void
	{
		cout << 3 << endl;
	});
}