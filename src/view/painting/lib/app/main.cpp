#include "AppContext/AppContext.h"
#include <emscripten.h>
#include <emscripten/html5.h>

AppContext& app = AppContext::instance();
ToolManager* toolManager;

EM_JS(void, windowCursorToCanvas, (int positionX, int positionY, int* positionOutX, int* positionOutY), {
    const canvas = Module.canvas.getBoundingClientRect();
    
     HEAP32[positionOutX>>2] = Math.floor((positionX - canvas.left)/ (Module.canvas.clientWidth / canvas.width));
     HEAP32[positionOutY>>2] = Math.floor((positionY - canvas.top)/ (Module.canvas.clientWidth / canvas.width));;
});
EM_JS(void, getCanvasRect, (const char* id, int* width, int* height), {    
    const element = document.querySelector(UTF8ToString(id));
    const viewportSize = element.getBoundingClientRect();
    
     HEAP32[width>>2] = element.clientWidth;
     HEAP32[height>>2] = element.clientHeight;
});

int canvasWidth;
int canvasHeight;

int main()
{
    getCanvasRect("#drawing-area", &canvasWidth, &canvasHeight);

    app.build(canvasWidth, canvasHeight);
    toolManager = app.getToolManager();

    emscripten_set_mousedown_callback("#painting", nullptr, EM_FALSE, [](int eventType, const EmscriptenMouseEvent *e, void *userData){
        toolManager->onPressed(e->targetX, e->targetY, e->button);
        
        return EM_TRUE;
    });
    emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, [](int eventType, const EmscriptenMouseEvent *e, void *userData){
        Point point;
        windowCursorToCanvas(e->targetX, e->targetY, &point.x, &point.y);

        toolManager->onTracking(point.x, point.y);

        return EM_TRUE;
    });
    emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, [](int eventType, const EmscriptenMouseEvent *e, void *userData){
        Point point;
        windowCursorToCanvas(e->targetX, e->targetY, &point.x, &point.y);

        toolManager->onReleased(point.x, point.y, e->button);

        return EM_TRUE;
    });
    emscripten_set_wheel_callback("#painting", nullptr, EM_FALSE, [](int eventType, const EmscriptenWheelEvent *e, void *userData){
        Point point;
        windowCursorToCanvas(e->mouse.targetX, e->mouse.targetY, &point.x, &point.y);
        
        toolManager->onScroll(e->deltaY, point.x, point.y);

        return EM_TRUE;
    });
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, [](int eventType, const  EmscriptenUiEvent *e, void *userData){
        getCanvasRect("#drawing-area", &canvasWidth, &canvasHeight);
        app.resize(canvasWidth, canvasHeight);

        return EM_TRUE;
    });


    emscripten_set_main_loop_arg(AppContext::loop, &AppContext::instance(), 0, true);
    return 0;
}
#ifdef __EMSCRIPTEN__
    #include <emscripten/bind.h>

    using namespace emscripten;
    EMSCRIPTEN_BINDINGS(pixel_editor_module){};
#endif