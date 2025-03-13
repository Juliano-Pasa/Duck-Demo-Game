#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

#include "entities/Transform.hpp"
#include <vector>

using namespace std;

class Entity
{
private:
	vector<Entity*> children;
	Entity* parent;

public:
	Transform transform;

	Entity(vec3 position, vec3 rotation, vec3 scale);

	void AddChild(Entity* entity);

	void UpdateSelfAndChildren(bool hasParentChanged);

	virtual void Initialize() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void Render(mat4 projection, mat4 view) = 0;
};

#endif