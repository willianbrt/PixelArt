#ifndef SURFACE
#define SURFACE

#include <memory>
#include <algorithm>
#include "../Pixel/Pixel.h"

class Surface{
private:
    unsigned int* _data = nullptr;
    unsigned int _width = 0;
    unsigned int _height = 0;
    unsigned int _length = 0;

public:
    Surface(unsigned int width, unsigned int height);
    ~Surface();
    unsigned int* getData();
    unsigned int getWidth();
    unsigned int getHeight();
    unsigned int getLength();
    Surface crop(Bounding bound);
    unsigned int getPixel(int x, int y);
    unsigned int getPixel(int index);
    void putPixel(int x, int y, unsigned int colorHex);
    void putPixel(int index, unsigned int colorHex);
    void setSize(int width, int height);
};
#endif