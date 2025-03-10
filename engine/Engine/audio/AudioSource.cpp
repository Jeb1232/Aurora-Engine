#include "AudioSource.h"

struct MemoryBuffer {
	unsigned char* data;
	sf_count_t size;
	sf_count_t position;
};

// Custom read function to read from memory
sf_count_t read_func(void* ptr, sf_count_t count, void* user_data) {
	MemoryBuffer* memBuffer = static_cast<MemoryBuffer*>(user_data);
	sf_count_t remaining = memBuffer->size - memBuffer->position;
	sf_count_t to_read = (remaining > count) ? count : remaining;

	std::memcpy(ptr, memBuffer->data + memBuffer->position, to_read);
	memBuffer->position += to_read;

	return to_read;
}

// Custom seek function
sf_count_t seek_func(sf_count_t offset, int whence, void* user_data) {
	MemoryBuffer* memBuffer = static_cast<MemoryBuffer*>(user_data);
	sf_count_t new_position = 0;

	switch (whence) {
	case SEEK_SET:
		new_position = offset;
		break;
	case SEEK_CUR:
		new_position = memBuffer->position + offset;
		break;
	case SEEK_END:
		new_position = memBuffer->size + offset;
		break;
	default:
		return -1;
	}

	if (new_position < 0 || new_position > memBuffer->size) {
		return -1;
	}

	memBuffer->position = new_position;
	return new_position;
}

// Custom tell function
sf_count_t tell_func(void* user_data) {
	MemoryBuffer* memBuffer = static_cast<MemoryBuffer*>(user_data);
	return memBuffer->position;
}

// Custom write function (not needed, just a placeholder for completeness)
sf_count_t write_func(const void* ptr, sf_count_t count, void* user_data) {
	return 0; // Writing is not supported in this case
}

AudioSource::AudioSource(ALCdevice* device, ALCcontext* context)
{
	sDevice = device;

	sContext = context;


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

void AudioSource::setPosition(float x, float y, float z) 
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

void AudioSource::setVelocity(float x, float y, float z) 
{
	velocity[0] = x;
	velocity[1] = y;
	velocity[2] = z;
}

void AudioSource::setMinDistance(float minDist) 
{
	minDistance = minDist;
}

void AudioSource::setMaxDistance(float maxDist)
{
	maxDistance = maxDist;
}

void AudioSource::setRollOffFactor(float factor) {
	rolloffFactor = factor;
}

/*
        std::string filename = "your-sound-file.wav";
        std::vector<char> fileData = loadFileIntoMemory(filename);
        
        // Initialize memory buffer
        MemoryBuffer memBuffer = { fileData.data(), static_cast<sf_count_t>(fileData.size()), 0 };

        // Set up virtual I/O for libsndfile
        SF_VIRTUAL_IO virtualIO;
        virtualIO.get_filelen = [](void *user_data) -> sf_count_t {
            MemoryBuffer *memBuffer = static_cast<MemoryBuffer*>(user_data);
            return memBuffer->size;
        };
        virtualIO.read = read_func;
        virtualIO.seek = seek_func;
        virtualIO.tell = tell_func;
        virtualIO.write = write_func; // No write support
*/

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
        return;
    }

	audioThread = std::thread(&AudioSource::playBuf, this, buffer);
}

void AudioSource::playM(const char* path)
{
	//std::string filename = "your-sound-file.wav";
	//std::vector<char> fileData = loadFileIntoMemory(filename);

	AssetPacker packer;
	AssetPacker::LoadedFile lFile = packer.LoadFileFromPackage("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/data001.pak", path);

	// Initialize memory buffer
	MemoryBuffer memBuf = { lFile.data, static_cast<sf_count_t>(lFile.fileSize), 0 };

	// Set up virtual I/O for libsndfile
	SF_VIRTUAL_IO virtualIO;
	virtualIO.get_filelen = [](void* user_data) -> sf_count_t {
		MemoryBuffer* memBuffer = static_cast<MemoryBuffer*>(user_data);
		return memBuffer->size;
	};
	virtualIO.read = read_func;
	virtualIO.seek = seek_func;
	virtualIO.tell = tell_func;
	virtualIO.write = write_func; // No write support
	// Open the audio file
	SF_INFO sfinfo;
	SNDFILE* sndfile = sf_open_virtual(&virtualIO, SFM_READ, &sfinfo, &memBuf);
	if (!sndfile) {
		std::cerr << "Failed to load sound from memory" << std::endl;
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
		std::cerr << "Failed to read all samples from file" << std::endl;
		std::cerr << "read frames: " << numFrames << " info frames: " << sfinfo.frames << std::endl;
		sf_close(sndfile);
		return;
	}

	std::cerr << "Read " << numFrames << " frames from file" << std::endl;

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
		return;
	}
	free(lFile.data);
	audioThread = std::thread(&AudioSource::playBuf, this, buffer);
}

void AudioSource::playBuf(ALuint buffer) {

	alSourcei(source, AL_BUFFER, buffer);

	alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);

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
		alSourcef(source, AL_ROLLOFF_FACTOR, rolloffFactor);
		alSourcef(source, AL_REFERENCE_DISTANCE, minDistance);
		alSourcef(source, AL_MAX_DISTANCE, maxDistance);
		alListenerfv(AL_POSITION, listener.position);
		alListenerfv(AL_VELOCITY, listener.velocity);
		alListenerfv(AL_ORIENTATION, listener.orientation);
		alListenerf(AL_GAIN, listener.gain);
		alGetSourcei(source, AL_SOURCE_STATE, &state);
	}
}