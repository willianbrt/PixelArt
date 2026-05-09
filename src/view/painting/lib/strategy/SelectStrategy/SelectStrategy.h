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
        RESIZE = 3,
        ROTATE = 4,
    };

    Point _from;
    SymmetryContext* _symmetryContext;
    SelectContext* _selectContext;
    ToolRuntimeContext _toolRuntimeContext;

    CursorContext* cursorContext;

    int sizeHitbox = 1;
    ENUM_MODE _mode;
    ENUM_MARKER _activeMarker;


    void putMirroredPixel(int x, int y, unsigned int color);
    void draw();

    void translate(float deltaX, float deltaY);
    void rotate(float rotateRad);
    Surface* copy();
    void paste(Surface& surface);
    void resize(int marker, float deltaX, float deltaY);

    void start();

public:
    SelectStrategy(SelectContext* selectContext, SymmetryContext* context);
    ~SelectStrategy();

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;

    CursorContext* getCursorContext() override;
    void done() override;
    void abort() override;

    bool insideCornerHitbox(Point p, Point cornerPosition);
    bool insideCornerRotateHitbox(Point p, Point cornerPosition);


    
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
    bool _cutting;

    std::optional<Bounding> _originalBounding;
    
    Corners _flagCorners;
    Bounding flagBounding;
};
#endif