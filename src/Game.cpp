#include "Game.hpp"
#include <imgui.h>
#include "Entities.hpp"
#include "EntityComponents.hpp"
#include "Systems.hpp"
#include "PixelOperations.hpp"
#include "Constants.hpp"

bool showColliders = false;

Game::Game(unsigned int width, unsigned int height) : state(GAME_ACTIVE), width(width), height(height)
{
}

Game::~Game()
{
	b2DestroyWorld(world);
	registry.clear();
}

void Game::Init()
{
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = {0.f, 0.f};
	world = b2CreateWorld(&worldDef);

	CreateGrounds(world, width, height);

	std::vector<Pixel> block = ConvertImageToPixels("resources/textures/box.png");
	auto player = CreateRigidBody(registry, world, std::move(block), {800.f, 200.f});
	registry.emplace<Velocity>(player, sf::Vector2f{200.f, 200.f});
	registry.emplace<PlayerTag>(player);
}

void Game::Update(float dt, sf::RenderWindow &window)
{
	if(this->state == GAME_ACTIVE)
	{
		b2World_Step(world, 1/60.f, 8);

		if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			System::DestructionSystem(registry, world, PixelsToMeters(mouse), 10.f / SCALE);
		}

		System::Movement(registry, input);
	}
}

void Game::ProcessInput(float dt)
{
	if(this->state == GAME_ACTIVE)
	{
		input.left  = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
		input.right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
		input.up  = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
		input.down  = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
	}
}

void Game::Render(sf::RenderWindow &window)
{
	System::RenderRigidBody(registry, window, showColliders);
}

void Game::RenderUI(float dt, double fps)
{
	ImGui::Begin("Debug");
	ImGui::Text("Delta Time: %.4f", dt);
	ImGui::Text("FPS: %.1f", fps);
	ImGui::Checkbox("Show Colliders", &showColliders);
	ImGui::Text("New Game");
	ImGui::End();
}
