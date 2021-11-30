/*
 * 	Author: Praise Ike
 * 	Date: Mon 29-11-2021
 *
 * 	Notes:
 * 		This project is as a result of following an implementation of the CHIP8 interpreter
 * 		on freecodecamp.com. The original was written in javascript with the use of canvas
 * 		for the video display and other utilities provided by javascript.
 *
 * 		This is an implementation written in C++ with the SDL2 libray to provide 
 * 		video and sound (SDL2_mixer).
 *
 * 		As of the date above, the only partially working components and the renderer and 
 * 		speaker sub classes, as for the keyboard, well ...
 * 	
 */




#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "assets/chip8.cpp"

int main(int argc, char ** argv)
{
	if(argc < 2)
	{
		printf("Usage: ./main ROM_IMAGE_FILE \n");
		return -1;
	}
	Chip8 chip = Chip8(argv[1]);
	chip.mainloop();
}


