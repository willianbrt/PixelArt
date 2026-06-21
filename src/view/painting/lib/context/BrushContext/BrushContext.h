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
    std::vector<unsigned int> buffer;
    int width;
    int height;

    Pattern(std::string n, std::vector<unsigned int> b, int w, int h)
        : name(std::move(n)), buffer(std::move(b)), width(w), height(h) {}
    int getWidth() override { return height; }
    int getHeight() override { return width; }
    unsigned int* getBuffer() override { return buffer.data(); }

    void putPixel(int x, int y, unsigned int color) override {}
    unsigned int getPixel(int x, int y) override {  return getPixel(x+y*width); }
    unsigned int getPixel(unsigned int index) override { return buffer[index]; }
};
class BrushContext {
private:

public:
    BrushContext();
    Transformation transformation;
    Pattern* selectedPattern;
    static std::vector<Pattern> pattern;
    std::vector<Pattern>::iterator findPattern(std::string name);
    void setActivePattern(std::string name);
    Pattern& getPattern(std::string name);
};

#endif
