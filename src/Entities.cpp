#include "Entities.hpp"
#include "Constants.hpp"
#include "PixelOperations.hpp"
#include "box2d/box2d.h"
#include <cstdint>

entt::entity CreateRigidBody(entt::registry& registry, b2WorldId world, std::vector<Pixel>&& pixels, const sf::Vector2f& position, const b2Rot& rotation, const b2Vec2& velocity, const float& angularVelocity) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = PixelsToMeters(position);
	bodyDef.rotation = rotation;
	bodyDef.linearVelocity = velocity;
	bodyDef.angularVelocity = angularVelocity;

	b2BodyId body = b2CreateBody(world, &bodyDef);

	b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.3f;
    shapeDef.material.restitution = 0.1f;

    sf::VertexArray vertices(sf::PrimitiveType::Triangles);
	std::uint8_t index = 0;
	float halfPixel = HALF_PIXEL / SCALE;

	for(Pixel& pixel : pixels) {
		sf::Vector2f pixelCenter = GetPixelCenter(pixel.localPos);

		b2Polygon box = b2MakeOffsetBox(halfPixel, halfPixel, PixelsToMeters(pixelCenter), b2Rot_identity);
		pixel.shapeId = b2CreatePolygonShape(body, &shapeDef, &box);

		SetRigidBodyVertices(vertices, pixelCenter, pixel.color, index);
	}

	entt::entity entity = registry.create();
	registry.emplace<RigidBody>(entity, body, std::move(pixels), std::move(vertices));

	return entity;
}

void CreateFragmentBodies(entt::registry& registry, b2WorldId world, const std::vector<std::vector<Pixel>>& fragments, const b2Transform& parentTransform, b2BodyId parentBody) {
    const b2Vec2 linearVel = b2Body_GetLinearVelocity(parentBody);
    const float angularVel = b2Body_GetAngularVelocity(parentBody);
    const b2Vec2 worldCenter = b2Body_GetWorldCenterOfMass(parentBody);

    for(const auto& fragmentPixels : fragments) {
        const sf::Vector2f localCenter = CalculateLocalCenterOfMass(fragmentPixels);

        const b2Vec2 worldCenter = b2TransformPoint(
            parentTransform,
            PixelsToMeters(localCenter)
        );

        const b2Vec2 r = b2Sub(worldCenter, worldCenter);
        const b2Vec2 tangentialVel = b2CrossSV(angularVel, r);
        const b2Vec2 fragmentVelocity = b2Add(linearVel, tangentialVel);

        std::vector<Pixel> rebasedPixels = fragmentPixels;
        RebasePixelsToNewCenter(rebasedPixels, localCenter);

        CreateRigidBody(registry, world, std::move(rebasedPixels), MetersToPixels(worldCenter), parentTransform.q, fragmentVelocity, angularVel);
    }
}

void CreateGrounds(b2WorldId world, int width, int height) {
	b2BodyDef groundDef = b2DefaultBodyDef();
    groundDef.type = b2_staticBody;
    groundDef.position = {0.f, height / SCALE };
    b2BodyId ground = b2CreateBody(world, &groundDef);

    b2Polygon box = b2MakeBox(10.f / SCALE, height / SCALE);
    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = 1.0f;
    sd.material.friction = .3f;
	sd.material.restitution = .8f;
    b2CreatePolygonShape(ground, &sd, &box);

	b2Polygon box2 = b2MakeBox(width / SCALE, 10.f / SCALE);
    b2CreatePolygonShape(ground, &sd, &box2);

	groundDef.position = {width / SCALE, 0.f};
    b2BodyId ground2 = b2CreateBody(world, &groundDef);
	b2CreatePolygonShape(ground2, &sd, &box);
	b2CreatePolygonShape(ground2, &sd, &box2);
}
