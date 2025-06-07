#include "Frame.h"

Frame::Frame(unsigned int width, unsigned int height) : id(Guid::generateUUID()) {
    _width = width;
    _height = height;

    // addLayer(new Layer("Layer 1", _width, _height));

    // layers.at(0)->putPixel(5, 5, 0xFF00FFFF);
    // layers.at(0)->putPixel(1, 57, 0xFFFFFFFF);
    // layers.at(0)->putPixel(99, 10, 0xFFFF00FF);
    // layers.at(0)->putPixel(9, 99, 0xFF0000FF);
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
void Frame::draw(IGraphic& graphic){
    activeLayer->draw(graphic);
}

unsigned int Frame::getWidth(){ return _width; }
unsigned int Frame::getHeight(){ return _height; }

unsigned int Frame::getPixel(int x, int y){ return getPixel(calcIndex(x,y)); }
unsigned int Frame::getPixel(int index){
    return getPixel(index, 0, layers.size());
}
unsigned int Frame::getPixel(int index, int fromIndex, int toIndex){
    unsigned int colorHex = 0;
    if(toIndex > layers.size()) throw std::runtime_error("ToIndex excede o tamanho maximo de Layers.");
    
    for(int layerIndex = fromIndex; layerIndex < toIndex; layerIndex++){
        Layer* layer = layers.at(layerIndex);
        if(!layer->isVisible()) continue;

        blending(colorHex, layer->getPixel(index));
    }

    return colorHex;
}

void Frame::putPixel(int x, int y, unsigned int colorHex){ putPixel(calcIndex(x, y), colorHex); }
void Frame::putPixel(int index, unsigned int colorHex){ 
    activeLayer->putPixel(index, colorHex); 
}

unsigned int Frame::calcIndex(int x, int y){ return x + y*_width; }

unsigned int Frame::getFrameDuration(){ return timeDuration; }

void Frame::blending(unsigned int& bottomColor, unsigned int topColor){
    const float factorAlphaSrc = ALPHA(topColor) / 255.0f;

    int rsrc = (topColor >> 24) & 0xFF;
    int gsrc = (topColor >> 16) & 0xFF;
    int bsrc = (topColor >> 8) & 0xFF;
    int asrc = (topColor & 0xFF);
    
    int rdst = (bottomColor >> 24) & 0xFF;
    int gdst = (bottomColor >> 16) & 0xFF;
    int bdst = (bottomColor >> 8) & 0xFF;
    int adst = (bottomColor & 0xFF);

    int rout = factorAlphaSrc* rsrc + (1.0f - factorAlphaSrc)*rdst;
    int gout = factorAlphaSrc* gsrc + (1.0f - factorAlphaSrc)*gdst;
    int bout = factorAlphaSrc* bsrc + (1.0f - factorAlphaSrc)*bdst;
    int aout = factorAlphaSrc* asrc + (1.0f - factorAlphaSrc)*adst;

    bottomColor = (rout << 24 | gout << 16 | bout << 8 | aout);
}

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
    
    emscripten::val::global("move_layer_to")(emscripten::val(id), emscripten::val(toIndex));
}
void Frame::removeLayer(Guid id){
    auto it = getIteratorLayerByID(id);
    size_t index = it - layers.begin();
    if (it != layers.end()) {
        layers.erase(it);
        
        emscripten::val::global("remove_layer")(emscripten::val(id));

        if(layers.size() == 0){
            addLayer(new Layer("", getWidth(), getHeight()));
            return;
        }

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
        .constructor<unsigned int, unsigned int>()
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