#ifndef PIXEL
#define PIXEL

#include <iostream>

#define IS_LITTLE_ENDIAN() (*(uint16_t*)"\x01\x00" == 0x01);
#if IS_LITTLE_ENDIAN
#define ENDIANNESS "LITTLE_ENDIAN"
#define swap_endian_uint32(data)            \
   (*data) = ((*data) & 0xFF) << 24 |       \
             ((*data) >> 8 & 0xFF) << 16 |  \
             ((*data) >> 16 & 0xFF) << 8 |  \
             ((*data) >> 24 & 0xFF);        \

#define RED(color) (color & 0xFF) << 24;
#define GREEN(color) (color >> 8 & 0xFF) << 16;
#define BLUE(color) (color >> 16 & 0xFF) << 8; 
#define ALPHA(color) (color >> 24 & 0xFF);
#else
#define ENDIANNESS "BIG_ENDIAN"
#define swap_endian_uint32(data)            \
   (*data) = ((*data) & 0xFF) << 24 |       \
             ((*data) >> 8 & 0xFF) << 16 |  \
             ((*data) >> 16 & 0xFF) << 8 |  \
             ((*data) >> 24 & 0xFF);        \

#define RED(color) (color & 0xFF) << 24;
#define GREEN(color) (color >> 8 & 0xFF) << 16;
#define BLUE(color) (color >> 16 & 0xFF) << 8; 
#define ALPHA(color) (color >> 24 & 0xFF);
#endif


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

    // Color(unsigned int colorHEX){
    //     hex = red << 24 | green << 16 | blue << 8 | alpha;
    // }
};

struct Point{
    int x;
    int y;
};

struct Pixel{
    Point point;
    Color color;
};
#endif