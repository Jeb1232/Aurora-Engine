#pragma once
#include"Entity.h"
class Scene
{
public:

	std::vector<Entity*> sceneObjects;

	void AURORAENGINE_API AddEntity(Entity* ent);
};

