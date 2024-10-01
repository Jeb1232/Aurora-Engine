#include "AudioListener.h"

AudioListener::AudioListener() 
{
	alListenerfv(AL_POSITION, position);
	alListenerfv(AL_VELOCITY, velocity);
	alListenerfv(AL_ORIENTATION, orientation);
	alListenerf(AL_GAIN, gain);
}

AudioListener::~AudioListener() {

}