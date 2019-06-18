#include "Mesh.h"
#include <fstream>
using namespace std;

namespace AnEngine::Assets
{
	std::byte* LoadFromFile(const wstring&);
#ifdef OPENFBX
	std::byte* LoadFromFile(const wstring& filePath)
	{
		ifstream fin(filePath, ios::binary);
		return nullptr;
		fin.seekg(0, ios::end);
		int fileLength = fin.tellg();
		fin.seekg(0, ios::beg);
		char* buffer = new char[fileLength];
		fin.read(buffer, fileLength);

		var sence = ofbx::load(reinterpret_cast<ofbx::u8*>(buffer), fileLength);
	}
#endif // OPENFBX


	Mesh::Mesh(const std::wstring& filePath)
	{

	}
}