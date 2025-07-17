#pragma once
#include"../dllHeader.h"
#include"AudioListener.h"
#include"AL/al.h"
#include"AL/alc.h"
#include"AL/alext.h"
#include"AL/sndfile.h"
#include<vector>
#include<iostream>
#include <chrono>
#include <thread>
#include"../file/asset_packer.h"

class AudioSource
{
public:
	AURORAENGINE_API AudioSource(ALCdevice* device, ALCcontext* context);
	AURORAENGINE_API ~AudioSource();
	void AURORAENGINE_API play(const char* path);
	void AURORAENGINE_API playM(const char* path);
	void AURORAENGINE_API setPosition(float x, float y, float z);
	void AURORAENGINE_API setVelocity(float x, float y, float z);
	void AURORAENGINE_API setMinDistance(float minDist);
	void AURORAENGINE_API setMaxDistance(float maxDist);
	void AURORAENGINE_API setRollOffFactor(float factor);

	// Parameters
	AudioListener listener;
	ALuint source;
	ALfloat volume = 1.0f;
	ALfloat pitch = 1.0f;
	ALfloat minDistance = 1.0f;
	ALfloat maxDistance = 1000.0f;
	ALfloat rolloffFactor = 1.0f;
	ALfloat position[3] = { 0.0f,0.0f,-10.0f };
	ALfloat velocity[3] = { 0.0f,0.0f,0.0f };
	bool looping = false;
	bool is3D = true;
	ALuint currentBuffer = 0;
	std::vector<ALuint> buffers;
private:
	void playBuf(ALuint buffer);
	std::thread audioThread;
	ALCdevice* sDevice;
	ALCcontext* sContext;
};

