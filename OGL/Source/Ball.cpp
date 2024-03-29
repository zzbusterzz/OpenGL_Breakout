#include "Ball.h"

Ball::Ball()
	: GameObject(), Radius(12.5f), Stuck(GL_TRUE), Sticky(GL_FALSE), PassThrough(GL_FALSE) { }

Ball::Ball(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D* sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(GL_TRUE), Sticky(GL_FALSE), PassThrough(GL_FALSE) { }

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

void Ball::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Velocity = velocity;
	this->Stuck = GL_TRUE;
	this->Sticky = GL_FALSE;
	this->PassThrough = GL_FALSE;
}