#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <memory>
#include <algorithm>
#include <vector>

#include "objects/componding/CompoundingTiles.h"
#include "objects/layer/Layers.h"
#include "objects/frame/Frame.h"
#include "interfaces/IGraphic/IGraphic.h"
#include "graphics/GraphicsEngine/GraphicsEngine.h"
#include "graphics/surface/Surface.h"
#include "graphics/Pixel/Pixel.h"

extern "C" {
    void renderCanvas(unsigned int projectWidth, unsigned int projectHeight, unsigned int* screen, size_t length, unsigned int viewportWidth, unsigned int viewportHeight, unsigned int x, unsigned int y, int nRows, int nCols);
    void clear(unsigned int viewportWidth, unsigned int viewportHeight, unsigned int x, unsigned int y);
    unsigned int get_viewport_width();
    unsigned int get_viewport_height();
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
    vector<Frame*> frames;
    Frame* activeFrame = nullptr;

public:
    Editor(int width, int height) :_screen(get_viewport_width(), get_viewport_height()), _sketch(width, height) {
        _sketchPosition = getInitialPosition();
    }
    ~Editor(){
        for (auto* f : frames) delete f;
        frames.clear();
        activeFrame = nullptr;
    }

    Point getInitialPosition(){
        Point p;
        p.x = floor((_screen.getHeight() - (_sketch.getWidth())) / 2);
        p.y = floor((_screen.getWidth() - (_sketch.getHeight())) / 2);
        return p;
    }

    Bounding getSketchBounding(){
        Point endPoint = Point(_sketchPosition.x + _sketch.getWidth(), _sketchPosition.y + _sketch.getHeight());
        return Bounding(_sketchPosition, endPoint);
    }

    void setNumberTiles(int rol, int col){
        _rows = rol; 
        _cols = col; 
    }

    void preview(IGraphic& graphic){
        activeFrame->preview(graphic);
        render();
    }

    void draw(IGraphic& graphic){
        activeFrame->draw(graphic);
        render();
    }

    void render(){
        Point endPoint = Point(_sketch.getWidth(), _sketch.getHeight());
        Bounding boundingSketch = Bounding(Point(0,0), endPoint);

        renderArea(boundingSketch);
    }
    void render(int startX, int endX, int startY,int endY){
        Bounding boundingSketch = Bounding(Point(startX,startY), Point(endX, endY));

        renderArea(boundingSketch);
    }
    void renderArea(Bounding area){
        if(area.start.x > _sketch.getWidth() || area.start.y > _sketch.getHeight()) return;

        area.start.x = (area.start.x < 0) ? 0 : area.start.x;
        area.start.y = (area.start.y < 0) ? 0 : area.start.y;
        area.end.x = (area.end.x > _sketch.getWidth()) ? _sketch.getWidth() : area.end.x;
        area.end.y = (area.end.y > _sketch.getHeight()) ? _sketch.getHeight() : area.end.y;

        size_t activeIndex = getFrameIndex(activeFrame->getID());
        Frame* previousFrame = frames[activeIndex > 0 ? activeIndex - 1 : 0];
        float opacity = 0.5;
        for(int y = area.start.y; y < area.end.y; y++){
            int index = y * _sketch.getWidth();
            for(int x = area.start.x; x < area.end.x; x++){
                unsigned int previousColorHex = previousFrame->getPixel(index);
                previousColorHex = (previousColorHex & 0xFFFFFF00) | static_cast<int>(opacity * (previousColorHex & 0xFF));

                unsigned int colorHex = GraphicsEngine::blendColors(previousColorHex, activeFrame->getPixel(index));
                // swap_endian_uint32(&colorHex);
                _sketch.putPixel(index, colorHex);

                index++;
            }
        }

        renderCanvas(
            _sketch.getWidth(), _sketch.getHeight(),
            _sketch.getBuffer(), _sketch.getLength(), 
            _sketch.getWidth(), _sketch.getHeight(), 
            area.start.x, area.start.y,
            _rows, _cols);
    }
    
    void bringFrameTo(Guid id, size_t toIndex){
        auto from = getIteratorFrameByID(id);

        if (from == frames.end()) return;

        size_t fromIndex = std::distance(frames.begin(), from);
        
        if (fromIndex == toIndex || fromIndex >= frames.size() || toIndex >= frames.size()) return;
        
        if (fromIndex < toIndex) {
            std::rotate(frames.begin() + fromIndex, frames.begin() + fromIndex + 1, frames.begin() + toIndex + 1);
        } else {
            std::rotate(frames.begin() + toIndex, frames.begin() + fromIndex, frames.begin() + fromIndex + 1);
        }
    }
    
    void removeFrame(Guid id){
        auto it = getIteratorFrameByID(id);
        size_t index = it - frames.begin();

        if (it > frames.end()) return;

        frames.erase(it);
        
        if(id.toString() == activeFrame->getID().toString()){
            size_t activeIndex = std::min(frames.size()-1, std::max<size_t>(0, index));
            changeActiveFrame(frames[activeIndex]->getID());
        }
    }
    void addFrame(Frame* frame){
        frames.emplace_back(frame);
        
        if(frames.size() == 1){
            changeActiveFrame(frames[0]->getID());
        }
    }
    vector<Frame*> getAllFrames(){
        return frames;
    }
    Frame* getFrameByID(Guid id){
        auto it = getIteratorFrameByID(id);
        return (it != frames.end()) ? *it : nullptr;
    }
    size_t getFrameIndex(Guid id){
        return std::distance(frames.begin(), getIteratorFrameByID(id));
    }
    std::vector<Frame*>::iterator getIteratorFrameByID(Guid id){
        string idStr = id.toString();
        return std::find_if(frames.begin(), frames.end(), [&idStr](Frame* f){ return f->getID().toString() == idStr; });
    }
    Frame* getActiveFrame(){
        return activeFrame;
    }
    void changeActiveFrame(Guid id){
        activeFrame = getFrameByID(id);
    }

    
    int getWidth(){
        return _sketch.getWidth();
    }
    int getHeight(){
        return _sketch.getHeight();
    }
};

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    register_vector<Frame*>("VectorFrame");
    class_<Editor>("Editor")
        .constructor<int, int>()
        .smart_ptr<std::shared_ptr<Editor>>("shared_ptr<Editor>")
        .function("renderArea", select_overload<void(int, int, int, int)>(&Editor::render))
        .function("preview", &Editor::preview)
        .function("draw", &Editor::draw)
        .function("setNumberTiles", &Editor::setNumberTiles)
        .function("render", select_overload<void()>(&Editor::render))
        .function("bringFrameTo", &Editor::bringFrameTo)
        .function("getFrameIndex", &Editor::getFrameIndex)
        .function("removeFrame", &Editor::removeFrame)
        .function("addFrame", &Editor::addFrame, allow_raw_pointers())
        .function("getAllFrames", &Editor::getAllFrames, allow_raw_pointers())
        .function("getFrameByID", &Editor::getFrameByID, allow_raw_pointers())
        .function("getActiveFrame", &Editor::getActiveFrame, allow_raw_pointers())
        .function("changeActiveFrame", &Editor::changeActiveFrame)
        .function("getWidth", &Editor::getWidth)
        .function("getHeight", &Editor::getHeight);
};
