#include "states/PlayingState.hpp"
#include "entities/Duck.hpp"
#include "terrain/TerrainGenerator.hpp"
#include "controllers/DuckPlayerController.hpp"
#include "controllers/DuckBotController.hpp"
#include <ctime>
#include <iostream>

char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY)             \
    (glfwGetKey(WINDOW, KEY) ?              \
     (keyOnce[KEY] ? false : (keyOnce[KEY] = true)) :   \
     (keyOnce[KEY] = false))

PlayingState::PlayingState(GLFWwindow* window) : GameState()
{
	loaded = false;
	camera = nullptr;
	wireframe = false;
	hiddenCursor = true;
	entities = vector<Entity*>();

	windSpawnTime = 10.0f;
	windSpawnCooldown = windSpawnTime;

	this->window = window;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
}

#pragma region StateTransitions

void PlayingState::OnStart()
{
	InitializeInputManager(window);

	camera = new Camera(inputManager, vec3(3000, 600, 3000));
	projectionMatrix = glm::perspective(glm::radians(60.0f), (float)windowWidth / (float)windowHeight, 0.1f, 10000.0f);
	worldLight = new WorldLight(vec3(1, 1, 1), vec3(1000, 10000, 1000));

	InitializeGL();
	InitializeTerrain();

	vec3 terrainCenterPosition = terrainV2->CenterPosition();

	int randomRange = 5000;
	int randomX = (rand() % randomRange) - randomRange / 2;
	int randomY = (rand() % randomRange) - randomRange / 2;
	worldLight->transform.position(terrainCenterPosition + vec3(randomX, 10000, randomY));

	player = InitializePlayer(inputManager, camera, worldLight, terrainCenterPosition);
	camera->SetEntityReference(player);

	wind = new Wind(camera->CameraPosition() + vec3(0, 100, 0), vec3(5));
	entities.push_back(wind);
	entities.back()->Initialize();
	terrainV2->AddChild(entities.back());

	InitializeBots(worldLight, terrainCenterPosition);

	vector<Entity*> collidables = vector<Entity*>{ player, entities.back()};
	collisionManager = new CollisionManager(collidables, player, terrainV2);

	loaded = true;
	OnPlay();
}

void PlayingState::OnPlay()
{
	if (!loaded)
	{
		return;
	}

	double deltaTime;
	double lastTime = glfwGetTime();
	do
	{
		ReadKeyboardInput();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		deltaTime = glfwGetTime() - lastTime;
		windSpawnCooldown -= deltaTime;

		if (windSpawnCooldown < 0)
		{
			SpawnNewWind();
		}

		for (Entity* entity : entities)
		{
			entity->Update(deltaTime);
		}

		collisionManager->CheckCollisions(false);
		camera->Update(deltaTime);

		lastTime += deltaTime;

		for (Entity* entity : entities)
		{
			entity->Render(projectionMatrix, camera->viewMatrix);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(window));
}

void PlayingState::OnPause()
{
	if (!loaded)
	{
		return;
	}
}

void PlayingState::OnResume()
{
	if (!loaded)
	{
		return;
	}
}

void PlayingState::OnExit()
{
	if (!loaded)
	{
		return;
	}
}

#pragma endregion


#pragma region InitializingFunctions

void PlayingState::InitializeGL()
{
	glClearColor(0.47055f, 0.55289f, 0.86785f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void PlayingState::InitializeTerrain()
{
	srand(time(NULL));

	int mapSize = 8;
	int randomRange = 1000;
	int seed = rand() % 1000;

	TerrainGenerator terrainGenerator = TerrainGenerator(mapSize, vector<double>{64, 128, 192, 128}, 1024, 0.2f, seed);
	string heightMapPath = "../assets/heightMap.csv";
	string normalMapPath = "../assets/normalMap.csv";

	terrainGenerator.GenerateDiamondSquare();
	terrainGenerator.GenerateNormalMap();
	terrainGenerator.WriteHeightMapToCSV(heightMapPath);
	terrainGenerator.WriteNormalMapToCSV(normalMapPath);
	terrainGenerator.FreeMap();

	int dimensions = (int)pow(2, mapSize) + 1;
	terrainV2 = new TerrainV2(dimensions, dimensions, 256, camera, heightMapPath, normalMapPath);
	terrainV2->worldLight = worldLight;
	terrainV2->Initialize();
	entities.push_back(terrainV2);

	goalPosition = terrainV2->GoalPosition();
}

void PlayingState::InitializeInputManager(GLFWwindow* window)
{
	vector<int> keys = { GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_ESCAPE };
	inputManager = new InputManager(keys, true);
	inputManager->SetupInputs(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Entity* PlayingState::InitializePlayer(InputManager* inputManager, Camera* camera, WorldLight* worldLight, vec3 position)
{
	DuckPlayerControler* controler = new DuckPlayerControler();
	controler->Initialize(inputManager, camera, 1000.0f, 500.0f, 200.0f, 300.0f, 1.0f);

	Duck* duck = new Duck(position, vec3(10), worldLight, controler);
	entities.push_back(duck);
	entities.back()->Initialize();
	terrainV2->AddChild(entities.back());

	return duck;
}

void PlayingState::InitializeBots(WorldLight* worldLight, vec3 position)
{
	DuckBotControler* controler = new DuckBotControler();
	controler->Initialize(1000.0f, 500.0f, 150.0f, 250.0f, 1.0f, goalPosition, wind);

	Duck* duck = new Duck(position, vec3(10), worldLight, controler);
	entities.push_back(duck);
	entities.back()->Initialize();
	terrainV2->AddChild(entities.back());
}

#pragma endregion

void PlayingState::ReadKeyboardInput()
{
	if (glfwGetKeyOnce(window, 'Q')) {
		wireframe = !wireframe;
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}

	if (glfwGetKeyOnce(window, 'Z')) {
		hiddenCursor = !hiddenCursor;
		if (hiddenCursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void PlayingState::SpawnNewWind()
{
	int randomRange = 500;
	int randomX = (rand() % randomRange) - randomRange / 2;
	int randomZ = (rand() % randomRange) - randomRange / 2;
	vec3 positionVector = player->transform.position() + vec3(randomX, 100, randomZ);

	randomRange = 200;
	randomX = (rand() % randomRange) - randomRange / 2;
	randomZ = (rand() % randomRange) - randomRange / 2;
	vec3 directionVector = goalPosition - positionVector;
	float angle = atan2(directionVector.x + randomX, directionVector.z + randomZ);

	wind->SetNewSpawn(positionVector, vec3(0, angle, 0));
	windSpawnCooldown = windSpawnTime;
}
