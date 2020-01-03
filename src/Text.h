#ifndef TEXT_H_INCLUDED
#define TEXT_H_INCLUDED

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "Render.h"

typedef struct
{
    stbtt_fontinfo info;

}AlesiaFont;

AlesiaFont* alesia__loadFont(void* data,int size);

void alesia__destroyFont(AlesiaFont* font);

void alesia__text(AlesiaPaint* paint,AlesiaSurface* surface,AlesiaFont* font,float x,float y,char* txt);

float alesia__computeTextPosition(AlesiaPaint* paint,AlesiaFont* font,float x,float y,char* txt,int info);

int priv__alesia__getUtf8(char* s);

int priv__alesia__getUtf8CharSize(char *s);

#endif // TEXT_H_INCLUDED
