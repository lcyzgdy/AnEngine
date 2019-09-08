#pragma once
#ifndef __MATERIAL_H__
#define	__MATERIAL_H__

#include "Texture.h"

namespace AnEngine::Resource
{
	Texture* LoadTextureFromPngFile(const std::wstring& filePath);
}

#endif // !__TEXTUREIMPORTER_H__
