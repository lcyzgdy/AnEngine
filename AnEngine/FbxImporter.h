#pragma once
#ifndef __FBXIMPORTER_H__
#define __FBXIMPORTER_H__

#define ASSIMP

#include "onwind.h"
#include "AssetsConfig.h"

namespace AnEngine::AssetsWrapper
{
	DLL_API LoadAssetsStatusCode LoadFbxFromFile(const std::string&);
	DLL_API LoadAssetsStatusCode LoadFbxFromFile(std::string&&);
}

#endif // !__FBXIMPORTER_H__