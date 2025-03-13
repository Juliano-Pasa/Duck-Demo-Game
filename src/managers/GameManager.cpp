#include "managers/GameManager.hpp"
#include "GLUtils.hpp"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

GameManager::GameManager()
{
	currentState = nullptr;
	window = nullptr;
}

void GameManager::Launch()
{
	InitializeGLFW();
	InitializeCallbacks();
	InitializeGLAD();

	currentState = new PlayingState(window);
	currentState->OnStart();
}

#pragma region InitializingFunctions

void GameManager::InitializeGLFW()
{
	assert(glfwInit() && "GLFW was unable to initialize.");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	//window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "glsl40_toon", glfwGetPrimaryMonitor(), NULL);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "glsl40_toon", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		assert(false && "Failed to create GLFW window.");
	}
}

void GameManager::InitializeCallbacks()
{
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyboardCallback);
	glfwSetErrorCallback(ErrorCallback);
}

void GameManager::InitializeGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		assert(false && "GLAD was unable to load OpenGL.");
	}

	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	GLUtils::dumpGLInfo();
}

#pragma endregion


#pragma region CallbackFunctions

void GameManager::ErrorCallback(int error, const char* description)
{
	fputs(description, stderr);
}

void GameManager::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

#pragma endregion


