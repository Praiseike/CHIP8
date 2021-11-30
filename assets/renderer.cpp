#include <SDL2/SDL.h>
#include <math.h>

#ifndef CHIP8_RENDERER_H
#define CHIP8_RENDERER_H
//#define LOGGING

void log(const char * msg)
{
	#if defined(LOGGING)
	printf("%s\n",msg);
	#endif
}

typedef struct
{
	int width;
	int height;
	int cols,rows;
	//Uint8 display[2048];
	Uint8 * display = NULL;
}display;

class Renderer
{

	public:
		Renderer(SDL_Renderer * renderer ,int scale);
		~Renderer();
		bool setPixel(int x , int y);
		void clear();	
		// draw to the screen
		void render();
		void testRender();
		// scale the draw size (pixel size or something similer)
		int scale;
		display screen;
		SDL_Renderer* mRenderer;
		
};

Renderer::~Renderer()
{
	// free the video memory
	if(screen.display)
		free(screen.display);
	screen.display = NULL;
}
Renderer::Renderer(SDL_Renderer * renderer,int scale)
{
	this->mRenderer = renderer;
	// screen dimensions 64 x 32
	screen.cols = 64;
	screen.rows = 32;
	this->scale = scale;

	screen.width = screen.cols * scale;
	screen.height = screen.rows * scale;
	
	//printf("%i %i \n",screen.cols,screen.rows);
	// allocate memory for the display
	// calloc - sets each memory location to zero
	screen.display = (Uint8 *)calloc(screen.rows * screen.cols,sizeof(Uint8));

	int size = screen.cols * screen.rows;
	//screen.display = new Uint8[size * sizeof(Uint8)];
	if(!screen.display)
		printf("Unable to allocate memory for video");

	//memset(&screen.display,0,sizeof(Uint8) * screen.cols * screen.height);
}

void Renderer::testRender()
{
	setPixel(5,2);
	setPixel(0,0);
}

bool Renderer::setPixel(int x,int y)
{
	// wrap pixel around the screen
	if(x > screen.cols)
		x -= screen.cols;
	else if(x < 0)
		x += screen.cols;

	if(y > screen.rows)
		y -= screen.rows;
	else if(y < 0)
		y += screen.rows;


	int pixelLocation = x + (y * screen.cols);
	// set pixel location in memory to 1
	screen.display[pixelLocation] ^= 1;
	return true; 
}

void Renderer::clear()
{
	// clear the screen by setting all cells to 0
	for(int i = 0; i < screen.cols * screen.rows ; i++)
	{
		screen.display[i] = 0;
	}
}

void Renderer::render()
{
	int cols = screen.cols;
	int rows = screen.rows;
	for(int i = 0 ; i < cols * rows; i++)
	{
		// get the x,y position
		Uint8 x = ( i % cols ) * scale;
		Uint8 y = floor(i / cols) * scale;
		
		// if the value of display[i] = 1 then draw pixel
		if(screen.display[i])
		{
			SDL_SetRenderDrawColor(mRenderer,0xff,0xff,0xff,0xff);
			SDL_Rect r = {x*4,y*4,scale * 2,scale * 2};
			SDL_RenderFillRect(mRenderer,&r);
		}
	}
}

#endif
