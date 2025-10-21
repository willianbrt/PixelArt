#include "Surface.h"

Surface::Surface(unsigned int width, unsigned int height){
    setSize(width, height);
}
Surface::~Surface(){
    free(_data);
}

unsigned int* Surface::getBuffer() {
    return _data;
}
unsigned int Surface::getWidth() {
    return _width;
}
unsigned int Surface::getHeight() {
    return _height;
}
unsigned int Surface::getLength() {
    return _height*_width;
}
Surface Surface::crop(Bounding bound){
    Surface dirtSurface = Surface(bound.getWidth(), bound.getHeight());
    
    unsigned int boundWidth = bound.getWidth();
    unsigned long length = boundWidth * sizeof(unsigned int);
    unsigned int* ptrDestination = dirtSurface.getBuffer();
    unsigned int* ptr = _data + (bound.start.x + boundWidth*bound.start.y);
    unsigned int* ptrEnd = _data + (bound.end.x + boundWidth*bound.end.y);
    
    for(; ptr <= ptrEnd; ptr += boundWidth){
        std::copy(ptr, ptr+length, ptrDestination);
    }

    return dirtSurface;
}
unsigned int Surface::getPixel(int x, int y) {
    if (!isInsideSkecth(x, y)) return 0;

    assert(x < _width && "error: x maior que a width;");
    assert(x >= 0 && "error: x menor que 0;");

    assert(y < _height && "error: y maior que a width;");
    assert(y >= 0 && "error: y menor que 0;");

    return getPixel(x + y*_width);
}
unsigned int Surface::getPixel(unsigned int index) {
    if (index < 0 || index >= _length)  return 0;

    assert(index < _length && "error: index maior que o buffer;");
    assert(index >= 0 && "error: index menor que 0;");
    
    return _data[index];
}
void Surface::putPixel(int x, int y, unsigned int colorHex){    
    if (!isInsideSkecth(x, y)) return;

    assert(x < _width && "error: x maior que a width;");
    assert(x >= 0 && "error: x menor que 0;");

    assert(y < _height && "error: y maior que a width;");
    assert(y >= 0 && "error: y menor que 0;");

    putPixel(x + y*_width, colorHex);
}
bool Surface::isInsideSkecth(int x, int y) {
    return x < _width && y < _height && x >= 0 && y >= 0;
}
void Surface::putPixel(unsigned int index, unsigned int colorHex){
    if (index < 0 || index >= _length) return;

    assert(index < _length && "error: index maior que o buffer;");
    assert(index >= 0 && "error: index menor que 0;");

    _data[index] = colorHex;
}
void Surface::setSize(int width, int height){
    _length = width*height;
    _width = width;
    _height = height;

    _data = (unsigned int*) malloc(_length*sizeof(unsigned int));

    if(!_data){
        free(_data);
        throw std::runtime_error("Impossível alocar memoria para layer");
    }
    
    memset(_data, 0, _length*sizeof(unsigned int));
}
EMSCRIPTEN_BINDINGS(surface_module){
    class_<Surface>("Surface")
        .constructor<unsigned int, unsigned int>()
        .function("putPixel",  select_overload<void(int, int, unsigned int)>(&Surface::putPixel))
        .function("getPixel", select_overload<unsigned int(int, int)>(&Surface::getPixel));
}