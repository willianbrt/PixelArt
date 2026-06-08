#ifndef SELECTCONTEXT_H
#define SELECTCONTEXT_H

#include "../../graphics/surface/Surface.h"
#include "../../objects/ResizeHandle/ResizeHandle.h"
#include "../../objects/RotateHandle/RotateHandle.h"
#include "../../graphics/Transformation/Transformation.h"
#include "../../graphics/SelectionBox/SelectionBox.h"
#include <array>

struct SelectContext {
private:

public:
    SelectContext();
    Surface* data = nullptr;
    Bounding srcArea;
    Transformation transformation;
    SelectionBox selectionBox;
    ResizeHandle handle[4];
    RotateHandle rotateHandle[4];
    bool enabled = false;
        
    std::array<float, 8> getAllHandle(CanvasSettings* viewport);
    std::array<float, 8> getAllRotateHandle(CanvasSettings* viewport);
};

#endif