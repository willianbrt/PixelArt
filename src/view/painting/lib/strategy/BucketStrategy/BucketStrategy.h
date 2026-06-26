#ifndef BUCKETSTRATEGY_H
#define BUCKETSTRATEGY_H

#include "../../app/AppContext/AppContext.h"
#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"
#include "../../context/CursorContext/CursorContext.h"
#include "../../context/DrawingContext/DrawingContext.h"
#include "../../context/LineContext/LineContext.h"
#include "../../context/SymmetryContext/SymmetryContext.h"
#include "../../context/BucketContext/BucketContext.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/layer/Layers.h"
#include "../../objects/DrawingSession/DrawingSession.h"

class BucketStrategy  : public IPressedStrategy {
private:
    Point _from;
    DrawingContext* _drawingContext = nullptr;
    BucketContext* _bucketContext = nullptr;
    CursorContext _cursorContext;
    SymmetryContext* _symmetryContext = nullptr;
    
    ToolRuntimeContext _toolRuntimeContext;

    bool _initialized = false;
    
    void draw(const Point& pixel);

    
public:
    BucketStrategy(BucketContext* bucketContext, DrawingContext* context, SymmetryContext* symmetryContext);
    ~BucketStrategy();

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;
    bool isInitialized() override;
    void done() override;
    void abort() override;
    CursorContext* getCursorContext() override;
};
#endif