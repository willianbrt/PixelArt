#ifndef GRAPHICS_ENGINE
#define GRAPHICS_ENGINE

#include <algorithm>
#include <math.h>

#include "../Pixel/Pixel.h"
#include "../Surface/Surface.h"

struct GraphicsEngine{
    static void translation(Surface* surface, Bounding bound, int deltaX, int deltaY);
    static void interpolation(Surface* surface, Bounding bound, float scaleX, float scaleY);
    static void rotate(Surface* surface, Surface* dirtSurface, Bounding bound, Point eixo, float radians);
    static Point rotate(Point point, Point eixo, float radians);
    static void blending(unsigned int& bottomColor, unsigned int topColor);
    static unsigned int blendColors(unsigned int bottomColor, unsigned int topColor) ;
    static bool computeVisibleShape(int originalAxis, int originalSize, int viewportSize, int& outStartAxis, int& outEndAxis);
};
#endif