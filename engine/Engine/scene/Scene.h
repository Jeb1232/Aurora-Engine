#pragma once
#include"Entity.h"
#include"../renderer/Light.h"
class Scene
{
public:

	std::vector<Entity*> sceneObjects;
	std::vector<Light*> lights;

	void AURORAENGINE_API AddEntity(Entity* ent);
};

