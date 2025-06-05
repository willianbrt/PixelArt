#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <memory>

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
    int active = 0;

public:
    Editor(unsigned int width, unsigned int height) :_screen(get_viewport_width(), get_viewport_height()), _sketch(width, height) {
        draw(_screen.getData(), _screen.getLength(), _screen.getWidth(), _screen.getHeight(), 0, 0);
        addFrame(new Frame(width, height));
    }
    ~Editor(){
        // free(_screen);
    }

    Frame* getFrame(Guid id){
        for(Frame* frame : frames){
            if(frame->getID().toString() == id.toString())
                return frame;
        }
    }
    void bringFrameTo(size_t from, size_t to){
        if (from == to || from >= frames.size() || to >= frames.size()) return;

        if (from < to)
            std::swap(to, from);

        std::rotate(frames.begin() + to, frames.begin() + from, frames.begin() + from + 1);
    }
    void removeFrame(int index){
        typename vector<Frame*>::iterator it = frames.begin();
        advance(it, index);
        frames.erase(it);
    }
    void addFrame(Frame* frame){
        frames.emplace_back(frame);
        emscripten::val::global("add_frame")(emscripten::val(*frame));
    }
    vector<Frame*> getAllFrames(){
        return frames;
    }
    Frame* getFrameByIndex(unsigned int index){
        return frames.at(index);
    }
    Frame* getActiveFrame(){
        return getFrameByIndex(active);
    }
    int getIndexFromActiveFrame(){
        return active;
    }
    void changeActiveFrame(unsigned int index){
        if(frames.size() < index && index >= 0)
            return;

        active = index;
    }
    size_t getNumberOfFrames(){
        return frames.size();
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
        .function("getFrameByIndex", &Editor::getFrameByIndex, allow_raw_pointers())
        .function("getActiveFrame", &Editor::getActiveFrame, allow_raw_pointers())
        .function("getIndexFromActiveFrame", &Editor::getIndexFromActiveFrame)
        .function("changeActiveFrame", &Editor::changeActiveFrame)
        .function("getNumberOfFrames", &Editor::getNumberOfFrames);
};
