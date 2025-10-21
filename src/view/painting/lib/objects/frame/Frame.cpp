#include "Frame.h"

Frame::Frame() : id(Guid::generateUUID()) {
}

Frame::~Frame(){}

void Frame::resize(int width, int height){
    for(auto& layer : layers){
        layer->resize(width, height);
    }
}
void Frame::move(int offsetX, int offsetY){
    for(auto& layer : layers){
        layer->move(offsetX, offsetY);
    }
}
void Frame::preview(IGraphic& graphic){
    previewLayer = new Layer(*activeLayer);
    previewLayer->draw(graphic);
}
void Frame::draw(IGraphic& graphic){
    activeLayer->draw(graphic);
    delete previewLayer;
}

unsigned int Frame::getPixel(unsigned int index){ return getPixel(index, 0, layers.size()); }
unsigned int Frame::getPixel(unsigned int index, int fromIndex, int toIndex){
    unsigned int colorHex = 0;
    if(toIndex > layers.size()) throw std::runtime_error("ToIndex excede o tamanho maximo de Layers.");
    
    for(int layerIndex = fromIndex; layerIndex < toIndex; layerIndex++){
        Layer* layer = layers.at(layerIndex);
        if(!layer->isVisible()) continue;


        if(layer->getID().toString() == previewLayer->getID().toString()){
            GraphicsEngine::blending(colorHex, previewLayer->getPixel(index));
        }else{
            GraphicsEngine::blending(colorHex, layer->getPixel(index));
        }
    }

    return colorHex;
}


unsigned int Frame::getFrameDuration(){ return timeDuration; }

Guid Frame::getID(){
    return id;
}


void Frame::bringLayerToFoward(Guid id){
    size_t i = std::distance(layers.begin(), getIteratorLayerByID(id));
    bringLayerTo(id, i + 1);
}
void Frame::bringLayerBack(Guid id){
    size_t i = std::distance(layers.begin(), getIteratorLayerByID(id));
    bringLayerTo(id, i - 1);
}
void Frame::bringLayerTo(Guid id, size_t toIndex){
    auto from = getIteratorLayerByID(id);

    if (from == layers.end()) return;

    size_t fromIndex = std::distance(layers.begin(), from);
    
    if (fromIndex == toIndex || fromIndex >= layers.size() || toIndex >= layers.size()) return;
    
    if (fromIndex < toIndex) {
        std::rotate(layers.begin() + fromIndex, layers.begin() + fromIndex + 1, layers.begin() + toIndex + 1);
    } else {
        std::rotate(layers.begin() + toIndex, layers.begin() + fromIndex, layers.begin() + fromIndex + 1);
    }

    if(activeLayer->getID().toString() == id.toString())
        emscripten::val::global("move_layer_to")(emscripten::val(id), emscripten::val(toIndex));
}
void Frame::removeLayer(Guid id){
    if(layers.size() == 1){ return; }

    auto it = getIteratorLayerByID(id);
    size_t index = it - layers.begin();
    if (it != layers.end()) {
        layers.erase(it);
        
        emscripten::val::global("remove_layer")(emscripten::val(id));

        if(id.toString() == activeLayer->getID().toString()){
            size_t activeIndex = std::min(layers.size()-1, std::max<size_t>(0, index));
            changeActiveLayer(layers[activeIndex]->getID());
        }
    }
}
void Frame::addLayer(Layer* layer){
    layers.emplace_back(layer);
    emscripten::val::global("add_layer")(layer);
    
    if(layers.size() == 1){
        changeActiveLayer(layers[0]->getID());
    }
}
vector<Layer*> Frame::getAllLayers(){
    return layers;
}
Layer* Frame::getLayerByID(Guid id){
    auto it = getIteratorLayerByID(id);
    return (it != layers.end()) ? *it : nullptr;
}
std::vector<Layer*>::iterator Frame::getIteratorLayerByID(Guid id){
    string idStr = id.toString();
    return std::find_if(layers.begin(), layers.end(), [&idStr](Layer* f){ return f->getID().toString() == idStr; });
}
Layer* Frame::getActiveLayer(){
    return activeLayer;
}
void Frame::changeActiveLayer(Guid id){
    activeLayer = getLayerByID(id);
    emscripten::val::global("change_active_layer")(activeLayer);
}


using namespace emscripten;
EMSCRIPTEN_BINDINGS(frame_module){
    register_vector<Layer*>("VectorLayer");

    class_<Frame>("Frame")
        .constructor<>()
        .smart_ptr<std::shared_ptr<Frame>>("shared_ptr<Frame>")
        .function("getID", &Frame::getID)
        .function("resize", &Frame::resize)
        .function("move", &Frame::move)
        .function("draw", &Frame::draw)
        .function("getFrameDuration", &Frame::getFrameDuration)
        
        .function("bringLayerToFoward", &Frame::bringLayerToFoward)
        .function("bringLayerBack", &Frame::bringLayerBack)
        .function("bringLayerTo", &Frame::bringLayerTo)
        .function("removeLayer", &Frame::removeLayer)
        .function("addLayer", &Frame::addLayer, allow_raw_pointers())
        .function("getAllLayers", &Frame::getAllLayers, allow_raw_pointers())
        .function("getLayerByID", &Frame::getLayerByID, allow_raw_pointers())
        .function("getActiveLayer", &Frame::getActiveLayer, allow_raw_pointers())
        .function("changeActiveLayer", &Frame::changeActiveLayer);
};