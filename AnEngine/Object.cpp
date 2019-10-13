#include "Object.h"
#include "Hash.hpp"
using namespace std;

namespace AnEngine
{
	Object::~Object()
	{
	}
	uint64_t Object::GetHashCode()
	{
		return Utility::GetHash(this);
	}

	string Object::ToString()
	{
		return typeid(this).name();
	}
}