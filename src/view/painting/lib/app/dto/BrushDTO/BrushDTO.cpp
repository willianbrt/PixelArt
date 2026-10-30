#include "./BrushDTO.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(pixel_editor_module){
    value_object<BrushDTO>("BrushDTO")
        .field("id", &BrushDTO::id)
        .field("pattern", &BrushDTO::pattern)
        .field("shape", &BrushDTO::shape)
        .field("color", &BrushDTO::color)
        .field("hardness", &BrushDTO::hardness)
        .field("smoothness", &BrushDTO::smoothness)
        .field("spacing", &BrushDTO::spacing)
        .field("fillMode", &BrushDTO::fillMode)
        .field("size", &BrushDTO::size)
        .field("alignMode", &BrushDTO::alignMode)
        .field("stabilizer", &BrushDTO::stabilizer)
        .field("pixelPerfect", &BrushDTO::pixelPerfect)
        .field("isPatternEnable", &BrushDTO::isPatternEnable)
        .field("isSolidColor", &BrushDTO::isSolidColor)
        .field("typeBrushTip", &BrushDTO::typeBrushTip)
        ;
}