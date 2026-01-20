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
#include "../../graphics/surface/Surface.h"

using namespace std;

class Layer : public ITile, public Surface {
private: 
    Guid _id;
    bool _isVisible = true;
    bool _isLock = false;
    float _opacity = 1.0f;
    std::string _name = nullptr;
    
public:
    Layer(std::string name, unsigned int width, unsigned int height);
    Layer(const Layer& layer);
    ~Layer();

    void resize(int width, int height);
    void move(int x, int y);
    Layer clone() const;
    void draw(IGraphic& graphic);
    
    void setID(Guid id);
    Guid getID() const;
    bool isVisible() const;
    void setVisible(bool isVisible);
    bool isLock() const;
    void setLock(bool isLock);
    float getOpacity() const;
    void setOpacity(float value);
    std::string getName() const;
    void setName(string name);
};
#endif