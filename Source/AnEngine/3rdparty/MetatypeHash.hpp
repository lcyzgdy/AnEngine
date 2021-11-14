#pragma once
#ifndef __METATYPEHASH_HPP
#define __METATYPEHASH_HPP

#include <string_view>

inline constexpr uint64_t hash_fnv1a(const char* key)
{
	uint64_t hash = 0xcbf29ce484222325;
	uint64_t prime = 0x100000001b3;

	int i = 0;
	while (key[i])
	{
		uint8_t value = key[i++];
		hash = hash ^ value;
		hash *= prime;
	}

	return hash;
}

/// <summary>
/// 来自purecpp公众号，编译期RTTI
/// </summary>
struct MetatypeHash
{
	size_t name_hash{ 0 };
	size_t matcher_hash{ 0 };

	bool operator==(const MetatypeHash& other) const
	{
		return name_hash == other.name_hash;
	}

	template<typename T>
	static constexpr const char* name_detail()
	{
		// #if RSTUDIO_CORE_PLATFORM == RSTUDIO_CORE_PLATFORM_LINUX			// 暂不考虑跨平台
		// 			return __PRETTY_FUNCTION__;
		// #else
		return __FUNCSIG__;
		// #endif
	}

#if RSTUDIO_CORE_PLATFORM != RSTUDIO_CORE_PLATFORM_WIN32
	template<typename T>
	static constexpr auto name_pretty()
	{
		std::string_view pretty_name = name_detail<T>();
		std::string_view prefix = "static constexpr const char* MetatypeHash::name_detail() [with T = ";
		std::string_view suffix = "]";
		pretty_name.remove_prefix(prefix.size());
		pretty_name.remove_suffix(suffix.size());
		return pretty_name;
}
#else
	template<typename T>
	static constexpr auto name_pretty()
	{
		std::string_view pretty_name = name_detail<T>();
		std::string_view prefix = "const char *__cdecl MetatypeHash::name_detail<";
		std::string_view suffix = ">(void)";
		pretty_name.remove_prefix(prefix.size());
		pretty_name.remove_suffix(suffix.size());

		size_t start_of = pretty_name.find_first_of(' ') + 1;
		return pretty_name.substr(start_of);
	}
#endif

	template<typename T>
	static constexpr size_t hash()
	{
		static_assert(!std::is_reference_v<T>, "Dont send reference to hash");
		static_assert(!std::is_const_v<T>, "Dont send const to hash");

		return hash_fnv1a(name_detail<T>());
	}

};


#endif