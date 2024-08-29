#include "AudioSource.h"

AudioSource::AudioSource() 
{
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

	// Create a source and set its position
	alGenSources(1, &source);

	/*
	if (alGetError() != AL_NO_ERROR) {
		std::cerr << "Failed to generate OpenAL source" << std::endl;
		alDeleteBuffers(1, &buffer);
		alcDestroyContext(context);
		alcCloseDevice(device);
		return;
	}
	*/
	// Set source properties for 3D sound with attenuation
	alSourcef(source, AL_PITCH, pitch);
	alSourcef(source, AL_GAIN, volume);
	alSourcefv(source, AL_POSITION, position);
	alSourcefv(source, AL_VELOCITY, velocity);
	alSourcei(source, AL_LOOPING, looping);

	// Set attenuation properties
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	alSourcef(source, AL_REFERENCE_DISTANCE, 1.0f);  // Distance at which the source is at full volume
	alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);      // How quickly the sound attenuates with distance
	alSourcef(source, AL_MAX_DISTANCE, 150.0f);       // Maximum distance before the sound is no longer heard
}


AudioSource::~AudioSource() 
{
	alDeleteSources(1, &source);
	if (audioThread.joinable()) {
		audioThread.join();
	}
}

void AudioSource::play(const char* path) 
{
	// Open the audio file
	SF_INFO sfinfo;
	SNDFILE* sndfile = sf_open(path, SFM_READ, &sfinfo);
	if (!sndfile) {
		std::cerr << "Failed to open file: " << path << std::endl;
		return;
	}

	// Check the format
	if (sfinfo.channels > 2) {
		std::cerr << "Unsupported channel count: " << sfinfo.channels << std::endl;
		sf_close(sndfile);
		return;
	}

	ALenum format;
	if (sfinfo.channels == 1) {
		format = AL_FORMAT_MONO16;
	}
	else {
		format = AL_FORMAT_STEREO16;
	}

	// Read the samples
	std::vector<short> samples(sfinfo.frames * sfinfo.channels * 2);
	sf_count_t numFrames = sf_readf_short(sndfile, samples.data(), sfinfo.frames);
	if (numFrames < sfinfo.frames) {
		std::cerr << "Failed to read all samples from file: " << path << std::endl;
		sf_close(sndfile);
		return;
	}

	std::cerr << "Read " << numFrames << " frames from file: " << path << std::endl;

	// Close the file
	sf_close(sndfile);

	// Calculate the buffer size
	ALsizei bufferSize = (ALsizei)(numFrames * sfinfo.channels * 2);
	std::cerr << "Buffer size: " << samples.size() << " bytes" << std::endl;

	// Create OpenAL buffer and fill it with the sound data
	ALuint buffer;
	alGenBuffers(1, &buffer);
	if (alGetError() != AL_NO_ERROR) {
		std::cerr << "Failed to generate OpenAL buffer" << std::endl;
		return;
	}

	alBufferData(buffer, format, samples.data(), bufferSize, sfinfo.samplerate);
	if (alGetError() != AL_NO_ERROR) {
		std::cerr << "Failed to fill OpenAL buffer" << std::endl;
		alDeleteBuffers(1, &buffer);
		return;
	}

	currentBuffer = buffer;

	bool bufferAlreadyExists = false;
	for (int i = 0; i < buffers.size(); i++) {
		if (buffers[i] == buffer) {
			bufferAlreadyExists = true;
		}
	}

	if (bufferAlreadyExists == false) { buffers.push_back(buffer); }

    // Load the sound data
    if (!buffer) {
        alcDestroyContext(context);
        alcCloseDevice(device);
        return;
    }

	audioThread = std::thread(&AudioSource::playBuf, this, buffer);
}

void AudioSource::playBuf(ALuint buffer) {

	alSourcei(source, AL_BUFFER, buffer);

	// Set listener properties
	ALfloat listenerPos[] = { 0.0f, 0.0f, 0.0f };
	ALfloat listenerVel[] = { 0.0f, 0.0f, 0.0f };
	ALfloat listenerOri[] = { 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f }; // at, up

	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alListenerf(AL_GAIN, 0.99f);

	// Play the sound
	alSourcePlay(source);

	ALint state = AL_PLAYING;

	while (state == AL_PLAYING)
	{
		alSourcef(source, AL_PITCH, pitch);
		alSourcef(source, AL_GAIN, volume);
		alSourcefv(source, AL_POSITION, position);
		alSourcefv(source, AL_VELOCITY, velocity);
		alSourcei(source, AL_LOOPING, looping);
		alGetSourcei(source, AL_SOURCE_STATE, &state);
	}
}