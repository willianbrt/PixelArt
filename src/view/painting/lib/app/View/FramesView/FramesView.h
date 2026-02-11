
#ifndef FRAMESVIEW_H
#define FRAMESVIEW_H

#include "../../Editor/Editor.h"
#include "../../../objects/frame/Frame.h"
#include "../../ViewModel/FramesViewModel/FramesViewModel.h"
// #include "../../../commands/AddFrameCommand/AddFrameCommand.h"
// #include "../../../commands/HistoryCommand/HistoryCommand.h"
// #include "../../../commands/CloneFrameCommand/CloneFrameCommand.h"
// #include "../../../commands/MoveFrameTo/MoveFrameTo.h"
// #include "../../../commands/RemoveFrameCommand/RemoveFrameCommand.h"

#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>

class FramesView{
private:
    FramesViewModel& _framesViewModel;

    static EM_BOOL addFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL duplicateFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL moveDownFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL moveUpFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL removeFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL flipXFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL flipYFrameClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    
    static void render();
    static void addFrame(EditorEvent event);
    static void removeFrame(EditorEvent event);
    static void moveFrameTo(EditorEvent event);
    static void changeActiveFrame(EditorEvent event);
    static void drawFrame(EditorEvent event);
    
public:
    FramesView(FramesViewModel& framesViewModel);
    ~FramesView();
    static void onFrameClick(string id);

/*

    void onChangeActiveFrame();
    void onAddFrame();
    void onDuplicateFrame(Guid id);
    void onMoveFrameTo(Guid id, int index);
    void onMoveDownFrame(Guid id);
    void onMoveUpFrame(Guid id);
    void onRemoveFrame(Guid id);
    void onFlipXFrame(Guid id);
    void onFlipYFrame(Guid id);
*/
};

#endif