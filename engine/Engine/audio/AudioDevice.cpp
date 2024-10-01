#include "AudioDevice.h"

AudioDevice::AudioDevice() {
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open audio device" << std::endl;
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context || alcMakeContextCurrent(context) == ALC_FALSE) {
        if (context) alcDestroyContext(context);
        alcCloseDevice(device);
        std::cerr << "Failed to create OpenAL context" << std::endl;
        return;
    }
}

AudioDevice::~AudioDevice() {
    alcDestroyContext(context);
    alcCloseDevice(device);
}