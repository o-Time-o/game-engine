#pragma once

#include <entt/entt.hpp>
#include "Game.hpp"

namespace System
{
	void RenderRigidBody(entt::registry& registry, sf::RenderWindow& window, bool showColliders);
	void DestructionSystem(entt::registry& registry, b2WorldId world, const b2Vec2& explosionCenter, const float& radius);
	void Movement(entt::registry& registry, const Input& in);
}
