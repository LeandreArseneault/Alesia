#include "Render.h"
#include "Math.h"

AlesiaSurface* alesia__createSurface(int w,int h)
{
    AlesiaSurface* surface = malloc(sizeof(AlesiaSurface));
    surface->memory = malloc(w * h * sizeof(AlesiaColor));
    surface->width = w;
    surface->height = h;
    return surface;
}

void alesia__destroySurface(AlesiaSurface* surface)
{
    free(surface->memory);
    free(surface);
}

void alesia__resizeSurface(AlesiaSurface* surface,int w,int h)
{

}

AlesiaPaint* alesia__createPaint()
{
    AlesiaPaint* result = malloc(sizeof(AlesiaPaint));
    result->quality = ALESIA_QUALITY_HIGH;
    result->textLetterSpacing = 5;
    result->fontSize = 14.0f;
    result->workingPolygon = NULL;
    result->sourceSurface = NULL;
    return result;
}

void alesia__destroyPaint(AlesiaPaint* paint)
{
    if(paint->workingPolygon != NULL)
        alesia__polygon__destroyPolygon(paint->workingPolygon);
    free(paint);
}

void alesia__setFontSize(AlesiaPaint* paint,float size)
{
    paint->fontSize = size;
}

void alesia__setLetterSpacing(AlesiaPaint* paint,float space)
{
    paint->textLetterSpacing = space;
}

void alesia__beginPath(AlesiaPaint* paint,int x,int y)
{
    if(paint->workingPolygon != NULL)
        alesia__polygon__destroyPolygon(paint->workingPolygon);

    paint->workingPolygon = alesia__polygon__createPolygon(x,y);
    paint->currentPolygon = paint->workingPolygon;
}

void alesia__beginSubPath(AlesiaPaint* paint,AlesiaOperation op,int x,int y)
{
    if(paint->workingPolygon != NULL) //make sure we're on a shape
    {
        paint->workingPolygon->minHeight = MIN(paint->workingPolygon->minHeight,y);
        paint->workingPolygon->maxHeight = MAX(paint->workingPolygon->maxHeight,y);


        paint->currentPolygon->next = alesia__polygon__createPolygon(x,y);
        paint->currentPolygon = paint->currentPolygon->next;
        paint->currentPolygon->operation = op;
    }
}

void alesia__closePath(AlesiaPaint* paint)
{
    AlesiaPolygon* currentPolygon = paint->workingPolygon;

    while(currentPolygon != NULL)
    {
        alesia__polygon__addLine(currentPolygon,currentPolygon->initX,currentPolygon->initY);
        currentPolygon = currentPolygon->next;
    }
}

void alesia__addLine(AlesiaPaint* paint,float x,float y)
{
    paint->workingPolygon->minHeight = MIN(paint->workingPolygon->minHeight,y);
    paint->workingPolygon->maxHeight = MAX(paint->workingPolygon->maxHeight,y);
    paint->workingPolygon->minWidth = MIN(paint->workingPolygon->minWidth,x);
    alesia__polygon__addLine(paint->currentPolygon,x,y);
}

void alesia__addBezier(AlesiaPaint* paint,float xc,float yc,float x2,float y2)
{
    paint->workingPolygon->minHeight = MIN(paint->workingPolygon->minHeight,y2);
    paint->workingPolygon->maxHeight = MAX(paint->workingPolygon->maxHeight,y2);
    paint->workingPolygon->minWidth = MIN(paint->workingPolygon->minWidth,x2);
    alesia__polygon__addCurve(paint->currentPolygon,xc,yc,xc,yc,x2,y2);
}

void alesia__addBezierCubic(AlesiaPaint* paint,float x1,float y1,float x2,float y2,float x3,float y3)
{
    alesia__polygon__addCurve(paint->currentPolygon,x1,y1,x2,y2,x3,y3);
}

void alesia__fill(AlesiaSurface* surface,AlesiaPaint* paint)
{
    if(paint->workingPolygon == NULL)
        return;

    const float minY = MAX(0,paint->workingPolygon->minHeight);
    const float maxY = MIN(surface->height,paint->workingPolygon->maxHeight);
    //fill use
    float y;
    for(y = minY;y <= maxY;++y) //for every line to render
    {
        //we start to make a list of vertices that the lines collide with
        int verticesCollisionCount = 0;
        AlesiaVertex* vertices[5000]; //if you go over 5000 thousands vertices for this line, what are you rendering m8
        float line[5000];

        verticesCollisionCount = priv__alesia__fillLine(paint->workingPolygon,&line,&vertices,y);

        AlesiaPolygon* currentPolygon = paint->workingPolygon->next;
        while(currentPolygon != NULL)
        {
            float secondLine[5000];
            AlesiaVertex* secondVertices[5000];
            int secondVerticeCount = priv__alesia__fillLine(currentPolygon,&secondLine,&secondVertices,y);
            verticesCollisionCount = priv__alesia__operateFillLine(currentPolygon->operation,&line,&vertices,verticesCollisionCount,&secondLine,&secondVertices,secondVerticeCount);
            currentPolygon = currentPolygon->next;
        }

        //Now we know with who we're colliding with for the line, we can start filling the line

        if(paint->sourceSurface== NULL && paint->currentRenderColor.a == 255) //filling set a flat color set, no alpha computation required
        {
            int i;
            for(i = 0; i < verticesCollisionCount - 1; i += 2)
            {
                int j;
                const int startLine = MAX(0,(int) (line[i]) + 1); //line + 1 since we don't want to render a not completely filled pixel
                const int endLine = MIN(surface->width,(int) line[i + 1]);
                for(j = startLine;j < endLine;j++)
                    alesia__setSurfaceColor(surface,j,y,paint->currentRenderColor);
            }
        }
        else if(paint->sourceSurface == NULL && paint->currentRenderColor.a != 255) //filling with a color blend
        {
            int i;
            for(i = 0; i < verticesCollisionCount - 1; i += 2)
            {
                int j;
                const int startLine = MAX(0,(int) (line[i]) + 1); //line + 1 since we don't want to render a not completely filled pixel
                const int endLine = MIN(surface->width,(int) line[i + 1]);
                for(j = startLine;j < endLine;j++)
                    alesia__blendSurfaceColor(surface,j,y,paint->currentRenderColor);
            }
        }
        else //filling with a source with no alpha computation
        {
            int i;
            const float xImgRatio = fabs((float) paint->sourceSurface->width / ((float) paint->sourceSurfaceEndX - paint->sourceSurfaceX));
            const float yImgRatio =fabs((float) paint->sourceSurface->height / ((float) paint->sourceSurfaceEndY - paint->sourceSurfaceY));
            for(i = 0; i < verticesCollisionCount - 1; i += 2)
            {
                AlesiaSurface* source = paint->sourceSurface;
                int j;
                const int startLine = MAX(0,(int) (line[i]) + 1); //line + 1 since we don't want to render a not completely filled pixel
                const int endLine = MIN(surface->width,(int) line[i + 1]);
                for(j = startLine;j < endLine;j++) //line + 1 since we don't want to render a not completely filled pixel
                {
                    int xImg = abs( (int)(((float)(j - paint->sourceSurfaceX)) * xImgRatio) % source->width);
                    int yImg = abs( (int)(((float)(y - paint->sourceSurfaceY)) * yImgRatio) % source->height);
                    alesia__blendSurfaceColor(surface,j,y,alesia__getSurfaceColor(source,xImg,yImg));

                    /*AlesiaColor* ptr = priv__alesia__getSurfacePixelPtr(surface,j,y);
                    AlesiaColor* srcPtr = priv__alesia__getSurfacePixelPtr(source,(int)(j - paint->sourceSurfaceX) % source->width,(int)(y - paint->sourceSurfaceY) % source->height);
                    int size = endLine - startLine;
                    memcpy(ptr,srcPtr,sizeof(AlesiaColor) * size);
                    j += size - 1;*/
                }
            }
        }

        //Now let's do the AA
        if(paint->quality == ALESIA_QUALITY_HIGH)
            ;//priv__alesia__lineAA(surface,paint,y,&line,&vertices,verticesCollisionCount);
    }

}

void priv__alesia__lineAA(AlesiaSurface* surface,AlesiaPaint* paint,float y,float* line,AlesiaVertex** vertices,int verticesCount)
{

    int i;
    for(i = 0;i < verticesCount;++i) //we compute AA for every vertices
    {
        AlesiaVertex* vertex = vertices[i];
        int start = i % 2 == 0;

        if(vertex->type == ALESIA_LINE) //the computation of AA for a line
        {
            AlesiaColor startAAColor = paint->currentRenderColor;
            float rate = fabs(priv__alesia__computeLineIncreaseRate(vertex->begin,vertex->end));
            if(rate > 0.75f)
            {
                float x = line[i];
                float yData = priv__alesia__computeLine(vertex->begin,vertex->end,x);
                if(start)
                    startAAColor.a *= 1.f - (x - floor(x));
                else
                    startAAColor.a *= (x - floor(x));
                alesia__blendSurfaceColor(surface,x,y,startAAColor);
            }
            else
            {
                //how many pixels of anti-aliasing for this line
                int aaXLenght = fabs(1.f / rate);

                if(aaXLenght < 1  && rate > 0.01f)
                    aaXLenght = 1;

                int gradient = startAAColor.a / (aaXLenght + 1);
                float j;
                for(j = 0;j <= aaXLenght;++j)
                {
                    float x;
                    if(start)
                        x = (line[i] - j);
                    else
                        x = (line[i] + j);
                    startAAColor.a -= gradient;
                    alesia__blendSurfaceColor(surface,x,y,startAAColor);
                }
            }
        }
        else if(vertex->type == ALESIA_BEZIER)
        {
             AlesiaColor bezierAAColor = paint->currentRenderColor;
             bezierAAColor.a *= line[i] - floor(i);
             if(start)
                     bezierAAColor.a *= 1.f - (line[i] - floor(i));
                else
                     bezierAAColor.a *= line[i] - floor(i);
             alesia__blendSurfaceColor(surface,line[i],y,bezierAAColor);
        }
    }
}

int priv__alesia__fillLine(AlesiaPolygon* polygon,float* line,AlesiaVertex** vertices,float y)
{
    inline int addVertice(float* poly,AlesiaVertex** vertices,int count,AlesiaVertex* newVertex,float newData)
    {
        int i,j;
        for(i = 0;i < count;++i)
        {
            if(newData <= poly[i]) //the insertion must be done here to keep the ordering
            {
                for(j = count;j > i ;--j)
                {
                    poly[j] = poly[j - 1];
                    vertices[j] = vertices[j - 1];
                }
                poly[i] = newData;
                vertices[i] = newVertex;
                return count + 1;
            }
        }
        poly[count] = newData;
        vertices[count] = newVertex;
        return count + 1;
    };
    //those 2 points represents a long segment used to see what vertices collide the current line to render
    AlesiaPoint p1;p1.x = -100000;
    AlesiaPoint p2;p2.x = 100000;
    p1.y = y + 0.5f;
    p2.y = y + 0.5f;

    //we start to make a list of vertices that the lines collide with
    int verticesCollisionCount = 0;
    int i = 0;
    for(i = 0;i < polygon->vertexCount;++i)
    {
        AlesiaVertex* vertex = &polygon->vertex[i];
        if(vertex->type == ALESIA_LINE)
        {
            AlesiaPoint begin = vertex->begin;
            AlesiaPoint end = vertex->end;
            if(begin.x > end.x)
            {
                AlesiaPoint buffer = begin;
                begin = end;
                end = buffer;
            }

            AlesiaPoint p;
            if(priv__alesia__lineIntersect(p1,p2,begin,end,&p) == TRUE)
            {
                float xValue = priv__alesia__getLineDomainAbs(begin,end,y);
                verticesCollisionCount = addVertice(line,vertices,verticesCollisionCount,vertex,xValue);
            }

        }
        else if(vertex->type == ALESIA_BEZIER)
        {
            float data[3];
            int resultCount = priv__alesia__getBezierTForValue(vertex->begin.y,vertex->yc,vertex->yc2,vertex->end.y,y + 0.5f,&data);
            if(resultCount > 0)
            {
                float xValue = priv__alesia__computeBezier(vertex->begin.x,vertex->xc,vertex->xc2,vertex->end.x,data[0]);
                verticesCollisionCount = addVertice(line,vertices,verticesCollisionCount,vertex,xValue);
            }
        }
    }

    return verticesCollisionCount;
}

int priv__alesia__operateFillLine(AlesiaOperation op,float* mainPolygon,AlesiaVertex** mainVertices,int mainCount,float* secondPolygon,AlesiaVertex** secondVertices,int secondCount)
{

    inline int addVertice(float* poly,AlesiaVertex** vertices,int count,AlesiaVertex* newVertex,float newData)
    {
        int i,j;
        for(i = 0;i < count;++i)
        {
            if(newData <= poly[i]) //the insertion must be done here to keep the ordering
            {
                for(j = count;j > i ;--j)
                {
                    poly[j] = poly[j - 1];
                    vertices[j] = vertices[j - 1];
                }
                poly[i] = newData;
                vertices[i] = newVertex;
                return count + 1;
            }
        }
        poly[count] = newData;
        vertices[count] = newVertex;
        return count + 1;
    };


    if(op == ALESIA_XOR)
    {
        int i,j;
        for(i = 0;i < secondCount - 1;i += 2)
        {
            int overlap = FALSE;
            for(j = 0;j < mainCount - 1;j += 2)
            {
                float p1X = mainPolygon[j];
                float p1W = mainPolygon[j + 1];
                float p2X = secondPolygon[i];
                float p2W = secondPolygon[i + 1];

                //there is mutliples cases
                if( p2X >= p1X && p2W <= p1W)//if our second segment thing is between segments
                {
                    mainCount = addVertice(mainPolygon,mainVertices,mainCount,secondVertices[i],secondPolygon[i]);
                    mainCount = addVertice(mainPolygon,mainVertices,mainCount,secondVertices[i + 1],secondPolygon[i + 1]);

                    overlap = TRUE;
                    j = mainCount;
                }
                else if(p2X <= p1X && p2W <= p1W && p2W >= p1W)//if our segment overlaps to the left
                {

                    mainCount = addVertice(mainPolygon,mainVertices,mainCount,secondVertices[i],secondPolygon[i]);
                    mainCount = addVertice(mainPolygon,mainVertices,mainCount,secondVertices[i + 1],secondPolygon[i + 1]);

                    overlap = TRUE;
                    j = mainCount;
                }
                else if(p2X >= p1X && p2X <= p1W && p2W >= p1W)//if our segments overlaps to the right
                {

                    mainCount = addVertice(mainPolygon,mainVertices,mainCount,secondVertices[i],secondPolygon[i]);
                    mainCount = addVertice(mainPolygon,mainVertices,mainCount,secondVertices[i + 1],secondPolygon[i + 1]);

                    overlap = TRUE;
                    j = mainCount;
                }

            }

            if(overlap == FALSE)
            {
                mainCount = addVertice(mainPolygon,mainVertices,mainCount,secondVertices[i],secondPolygon[i]);
                mainCount = addVertice(mainPolygon,mainVertices,mainCount,secondVertices[i + 1],secondPolygon[i + 1]);
            }

        }
    }
    else if(op == ALESIA_UNION)
    {
        int i,j;
        for(i = 0;i < secondCount - 1;i += 2)
        {
            int overlap = FALSE;
            for(j = 0;j < mainCount - 1;j += 2)
            {
                float p1X = mainPolygon[j];
                float p1W = mainPolygon[j + 1];
                float p2X = secondPolygon[i];
                float p2W = secondPolygon[i + 1];

                //there is mutliples cases
                if( p2X >= p1X && p2W <= p1W)//if our second segment thing is between segments
                {
                    //we do nothing and we close the loop
                    overlap = TRUE;
                    j = mainCount;
                }
                else if(p2X <= p1X && p2W <= p1W && p2W >= p1W)//if our segment overlaps to the left
                {
                    mainPolygon[j + 1] = p2X;
                    mainVertices[j + 1] = secondVertices[i];
                    overlap = TRUE;
                    j = mainCount;
                }
                else if(p2X >= p1X && p2X <= p1W && p2W >= p1W)//if our segments overlaps to the right
                {
                    mainPolygon[j + 1] = p2W;
                    mainVertices[j + 1] = secondVertices[i + 1];
                    overlap = TRUE;
                    j = mainCount;
                }

            }

            if(overlap == FALSE)
            {
                mainCount = addVertice(mainPolygon,mainVertices,mainCount,secondVertices[i],secondPolygon[i]);
                mainCount = addVertice(mainPolygon,mainVertices,mainCount,secondVertices[i + 1],secondPolygon[i + 1]);
            }

        }
    }

    return mainCount;
}

void alesia__setPaintColor(AlesiaPaint* paint,AlesiaColor color)
{
    paint->currentRenderColor.r = color.r;
    paint->currentRenderColor.g = color.g;
    paint->currentRenderColor.b = color.b;
    paint->currentRenderColor.a = color.a;

    int i;
    for(i = 0;i < RENDER_MAX_WIDTH;i++)
        paint->fillBuffer[i] = color;
}

void alesia__setStrokeColor(AlesiaPaint* paint,AlesiaColor color)
{
    paint->strokeColor = color;
}

void alesia__setSourceSurface(AlesiaPaint* paint,AlesiaSurface* renderSurface,int startX,int startY,int endX,int endY)
{
    paint->sourceSurface = renderSurface;
    paint->sourceSurfaceX = startX;
    paint->sourceSurfaceY = startY;
    paint->sourceSurfaceEndX = endX;
    paint->sourceSurfaceEndY = endY;
}

void alesia__blitSurface(AlesiaSurface* dest,AlesiaSurface* src,int destX,int destY)
{
    int x;
    int y;
    for(x = 0;x < src->width; x++)
        for(y = 0;y < src->height; y++)
            alesia__setSurfaceColor(dest,x + destX,y + destY,alesia__getSurfaceColor(src,x,y));
}

AlesiaColor alesia__makeColor(unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
    AlesiaColor result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

AlesiaColor alesia__blendColor(AlesiaColor back,AlesiaColor front)
{
    AlesiaColor r;
    r.a = (255 - (255 - back.a)*(255 - front.a));
    r.r   = (back.r   * (255 - front.a) + front.r   * front.a) / 255;
    r.g = (back.g * (255 - front.a) + front.g * front.a) / 255;
    r.b  = (back.b  * (255 - front.a) + front.b  * front.a) / 255;
    return r;
}

void alesia__setSurfaceColor(AlesiaSurface* surface,int x,int y,AlesiaColor color)
{
    surface->memory[surface->width * y + x] = color;
}

void alesia__blendSurfaceColor(AlesiaSurface* surface,int x,int y,AlesiaColor color)
{
    surface->memory[surface->width * y + x] = alesia__blendColor(surface->memory[surface->width * y + x],color);
}

void alesia__clearColor(AlesiaSurface* surface,AlesiaColor color)
{
    int x;
    for(x = 0;x < surface->width;x++)
    {
        int y;
        for(y = 0;y < surface->height;y++)
            alesia__setSurfaceColor(surface,x,y,color);
    }
}

AlesiaColor alesia__getSurfaceColor(AlesiaSurface* surface,int x,int y)
{
    return surface->memory[surface->width * y + x];
}

AlesiaColor* priv__alesia__getSurfacePixelPtr(AlesiaSurface* surface,int x,int y)
{
    return &surface->memory[surface->width * y + x];
}
