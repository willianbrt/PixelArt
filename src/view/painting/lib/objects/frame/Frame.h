#ifndef FRAME_H
#define FRAME_H

#include <emscripten.h>
#include <emscripten/bind.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <array>

#include "../../graphics/Pixel/Pixel.h"
#include "../../helpers/Guid/Guid.h"

#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../interfaces/ITile/ITile.h"

#include "../layer/Layers.h"
#include "../componding/CompoundingTiles.h"

#include "../../graphics/GraphicsEngine/GraphicsEngine.cpp"

const int MAX_LAYERS = 30;

class Frame : public ITile
{
public:
    Frame(unsigned int width, unsigned int height);
    ~Frame();

    void draw(IGraphic& graphic);
    void resize(int width, int height);
    void move(int offsetX, int offsetY);

    unsigned int getPixel(int index);
    unsigned int getPixel(int index, int fromIndex, int toIndex);

    unsigned int getFrameDuration();
    
    Guid getID();
    void bringLayerToFoward(Guid id);
    void bringLayerBack(Guid id);
    void bringLayerTo(Guid id, size_t to);
    void removeLayer(Guid id);
    void addLayer(Layer* tile);
    vector<Layer*> getAllLayers();
    Layer* getActiveLayer();
    int getIndexFromActiveLayer();
    void changeActiveLayer(Guid id);
    size_t getNumberOfLayers();
    Layer* getLayerByID(Guid id);
    std::vector<Layer*>::iterator getIteratorLayerByID(Guid id);

private:
    void blending(unsigned int& bottomColor, unsigned int topColor);
    
    unsigned int timeDuration = 800;
    // vector<Layer*> tiles = vector<Layer*>(MAX_LAYERS);
    vector<Layer*> layers;
    Layer* activeLayer = 0;
    
    Guid id;
};


#endif