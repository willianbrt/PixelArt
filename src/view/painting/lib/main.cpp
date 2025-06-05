#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <memory>
#include <algorithm>

#include "objects/componding/CompoundingTiles.h"
#include "objects/layer/Layers.h"
#include "objects/frame/Frame.h"
#include "interfaces/IGraphic/IGraphic.h"
#include "graphics/GraphicsEngine/GraphicsEngine.cpp"
#include "graphics/Surface/Surface.cpp"
#include "graphics/Pixel/Pixel.h"

extern "C" {
    void draw(unsigned int* screen, size_t length, unsigned int viewportWidth, unsigned int viewportHeight, unsigned int x, unsigned int y);
    unsigned int get_viewport_width();
    unsigned int get_viewport_height();
}


class Editor
{
private:
    Surface _screen;
    Surface _sketch;
    unsigned int _scale = 1;
    vector<Frame*> frames;
    Frame* activeFrame;

public:
    Editor(unsigned int width, unsigned int height) :_screen(get_viewport_width(), get_viewport_height()), _sketch(width, height) {
        draw(_screen.getData(), _screen.getLength(), _screen.getWidth(), _screen.getHeight(), 0, 0);
        addFrame(new Frame(width, height));
    }
    ~Editor(){
        // free(_screen);
    }

    void bringFrameTo(size_t from, size_t to){
        if (from == to || from >= frames.size() || to >= frames.size()) return;

        if (from < to)
            std::swap(to, from);

        std::rotate(frames.begin() + to, frames.begin() + from, frames.begin() + from + 1);
        
        // emscripten::val::global("move_frame_to")(emscripten::val(id), to);
    }
    void removeFrame(Guid id){
        auto it = getIteratorFrameByID(id);
        size_t index = it - frames.begin();
        if (it != frames.end()) {
            frames.erase(it);
            
            emscripten::val::global("remove_frame")(emscripten::val(id));

            if(frames.size() == 0){
                addFrame(new Frame(_sketch.getWidth(), _sketch.getHeight()));
                return;
            }

            if(id.toString() == activeFrame->getID().toString()){
                size_t activeIndex = std::min(frames.size()-1, std::max<size_t>(0, index));
                changeActiveFrame(frames[activeIndex]->getID());
            }
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
    class_<Editor>("Editor")
        .constructor<unsigned int, unsigned int>()
        .smart_ptr<std::shared_ptr<Editor>>("shared_ptr<Editor>")
        .function("bringFrameTo", &Editor::bringFrameTo)
        .function("removeFrame", &Editor::removeFrame)
        .function("addFrame", &Editor::addFrame, allow_raw_pointers())
        .function("getAllFrames", &Editor::getAllFrames, allow_raw_pointers())
        .function("getFrameByID", &Editor::getFrameByID, allow_raw_pointers())
        .function("getActiveFrame", &Editor::getActiveFrame, allow_raw_pointers())
        .function("changeActiveFrame", &Editor::changeActiveFrame);
};
