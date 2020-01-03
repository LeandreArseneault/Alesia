
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Math.h"
#include "Polygon.h"

#ifndef RENDER_H
#define RENDER_H

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
}AlesiaColor;

typedef struct
{
    AlesiaColor* memory;
    int width;
    int height;
}AlesiaSurface;

typedef enum
{
    ALESIA_QUALITY_HIGH = 1,
    ALESIA_QUALITY_LOW = 2
}RenderQuality;

typedef struct
{
    RenderQuality quality;
    int textLetterSpacing;
    float fontSize;
    AlesiaColor currentRenderColor;
    AlesiaColor strokeColor;

    AlesiaPolygon* workingPolygon;
    AlesiaPolygon* currentPolygon;


    AlesiaSurface* sourceSurface;
    int sourceSurfaceX;
    int sourceSurfaceY;
    int sourceSurfaceEndX;
    int sourceSurfaceEndY;

    AlesiaColor fillBuffer[RENDER_MAX_WIDTH];

}AlesiaPaint;

//surface API
AlesiaSurface* alesia__createSurface(int w,int h);

void alesia__destroySurface(AlesiaSurface* surface);

void alesia__resizeSurface(AlesiaSurface* surface,int w,int h);

void alesia__blitSurface(AlesiaSurface* dest,AlesiaSurface* src,int destX,int destY);

void alesia__setSurfaceColor(AlesiaSurface* surface,int x,int y,AlesiaColor color);

void alesia__blendSurfaceColor(AlesiaSurface* surface,int x,int y,AlesiaColor color);

void alesia__clearColor(AlesiaSurface* surface,AlesiaColor color);

AlesiaColor alesia__getSurfaceColor(AlesiaSurface* surface,int x,int y);

AlesiaColor* priv__alesia__getSurfacePixelPtr(AlesiaSurface* surface,int x,int y);


//public paint API

AlesiaPaint* alesia__createPaint();

void alesia__destroyPaint(AlesiaPaint* paint);

void alesia__setFontSize(AlesiaPaint* paint,float size);

void alesia__setLetterSpacing(AlesiaPaint* paint,float space);

void alesia__setPaintColor(AlesiaPaint* paint,AlesiaColor color);

void alesia__setStrokeColor(AlesiaPaint* paint,AlesiaColor color);

void alesia__setSourceSurface(AlesiaPaint* paint,AlesiaSurface* renderSurface,int startX,int startY,int endX,int endY);

void alesia__beginPath(AlesiaPaint* paint,int x,int y);

void alesia__beginSubPath(AlesiaPaint* paint,AlesiaOperation op,int x,int y);

void alesia__closePath(AlesiaPaint* paint);

void alesia__addLine(AlesiaPaint* paint,float x,float y);

void alesia__addBezier(AlesiaPaint* paint,float xc,float yc,float x2,float y2);

void alesia__addBezierCubic(AlesiaPaint* paint,float x1,float y1,float x2,float y2,float x3,float y3);

void alesia__fill(AlesiaSurface* surface,AlesiaPaint* paint);


//private render API

void priv__alesia__lineAA(AlesiaSurface* surface,AlesiaPaint* paint,float y,float* line,AlesiaVertex** vertices,int verticesCount);

int priv__alesia__fillLine(AlesiaPolygon* polygon,float* line,AlesiaVertex** vertices,float y);

int priv__alesia__operateFillLine(AlesiaOperation op,float* mainPolygon,AlesiaVertex** mainVertices,int mainCount,float* secondPolygon,AlesiaVertex** secondVertices,int secondCount);


//public misc

AlesiaColor alesia__makeColor(unsigned char r,unsigned char g,unsigned char b,unsigned char a);

AlesiaColor alesia__blendColor(AlesiaColor back,AlesiaColor front);



#endif

