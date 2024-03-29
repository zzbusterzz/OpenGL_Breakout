#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <vector>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "Resource_manager.h"
#include "SpriteRenderer.h"

class GameLevel
{
    public:
        std::vector<GameObject> Bricks;

        GameLevel() { }
        // Loads level from file
        void Load(const GLchar* file, GLuint levelWidth, GLuint levelHeight);
        // Render level
        void Draw(SpriteRenderer& renderer);
        // Check if the level is completed (all non-solid tiles are destroyed)
        GLboolean IsCompleted();
    private:
        // Initialize level from tile data
        void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};
#endif