#pragma once
#include <glad\glad.h>
#include "SpriteRenderer.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
    public:
        // Game state
        GameState  State;
        GLboolean  Keys[1024];
        GLuint	   Width, Height;
        // Constructor/Destructor
        Game(GLuint width, GLuint height);
        ~Game();
        // Initialize game state (load all shaders/textures/levels)
        void Init();
        // GameLoop
        void ProcessInput(GLfloat dt);
        void Update(GLfloat dt);
        void Render();

    private :
        SpriteRenderer* Renderer;
};

