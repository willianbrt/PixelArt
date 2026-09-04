#include "ToolManager.h"
ToolManager::ToolManager(EditorManager* editorManager, ViewportContext* viewport) :
_editorManager(editorManager),
_viewport(viewport)
{
    _leftButtonPressed = new NonePressedEvent();
    _rightButtonPressed = new NonePressedEvent();
    _otherButtonPressed = new NonePressedEvent();

}

void ToolManager::finishActiveTool(){
    if(!_rightButtonPressed) return;
    if(_rightButtonPressed->isInitialized()){
        _rightButtonPressed->done();
    }
}

void ToolManager::setRightToolPressed(IPressedStrategy* toolPressed){
    finishActiveTool();
    
    _rightButtonPressed = toolPressed;
}
void ToolManager::setLeftToolPressed(IPressedStrategy* toolPressed){
    _leftButtonPressed = toolPressed;
}
void ToolManager::setOtherToolPressed(IPressedStrategy* toolPressed){
    _otherButtonPressed = toolPressed;
}

IPressedStrategy* ToolManager::getToolPressed(){
    return _rightButtonPressed;
}

void ToolManager::build(){
    Editor* editor = _editorManager->getActiveEditor();

    toolRuntimeContext.editor = editor;
    toolRuntimeContext.layer = editor->getActiveFrame()->getActiveLayer();
    toolRuntimeContext.preview = editor->preview();
    toolRuntimeContext.drawingSession = editor->getDrawingSession();
    toolRuntimeContext.viewport = _viewport;
    toolRuntimeContext.canvasSettings = editor->getCanvasSettings();
    toolRuntimeContext.drawingAreaSize = editor->getDrawingAreaSize();

}


void ToolManager::onPressed(int x, int y, int button){
    build();
    if(buttonMousePressed != KEY_MOUSE::UNPRESSED) return;
    
    buttonMousePressed = (KEY_MOUSE)button;

    switch(buttonMousePressed){
        case KEY_MOUSE::RIGHT_BUTTON: _rightButtonPressed->onPressed(x, y, toolRuntimeContext);  break;
        case KEY_MOUSE::LEFT_BUTTON: _leftButtonPressed->onPressed(x, y, toolRuntimeContext); break;
        default: _otherButtonPressed->onPressed(x, y, toolRuntimeContext); break;
    }
}
void ToolManager::onTracking(int x, int y){
    if(buttonMousePressed == KEY_MOUSE::UNPRESSED) return;
    
    switch(buttonMousePressed){
        case KEY_MOUSE::RIGHT_BUTTON: _rightButtonPressed->onTracking(x, y); break;
        case KEY_MOUSE::LEFT_BUTTON: _leftButtonPressed->onTracking(x, y); break;
        default: _otherButtonPressed->onTracking(x, y); break;
    }
}
void ToolManager::onReleased(int x, int y, int button){
    if(static_cast<int>(buttonMousePressed) != button) return;
    
    switch((KEY_MOUSE)button){
        case KEY_MOUSE::RIGHT_BUTTON: _rightButtonPressed->onRelease(); break;
        case KEY_MOUSE::LEFT_BUTTON: _leftButtonPressed->onRelease(); break;
        default: _otherButtonPressed->onRelease(); break;
    }
    
    buttonMousePressed = KEY_MOUSE::UNPRESSED;
}

void ToolManager::onPinchPressed(int x1, int y1, int x2, int y2){

}
void ToolManager::onPinchTracking(int x1, int y1, int x2, int y2){}
void ToolManager::onPinchReleased(int x1, int y1, int x2, int y2){}

void ToolManager::onScroll(int deltaY, int x, int y){
    // if(deltaY < 0)
    //     onScroll(x, y);
    // else
    //     onScroll(x, y);
}
CursorContext* ToolManager::getCursorContext(){
    return _rightButtonPressed->getCursorContext();
}
