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
    ViewportContext* viewport = AppContext::instance().getViewport();
    Editor* editor = AppContext::instance().getEditorManager()->getActiveEditor();
    Surface* surface = editor->getSurface();
    CanvasSettings* canvas = editor->getCanvasSettings();

    canvas->tilingContext.isTilingX = enabled;

    float scale = canvas->getFitScale(viewport, surface);

    canvas->zoom(scale, {0,0}, viewport, surface);
    canvas->pan(canvas->getFitPosition(viewport, surface), viewport, surface);
}
void SymmetrySettingsVM::enabledTilingY(bool enabled){
    ViewportContext* viewport = AppContext::instance().getViewport();
    Editor* editor = AppContext::instance().getEditorManager()->getActiveEditor();
    Surface* surface = editor->getSurface();
    CanvasSettings* canvas = editor->getCanvasSettings();

    canvas->tilingContext.isTilingY = enabled;

    float scale = canvas->getFitScale(viewport, surface);

    canvas->zoom(scale, {0,0}, viewport,surface);
    canvas->pan(canvas->getFitPosition(viewport, surface), viewport, surface);
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