
#include <SDL.h>

#include "../src/Render.h"
#include "../src/Math.h"
#include "../src/Image.h"
#include "../src/Text.h"

#include <unistd.h>

int main(int argc,char* argv[])
{

	AlesiaSurface* libSurface = alesia__createSurface(1920,1080);
	AlesiaPaint* paint = alesia__createPaint();
    alesia__clearColor(libSurface,alesia__makeColor(255,255,255,255));

    alesia__setPaintColor(paint,alesia__makeColor(255,0,0,255));
    alesia__beginPath(paint,100,400);
    alesia__addLine(paint,400,100);
    alesia__addLine(paint,400,400);
    alesia__addLine(paint,100,400);
    alesia__closePath(paint);
    alesia__fill(libSurface,paint);

	return 0;
}

