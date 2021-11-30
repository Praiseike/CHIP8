#ifndef CHIP8_SPEAKER_H
#define CHIP8_SPEAKER_H
#include <SDL2/SDL_mixer.h>

class Speaker
{
	public:
	Speaker();
	void play(int frequency);
	void stop();
	Mix_Chunk * sound;
};

Speaker::Speaker()
{
	log("Initializing speaker");
	if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048) < 0)
		printf("Unable to Open audio device! %s\n",Mix_GetError());
	sound = Mix_LoadWAV("assets/sound/1up.wav");
	if(!sound)
	{
		printf("Unable to load sound file! %s\n",Mix_GetError());
	}
	Mix_PlayChannel(-1,sound,0);
}

void Speaker::play(int frequency)
{

}

void Speaker::stop()
{}

#endif
