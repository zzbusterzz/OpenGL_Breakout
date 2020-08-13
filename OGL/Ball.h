#pragma once
#include "GameObject.h";

class Ball : public GameObject
{
public:
	// Ball state
	GLfloat Radius;
	GLboolean Stuck;
	Ball();
	Ball(glm::vec2 pos, GLfloat radius, glm::vec2 velocity,Texture2D sprite);
	glm::vec2 Move(GLfloat dt, GLuint window_width);
	void Reset(glm::vec2 position, glm::vec2 velocity);
};