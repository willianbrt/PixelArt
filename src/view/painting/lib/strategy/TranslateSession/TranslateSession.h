#ifndef TRANSLATESESSION_H
#define TRANSLATESESSION_H

#include "../../context/ToolRuntimeContext/ToolRuntimeContext.h"
#include "../../context/SelectContext/SelectContext.h"
#include "../../graphics/Pixel/Pixel.h"

class TranslateSession {
    private:
    ToolRuntimeContext _toolRuntimeContext;
    SelectContext* _selectionContext;
    Point _startPoint;

    public:
    TranslateSession(SelectContext* selection);
    bool begin(Point point, const ToolRuntimeContext& toolRuntimeContext);
    void update(const Point& mouse);
};
#endif