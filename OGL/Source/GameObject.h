#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "texture.h"
#include "SpriteRenderer.h"

class GameObject
{
    public:
        // Object state
        glm::vec2   Position, Size, Velocity;
        glm::vec3   Color;
        GLfloat     Rotation;
		GLfloat		Radius;
        GLboolean   IsSolid;
        GLboolean   Destroyed;
        // Render state
        Texture2D*   Sprite;
        // Constructor(s)
        GameObject();
        GameObject(glm::vec2 pos, glm::vec2 size, Texture2D* sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
        // Draw sprite
        virtual void Draw(SpriteRenderer& renderer);
};


#endif