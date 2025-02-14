#include "EmptyScroll.h"

#include <emscripten.h>
#include <emscripten/bind.h>

EmptyScroll::EmptyScroll() {}
EmptyScroll::~EmptyScroll(){ }
void EmptyScroll::onScroll(float deltaX, float deltaY, Point targetMouse){}


using namespace emscripten;
EMSCRIPTEN_BINDINGS(emptyscroll_module){
    class_<EmptyScroll, emscripten::base<IScroll>>("EmptyScroll")
        .constructor<>();
}