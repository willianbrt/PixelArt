#include "BrushContext.h"

std::vector<Pattern> BrushContext::pattern = {
    Pattern(
        "dot",
        {
            0x000000FF 
        }, 1, 1
    ),
    Pattern(
        "brush_1",
        {
            0x00000000, 0x00000022, 0x00000022, 0x00000033,
            0x00000033, 0x00000055, 0x00000000, 0x00000022,
            0x00000033, 0x00000055, 0x00000088, 0x00000088,
            0x00000022, 0x00000000, 0x00000033, 0x00000022,
            0x00000000, 0x00000022, 0x00000022, 0x00000033
        }, 4, 5
    )
};

BrushContext::BrushContext(){}
void BrushContext::setActivePattern(std::string name){
    selectedPattern = &getPattern(name);
}
Pattern& BrushContext::getPattern(std::string name){
    return *findPattern(name);
}
std::vector<Pattern>::iterator BrushContext::findPattern(std::string name){
    auto it = std::find_if(
        pattern.begin(), 
        pattern.end(), 
        [&name](const Pattern& p){ return p.name == name; });
    
    if(it == pattern.end()){
        throw std::runtime_error("pattern nao encontrado");
    }
    return it;
}