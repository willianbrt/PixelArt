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
void SymmetrySettingsVM::enabledTilingX(bool enabled){
    AppContext::instance().getEditorManager()->getActiveEditor()->getCanvasSettings()->tilingContext.isTilingX = enabled;
}
void SymmetrySettingsVM::enabledTilingY(bool enabled){
    AppContext::instance().getEditorManager()->getActiveEditor()->getCanvasSettings()->tilingContext.isTilingY = enabled;
}

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    class_<SymmetrySettingsVM>("SymmetrySettingsVM")
        .constructor<>()
        .function("setMirrorX", &SymmetrySettingsVM::setMirrorX)
        .function("setMirrorY", &SymmetrySettingsVM::setMirrorY)
        .function("enabledTilingX", &SymmetrySettingsVM::enabledTilingX)
        .function("enabledTilingY", &SymmetrySettingsVM::enabledTilingY)
        ;
};