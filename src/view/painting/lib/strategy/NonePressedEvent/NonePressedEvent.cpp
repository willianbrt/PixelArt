#include "NonePressedEvent.h"

NonePressedEvent::NonePressedEvent()
{
}
void NonePressedEvent::onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext){
    _toolRuntimeContext = toolRuntimeContext;
    _toolRuntimeContext.cursor->enable = false;
    _toolRuntimeContext.cursor->scale = 1.0f;
}
void NonePressedEvent::onTracking(int x, int y){    
}
void NonePressedEvent::onRelease(){
}
void NonePressedEvent::done(){};
void NonePressedEvent::abort(){};