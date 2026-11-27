#include "DrawTool.h"

DrawTool::DrawTool(DrawingContext* drawingContext, ToolRuntimeContext* toolRuntimeContext) :
_drawingContext(drawingContext), _toolRuntimeContext(toolRuntimeContext)
{}
// void DrawTool::process(const Color& base){
//     Color color;

//     if(_drawingContext->fillMode == FILL_MODE::PRESERVE_ALPHA){// preserve alpha
//         color = _drawingContext->color;
//         color.alpha *= base.alpha;
//     } 
//     if(_drawingContext->fillMode ==  FILL_MODE::STAMP){ // stamp
//         color = _drawingContext->shape->getType() == TYPE_BRUSH_TIP::IMAGE ? base : _drawingContext->color;
//     }
//     if(_drawingContext->fillMode == FILL_MODE::FILL){ // stamp
//         color = _drawingContext->color;
//     }

//     color.alpha *= _drawingContext->hardness;
// }
void DrawTool::shape(const int& x, const int& y){
    ISurface* shape = _drawingContext->shape->getMask();

    StampRasterize stamp({(float)x + 0.5f, (float)y + 0.5f},
        {shape->getWidth(), shape->getHeight()},
        _toolRuntimeContext->drawingAreaSize,
        _drawingContext->shape->getTransformation());

    while(stamp.hasNext()){
        Point it = stamp.next();
        Point src = stamp.getSrcPoint();

        if(src.x < 0 || src.y < 0 || src.x >= shape->getWidth() || src.y >= shape->getHeight()) continue;
        
        Color shapeColor = shape->getPixel(src.x, src.y);
        if(shapeColor.alpha == 0.0f) continue;
        
        if(_drawingContext->alignMode == ALIGN_MODE::BRUSHING){
            Color color;


            if(_drawingContext->fillMode == FILL_MODE::PRESERVE_ALPHA){// preserve alpha
                color = _drawingContext->color;
                color.alpha *= shapeColor.alpha;
            } 
            if(_drawingContext->fillMode ==  FILL_MODE::STAMP){ // stamp
                color = _drawingContext->shape->getType() == TYPE_BRUSH_TIP::IMAGE ? shapeColor : _drawingContext->color;
            }
            if(_drawingContext->fillMode == FILL_MODE::FILL){ // stamp
                color = _drawingContext->color;
            }

            color.alpha *= _drawingContext->hardness;
            _toolRuntimeContext->drawingSession->putPixel(it.x, it.y, color, &WriteMode::normal);
            
        } else {
            pattern(it.x, it.y);
        }
    }
}

void DrawTool::pattern(const int& x, const int& y){
    Color color;

    ISurface* pattern =
        (_drawingContext->pattern && _drawingContext->enablePattern) ? _drawingContext->pattern->getMask() : 
        _drawingContext->shape->getType() == TYPE_BRUSH_TIP::IMAGE ? _drawingContext->shape->getMask() :
        nullptr;

    if(pattern != nullptr){
        Point origem;
        Point _startPoint;
        if(_drawingContext->alignMode == ALIGN_MODE::LOCAL){
            origem = {
                _startPoint.x - (pattern->getWidth() >> 1),
                _startPoint.y - (pattern->getHeight() >> 1)
            };
        } else if(_drawingContext->alignMode == ALIGN_MODE::SOURCE){
            origem = { 0, 0 };
        } 
        Point srcPattern = {
            GraphicsEngine::clampedTilePoint((x - origem.x) % pattern->getWidth(), pattern->getWidth()),
            GraphicsEngine::clampedTilePoint((y - origem.y) % pattern->getHeight(), pattern->getHeight())
        };
        
        Color patternColor = pattern->getPixel(srcPattern.x, srcPattern.y);

        // if(_drawingContext->isSolidColor){
        //     color = _drawingContext->color;
        //     color.alpha *= patternColor.alpha;
        // } else {
        //     color = patternColor;
        // }
        if(_drawingContext->fillMode == FILL_MODE::PRESERVE_ALPHA){
            color = _drawingContext->color;
            color.alpha *= patternColor.alpha;
        } else {
            color = patternColor;
        }
    }

    color.alpha *= _drawingContext->hardness;
    _toolRuntimeContext->drawingSession->putPixel(x, y, color, &WriteMode::mask);
}


// Point DrawTool::tiling(int& x, int& y){
//     Layer* layer = _preview->getTarget();
//     Point p = { x, y };
    
//     if(_canvasSettings->tilingContext.isTilingX){
//         p.x = GraphicsEngine::clampedTilePoint(x, layer->getWidth());
//     }
//     if(_canvasSettings->tilingContext.isTilingY){
//         p.y = GraphicsEngine::clampedTilePoint(y, layer->getHeight());
//     }

//     return p;
// }

// void DrawTool::symmetry(int& x, int& y, const Color& color, IPixelWriter* pixelWriter){    
//     Layer* layer = _preview->getTarget();
    
//     int toMirrorX = _symmetryContext->pointMirrored(x, layer->getWidth());
//     int toMirrorY = _symmetryContext->pointMirrored(y, layer->getHeight());

//     if(_symmetryContext->isMirrorX){
//         pixelWriter->plot(_preview, toMirrorX, y, color);
//     }            
//     if(_symmetryContext->isMirrorY){
//         pixelWriter->plot(_preview, x, toMirrorY, color);
//     }
//     if(_symmetryContext->isMirrorX && _symmetryContext->isMirrorY){
//         pixelWriter->plot(_preview, toMirrorX, toMirrorY, color);
//     }
// }

