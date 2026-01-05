#ifndef CIRCLE_H 
#define CIRCLE_H
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <algorithm>
#include <functional>

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/layer/Layers.h"

class Selection : public IGraphic{
public:
    enum ENUM_MARKER {
        TOP_LEFT = 0,
        BOTTOM_LEFT = 1,
        TOP_RIGHT = 2,
        BOTTOM_RIGHT = 3
    };

    Selection(int from_start_x, int from_start_y,
            int to_start_x, int to_start_y,
            Surface& layer,
            bool cleanTheArea);
    Bounding getBounding();
    Corners getDestinationCorners();
    void draw(Layer& layer);
    Point getCenter();
    int getResizedWidth();
    int getResizedHeight();

    void translate(float deltaX, float deltaY);
    void rotate(float rotateRad);
    void remove();
    Surface* copy();
    float getRotateRad();
    void resize(int marker, float deltaX, float deltaY);
private:
    float _dstCenterX;
    float _dstCenterY;
    float _scaleX = 1.0f;
    float _scaleY = 1.0f;

    float _angleRad;
    float _cosA;
    float _sinA;
    float _resizedWidth;
    float _resizedHeight;
    float _origCenterX;
    float _origCenterY; 
    bool _cleanTheArea;

    unsigned int _newColorHex;
    Surface* _data;
    Bounding _originalBounding;
    Bounding _destBounding;
    Corners _corners;
};

#endif