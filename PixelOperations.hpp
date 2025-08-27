#pragma once

#include <vector>
#include "EntityComponents.hpp"

b2Vec2 GetPixelRigidBodyPosition(sf::Vector2f position);
Bounds ComputeBounds(const std::vector<Pixel>& pixels);
std::vector<Rect> MergePixelsIntoRects(const std::vector<Pixel>& pixels);
void GetPixelRectVertices(sf::VertexArray &localVertices, const Rect& rect);
b2Vec2 GetRectSize(sf::Vector2i size);
b2Vec2 GetRectCenter(sf::Vector2i position, sf::Vector2i size);
std::vector<std::vector<Pixel>> FindConnectedComponents(const std::vector<Pixel>& pixels);
b2Vec2 CalculateComponentCentroid(const std::vector<Pixel>& component);
std::vector<Pixel> AdjustPixelCoordinates(const std::vector<Pixel>& component, const b2Vec2& centroidPixelSpace);
std::vector<Pixel> ConvertImageToPixels(const std::string& filename, const sf::Color& ignoreColor = sf::Color::Transparent);
