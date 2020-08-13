#include "Game.h"
#include "Resource_manager.h"
#include <iostream>
#include <GLFW\glfw3.h>
#include "ParticleGenerator.h"
#include "PostProcess.h"
#include "Powerup.h"
#include <algorithm>

GameObject* Player;
Ball* Ballobj;
ParticleGenerator* Particles;
PostProcess* Effects;

GLfloat ShakeTime = 0.0f;

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
	ResourceManager::LoadShader("Shaders/Particle.vs", "Shaders/Particle.fs", nullptr, "particle");
	ResourceManager::LoadShader("Shaders/postprocessing.vs", "Shaders/postprocessing.fs", nullptr, "postprocessing");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite")->Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite")->SetMatrix4("projection", projection);
   
	ResourceManager::GetShader("particle")->Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle")->SetMatrix4("projection", projection);

    // Set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	// Load textures
	
	ResourceManager::LoadTexture("Resources/powerup_chaos.jpg", GL_FALSE, "chaos");
	ResourceManager::LoadTexture("Resources/powerup_confuse.png", GL_TRUE, "confuse");
	ResourceManager::LoadTexture("Resources/powerup_increase.png", GL_FALSE, "increase");
	ResourceManager::LoadTexture("Resources/powerup_passthrough.jpg", GL_FALSE, "passthrough");
	ResourceManager::LoadTexture("Resources/powerup_speed.png", GL_TRUE, "speed");
	ResourceManager::LoadTexture("Resources/powerup_sticky.png", GL_TRUE, "sticky");

	ResourceManager::LoadTexture("Resources/bg.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("Resources/ball.png", GL_TRUE, "ball");
	ResourceManager::LoadTexture("Resources/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("Resources/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("Resources/paddle.png", GL_TRUE, "paddle");

	ResourceManager::LoadTexture("Resources/particle.png", GL_TRUE, "particle");
	
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	Effects = new PostProcess(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);




	// Load levels	
	GameLevel one; one.Load("Resources/Levels/LevelOne", this->Width, this->Height * 0.5);
	GameLevel two; two.Load("Resources/Levels/LevelOne", this->Width, this->Height);
	
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Level = 0;

	glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	//Player->Radius = 25.0f;

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	Ballobj = new Ball(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("ball"));
}

void Game::Update(GLfloat dt)
{
	Ballobj->Move(dt, this->Width);
	

	// Check for collisions
	this->DoCollisions();


	Particles->Update(dt, *Ballobj, 2, glm::vec2(Ballobj->Radius / 2));

	if (Ballobj->Position.y >= this->Height) // Did ball reach bottom edge?
	{
		this->ResetLevel();
		this->ResetPlayer();
	}

	if (ShakeTime > 0.0f)
	{
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = false;
	}

}

Direction VectorDirection(glm::vec2 closest);

void Game::DoCollisions()
{
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ballobj, box);
			if (std::get<0>(collision)) // If collision is true
			{
				// Destroy block if not solid
				if (!box.IsSolid) {
					box.Destroyed = GL_TRUE;
					this->SpawnPowerUps(box);
				}	
				else
				{ // if block is solid, enable shake effect
					ShakeTime = 0.05f;
					Effects->Shake = true;
				}

				// Collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (!(Ballobj->PassThrough && !box.IsSolid)) // don't do collision resolution on non-solid bricks if pass-through activated
				{
					if (dir == LEFT || dir == RIGHT) // Horizontal collision
					{
						Ballobj->Velocity.x = -Ballobj->Velocity.x; // Reverse horizontal velocity
							// Relocate
						GLfloat penetration = Ballobj->Radius - std::abs(diff_vector.x);
						if (dir == LEFT)
							Ballobj->Position.x += penetration; // Move ball to right
						else
							Ballobj->Position.x -= penetration; // Move ball to left;
					}
					else // Vertical collision
					{
						Ballobj->Velocity.y = -Ballobj->Velocity.y; // Reverse vertical velocity
							// Relocate
						GLfloat penetration = Ballobj->Radius - std::abs(diff_vector.y);
						if (dir == UP)
							Ballobj->Position.y -= penetration; // Move ball back up
						else
							Ballobj->Position.y += penetration; // Move ball back down
					}
				}
			}
		}
	}

	for (Powerup& powerUp : this->PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = true;
			if (CheckCollisionAABB(*Player, powerUp))
			{	// collided with player, now activate powerup
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = true;
				powerUp.Activated = true;
			}
		}
	}

	Collision result = CheckCollision(*Ballobj, *Player);
	if (!Ballobj->Stuck && std::get<0>(result))
	{
		// Check where it hit the board, and change velocity based on where it hit the board
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		GLfloat distance = (Ballobj->Position.x + Ballobj->Radius) - centerBoard;
		GLfloat percentage = distance / (Player->Size.x / 2);
		// Then move accordingly
		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ballobj->Velocity;
		Ballobj->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		//Ballobj->Velocity.y = -Ballobj->Velocity.y;
		Ballobj->Velocity.y = -1 * abs(Ballobj->Velocity.y);

		Ballobj->Velocity = glm::normalize(Ballobj->Velocity) * glm::length(oldVelocity);

		Ballobj->Stuck = Ballobj->Sticky;
	}
}

void Game::UpdatePowerUps(float dt)
{
	for (Powerup& powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				// remove powerup from list (will later be removed)
				powerUp.Activated = false;
				// deactivate effects
				if (powerUp.Type == "sticky")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
					{	// only reset if no other PowerUp of type sticky is active
						Ballobj->Sticky = false;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
					{	// only reset if no other PowerUp of type pass-through is active
						Ballobj->PassThrough = false;
						Ballobj->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
					{	// only reset if no other PowerUp of type confuse is active
						Effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
					{	// only reset if no other PowerUp of type chaos is active
						Effects->Chaos = false;
					}
				}
			}
		}
	}
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const Powerup& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}

bool Game::IsOtherPowerUpActive(std::vector<Powerup>& powerUps, std::string type)
{
	for (const Powerup& powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}
	return false;
}

void Game::ActivatePowerUp(Powerup& powerUp)
{
	if (powerUp.Type == "speed")
	{
		Ballobj->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky")
	{
		Ballobj->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		Ballobj->PassThrough = true;
		Ballobj->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = true; // only activate if chaos wasn't already active
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = true;
	}
}

GLboolean Game::CheckCollisionAABB(GameObject& one, GameObject& two) // AABB - AABB collision
{
	// Collision x-axis?
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// Collision y-axis?
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// Collision only if on both axes
	return collisionX && collisionY;
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
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// Add clamped value to AABB_center and we get the value of box closest	to circle
	glm::vec2 closest = aabb_center + clamped;
	// Retrieve vector between center circle and closest point AABB and	check if length <= radius
	difference = closest - center;

	if (glm::length(difference) <= one.Radius)
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}

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



void Game::ProcessInput(GLfloat dt)
{
	if (this->State == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// Move playerboard
		if (this->Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0)
			{
				Player->Position.x -= velocity;
				if (Ballobj->Stuck)
					Ballobj->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ballobj->Stuck)
					Ballobj->Position.x += velocity;
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
			Ballobj->Stuck = false;
	}
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE)
	{
		Effects->BeginRender();

		// Draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f
		);
		// Draw level
		this->Levels[this->Level].Draw(*Renderer);
		//one.Draw(*Renderer);

		Player->Draw(*Renderer);

		for (Powerup& powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);

		Particles->Draw();
		Ballobj->Draw(*Renderer);

		Effects->EndRender();
		Effects->Render(glfwGetTime());
	}
  
}

void Game::ResetLevel()
{
	if (this->Level == 0)this->Levels[0].Load("res/levels/one.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 1)
		this->Levels[1].Load("res/levels/two.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 2)
		this->Levels[2].Load("res/levels/three.lvl", this->Width, this->Height * 0.5f);
	else if (this->Level == 3)
		this->Levels[3].Load("res/levels/four.lvl", this->Width, this->Height * 0.5f);

	this->Lives = 3;
}

void Game::ResetPlayer()
{
	// Reset player/ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
	//Ballobj->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
	// Also disable all active powerups
	//Effects->Chaos = Effects->Confuse = GL_FALSE;
	Ballobj->PassThrough = Ballobj->Sticky = GL_FALSE;
	Player->Color = glm::vec3(1.0f);
	Ballobj->Color = glm::vec3(1.0f);
}


bool ShouldSpawn(unsigned int chance)
{
	unsigned int random = rand() % chance;
	return random == 0;
}

void Game::SpawnPowerUps(GameObject& block)
{
	if (ShouldSpawn(75)) // 1 in 75 chance
		this->PowerUps.push_back(
			Powerup("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("speed")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(
			Powerup("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("sticky")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(
			Powerup("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("passthrough")) );
	if (ShouldSpawn(75))
		this->PowerUps.push_back(
			Powerup("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("increase") ));
	if (ShouldSpawn(15)) // negative powerups should spawn more often
		this->PowerUps.push_back(
			Powerup("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("confuse") ));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(
			Powerup("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("chaos")));
}