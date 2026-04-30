#ifndef NONEPRESSEDEVENT_H
#define NONEPRESSEDEVENT_H

#include "../../interfaces/IPressedStrategy/IPressedStrategy.h"

class NonePressedEvent : public IPressedStrategy {
private:
    CursorContext* cursorContext;
public:
    NonePressedEvent();

    void onPressed(int x, int y, ToolRuntimeContext toolRuntimeContext) override;
    void onTracking(int x, int y) override;
    void onRelease(int x, int y) override;
    CursorContext* getCursorContext() override;
};
#endif