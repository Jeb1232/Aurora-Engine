#pragma once
#include"../dllHeader.h"
#include<iostream>
#include <chrono>
#include<d3d11.h>
//#include<dxgi.h>
#include<d3dcompiler.h>
#include<thread>
#include<exception>
#include<fstream>
#include<vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include<glm/gtx/matrix_major_storage.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include"Model.h"
class Light
{
public:
	enum LightType {
		LIGHT_DIRECTIONAL,
		LIGHT_POINT,
		LIGHT_SPOT,
		LIGHT_AREA
	};

	Light();
	~Light();
	void AURORAENGINE_API UpdateLightBuffer();
	glm::vec3 position;
	glm::vec3 rotation;
	float intensity;
	glm::vec3 lightColor;
private:
};

