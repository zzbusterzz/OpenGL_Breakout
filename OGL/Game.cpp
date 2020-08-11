#include "Game.h"
#include "Resource_manager.h"
#include <iostream>
#include <GLFW\glfw3.h>


Game::Game(GLuint width, GLuint height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{

}


void Game::Init()
{
    // Load shaders
    ResourceManager::LoadShader("Shaders/sprite.vs", "Shaders/sprite.fs", nullptr, "sprite");
   
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite")->Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite")->SetMatrix4("projection", projection);
   
    // Set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	// Load textures
	ResourceManager::LoadTexture("Resources/bg.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("Resources/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("Resources/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("Resources/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("Resources/player.png", GL_TRUE, "player");
	// Load levels
	
	GameLevel one; one.Load("Resources/Levels/LevelOne", this->Width, this->Height * 0.5);
	GameLevel two; two.Load("Resources/Levels/LevelOne", this->Width, this->Height);
	
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Level = 0;

	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));
	Player->Radius = 12.5f;
}

void Game::Update(GLfloat dt)
{
	// Check for collisions
	this->DoCollisions();
}

void Game::DoCollisions()
{
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Player, box);
			if (std::get<0>(collision)) // If collision is true
			{
				// Destroy block if not solid
				if (!box.IsSolid)
					box.Destroyed = GL_TRUE;
				// Collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT) // Horizontal collision
				{
					Player->Velocity.x = -Player->Velocity.x; // Reverse horizontal velocity
						// Relocate
						GLfloat penetration = Player->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						Player->Position.x += penetration; // Move ball to right
					else
						Player->Position.x -= penetration; // Move ball to left;
				}
				else // Vertical collision
				{
					Player->Velocity.y = -Player->Velocity.y; // Reverse vertical velocity
						// Relocate
						GLfloat penetration = Player->Radius - std::abs(
							diff_vector.y);
					if (dir == UP)
						Player->Position.y -= penetration; // Move ball back up
					else
						Player->Position.y += penetration; // Move ball back down
				}
			}
		}
	}
}

//GLboolean Game::CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
//{
//	// Collision x-axis?
//	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
//		two.Position.x + two.Size.x >= one.Position.x;
//	// Collision y-axis?
//	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
//		two.Position.y + two.Size.y >= one.Position.y;
//	// Collision only if on both axes
//	return collisionX && collisionY;
//}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
	glm::vec2(0.0f, 1.0f), // up
	glm::vec2(1.0f, 0.0f), // right
	glm::vec2(0.0f, -1.0f), // down
	glm::vec2(-1.0f, 0.0f) // left
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

Collision Game::CheckCollision(GameObject& one, GameObject& two) {//Circle - AABB collision
	// Get center point circle first
	glm::vec2 center(one.Position + one.Radius);
	// Calculate AABB info (center, half-extents)
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(
		two.Position.x + aabb_half_extents.x,
		two.Position.y + aabb_half_extents.y
	);
	// Get difference vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents,
		aabb_half_extents);
	// Add clamped value to AABB_center and we get the value of box closest to circle
		glm::vec2 closest = aabb_center + clamped;
	// Retrieve vector between center circle and closest point AABB and check if length <= radius
		difference = closest - center;

		if (glm::length(difference) <= one.Radius)
			return std::make_tuple(GL_TRUE, VectorDirection(difference),
				difference);
		else
			return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}
	//return glm::length(difference) < one.Radius;
//}




void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// Move playerboard
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0)
				Player->Position.x -= velocity;
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
				Player->Position.x += velocity;
		}
		if (this->Keys[GLFW_KEY_W])
		{
			if (Player->Position.y >= 0)
				Player->Position.y -= velocity;
		}
		if (this->Keys[GLFW_KEY_S])
		{
			if (Player->Position.y <= this->Height - Player->Size.x)
				Player->Position.y += velocity;
		}
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		// Draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		// Draw level
		this->Levels[this->Level].Draw(*Renderer);
		//one.Draw(*Renderer);

		Player->Draw(*Renderer);
	}
  
}