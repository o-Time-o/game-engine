#ifndef GAME_H
#define GAME_H

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
    public:
        GameState state;
        bool keys[1024];
		bool mouse[8];
		// glm::vec2 cursor;
		// glm::vec2 scroll;
        unsigned int width, height;

        Game(unsigned int width, unsigned int height);
        ~Game();

        void Init();

        void ProcessInput(float dt);
        void Update(float dt);
        void Render();
		void RenderUI(float dt, double fps);
};

#endif
