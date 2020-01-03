#include "Image.h"


AlesiaSurface* alesia__loadImage(void* data,int size)
{
    int w,h,channels;
    void* img = stbi_load_from_memory(data,size,&w,&h,&channels,STBI_rgb_alpha);
    if(img == NULL)
        return NULL;

    AlesiaSurface* result = alesia__createSurface(w,h);

    //TODO: We could probably avoid the copy by directly using the data if the pixels are the right format
    int x,y;
    for(x = 0;x < w;x++)
        for(y = 0;y < h;y++)
            alesia__setSurfaceColor(result,x,y,alesia__getImagePixel(img,x,y,w,h,channels));

    stbi_image_free(img);
    return result;
}

AlesiaColor alesia__getImagePixel(void* data,int x,int y,int w,int h,int channel)
{
    AlesiaColor result;
    unsigned char* pixelOffset = data + ((w * y) + x) * channel;
    result.r = pixelOffset[0];
    result.g = pixelOffset[1];
    result.b = pixelOffset[2];
    result.a = channel >= 4 ? pixelOffset[3] : 0xff;
    return result;
}
