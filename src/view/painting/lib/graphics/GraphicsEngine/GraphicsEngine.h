#ifndef GRAPHICS_ENGINE
#define GRAPHICS_ENGINE

#include <algorithm>
#include <math.h>

#include "../Pixel/Pixel.h"

struct GraphicsEngine{
    static int clampedTilePoint(int point, int comprimento);
    static int pointMirrored(int point, int comprimento);
    static int pointMirrored(float point, float comprimento);
    static int pointMirrored(int point, int center, int comprimento);
    static Point rotate(Point point, float cx, float cy, float radians);
    
    
    static Color blendColors(const Color& bottomColor, const Color& topColor) ;
    static Color mix(const Color& bottomColor, const Color& topColor, const float& alpha);


    static bool computeVisibleShape(int originalAxis, int originalSize, int viewportSize, int& outStartAxis, int& outEndAxis);
};
#endif