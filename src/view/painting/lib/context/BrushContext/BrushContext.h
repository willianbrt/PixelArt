#ifndef BRUSHCONTEXT_H
#define BRUSHCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"
#include "../../interfaces/ISurface/ISurface.h"
#include "../../graphics/Transformation/Transformation.h"
#include <unordered_map>
#include <vector>
#include <string>

struct Pattern : public ISurface{
public:
    std::string name;
    // std::vector<unsigned int> buffer;
    unsigned int* buffer;
    int width;
    int height;
    Point source;

    // Pattern(std::string n, std::vector<unsigned int> b, int w, int h)   
    //     : name(std::move(n)), buffer(std::move(b)), width(w), height(h) {}

    Pattern(std::string n, unsigned int* b, int size, int width)   
        : name(std::move(n)), buffer(b), width(width), height(size/width) {}
    ~Pattern(){
        std::free(buffer);
    }
    int getWidth() override { return width; }
    int getHeight() override { return height; }
    unsigned int* getBuffer() override { return buffer; }

    void putPixel(int x, int y, unsigned int color) override {}
    unsigned int getPixel(int x, int y) override {  return getPixel(x+y*width); }
    unsigned int getPixel(unsigned int index) override { return buffer[index]; }
};

enum FILL_MODE{
    FILL = 0,
    MASK = 1,
    STAMP = 2
};

enum ALIGN_MODE{
    BRUSHING = 0,
    LOCAL = 1,
    SOURCE = 2
};


class BrushContext {
private:

public:
    BrushContext();

    FILL_MODE fillMode = FILL_MODE::FILL;
    ALIGN_MODE alignMode = ALIGN_MODE::BRUSHING;

    Transformation transformation;
    Pattern* selectedPattern;
    unsigned int* getShape(std::string name);
    void setShape(std::string name);
};

#endif
