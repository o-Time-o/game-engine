#pragma once

#include "ECS/Manager.hpp"
#include <glm/glm.hpp>
#include "Graphics/Texture.hpp"

struct PlayerTag {};
struct EnemyTag {};

struct Transform
{
	glm::vec3 pos;
	float rotate;
	glm::vec2 size;

	Transform() {}
	Transform(glm::vec3 pos, glm::vec2 size, float rotate) : pos(pos), size(size), rotate(rotate) {}
};

struct Physic
{
	glm::vec2 vel;

	Physic() {}
	Physic(glm::vec2 vel) : vel(vel) {}
};

struct Sprite
{
	Texture2D texture;
	glm::vec4 color;

	Sprite() {}
	Sprite(Texture2D texture, glm::vec4 color) : texture(texture), color(color) {}
};

using AllComponents = std::tuple<
	PlayerTag, EnemyTag,
	Transform, Sprite, Physic
>;

template <typename... Ts>
void RegisterComponentsFromTuple(Manager& manager, std::tuple<Ts...>)
{
    (manager.RegisterComponent<Ts>(), ...);
}

namespace Components
{
	inline void RegisterAll(Manager &manager)
	{
		RegisterComponentsFromTuple(manager, AllComponents{});
	}
}
