#include "NonePressedEvent.h"

NonePressedEvent::NonePressedEvent()
{
    cursorContext = new CursorContext();
}
void NonePressedEvent::onPressed(int x, int y, ToolRuntimeContext toolRuntimeContext){
}
void NonePressedEvent::onTracking(int x, int y){    
}
void NonePressedEvent::onRelease(int x, int y){
}
CursorContext* NonePressedEvent::getCursorContext(){
    return cursorContext;
}