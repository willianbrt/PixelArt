#ifndef SELECTSTRATEGY_H
#define SELECTSTRATEGY_H
#include "../../app/AppContext/AppContext.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"
#include "../../context/CursorContext/CursorContext.h"
#include "../../context/DrawingContext/DrawingContext.h"
#include "../../context/BrushContext/BrushContext.h"
#include "../../context/SymmetryContext/SymmetryContext.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/layer/Layers.h"

#include <optional>

class SelectStrategy : public IPressedStrategy {
private:
    enum ENUM_MARKER {
        TOP_LEFT = 0,
        BOTTOM_LEFT = 1,
        TOP_RIGHT = 2,
        BOTTOM_RIGHT = 3,
        UNKNOW = -1,
    };
    enum ENUM_MODE {
        SELECT = 0,
        SELECTED = 1,
        TRANSLATE = 2,
        RESIZE_TOP_LEFT = 3,
        RESIZE_TOP_RIGHT = 4,
        RESIZE_BOTTOM_LEFT = 5,
        RESIZE_BOTTOM_RIGHT = 6,
        ROTATE_TOP_LEFT = 7,
        ROTATE_TOP_RIGHT = 8,
        ROTATE_BOTTOM_LEFT = 9,
        ROTATE_BOTTOM_RIGHT = 10,
    };

    Point _to,_from;
    SymmetryContext* _symmetryContext;
    SelectContext* _selectContext;
    ToolRuntimeContext _toolRuntimeContext;

    CursorContext* cursorContext;

    int sizeHitbox;
    ENUM_MODE _mode;


    void putMirroredPixel(int x, int y, unsigned int color);
    optional<Bounding> getBounding();
    void draw();

    void translate(float deltaX, float deltaY);
    void rotate(float rotateRad);
    void remove();
    Surface* copy();
    void paste(Surface& surface);
    void resize(int marker, float deltaX, float deltaY);


    void start();
    void abort();

public:
    SelectStrategy(SelectContext* selectContext, SymmetryContext* context);
    ~SelectStrategy();

    void onPressed(int x, int y, ToolRuntimeContext toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease(int x, int y) override;

    CursorContext* getCursorContext() override;
    Corners getDestinationCorners();
    bool insideCornerHitbox(Point p, Point cornerPosition);


    
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
    std::optional<Bounding> _originalBounding;
    
    Corners _flagCorners;
    Bounding flagBounding;
};
#endif