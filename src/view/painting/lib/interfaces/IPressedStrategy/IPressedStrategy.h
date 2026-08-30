#ifndef IPRESSEDSTRATEGY_H
#define IPRESSEDSTRATEGY_H

#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"
#include "../../context/CursorContext/CursorContext.h"
#include "../../context/BrushContext/BrushContext.h"

class IToolStrategy{
protected:
public:
    virtual ~IToolStrategy() = default;
    virtual void done() = 0;
    virtual void abort() = 0;
    virtual bool isInitialized() = 0;
    virtual CursorContext* getCursorContext() = 0;

};

class IDraw{
public:
    virtual void plot(const int& x, const int& y) = 0;
};

class IPressedStrategy : public IToolStrategy {
protected:
public:
    virtual ~IPressedStrategy() = default;
    virtual void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) = 0;
    virtual void onTracking(int x, int y) = 0;
    virtual void onRelease() = 0;
};
#endif
