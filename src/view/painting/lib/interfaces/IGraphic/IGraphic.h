#ifndef IGRAPHICS_H
#define IGRAPHICS_H

// #include "../../objects/layer/Layers.h"

class Layer;

class IGraphic{
public:
    virtual ~IGraphic() = default;
    virtual void draw(Layer& layer) = 0;
};

#endif