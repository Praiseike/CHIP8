/*
 *	The Chip8 Class is the master behind the scenes
 *	It handles the initialization of other components
 *	like video, speaker, keyboard and even the cpu
 *	
 *
 *
 */




#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


#ifndef CHIP8_H
#define CHIP8_H
#define LOGGING

#include "renderer.cpp"
#include "keyboard.cpp"
#include "speaker.cpp"
#include "cpu.cpp"

class Chip8
{
	SDL_Renderer * mRenderer = NULL;
	SDL_Window * mWindow = NULL;
	SDL_Event mEvent;
	
	// Component classes
	Renderer *  display;
	Keyboard * keyboard;
	Speaker * speaker;
	CPU * cpu;

	const int WIDTH = 800;
	const int HEIGHT = 600;
	
	int fps = 60;
	int ticksPerFrame;
	int startTime;


	bool mRunning = true;
	void init();
	public:
		~Chip8();
		Chip8(const char * ROMFile);
		void mainloop();

};


Chip8::Chip8(const char * ROMFile)
{
	log("Log message");
	init();
	ticksPerFrame = 1000 / fps;

	// initializing and allocating memory for the
	// different components
	display = new Renderer(this->mRenderer,3);
	keyboard = new Keyboard();
	speaker = new Speaker();
	cpu = new CPU(display,keyboard,speaker);
	// load cpu sprites
	// see CPU::loadSpritesIntoMemory
	cpu->loadSpritesIntoMemory();
	// read the chip8 instructions from disk to memory
	cpu->loadROM(ROMFile);
}



Chip8::~Chip8()
{

	// freeing up memory 
	delete cpu;
	delete speaker;
	delete keyboard;
	delete display;

	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
	SDL_Quit();

}
void Chip8::init()
{

	// Initialize the SDL library for sound and video
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0)
	{
		mWindow = SDL_CreateWindow("new window",0,0,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);
		if(!mWindow)
		{
			printf("Unable to create window! %s \n",SDL_GetError());
		}
		mRenderer = SDL_CreateRenderer(mWindow,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(!mRenderer)
		{
			printf("Unable to create vsync renderer! %s\n",SDL_GetError());
		}
	}
	else{
		printf("failed to init sdl\n");
	}
}

void Chip8::mainloop()
{
	// the mainloop
	while(mRunning)
	{
		// polling events for key input
		while(SDL_PollEvent(&mEvent) != 0)
		{
			if(mEvent.type == SDL_QUIT)
				mRunning = false;
			if(mEvent.type == SDL_KEYDOWN)
				keyboard->handleKeyDown(mEvent.key.keysym.sym);
			if(mEvent.type == SDL_KEYUP)
				keyboard->handleKeyUp(mEvent.key.keysym.sym);
		}

		// capping frame rate... or something like that
		startTime = SDL_GetTicks();
		SDL_SetRenderDrawColor(mRenderer,0x0,0x0,0x0,0xFF);
		SDL_RenderClear(mRenderer);
		// call the cpu to execute stuff
		cpu->cycle();
		SDL_RenderPresent(mRenderer);
		int frameTicks = SDL_GetTicks();
		if(frameTicks < ticksPerFrame)
		{
			SDL_Delay(ticksPerFrame - frameTicks);
		}

	}
}

#endif
