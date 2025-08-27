#include "PixelOperations.hpp"
#include "Constants.hpp"
#include <queue>

b2Vec2 PixelsToMeters(const sf::Vector2i& position) {
    return {
        static_cast<float>(position.x) / SCALE,
        static_cast<float>(position.y) / SCALE
    };
}

b2Vec2 PixelsToMeters(const sf::Vector2f& position) {
    return {position.x / SCALE, position.y / SCALE};
}

sf::Vector2f MetersToPixels(const b2Vec2& position) {
    return sf::Vector2f(position.x * SCALE, position.y * SCALE);
}

sf::Vector2f GetPixelCenter(const sf::Vector2i& localPixelPosition) {
	return {
		localPixelPosition.x * PIXEL_SIZE + HALF_PIXEL,
		localPixelPosition.y * PIXEL_SIZE + HALF_PIXEL
	};
}

sf::Vector2f CalculateLocalCenterOfMass(const std::vector<Pixel>& pixels) {
    sf::Vector2f sum(0.f, 0.f);
    for(const Pixel& pixel : pixels)
        sum += GetPixelCenter(pixel.localPos);

    return sum / static_cast<float>(pixels.size());
}

void RebasePixelsToNewCenter(std::vector<Pixel>& pixels, const sf::Vector2f& newCenter) {
    for(Pixel& pixel : pixels) {
        sf::Vector2f oldPos = GetPixelCenter(pixel.localPos);
        sf::Vector2f rebasedPos = oldPos - newCenter;
        pixel.localPos = {
            static_cast<int>(std::round(rebasedPos.x / PIXEL_SIZE)),
            static_cast<int>(std::round(rebasedPos.y / PIXEL_SIZE))
        };
    }
}

void SetRigidBodyVertices(sf::VertexArray& vertices, const sf::Vector2f& center, const sf::Color& color, std::uint8_t& index) {
    sf::Vector2f topLeft(center.x - HALF_PIXEL, center.y - HALF_PIXEL);
    sf::Vector2f topRight(center.x + HALF_PIXEL, center.y - HALF_PIXEL);
    sf::Vector2f bottomLeft(center.x - HALF_PIXEL, center.y + HALF_PIXEL);
    sf::Vector2f bottomRight(center.x + HALF_PIXEL, center.y + HALF_PIXEL);

    vertices.append(sf::Vertex({topLeft, color}));
    vertices.append(sf::Vertex({bottomLeft, color}));
    vertices.append(sf::Vertex({bottomRight, color}));

    vertices.append(sf::Vertex({topLeft, color}));
    vertices.append(sf::Vertex({bottomRight, color}));
    vertices.append(sf::Vertex({topRight, color}));

    index += 6;
}

void RebuildVertexArray(RigidBody& rb) {
    sf::VertexArray vertices(sf::PrimitiveType::Triangles);
    std::uint8_t index = 0;

    for (const Pixel& pixel : rb.pixels) {
        sf::Vector2f pixelCenter = GetPixelCenter(pixel.localPos);
        SetRigidBodyVertices(vertices, pixelCenter, pixel.color, index);
    }

    rb.vertices = std::move(vertices);
}

bool IsPolygonInRadius(const b2Polygon& poly, const b2Transform& transformPoly, const b2Vec2& center, float radius) {
    b2Circle circle = { center, radius };
    b2Transform transformCircle = b2Transform_identity;

    b2Manifold manifold = b2CollidePolygonAndCircle(&poly, transformPoly, &circle, transformCircle);
    return manifold.pointCount > 0;
}

bool IsRigidBodyHit(b2BodyId body, const b2Vec2& position) {
	int shapeCount = b2Body_GetShapeCount(body);
    if(shapeCount == 0) return false;

    std::vector<b2ShapeId> shapes(shapeCount);
    b2Body_GetShapes(body, shapes.data(), shapeCount);

    for(b2ShapeId shapeId : shapes)
        if(b2Shape_TestPoint(shapeId, position)) return true;

	return false;
}

bool IsRigidBodyHitWithRadius(b2BodyId body, const b2Vec2& position, const float& radius) {
    int shapeCount = b2Body_GetShapeCount(body);
    if(shapeCount == 0) return false;

    std::vector<b2ShapeId> shapes(shapeCount);
    b2Body_GetShapes(body, shapes.data(), shapeCount);

	b2Transform transformPoly = b2Body_GetTransform(body);

    for(const auto& shapeId : shapes) {
        if(b2Shape_GetType(shapeId) != b2_polygonShape) continue;
		b2Polygon polygon = b2Shape_GetPolygon(shapeId);
		if(IsPolygonInRadius(polygon, transformPoly, position, radius))
			return true;
    }

    return false;
}

void RemovePixelsInRadius(std::vector<Pixel>& pixels, const b2Transform& bodyTransform, const b2Vec2& position, const float& radius) {
	pixels.erase(std::remove_if(pixels.begin(), pixels.end(), [&](const Pixel& pixel) {
		if(b2Shape_GetType(pixel.shapeId) != b2_polygonShape)
			return false;

		b2Polygon polygon = b2Shape_GetPolygon(pixel.shapeId);
		if(IsPolygonInRadius(polygon, bodyTransform, position, radius)) {
			b2DestroyShape(pixel.shapeId, true);
			return true;
		}
		return false;
	}), pixels.end());
}

std::vector<sf::Vector2i> GetPixelNeighbors(const sf::Vector2i& pos) {
    return {
        { pos.x + 1, pos.y },
        { pos.x - 1, pos.y },
        { pos.x, pos.y + 1 },
        { pos.x, pos.y - 1 }
    };
}

std::vector<std::vector<Pixel>> GetPixelBodies(const std::vector<Pixel>& pixels) {
    std::unordered_map<sf::Vector2i, const Pixel*, Vector2iHash> pixelMap;
    for(const Pixel& px : pixels)
        pixelMap[px.localPos] = &px;

    std::unordered_set<sf::Vector2i, Vector2iHash> visited;
    std::vector<std::vector<Pixel>> bodies;

    for(const auto& [pos, _] : pixelMap) {
        if(visited.count(pos)) continue;

        std::vector<Pixel> group;
        std::queue<sf::Vector2i> q;
        q.push(pos);
        visited.insert(pos);

        while(!q.empty()) {
            sf::Vector2i current = q.front();
			q.pop();
            group.push_back(*pixelMap[current]);

            for(const auto& neighbor : GetPixelNeighbors(current)) {
                if(pixelMap.count(neighbor) && !visited.count(neighbor)) {
                    visited.insert(neighbor);
                    q.push(neighbor);
                }
            }
        }

        bodies.push_back(std::move(group));
    }

    return bodies;
}

std::vector<Pixel> ConvertImageToPixels(const std::string& filename, const sf::Color& ignoreColor) {
	std::vector<Pixel> pixels;
    
    sf::Image image;
    if(!image.loadFromFile(filename))
        throw std::runtime_error("Failed to load image: " + filename);
    
    sf::Vector2u size = image.getSize();
    pixels.reserve(size.x * size.y);
    
    for(unsigned y = 0; y < size.y; ++y) {
        for(unsigned x = 0; x < size.x; ++x) {
            sf::Color color = image.getPixel({x, y});
            
            if(color != ignoreColor)
                pixels.push_back({{static_cast<int>(x), static_cast<int>(y)}, color});
        }
    }
    
    return pixels;
}
