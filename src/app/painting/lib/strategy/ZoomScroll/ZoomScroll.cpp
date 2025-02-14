#include "ZoomScroll.h"

#include <emscripten.h>
#include <emscripten/bind.h>

ZoomScroll::ZoomScroll(PixelEditor* editor) : _editor(editor) {
}

ZoomScroll::~ZoomScroll(){
    delete _editor;
}

void ZoomScroll::onScroll(float deltaX, float deltaY, Point mouseTarget){
    if(deltaY < 0)
        _editor->zoomIn(mouseTarget);
    else
        _editor->zoomOut(mouseTarget);

    _editor->render();
}

using namespace emscripten;
EMSCRIPTEN_BINDINGS(zoomscroll_module){
    class_<ZoomScroll, emscripten::base<IScroll>>("ZoomScroll")
        .constructor<PixelEditor*>()
        .smart_ptr<std::shared_ptr<ZoomScroll>>("shared_ptr<ZoomScroll>");
}
