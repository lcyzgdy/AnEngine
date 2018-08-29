#include "TransformSystem.h"

namespace AnEngine::Game
{
	void TransformSystem::Execute(int index)
	{
		var vec = m_data[index]->LocalPosition();
	}
}