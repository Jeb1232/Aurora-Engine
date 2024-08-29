#pragma once
#include"../dllHeader.h"
#include"AL/al.h"
#include"AL/alc.h"
#include"AL/alext.h"
#include"AL/sndfile.h"
#include<vector>
#include<iostream>
#include <chrono>
#include <thread>

class AudioSource
{
public:
	AURORAENGINE_API AudioSource();
	AURORAENGINE_API ~AudioSource();
	void AURORAENGINE_API play(const char* path);

	// Parameters
	ALuint source;
	ALfloat volume = 1.0f;
	ALfloat pitch = 1.0f;
	ALfloat position[3] = { 0.0f,0.0f,-10.0f };
	ALfloat velocity[3] = { 0.0f,0.0f,0.0f };
	bool looping = false;
	ALuint currentBuffer = 0;
	std::vector<ALuint> buffers;
private:
	void playBuf(ALuint buffer);
	std::thread audioThread;
	ALCdevice* device;
	ALCcontext* context;
};

