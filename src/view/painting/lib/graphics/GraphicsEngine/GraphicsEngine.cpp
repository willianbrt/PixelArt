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
Color GraphicsEngine::blendColors(const Color& bottomColor, const Color& topColor) {
    Color result = 0x00;

    result.alpha = topColor.alpha + bottomColor.alpha * (1.0f - topColor.alpha);
    if (result.alpha == 0.0f)
        return 0x00;

    result.red   = (topColor.red * topColor.alpha + bottomColor.red * bottomColor.alpha * (1.0f - topColor.alpha)) / result.alpha;
    result.green = (topColor.green * topColor.alpha + bottomColor.green * bottomColor.alpha * (1.0f - topColor.alpha)) / result.alpha;
    result.blue  = (topColor.blue * topColor.alpha + bottomColor.blue * bottomColor.alpha * (1.0f - topColor.alpha)) / result.alpha;

    return result;
}
Color GraphicsEngine::mix(const Color& bottomColor, const Color& topColor, const float& alpha) { 
    Color result = 0x00;
    result.alpha = alpha + bottomColor.alpha * (1.0f - alpha);

    if (result.alpha == 0.0f)
        return 0x00;

    result.red = (topColor.red * alpha + bottomColor.alpha * bottomColor.alpha * (1.0f - alpha)) / result.alpha;
    result.green = (topColor.green * alpha + bottomColor.alpha * bottomColor.green * (1.0f - alpha)) / result.alpha;
    result.blue = (topColor.blue * alpha + bottomColor.alpha * bottomColor.blue * (1.0f - alpha)) / result.alpha;

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