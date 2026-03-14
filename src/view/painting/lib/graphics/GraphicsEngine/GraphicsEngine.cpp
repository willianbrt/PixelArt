#include "GraphicsEngine.h"

Point GraphicsEngine::rotate(Point point, float cx, float cy, float radians){
    float cos = std::cos(radians);
    float sin = std::sin(radians);

    float dx = (float)point.x - cx;
    float dy = (float)point.y - cy;

    float rx = dx * cos + dy * -sin;
    float ry = dx * sin + dy * cos;
    
    point.x = static_cast<int>(std::round(rx + cx));
    point.y = static_cast<int>(std::round(ry + cy));
    return point;
}
unsigned int GraphicsEngine::blendColors(unsigned int bottomColor, unsigned int topColor) {
    float aTop = (topColor & 0xFF) / 255.0f;
    float bTop = (topColor >> 8 & 0xFF) / 255.0f;
    float gTop = (topColor >> 16 & 0xFF) / 255.0f;
    float rTop = (topColor >> 24 & 0xFF) / 255.0f;

    float aBottom= (bottomColor & 0xFF) / 255.0f;
    float bBottom= (bottomColor >> 8 & 0xFF) / 255.0f;
    float gBottom= (bottomColor >> 16 & 0xFF) / 255.0f;
    float rBottom= (bottomColor >> 24 & 0xFF) / 255.0f;
    
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

int GraphicsEngine::clampedTilePoint(int point, int comprimento){
    return (point+comprimento) % comprimento; 
}
int GraphicsEngine::pointMirrored(int point, int comprimento){
    return comprimento - point - 1; 
}
int GraphicsEngine::pointMirrored(float point, float comprimento){
    return comprimento - point - 1; 
}
int GraphicsEngine::pointMirrored(int point, int center, int comprimento){
    return comprimento - center - point - 1; 
}
void GraphicsEngine::setOpacity(unsigned int& color, float opacity){
    // color = static_cast<int>(opacity * (color >> 24 & 0xFF)) << 24 | (color & 0x00FFFFFF);
    color = (color & 0xFFFFFF00) | static_cast<int>(opacity * (color & 0xFF));
}