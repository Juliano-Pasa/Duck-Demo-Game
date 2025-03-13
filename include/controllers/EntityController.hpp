#ifndef __ENTITYCONTROLER_HPP__
#define __ENTITYCONTROLER_HPP__

#include "entities/Entity.hpp"
#include <vector>

using namespace std;

class EntityControler
{
protected:
	Entity* entity;
	bool initialized;

public:
	EntityControler();
	void SetEntity(Entity* entity);

	virtual void Update(float deltaTime) = 0;
};

#endif // __ENTITYCONTROLER_HPP__