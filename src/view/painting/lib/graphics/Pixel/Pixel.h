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

struct PointF{
    float x = 0;
    float y = 0;
    PointF(float x, float y) : x(x), y(y){}
    PointF(){}
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
    Corners(Bounding bounding){
        topLeft     = Point(bounding.start.x, bounding.start.y);
        bottomRight = Point(bounding.end.x, bounding.end.y);
        topRight    = Point(bounding.end.x, bounding.start.y);
        bottomLeft  = Point(bounding.start.x, bounding.end.y);
    }
    Corners(Point top_left, Point bottom_left, Point top_right, Point bottom_right){
        topLeft = top_left;
        bottomLeft = bottom_left;
        topRight = top_right;
        bottomRight = bottom_right;
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

    float cross(Point p1, Point p2, Point p3){
        return (p1.x - p2.x) * (p3.y - p2.y) - (p1.y - p2.y) * (p3.x - p2.x);
    }
    bool isInsideRotatedBounding(Point point){
        bool b1 = cross(point, topLeft, topRight) > 0;
        bool b2 = cross(point, topRight, bottomRight) > 0;
        bool b3 = cross(point, bottomRight, bottomLeft) > 0;
        bool b4 = cross(point, bottomLeft, topLeft) > 0;

        return (b1 && b2 && b3 && b4) || (!b1 && !b2 && !b3 && !b4);
    }
};

#endif