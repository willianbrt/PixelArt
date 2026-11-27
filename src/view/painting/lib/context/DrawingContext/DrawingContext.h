#ifndef DRAWINGCONTEXT_H
#define DRAWINGCONTEXT_H

#include "../../interfaces/IToolContext/IToolContext.h"
#include "../../interfaces/ISurface/ISurface.h"
#include "../../interfaces/IBrushTip/IBrushTip.h"
#include "../../graphics/Transformation/Transformation.h"
#include "../../rasterize/NoneTip/NoneTip.h"
#include <unordered_map>
#include <vector>
#include <string>

#include "../../rasterize/LineRasterize/LineRasterize.h"


enum FILL_MODE{
    FILL = 0,
    PRESERVE_ALPHA = 1,
    STAMP = 2
};

enum ALIGN_MODE{
    BRUSHING = 0,
    LOCAL = 1,
    SOURCE = 2
};


class DrawingContext {
public:
    Color color;
    float hardness = 1.0f;

    float stabilizer = 0.0f;
    int spacing = 0;
    bool pixelPerfect = false;
    bool enablePattern = false;
    bool isSolidColor = true;

    IBrushTip* shape = nullptr;
    IBrushTip* pattern = nullptr;
    FILL_MODE fillMode = FILL_MODE::FILL;
    ALIGN_MODE alignMode = ALIGN_MODE::BRUSHING;

};

#endif
