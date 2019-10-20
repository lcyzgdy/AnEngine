#pragma once
#ifndef __ARCHETYPEMANAGER_H__
#define __ARCHETYPEMANAGER_H__

#include "onwind.h"
#include "Archetype.h"

namespace AnEngine
{
	class ArchetypeManager : public Singleton<ArchetypeManager>
	{
		friend class Singleton<ArchetypeManager>;
		std::list<Archetype*> m_archetypeList;
	public:
		template<typename _Ty, typename... _Ty2>
		Archetype* GetArchetype()
		{
			var arche1 = GetArchetype<_Ty>();
			Archetype* arche2;
			if constexpr (sizeof...(_Ty2) == 1)
			{
				arche2 = GetArchetype<_Ty2>();
			}
			else
			{
				arche2 = GetArchetypeRec<_Ty2...>();
			}
			arche1.Combine(arche2);
			delete arche2;

			arche1->Sort();
			var it = std::find(m_archetypeList.begin(), m_archetypeList.end(), [arche1](Archetype* arche) {return arche == arche1});
			if (it == m_archetypeList.end())
			{
				m_archetypeList.push_back(arche1);
				return arche1;
			}
			else
			{
				delete arche1;
				return *it
			}
		}

		template<typename... _Ty, typename... _Ty2>
		Archetype* GetArchetypeRec()
		{
			var arche1 = GetArchetype<_Ty>();
			var arche2 = GetArchetypeReq<_Ty2...>();
			arche1.Combine(arche2);
			delete arche2;
			return arche1
		}

		template<typename _Ty>
		Archetype* GetArchetype()
		{
			return new Archetype({ typeid(_Ty).hash_code() });
		}
	};
}

#endif // !__ARCHETYPEMANAGER_H__
