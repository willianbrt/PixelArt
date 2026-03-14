#ifndef ISURFACE_H
#define ISURFACE_H

class ISurface{
public:
    virtual ~ISurface() = default;
    virtual void putPixel(int x, int y, unsigned int color) = 0;
    virtual unsigned int getPixel(int x, int y) = 0;
    virtual unsigned int getPixel(unsigned int index) = 0;
};
#endif