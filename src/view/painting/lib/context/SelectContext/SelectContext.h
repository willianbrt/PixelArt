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
    Surface* data = nullptr;
    Bounding srcArea;
    Transformation transformation;
    SelectionBox selectionBox;
    ResizeHandle handle[4];
    RotateHandle rotateHandle[4];
        
    std::array<float, 8> getAllHandle(Viewport* viewport);
    std::array<float, 8> getAllRotateHandle(Viewport* viewport);
};

#endif