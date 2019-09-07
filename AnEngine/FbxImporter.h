#pragma once
#ifndef __FBXIMPORTER_H__
#define __FBXIMPORTER_H__

#define ASSIMP

#include "onwind.h"

namespace AnEngine::Resource
{
	std::byte* LoadFbxFromFile(const std::wstring&);
}

#endif // !__FBXIMPORTER_H__