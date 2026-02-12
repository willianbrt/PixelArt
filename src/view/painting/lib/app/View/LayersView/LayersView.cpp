#include "LayersView.h"

namespace{
    extern "C" {
        
    }
    EM_JS(void, add_layer, (string layer_id, int index),{
        let listLayer = document.getElementById("list-Layers");
        let layers = listLayer.querySelectorAll("div.layer");

        let layerElement = document.createElement("div");
        let nameLayer = document.createElement("div");
        let h5 = document.createElement("h5");
        let btnHideLayer = document.createElement("button");
        let btnLockLayer = document.createElement("button");
        let btnGrabLayer = document.createElement("button");

        layerElement.classList.add("layer");
        layerElement.dataset.id = layer.getID().toString();
        nameLayer.className = "text";
        nameLayer.className = "name-layer"
        h5.innerText = layer.getName();

        btnHideLayer.className = "hide-layer";
        btnHideLayer.innerHTML = `<i class=\"fa ${layer.isVisible() ? "fa-eye" : "fa-eye-slash" }\"></i>`;

        btnLockLayer.className = "lock-layer";
        btnLockLayer.innerHTML = `<i class=\"fa ${layer.isLock() ? "fa-lock" : "fa-unlock" }\"></i>`;

        btnGrabLayer.className = "grab-layer";
        btnGrabLayer.innerHTML = "<i class=\"fa fa-grip-lines\"></i>";

        layerElement.append(btnHideLayer);
        nameLayer.append(h5);
        layerElement.append(nameLayer);
        layerElement.append(btnLockLayer);
        layerElement.append(btnGrabLayer);

        if(layers.length > 0)
            layers[layers.length-index].before(layerElement);
        else
            listLayer.prepend(layerElement);

        layerElement.addEventListener("click", ()=>{
            const frame = editor.getActiveFrame();
            frame.changeActiveLayer(layer.getID());
            changeActiveLayer(layer);
        });
        btnLockLayer.addEventListener("click", toggleLockLayer);
        btnHideLayer.addEventListener("click", toggleHideLayer);

        btnGrabLayer.addEventListener("mousedown", grabLayer);
        btnGrabLayer.addEventListener("touchstart", grabLayer);

        let editing = false;
        let dbl = false;
        nameLayer.addEventListener("dblclick", renameLayer);
        nameLayer.addEventListener("touchstart", (e)=>{
            if(e.touches.length > 1) return;

            if(!dbl){
                setTimeout(()=>{
                    dbl = false;
                }, 250);
                dbl = true;
                return;
            }
            renameLayer(e)
        });

        function toggleLockLayer(){
            let icon = this.querySelector("i");

            layer.setLock(!layer.isLock());

            if(layer.isLock()){
                icon.classList.replace("fa-unlock","fa-lock");
                return;
            }
            icon.classList.replace("fa-lock", "fa-unlock");
        }
        function toggleHideLayer(){
            let icon = this.querySelector("i");

            layer.setVisible(!layer.isVisible());
            updateFramePreview(activeFrame);
            editor.render();

            if(layer.isVisible()){
                icon.classList.replace("fa-eye-slash", "fa-eye");
                layerElement.classList.toggle("hidden-layer", false);
                return;
            }

            icon.classList.replace("fa-eye", "fa-eye-slash");
            layerElement.classList.toggle("hidden-layer", true);
        }
        function grabLayer(e){
            let areaListLayer = document.getElementById("list-Layers");
            let listLayer = areaListLayer.querySelectorAll("#list-Layers .layer");
            let abort = new AbortController();
            e.preventDefault();

            let onTracking = (e)=> {
                let elementLast;
                listLayer.forEach(el => {
                    let box  = el.getBoundingClientRect();

                    el.classList.remove("after-indicator")
                    el.classList.remove("before-indicator")

                    if((e?.touches?.[0].clientY ?? e?.clientY) > box.y){
                        elementLast = el;
                    }
                });

                if(!elementLast)
                    listLayer[0]?.classList.add("before-indicator");
                else
                    elementLast?.classList.add("after-indicator");
            }
            let onRelease = (e)=>{
                let elementLast;
                listLayer.forEach(el => {
                    let box  = el.getBoundingClientRect();

                    el.classList.remove("swap")
                    if((e?.changedTouches?.[0].clientY ?? e?.clientY) > box.y){
                        elementLast = el;
                    }
                });

                let activeFrame = editor.getActiveFrame();

                if(elementLast){
                    elementLast?.classList.remove("after-indicator");
                    elementLast?.after(layerElement);
                } else {
                    elementLast = listLayer[0];
                    elementLast?.classList.remove("before-indicator");
                    areaListLayer.prepend(layerElement);
                }

                let indexDst = activeFrame.getLayerIndex(new module.Guid(elementLast.dataset.id));
                activeFrame.bringLayerTo(layer.getID(), indexDst);
                updateFramePreview(activeFrame);
                editor.render();

                abort.abort();
            }

            window.addEventListener("mousemove", onTracking, {signal: abort.signal});
            window.addEventListener("touchmove", onTracking, {signal: abort.signal});

            window.addEventListener("mouseup", onRelease, { once: true });
            window.addEventListener("touchend", onRelease, { once: true });

        }
        function renameLayer(e){
            if(editing) return;
            editing = true;

            let inpNameLayer = document.createElement("input");
            inpNameLayer.value = layer.getName();
            inpNameLayer.type = "text";

            nameLayer.replaceChild(inpNameLayer, h5);
            inpNameLayer.focus();

            function done(){
                if(inpNameLayer.value != "" && inpNameLayer.value != layer.getName()){
                    let nome = findTitle(inpNameLayer.value);
                    layer.setName(nome);
                    h5.innerText = layer.getName();
                }

                nameLayer.replaceChild(h5, inpNameLayer);
                editing = false;
            }
            inpNameLayer.addEventListener("blur", done);
            inpNameLayer.addEventListener("keypress", function(e){
                if(e.keyCode == 13)
                    done();
            });
        }
    });
    EM_JS(void, change_active_layer, (string layer_id), {
        let layerElement = getLayerById(layer.getID().toString());
        if(!layerElement)
            layerElement = addLayer(layer);

        listLayer.querySelectorAll("div.layer.active")
                    .forEach((f)=>f.classList.remove("active"));
        layerElement?.classList.toggle("active", true);

        let inpOpacity = document.querySelector("input[name='opacity-layer']");
        inpOpacity.value = layer.getOpacity() * 100.0;
        document.querySelector("#opacity-label h5").innerText = "Transparência " + inpOpacity.value + "%"
    });
    EM_JS(void, remove_layer, (string layer_id), {
        let frameElement = listLayer.querySelector(`.layer[data-id="${layer_id}"]`);
        frameElement.remove();
    });
    EM_JS(void, move_layer, (string layer_id, int index), {
        if(!activeFrameContainLayer(id))
            return;

        let layers = listLayer.querySelectorAll("div.layer");
        index = layers.length - index - 1;
        let layerElement = getLayerById(id.toString());

        if (layerElement === layers[index] || index < 0 || index >= layers.length) {
            return;
        }

        if (layerElement.compareDocumentPosition(layers[index]) & Node.DOCUMENT_POSITION_FOLLOWING) {
            layers[index].after(layerElement);
        } else {
            layers[index].before(layerElement);
        }
    });
}

LayersView::LayersView(LayersViewModel& layersViewModel) : _layersViewModel(layersViewModel){
    emscripten_set_click_callback("#add-frame", &_layersViewModel, 0, addLayerClick); 
    emscripten_set_click_callback("#duplicate-frame", &_layersViewModel, 0, duplicateLayerClick);
    emscripten_set_click_callback("#move-down-frame", &_layersViewModel, 0, moveDownLayerClick);
    emscripten_set_click_callback("#move-up-frame", &_layersViewModel, 0, moveUpLayerClick);
    emscripten_set_click_callback("#remove-frame", &_layersViewModel, 0, removeLayerClick);
    emscripten_set_click_callback("#flip-x", &_layersViewModel, 0, flipXLayerClick);
    emscripten_set_click_callback("#flip-y", &_layersViewModel, 0, flipYLayerClick);

    
    layersViewModel.registerEvent(FRAME_EVENT_TYPE::ADD_LAYER, addLayer);
    layersViewModel.registerEvent(FRAME_EVENT_TYPE::REMOVE_LAYER, removeLayer);
    layersViewModel.registerEvent(FRAME_EVENT_TYPE::MOVE_LAYER_TO, moveLayerTo);
    layersViewModel.registerEvent(FRAME_EVENT_TYPE::CHANGE_ACTIVE_LAYER, changeActiveLayer);
}
LayersView::~LayersView(){
    emscripten_set_click_callback("#add-frame", nullptr, 0, nullptr); 
    emscripten_set_click_callback("#duplicate-frame", nullptr, 0, nullptr);
    emscripten_set_click_callback("#move-down-frame", nullptr, 0, nullptr);
    emscripten_set_click_callback("#move-up-frame", nullptr, 0, nullptr);
    emscripten_set_click_callback("#remove-frame", nullptr, 0, nullptr);
    emscripten_set_click_callback("#flip-x", nullptr, 0, nullptr);
    emscripten_set_click_callback("#flip-y", nullptr, 0, nullptr);
}
void LayersView::render(){
    // for(Layer* frame : _editor.getAllLayers()){
    //     add_layer(frame->getID().toString());
    // }
}
void LayersView::addLayer(FrameEvent event){
    add_layer(event.layer_id.toString(), event.index);
}
void LayersView::removeLayer(FrameEvent event){
    remove_layer(event.layer_id.toString());
}
void LayersView::moveLayerTo(FrameEvent event){
    move_layer(event.layer_id.toString(), event.index);
}
void LayersView::changeActiveLayer(FrameEvent event){
    change_active_layer(event.layer_id.toString());
}
void LayersView::drawLayer(FrameEvent event){
    printf("draw_alo\n");
    // update_layer_preview(event.layer_id);
}

void LayersView::onLayerClick(string frameId){
    // Guid id(frameId);
    // _layersViewModel.onChangeActiveLayer(id);
}
// void LayersView::onAddLayerClick(){
//     _layersViewModel.onAddLayer();
// }
// void LayersView::onRemoveLayerClick(){
//     _layersViewModel.onChangeActiveLayer();
// }
// void LayersView::onMoveUpLayerClick(){
//     _layersViewModel.onMoveUpLayer(id);
// }
// void LayersView::onMoveDownLayerClick(){
//     _layersViewModel.onMoveDownLayer(id);
// }
// void LayersView::onDuplicateLayerClick(){
//     _layersViewModel.onDuplicateLayer(id);
// }

EM_BOOL LayersView::addLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    LayersViewModel* layerView = static_cast<LayersViewModel*>(userData);
    layerView->onAddLayer();

    return EM_FALSE;
}
EM_BOOL LayersView::duplicateLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    LayersViewModel* layerView = static_cast<LayersViewModel*>(userData);
    layerView->onDuplicateLayer();

    return EM_FALSE;
}
EM_BOOL LayersView::moveDownLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    LayersViewModel* layerView = static_cast<LayersViewModel*>(userData);
    layerView->onMoveDownLayer();

    return EM_FALSE;
}
EM_BOOL LayersView::moveUpLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    LayersViewModel* layerView = static_cast<LayersViewModel*>(userData);
    layerView->onMoveUpLayer();

    return EM_FALSE;
}
EM_BOOL LayersView::removeLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    LayersViewModel* layerView = static_cast<LayersViewModel*>(userData);
    layerView->onRemoveLayer();

    return EM_FALSE;
}
EM_BOOL LayersView::flipXLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    LayersViewModel* layerView = static_cast<LayersViewModel*>(userData);
    layerView->onFlipXLayer();

    return EM_FALSE;
}
EM_BOOL LayersView::flipYLayerClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData){
    LayersViewModel* layerView = static_cast<LayersViewModel*>(userData);
    layerView->onFlipYLayer();

    return EM_FALSE;
}


#include <emscripten/bind.h>
using namespace emscripten;
EMSCRIPTEN_BINDINGS(app)
{
    emscripten::function("onLayerClick", &LayersView::onLayerClick);
}