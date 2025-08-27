#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include "Game.hpp"

const unsigned int WIDTH_SIZE = 1920;
const unsigned int HEIGHT_SIZE = 1080;
const float TARGET_ASPECT_RATIO = static_cast<float>(WIDTH_SIZE) / HEIGHT_SIZE;

void UpdateAspectRatio(sf::RenderWindow& window, sf::View& view);

Game game(WIDTH_SIZE, HEIGHT_SIZE);

int main() {
    sf::RenderWindow window(sf::VideoMode({WIDTH_SIZE, HEIGHT_SIZE}), "Game", sf::Style::Default, sf::State::Fullscreen);
	window.setFramerateLimit(60);

	sf::View view(sf::FloatRect({0.f, 0.f}, {WIDTH_SIZE, HEIGHT_SIZE}));
    window.setView(view);

	if(!ImGui::SFML::Init(window)) {
		std::cerr << "Failed to initialize ImGui-SFML\n";
		return 1;
	}

	sf::Clock deltaClock;

	float deltaTime = 0.f;
    float fps = 0.f;

	game.Init();

    while(window.isOpen())
	{
		sf::Time dt = deltaClock.restart();
        deltaTime = dt.asSeconds();
        fps = 1.f / deltaTime;

		while(const auto event = window.pollEvent())
        {
			ImGui::SFML::ProcessEvent(window, *event);
            if(event->is<sf::Event::Closed>())
                window.close();

			if (const auto* resized = event->getIf<sf::Event::Resized>())
			{
				UpdateAspectRatio(window, view);
				window.setView(view);
			}

			game.ProcessInput(deltaTime);
        }

		game.Update(deltaTime, window);

		ImGui::SFML::Update(window, dt);
        game.RenderUI(deltaTime, fps);

		window.clear();

		game.Render(window);
		ImGui::SFML::Render(window);

        window.display();
    }

	ImGui::SFML::Shutdown();
    return 0;
}

void UpdateAspectRatio(sf::RenderWindow& window, sf::View& view) {
    float windowAspectRatio = static_cast<float>(window.getSize().x) / window.getSize().y;
    float viewWidth, viewHeight;
    sf::Vector2f viewSize(WIDTH_SIZE, HEIGHT_SIZE);

    if (windowAspectRatio > TARGET_ASPECT_RATIO) {
        viewHeight = HEIGHT_SIZE;
        viewWidth = viewHeight * windowAspectRatio;
        viewSize.x = viewWidth;
        view.setViewport(sf::FloatRect({(1.f - TARGET_ASPECT_RATIO / windowAspectRatio) / 2.f, 0.f}, {TARGET_ASPECT_RATIO / windowAspectRatio, 1.f}));
    } else {
        viewWidth = WIDTH_SIZE;
        viewHeight = viewWidth / windowAspectRatio;
        viewSize.y = viewHeight;
        view.setViewport(sf::FloatRect({0.f, (1.f - windowAspectRatio / TARGET_ASPECT_RATIO) / 2.f}, {1.f, windowAspectRatio / TARGET_ASPECT_RATIO}));
    }

    view.setSize(viewSize);
    view.setCenter({viewSize.x / 2.f, viewSize.y / 2.f});
}
