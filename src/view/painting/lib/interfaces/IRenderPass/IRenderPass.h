#ifndef IRENDERPASS_H
#define IRENDERPASS_H

#include "../../graphics/Pixel/Pixel.h"
class IRenderPass {
private:
    
public:
virtual ~IRenderPass() = default;
virtual void render() = 0;
};
#endif