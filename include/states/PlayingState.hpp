#ifndef __PLAYINGSTATE_HPP__
#define __PLAYINGSTATE_HPP__

#include "glad.h"
#include <GLFW/glfw3.h>

#include "states/GameState.hpp"
#include "managers/CollisionManager.hpp"
#include "entities/Camera.hpp"
#include "terrain/TerrainV2.hpp"
#include "entities/Wind.hpp"
#include "entities/Entity.hpp"
#include "entities/WorldLight.hpp"
#include "managers/InputManager.hpp"

#include <vector>

using namespace std;

class PlayingState : public GameState
{
private:
	Camera* camera;
	TerrainV2* terrainV2;
	WorldLight* worldLight;

	vector<Entity*> entities;
	bool loaded;
	bool wireframe;
	bool hiddenCursor;

	float windSpawnTime;
	double windSpawnCooldown;
	Wind* wind;

	vec3 goalPosition;
	Entity* player;

	InputManager* inputManager;
	CollisionManager* collisionManager;

	GLFWwindow* window;
	int windowWidth;
	int windowHeight;

	mat4 projectionMatrix;

	void InitializeGL();
	void InitializeTerrain();
	void InitializeInputManager(GLFWwindow* window);
	void InitializeBots(WorldLight* worldLight, vec3 position);
	Entity* InitializePlayer(InputManager* inputManager, Camera* camera, WorldLight* worldLight, vec3 position);

	void SpawnNewWind();
	void ReadKeyboardInput();

public:
	PlayingState(GLFWwindow* window);

	void OnStart();
	void OnPlay();
	void OnPause();
	void OnResume();
	void OnExit();
};

#endif // __PLAYINGSTATE_HPP__
