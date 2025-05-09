#pragma once

#include "glm/glm.hpp"
#include "ECS/Manager.hpp"

namespace Entities
{
	void CreatePlayer(Manager &manager);
	void CreateEnemy(Manager &manager, glm::vec3 pos);
}
