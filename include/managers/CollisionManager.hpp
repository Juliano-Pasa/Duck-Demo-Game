#ifndef __COLLISIONMANAGER_HPP__
#define __COLLISIONMANAGER_HPP__

#include "glslprogram.hpp"
#include "entities/Entity.hpp"
#include "terrain/TerrainV2.hpp"
#include <vector>

using namespace std;

class CollisionManager
{
private:
	Entity* referenceEntity;
	vector<Entity*> entities;

	TerrainV2* terrain;
	vector<vec3> nearbyPatches;

public:
	CollisionManager(vector<Entity*> entities, Entity* referenceEntity, TerrainV2* terrain);

	void CheckCollisions(bool nearby);
	vec3 InterpolatePosition(vec3 position, vec3 p0, vec3 p1, vec3 p2, vec3 p3);
};

#endif // __COLLISIONMANAGER_HPP__