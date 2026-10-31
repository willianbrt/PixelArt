#include "SymmetrySettingsVM.h"


SymmetrySettingsVM::SymmetrySettingsVM(){
   _toolManager = AppContext::instance().getToolManager();
   _toolSettings = AppContext::instance().getToolSettings();
}
SymmetrySettingsVM::~SymmetrySettingsVM(){
}
bool SymmetrySettingsVM::getMirrorX(){
    return AppContext::instance().getEditorManager()->getActiveEditor()->getSymmetryContext()->isMirrorX;
}
bool SymmetrySettingsVM::getMirrorY(){
    return AppContext::instance().getEditorManager()->getActiveEditor()->getSymmetryContext()->isMirrorY;
}
bool SymmetrySettingsVM::isTilingX(){
    return AppContext::instance().getEditorManager()->getActiveEditor()->getCanvasSettings()->tilingContext.isTilingX;
}
bool SymmetrySettingsVM::isTilingY(){
    return AppContext::instance().getEditorManager()->getActiveEditor()->getCanvasSettings()->tilingContext.isTilingY;
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
        .function("getMirrorX", &SymmetrySettingsVM::getMirrorX)
        .function("getMirrorY", &SymmetrySettingsVM::getMirrorY)
        .function("isTilingX", &SymmetrySettingsVM::isTilingX)
        .function("isTilingY", &SymmetrySettingsVM::isTilingY)

        .function("setMirrorX", &SymmetrySettingsVM::setMirrorX)
        .function("setMirrorY", &SymmetrySettingsVM::setMirrorY)
        .function("enabledTilingX", &SymmetrySettingsVM::enabledTilingX)
        .function("enabledTilingY", &SymmetrySettingsVM::enabledTilingY)
        ;
};