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
#include <unordered_map>

#include "../../graphics/Pixel/Pixel.h"
#include "../../helpers/Guid/Guid.h"

#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../interfaces/ITile/ITile.h"
#include "../../interfaces/IFrameObserver/IFrameObserver.h"

#include "../layer/Layers.h"
#include "../preview/Preview.h"

#include "../../graphics/GraphicsEngine/GraphicsEngine.h"

enum FRAME_EVENT_TYPE{
    ADD_LAYER,
    REMOVE_LAYER,
    MOVE_LAYER_TO,
    CHANGE_ACTIVE_LAYER
};

const int MAX_LAYERS = 30;

class Frame : public ITile
{
public:
    Frame();
    Frame(const Frame& frame);
    ~Frame();

    void registerEvent(IFrameObserver* observer);

    void preview(IGraphic& graphic);
    void draw(IGraphic& graphic);
    void resize(int width, int height);
    void move(int offsetX, int offsetY);
    Frame clone() const;
    void flipX();
    void flipY();

    unsigned int getPixel(unsigned int index);
    unsigned int getPixel(unsigned int index, int fromIndex, int toIndex);
    unsigned int* getBuffer();

    
    void setID(Guid id);
    Guid getID() const;
    unsigned int getFrameDuration() const;

    void addLayer(unique_ptr<Layer> tile, size_t index);
    unique_ptr<Layer>  removeLayer(size_t index);
    void bringLayerTo(Guid id, size_t to);
    void changeActiveLayer(Guid id);
    
    Layer* getActiveLayer() const;
    size_t getLayerIndex(Guid id) const;
    Layer* getLayerByID(Guid id) const;
    size_t getLayersLength() const;
    Layer* getLayerByIndex(size_t index) const; 
    
private:
    std::vector<unique_ptr<Layer>>::const_iterator getIteratorLayerByID(Guid id) const;
    void blending(unsigned int& bottomColor, unsigned int topColor);
    
    unsigned int timeDuration = 800;
    vector<std::unique_ptr<Layer>> layers;
    vector<IFrameObserver*> observers;
    Layer* activeLayer = 0;
    Layer* previewLayer = 0;
    
    Guid _id;
};


#endif