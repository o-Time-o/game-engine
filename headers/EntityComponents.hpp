#pragma once

#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

//STRUCTS
struct Pixel
{
	sf::Vector2i localPos;
	sf::Color color;
	b2ShapeId shapeId;

	Pixel() = default;
	Pixel(sf::Vector2i localPos, sf::Color color) : localPos(localPos), color(color) {}
};

struct Vector2iHash
{
    std::size_t operator()(const sf::Vector2i& p) const
	{
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};

using ColorMap = std::unordered_map<sf::Vector2i, sf::Color, Vector2iHash>;


//COMPONENTS
struct PlayerTag{};

struct RigidBody
{
	b2BodyId body;
	std::vector<Pixel> pixels;
	sf::VertexArray vertices;
};

struct Velocity
{
	sf::Vector2f vel;
};
