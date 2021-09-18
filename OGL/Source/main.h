#include <glad/glad.h>
#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3.lib")
#include <iostream>

#include "../Game.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// GLFW function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const int SCREEN_WIDTH = 1280;
// The height of the screen
const int SCREEN_HEIGHT = 720;

Game PathFinder(SCREEN_WIDTH, SCREEN_HEIGHT);