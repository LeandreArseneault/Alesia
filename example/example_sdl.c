
#include <SDL.h>

#include "../src/Render.h"
#include "../src/Math.h"
#include "../src/Image.h"
#include "../src/Text.h"

#include <unistd.h>

void* readAllFile(char* path,int* size)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    *size = (int) fsize;
    void *data = malloc(fsize);
    fread(data, 1, fsize, f);
    fclose(f);
    return data;
}

int main(int argc,char* argv[])
{

    SDL_Window* window =  SDL_CreateWindow("Alesia example",0,0,800,600,0);

	AlesiaSurface* libSurface = alesia__createSurface(1920,1080);
	SDL_Surface *sdlsurf = SDL_CreateRGBSurfaceFrom(libSurface->memory, 800, 600, 32,4 * libSurface->width,0x000000ff,0x0000ff00,0x00ff0000,0xff000000);
	AlesiaPaint* paint = alesia__createPaint();

    alesia__clearColor(libSurface,alesia__makeColor(255,255,255,255));

    alesia__setPaintColor(paint,alesia__makeColor(255,0,0,255));
    alesia__beginPath(paint,100,400);
    alesia__addLine(paint,400,100);
    alesia__addLine(paint,400,400);
    alesia__addLine(paint,100,400);
    alesia__closePath(paint);
    alesia__fill(libSurface,paint);

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_BlitSurface(sdlsurf, NULL, surface, NULL);
    SDL_UpdateWindowSurface(window);
    sleep(2);

	return 0;
}

