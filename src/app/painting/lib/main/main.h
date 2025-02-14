#ifndef MAIN_H
#define MAIN_H

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/bind.h>
#include <iostream>


#include "../helpers/Pixel/Pixel.h"
#include "../interfaces/IPressed/IPressed.h"
#include "../interfaces/IScroll/IScroll.h"

#include "../strategy/EmptyPressed/EmptyPressed.h"
#include "../strategy/EmptyScroll/EmptyScroll.h"

extern const char * canvasID;

enum class KEY_MOUSE
{
    LEFT_BUTTON = 0,
    MIDDLE_BUTTON = 1,
    RIGHT_BUTTON = 2,
    NO_PRESSED = -1
};

extern KEY_MOUSE buttonPressed;

extern IPressed* rightButtonTool;
extern IPressed* middleButtonTool;
extern IPressed* leftButtonTool;
extern IScroll* scrollTool;

extern void defineLeftPressedEvent(IPressed* tool);
extern void defineMiddlePressedEvent(IPressed* tool);
extern void defineScrollEvent(IScroll* tool);

EM_BOOL OnPressedEvent(int eventType, const EmscriptenMouseEvent* e, void* userData);
EM_BOOL OnMoveEvent(int eventType, const EmscriptenMouseEvent* e, void* userData);
EM_BOOL OnReleaseEvent(int eventType, const EmscriptenMouseEvent* e, void* userData);
EM_BOOL OnResizeEvent(int eventType, const EmscriptenUiEvent *e, void *userData);
EM_BOOL OnWhellEvent(int eventType, const EmscriptenWheelEvent* e, void* userData);

EMSCRIPTEN_KEEPALIVE;
int main();

#endif