#pragma once

#include "EntityComponents.hpp"

b2Vec2 PixelsToMeters(const sf::Vector2i& position);
b2Vec2 PixelsToMeters(const sf::Vector2f& position);
sf::Vector2f MetersToPixels(const b2Vec2& position);
sf::Vector2f GetPixelCenter(const sf::Vector2i& localPixelPosition);
sf::Vector2f CalculateLocalCenterOfMass(const std::vector<Pixel>& pixels);
void RebasePixelsToNewCenter(std::vector<Pixel>& pixels, const sf::Vector2f& newCenter);

void SetRigidBodyVertices(sf::VertexArray& vertices, const sf::Vector2f& center, const sf::Color& color, std::uint8_t& index);
void RebuildVertexArray(RigidBody& rb);

bool IsPolygonInRadius(const b2Polygon& poly, const b2Transform& xfPoly, const b2Vec2& center, float radius);
bool IsRigidBodyHit(b2BodyId body, const b2Vec2& position);
bool IsRigidBodyHitWithRadius(b2BodyId body, const b2Vec2& position, const float& radius);
void RemovePixelsInRadius(std::vector<Pixel>& pixels, const b2Transform& bodyTransform, const b2Vec2& position, const float& radius);

std::vector<sf::Vector2i> GetPixelNeighbors(const sf::Vector2i& pos);
std::vector<std::vector<Pixel>> GetPixelBodies(const std::vector<Pixel>& pixels);
std::vector<Pixel> ConvertImageToPixels(const std::string& filename, const sf::Color& ignoreColor = sf::Color::Transparent);
