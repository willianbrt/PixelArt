#ifndef SURFACE_H
#define SURFACE_H

#include <emscripten/val.h>
#include <emscripten/bind.h>
using namespace emscripten;

#include <memory>
#include <algorithm>
#include "../Pixel/Pixel.h"
#include <assert.h>

class Surface{
protected:
    unsigned int* _data = nullptr;
    int _width = 0;
    int _height = 0;
    unsigned int _length = 0;

public:
    Surface() = default;
    Surface(int width, int height);
    ~Surface();

    uintptr_t getBufferPtr();
    unsigned int* getBuffer();
    int getWidth();
    int getHeight();
    unsigned int getLength();
    Surface* crop(Bounding bound);
    unsigned int getPixel(int x, int y);
    unsigned int getPixel(unsigned int index);
    void putPixel(int x, int y, unsigned int colorHex);
    void putPixel(unsigned int index, unsigned int colorHex);
    void setSize(int width, int height);
    bool isInsideSkecth(int x, int y);
};
#endif