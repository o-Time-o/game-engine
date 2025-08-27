#include "Systems.hpp"
#include "Constants.hpp"
#include "Entities.hpp"
#include "EntityComponents.hpp"
#include "PixelOperations.hpp"
#include <iostream>

void DrawRigidBodyColliders(b2BodyId body, sf::RenderWindow &window) {
    int shapeCount = b2Body_GetShapeCount(body);
    if (shapeCount == 0) return;

    std::vector<b2ShapeId> shapes(shapeCount);
    b2Body_GetShapes(body, shapes.data(), shapeCount);

    b2Transform xf = b2Body_GetTransform(body);
    sf::Transform sfXf(
        xf.q.c, -xf.q.s, xf.p.x * SCALE,
        xf.q.s,  xf.q.c, xf.p.y * SCALE,
        0.f,    0.f,    1.f
    );

    for (b2ShapeId shapeId : shapes) {
        b2Polygon polygon = b2Shape_GetPolygon(shapeId);

        sf::VertexArray shapeVerts(sf::PrimitiveType::LineStrip, polygon.count + 1);

        for (int i = 0; i < polygon.count; ++i) {
            sf::Vector2f point = MetersToPixels(polygon.vertices[i]);
            shapeVerts[i].position = point;
            shapeVerts[i].color = sf::Color::Green;
        }
        // Close the loop
        shapeVerts[polygon.count].position = MetersToPixels(polygon.vertices[0]);
        shapeVerts[polygon.count].color = sf::Color::Green;

        window.draw(shapeVerts, sfXf);
    }
}

void System::RenderRigidBody(entt::registry& registry, sf::RenderWindow& window, bool showColliders) {
	registry.view<RigidBody>().each([&](RigidBody& rb) {
		b2Transform b2Transform = b2Body_GetTransform(rb.body);

		sf::Transform sfTransform(
			b2Transform.q.c, -b2Transform.q.s, b2Transform.p.x * SCALE,
			b2Transform.q.s,  b2Transform.q.c, b2Transform.p.y * SCALE,
			0.f, 0.f, 1.f
		);

		window.draw(rb.vertices, sfTransform);
		if(showColliders) DrawRigidBodyColliders(rb.body, window);
	});
}

void System::DestructionSystem(entt::registry& registry, b2WorldId world, const b2Vec2& explosionCenter, const float& radius) {
	registry.view<RigidBody>().each([&](entt::entity entity, RigidBody& rb) {
		if(!IsRigidBodyHitWithRadius(rb.body, explosionCenter, radius)) return;

		b2Transform transform = b2Body_GetTransform(rb.body);
		RemovePixelsInRadius(rb.pixels, transform, explosionCenter, radius);
		std::vector<std::vector<Pixel>> bodies = GetPixelBodies(rb.pixels);

		if(bodies.empty()) {
            b2DestroyBody(rb.body);
            registry.destroy(entity);
            return;
        }

		auto largestBody = std::max_element(bodies.begin(), bodies.end(), [](const auto& a, const auto& b) {
			return a.size() < b.size();
		});
		rb.pixels = std::move(*largestBody);
		bodies.erase(largestBody);

		for(const std::vector<Pixel>& body : bodies) {
			for(const Pixel& pixel : body) {
				b2DestroyShape(pixel.shapeId, true);
			}
		}

		b2Body_ApplyMassFromShapes(rb.body);
		RebuildVertexArray(rb);

		CreateFragmentBodies(registry, world, bodies, transform, rb.body);
	});
}

void System::Movement(entt::registry& registry, const Input& in) {
	auto view = registry.view<RigidBody, Velocity, PlayerTag>();
	view.each([&](auto &rb, auto &vel) {
		vel.vel = {0.f, 0.f};
		if(in.left) vel.vel.x = -500.f;
		if(in.right) vel.vel.x = 500.f;
		if(in.up) vel.vel.y = -500.f;
		if(in.down) vel.vel.y = 500.f;
		b2Body_SetLinearVelocity(rb.body, { vel.vel.x / SCALE, vel.vel.y / SCALE });
	});
}
