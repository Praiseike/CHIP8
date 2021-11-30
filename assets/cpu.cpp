/*
 *	4KB memory
 *	16 8-bit registers
 *	A 16-bit register I store memory address
 *	2 timers, one for delay, the other for sound
 *	A program counter that stores the address
 *	An array to represent the stack
 */


#include <iostream>
#include <stack>
#include <time.h>
#include <stdlib.h>

using namespace std;
#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#define INSTRUCTIONS_OFFSET 0x200


class CPU
{
	
	public:
	CPU(Renderer * renderer, Keyboard * keyboard , Speaker * speaker)
	{
		srand(time(NULL));
		// copy component pointers provided by the chip8 class
		this->cRenderer = renderer;
		this->cKeyboard = keyboard;
		this->cSpeaker = speaker;

		// 4KB Memory
		this->memory = new Uint8[ 4 * 1024 ]; 
		if(!this->memory)
		{
			printf("Unable to allocate memory\n");
		}

		//memset(&this->memory,0,4 * 1024);
		// 16 8-bit registers 
		this->v = new Uint8[16];
		
		// holds the memory address
		this->I = 0;

		// timers
		this->delayTimer = 0;
		this->soundTimer = 0;

		// Init the program counter for currently executing address
		this->PC = 0x200;

		this->paused = false;
		this->speed = 4;
	}

		
		~CPU()
		{
			delete this->memory;
			delete this->v;
		}
	void loadSpritesIntoMemory();
		void loadROM(const char * filename);
		void loadProgramIntoMemory(Uint8 * program,size_t n);
		void cycle();
	private:



		inline void updateTimers()
		{
			if(this->delayTimer > 0)
				this->delayTimer -= 1;
			if(this->soundTimer > 0)
				this->soundTimer -= 1;
		}
		
		void playSound();
		void executeInstruction(Uint16 opcode);

		Keyboard * cKeyboard;
		Speaker * cSpeaker;
		Renderer * cRenderer;
			
		Uint8 * memory = NULL;
		Uint8 * v;
		stack<Uint8>cStack;

		int I,PC;
		int speed;
		int delayTimer,soundTimer;
		bool paused;
};

void CPU::loadSpritesIntoMemory()
{
	const unsigned short sprites[] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
        	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		
	};
	// copy the sprites to memory location 0x0000
	for(int i = 0; i < 80; i++)
	{
		this->memory[i] = sprites[i];
	}
}

void CPU::loadProgramIntoMemory(Uint8 * program,size_t n)
{
	// copy the program from the buffer provide py the loadROM function
	// int memory location 0x200
	for(int offset = 0; offset < n ; offset++)
	{
		this->memory[INSTRUCTIONS_OFFSET + offset] = program[offset];
		printf("%x ",this->memory[INSTRUCTIONS_OFFSET] + offset);
	
	}
}

void CPU::loadROM(const char * filename)
{

	// read instructions from the binary file
	// given the filename
	
	FILE * romImage = fopen(filename,"rb");
	if(!romImage)
	{
		printf("Unable to open %s\n",filename);
	}
	// seek to the end of file and 
	// get filesize by calling ftell(FILE * file);
	fseek(romImage,0L,SEEK_END);
	int fileSize = ftell(romImage);
	// set the file pointer to the beginning
	fseek(romImage,0L,SEEK_SET);

	Uint8 buffer[fileSize] = {0}; // set all to zeroes
	fread(buffer,1,fileSize,romImage);
	this->loadProgramIntoMemory(buffer,fileSize);
	printf("Loaded %i bytes to 0x%x\n",fileSize,buffer);
	fclose(romImage);
}

void CPU::cycle()
{
	// actual cpu execution cycle
	for(int i = 0 ; i< this->speed ; i++)
	{
		if(!this->paused)
		{
			// FETCH:
			// fetch instruction from memory pointed to py the program counter (Instruction pointer)
			// this->PC
		
			// since each opcode should be words (2bytes)
			// grab [PC] and [PC + 1]
			// left shift [PC] 8bits to the left
			// e.g 0x8E becomes 0x8E00 = 0x8E * 2^8
			// Bitwise OR the result with the second byte [PC+1] = 0x7F
			// 0x8E00
			// 0x007F
			// ------
			// 0x837F
			// ------
			//
			
			Uint16 opcode = (this->memory[this->PC] << 8 | this->memory[this->PC +1]);
			// DECODE AND EXECUTE
			this->executeInstruction(opcode);
		}
	}
	if(!this->paused)
		this->updateTimers();
	this->playSound();
	this->cRenderer->render();
}

void CPU::playSound()
{
	// play sound at a specific frequency
	// NOT IMPLEMENTED
	if(this->soundTimer > 0)
		this->cSpeaker->play(440);
	else
		this->cSpeaker->stop();

}

void CPU::executeInstruction(Uint16 opcode)
{
	
	// for logging and debugging purposes
	char buff[64];
	sprintf(buff,"[0x%x] %x",this->PC,opcode);
	//log((const char * )buff);

	// increment program counter
	this->PC += 2;
	
	// from the CHIP8 instruction manual
	// each opcode is in the form 0xNxyN
	Uint8 x = (opcode & 0x0F00) >> 8;
	Uint8 y = (opcode & 0x00F0) >> 4;

	int width,height,r;

	// the rest is just a long,nested,nerve-racking,switch operation
	// depending on the nature of the opcode
	switch(opcode & 0xF000)
	{
		case 0x0000:
			switch(opcode) {
				case 0x00E0: // clear the display
					this->cRenderer->clear();
					break;
				case 0x00EE:
					this->PC = this->cStack.top();
					this->cStack.pop();
					break;
			}
			break;
		case 0x1000:
			this->PC = (opcode & 0xFFF);
			break;

		case 0x2000:
			this->cStack.push(this->PC);
			this->PC = (opcode & 0xFFF);
			break;

		case 0x3000:
			if( this->v[x] == (opcode &  0xFF))
			{
				this->PC += 2;
			}
			break;
			
		case 0x4000:
			if(this->v[x] != (opcode & 0xFF))
			{
				this->PC += 2;
			}
			break;

		case 0x5000:
			if(this->v[x] == this->v[y])
			{
				this->PC += 2;
			}
			break;

		case 0x6000:
			this->v[x] = (opcode & 0xFF);
			break;

		case 0x7000:
			this->v[x] += (opcode & 0xFF);
			break;

		case 0x8000:
			switch(opcode & 0xF)
			{
				case 0x0:
					this->v[x] = this->v[y];
					break;

				case 0x1:
					this->v[x] |= this->v[y];
					break;

				case 0x2:
					this->v[x] &= this->v[y];
					break;

				case 0x3:
					this->v[x] ^= this->v[y];
					break;

				case 0x4:
					{
					int sum = 0;//(this->v[x] += this->v[y]);
					this->v[0xF] = 0;
					if(sum > 0xFF) 
						this->v[0xF] = 1;
					this->v[x] = sum;	
					}
					break;

				case 0x5:
					this->v[0xF]  = 0;
					if(this->v[x] > this->v[y])
						this->v[0xF] = 1;
					this->v[x] -= this->v[y];
					break;

				case 0x6:
					this->v[0xF] = ( this->v[x] & 0x1);
					this->v[x] >>= 1;	
					break;

				case 0x7:
					this->v[0xF] = 0;
					if(this->v[y] > this->v[x])
						this->v[0xF] = 1;
					this->v[x] = this->v[y] - this->v[x];
					break;

				case 0xE:
					this->v[0xF] = (this->v[x] & 0x80);
					this->v[x] <<= 1;
					break;

			}
			break;
		case 0x9000:
			if(this->v[x] != this->v[y])
			{
				this->PC += 2;
       		}
       		break;

		case 0xA000:
    		this->I = (opcode & 0xFFF);
			break;
			
		case 0xB000:
	    	this->PC = ((opcode & 0xFFF) + this->v[0]);
			break;
		
		case 0xC000:
	    	r = (rand() % 255);
			this->v[x] = (r & (opcode & 0XFF));
			break;

		case 0xD000:
    		width = 8;
			height = (opcode  & 0xF);

			this->v[0xF] = 0;

			for (int row = 0; row < height; row++)
			{
				int sprite = this->memory[this->I + row];
				for(int col = 0; col < width; col++)
				{
					// if the sprite is not zero
					// render or erase the pixel
					if((sprite & 0x80) > 0)
					{
						// if setPixel returns 1 which means a pixel was erased
						// set VF to 1
						if(this->cRenderer->setPixel(this->v[x] + col,this->v[y]+ row))
						{
							this->v[0xF] = 1;
						}
					}

					// shift the sprite left 1. This will move the next col/bit of the sprite into the first
					// position
					// Ex. 10010100 << 1 will become 00101000
					sprite <<=1;
				}
			}
			break;

		case 0xE000:
		// Handle key input
    		switch (opcode & 0xFF) {
        			case 0x9E:
				if(this->cKeyboard->isKeyPressed(this->v[x]))
					this->PC += 2; 
            			break;

        			case 0xA1:
            			if(!this->cKeyboard->isKeyPressed(this->v[x]))
					this->PC += 2;
				break;

    		}

    		break;
		case 0xF000:
    		switch (opcode & 0xFF)
			{
        		case 0x07:
					this->v[x] = this->delayTimer;
            		break;

       			case 0x0A:
           			this->paused = true;
					// I NEED TO CHILL MY BRAIN BEFORE DOING THIS

					//this->keyboard->onNextPress = [](auto t){
					//	this->v[x] = t;
					//	this->paused = false;
					//}
					break;

       			case 0x15:
					this->delayTimer = this->v[x];
            		break;

        		case 0x18:
					this->soundTimer = this->v[x];
            		break;

        		case 0x1E:
           			this->I += this->v[x];
					break;

        		case 0x29:
					this->I = this->v[x] * 5;
					break;

				case 0x33:
					// get the hundreds digit and place it in I
					this->memory[this->I] = int(this->v[x] / 100);
					// get tens digit and place in I+1. Get a value btw 0-99
					// then divide by 10 to give us a value betwwn 0-9
					this->memory[this->I + 1] = int((this->v[x] % 100) /10);
	            			// get the value of th ones (last digit and place it in I + 2
					this->memory[this->I + 2] = int(this->v[x] % 10);
					break;

        		case 0x55:
					for( int reg = 0; reg <= x; reg++)
					{
						this->memory[this->I + reg] = this->v[reg];
					}
            		break;
        	
        		case 0x65:
					for(int i = 0 ;i < x; i++)
					{
						this->v[i] = this->memory[this->I + i];
					}
	      			break;
   			}

    	break;	

   
	}

}

#endif



