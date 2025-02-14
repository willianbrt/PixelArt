#include "Frame.h"

Frame::Frame(unsigned int width, unsigned int height) {
    _width = width;
    _height = height;
    addTile(new Layer("Layer 1", _width, _height));

    tiles.at(0)->putPixel(5, 5, 0xFF00FFFF);
    tiles.at(0)->putPixel(1, 57, 0xFFFFFFFF);
    tiles.at(0)->putPixel(99, 10, 0xFFFF00FF);
    tiles.at(0)->putPixel(9, 99, 0xFF0000FF);

    printf("Layer: %s \n", tiles.at(0)->getName().c_str());
}

Frame::~Frame(){}

// void accept(ITileVisitor visitor){
//     visitor.layerVisitor(*this);
// }

void Frame::resize(int width, int height){
    for(auto& layer : tiles){
        layer->resize(width, height);
    }
}
void Frame::move(int offsetX, int offsetY){
    for(auto& layer : tiles){
        layer->move(offsetX, offsetY);
    }
}
void Frame::draw(IGraphic& graphic){
    tiles.at(active)->draw(graphic);
}

unsigned int Frame::getWidth(){ return _width; }
unsigned int Frame::getHeight(){ return _height; }

unsigned int Frame::getPixel(int x, int y){ return getPixel(calcIndex(x,y)); }
unsigned int Frame::getPixel(int index){
    return getPixel(index, 0, tiles.size());
}
unsigned int Frame::getPixel(int index, int fromIndex, int toIndex){
    unsigned int colorHex = 0;
    if(toIndex > tiles.size()) throw std::runtime_error("ToIndex excede o tamanho maximo de Layers.");
    
    for(int layerIndex = fromIndex; layerIndex < toIndex; layerIndex++){
        Layer* layer = tiles.at(layerIndex);
        if(!layer->isVisible()) continue;

        blending(colorHex, layer->getPixel(index));
    }

    return colorHex;
}

void Frame::putPixel(int x, int y, unsigned int colorHex){ putPixel(calcIndex(x, y), colorHex); }
void Frame::putPixel(int index, unsigned int colorHex){ 
    tiles.at(active)->putPixel(index, colorHex); 
}

unsigned int Frame::calcIndex(int x, int y){ return x + y*_width; }

unsigned int Frame::getFrameDuration(){ return timeDuration; }

void Frame::blending(unsigned int& bottomColor, unsigned int topColor){
    const float factorAlphaSrc = ALPHA(topColor) / 255.0f;

        int rsrc = (topColor >> 24) & 0xFF;
        int gsrc = (topColor >> 16) & 0xFF;
        int bsrc = (topColor >> 8) & 0xFF;
        int asrc = (topColor & 0xFF);
        
        int rdst = (bottomColor >> 24) & 0xFF;
        int gdst = (bottomColor >> 16) & 0xFF;
        int bdst = (bottomColor >> 8) & 0xFF;
        int adst = (bottomColor & 0xFF);

        int rout = factorAlphaSrc* rsrc + (1.0f - factorAlphaSrc)*rdst;
        int gout = factorAlphaSrc* gsrc + (1.0f - factorAlphaSrc)*gdst;
        int bout = factorAlphaSrc* bsrc + (1.0f - factorAlphaSrc)*bdst;
        int aout = factorAlphaSrc* asrc + (1.0f - factorAlphaSrc)*adst;

        bottomColor = (rout << 24 | gout << 16 | bout << 8 | aout);

}

void Frame::bringTileTo(size_t from, size_t to){
    if (from == to || from >= tiles.size() || to >= tiles.size()) return;
    
    if (from < to)
        std::swap(to, from);

    // if (from < to) {
    //     std::rotate(tiles.begin() + from, tiles.begin() + from + 1, tiles.begin() + to + 1);
    // } else {
    // }
    std::rotate(tiles.begin() + to, tiles.begin() + from, tiles.begin() + from + 1);
}
void Frame::removeTile(int index){
    typename vector<Layer*>::iterator it = tiles.begin();
    advance(it, index);
    tiles.erase(it);
}
void Frame::addTile(Layer* tile){
    tiles.emplace_back(tile);
}
vector<Layer*> Frame::getAllTiles(){
    return tiles;
}
Layer* Frame::getTileByIndex(unsigned int index){
    return tiles.at(index);
}
Layer* Frame::getActiveTile(){
    return getTileByIndex(active);
}
int Frame::getIndexFromActiveTile(){
    return active;
}
void Frame::changeActiveTile(unsigned int index){
    if(tiles.size() < index && index >= 0)
        return;

    active = index;
}
size_t Frame::getNumberOfTiles(){
    return tiles.size();
}


using namespace emscripten;
EMSCRIPTEN_BINDINGS(frame_module){

    // // class_<Frame, base<CompoundingTiles<Layer>>>("Frame")
    // class_<Frame, base<ITile>>("Frame")
    //     .constructor<unsigned int, unsigned int>()
    //     .function("getFrameDuration", &Frame::getFrameDuration)
                
    //     .function("changeActiveTile", &Frame::changeActiveTile)
    //     .function("getIndexFromActiveTile", &Frame::getIndexFromActiveTile)
    //     .function("getActiveTile", &Frame::getActiveTile, allow_raw_pointers())
    //     .function("getTileByIndex", &Frame::getTileByIndex, allow_raw_pointers())
    //     .function("getAllTiles", &Frame::getAllTiles)
    //     .function("addTile", &Frame::addTile)
    //     .function("removeTile", &Frame::removeTile)
    //     .function("bringTileTo", &Frame::bringTileTo);

    register_vector<Layer*>("VectorLayer");

    class_<Frame>("Frame")
        .constructor<unsigned int, unsigned int>()
        .smart_ptr<std::shared_ptr<Frame>>("shared_ptr<Frame>")
        .function("resize", &Frame::resize)
        .function("move", &Frame::move)
        .function("draw", &Frame::draw)
        // .function("getPixel", static_cast<unsigned int (Frame::*)(int, int)>(&Frame::getPixel))
        // .function("putPixel", static_cast<void (Frame::*)(int, int, unsigned int)>(&Frame::putPixel))
        // .function("getWidth", &Frame::getWidth)
        // .function("getHeight", &Frame::getHeight)
        // .function("calcIndex", &Frame::calcIndex)
        .function("getFrameDuration", &Frame::getFrameDuration)
        
        .function("bringTileTo", &Frame::bringTileTo)
        .function("removeTile", &Frame::removeTile)
        .function("addTile", &Frame::addTile, allow_raw_pointers())
        .function("getAllTiles", &Frame::getAllTiles, allow_raw_pointers())
        .function("getTileByIndex", &Frame::getTileByIndex, allow_raw_pointers())
        .function("getActiveTile", &Frame::getActiveTile, allow_raw_pointers())
        .function("getIndexFromActiveTile", &Frame::getIndexFromActiveTile)
        .function("changeActiveTile", &Frame::changeActiveTile)
        .function("getNumberOfTiles", &Frame::getNumberOfTiles);
};