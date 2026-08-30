#ifndef DRAWINGSESSION_H
#define DRAWINGSESSION_H

#include "../../context/SymmetryContext/SymmetryContext.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/DirtyManager/DirtyManager.h"
#include "../../objects/layer/Layers.h"
#include <array>

class DrawingSession {
    private:
    Preview* _preview = nullptr;
    DirtyManager* _dirtyManager = nullptr;
    SymmetryContext* _symmetryContext = nullptr;

    public:
    DrawingSession(Preview* preview, DirtyManager* dirtyManager, SymmetryContext* symmetryContext);
    void begin(Layer* layer);
    void commit();
    void clear();
    void blendMirroredPixel(int x, int y, unsigned int color, SymmetryContext* symmetryContext);
    void putMirroredPixel(int x, int y, unsigned int color, SymmetryContext* symmetryContext);
    void blendPixel(int x, int y, unsigned int color);
    void putPixel(int x, int y, unsigned int color);
};
#endif