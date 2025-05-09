#include "Main/Entities.hpp"
#include "Main/EntityComponents.hpp"
#include "Graphics/ResourceManager.hpp"

void Entities::CreatePlayer(Manager &manager)
{
	auto& player = manager.AddEntity();

	player.AddComponent<Transform>(glm::vec3(0.0f), glm::vec2(200.0f), 0.0f);
	player.AddComponent<Sprite>(ResourceManager::GetTexture("tired"), glm::vec4(1.0f));
	player.AddComponent<Physic>(glm::vec2(200.0f));
	player.AddComponent<PlayerTag>();
}

void Entities::CreateEnemy(Manager &manager, glm::vec3 pos)
{
	auto& enemy = manager.AddEntity();

	enemy.AddComponent<Transform>(pos, glm::vec2(200.0f), 0.0f);
	enemy.AddComponent<Sprite>(ResourceManager::GetTexture("tired2"), glm::vec4(1.0f));
	enemy.AddComponent<EnemyTag>();
}
