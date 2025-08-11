#include "GraphicsEngine.h"

void GraphicsEngine::translation(Surface* surface, Bounding bound, int deltaX, int deltaY){
    int width = surface->getWidth();
    unsigned int* data = surface->getData();
    for(int y = bound.start.y; y < bound.end.y; y++){
        for(int x = bound.start.x; x < bound.end.x; x++){
            int index = x + width*y;
            int newIndex = (x + deltaX) + width*(y + deltaY);

            data[newIndex] = data[index];
            data[index] = 0x0;
        }
    }
}
void GraphicsEngine::interpolation(Surface* surface, Bounding bound, float scaleX, float scaleY){
    Surface dirtSurface =  surface->crop(bound);

    int resizedWidth = scaleX * bound.getWidth();
    int resizedHeight = scaleY * bound.getHeight();

    int width = surface->getWidth();

    unsigned int* data = surface->getData();
    for(int y = bound.start.y; y < bound.end.y; y++){
        for(int x = bound.start.x; x < bound.end.x; x++){
            int index = x + width*y;

            data[index] = 0x0;
        }
    }
}
void GraphicsEngine::rotate(Surface* surface, Surface* dirtSurface, Bounding bound, Point eixo, float radians){
    // float radians = deg * std::M_PI/180;
    // Point eixo = Point(bound.start.x + bound.getWidth()/2, bound.start.y + bound.getHeight()/2);
    // Point dirtStart = rotate(bound.start, rad);
    // Point dirtEnd = rotate(bound.end, rad);
    // Bounding newBounding = Bounding(start, end);
    
    unsigned int* data = surface->getData();
    int width = surface->getWidth();
    for(int y = bound.start.y; y < bound.end.y; y++){
        for(int x = bound.start.x; x < bound.end.x; x++){
            Point point = rotate(Point(x, y), eixo, radians);
            int rotatedIndex = point.x + bound.getWidth()*point.y;
            int index = x + width*y;

            data[index] = 0x0;
            data[rotatedIndex] = dirtSurface->getData()[index];
        }
    }
}
Point GraphicsEngine::rotate(Point point, Point eixo, float radians){
    float cos = std::cos(radians);
    float sin = 1 - cos;

    point.x -= eixo.x;
    point.y -= eixo.y;

    point.x = std::abs(point.x*cos - point.y*sin + eixo.x);
    point.y = std::abs(point.x*sin + point.y*cos + eixo.y);
    return point;
}
void GraphicsEngine::blending(unsigned int& bottomColor, unsigned int topColor) {
    const float alphaSrc = (topColor & 0xFF) / 255.0f;
    const float alphaDst = 1.0f - alphaSrc;

    bottomColor = 
        (static_cast<int>(alphaSrc * ((topColor >> 24) & 0xFF) + alphaDst * ((bottomColor >> 24) & 0xFF)) << 24) |
        (static_cast<int>(alphaSrc * ((topColor >> 16) & 0xFF) + alphaDst * ((bottomColor >> 16) & 0xFF)) << 16) |
        (static_cast<int>(alphaSrc * ((topColor >> 8) & 0xFF) + alphaDst * ((bottomColor >> 8) & 0xFF)) << 8) |
        (static_cast<int>(alphaSrc * (topColor  & 0xFF) + alphaDst * (bottomColor & 0xFF)));
}
unsigned int GraphicsEngine::blendColors(unsigned int bottomColor, unsigned int topColor) {
    float aTop = (topColor & 0xFF) / 255.0f;
    float aBottom = (bottomColor & 0xFF) / 255.0f;

    float rTop = ((topColor >> 24) & 0xFF) / 255.0f;
    float gTop = ((topColor >> 16) & 0xFF) / 255.0f;
    float bTop = ((topColor >>  8) & 0xFF) / 255.0f;

    float rBottom = ((bottomColor >> 24) & 0xFF) / 255.0f;
    float gBottom = ((bottomColor >> 16) & 0xFF) / 255.0f;
    float bBottom = ((bottomColor >>  8) & 0xFF) / 255.0f;

    // Alpha compositing (Normal mode)
    float outA = aTop + aBottom * (1.0f - aTop);
    float outR = (rTop * aTop + rBottom * aBottom * (1.0f - aTop)) / outA;
    float outG = (gTop * aTop + gBottom * aBottom * (1.0f - aTop)) / outA;
    float outB = (bTop * aTop + bBottom * aBottom * (1.0f - aTop)) / outA;

    unsigned int result =
        (static_cast<unsigned int>(outR * 255) << 24) |
        (static_cast<unsigned int>(outG * 255) << 16) |
        (static_cast<unsigned int>(outB * 255) << 8)  |
        (static_cast<unsigned int>(outA * 255));

    return result;
}
bool GraphicsEngine::computeVisibleShape(int originalAxis, int originalSize, int viewportSize, int& outStartAxis, int& outEndAxis){
    if (originalAxis <= -originalSize || originalAxis >= viewportSize){
        outEndAxis = 0;
        outStartAxis = 0;
        return false;
    }

    outStartAxis = std::max(0, originalAxis);
    outEndAxis = std::min(viewportSize, originalAxis + originalSize);

    return true;
}
