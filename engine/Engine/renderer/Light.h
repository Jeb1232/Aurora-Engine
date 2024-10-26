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
//#include"Model.h"
class Light
{
public:
	enum LightType {
		LIGHT_DIRECTIONAL = 1,
		LIGHT_POINT = 2,
		LIGHT_SPOT = 3,
		LIGHT_AREA = 4
	};



	//__declspec(align(16))

	// retarded byte alignment (fuck you directx)
		struct LightCB {
		glm::vec4 position;
		glm::vec4 direction;
		glm::vec4 lightColor;
		glm::vec4 ambientColor;
		glm::vec3 padding; // fuck my life
		float intensity;
	};

	Light();
	~Light();
	void AURORAENGINE_API UpdateLightBuffer(ID3D11Device* m_device, ID3D11DeviceContext* m_deviceContext);
	void AURORAENGINE_API setPosition(glm::vec3 pos);
	void AURORAENGINE_API setRotation(glm::vec3 rot);
	void AURORAENGINE_API setLightColor(glm::vec3 color);
	void AURORAENGINE_API setAmbientColor(glm::vec3 color);
	glm::vec4 position;
	glm::vec4 direction;
	float intensity = 1;
	glm::vec4 lightColor;
	glm::vec4 ambientColor;
private:
};

