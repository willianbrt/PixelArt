#include <emscripten.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

unsigned int* _buffer;
unsigned int _width;
unsigned int _height;
unsigned long _colorHEX;
unsigned int* tempBuffer;

extern "C"{

unsigned long convertABGRtoRGBA(unsigned int color) {
    unsigned char alpha = (color >> 24) & 0xFF;
    unsigned char blue = (color >> 16) & 0xFF;
    unsigned char green = (color >> 8) & 0xFF;
    unsigned char red = color & 0xFF;
    return (red << 24) | (green << 16) | (blue << 8) | alpha;
}

EMSCRIPTEN_KEEPALIVE;
unsigned int calculateIndex(unsigned int x, unsigned int y, unsigned int width) {
    return y * width + x;
}

EMSCRIPTEN_KEEPALIVE;
void putPixel(int x, int y, unsigned int colorHEX) {
    _buffer[calculateIndex(x, y, _width)] = convertABGRtoRGBA(colorHEX);
}

EMSCRIPTEN_KEEPALIVE;
unsigned int* render(unsigned int* buffer, int viewportWidth, int viewportHeight, int sketchPositionX, int sketchPositionY, int scale) {
    if (!buffer || scale <= 0 || viewportWidth <= 0 || viewportHeight <= 0) return NULL;

    unsigned int resizedWidth = _width * scale;
    unsigned int resizedHeight = _height * scale;
    
    if(sketchPositionX > viewportWidth || sketchPositionX < static_cast<long>(-resizedWidth)) return NULL;
    if(sketchPositionY > viewportHeight || sketchPositionY < static_cast<long>(-resizedHeight)) return NULL;


    long maxPositionX = viewportWidth - resizedWidth;
    long maxPositionY = viewportHeight - resizedHeight;

    int startVisibleX = (sketchPositionX < 0) ? -sketchPositionX : 0;
    int endVisibleX =  (sketchPositionX < maxPositionX) ? resizedWidth : resizedWidth - (sketchPositionX - maxPositionX);

    int startVisibleY = (sketchPositionY < 0) ? -sketchPositionY : 0;
    int endVisibleY = (sketchPositionY < maxPositionY) ? resizedHeight : resizedHeight - (sketchPositionY - maxPositionY);

    int visibleWidth = endVisibleX - startVisibleX;
    int visibleHeight = endVisibleY - startVisibleY;

    tempBuffer = (unsigned int*)malloc(visibleWidth * visibleHeight * sizeof(unsigned int));
    
    if (!tempBuffer){ 
        free(tempBuffer);
        return NULL;
    }
    
    for (long x = startVisibleX; x < endVisibleX; x++) {
        const unsigned int originalX = floor(x / scale);

        for (long y = startVisibleY; y < endVisibleY; y++) {
            const unsigned originalY = floor(y / scale);

            const unsigned int originalIndex = calculateIndex(originalX, originalY, _width);
            const unsigned int resizedIndex = calculateIndex((x - startVisibleX), (y - startVisibleY), visibleWidth);

            tempBuffer[resizedIndex] = buffer[originalIndex];
        }
    }
    
    return tempBuffer;
}

EMSCRIPTEN_KEEPALIVE;
void freeBuffer(unsigned int* buffer) {
    free(buffer);
}


EMSCRIPTEN_KEEPALIVE;
unsigned int* build(int width, int height, unsigned int colorHEX) {
    _buffer = (unsigned int*)malloc(height*width* sizeof(unsigned int));
    if (!_buffer) return NULL; 

    _width = width;
    _height = height;
    _colorHEX = convertABGRtoRGBA(colorHEX);


    _buffer[0] = 0xff0000ff;
    for (long x = 1; x < _width* _height-1; x++) {
        _buffer[x] = _colorHEX;
    }
    _buffer[_width* _height-1] = 0xff0000ff;

    return _buffer;
}

}