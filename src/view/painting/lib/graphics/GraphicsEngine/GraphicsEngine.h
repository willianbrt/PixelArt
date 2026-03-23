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
    static void setOpacity(unsigned int& color, float opacity);
    static Point rotate(Point point, float cx, float cy, float radians);
    static unsigned int blendColors(unsigned int bottomColor, unsigned int topColor) ;
    static unsigned int mix(unsigned int bottomColor, unsigned int topColor, float alpha);
    static bool computeVisibleShape(int originalAxis, int originalSize, int viewportSize, int& outStartAxis, int& outEndAxis);
};
#endif