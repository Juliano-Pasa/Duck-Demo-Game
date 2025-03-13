#ifndef __DUCKPLAYERCONTROLLER_HPP__
#define __DUCKPLAYERCONTROLLER_HPP__

#include "glslprogram.hpp"
#include "controllers/EntityController.hpp"
#include "managers/InputManager.hpp"
#include "entities/Camera.hpp"

class DuckPlayerControler : public EntityControler
{
	InputManager* inputManager;
	void ReadKeyboardInputs();
	void ReadMouseInputs();

	bool holdRotation;
	bool firstMouseMove;
	dvec2 lastMouseCoords;

	vec3 newForce;
	vec3 resultingForce;
	float maxForce;
	float movementStrength;

	float maxSpeed;
	float baseSpeed;
	float sprintSpeed;
	vec3 currentSpeed;

	float mass;

	bool wobble;
	float wobbleAngle;
	float maxWobble;
	float wobbleSpeed;

	vec3 orientation;
	vec3 sideOrientation;

	Camera* camera;

	void CalculateOrientation();
	void ApplyForces(float deltaTime);
	void ApplyWobble(float deltaTime);
	vec3 TruncateMagnitude(vec3 vec, float maxMagnitude);

public:
	DuckPlayerControler();

	void Initialize(InputManager* inputManager, Camera* camera, float maxForce, float movementStrength, float baseSpeed, float sprintSpeed, float mass);
	void Update(float deltaTime);
};

#endif // __DUCKPLAYERCONTROLLER_HPP__