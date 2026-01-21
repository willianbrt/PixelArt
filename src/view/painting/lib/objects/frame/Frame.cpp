#include "Frame.h"

Frame::Frame() : _id(Guid::generateUUID()) {
}

Frame::Frame(const Frame& frame) : _id(frame.getID()) {
    timeDuration = frame.getFrameDuration();

    for(Layer* layer : frame.getAllLayers()){
        layers.emplace_back(new Layer(*layer));
    }
    activeLayer = getLayerByID(frame.getActiveLayer()->getID());
}

Frame::~Frame(){
    for (auto* l : layers) delete l;
    layers.clear();
    delete activeLayer;
    delete previewLayer;
}

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
Frame Frame::clone() const {
    return Frame(*this);
}
void Frame::flipX(){
    for(Layer* layer : layers){
        unsigned int* buffer = layer->getBuffer();
        unsigned int len = layer->getLength();
        int width = layer->getWidth();
        
        if(width <= 0) return;

        int incrementY = (width+1) >> 1;
        int index = incrementY;
        int oppositeIndex = width - 1 - index;
        while(index < len){
            std::swap(buffer[index], buffer[oppositeIndex]);

            index++;
            incrementY++;
            
            if(incrementY >= width){
                incrementY = (width+1) >> 1;
                index += incrementY;
                oppositeIndex += width + ((width) >> 1) - 1;
            } else {
                oppositeIndex--;
            }
        }
    }
}
void Frame::flipY(){
    for(Layer* layer : layers){
        unsigned int* buffer = layer->getBuffer();
        unsigned int len = layer->getLength();
        int width = layer->getWidth();
        
        int incrementX = 0;
        int index = ((layer->getHeight() + 1) >> 1) * width;
        int oppositeIndex = len - width - index;
        while(index < len){
            std::swap(buffer[index], buffer[oppositeIndex]);
            
            index++;
            incrementX++;
            
            if(incrementX >= width){
                oppositeIndex -=  width + width - 1;
                incrementX = 0;
            } else{
                oppositeIndex++;
            }
        }
    }
}

void Frame::preview(IGraphic& graphic){
    if (!activeLayer) return;

    if (previewLayer) {
        delete previewLayer;
        previewLayer = nullptr;
    }

    previewLayer = new Layer(*activeLayer);
    
    if(activeLayer->isLock()) return;
    previewLayer->draw(graphic);
}
void Frame::draw(IGraphic& graphic){
    if (activeLayer){
        if(activeLayer->isLock()) return;

        activeLayer->draw(graphic);
    }

    if (previewLayer) {
        delete previewLayer;
        previewLayer = nullptr;
    }
}

unsigned int Frame::getPixel(unsigned int index){ return getPixel(index, 0, layers.size()); }
unsigned int Frame::getPixel(unsigned int index, int fromIndex, int toIndex){
    unsigned int colorHex = 0;
    float weight = 1.0f;
    if(toIndex > layers.size()) throw std::runtime_error("ToIndex excede o tamanho maximo de Layers.");
    
    for(int layerIndex = fromIndex; layerIndex < toIndex; layerIndex++){
        Layer* layer = layers.at(layerIndex);
        if(!layer->isVisible()) continue;

        unsigned int colorLayer;
        if(layer->getID().toString() == previewLayer->getID().toString()){
            weight = previewLayer->getOpacity();
            colorLayer = previewLayer->getPixel(index);
        }else{
            weight = layer->getOpacity();
            colorLayer = layer->getPixel(index);
        }
        colorLayer = (colorLayer & 0xFFFFFF00) | static_cast<int>(weight * (colorLayer & 0xFF));
        GraphicsEngine::blending(colorHex, colorLayer);
    }
    
    return colorHex;
}

unsigned int Frame::getFrameDuration() const{ return timeDuration; }

void Frame::setID(Guid id){
    _id = id;
}
Guid Frame::getID() const{
    return _id;
}

emscripten::val Frame::getBufferJS() {
    unsigned int* buffer = (unsigned int*) malloc(activeLayer->getLength() *sizeof(unsigned int));
    memset(buffer, 0, activeLayer->getLength()*sizeof(unsigned int));

    for(int layerIndex = 0; layerIndex < layers.size(); layerIndex++){
        Layer* layer = layers.at(layerIndex);
        if(!layer->isVisible()) continue;

        Bounding dirtyArea = Bounding(Point(0,0), Point(layer->getWidth(), layer->getHeight()));

        int index = dirtyArea.start.x + dirtyArea.start.y * layer->getWidth();
        int incrementY = 0;

        while(index < layer->getLength()){
            unsigned int colorLayer = layer->getPixel(index);
            GraphicsEngine::blending(buffer[index], colorLayer);
            
            // swap_endian_uint32(&buffer[index]);

            index++;
            incrementY++;
            if(incrementY >= dirtyArea.getWidth()){
                index += layer->getWidth() + dirtyArea.start.x - dirtyArea.getWidth();
                incrementY = 0;
            }
        }
    }

    int index = 0;
    while(index < activeLayer->getLength()){
        swap_endian_uint32(&buffer[index]);
        index++;
    }

    return emscripten::val(
        emscripten::typed_memory_view(activeLayer->getLength(), buffer)
    );
}
size_t Frame::getLayerIndex(Guid id) const{
    return std::distance(layers.cbegin(), getIteratorLayerByID(id));
}
void Frame::bringLayerTo(Guid id, size_t toIndex){
    auto from = getIteratorLayerByID(id);

    if (from == layers.end()) return;

    size_t fromIndex = std::distance(layers.cbegin(), from);
    
    if (fromIndex == toIndex || fromIndex >= layers.size() || toIndex >= layers.size()) return;
    
    if (fromIndex < toIndex) {
        std::rotate(layers.begin() + fromIndex, layers.begin() + fromIndex + 1, layers.begin() + toIndex + 1);
    } else {
        std::rotate(layers.begin() + toIndex, layers.begin() + fromIndex, layers.begin() + fromIndex + 1);
    }

    // if(activeLayer->getID().toString() == id.toString())
    // emscripten::val::global("move_layer_to")(emscripten::val(id), emscripten::val(toIndex));
}
void Frame::removeLayer(Guid id){
    auto it = getIteratorLayerByID(id);
    size_t index = it - layers.begin();

    if (it > layers.end()) return;

    layers.erase(it);

    if(id.toString() == activeLayer->getID().toString()){
        size_t activeIndex = std::min(layers.size()-1, std::max<size_t>(0, index));
        changeActiveLayer(layers[activeIndex]->getID());
    }
}
void Frame::addLayer(Layer* layer){
    layers.emplace_back(layer);
    
    if(layers.size() == 1){
        changeActiveLayer(layers[0]->getID());
    }
}
vector<Layer*> Frame::getAllLayers() const{
    return layers;
}
Layer* Frame::getLayerByID(Guid id) const{
    auto it = getIteratorLayerByID(id);
    return (it != layers.end()) ? *it : nullptr;
}
std::vector<Layer*>::const_iterator Frame::getIteratorLayerByID(Guid id) const{
    string idStr = id.toString();
    return std::find_if(layers.cbegin(), layers.cend(),
     [&idStr](Layer* f){
        return f->getID().toString() == idStr;
    });
}
Layer* Frame::getActiveLayer() const{
    return activeLayer;
}
void Frame::changeActiveLayer(Guid id){
    activeLayer = getLayerByID(id);
}




using namespace emscripten;
EMSCRIPTEN_BINDINGS(frame_module){
    register_vector<Layer*>("VectorLayer");

    class_<Frame>("Frame")
        .constructor<>()
        .smart_ptr<std::shared_ptr<Frame>>("shared_ptr<Frame>")
        .function("setID", &Frame::setID)
        .function("getID", &Frame::getID)
        .function("resize", &Frame::resize)
        .function("move", &Frame::move)
        .function("clone", &Frame::clone)
        .function("flipX", &Frame::flipX)
        .function("flipY", &Frame::flipY)
        .function("draw", &Frame::draw)
        .function("getFrameDuration", &Frame::getFrameDuration)
        .function("getBufferJS", &Frame::getBufferJS)
        
        .function("getLayerIndex", &Frame::getLayerIndex)
        .function("bringLayerTo", &Frame::bringLayerTo)
        .function("removeLayer", &Frame::removeLayer)
        .function("addLayer", &Frame::addLayer, allow_raw_pointers())
        .function("getAllLayers", &Frame::getAllLayers, allow_raw_pointers())
        .function("getLayerByID", &Frame::getLayerByID, allow_raw_pointers())
        .function("getActiveLayer", &Frame::getActiveLayer, allow_raw_pointers())
        .function("changeActiveLayer", &Frame::changeActiveLayer);
};