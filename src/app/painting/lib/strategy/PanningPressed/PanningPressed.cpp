#include "PanningPressed.h"

#include <emscripten.h>
#include <emscripten/bind.h>


PanningPressed::PanningPressed(PixelEditor* editor) : _editor(editor) {
}

PanningPressed::~PanningPressed(){
    delete _editor;
}

void PanningPressed::onPressed(Point point){
    printf("panning pressed\n");
    flagPoint = point;
}
void PanningPressed::onTracking(Point point){
    printf("panning tracking\n");

    _editor->panning(flagPoint, point);
    _editor->render();
    flagPoint = point;
}
void PanningPressed::onRelease(Point point){

}

EMSCRIPTEN_BINDINGS(panningpressed_module){ 
    emscripten::class_<PanningPressed, emscripten::base<IPressed>>("PanningPressed")
        .constructor<PixelEditor*>()
        .smart_ptr<std::shared_ptr<PanningPressed>>("shared_ptr<PanningPressed>");
}