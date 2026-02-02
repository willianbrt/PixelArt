#include "Surface.h"

Surface::Surface(int width, int height){
    setSize(width, height);
}
Surface::~Surface(){
    free(_data);
}

uintptr_t Surface::getBufferPtr() {
    return reinterpret_cast<uintptr_t>(_data);
}
unsigned int* Surface::getBuffer() {
    return _data;
}
int Surface::getWidth() {
    return _width;
}
int Surface::getHeight() {
    return _height;
}
unsigned int Surface::getLength() {
    return _height*_width;
}
Surface* Surface::crop(Bounding bound){
    bound.start.x = std::min(_width, std::max(0 ,bound.start.x));
    bound.start.y = std::min(_height, std::max(0, bound.start.y));
    bound.end.x = std::min(_width, std::max(0, bound.end.x));
    bound.end.y = std::min(_height, std::max(0, bound.end.y));

    Surface* dirtSurface = new Surface(bound.getWidth(), bound.getHeight());
    
    int boundWidth = bound.getWidth();
    unsigned long length = boundWidth * sizeof(unsigned int);
    unsigned int* ptrDestination = dirtSurface->getBuffer();
    unsigned int* ptr = _data + (bound.start.x + _width*bound.start.y);
    unsigned int* ptrEnd = ptr + (_width*bound.end.y);

    for (unsigned int* index = ptr; index < ptrEnd; index += _width) {
        memcpy(ptrDestination, index, length);
        ptrDestination += boundWidth;
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
void Surface::putPixel(int x, int y, unsigned int colorHex,
     int nRows, int nCols,
     bool isMirrorX, bool isMirrorY){
    if(x >= _width*nRows || y >= _height*nCols || x < 0 || y < 0) return;

    x %= _width;
    y %= _height;

    putPixel(x + y*_width, colorHex);
    
    if(isMirrorX)
        putPixel((_width - x - 1) + y*_width, colorHex);
    if(isMirrorY)
        putPixel(x + (_height - y - 1)*_width, colorHex);
    if(isMirrorX && isMirrorY)
        putPixel((_width - x - 1) + (_height - y - 1)*_width, colorHex);
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

void Surface::translation(Bounding bound, int deltaX, int deltaY){
    for(int y = bound.start.y; y < bound.end.y; y++){
        for(int x = bound.start.x; x < bound.end.x; x++){
            int index = x + _width*y;
            int newIndex = (x + deltaX) + _width*(y + deltaY);

            _data[newIndex] = _data[index];
            _data[index] = 0x0;
        }
    }
}

EMSCRIPTEN_BINDINGS(surface_module){
    class_<Surface>("Surface")
        .constructor<int, int>()
        .smart_ptr<std::shared_ptr<Surface>>("shared_ptr<Surface>")
        .function("getWidth",  &Surface::getWidth)
        .function("getHeight", &Surface::getHeight)
        .function("getBufferPtr", &Surface::getBufferPtr)
        .function("getLength", &Surface::getLength)
        .function("putPixel",  select_overload<void(int, int, unsigned int)>(&Surface::putPixel))
        .function("putPixelByIndex",  select_overload<void(unsigned int, unsigned int)>(&Surface::putPixel))
        .function("getPixel", select_overload<unsigned int(int, int)>(&Surface::getPixel))
        .function("getPixelByIndex", select_overload<unsigned int(unsigned int)>(&Surface::getPixel))
        ;
}