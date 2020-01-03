#ifndef POLYGON_H_INCLUDED
#define POLYGON_H_INCLUDED
#include <string.h>
#include "Info.h"
#include "Math.h"

typedef enum
{
    ALESIA_UNION = 1,
    ALESIA_INTERSECT = 2,
    ALESIA_SUBSTRACT = 3,
    ALESIA_XOR = 4
}AlesiaOperation;

typedef enum
{
    ALESIA_LINE,
    ALESIA_BEZIER
}AlesiaVertexType;


typedef struct
{
    AlesiaVertexType type;
    float xc;
    float yc;
    float xc2;
    float yc2;

    AlesiaPoint begin;
    AlesiaPoint end;
}AlesiaVertex;

typedef struct
{
    AlesiaOperation operation;

    AlesiaVertex* vertex;
    int vertexCount;

    int minHeight;
    int maxHeight;
    int minWidth;

    struct AlesiaPolygon* next;

    int initX;
    int initY;
    int lastX;
    int lastY;
}AlesiaPolygon;

//polygon logic
AlesiaPolygon* alesia__polygon__createPolygon(float x,float y);

void alesia__polygon__destroyPolygon(AlesiaPolygon* polygon);

void alesia__polygon__addLine(AlesiaPolygon* polygon,int x2,int y2);

void alesia__polygon__addCurve(AlesiaPolygon* polygon,float xc,float yc,float xc2,float yc2,int endX,int endY);

void priv__alesia__polygon__addVertex(AlesiaPolygon* polygon,AlesiaVertex vertex);

#endif // POLYGON_H_INCLUDED
