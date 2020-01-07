
#include <SDL.h>

#include "../src/Render.h"
#include "../src/Math.h"
#include "../src/Image.h"
#include "../src/Text.h"

#include <unistd.h>

int main(int argc,char* argv[])
{
	SDL_Window* window =  SDL_CreateWindow("Alesia example",0,0,800,600,0);

	AlesiaSurface* libSurface = alesia__createSurface(1920,1080);
	SDL_Surface *sdlsurf = SDL_CreateRGBSurfaceFrom(libSurface->memory, 800, 600, 32,4 * libSurface->width,0x000000ff,0x0000ff00,0x00ff0000,0xff000000);
	AlesiaPaint* paint = alesia__createPaint();

    alesia__clearColor(libSurface,alesia__makeColor(255,255,255,255));
	alesia__setPaintColor(paint,alesia__makeColor(255,0,0,255));


	AlesiaPolygon* polygonBase = alesia__polygon__createPolygon(300,300);
	alesia__polygon__addLine(polygonBase,600,300);
	alesia__polygon__addLine(polygonBase,600,600);
	alesia__polygon__addLine(polygonBase,600,300);
	alesia__polygon__addLine(polygonBase,300,300);

	AlesiaPolygon* polygonOperand = alesia__polygon__createPolygon(450,100);
    alesia__polygon__addLine(polygonOperand,550,450);
    alesia__polygon__addLine(polygonOperand,350,450);
    alesia__polygon__addLine(polygonOperand,450,100);


	paint->workingPolygon = polygonBase;
	alesia__fill(libSurface,paint);

	//paint->workingPolygon = polygonOperand;
	//alesia__fill(libSurface,paint);

	SDL_Surface* surface = SDL_GetWindowSurface(window);
	SDL_BlitSurface(sdlsurf, NULL, surface, NULL);
	SDL_UpdateWindowSurface(window);
	sleep(2);

	return 0;
}

