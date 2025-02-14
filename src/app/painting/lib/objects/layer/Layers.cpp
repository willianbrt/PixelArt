#include "Layers.h"

Layer::Layer(std::string name, unsigned int width, unsigned int height){
    int length = width*height;

    _buffer = (unsigned int*) malloc(length*sizeof(unsigned int));

    if(!_buffer){
        free(_buffer);
        throw runtime_error("Impossível alocar memoria para layer");
    }
    
    memset(_buffer, 0, length*sizeof(unsigned int));
    
    _width = width;
    _height = height;
    _length = length;
    _name = name;
}
Layer::~Layer(){
    free(_buffer);
}
void Layer::resize(int width, int height){
    int length = width*height;

    _buffer = (unsigned int*) malloc(length*sizeof(unsigned int));

    if(!_buffer){
        free(_buffer);
        throw runtime_error("Impossível alocar memoria para layer");
    }
    
    memset(_buffer, 0, length*sizeof(unsigned int));
}
bool Layer::isVisible(){
    return _isVisible;
}
void Layer::setVisible(bool isVisible){
    _isVisible = isVisible;
}   
std::string Layer::getName() {
    return _name;
}

void Layer::move(int x, int y){
    int offset = y*_width + x;
    
    for(unsigned int i = 0; i < (_width-y)*(_height-x); i++){
        _buffer[offset + i] = _buffer[i];
    }
    
    for(unsigned int i = offset; i < (_width+y)*(_height+x); i+=y){
        _buffer[i] = _buffer[offset + i];
    }
}

// void accept(ITileVisitor visitor){
//     visitor.layerVisitor(this);
// }

void Layer::draw(IGraphic& graphic){
    graphic.draw(*this);
}

unsigned int* Layer::getBuffer(){ return _buffer; }
unsigned int Layer::getWidth(){ return _width; }
unsigned int Layer::getHeight(){ return _height; }

unsigned int Layer::getPixel(int x, int y){ return getPixel(calcIndex(x,y)); }
unsigned int Layer::getPixel(int index){ return _buffer[index]; }

void Layer::putPixel(int x, int y, unsigned int colorHex){ putPixel(calcIndex(x, y), colorHex); }
void Layer::putPixel(int index, unsigned int colorHex){ _buffer[index] = colorHex; }

unsigned int Layer::calcIndex(int x, int y){ return x + y*_width; }

using namespace emscripten;

EMSCRIPTEN_BINDINGS(layer_module){        
    // class_<Layer>("Layer")
    //             .constructor<std::string, unsigned int, unsigned int>()
    //             .smart_ptr<std::shared_ptr<Layer>>("shared_ptr<Layer>")
    //             // .function("getPixel", &Layer::getPixel)
    //             // .function("putPixel", &Layer::putPixel)
    //             .function("getBuffer", &Layer::getBuffer, allow_raw_pointers());
    class_<Layer>("Layer")
    .constructor<std::string, unsigned int, unsigned int>()
    .smart_ptr<std::shared_ptr<Layer>>("shared_ptr<Layer>")
    .function("resize", &Layer::resize)
    .function("move", &Layer::move)
    .function("draw", &Layer::draw)
    // .function("getPixel", &Layer::getPixel)
    // .function("putPixel", &Layer::putPixel)
    .function("getName", &Layer::getName)
    .function("getWidth", &Layer::getWidth)
    .function("getHeight", &Layer::getHeight)
    .function("setVisible", &Layer::setVisible)
    .function("isVisible", &Layer::isVisible)
    .function("getBuffer", &Layer::getBuffer, emscripten::allow_raw_pointer<unsigned int*>() );
};