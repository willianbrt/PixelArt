#ifndef IPRESSEDSTRATEGY_H
#define IPRESSEDSTRATEGY_H

#include "../../context/BrushContext/BrushContext.h"

struct HoverPreview {
    Pattern* pattern;
    float scale = 1.0f;
    bool enable = true;
};

class IToolStrategy{
protected:
public:
    virtual ~IToolStrategy() = default;
    virtual HoverPreview* getHoverPreview() = 0;
};

class IPressedStrategy : public IToolStrategy {
protected:
public:
    virtual ~IPressedStrategy() = default;
    virtual void onPressed(int x, int y) = 0;
    virtual void onTracking(int x, int y) = 0;
    virtual void onRelease(int x, int y) = 0;
};
#endif
