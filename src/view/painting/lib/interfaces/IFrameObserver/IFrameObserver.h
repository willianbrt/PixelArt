#ifndef IFRAMEOBSERVER_H
#define IFRAMEOBSERVER_H

#include "../../objects/layer/Layers.h"

class IFrameObserver{
public:
    virtual void onChangeActiveLayer(Guid id) = 0;
    virtual void onAddLayer(Layer* layer, size_t index) = 0;
    virtual void onRemoveLayer(Guid id) = 0;
    virtual void onMoveLayerTo(Guid id, int index) = 0;
};

#endif