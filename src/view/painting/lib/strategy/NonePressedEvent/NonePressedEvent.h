#ifndef NONEPRESSEDEVENT_H
#define NONEPRESSEDEVENT_H

#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"

class NonePressedEvent : public IPressedStrategy {
private:
    ToolRuntimeContext _toolRuntimeContext;
public:
    NonePressedEvent();

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;
    void done() override;
    void abort() override;
    CursorContext* getCursorContext() override;
};
#endif