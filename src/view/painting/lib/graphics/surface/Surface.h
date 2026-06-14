#ifndef SURFACE_H
#define SURFACE_H

#include <memory>
#include <algorithm>
#include "../Pixel/Pixel.h"
#include "../GraphicsEngine/GraphicsEngine.h"
#include "../../interfaces/ISurface/ISurface.h"
#include <assert.h>

class Surface : public ISurface{
protected:
    unsigned int* _data = nullptr;
    int _width = 0;
    int _height = 0;
    unsigned int _length = 0;
private:
    void allocBuffer();
public:
    Surface() = default;
    Surface(unsigned int* buffer, int width, int height);
    Surface(int width, int height);
    ~Surface();

    uintptr_t getBufferPtr();
    Surface* crop(Bounding bound);

    unsigned int getPixel(int x, int y) override;
    unsigned int getPixel(unsigned int index) override;
    void putPixel(int x, int y, unsigned int colorHex) override;
    unsigned int* getBuffer() override;
    int getWidth() override;
    int getHeight() override;
    unsigned int getLength();


    void putPixel(unsigned int index, unsigned int colorHex);
    void setSize(int width, int height);
    bool isInsideSkecth(int x, int y);

    void translation(Bounding bound, int deltaX, int deltaY);
};
#endif