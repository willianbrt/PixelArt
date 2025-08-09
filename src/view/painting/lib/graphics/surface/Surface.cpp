#include "Surface.h"

Surface::Surface(unsigned int width, unsigned int height){
    setSize(width, height);
}
Surface::~Surface(){
    free(_data);
}

unsigned int* Surface::getData(){
    return _data;
}
unsigned int Surface::getWidth(){
    return _width;
}
unsigned int Surface::getHeight(){
    return _height;
}
unsigned int Surface::getLength(){
    return _height*_width;
}
Surface Surface::crop(Bounding bound){
    Surface dirtSurface = Surface(bound.getWidth(), bound.getHeight());
    
    unsigned int boundWidth = bound.getWidth();
    unsigned long length = boundWidth * sizeof(unsigned int);
    unsigned int* ptrDestination = dirtSurface.getData();
    unsigned int* ptr = _data + (bound.start.x + boundWidth*bound.start.y);
    unsigned int* ptrEnd = _data + (bound.end.x + boundWidth*bound.end.y);
    
    for(; ptr <= ptrEnd; ptr += boundWidth){
        std::copy(ptr, ptr+length, ptrDestination);
    }

    return dirtSurface;
}
unsigned int Surface::getPixel(int x, int y){ return getPixel(x + y*_width); }
unsigned int Surface::getPixel(int index){ return _data[index]; }
void Surface::putPixel(int x, int y, unsigned int colorHex){ putPixel(x + y*_width, colorHex); }
void Surface::putPixel(int index, unsigned int colorHex){ _data[index] = colorHex; }
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