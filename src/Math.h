#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define PI 3.1415f
#define TAU 6.283185f


typedef struct
{
    float x;
    float y;
}AlesiaPoint;

typedef struct
{
    float x;
    float y;
    float w;
    float h;
}AlesiaRect;

typedef struct
{
    AlesiaPoint p1; //begin
    AlesiaPoint p2; //control 1
    AlesiaPoint p3; //control 2
    AlesiaPoint p4; //end
    float t;
}AlesiaBezier;

AlesiaPoint priv__alesia__makePoint(float x,float y);

void priv__alesia__splitBezier(AlesiaBezier bezierToSplit,float t,AlesiaBezier* result1,AlesiaBezier* result2);

float priv__alesia__computeBezier(float x0,float x1,float x2,float x3,float t);

AlesiaRect priv__alesia__bezierRect(AlesiaBezier bez);

int priv__alesia__bezierLineIntersect(AlesiaPoint l1,AlesiaPoint l2,AlesiaBezier bezier,float* finalT);

int priv__alesia__bezierBezierIntersect(AlesiaBezier bezier1,AlesiaBezier bezier2,float* finalT);

int priv__alesia__lineIntersect(AlesiaPoint p1,AlesiaPoint p2,AlesiaPoint p3,AlesiaPoint p4);

float priv__alesia__computeLineIncreaseRate(AlesiaPoint p1,AlesiaPoint p2);

float priv__alesia__computeLine(AlesiaPoint p1,AlesiaPoint p2,float y);

float priv__alesia__getLineDomainAbs(AlesiaPoint p1,AlesiaPoint p2,float y);

float priv__alesia__lerp(float a, float b, float f);

AlesiaPoint priv__alesia__lerpPoint(AlesiaPoint a,AlesiaPoint b,float f);

int priv__alesia__rectIntersect(AlesiaRect box1,AlesiaRect box2);

float priv__alesia__rectArea(AlesiaRect rect);

int priv__alesia__circlePointIntersection(AlesiaPoint point,float xC,float yC,float rad);

float priv__alesia__distancePoints(AlesiaPoint p1,AlesiaPoint p2);

int priv__alesia__solveCubicBezier(float pa, float pb, float pc, float pd,float x,float* result);

int priv__alesia__getBezierTForValue(float pa,float pb,float pc,float pd,float x,float* resultData);


#endif // MATH_H_INCLUDED
