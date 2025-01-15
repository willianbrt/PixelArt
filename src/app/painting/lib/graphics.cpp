#include <emscripten.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

unsigned int* _buffer;
unsigned int _width;
unsigned int _height;
unsigned int* tempBuffer;


struct Color{
public:
    unsigned char red = 0;
    unsigned char green = 0;
    unsigned char blue = 0;
    unsigned char alpha = 0;

    Color(unsigned int colorHEX){
        red = colorHEX >> 24 & 0xFF;
        green = colorHEX >> 16 & 0xFF;
        blue = colorHEX >> 8 & 0xFF;
        alpha = colorHEX & 0xFF;
    }
};

void swap_endian_uint32(unsigned int& data) {
    data = (data & 0xFF) << 24 | 
           (data >> 8 & 0xFF) << 16 | 
           (data >> 16 & 0xFF) << 8 | 
           (data >> 24 & 0xFF);
}

extern "C"{

EMSCRIPTEN_KEEPALIVE;
unsigned int* build(unsigned int width, unsigned int height) {
    _buffer = (unsigned int*)malloc(height*width * sizeof(unsigned int));

    _width = width;
    _height = height;

    return _buffer;
}

EMSCRIPTEN_KEEPALIVE;
unsigned int calculateIndex(unsigned int x, unsigned int y, unsigned int width) {
    return y * width + x;
}

EMSCRIPTEN_KEEPALIVE;
void putPixel(unsigned int x, unsigned int y, unsigned int colorHEX) {
    swap_endian_uint32(colorHEX);
    _buffer[calculateIndex(x, y, _width)] = colorHEX;
}

EMSCRIPTEN_KEEPALIVE;
unsigned int getPixel(unsigned int x, unsigned int y) {    
    unsigned int colorHEX = _buffer[calculateIndex(x, y, _width)];
    swap_endian_uint32(colorHEX);
    return colorHEX;
}

EMSCRIPTEN_KEEPALIVE;
unsigned int blending(unsigned int topColor, unsigned int bottomColor){
    const Color dst = Color(bottomColor);
    const Color src = Color(topColor);

    const float factorAlphaSrc = src.alpha / 255.0f;

    const unsigned char outRed   = factorAlphaSrc*src.red   + (1.0f - factorAlphaSrc)*dst.red;
    const unsigned char outGreen = factorAlphaSrc*src.green + (1.0f - factorAlphaSrc)*dst.green;
    const unsigned char outBlue  = factorAlphaSrc*src.blue  + (1.0f - factorAlphaSrc)*dst.blue;
    const unsigned char outAlpha = factorAlphaSrc*src.alpha + (1.0f - factorAlphaSrc)*dst.alpha;
    
    const unsigned int outColor = outRed << 24 | outGreen << 16 | outBlue << 8 | outAlpha;

    return outColor;
}

EMSCRIPTEN_KEEPALIVE;
unsigned int* render(unsigned int* originalBuffer, unsigned int startVisibleX, unsigned int endVisibleX, unsigned int startVisibleY, unsigned int endVisibleY, unsigned int scale){
    int visibleWidth = endVisibleX - startVisibleX;
    int visibleHeight = endVisibleY - startVisibleY;

    tempBuffer = (unsigned int*)malloc(visibleWidth * visibleHeight * sizeof(unsigned int));

    if (!tempBuffer){
        free(tempBuffer);
        return NULL;
    }
    
    int yIncrement = startVisibleY % scale;
    int startYIncrement = startVisibleY % scale;
    
    int startXIncrement = startVisibleX % scale;
    
    int startOriginalY = (startVisibleY - yIncrement) / scale;
    int startOriginalX = (startVisibleX - startXIncrement) / scale;

    int startOriginalIndex = (startOriginalY * _width + startOriginalX);

    int resizedIndex = 0;

    for (int y = 0; y < visibleHeight; y++) {
        int originalIndex = startOriginalIndex;
        int xIncrement = startXIncrement;
        
        for (int x = 0; x < visibleWidth; x++) {
            tempBuffer[resizedIndex] = _buffer[originalIndex];
            
            resizedIndex++;
            
            xIncrement++;
            if(xIncrement == scale){
                xIncrement = 0;
                originalIndex++;
            }
        }
        
        yIncrement++;
        if(yIncrement == scale){
            yIncrement = 0;
            startOriginalIndex += _width;
        }
    }

    return tempBuffer;
}


EMSCRIPTEN_KEEPALIVE;
void freeBuffer(unsigned int* buffer) {
    free(buffer);
}

}