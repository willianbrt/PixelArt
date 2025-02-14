#include "main.h"

const char * canvasID = "#paintingCanvas";

KEY_MOUSE buttonPressed = KEY_MOUSE::NO_PRESSED;

IPressed* rightButtonTool = new EmptyPressed();
IPressed* middleButtonTool = new EmptyPressed();
IPressed* leftButtonTool = new EmptyPressed();
IScroll* scrollTool = new EmptyScroll();

void defineLeftPressedEvent(IPressed* tool){
    free(leftButtonTool);
    leftButtonTool = tool;
}

void defineMiddlePressedEvent(IPressed* tool){
    free(middleButtonTool);
    middleButtonTool = tool;
}

void defineScrollEvent(IScroll* tool){
    free(scrollTool);
    scrollTool = tool;
}


EM_BOOL OnPressedEvent(int eventType, const EmscriptenMouseEvent* e, void* userData){
    #ifdef __DEBUG
    printf("Pressed\n");
    #endif

    KEY_MOUSE button = static_cast<KEY_MOUSE>(e->button);

    if(buttonPressed != KEY_MOUSE::NO_PRESSED) return EM_TRUE;
    
    switch (button)
    {
    case KEY_MOUSE::LEFT_BUTTON:
        leftButtonTool->onPressed(Point(e->targetX, e->targetY));
        break;

    default:
    case KEY_MOUSE::MIDDLE_BUTTON:
        middleButtonTool->onPressed(Point(e->targetX, e->targetY));
        break;

    case KEY_MOUSE::RIGHT_BUTTON:
        rightButtonTool->onPressed(Point(e->targetX, e->targetY));
        break;
    }
    
    buttonPressed = button;

    return EM_TRUE;
}

EM_BOOL OnMoveEvent(int eventType, const EmscriptenMouseEvent* e, void* userData){
    #ifdef __DEBUG
    printf("Mouse moved to: %d, %d\n", e->targetX, e->targetY);
    #endif

    KEY_MOUSE button = static_cast<KEY_MOUSE>(e->button);

    if(buttonPressed == KEY_MOUSE::NO_PRESSED) return EM_TRUE;

    switch (button)
    {
    case KEY_MOUSE::LEFT_BUTTON:
        leftButtonTool->onTracking(Point(e->targetX, e->targetY));
        break;

    default:
    case KEY_MOUSE::MIDDLE_BUTTON:
        middleButtonTool->onTracking(Point(e->targetX, e->targetY));
        break;

    case KEY_MOUSE::RIGHT_BUTTON:
        rightButtonTool->onTracking(Point(e->targetX, e->targetY));
        break;
    }

    return EM_TRUE;
}

EM_BOOL OnLeave(int eventType, const EmscriptenMouseEvent* e, void* userData){
    switch (buttonPressed)
    {
    case KEY_MOUSE::LEFT_BUTTON:
        leftButtonTool->onTracking(Point(e->targetX, e->targetY));
        break;

    default:
    case KEY_MOUSE::MIDDLE_BUTTON:
        middleButtonTool->onTracking(Point(e->targetX, e->targetY));
        break;

    case KEY_MOUSE::RIGHT_BUTTON:
        rightButtonTool->onTracking(Point(e->targetX, e->targetY));
        break;
    }
    buttonPressed = KEY_MOUSE::NO_PRESSED;
    
    return EM_TRUE;
}
EM_BOOL OnReleaseEvent(int eventType, const EmscriptenMouseEvent* e, void* userData){
    #ifdef __DEBUG
    printf("Largar\n");
    #endif
    
    KEY_MOUSE button = static_cast<KEY_MOUSE>(e->button);

    if(button != buttonPressed)
        return EM_TRUE;

    switch (button)
    {
    case KEY_MOUSE::LEFT_BUTTON:
        leftButtonTool->onRelease(Point(e->targetX, e->targetY));
        break;

    default:
    case KEY_MOUSE::MIDDLE_BUTTON:
        middleButtonTool->onRelease(Point(e->targetX, e->targetY));
        break;

    case KEY_MOUSE::RIGHT_BUTTON:
        rightButtonTool->onRelease(Point(e->targetX, e->targetY));
        break;
    }
    
    buttonPressed = KEY_MOUSE::NO_PRESSED;
    
    return EM_TRUE;
}

EM_BOOL OnResizeEvent(int eventType, const EmscriptenUiEvent *e, void *userData){
    #ifdef __DEBUG
    printf("Redimensionando\n");
    #endif
    
    return EM_TRUE;
}

EM_BOOL OnWhellEvent(int eventType, const EmscriptenWheelEvent* e, void* userData){
    #ifdef __DEBUG
    printf("Wheel\n");
    #endif

    scrollTool->onScroll(e->deltaX, e->deltaY, Point(e->mouse.targetX, e->mouse.targetY));

    return EM_TRUE;
}


EMSCRIPTEN_KEEPALIVE;
int main(){
    emscripten_set_mousemove_callback(canvasID, nullptr, 1, OnMoveEvent);
    emscripten_set_mousedown_callback(canvasID, nullptr, 1, OnPressedEvent);
    emscripten_set_mouseup_callback(canvasID, nullptr, 1, OnReleaseEvent);
    emscripten_set_mouseleave_callback(canvasID, nullptr, 1, OnLeave);
    emscripten_set_wheel_callback(canvasID, nullptr, 1, OnWhellEvent);
    emscripten_set_resize_callback(canvasID, nullptr, 1, OnResizeEvent);

    emscripten_exit_with_live_runtime();
    return 0;
}
using namespace emscripten;
EMSCRIPTEN_BINDINGS(main_module){
    // emscripten::class_<IPressed>("IPressed");

    // emscripten::class_<PanningStrategy, emscripten::base<IPressed>>("PanningStrategy")
    //     .constructor<>();

    emscripten::function("defineLeftPressedEvent", &defineLeftPressedEvent, emscripten::allow_raw_pointer<IPressed*>());
    emscripten::function("defineMiddlePressedEvent", &defineMiddlePressedEvent, emscripten::allow_raw_pointer<IPressed*>());
    emscripten::function("defineScrollEvent", &defineScrollEvent, emscripten::allow_raw_pointer<IScroll*>());
};