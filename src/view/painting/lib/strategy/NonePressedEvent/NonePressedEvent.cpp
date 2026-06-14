#include "NonePressedEvent.h"

NonePressedEvent::NonePressedEvent()
{
}
void NonePressedEvent::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _cursorContext.enable = false;
    _cursorContext.scale = 1.0f;
}
void NonePressedEvent::onTracking(int x, int y){    
}
void NonePressedEvent::onRelease(){
}
bool NonePressedEvent::isInitialized(){
    return _initialized;
};
void NonePressedEvent::done(){};
void NonePressedEvent::abort(){};
CursorContext* NonePressedEvent::getCursorContext() {return &_cursorContext;};