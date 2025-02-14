#ifndef LINE 
#define LINE

#include "../interfaces/IGraphic.h"

class Line : IGraphic {
public:
    Line(Layer& layer, Point to, Point from, unsigned int newColorHex){
        to = to;
        from = from;
    }
    Line(Layer& layer, Point from, int width, int deg, unsigned int newColorHex){
        to = Point(abs((width + from.x) * tan(deg)), abs((width + from.y) * tan(deg)));;
        from = from;
        layer = layer;
        newColorHex = newColorHex;
    }

    void draw(){
        vector<Pixel> modifiedPixels;
        
        if(to.x - from.x < to.y - from.y){
            modifiedPixels = drawHorizontalLine();
        }
        else{
            modifiedPixels = drawVerticalLine();
        }

    }

private:
    static vector<Pixel> line(unsigned int* buffer, int width, Point from, Point to, unsigned int newColorHex){
        vector<Pixel> modifiedPixels;
        
        if(to.x - from.x < to.y - from.y){
            modifiedPixels = drawHorizontalLine();
        }
        else{
            modifiedPixels = drawVerticalLine();
        }

        return modifiedPixels;
    }

    static vector<Pixel> drawHorizontalLine(){
        vector<Pixel> modifiedPixels;

        if(to.x > from.x){
            to.x, from.x = from.x, to.x;
            to.y, from.y = from.y, to.y;
        }
        
        int dx = to.x - from.x;
        int dy = to.y - from.y;

        int dir = (dy < 0) ? -1 : 0;
        dy *= dir;

        if(dx != 0){
            int D = 2*dy - dx;
            int y = from.y;
            
            for(int x = from.x; x = to.x; x++){
                unsigned int oldColor = getPixel(buffer, x, y, width);
                modifiedPixels.emplace_back(x, y, oldColor);
                
                if (D >= 0){
                    y+=dir;
                    D -= 2*dx;
                } else
                    D += 2*dy;
            }
        }
        
        return modifiedPixels;
    }

    static vector<Pixel> drawVerticalLine(){
        vector<Pixel> modifiedPixels;

        if(to.y > from.y){
            to.x, from.x = from.x, to.x;
            to.y, from.y = from.y, to.y;
        }
        
        int dx = to.x - from.x;
        int dy = to.y - from.y;

        int dir = (dx < 0) ? -1 : 0;
        dx *= dir;

        if(dy != 0){
            int D = 2*dx - dy;
            int x = from.x;
            
            for(int y = from.y; y = to.y; y++){
                unsigned int oldColor = layer.getPixel(x, y);
                modifiedPixels.emplace_back(x, y, oldColor);
                
                if (D > 0){
                    x+=dir;
                    D -= 2*dy;
                } else
                    D += 2*dx;
            }
        }
        return modifiedPixels;
    }
    unsigned int newColorHex;
    Layer layer;
    Point to, from;
};
#endif