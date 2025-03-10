#include "Scene.h"

void Scene::AddEntity(Entity* ent) {
	bool alreadyInScene = false;
	if (sceneObjects.size() > 0) {
		for (int i = 0; i < sceneObjects.size(); i++) {
			
			if (sceneObjects[i] == ent) {
				alreadyInScene == true;
			}
			
		}
	}
	if (!alreadyInScene) {
		sceneObjects.push_back(ent);
	}
}

Entity* Scene::GetEntity(Entity* ent) {
	for (int i = 0; i < sceneObjects.size(); i++) {
		if (sceneObjects[i] == ent) {
			return sceneObjects[i];
		}
	}
}