#ifndef PIXEL
#define PIXEL

#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
// __builtin_bswap32();

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
    Point(int x, int y) : x(x), y(y){}
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

struct Bounding{
    Point start = Point(0,0);
    Point end = Point(0,0);
    Bounding(){}
    Bounding(Point s, Point e) {
        start.x = std::min(s.x, e.x);
        start.y = std::min(s.y, e.y);
        end.x   = std::max(s.x, e.x);
        end.y   = std::max(s.y, e.y);
    }
    int getWidth(){ return end.x - start.x; }
    int getHeight(){ return end.y - start.y; }
};

struct Corners{
public:
    Point topLeft;
    Point bottomLeft;
    Point topRight;
    Point bottomRight;

    Corners(){
    }
    Corners(Point top_left, Point bottom_left, Point top_right, Point bottom_right){
        topLeft = top_left;
        bottomLeft = bottom_left;
        topRight = top_right;
        bottomRight = bottom_right;        

        // if (topLeft.x > topRight.x) {
        //     std::swap(topLeft, topRight);
        // }

        // if (bottomLeft.x > bottomRight.x) {
        //     std::swap(bottomLeft, bottomRight);
        // }

        // if (topLeft.y > bottomLeft.y) {
        //     std::swap(topLeft, bottomLeft);
        // }

        // if (topRight.y > bottomRight.y) {
        //     std::swap(topRight, bottomRight);
        // }

    //     if (topLeft.y > bottomLeft.y) std::swap(topLeft, bottomLeft);
    //     if (topRight.y > bottomRight.y) std::swap(topRight, bottomRight);

    //     if (topLeft.x > topRight.x) std::swap(topLeft, topRight);
    //     if (bottomLeft.x > bottomRight.x) std::swap(bottomLeft, bottomRight);
    // :
    //     if (topLeft.y > topRight.y && bottomLeft.y > bottomRight.y) {
    //         std::swap(topLeft, topRight);
    //         std::swap(bottomLeft, bottomRight);
    //     }

    //     if (topLeft.y > bottomLeft.y || topRight.y > bottomRight.y) {
    //         std::swap(topLeft, bottomLeft);
    //         std::swap(topRight, bottomRight);
    //     }
    }
    Bounding getBounding(){
        int startX = std::min({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x});
        int endX = std::max({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x});
        int startY = std::min({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y});
        int endY = std::max({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y});
        
        Point start = Point(startX, startY);
        Point end = Point(endX, endY);
        // Point start = Point(topLeft.x, topLeft.y);
        // Point end = Point(bottomRight.x, bottomRight.y);
        return Bounding(start, end);
    }
};

#endif