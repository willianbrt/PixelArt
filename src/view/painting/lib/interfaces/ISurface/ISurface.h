#ifndef ISURFACE_H
#define ISURFACE_H

class ISurface{
public:
    virtual ~ISurface() = default;
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual void putPixel(int x, int y, unsigned int color) = 0;
    virtual unsigned int getPixel(int x, int y) = 0;
    virtual unsigned int getPixel(unsigned int index) = 0;
    virtual unsigned int* getBuffer() = 0;
};
#endif