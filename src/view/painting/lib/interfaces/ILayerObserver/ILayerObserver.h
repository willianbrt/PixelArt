#ifndef ILAYEROBSERVER_H
#define ILAYEROBSERVER_H

class ILayerObserver{
public:
    virtual void onIsVisibleLayer() = 0;
    virtual void onIsLockLayer() = 0;
    virtual void onOpacityLayer() = 0;
    virtual void onRenameLayer() = 0;
};

#endif