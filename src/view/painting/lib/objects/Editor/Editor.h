
#ifndef IEDITOR_H
#define IEDITOR_H


#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <memory>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <functional>

#include "../layer/Layers.h"
#include "../frame/Frame.h"

#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../interfaces/IEditorObserver/IEditorObserver.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/surface/Surface.h"
#include "../../graphics/Pixel/Pixel.h"

extern "C" {
    void renderCanvas(unsigned int projectWidth, unsigned int projectHeight, unsigned int* screen, size_t length, unsigned int viewportWidth, unsigned int viewportHeight, unsigned int x, unsigned int y, int nRows, int nCols);
    void clear(unsigned int viewportWidth, unsigned int viewportHeight, unsigned int x, unsigned int y);
    unsigned int get_viewport_width();
    unsigned int get_viewport_height();
};

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
    const std::string DEFAULT_NAME_LAYER = "Layer";
    Surface _screen;
    Surface _sketch;
    Point _sketchPosition;
    unsigned int _scale = 1;
    int _rows = 1;
    int _cols = 1;
    vector<std::unique_ptr<Frame>> frames;
    vector<IEditorObserver*> observers;
    Frame* activeFrame = nullptr;

    std::vector<unique_ptr<Frame>>::iterator getIteratorFrameByID(Guid id);

public:
    Editor(int width, int height);
    ~Editor();

    void registerEvent(IEditorObserver* observer);

    Point getInitialPosition();
    Bounding getSketchBounding();
    void setNumberTiles(int rol, int col);
    void preview(IGraphic& graphic);
    void draw(IGraphic& graphic);
    void render();
    void render(int startX, int endX, int startY,int endY);
    void renderArea(Bounding area);

    void addFrame(unique_ptr<Frame>frame, size_t index);
    unique_ptr<Frame> removeFrame(size_t index);
    void bringFrameTo(Guid id, size_t toIndex);
    void changeActiveFrame(Guid id);

    Frame* getActiveFrame();
    Frame* getFrameByID(Guid id);
    size_t getFrameIndex(Guid id);
    size_t getFramesLength();
    Frame* getFrameByIndex(size_t index); 

    int getWidth();
    int getHeight();
};
#endif