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

#include "../../helpers/Guid/Guid.h"

#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../graphics/Surface/Surface.cpp"

using namespace std;

class Layer : public ITile {
private: 
    Guid id;
    Surface _sketch;
    bool _isVisible = true;
    bool _isLock = false;
    unsigned int _opacity = 100;
    std::string _name = nullptr;
    
public:
    Layer(std::string name, unsigned int width, unsigned int height);
    ~Layer();

    void resize(int width, int height);
    void move(int x, int y);
    void draw(IGraphic& graphic);

    unsigned int* getBuffer();
    unsigned int getPixel(int x, int y);
    unsigned int getPixel(int index);
    void putPixel(int x, int y, unsigned int colorHex);
    void putPixel(int index, unsigned int colorHex);
    
    Guid getID();
    bool isVisible();
    void setVisible(bool isVisible);
    bool isLock();
    void setLock(bool isLock);
    unsigned int getOpacity();
    void setOpacity(unsigned int value);
    std::string getName();
    void setName(string name);
};
#endif