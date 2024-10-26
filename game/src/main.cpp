#include<iostream>
#include<thread>
#include"renderer/Renderer.h"
#include"audio/AudioSource.h"
#include"audio/AudioDevice.h"
#include"audio/AudioListener.h"

int main(int argc, char* argv[]) 
{
	//AudioDevice device;
	//AudioListener listener;
	//AudioSource source(device.device, device.context);
	//source.listener = listener;
	Renderer renderer;
	Scene scene;
	renderer.SetActiveScene(&scene);

	//source.play("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/music.ogg");
	//source.looping = true;
	renderer.CreateRenderWindow(1280, 720, false);
	while (!renderer.finishedRendering);
	return 0;
}