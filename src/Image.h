#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Render.h"

AlesiaSurface* alesia__loadImage(void* data,int size);

//private
AlesiaColor alesia__getImagePixel(void* data,int x,int y,int w,int h,int channel);


#endif // IMAGE_H_INCLUDED
