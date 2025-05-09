#include "Main/Game.hpp"
#include "Main/Systems.hpp"
#include "Main/Entities.hpp"
#include "Main/EntityComponents.hpp"
#include "Graphics/ResourceManager.hpp"

#include "imgui/imgui.h"

SpriteRenderer *Renderer;
Manager manager;

Game::Game(unsigned int width, unsigned int height) : state(GAME_ACTIVE), keys(), width(width), height(height)
{
}

Game::~Game()
{
	delete Renderer;
}

void Game::Init()
{
	ResourceManager::LoadShader("resources/shaders/default.vert", "resources/shaders/default.frag", nullptr, "default");
	glm::mat4 projection = ResourceManager::SetProjection(static_cast<float>(this->width), static_cast<float>(this->height));

    ResourceManager::GetShader("default").Use().SetInteger("texture0", 0);
    ResourceManager::GetShader("default").SetMatrix4("projection", projection);

	Shader shader = ResourceManager::GetShader("default");
    Renderer = new SpriteRenderer(shader);

    ResourceManager::LoadTexture("resources/textures/image.jpg", false, "tired");
	ResourceManager::LoadTexture("resources/textures/image2.jpg", false, "tired2");

	Components::RegisterAll(manager);

	Entities::CreatePlayer(manager);
}

void Game::Update(float dt)
{
	if(state == GAME_ACTIVE)
	{
		bool moveKeys[4] = {
			keys[GLFW_KEY_A],
			keys[GLFW_KEY_D],
			keys[GLFW_KEY_W],
			keys[GLFW_KEY_S]
		};

		PlayerMovementSystem(manager, moveKeys, dt);
	}
}

void Game::ProcessInput(float dt)
{
	if(state == GAME_ACTIVE)
	{
		// if(mouse[GLFW_MOUSE_BUTTON_RIGHT])
		// {
		// 	DeleteAllSystem(manager);
		// }

		if(mouse[GLFW_MOUSE_BUTTON_LEFT])
		{
			Entities::CreateEnemy(manager, glm::vec3(cursor, 0.0f));
		}
	}
}

void Game::Render()
{
	RenderSystem(manager, Renderer);
}

void Game::RenderUI(float dt, double fps)
{
	ImGui::Begin("STATS");
	ImGui::Text("Delta Time: %.4f", dt);
	ImGui::Text("FPS: %.1f", fps);
	ImGui::Text("Objects: %zu/%zu", manager.GetAliveEntityCount(), static_cast<size_t>(MAX_ENTITIES));
	ImGui::End();
}
