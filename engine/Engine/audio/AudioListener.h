#pragma once
#include"../dllHeader.h"
#include"glm/glm.hpp"
#include"AL/al.h"
#include"AL/alc.h"
#include"AL/alext.h"
#include"AL/sndfile.h"
#include<vector>
#include<iostream>
#include <chrono>
#include <thread>

class AudioListener
{
public:
	AURORAENGINE_API AudioListener();
	AURORAENGINE_API ~AudioListener();
	void AURORAENGINE_API setPosition(glm::vec3 pos);
	void AURORAENGINE_API setRotation(glm::vec3 lookRot, glm::vec3 upRot);
	void AURORAENGINE_API setVelocity(glm::vec3 vel);

	// Set listener properties
	ALfloat position[3] = { 0.0f, 0.0f, 0.0f };
	ALfloat velocity[3] = { 0.0f, 0.0f, 0.0f };
	ALfloat orientation[6] = { 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f };
	ALfloat gain = 1.0f;
};

