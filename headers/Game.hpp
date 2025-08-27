#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <box2d/box2d.h>

enum GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

struct Input
{
	bool left  = false;
    bool right = false;
    bool up  = false;
    bool down  = false;
};

class Game
{
    public:
        GameState state;
        unsigned int width, height;
		Input input;
		entt::registry registry;
		b2WorldId world;

        Game(unsigned int width, unsigned int height);
        ~Game();

        void Init();

        void ProcessInput(float dt);
        void Update(float dt, sf::RenderWindow &window);
        void Render(sf::RenderWindow &window);
		void RenderUI(float dt, double fps);
};

#endif
