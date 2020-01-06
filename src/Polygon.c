
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
    vertex.bezier.p1 = vertex.begin;
    vertex.bezier.p4 = vertex.end;
    vertex.bezier.p2 = priv__alesia__makePoint(xc,yc);
    vertex.bezier.p3 = priv__alesia__makePoint(xc2,yc2);
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

int priv__alesia__isPointInPolygon(AlesiaPolygon* polygon,float x,float y)
{

    AlesiaVertex fakeVertex;
    fakeVertex.type = ALESIA_LINE;
    fakeVertex.begin.y = y;
    fakeVertex.begin.x = -100000;
    fakeVertex.end = priv__alesia__makePoint(x,y);
    int i;
    unsigned int collision = 0;
    for(i = 0; i < polygon->vertexCount;i++)
    {
        AlesiaPoint point[10];
        if(priv__alesia__vertexIntersect(fakeVertex,polygon->vertex[i],&point) > 0)
            collision++;
    }

    if(collision % 2 == 0)
        return FALSE;
    return TRUE;
}

AlesiaPolygon* priv__alesia__operatePolygon(AlesiaOperation op,AlesiaPolygon* base,AlesiaPolygon* operand)
{
    AlesiaPolygon* result = alesia__polygon__createPolygon(base->initX,base->initY);

    int i,j;

    for(i = 0; i < base->vertexCount;i++)
    {
        for(j = 0;j < operand->vertexCount;j++)
        {
            AlesiaVertex* operandVertex = &operand->vertex[j];
            AlesiaVertex* baseVertex = &base->vertex[i];

            AlesiaPoint points[10];
            int collisionCount = priv__alesia__vertexIntersect(*operandVertex,*baseVertex,&points);
            int k;

            for(k = 0;k < collisionCount;k++)
            {
                AlesiaPoint p1;
                AlesiaPoint p2 = points[k];

                if(k == 0)
                    p1 = operandVertex->begin;
                else if(k == collisionCount - 1)
                    p1 = operandVertex->end;
                else
                    p1 = points[k - 1];

                //we split the operand vertice
                if(baseVertex->type == ALESIA_LINE)
                {
                    if(operandVertex->type == ALESIA_LINE)
                    {

                    }
                    else if(operandVertex->type == ALESIA_BEZIER)
                    {

                    }
                }
            }

            if(collisionCount == 0) //No collision we add the vertex directly
            {
                //one condition that lines is not exactly inside the shape
                //if the points is in the shape, we remove the vertex completely
                if(priv__alesia__isPointInPolygon(base,operandVertex->begin.x,operandVertex->begin.y) == FALSE)
                    priv__alesia__polygon__addVertex(result,*baseVertex);
            }
        }
    }


    return result;
}

int priv__alesia__vertexIntersect(AlesiaVertex v1,AlesiaVertex v2,AlesiaPoint* points)
{
    AlesiaPoint* p1 = NULL;
    AlesiaPoint* p2 = NULL;
    AlesiaPoint* p3 = NULL;
    AlesiaPoint* p4 = NULL;
    AlesiaBezier* bez1 = NULL;
    AlesiaBezier* bez2 = NULL;

    if(v1.type == ALESIA_LINE)
    {
        p1 = &v1.begin;
        p2 = &v1.end;
    }
    else if(v1.type == ALESIA_BEZIER)
    {
        bez1 = &v1.bezier;
    }

    if(v2.type == ALESIA_LINE)
    {
        if(p1 == NULL)
        {
            p1 = &v2.begin;
            p2 = &v2.end;
        }
        else
        {
            p3 = &v2.begin;
            p4 = &v2.end;
        }
    }
    else if(v2.type == ALESIA_BEZIER)
    {
        if(bez1 == NULL)
            bez1 = &v2.bezier;
        else
            bez2 = &v2.begin;
    }

    if(bez1 != NULL & bez2 != NULL)
        return priv__alesia__bezierBezierIntersect(*bez1,*bez2,points);
    else if(p1 != NULL && p3 != NULL)
        return priv__alesia__lineIntersect(*p1,*p2,*p3,*p4,points);
    return priv__alesia__bezierLineIntersect(*p1,*p2,*bez1,points);
}
