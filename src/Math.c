
#include "Math.h"

AlesiaPoint priv__alesia__makePoint(float x,float y)
{
    AlesiaPoint point;
    point.x = x;
    point.y = y;
    return point;
}

void priv__alesia__splitBezier(AlesiaBezier bezierToSplit,float t,AlesiaBezier* result1,AlesiaBezier* result2)
{
    AlesiaPoint p0 = bezierToSplit.p1;
    AlesiaPoint p1 = bezierToSplit.p2;
    AlesiaPoint p2 = bezierToSplit.p3;
    AlesiaPoint p3 = bezierToSplit.p4;
    AlesiaPoint p4 = priv__alesia__lerpPoint(p0, p1, t);
    AlesiaPoint p5 = priv__alesia__lerpPoint(p1, p2, t);
    AlesiaPoint p6 = priv__alesia__lerpPoint(p2, p3, t);
    AlesiaPoint p7 = priv__alesia__lerpPoint(p4, p5, t);
    AlesiaPoint p8 = priv__alesia__lerpPoint(p5, p6, t);
    AlesiaPoint p9 = priv__alesia__lerpPoint(p7, p8, t);

    result1->p1 = p0;
    result1->p2 = p4;
    result1->p3 = p7;
    result1->p4 = p9;
    result1->t *= 0.25f;

    result2->p1 = p9;
    result2->p2 = p8;
    result2->p3 = p6;
    result2->p4 = p3;
    result1->t *= 0.75f;
}

float priv__alesia__computeBezier(float x0,float x1,float x2,float x3,float t)
{
    return (1.f-t)*(1.f-t)*(1.f-t)*x0 + 3.f*(1.f-t)*(1.f-t)*t*x1 + 3.f*(1.f-t)*t*t*x2 + t*t*t*x3;
}

AlesiaRect priv__alesia__bezierRect(AlesiaBezier bez)
{
    AlesiaRect result;

    result.x = bez.p1.x;result.w = bez.p1.x;
    result.y = bez.p1.y;result.h = bez.p1.y;

    AlesiaPoint points[3];
    points[0] = bez.p2;
    points[1] = bez.p3;
    points[2] = bez.p4;
    int i;
    for(i = 0;i < 3;i++)
    {
        result.x = MIN(result.x,points[i].x);
        result.y = MIN(result.y,points[i].y);
        result.w = MAX(result.w,points[i].x);
        result.h = MAX(result.h,points[i].y);
    }

    result.w -= result.x;
    result.h -= result.y;
    return result;
}

int priv__alesia__bezierLineIntersect(AlesiaPoint l1,AlesiaPoint l2,AlesiaBezier bezier,float* finalT)
{
    //for simplicity, we use the same algoritm by create a fake bezier that is our "line"
    AlesiaBezier fakeBezier;
    fakeBezier.p1 = l1;
    fakeBezier.p4 = l2;
    fakeBezier.p2.x = l1.x + ((l2.x - l1.x) * 0.3f);
    fakeBezier.p2.y = l1.y + ((l2.y - l1.y) * 0.3f);
    fakeBezier.p3.x = l1.x + ((l2.x - l1.x) * 0.6f);
    fakeBezier.p3.y = l1.y + ((l2.y - l1.y) * 0.6f);

    fakeBezier.t = 1;
    bezier.t = 1;

    return priv__alesia__bezierBezierIntersect(fakeBezier,bezier,finalT);
}

int priv__alesia__bezierBezierIntersect(AlesiaBezier bezier1,AlesiaBezier bezier2,float* finalT)
{
    const float treshold = 1.f;
    AlesiaRect rect1 = priv__alesia__bezierRect(bezier1);
    AlesiaRect rect2 = priv__alesia__bezierRect(bezier2);

    //if they don't intersect, we don't bother with it
    if(priv__alesia__rectIntersect(rect1,rect2) == TRUE)
    {

        if(priv__alesia__rectArea(rect1) + priv__alesia__rectArea(rect2) <= treshold) // collision detected with the precision required
        {
            if(finalT != NULL)
                *finalT = bezier2.t;
            return TRUE;
        }


        AlesiaBezier b1a;
        AlesiaBezier b1b;
        priv__alesia__splitBezier(bezier1,0.5f,&b1a,&b1b);

        AlesiaBezier b2a;
        AlesiaBezier b2b;
        priv__alesia__splitBezier(bezier2,0.5f,&b2a,&b2b);

        return priv__alesia__bezierBezierIntersect(b1a, b2a,finalT) ||
        priv__alesia__bezierBezierIntersect(b1a, b2b,finalT) ||
        priv__alesia__bezierBezierIntersect(b1b, b2a,finalT) ||
        priv__alesia__bezierBezierIntersect(b1b, b2b,finalT);
    }

    return FALSE;
}


int priv__alesia__lineIntersect(AlesiaPoint p1,AlesiaPoint p2,AlesiaPoint p3,AlesiaPoint p4)
{
  float ua = 0.0;
  float ub = 0.0;
  float ud = (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y);

  if (ud != 0.0) {
    ua = ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x)) / ud;
    ub = ((p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x)) / ud;
        if (ua < 0.0 || ua >= 1.0 || ub < 0.0 || ub >= 1.0) ua = 0.0;
  }

  if(ua >0.0)
    return TRUE;
  return FALSE;
}

float priv__alesia__computeLineIncreaseRate(AlesiaPoint p1,AlesiaPoint p2)
{
    float ya = (p2.y - p1.y);
    float xa = (p2.x - p1.x);
    if(ya != 0 && xa != 0)
        return ya / xa;

    return 0.f;
}

float priv__alesia__computeLine(AlesiaPoint p1,AlesiaPoint p2,float x)
{
    float ya = (p2.y - p1.y);
    float xa = (p2.x - p1.x);
    float a;
    float b;

    if(ya != 0 && xa != 0)
    {
        a = ya / xa;
        b = p2.y - (a * p2.x);
        return a * x + b;
    }

    return x;
}

float priv__alesia__getLineDomainAbs(AlesiaPoint p1,AlesiaPoint p2,float y)
{
    float ya = (p2.y - p1.y);
    float xa = (p2.x - p1.x);
    float a;
    float b;

    if(ya != 0 && xa != 0)
    {
        a = ya / xa;
        b = p2.y - (a * p2.x);
        return (y - b) / a;
    }

    return p2.x;
}

float priv__alesia__lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

AlesiaPoint priv__alesia__lerpPoint(AlesiaPoint a,AlesiaPoint b,float t)
{
    AlesiaPoint result;
    result.x = priv__alesia__lerp(a.x,b.x,t);
    result.y = priv__alesia__lerp(a.y,b.y,t);
    return result;
}

int priv__alesia__rectIntersect(AlesiaRect box1,AlesiaRect box2)
{
   if((box2.x >= box1.x + box1.w)
	|| (box2.x + box2.w <= box1.x)
	|| (box2.y >= box1.y + box1.h)
	|| (box2.y + box2.h <= box1.y))
          return FALSE;
    return TRUE;
}

float priv__alesia__rectArea(AlesiaRect rect)
{
    return rect.w * rect.h;
}

int priv__alesia__circlePointIntersection(AlesiaPoint point,float xC,float yC,float rad)
{
    float d2 = (point.x-xC)*(point.x-xC) + (point.y-yC)*(point.y-yC);
    if (d2>rad*rad)
        return FALSE;
    return TRUE;
}

float priv__alesia__distancePoints(AlesiaPoint p1,AlesiaPoint p2)
{
    float square_difference_x = (p2.x - p1.x) * (p2.x - p1.x);
    float square_difference_y = (p2.y - p1.y) * (p2.y - p1.y);
    float sum = square_difference_x + square_difference_y;
    float value = sqrt(sum);
    return value;
}

int priv__alesia__solveCubicBezier(float pa, float pb, float pc, float pd,float x,float* result)
{

    float crt(float x)
    {
        return x < 0.0f ? -powf(-x, 1.f/3.f) : powf(x, 1.f/3.f);
    };

    int approximately(float a,float b)
    {
        return abs(a-b) < 0.0001f;
    };

    float
    pa3 = 3.f * pa,
    pb3 = 3.f * pb,
    pc3 = 3.f * pc,
    a = -pa  +   pb3 - pc3 + pd,
    b =  pa3 - 2.f*pb3 + pc3,
    c = -pa3 +   pb3,
    d =  pa  -     x;

  // Fun fact: any Bezier curve may (accidentally or on purpose)
  // perfectly model any lower order curve, so we want to test
  // for that: lower order curves are much easier to root-find.
  if (approximately(a, 0)) {
    // this is not a cubic curve.
    if (approximately(b, 0)) {
      // in fact, this is not a quadratic curve either.
      if (approximately(c, 0)) {
        // in fact in fact, there are no solutions.
        return 0;
      }
      // linear solution:
      result[0] = -d / c;
      return 1;
    }

    // quadratic solution:
    float
      q = sqrtf(c * c - 4.f * b * d),
      b2 = 2.f * b;
      result[0] =  (q - c) / b2;
      result[1] = (-c - q) / b2;
    return 2;
  }

  // At this point, we know we need a cubic solution,
  // and the above a/b/c/d values were technically
  // a pre-optimized set because a might be zero and
  // that would cause the following divisions to error.

  b /= a;
  c /= a;
  d /= a;

  float
    b3 = b / 3.f,
    p = (3.f * c - b*b) / 3.f,
    p3 = p / 3.f,
    q = (2.f * b*b*b - 9.f * b * c + 27.f * d) / 27.f,
    q2 = q / 2.f,
    discriminant = q2*q2 + p3*p3*p3,
    u1, v1;

  // case 1: three real roots, but finding them involves complex
  // maths. Since we don't have a complex data type, we use trig
  // instead, because complex numbers have nice geometric properties.
  if (discriminant < 0.f) {
    float
      mp3 = -p/3.f,
      r = sqrtf(mp3*mp3*mp3),
      t = -q / (2.f * r),
      cosphi = t < -1.f ? -1.f : t > 1.f ? 1.f : t,
      phi = acosf(cosphi),
      crtr = crt(r),
      t1 = 2.f * crtr;
      result[0] = t1 * cosf(phi / 3.f) - b3;
      result[1] = t1 * cosf((phi + TAU) / 3.f) - b3;
      result[2] =  t1 * cosf((phi + 2 * TAU) / 3.f) - b3;
    return 3;
  }

  // case 2: three real roots, but two form a "double root",
  // and so will have the same resultant value. We only need
  // to return two values in this case.
  else if (discriminant == 0.f) {
    u1 = q2 < 0 ? crt(-q2) : -crt(q2);
    result[0] = 2.f * u1 - b3;
    result[1] = -u1 - b3;
    return 2;
  }

  // case 3: one real root, 2 complex roots. We don't care about
  // complex results so we just ignore those and directly compute
  // that single real root.
  else {
    float sd = sqrtf(discriminant);
    u1 = crt(-q2 + sd);
    v1 = crt(q2 + sd);
    result[0] = u1 - v1 - b3;
    return 1;
  }
}

int priv__alesia__getBezierTForValue(float pa,float pb,float pc,float pd,float x,float* resultData)
{
    int result = 0;

    float data[3];
    int dataCount = priv__alesia__solveCubicBezier(pa,pb,pc,pd,x,&data);

    int i;
    for(i = 0;i < dataCount;i++)
    {
        if(data[i] > 0.f && data[i] < 1.f)
        {
            resultData[result] = data[i];
            result++;
        }
    }
    return result;
}

