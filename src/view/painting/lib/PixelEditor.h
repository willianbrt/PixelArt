#ifndef PIXEL_EDITOR
#define PIXEL_EDITOR


#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>

#include "objects/componding/CompoundingTiles.h"
#include "objects/layer/Layers.h"
#include "objects/frame/Frame.h"
#include "interfaces/IGraphic/IGraphic.h"
#include "helpers/Pixel/Pixel.h"

const unsigned int CHECKERED_LIGHT_COLOR_HEX = 0xFFFFFFFF;
const unsigned int CHECKERED_DARK_COLOR_HEX = 0xDDDDDDFF;
const unsigned int CHECKERED_HOVER_LIGHT_COLOR_HEX = 0x555555FF;
const unsigned int CHECKERED_HOVER_DARK_COLOR_HEX = 0x222222FF;
const int MAX_FRAMES = 10;

extern "C" {
    void add_layer(ITile tile);
    void remove_layer(ITile tile);
    void draw_layer(Guid tile);
    void move_layer();
    void draw_frame(IGraphic& graphic);
}

class PixelEditor
 {
    Point sketchLocation;
    unsigned int* screen = nullptr;
    unsigned int _width = 0;
    unsigned int _height = 0;
    int _viewportWidth = 0;
    int _viewportHeight = 0;
    unsigned int _length = 0;
    unsigned int _scale = 1;
    Layer previewLayer;
    vector<Frame*> tiles;
    
    int active = 0;

public:
    PixelEditor(unsigned int width, unsigned int height);
    ~PixelEditor();

    void renderPreview(IGraphic& graphic);
    void abortPreview();
    void draw(IGraphic& graphic);
    
    void render();
    void renderArea(Point start, Point end);
    void clear(Point start, Point end);

    void zoomIn(Point cursorPosition);
    void zoomOut(Point cursorPosition);
    void zoom(int targetScale, Point cursorPosition);

    void panning(Point fromCursor, Point toCursor);
    void move(int x, int y);

    int getCurrentWidth();
    int getCurrentHeight();

    Point getInitialPosition();

    int getViewportWidth();
    int getViewportHeight();

    int getScale();
    int getMinScale();
    int getMaxScale();
    Point getPosition();

    unsigned int getPixel(int index);
    unsigned int getPixel(int index, int fromIndex, int toIndex);
    
    void putPixel(int x, int y, unsigned int colorHex);
    void putPixel(int index, unsigned int colorHex);
    void hover(int x, int y);


    void bringTileTo(size_t from, size_t to);
    void removeTile(int index);
    void addTile(Frame* tile);
    vector<Frame*> getAllTiles();
    size_t getNumberOfTiles();
    Frame* getTileByIndex(unsigned int index);
    Frame* getActiveTile();
    int getIndexFromActiveTile();
    void changeActiveTile(unsigned int index);

private:
    void updatePixel(int x, int y);
    unsigned int getDefaultColor(int x, int y);
    unsigned int getPixelWithPreview(int x, int y);
    unsigned int getPixelWithPreview(int index);

    void blending(unsigned int& bottomColor, unsigned int topColor);
    
    bool computeVisibleShape(int originalAxis, int originalSize, int viewportSize, int& outStartAxis, int& outEndAxis);
    void renderScreen();
};
#endif