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
    bool operator!=(const Point& point) const{
        return point.x != x || point.y != y;
    }
    bool operator==(const Point& point) const{
        return point.x == x && point.y == y;
    }
    
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
    Point start;
    Point end;
    Bounding(){
       start.x = INT_MAX;
       start.y = INT_MAX;
       end.x = INT_MIN;
       end.y = INT_MIN;
    }
    Bounding(Point s, Point e) {
        start.x = std::min(s.x, e.x);
        start.y = std::min(s.y, e.y);
        end.x   = std::max(s.x, e.x);
        end.y   = std::max(s.y, e.y);
    }
    bool operator!=(const Bounding& area) const{
        return area.start.x != start.x || area.start.y != start.y || area.end.x != end.x || area.end.y != end.y;
    }
    bool operator==(const Bounding& area) const{
        return area.start.x == start.x && area.start.y == start.y && area.end.x == end.x && area.end.y == end.y;
    }
    int getWidth() const { return end.x - start.x + 1; }
    int getHeight() const { return end.y - start.y + 1; }
    PointF getCenter()  const {
        return {
            (start.x + end.x) * 0.5f,
            (start.y + end.y) * 0.5f
        };
    }
};


#endif