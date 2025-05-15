#include "SFML/Window/Keyboard.hpp"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

static constexpr float SCALE = 30.f;
static constexpr float MOVE_FORCE = 10.f;
static constexpr float JUMP_IMPULSE = 5.f;

struct Transform {
    sf::RectangleShape shape;
};

struct RigidBody {
    b2BodyId body;
};

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Game");
	
	if(!ImGui::SFML::Init(window)) {
		std::cerr << "Failed to initialize ImGui-SFML\n";
		return 1;
	}
	window.setFramerateLimit(60);

	b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.f, 9.8f};
    b2WorldId world = b2CreateWorld(&worldDef);

	entt::registry registry;

    auto entity = registry.create();

	sf::RectangleShape square(sf::Vector2f(50.f, 50.f));
    square.setFillColor(sf::Color::Green);
    square.setOrigin({25.f, 25.f});
    square.setPosition({400.f, 100.f});

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {square.getPosition().x / SCALE, square.getPosition().y / SCALE};
    b2BodyId bodyId = b2CreateBody(world, &bodyDef);

	b2Polygon boxShape = b2MakeBox((square.getSize().x * 0.5f) / SCALE, (square.getSize().y * 0.5f) / SCALE);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.f;
    shapeDef.material.friction = 0.3f;
	shapeDef.material.restitution = 0.0f;
    b2CreatePolygonShape(bodyId, &shapeDef, &boxShape);

    registry.emplace<Transform>(entity, square);
    registry.emplace<RigidBody>(entity, bodyId);

    b2BodyDef groundDef = b2DefaultBodyDef();
    groundDef.position = {400.f / SCALE, 580.f / SCALE};
    b2BodyId groundBody = b2CreateBody(world, &groundDef);

    b2Polygon groundBox = b2MakeBox(400.f / SCALE, 10.f / SCALE);
    b2CreatePolygonShape(groundBody, &shapeDef, &groundBox);

	sf::Clock deltaClock;

	float deltaTime = 0.f;
    float fps = 0.f;

    while(window.isOpen())
	{
		sf::Time dt = deltaClock.restart();
        deltaTime = dt.asSeconds();
        fps = 1.f / deltaTime;

		while(const auto event = window.pollEvent())
        {
			ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>())
                window.close();
        }

		bool moveLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
        bool moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
        bool jump = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

		auto [xf, rb] = registry.get<Transform, RigidBody>(entity);
        if (moveLeft) {
			b2Body_ApplyForceToCenter(rb.body, {-MOVE_FORCE, 0.f}, true);
		}
		if (moveRight) {
			b2Body_ApplyForceToCenter(rb.body, { MOVE_FORCE, 0.f}, true);
		}

		if (jump) {
			b2Vec2 vel = b2Body_GetLinearVelocity(rb.body);
			if (std::abs(vel.y) < 0.01f) {
				b2Body_ApplyLinearImpulseToCenter(rb.body, {0.f, - JUMP_IMPULSE}, true);
			}
		}

		ImGui::SFML::Update(window, dt);
        ImGui::Begin("Debug");
        ImGui::Text("Delta Time: %.4f", deltaTime);
        ImGui::Text("FPS: %.1f", fps);
        ImGui::Text("Use A/D to move, Space to jump");
        ImGui::End();

		b2World_Step(world, 1.f/60.f, 8);

		registry.view<Transform, RigidBody>().each([&](auto &xf, auto &rb){
            b2Vec2 pos = b2Body_GetPosition(rb.body);
            b2Rot rot = b2Body_GetRotation(rb.body);
            xf.shape.setPosition({pos.x * SCALE, pos.y * SCALE});
            xf.shape.setRotation(sf::degrees(b2Rot_GetAngle(rot)) * 180.f / B2_PI);
        });

		window.clear();

		registry.view<Transform>().each([&](auto& transform) {
            window.draw(transform.shape);
        });

		ImGui::SFML::Render(window);

        window.display();
    }

	b2DestroyWorld(world);
	ImGui::SFML::Shutdown();
    return 0;
}
