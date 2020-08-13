#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H
#pragma once
#include "Texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Source/Shader.h"



class SpriteRenderer
{

	int spriteWidth, spriteHeight;
	int texWidth, texHeight;

	int tex;

public:
    // Constructor (inits shaders/shapes)
    SpriteRenderer(Shader* shader);
    // Destructor
    ~SpriteRenderer();
    // Renders a defined quad textured with given sprite
    void DrawSprite(Texture2D* texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

	void DrawAnimSprite(Texture2D* texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

	SpriteRenderer(int t, int tW, int tH, int sW, int sH)
		: tex(t), texWidth(tW), texHeight(tH), spriteWidth(sW), spriteHeight(sH)
	{}

	void drawSprite(float posX, float posY, int frameIndex);

private:
    // Render state
    Shader* shader;
    GLuint quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();

	void initAnimRenderData();
};

#endif