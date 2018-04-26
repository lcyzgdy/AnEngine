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

	wstring Object::ToString()
	{
		wchar_t wc[256];
		memset(wc, 0, sizeof(wc));
		const char* c = typeid(this).name();
		for (int i = 0;; i++)
		{
			if (c == '\0') break;
			wc[i] = (wchar_t)(*c);
			c++;
		}
		return move(wstring(wc));
	}
}