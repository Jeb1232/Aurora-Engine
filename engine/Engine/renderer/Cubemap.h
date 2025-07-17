#pragma once
#include"../renderer/Texture.h"
#include"../renderer/Renderer.h"
class Cubemap
{
public:
	Texture* m_cubeMap = nullptr;
	glm::vec3 position;

	void AURORAENGINE_API RenderCubemap(int res);

private:
};

