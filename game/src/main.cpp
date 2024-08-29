#include<iostream>
#include<thread>
#include"renderer/Renderer.h"
#include"audio/AudioSource.h"

int main(int argc, char* argv[]) 
{
	AudioSource source;
	Renderer renderer;
	//source.play("C:/Users/Owner/source/repos/Aurora Engine/x64/Release/data/boowomp.wav");
	renderer.CreateRenderWindow(1280, 720, false);
	while (!renderer.finishedRendering);
	return 0;
}