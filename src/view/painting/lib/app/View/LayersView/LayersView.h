
#ifndef LAYERSVIEW_H
#define LAYERSVIEW_H

#include "../../Editor/Editor.h"
#include "../../../objects/frame/Frame.h"
#include "../../../objects/layer/Layers.h"
#include "../../ViewModel/LayersViewModel/LayersViewModel.h"

#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>

class LayersView{
private:
    LayersViewModel& _layersViewModel;

    static EM_BOOL addLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL duplicateLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL moveDownLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL moveUpLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL removeLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL flipXLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL flipYLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    
    static void render();
    static void addLayer(FrameEvent event);
    static void removeLayer(FrameEvent event);
    static void moveLayerTo(FrameEvent event);
    static void changeActiveLayer(FrameEvent event);
    static void drawLayer(FrameEvent event);
    
public:
    LayersView(LayersViewModel& layersViewModel);
    ~LayersView();
    static void onLayerClick(string id);

/*

    void onChangeActiveLayer();
    void onAddLayer();
    void onDuplicateLayer(Guid id);
    void onMoveLayerTo(Guid id, int index);
    void onMoveDownLayer(Guid id);
    void onMoveUpLayer(Guid id);
    void onRemoveLayer(Guid id);
    void onFlipXLayer(Guid id);
    void onFlipYLayer(Guid id);
*/
};

#endif