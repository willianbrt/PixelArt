#ifndef LAYERS_H
#define LAYERS_H

#include <emscripten.h>
#include <emscripten/bind.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"

using namespace std;

class Layer : public ITile {
private: 
    unsigned int* _buffer;
    unsigned int _width = 0;
    unsigned int _height = 0;
    unsigned int _length = 0;
    bool _isVisible = true;
    std::string _name = nullptr;
    
public:
    Layer(std::string name, unsigned int width, unsigned int height);
    ~Layer();

    void resize(int width, int height);
    void move(int x, int y);
    void draw(IGraphic& graphic);

    unsigned int getPixel(int x, int y);
    unsigned int getPixel(int index);

    void putPixel(int x, int y, unsigned int colorHex);
    void putPixel(int index, unsigned int colorHex);
    unsigned int calcIndex(int x, int y);
    
    bool isVisible();
    void setVisible(bool isVisible);
    std::string getName();
    
    unsigned int* getBuffer();
    unsigned int getWidth();
    unsigned int getHeight();
    
};
#endif