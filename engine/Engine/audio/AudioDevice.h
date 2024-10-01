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
class AudioDevice
{
public:
	AURORAENGINE_API AudioDevice();
	AURORAENGINE_API ~AudioDevice();

	ALCdevice* device;
	ALCcontext* context;
};

