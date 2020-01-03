#include "Text.h"



AlesiaFont* alesia__loadFont(void* data,int size)
{
    AlesiaFont* result = malloc(sizeof(AlesiaFont));

    if (!stbtt_InitFont(&result->info,data,0))
    {
        free(result);
        return NULL;
    }

    return result;
}

void alesia__render__destroyFont(AlesiaFont* font)
{
    free(font);
}

void alesia__text(AlesiaPaint* paint,AlesiaSurface* surface,AlesiaFont* font,float x,float y,char* txt)
{
    const int strSize = strlen(txt);
    y += paint->fontSize;
    int i,ascent,descent,lineGap;
    stbtt_GetFontVMetrics(&font->info,&ascent,&descent,&lineGap);
    float totalWidth = 0;

    for (i = 0; i < strSize; i += priv__alesia__getUtf8CharSize(&txt[i])) //for every letter
    {
        unsigned int numberOfMove = 0;
        if(txt[i] != ' ')
        {
            stbtt_vertex* vertex;
            int glyphCodePoint = priv__alesia__getUtf8(&txt[i]);
            int glyphIndex = stbtt_FindGlyphIndex(&font->info,glyphCodePoint);
            int count = stbtt_GetGlyphShape(&font->info,glyphIndex,&vertex);
            float scale = (float) paint->fontSize / ((float) ascent + (float) descent);
            int j;

            float maxWidth = 0;
            for(j = 0;j < count; j++) //for every character
                maxWidth = MAX(maxWidth,vertex[j].x);

            float scaleFactor = scale;//paint->fontSize / maxHeight;

            for(j = 0;j < count; j++)
            {
                float mirrorY = vertex[j].y;
                mirrorY = -mirrorY ;

                float mirrorYC = vertex[j].cy;
                mirrorYC = -mirrorYC ;

                float mirrorYC1 = vertex[j].cy1;
                mirrorYC1 = -mirrorYC1 ;

                float xVert =  scaleFactor * vertex[j].x;
                float yVert = scaleFactor * mirrorY;

                float xcVert = scaleFactor * vertex[j].cx;
                float ycVert = scaleFactor * mirrorYC;

                float endPointX = xVert + totalWidth + x;
                float endPointY = yVert + y;

                if(vertex[j].type == STBTT_vline)
                {
                    alesia__addLine(paint,endPointX,endPointY);
                }
                else if(vertex[j].type == STBTT_vcurve)
                {
                    alesia__addBezier(paint,xcVert + x + totalWidth,ycVert + y,endPointX,endPointY);
                }
                else if(vertex[j].type== STBTT_vmove)
                {
                    if(numberOfMove == 0)
                    {
                        alesia__beginPath(paint,endPointX,endPointY);
                    }
                    else
                        alesia__beginSubPath(paint,ALESIA_XOR,endPointX,endPointY);
                    numberOfMove++;
                }

            }

            totalWidth += (maxWidth * scaleFactor) + paint->textLetterSpacing;
            stbtt_FreeShape(&font->info,vertex);

            alesia__closePath(paint);
            alesia__fill(surface,paint);
        }
        else
            totalWidth += paint->textLetterSpacing;
    }

}

float alesia__computeTextPosition(AlesiaPaint* paint,AlesiaFont* font,float x,float y,char* txt,int info)
{
    const int strSize = strlen(txt);
    y += paint->fontSize;
    int i,ascent,descent,lineGap;
    stbtt_GetFontVMetrics(&font->info,&ascent,&descent,&lineGap);
    float totalWidth = 0;

    for (i = 0; i < strSize; i += priv__alesia__getUtf8CharSize(&txt[i])) //for every letter
    {
        if(txt[i] != ' ')
        {
            stbtt_vertex* vertex;
            int glyphCodePoint = priv__alesia__getUtf8(&txt[i]);
            int glyphIndex = stbtt_FindGlyphIndex(&font->info,glyphCodePoint);
            int count = stbtt_GetGlyphShape(&font->info,glyphIndex,&vertex);
            float scale = (float) paint->fontSize / ((float) ascent + (float) descent);
            int j;
            float maxWidth = 0;
            for(j = 0;j < count; j++) //for every character
                maxWidth = MAX(maxWidth,vertex[j].x);

            float scaleFactor = scale;//paint->fontSize / maxHeight;

            totalWidth += (maxWidth * scaleFactor) + paint->textLetterSpacing;
            stbtt_FreeShape(&font->info,vertex);
        }
        else
            totalWidth += paint->textLetterSpacing;
    }

    if(info == 3)
        return x + totalWidth;
    else if(info == 2)
        return y;
    else if(info == 1)
        return y + paint->fontSize;
    else if(info == 0)
        return x;

    return 0;
}

int priv__alesia__getUtf8(char* s)
{
    int k = *s ? __builtin_clz(~(*s << 24)) : 0;  // Count # of leading 1 bits.
    int mask = (1 << (8 - k)) - 1;                  // All 1's with k leading 0's.
    int value = *s & mask;
    for (++(s), --k; k > 0 && *s; --k, ++(s))
    {  // Note that k = #total bytes, or 0.
        value <<= 6;
        value += (*s & 0x3F);
    }
  return value;
}

int priv__alesia__getUtf8CharSize(char *s)
{
    int en = *s;
    if(en > 0)
        return 1;
    else if(en >= -64 && en < -32)
        return 2;
	else if(en >= -32 && en < -16)
        return 3;
	else if (en >= -16 && en < 0)
        return 4;
}
