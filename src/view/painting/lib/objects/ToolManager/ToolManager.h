#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"
#include "../../strategy/NonePressedEvent/NonePressedEvent.h"
#include "../../interfaces/IToolContext/IToolContext.h"
#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"

#include "../../objects/EditorManager/EditorManager.h"

#include "../../context/CursorContext/CursorContext.h"
#include "../../context/ViewportContext/ViewportContext.h"
#include "../../context/DrawingContext/DrawingContext.h"
#include "../../context/SymmetryContext/SymmetryContext.h"
#include "../../context/LineContext/LineContext.h"
#include "../../context/EraserContext/EraserContext.h"
#include "../../context/BucketContext/BucketContext.h"
#include "../../context/CircleContext/CircleContext.h"

enum class KEY_MOUSE {
    UNPRESSED = -1,
    RIGHT_BUTTON = 0,
    WHELL_BUTTON = 1,
    LEFT_BUTTON = 2
};

struct ToolSettings{
    BrushContext    brushContext;
    DrawingContext  drawingContext;
    LineContext  lineContext;
    EraserContext  eraserContext;
    BucketContext  bucketContext;
    SymmetryContext symmetryContext;
    CircleContext circleContext;
};

class ToolManager {
    IPressedStrategy* _leftButtonPressed = nullptr;
    IPressedStrategy* _rightButtonPressed = nullptr;
    IPressedStrategy* _otherButtonPressed = nullptr;

    EditorManager* _editorManager = nullptr;
    ViewportContext* _viewport = nullptr;
    ToolRuntimeContext toolRuntimeContext;

    KEY_MOUSE buttonMousePressed = KEY_MOUSE::UNPRESSED; 
public:
    ToolManager(EditorManager* editorManager, ViewportContext* viewport);
    
    void finishActiveTool();
    void setRightToolPressed(IPressedStrategy* toolPressed);
    void setLeftToolPressed(IPressedStrategy* toolPressed);
    void setOtherToolPressed(IPressedStrategy* toolPressed);

    void onPressed();
    void onPressed(int x, int y, int button);
    void onTracking(int x, int y);
    void onReleased(int x, int y, int button);

    void onPinchPressed(int x1, int y1, int x2, int y2);
    void onPinchTracking(int x1, int y1, int x2, int y2);
    void onPinchReleased(int x1, int y1, int x2, int y2);

    void onScroll(int deltaY, int x, int y);



    IPressedStrategy* getToolPressed();
    CursorContext* getCursorContext();
    void build();
};
#endif