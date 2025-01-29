#include "AudioListener.h"

AudioListener::AudioListener() 
{
	alListenerfv(AL_POSITION, position);
	alListenerfv(AL_VELOCITY, velocity);
	alListenerfv(AL_ORIENTATION, orientation);
	alListenerf(AL_GAIN, gain);
}

void AudioListener::setPosition(glm::vec3 pos) {
	ALfloat newPos[3] = { pos.x,pos.y,pos.z };
	position[0] = newPos[0];
	position[1] = newPos[1];
	position[2] = newPos[2];
	alListenerfv(AL_POSITION, position);
}

void AudioListener::setRotation(glm::vec3 lookRot, glm::vec3 upRot) {
	ALfloat newLookRot[3] = { lookRot.x,lookRot.y,lookRot.z };
	ALfloat newupRot[3] = { upRot.x,upRot.y,upRot.z };
	orientation[0] = newLookRot[0];
	orientation[1] = newLookRot[1];
	orientation[2] = newLookRot[2];
	orientation[3] = newupRot[0];
	orientation[4] = newupRot[1];
	orientation[5] = newupRot[2];

	alListenerfv(AL_ORIENTATION, orientation);
}

void AudioListener::setVelocity(glm::vec3 vel) {
	ALfloat newVel[3] = { vel.x,vel.y,vel.z };

	velocity[0] = newVel[0];
	velocity[1] = newVel[1];
	velocity[2] = newVel[2];

	alListenerfv(AL_POSITION, velocity);
}

AudioListener::~AudioListener() {

}