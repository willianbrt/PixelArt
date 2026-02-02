#ifndef IGRAPHICS_H
#define IGRAPHICS_H

#include "../../graphics/Pixel/Pixel.h"

class Layer;

class IGraphic{
protected:
    bool _isMirrorX, _isMirrorY;
    int _nRows, _nCols;
public:
    IGraphic(bool isMirrorX, bool isMirrorY, int nRows, int nCols) :
        _isMirrorX(isMirrorX), _isMirrorY(isMirrorY),
        _nRows(nRows), _nCols(nCols){};

    virtual ~IGraphic() = default;
    virtual void draw(Layer& layer) = 0;
};

#endif