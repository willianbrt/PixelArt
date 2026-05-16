#ifndef SELECTCONTEXT_H
#define SELECTCONTEXT_H

// #include "../../interfaces/IToolContext/IToolContext.h"
#include "../../graphics/surface/Surface.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../objects/ResizeHandle/ResizeHandle.h"
#include "../../objects/RotateHandle/RotateHandle.h"
#include "../../graphics/Transformation/Transformation.h"
#include "../../graphics/SelectionBox/SelectionBox.h"
#include <array>

struct SelectContext {
private:

public:
    SelectContext();
    Surface* data;
    Bounding srcArea;
    Transformation transformation;
    SelectionBox selectionBox  = SelectionBox();
    ResizeHandle handle[4] = {
        ResizeHandle(&selectionBox, ENUM_MARKER::TOP_LEFT),
        ResizeHandle(&selectionBox, ENUM_MARKER::TOP_RIGHT),
        ResizeHandle(&selectionBox, ENUM_MARKER::BOTTOM_RIGHT),
        ResizeHandle(&selectionBox, ENUM_MARKER::BOTTOM_LEFT)
    };
    RotateHandle rotateHandle[4] = {
        RotateHandle(&selectionBox, ENUM_MARKER::TOP_LEFT),
        RotateHandle(&selectionBox, ENUM_MARKER::TOP_RIGHT),
        RotateHandle(&selectionBox, ENUM_MARKER::BOTTOM_RIGHT),
        RotateHandle(&selectionBox, ENUM_MARKER::BOTTOM_LEFT)
    };
        
    std::array<float, 8> getAllHandle(Viewport* viewport);
    std::array<float, 8> getAllRotateHandle(Viewport* viewport);
};

#endif