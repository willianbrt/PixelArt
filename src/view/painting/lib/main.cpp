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
    void renderCanvas(unsigned int* screen, size_t length, unsigned int viewportWidth, unsigned int viewportHeight, unsigned int x, unsigned int y);
    void clear(unsigned int viewportWidth, unsigned int viewportHeight, unsigned int x, unsigned int y);
    unsigned int get_viewport_width();
    unsigned int get_viewport_height();
}


class Editor
{
private:
    const std::string DEFAULT_NAME_LAYER = "Layer";
    Surface _screen;
    Surface _sketch;
    Point _sketchPosition;
    unsigned int _scale = 1;
    vector<Frame*> frames;
    Frame* activeFrame = nullptr;

public:
    Editor(unsigned int width, unsigned int height) :_screen(get_viewport_width(), get_viewport_height()), _sketch(width, height) {
        _sketchPosition = getInitialPosition();
    }
    ~Editor(){
        // free(_screen);
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

        for(int y = area.start.y; y < area.end.y; y++){
            int index = y * _sketch.getWidth();
            for(int x = area.start.x; x < area.end.x; x++){
                unsigned int colorHex = activeFrame->getPixel(index);
                swap_endian_uint32(&colorHex);
                _sketch.putPixel(index, colorHex);

                index++;
            }
        }

        renderCanvas(_sketch.getBuffer(), _sketch.getLength(), _sketch.getWidth(), _sketch.getHeight(), 0, 0);
    }
    void cloneActiveFrame(){
        Frame* cloneFrame = new Frame(*activeFrame);
        cloneFrame->setID(Guid::generateUUID());

        addFrame(cloneFrame);

        size_t i = std::distance(frames.begin(), getIteratorFrameByID(activeFrame->getID()));
        bringFrameTo(cloneFrame->getID(), i + 1);
        changeActiveFrame(cloneFrame->getID());
    }
    void bringFrameToFoward(Guid id){
        size_t i = std::distance(frames.begin(), getIteratorFrameByID(id));
        bringFrameTo(id, i + 1);
    }
    void bringFrameBack(Guid id){
        size_t i = std::distance(frames.begin(), getIteratorFrameByID(id));
        bringFrameTo(id, i - 1);
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
        
        emscripten::val::global("move_frame_to")(emscripten::val(id), emscripten::val(toIndex));
    }
    
    void removeFrame(Guid id){
        auto it = getIteratorFrameByID(id);
        size_t index = it - frames.begin();
        if (it == frames.end()) return;

        frames.erase(it);
        emscripten::val::global("remove_frame")(emscripten::val(id));

        if(frames.size() == 0){
            Frame* f = new Frame();
            f->addLayer(new Layer(DEFAULT_NAME_LAYER, _sketch.getWidth(), _sketch.getHeight()));
            addFrame(f);
            return;
        }

        if(id.toString() == activeFrame->getID().toString()){
            size_t activeIndex = std::min(frames.size()-1, std::max<size_t>(0, index));
            changeActiveFrame(frames[activeIndex]->getID());
        }
    }
    void addFrame(Frame* frame){
        frames.emplace_back(frame);
        emscripten::val::global("add_frame")(emscripten::val(*frame));
        
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
    std::vector<Frame*>::iterator getIteratorFrameByID(Guid id){
        string idStr = id.toString();
        return std::find_if(frames.begin(), frames.end(), [&idStr](Frame* f){ return f->getID().toString() == idStr; });
    }
    Frame* getActiveFrame(){
        return activeFrame;
    }
    void changeActiveFrame(Guid id){
        activeFrame = getFrameByID(id);
        emscripten::val::global("change_active_frame")(emscripten::val(id));
    }
};

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    register_vector<Frame*>("VectorFrame");
    class_<Editor>("Editor")
        .constructor<unsigned int, unsigned int>()
        .smart_ptr<std::shared_ptr<Editor>>("shared_ptr<Editor>")
        .function("renderArea", select_overload<void(int, int, int, int)>(&Editor::render))
        .function("preview", &Editor::preview)
        .function("draw", &Editor::draw)
        .function("render", select_overload<void()>(&Editor::render))
        .function("bringFrameToFoward", &Editor::bringFrameToFoward)
        .function("bringFrameBack", &Editor::bringFrameBack)
        .function("bringFrameTo", &Editor::bringFrameTo)
        .function("removeFrame", &Editor::removeFrame)
        .function("cloneActiveFrame", &Editor::cloneActiveFrame)
        .function("addFrame", &Editor::addFrame, allow_raw_pointers())
        .function("getAllFrames", &Editor::getAllFrames, allow_raw_pointers())
        .function("getFrameByID", &Editor::getFrameByID, allow_raw_pointers())
        .function("getActiveFrame", &Editor::getActiveFrame, allow_raw_pointers())
        .function("changeActiveFrame", &Editor::changeActiveFrame);
};
