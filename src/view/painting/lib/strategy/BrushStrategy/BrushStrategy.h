#ifndef BRUSHSTRATEGY_H
#define BRUSHSTRATEGY_H
#include "../../app/AppContext/AppContext.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../context/DrawingContext/DrawingContext.h"
#include "../../context/BrushContext/BrushContext.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/layer/Layers.h"


// #include "../../tools/Brush/Brush.h"

class BrushStrategy : IPressedStrategy {
private:
    Point _from;
    DrawingContext* _context;
    BrushContext* _brushContext;
    
    Editor* editor;
    Layer* layer;
    Surface* overlay;
    Preview* preview;
    Viewport* viewport;
    int screenWidth, screenHeight;
    
    void drawHorizontalBrush(Point to, Point from);
    void drawVerticalBrush(Point to, Point from);
    void stamp(Point pixel);
    void putMirroredPixel(int x, int y, unsigned int color);

public:
    BrushStrategy(BrushContext* brushContext, DrawingContext* context);

    void onPressed(int x, int y) override;
    void onTracking(int x, int y) override;
    void onRelease(int x, int y) override;
};
#endif