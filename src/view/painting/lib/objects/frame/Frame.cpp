#include "Frame.h"

Frame::Frame() : _id(Guid::generateUUID()) {
}

Frame::Frame(const Frame& frame) : _id(frame.getID()) {
    timeDuration = frame.getFrameDuration();

    for(size_t i = 0; i < frame.getLayersLength(); i++){
        Layer* layer = frame.getLayerByIndex(i);
        layers.emplace_back(new Layer(*layer));
    }
    activeLayer = getLayerByID(frame.getActiveLayer()->getID());
}

Frame::~Frame(){
    layers.clear();
    activeLayer = nullptr;
    delete previewLayer;
}

void Frame::registerEvent(IFrameObserver* observer){
    observers.push_back(observer);
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
    Frame frame = Frame(*this);
    frame.setID(Guid::generateUUID());
    return frame;
}
void Frame::flipX(){
    for(size_t i = 0; i < layers.size(); i++){
        Layer* layer = layers[i].get();

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
    for(size_t i = 0; i < layers.size(); i++){
        Layer* layer = layers[i].get();

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
    if(toIndex > layers.size()) throw std::runtime_error("ToIndex excede o tamanho maximo de Layers.");
    
    for(int layerIndex = fromIndex; layerIndex < toIndex; layerIndex++){
        Layer* layer = layers.at(layerIndex).get();
        if(!layer->isVisible()) continue;

        unsigned int colorLayer;
        if(layer->getID().toString() == previewLayer->getID().toString()){
            colorLayer = previewLayer->getFilteredPixel(index);
        }else{
            colorLayer = layer->getFilteredPixel(index);
        }
        
        colorHex = GraphicsEngine::blendColors(colorHex, colorLayer);
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

unsigned int* Frame::getBuffer() {
    unsigned int* buffer = (unsigned int*) malloc(activeLayer->getLength() *sizeof(unsigned int));
    memset(buffer, 0, activeLayer->getLength()*sizeof(unsigned int));

    for(int layerIndex = 0; layerIndex < layers.size(); layerIndex++){
        Layer* layer = layers.at(layerIndex).get();
        if(!layer->isVisible()) continue;

        Bounding dirtyArea = Bounding(Point(0,0), Point(layer->getWidth(), layer->getHeight()));

        int index = dirtyArea.start.x + dirtyArea.start.y * layer->getWidth();
        int incrementY = 0;

        while(index < layer->getLength()){
            unsigned int colorLayer = layer->getFilteredPixel(index);
            buffer[index] = GraphicsEngine::blendColors(buffer[index], colorLayer);

            index++;
            incrementY++;
            if(incrementY >= dirtyArea.getWidth()){
                index += layer->getWidth() + dirtyArea.start.x - dirtyArea.getWidth();
                incrementY = 0;
            }
        }
    }

    return buffer;
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
    
    Layer* layerPtr = layers[toIndex].get();
    for (auto* obs : observers) {
        obs->onMoveLayerTo(layerPtr->getID(), toIndex);
    }
}
unique_ptr<Layer> Frame::removeLayer(size_t index){
    if(index < 0 || index >= layers.size()) return nullptr;
    
    for (auto* obs : observers) {
        obs->onRemoveLayer(layers[index]->getID());
    }
    unique_ptr<Layer> layerFrame = std::move(layers[index]);
    layers.erase(layers.begin() + index);
    return layerFrame;
}
void Frame::addLayer(unique_ptr<Layer> layer, size_t index){
    layers.insert(layers.begin() + index,  std::move(layer));

    Layer* layerPtr = layers[index].get();
    for (auto* obs : observers) {
        obs->onAddLayer(layerPtr, index);
    }

    changeActiveLayer(layerPtr->getID());
}

Layer* Frame::getLayerByIndex(size_t index) const{
    return layers[index].get();
}

size_t Frame::getLayersLength()  const{
    return layers.size();
}

Layer* Frame::getLayerByID(Guid id) const{
    auto it = getIteratorLayerByID(id);
    return (it != layers.end()) ?  it->get() : nullptr;
}

size_t Frame::getLayerIndex(Guid id) const{
    return std::distance(layers.cbegin(), getIteratorLayerByID(id));
}
std::vector<unique_ptr<Layer>>::const_iterator Frame::getIteratorLayerByID(Guid id) const{
    string idStr = id.toString();
    return std::find_if(layers.cbegin(), layers.cend(),
     [&idStr](const std::unique_ptr<Layer>& l){
        return l.get()->getID().toString() == idStr;
    });
}
Layer* Frame::getActiveLayer() const{
    return activeLayer;
}
void Frame::changeActiveLayer(Guid id){
    activeLayer = getLayerByID(id);
    for (auto* obs : observers) {
        obs->onChangeActiveLayer(activeLayer->getID());
    }
}

