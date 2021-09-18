
#ifndef POWER_UP_H
#define POWER_UP_H

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "GameObject.h"

const glm::vec2 POWERUP_SIZE(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.0f);
class Powerup : public GameObject
{
public:
	// PowerUp State
	std::string Type;
	GLfloat Duration;
	GLboolean Activated;
	// Constructor
	Powerup(std::string type, glm::vec3 color, GLfloat duration, glm::vec2 position, Texture2D* texture)
		: GameObject(position, POWERUP_SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated() { }
};

#endif