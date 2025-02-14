#ifndef BUCKET 
#define BUCKET

#include "../interfaces/IGraphic.cpp"

#include <vector>

// class Bucket : IGraphic {
// bool isDrawing = true;

// public:
//     Bucket(){

//     }
//     void resize(int width, int height){}
//     void move(int x, int y){}

//     bool draw(Layers& layer){
//         const unsigned int startColorHEX = layer.getPixel(x, y);

//         if(startColorHEX == newColorHex) return modifiedPixels;

//         vector<Point> queue;
//         queue.push_back(Point(x, y));

//         bounds.end = Point(x, y);
//         bounds.start = Point(x, y);
        
//         while(queue.size() != 0) {
//             Point point = queue.back();
//             queue.pop_back();
//             const unsigned int currentColorHEX = getPixel(point.x, point.y);

//             const bool isItOutsideTheAxisX = point.x < 0 || point.x > _width;
//             const bool isItOutsideTheAxisY =  point.y < 0 || point.y > _height;

//             if(isItOutsideTheAxisX || isItOutsideTheAxisY || currentColorHEX != startColorHEX)
//                 continue;

//             modifyPixel(layer, point, newColorHex);
            
//             queue.push_back(Point(point.x+1, point.y));
//             queue.push_back(Point(point.x-1, point.y);
//             queue.push_back(Point(point.x,   point.y+1));
//             queue.push_back(Point(point.x,   point.y-1));
//         }
//     }
// };
#endif