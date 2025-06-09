#include "Layers.h"

Layer::Layer(std::string name, unsigned int width, unsigned int height) : id(Guid::generateUUID()), _sketch(width, height){
    _name = name;
}
Layer::~Layer(){}

void Layer::draw(IGraphic& graphic){ graphic.draw(*this); }
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

unsigned int* Layer::getBuffer(){ return _sketch.getData(); }
unsigned int Layer::getPixel(int x, int y){ return _sketch.getPixel(x, y); }
unsigned int Layer::getPixel(int index){ return _sketch.getPixel(index); }
void Layer::putPixel(int x, int y, unsigned int colorHex){ _sketch.putPixel(x, y, colorHex); }
void Layer::putPixel(int index, unsigned int colorHex){ _sketch.putPixel(index, colorHex); }

Guid Layer::getID(){ return id; }
bool Layer::isVisible(){ return _isVisible; }
void Layer::setVisible(bool isVisible){ _isVisible = isVisible; }
bool Layer::isLock(){ return _isLock;}
void Layer::setLock(bool isLock){ _isLock = isLock; }
std::string Layer::getName() { return _name; }
void Layer::setName(std::string name) { _name = name; }
unsigned int Layer::getOpacity(){ return _opacity; }
void Layer::setOpacity(unsigned int value){ _opacity = value; }


using namespace emscripten;

EMSCRIPTEN_BINDINGS(layer_module){
    class_<Layer>("Layer")
    .constructor<std::string, unsigned int, unsigned int>()
    .smart_ptr<std::shared_ptr<Layer>>("shared_ptr<Layer>")
    .function("getID", &Layer::getID)
    .function("resize", &Layer::resize)
    .function("move", &Layer::move)
    .function("draw", &Layer::draw)
    .function("putPixel", select_overload<void(int, int, unsigned int)>(&Layer::putPixel))
    .function("getPixel", select_overload<unsigned int(int, int)>(&Layer::getPixel))
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