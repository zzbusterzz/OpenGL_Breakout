#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "../Game.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// GLFW function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const int SCREEN_WIDTH = 800;
// The height of the screen
const int SCREEN_HEIGHT = 600;

Game PathFinder(SCREEN_WIDTH, SCREEN_HEIGHT);