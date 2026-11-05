#ifndef DRAWINGSESSION_H
#define DRAWINGSESSION_H

#include "../../context/SymmetryContext/SymmetryContext.h"
#include "../../graphics/Pixel/Pixel.h"
#include "../../objects/preview/Preview.h"
#include "../../objects/DirtyManager/DirtyManager.h"
#include "../../objects/layer/Layers.h"
#include "../../objects/CanvasSettings/CanvasSettings.h"
#include <array>
#include <memory>

enum PIXEL_WRITER{
    OVERWRITER = 0,
    MASCARA = 1,
    ERASER = 2,
    ALPHA_BLEND = 3,
    PRESERVE_ALPHA = 4,
    NORMAL = 5
};


class IPixelWriter{
public:
    IPixelWriter() = default;
    ~IPixelWriter() = default;
    virtual void     plot(Preview* drawing, int& x, int& y, unsigned int& color) = 0;
};


class DrawingSession {
    private:
    Preview* _preview = nullptr;
    DirtyManager* _dirtyManager = nullptr;
    SymmetryContext* _symmetryContext = nullptr;
    CanvasSettings* _canvasSettings = nullptr;

    public:
    DrawingSession(Preview* preview, DirtyManager* dirtyManager, SymmetryContext* symmetryContext, CanvasSettings* canvasSettings);
    void begin(Layer* layer);
    void commit();
    void clear();

    void putPixel(int x, int y, unsigned int color, IPixelWriter* pixelWriter, ISurface* mask = nullptr);

    unsigned int getModifiedPixel(int x, int y);
    unsigned int getOriginalPixel(int x, int y);
    
    Point tiling(int& x, int& y);
    void symmetry(int& x, int& y, unsigned int& color, IPixelWriter* pixelWriter);

};
class Eraser : public IPixelWriter{
private:
public:
    Eraser();
    void plot(Preview* drawing, int& x, int& y, unsigned int& color) override;
};

class PreserveAlpha : public IPixelWriter{
private:
public:
    PreserveAlpha();
    void plot(Preview* drawing, int& x, int& y, unsigned int& color) override;
};

class Mask : public IPixelWriter{
private:
public:
    Mask();
    void plot(Preview* drawing, int& x, int& y, unsigned int& color) override;
};
class Normal : public IPixelWriter{
private:
public:
    Normal();
    void plot(Preview* drawing, int& x, int& y, unsigned int& color) override;
};

class WriteMode {
public:
    static Eraser eraser;
    static Normal normal;
    static PreserveAlpha alpha;
    static Mask mask;
};

#endif