#ifndef FRAME_H
#define FRAME_H

#include <emscripten.h>
#include <emscripten/bind.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <array>

#include "../../helpers/Pixel/Pixel.h"
#include "../../interfaces/IGraphic/IGraphic.h"
#include "../../interfaces/ITile/ITile.h"
#include "../layer/Layers.h"
#include "../componding/CompoundingTiles.h"

const int MAX_LAYERS = 30;

class Frame : public ITile
{
public:
    Frame(unsigned int width, unsigned int height);
    ~Frame();

    void resize(int width, int height);
    void move(int offsetX, int offsetY);
    void draw(IGraphic& graphic);

    unsigned int getWidth();
    unsigned int getHeight();

    unsigned int getPixel(int x, int y);
    unsigned int getPixel(int index);
    unsigned int getPixel(int index, int fromIndex, int toIndex);

    void putPixel(int x, int y, unsigned int colorHex);
    void putPixel(int index, unsigned int colorHex);

    unsigned int calcIndex(int x, int y);

    unsigned int getFrameDuration();
    
    void bringTileTo(size_t from, size_t to);
    void removeTile(int index);
    void addTile(Layer* tile);
    vector<Layer*> getAllTiles();
    Layer* getTileByIndex(unsigned int index);
    Layer* getActiveTile();
    int getIndexFromActiveTile();
    void changeActiveTile(unsigned int index);
    size_t getNumberOfTiles();


private:
    void blending(unsigned int& bottomColor, unsigned int topColor);
    
    unsigned int timeDuration = 800;
    unsigned int _width = 0;
    unsigned int _height = 0;

    // vector<Layer*> tiles = vector<Layer*>(MAX_LAYERS);
    vector<Layer*> tiles;
    int active = 0;
};


#endif