#ifndef BRUSHCONTEXT_H
#define BRUSHCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"
#include "../../interfaces/ISurface/ISurface.h"
#include "../../interfaces/IBrushTip/IBrushTip.h"
#include "../../graphics/Transformation/Transformation.h"
#include <unordered_map>
#include <vector>
#include <string>

#include "../../rasterize/LineRasterize/LineRasterize.h"


enum FILL_MODE{
    FILL = 0,
    MASK = 1,
    STAMP = 2
};

enum ALIGN_MODE{
    BRUSHING = 0,
    LOCAL = 1,
    SOURCE = 2
};
enum BRUSH_TIP{
    CUSTOM = 0,
    SQUARE = 1,
    CIRCLE = 2,
    LINE = 3
};

class BrushContext {
private:
    bool isDirtyTip = false;
    bool isDirtyPattern = false;
    IBrushTip* brushTip;
    IBrushTip* brushPattern;
    int _size = 1;

    Surface* _shape = nullptr;
    Surface* _pattern = nullptr;
    FILL_MODE _fillMode = FILL_MODE::FILL;
    ALIGN_MODE _alignMode = ALIGN_MODE::BRUSHING;
public:
    BrushContext();


    unsigned int color = 0x000000FF;
    float hardness = 1.0f;

    Transformation transformation;
    void setSize(int size);
    int getSize();
    Surface* getShape();
    void setShape(IBrushTip* tip);
    Surface* getPattern();
    void setPattern(IBrushTip* tip);
    void setFillMode(FILL_MODE fillMode);
    FILL_MODE getFillMode();
    void setAlignMode(ALIGN_MODE alignMode);
    ALIGN_MODE getAlignMode();
};

#endif
