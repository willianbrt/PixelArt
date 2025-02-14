#include "EmptyPressed.h"

#include <emscripten.h>
#include <emscripten/bind.h>


EmptyPressed::EmptyPressed() {}
EmptyPressed::~EmptyPressed(){ }
void EmptyPressed::onPressed(Point point){printf("vazio\n");}
void EmptyPressed::onTracking(Point point){}
void EmptyPressed::onRelease(Point point){}

using namespace emscripten;
EMSCRIPTEN_BINDINGS(emptypressed_module){
    class_<EmptyPressed, emscripten::base<IPressed>>("EmptyPressed")
        .constructor<>();
}