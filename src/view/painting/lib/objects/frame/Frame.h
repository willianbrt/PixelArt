#ifndef FRAME_H
#define FRAME_H

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <array>
#include <regex>

#include "../../graphics/Pixel/Pixel.h"
#include "../../helpers/Guid/Guid.h"

#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../interfaces/ITile/ITile.h"

#include "../layer/Layers.h"
#include "../preview/Preview.h"
#include "../componding/CompoundingTiles.h"

#include "../../graphics/GraphicsEngine/GraphicsEngine.h"

const int MAX_LAYERS = 30;

class Frame : public ITile
{
public:
    Frame();
    Frame(const Frame& frame);
    ~Frame();

    void preview(IGraphic& graphic);
    void draw(IGraphic& graphic);
    void resize(int width, int height);
    void move(int offsetX, int offsetY);
    Frame clone() const;
    void flipX();
    void flipY();

    unsigned int getPixel(unsigned int index);
    unsigned int getPixel(unsigned int index, int fromIndex, int toIndex);
    emscripten::val getBufferJS();

    unsigned int getFrameDuration() const;
    
    void setID(Guid id);
    Guid getID() const;
    void bringLayerTo(Guid id, size_t to);
    size_t getLayerIndex(Guid id) const;
    void removeLayer(Guid id);
    void addLayer(Layer* tile);
    vector<Layer*> getAllLayers() const;
    Layer* getActiveLayer() const;
    int getIndexFromActiveLayer();
    void changeActiveLayer(Guid id);
    size_t getNumberOfLayers();
    Layer* getLayerByID(Guid id) const;
    std::vector<Layer*>::const_iterator getIteratorLayerByID(Guid id) const;

private:
    void blending(unsigned int& bottomColor, unsigned int topColor);
    
    unsigned int timeDuration = 800;
    // vector<Layer*> tiles = vector<Layer*>(MAX_LAYERS);
    vector<Layer*> layers;
    Layer* activeLayer = 0;
    Layer* previewLayer = 0;
    
    Guid _id;
};


#endif