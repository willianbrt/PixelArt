
#ifndef IEDITOR_H
#define IEDITOR_H


#include <memory>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <functional>

#include "../layer/Layers.h"
#include "../preview/Preview.h"
#include "../frame/Frame.h"
#include "../DirtyManager/DirtyManager.h"
#include "../CanvasSettings/CanvasSettings.h"

#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../interfaces/IEditorObserver/IEditorObserver.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/surface/Surface.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../context/SelectContext/SelectContext.h"

enum EDITOR_EVENT_TYPE{
    DRAW,
    ADD_FRAME,
    REMOVE_FRAME,
    MOVE_FRAME_TO,
    CHANGE_ACTIVE_FRAME
};

class Editor
{
private:
    Surface* _sketch;
    SelectContext* _select;

    vector<std::unique_ptr<Frame>> frames;
    vector<IEditorObserver*> observers;
    Frame* activeFrame = nullptr;
    Preview* _preview;
    CanvasSettings* _canvasSettings;
    DirtyManager dirtyManager;

    std::vector<unique_ptr<Frame>>::iterator getIteratorFrameByID(Guid id);

public:
    Editor(int width, int height);
    ~Editor();

    void registerEvent(IEditorObserver* observer);
    
    Preview* preview();
    void draw(IGraphic& graphic);
    
    void compose();
    void compose(Bounding area);
    
    void resize(int width, int height);
    
    Surface* getSurface();
    unsigned int* getBuffer();
    int getWidth();
    int getHeight();
    
    void addFrame(unique_ptr<Frame>frame, size_t index);
    unique_ptr<Frame> removeFrame(size_t index);
    void bringFrameTo(Guid id, size_t toIndex);
    void changeActiveFrame(Guid id);
    
    Frame* getActiveFrame();
    Frame* getFrameByID(Guid id);
    Frame* getFrameByIndex(size_t index);
    size_t getFrameIndex(Guid id);
    size_t getFramesLength();

    CanvasSettings* getCanvasSettings();
    SelectContext* getSelectContext();
    Point getCanvasSize();
    DirtyManager* getDirtyManager();
};
#endif