#pragma once
#ifndef __FBXIMPORTER_H__
#define __FBXIMPORTER_H__

#define OPENFBX

#include "onwind.h"

#if defined OPENFBX

#include "ofbx.h"

#endif	

namespace AnEngine::Resource
{
	std::byte* LoadFbxFromFile(const std::wstring&);
}

#endif // !__FBXIMPORTER_H__
