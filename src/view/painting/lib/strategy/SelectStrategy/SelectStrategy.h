#ifndef SELECTSTRATEGY_H
#define SELECTSTRATEGY_H
#include "../../app/AppContext/AppContext.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../context/DrawingContext/DrawingContext.h"
#include "../../context/BrushContext/BrushContext.h"
#include "../../context/SymmetryContext/SymmetryContext.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/layer/Layers.h"

class SelectStrategy : public IPressedStrategy {
private:
    enum ENUM_MARKER {
        TOP_LEFT = 0,
        BOTTOM_LEFT = 1,
        TOP_RIGHT = 2,
        BOTTOM_RIGHT = 3
    };

    Point _from;
    IToolContext* _context;
    
    Editor* editor;
    Layer* layer;
    Surface* overlay;
    Preview* preview;
    Viewport* viewport;
    HoverPreview* hoverPreview; 
    Pattern _pattern;
    
    int _widthPattern, _heightPattern;
    int screenWidth, screenHeight;


    int hitbox;
    Point corner[4];
    

    void stamp(Point pixel);
    void putMirroredPixel(int x, int y, unsigned int color);

    Bounding getBounding();
    void draw();

    Point getCenter();
    int getResizedWidth();
    int getResizedHeight();

    void translate(float deltaX, float deltaY);
    void rotate(float rotateRad);
    void remove();
    Surface* copy();
    float getRotateRad();
    void resize(int marker, float deltaX, float deltaY);

    void startSelection(int from_start_x, int from_start_y,
            int to_start_x, int to_start_y,
            Surface& surface,
            bool cleanTheArea);

public:
    SelectStrategy(SymmetryContext* context);
    ~SelectStrategy();

    void onPressed(int x, int y) override;
    void onTracking(int x, int y) override;
    void onRelease(int x, int y) override;

    HoverPreview* getHoverPreview() override;
    Corners getDestinationCorners();
    bool insideCornerHitbox(Point p, int size);


    
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