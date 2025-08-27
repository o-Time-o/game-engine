#pragma once

#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include "EntityComponents.hpp"

entt::entity CreateRigidBody(entt::registry& registry, b2WorldId world, std::vector<Pixel>&& pixels, const sf::Vector2f& position, const b2Rot& rotation = b2MakeRot(0.f), const b2Vec2& velocity = {0.f, 0.f}, const float& angularVelocity = 0.f);
void CreateFragmentBodies(entt::registry& registry, b2WorldId world, const std::vector<std::vector<Pixel>>& fragments, const b2Transform& parentTransform, b2BodyId parentBody);
void CreateGrounds(b2WorldId world, int width, int height);
