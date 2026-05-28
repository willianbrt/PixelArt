#ifndef IRENDERPASS_H
#define IRENDERPASS_H

#include "../../graphics/Pixel/Pixel.h"
class IRenderPass {
private:
    
public:
virtual ~IRenderPass() = default;

virtual void init() = 0;
virtual void upload(Bounding area) = 0;
virtual void draw() = 0;
};
#endif