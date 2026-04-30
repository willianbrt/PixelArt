#ifndef IPRESSEDSTRATEGY_H
#define IPRESSEDSTRATEGY_H

#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"
#include "../../context/CursorContext/CursorContext.h"

class IToolStrategy{
protected:
public:
    virtual ~IToolStrategy() = default;
    virtual CursorContext* getCursorContext() = 0;
};

class IPressedStrategy : public IToolStrategy {
protected:
public:
    virtual ~IPressedStrategy() = default;
    virtual void onPressed(int x, int y, ToolRuntimeContext toolRuntimeContext) = 0;
    virtual void onTracking(int x, int y) = 0;
    virtual void onRelease(int x, int y) = 0;
};
#endif
