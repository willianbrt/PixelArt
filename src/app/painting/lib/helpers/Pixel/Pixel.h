#ifndef PIXEL
#define PIXEL

#include <iostream>

#define IS_LITTLE_ENDIAN() (*(uint16_t*)"\x01\x00" == 0x01)
#if IS_LITTLE_ENDIAN
// #define ENDIANNESS "LITTLE_ENDIAN"
#define swap_endian_uint32(data)            \
   (*data) = ((*data) & 0xFF) << 24 |       \
             ((*data) >> 8 & 0xFF) << 16 |  \
             ((*data) >> 16 & 0xFF) << 8 |  \
             ((*data) >> 24 & 0xFF)        \

#define RED(color) ((color & 0xFF) << 24)
#define GREEN(color) ((color >> 8 & 0xFF) << 16)
#define BLUE(color) ((color >> 16 & 0xFF) << 8) 
#define ALPHA(color) (color >> 24 & 0xFF)
#define RGBA(r,g,b,a) (r << 24 | g << 16 | b << 8 | a)
#else
#define ENDIANNESS "BIG_ENDIAN"
#define swap_endian_uint32(data)            \
   (*data) = ((*data) & 0xFF) << 24 |       \
             ((*data) >> 8 & 0xFF) << 16 |  \
             ((*data) >> 16 & 0xFF) << 8 |  \
             ((*data) >> 24 & 0xFF)        \

#define RED(color) ((color & 0xFF) << 24)
#define GREEN(color) ((color >> 8 & 0xFF) << 16)
#define BLUE(color) ((color >> 16 & 0xFF) << 8) 
#define ALPHA(color) (color >> 24 & 0xFF)
#define RGBA(r,g,b,a) (a << 24 | b << 16 | g << 8 | r)
#endif


struct Point{
    int x = 0;
    int y = 0;
    Point(int x,int y) : x(x), y(y){}
    Point(){}
};

struct Pixel{
    Point point;
    unsigned int colorHEX;
    Pixel(Point point, unsigned int colorHex) : point(point), colorHEX(colorHex){}
};

struct HistoryPixel{
    unsigned int _oldColor = 0;
    unsigned int _newColor = 0;
    unsigned int _x = 0;
    unsigned int _y = 0;
    HistoryPixel(unsigned int oldColor, unsigned int newColor, unsigned int x, unsigned int y){
        _oldColor = oldColor;
        _newColor = newColor;
        _x = x;
        _y = y;
    }
};
#endif