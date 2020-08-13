#pragma once
#include <glad\glad.h>
#include <vector>
#include "SpriteRenderer.h"
#include "GameLevel.h"
#include "Ball.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

const glm::vec2 PLAYER_SIZE(100, 20);

const GLfloat PLAYER_VELOCITY(500.0f);

GameObject* Player;

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;

Ball *Ballobj;


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

		void DoCollisions();

		Collision CheckCollision(GameObject &one, GameObject &two);


    private :
		std::vector<GameLevel> Levels;
		GLuint                 Level;

        SpriteRenderer* Renderer;
		//const GLfloat Player_Radius = 12.5f;

};

