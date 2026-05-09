#ifndef NONEPRESSEDEVENT_H
#define NONEPRESSEDEVENT_H

#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"

class NonePressedEvent : public IPressedStrategy {
private:
    CursorContext* cursorContext;
    ToolRuntimeContext _toolRuntimeContext;
public:
    NonePressedEvent();

    void onPressed(int x, int y, const ToolRuntimeContext& toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease() override;
    CursorContext* getCursorContext() override;
    void done() override;
    void abort() override;
};
#endif