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
    return Layer(*this);
}

void Layer::setID(Guid id) { _id = id; }
Guid Layer::getID() const { return _id; }
bool Layer::isVisible() const { return _isVisible; }
void Layer::setVisible(bool isVisible){ _isVisible = isVisible; }
bool Layer::isLock() const { return _isLock;}
void Layer::setLock(bool isLock){ _isLock = isLock; }
std::string Layer::getName() const { return _name; }
void Layer::setName(std::string name) { _name = name; }
float Layer::getOpacity() const { return _opacity; }
void Layer::setOpacity(float value){ _opacity = value; }


using namespace emscripten;

EMSCRIPTEN_BINDINGS(layer_module){
    class_<Layer, base<Surface>>("Layer")
    .constructor<std::string, unsigned int, unsigned int>()
    .smart_ptr<std::shared_ptr<Layer>>("shared_ptr<Layer>")
    .function("setID", &Layer::setID)
    .function("getID", &Layer::getID)
    .function("resize", &Layer::resize)
    .function("move", &Layer::move)
    .function("clone", &Layer::clone)
    .function("draw", &Layer::draw)
    .function("getName", &Layer::getName)
    .function("setName", &Layer::setName)
    .function("getOpacity", &Layer::getOpacity)
    .function("setOpacity", &Layer::setOpacity)
    .function("setVisible", &Layer::setVisible)
    .function("isVisible", &Layer::isVisible)
    .function("setLock", &Layer::setLock)
    .function("isLock", &Layer::isLock)
    .function("getBuffer", &Layer::getBuffer, emscripten::allow_raw_pointer<unsigned int*>() );
};