#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <SDL2/SDL.h>
using namespace std;

#ifndef CHIP8_KEYBOARD
#define CHIP8_KEYBOARD

class Keyboard
{
	public:
		Keyboard();
		map<SDL_Keycode,Uint8> keys;
		bool keysPressed[256] = {false};
//		auto onNextKeyPress = NULL;
		void handleKeyDown(SDL_Keycode code);
		void handleKeyUp(SDL_Keycode code);
		inline bool isKeyPressed(Uint8 scanCode)
		{
			cout << "Key code: "<<scanCode<<endl;
			return !this->keysPressed[0];
			//	return this->keysPressed[scanCode];
		}

};

Keyboard::Keyboard()
{
	// store key maps for keys 0x0 to 0xf
	// 0-9,a-f
	keys[SDLK_0] = 0x0;
	keys[SDLK_1] = 0x1;
	keys[SDLK_2] = 0x2;
	keys[SDLK_3] = 0x3;
	keys[SDLK_4] = 0x4;
	keys[SDLK_5] = 0x5;
	keys[SDLK_6] = 0x6;
	keys[SDLK_7] = 0x7;
	keys[SDLK_8] = 0x8;
	keys[SDLK_9] = 0x9;
	keys[SDLK_a] = 0xa;
	keys[SDLK_b] = 0xb;
	keys[SDLK_c] = 0xc;
	keys[SDLK_d] = 0xd;
	keys[SDLK_e] = 0xe;
	keys[SDLK_f] = 0xf;
}

void Keyboard::handleKeyDown(SDL_Keycode code)
{
}	


void Keyboard::handleKeyUp(SDL_Keycode code)
{
}
#endif 
