#include "Layers.h"

Layer::Layer(std::string name, unsigned int width, unsigned int height) : 
_id(Guid::generateUUID()),
 Surface(width, height)
 {
    _name = name;
}
Layer::Layer(const Layer& layer) : 
_id(layer.getID()),
 Surface(layer._width, layer._height)
{
    _data = new unsigned int[_length];;
    memcpy(_data, layer._data, sizeof(unsigned int) *_length);
    _name = layer._name;
    _isVisible = layer._isVisible;
    _isLock = layer._isLock;
    _opacity = layer._opacity;
}
Layer::~Layer(){}

void Layer::draw(IGraphic& graphic){ 
    graphic.draw(*this);
}
void Layer::move(int x, int y){
    // TODO: IMPLEMENTAR
    // int offset = y*_width + x;
    
    // for(unsigned int i = 0; i < (_width-y)*(_height-x); i++){
    //     _buffer[offset + i] = _buffer[i];
    // }
    
    // for(unsigned int i = offset; i < (_width+y)*(_height+x); i+=y){
    //     _buffer[i] = _buffer[offset + i];
    // }
}

void Layer::resize(int width, int height){
    // TODO: IMPLEMENTAR
}
Layer Layer::clone() const {
    Layer layer(*this);
    layer.setID(Guid::generateUUID());
    return layer;
}

void Layer::registerEvent(ILayerObserver* observer){
    observers.push_back(observer);
}


void Layer::setID(Guid id) { _id = id; }
Guid Layer::getID() const { return _id; }
bool Layer::isVisible() const { return _isVisible; }
void Layer::setVisible(bool isVisible){ 
    _isVisible = isVisible;
    
    for (auto* obs : observers) {
        obs->onIsVisibleLayer();
    }
}
bool Layer::isLock() const { return _isLock;}
void Layer::setLock(bool isLock){
    _isLock = isLock;
    
    for (auto* obs : observers) {
        obs->onIsLockLayer();
    }
}
std::string Layer::getName() const { return _name; }
void Layer::setName(std::string name) {
    _name = name;
    for (auto* obs : observers) {
        obs->onRenameLayer();
    }
}
float Layer::getOpacity() const { return _opacity; }
void Layer::setOpacity(float value){
    _opacity = value;

    for (auto* obs : observers) {
        obs->onOpacityLayer();
    }
}
unsigned int Layer::getFilteredPixel(unsigned int index) {
    if (index < 0 || index >= _length)  return 0;
    
    return static_cast<int>(_opacity * (_data[index] >> 24 & 0xFF)) << 24 | (_data[index] & 0x00FFFFFF);
}
