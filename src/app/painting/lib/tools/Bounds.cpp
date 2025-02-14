#ifndef BOUNDS
#define BOUNDS

#include "../helpers/pixel.cpp"

class Bounds{
    Point start;
    Point end;

public:
    Bounds(Point start, Point end){
        if(start.x < end.x){
            start.x = start.x;
            end.x = end.x;
        }else{
            start.x = end.x;
            end.x = start.x;
        }
        
        if(start.y < end.y){
            start.y = start.y;
            end.y = end.y;
        }else{
            start.y = end.y;
            end.y = start.y;
        }
    }

    Point getBoundStart(){
        return start;
    }

    Point getBoundEnd(){
        return end;
    }

    void updateBounds(Point point){
        if(start.x < point.x) start.x = point.x;
        else if(end.x > point.x) end.x = point.x;
        
        if(start.y < point.y) start.y = point.y;
        else if(end.y > point.y) end.y = point.y;
    }
};
#endif