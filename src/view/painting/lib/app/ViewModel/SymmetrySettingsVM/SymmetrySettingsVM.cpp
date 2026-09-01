#include "SymmetrySettingsVM.h"


SymmetrySettingsVM::SymmetrySettingsVM(){
   _toolManager = AppContext::instance().getToolManager();
   _toolSettings = AppContext::instance().getToolSettings();
}
SymmetrySettingsVM::~SymmetrySettingsVM(){
}
void SymmetrySettingsVM::setMirrorX(bool active){
    AppContext::instance().getEditorManager()->getActiveEditor()->getSymmetryContext()->isMirrorX = active;
}
void SymmetrySettingsVM::setMirrorY(bool active){
    AppContext::instance().getEditorManager()->getActiveEditor()->getSymmetryContext()->isMirrorY = active;
}
void SymmetrySettingsVM::setNTileX(int size){
    AppContext::instance().getEditorManager()->getActiveEditor()->getSymmetryContext()->nTileX = size;
}
void SymmetrySettingsVM::setNTileY(int size){
    AppContext::instance().getEditorManager()->getActiveEditor()->getSymmetryContext()->nTileY = size;
}

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<SymmetrySettingsVM>("SymmetrySettingsVM")
        .constructor<>()
        .function("setMirrorX", &SymmetrySettingsVM::setMirrorX)
        .function("setMirrorY", &SymmetrySettingsVM::setMirrorY)
        .function("setNTileX", &SymmetrySettingsVM::setNTileX)
        .function("setNTileY", &SymmetrySettingsVM::setNTileY)
        ;
};