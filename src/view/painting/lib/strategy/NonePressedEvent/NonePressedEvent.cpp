#include "NonePressedEvent.h"

NonePressedEvent::NonePressedEvent()
{
    cursorContext = new CursorContext();
}
void NonePressedEvent::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
}
void NonePressedEvent::onTracking(int x, int y){    
}
void NonePressedEvent::onRelease(){
}
CursorContext* NonePressedEvent::getCursorContext(){
    return cursorContext;
}

void NonePressedEvent::done(){};
void NonePressedEvent::abort(){};