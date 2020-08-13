#include "Ball.h"

Ball::Ball(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
{
}

glm::vec2 Ball::Move(GLfloat dt, GLuint window_width)
{
	// If not stuck to player board
	if (!this->Stuck)
	{
		// Move the ball
		this->Position += this->Velocity * dt;
		// Check if outside window bounds; if so, reverse velocity and restore at correct position
			if (this->Position.x <= 0.0f)
			{
				this->Velocity.x = -this->Velocity.x;
				this->Position.x = 0.0f;
			}
			else if (this->Position.x + this->Size.x >= window_width)
			{
				this->Velocity.x = -this->Velocity.x;
				this->Position.x = window_width - this->Size.x;
			}
		if (this->Position.y <= 0.0f)
		{
			this->Velocity.y = -this->Velocity.y;
			this->Position.y = 0.0f;
		}
	}
	return this->Position;
}