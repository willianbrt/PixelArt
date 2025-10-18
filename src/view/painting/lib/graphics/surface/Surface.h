#ifndef SURFACE_H
#define SURFACE_H

#include <emscripten/bind.h>
using namespace emscripten;

#include <memory>
#include <algorithm>
#include "../Pixel/Pixel.h"
#include <assert.h>

class Surface{
private:
    unsigned int* _data = nullptr;
    unsigned int _width = 0;
    unsigned int _height = 0;
    unsigned int _length = 0;

public:
    Surface(unsigned int width, unsigned int height);
    ~Surface();
    
    unsigned int* getBuffer();
    unsigned int getWidth();
    unsigned int getHeight();
    unsigned int getLength();
    Surface crop(Bounding bound);
    unsigned int getPixel(int x, int y);
    unsigned int getPixel(unsigned int index);
    void putPixel(int x, int y, unsigned int colorHex);
    void putPixel(unsigned int index, unsigned int colorHex);
    void setSize(int width, int height);
    bool isInsideSkecth(int x, int y);
};
#endif