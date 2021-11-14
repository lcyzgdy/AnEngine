#pragma once
#ifndef __STRINGBUILDER_HPP__
#define __STRINGBUILDER_HPP__

#include <string>
#include <list>
#include <numeric>

namespace ThirdParty
{
	// 修改自 https://blog.csdn.net/garfielder007/article/details/54432441
	// 原作者 GarfieldEr007 

	// Subset of http://msdn.microsoft.com/en-us/library/system.text.stringbuilder.aspx
	template <typename _Elem>
	class StringBuilder
	{
		using string_t = std::basic_string<_Elem>;
		using container_t = std::list<string_t>; // Reasons not to use vector below. 
		typedef typename string_t::size_type size_type; // Reuse the size type in the string.
		container_t m_data;
		size_type m_totalSize;

		void Append_Internal(const string_t& src)
		{
			m_data.push_back(src);
			m_totalSize += src.size();
		}
		// No copy constructor, no assignement.
		StringBuilder(const StringBuilder&) = delete;
		StringBuilder& operator = (const StringBuilder&) = delete;
	public:
		StringBuilder(const string_t& src)
		{
			if (!src.empty())
			{
				m_data.push_back(src);
			}
			m_totalSize = src.size();
		}
		StringBuilder()
		{
			m_totalSize = 0;
		}
		// TODO: Constructor that takes an array of strings.


		StringBuilder& Append(const string_t& src)
		{
			Append_Internal(src);
			return *this; // allow chaining.
		}
		// This one lets you add any STL container to the string builder. 
		template<class _InputIteratorType>
		StringBuilder& Add(const _InputIteratorType& first, const _InputIteratorType& afterLast)
		{
			// std::for_each and a lambda look like overkill here.
					// <b>Not</b> using std::copy, since we want to update m_totalSize too.
			for (_InputIteratorType f = first; f != afterLast; ++f)
			{
				Append_Internal(*f);
			}
			return *this; // allow chaining.
		}
		StringBuilder& AppendLine(const string_t& src)
		{
			static _Elem lineFeed[]{ 10, 0 }; // C++ 11. Feel the love!
			m_data.push_back(src + lineFeed);
			m_totalSize += 1 + src.size();
			return *this; // allow chaining.
		}
		StringBuilder& AppendLine()
		{
			static _Elem lineFeed[]{ 10, 0 };
			m_data.push_back(lineFeed);
			++m_totalSize;
			return *this; // allow chaining.
		}

		// TODO: AppendFormat implementation. Not relevant for the article. 

		// Like C# StringBuilder.ToString()
		// Note the use of reserve() to avoid reallocations. 
		string_t ToString() const
		{
			string_t result;
			// The whole point of the exercise!
			// If the container has a lot of strings, reallocation (each time the result grows) will take a serious toll,
			// both in performance and chances of failure.
			// I measured (in code I cannot publish) fractions of a second using 'reserve', and almost two minutes using +=.
			result.reserve(m_totalSize + 1);
			//	result = std::accumulate(m_Data.begin(), m_Data.end(), result); // This would lose the advantage of 'reserve'
			/*for (auto iter = m_data.begin(); iter != m_data.end(); ++iter)
			{
				result += *iter;
			}
			return result;*/

			for (auto&& con : m_data)
			{
				result += con;
			}
			return result;
		}

		// like javascript Array.join()
		string_t Join(const string_t& delim) const
		{
			if (delim.empty())
			{
				return ToString();
			}
			string_t result;
			if (m_data.empty())
			{
				return result;
			}
			// Hope we don't overflow the size type.
			size_type st = (delim.size() * (m_data.size() - 1)) + m_totalSize + 1;
			result.reserve(st);
			// If you need reasons to love C++11, here is one.
			struct adder
			{
				string_t m_Joiner;
				adder(const string_t& s) : m_Joiner(s)
				{
					// This constructor is NOT empty.
				}
				// This functor runs under accumulate() without reallocations, if 'l' has reserved enough memory. 
				string_t operator()(string_t& l, const string_t& r)
				{
					l += m_Joiner;
					l += r;
					return l;
				}
			} adr(delim);
			auto&& iter = m_data.begin();
			// Skip the delimiter before the first element in the container.
			result += *iter;
			return std::accumulate(++iter, m_data.end(), result, adr);
		}

	}; // class StringBuilder
}
#endif // !__STRINGBUILDER_HPP__
