#ifndef SURFACE
#define SURFACE

#include <memory>
#include <algorithm>

class Surface{
private:
    unsigned int* _data = nullptr;
    unsigned int _width = 0;
    unsigned int _height = 0;
    unsigned int _length = 0;

public:
    Surface(unsigned int width, unsigned int height){
        setSize(width, height);
    }
    ~Surface(){
        free(_data);
    }
    
    unsigned int* getData(){
        return _data;
    }
    unsigned int getWidth(){
        return _width;
    }
    unsigned int getHeight(){
        return _height;
    }
    unsigned int getLength(){
        return _height*_width;
    }
    Surface crop(Bounding bound){
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
    unsigned int getPixel(int x, int y){ return getPixel(x + y*_width); }
    unsigned int getPixel(int index){ return _data[index]; }
    void putPixel(int x, int y, unsigned int colorHex){ putPixel(x + y*_width, colorHex); }
    void putPixel(int index, unsigned int colorHex){ _data[index] = colorHex; }
    void setSize(int width, int height){
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
};
#endif