#pragma once
#include <glad\glad.h>
#include <vector>
#include "SpriteRenderer.h"
#include "GameLevel.h"
#include "Ball.h"
#include "Powerup.h"

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



typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;

class Game
{
    public:
        // Game state
        GameState  State;
        GLboolean  Keys[1024];
        GLuint	   Width, Height;

		GLuint Lives;

		std::vector<Powerup>  PowerUps;

        // Constructor/Destructor
        Game(GLuint width, GLuint height);
        ~Game();
        // Initialize game state (load all shaders/textures/levels)
        void Init();
        // GameLoop
        void ProcessInput(GLfloat dt);
        void Update(GLfloat dt);
        void Render();

		void ResetLevel();

		void ResetPlayer();

		void DoCollisions();

		void ActivatePowerUp(Powerup& powerUp);

		Collision CheckCollision(GameObject& one, GameObject& two);

		GLboolean CheckCollisionAABB(GameObject& one, GameObject& two);

		void SpawnPowerUps(GameObject& block);
		
		bool IsOtherPowerUpActive(std::vector<Powerup>& powerUps, std::string type);

		void UpdatePowerUps(float dt);

    private :
		std::vector<GameLevel> Levels;
		GLuint                 Level;

        SpriteRenderer* Renderer;
		//const GLfloat Player_Radius = 12.5f;

};

