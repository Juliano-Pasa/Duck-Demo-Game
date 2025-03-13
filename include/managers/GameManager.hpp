#ifndef __GAMEMANAGER_HPP__
#define __GAMEMANAGER_HPP__

#include "states/GameStates.hpp"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstdio>
#include <string>

class GameManager
{
private:
	GameState *currentState;
	GLFWwindow* window;

	void InitializeGLFW();
	void InitializeCallbacks();
	void InitializeGLEW();

	static void ErrorCallback(int error, const char* description);
	static void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
	GameManager();
	void Launch();
};

#endif // __GAMEMANAGER_HPP__