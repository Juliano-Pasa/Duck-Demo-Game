#ifndef __DUCKBOTCONTROLLER_HPP__
#define __DUCKBOTCONTROLLER_HPP__

#include "glslprogram.hpp"
#include "controllers/EntityController.hpp"
#include "entities/Wind.hpp"

enum DuckBotState
{
	WANDERING,
	SEEKING
};

class DuckBotControler : public EntityControler
{
private:
	vec3 resultingForce;
	float maxForce;
	float movementStrength;

	float maxSpeed;
	float baseSpeed;
	float sprintSpeed;
	vec3 currentSpeed;

	float mass;
	vec3 orientation;
	vec3 goalPosition;

	bool wobble;
	float wobbleAngle;
	float maxWobble;
	float wobbleSpeed;

	float wanderAngle;
	float wanderDistance;
	float wanderRadius;

	Wind* wind;
	float maxDistanceFromWind;
	DuckBotState currentState;

	float currentSeekTime;
	float seekDuration;

	void CalculateOrientation();
	vec3 TruncateMagnitude(vec3 vec, float maxMagnitude);
	void ApplyForces(float deltaTime);
	void ApplyWobble(float deltaTime);

	vec3 Wander();
	vec3 Seek();

	vec3 CalculateCurrentForce();
	DuckBotState GetCurrentState(float deltaTime);

public:
	DuckBotControler();

	void Initialize(float maxForce, float movementStrength, float baseSpeed, float sprintSpeed, float mass, vec3 goalPosition, Wind* wind);
	void Update(float deltaTime);
};

#endif // __DUCKBOTCONTROLLER_HPP__
