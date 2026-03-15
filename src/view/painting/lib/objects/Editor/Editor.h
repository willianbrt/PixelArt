
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
#include "../Renderer/Renderer.h"

#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../interfaces/IEditorObserver/IEditorObserver.h"
#include "../../graphics/GraphicsEngine/GraphicsEngine.h"
#include "../../graphics/surface/Surface.h"
#include "../../graphics/Pixel/Pixel.h"

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
    Surface _sketch;
    Point _sketchPosition;
    float _scale = 1.0f;
    int _rows = 1;
    int _cols = 1;

    vector<std::unique_ptr<Frame>> frames;
    vector<IEditorObserver*> observers;
    Frame* activeFrame = nullptr;
    Preview* _preview;
    Renderer* _renderer;
    Surface* _overlay;

    std::vector<unique_ptr<Frame>>::iterator getIteratorFrameByID(Guid id);

public:
    Editor(int width, int height);
    ~Editor();

    void registerEvent(IEditorObserver* observer);

    Bounding getSketchBounding();
    void setNumberTiles(int rol, int col);
    Preview* preview();
    // void preview(IGraphic& graphic);
    void draw(IGraphic& graphic);
    void render();
    void resize(int width, int height);
    void renderArea(Bounding area);
    unsigned int* getBuffer();

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
    float getScale();
    Point getSketchPosition();
};
#endif