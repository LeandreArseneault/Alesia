
#include "Polygon.h"

//polygon logic

AlesiaPolygon* alesia__polygon__createPolygon(float x,float y)
{
    AlesiaPolygon* result = calloc(sizeof(AlesiaPolygon),1);
    result->next = NULL;
    result->vertex = NULL;
    result->vertexCount = 0;
    result->initX = x;
    result->initY = y;
    result->lastX = x;
    result->lastY = y;
    result->minHeight = y;
    result->maxHeight = y;
    result->minWidth = x;
    return result;
}

void alesia__polygon__destroyPolygon(AlesiaPolygon* polygon)
{
    if(polygon->next != NULL)
        alesia__polygon__destroyPolygon(polygon->next);

    free(polygon->vertex);
    free(polygon);
}

void alesia__polygon__addLine(AlesiaPolygon* polygon,int x2,int y2)
{
    AlesiaVertex vertex;
    vertex.type = ALESIA_LINE;
    vertex.begin = priv__alesia__makePoint(polygon->lastX,polygon->lastY);
    vertex.end = priv__alesia__makePoint(x2,y2);
    priv__alesia__polygon__addVertex(polygon,vertex);

    polygon->lastX = x2;
    polygon->lastY = y2;
    polygon->maxHeight = MAX(polygon->maxHeight,y2);
    polygon->minHeight = MIN(polygon->minHeight,y2);
    polygon->minWidth = MIN(polygon->minWidth,x2);
}

void priv__alesia__polygon__addVertex(AlesiaPolygon* polygon,AlesiaVertex newVertex)
{
    polygon->vertexCount++;
    polygon->vertex = realloc(polygon->vertex,polygon->vertexCount * sizeof(AlesiaVertex));
    polygon->vertex[polygon->vertexCount - 1] = newVertex;
}

void alesia__polygon__addCurve(AlesiaPolygon* polygon,float xc,float yc,float xc2,float yc2,int endX,int endY)
{

    AlesiaVertex vertex;
    vertex.type = ALESIA_BEZIER;
    vertex.begin = priv__alesia__makePoint(polygon->lastX,polygon->lastY);
    vertex.end = priv__alesia__makePoint(endX,endY);
    vertex.xc2 = xc2;
    vertex.yc2 = yc2;
    vertex.xc = xc;
    vertex.yc = yc;
    priv__alesia__polygon__addVertex(polygon,vertex);

    polygon->lastX = endX;
    polygon->lastY = endY;

    polygon->minHeight = MIN(polygon->minHeight,yc);
    polygon->minHeight = MIN(polygon->minHeight,yc2);
    polygon->minHeight = MIN(polygon->minHeight,endY);

    polygon->maxHeight = MAX(polygon->maxHeight,yc);
    polygon->maxHeight = MAX(polygon->maxHeight,yc2);
    polygon->maxHeight = MAX(polygon->maxHeight,endY);

    polygon->minWidth = MAX(polygon->minWidth,xc);
    polygon->minWidth = MAX(polygon->minWidth,xc2);
    polygon->minWidth = MAX(polygon->minWidth,endX);
}
