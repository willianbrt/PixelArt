#ifndef ISURFACE_H
#define ISURFACE_H

class ISurface{
public:
    virtual ~ISurface() = default;
    virtual void putPixel(unsigned int index, unsigned int color) = 0;
    virtual unsigned int getPixel(unsigned int index) = 0;
};
#endif