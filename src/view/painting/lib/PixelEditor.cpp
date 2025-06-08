// #include "PixelEditor.h"

// #include <emscripten.h>
// #include <emscripten/bind.h>

// PixelEditor::PixelEditor(unsigned int width, unsigned int height) : previewLayer("preview", width, height) {
//     int length = getViewportWidth()*getViewportHeight();

//     _length = length;
//     _width = width;
//     _height = height;

//     _scale = getMinScale();
//     sketchLocation = getInitialPosition();
//     screen = (unsigned int*)malloc(length * sizeof(unsigned int));

//     addTile(new Frame(_width, _height));
//     emscripten_get_canvas_element_size("#paintingCanvas", &_viewportWidth , &_viewportHeight);
// }

// PixelEditor::~PixelEditor(){
//     free(screen);
// }

// void PixelEditor::renderPreview(IGraphic& graphic){
//     previewLayer.draw(graphic);
//     render();
// }
// void PixelEditor::abortPreview(){
//     previewLayer = Layer("preview", _width, _height);
//     render();
// }
// void PixelEditor::draw(IGraphic& graphic){
//     Frame* activeFrame = getActiveTile();
//     activeFrame->draw(graphic);
//     render();
// }

// void PixelEditor::clear(Point start, Point end){
//     memset(screen, 0, _length*sizeof(unsigned int));
//     std::fill_n(screen, _length, 0x0);
// }
// void PixelEditor::render(){
//     std::fill_n(screen, _length, 0x0);

//     Point start, end;

//     if(!computeVisibleShape(sketchLocation.x, getCurrentWidth(), getViewportWidth(), start.x, end.x))
//         return;

//     if(!computeVisibleShape(sketchLocation.y, getCurrentHeight(), getViewportHeight(), start.y, end.y))
//         return;

//     renderArea(start, end);
// }
// void PixelEditor::renderArea(Point start, Point end){
//     int viewportWidth = getViewportWidth();
//     int visibleWidth = end.x - start.x;
//     int visibleHeight = end.y - start.y;

//     Point original;
//     original.x = (start.x - sketchLocation.x) / _scale;
//     original.y = (start.y - sketchLocation.y) / _scale;

//     unsigned int indexOriginal = original.y * _width + original.x;
//     unsigned int flagIndexOriginal = indexOriginal;

//     unsigned int index = start.y * viewportWidth + start.x;

//     int heightPixel = _scale - ((start.y - sketchLocation.y) % _scale);
//     int endLine = index + (viewportWidth * heightPixel);

//     for (int y = 0; y < visibleHeight; y += heightPixel) {
//         int xIncrement = (sketchLocation.x < 0) ? ((start.x - sketchLocation.x) % _scale) : 0;

//         heightPixel = (sketchLocation.y < 0 && y == 0)
//                     ? _scale - ((start.y - sketchLocation.y) % _scale)
//                     : ((y + _scale < visibleHeight) ? _scale : (visibleHeight - y));

//         endLine = index + (viewportWidth * heightPixel);

//         int originalX = original.x;

//         unsigned int colorHex = getDefaultColor(originalX, original.y);
//         blending(colorHex, getPixel(indexOriginal));
//         swap_endian_uint32(&colorHex);

//         for (int x = 0; x < visibleWidth; x++) {
//             for(int i = index; i < endLine; i += viewportWidth){
//                 screen[i] = colorHex;
//             }

//             index++;
//             if(++xIncrement >= _scale){
//                 originalX++;
//                 indexOriginal++;

//                 colorHex = getDefaultColor(originalX, original.y);
//                 blending(colorHex, getPixel(indexOriginal));
//                 swap_endian_uint32(&colorHex);

//                 xIncrement = 0;
//             }
//         }
//         original.y++;

//         indexOriginal = flagIndexOriginal + _width;
//         flagIndexOriginal = indexOriginal;

//         index = endLine;
//     }

//     #ifdef __DEBUG
//     Point end;
//     end.x = start.x + visibleWidth;
//     end.y = start.y + visibleHeight;
//     printf("screen_ptr: %i \n", screen);
//     printf("s: %i, h: %i \n", _scale,  heightPixel);
//     // printf("vw: %i, vh: %i, vpw: %i, vph: %i \n", visibleWidth, visibleHeight, viewportWidth,viewportHeight);
//     // printf("rw: %i, rh: %i, ow: %i, oh: %i \n", resizedWidth, resizedHeight, _width, _height);
//     printf("sx: %i, sy: %i, ex: %i, ey: %i \n", sketchLocation.x, sketchLocation.y, end.x, end.y);
//     printf("sx: %i, sy: %i, ex: %i, ey: %i \n", start.x, start.y, end.x, end.y);
//     #endif

//     renderScreen();
// }

// void PixelEditor::zoomIn(Point cursorPosition){
//     int targetScale = _scale + 1;

//     if(targetScale > getMaxScale()) return;
    
//     zoom(targetScale, cursorPosition);
// }
// void PixelEditor::zoomOut(Point cursorPosition){
//     int targetScale = _scale - 1;

//     if(targetScale < getMinScale()) return;

//     zoom(targetScale, cursorPosition);
// }
// void PixelEditor::zoom(int targetScale, Point cursorPosition){
//     if(targetScale < 0) return;

//     int currentWidth = getCurrentWidth();
//     int currentHeight = getCurrentHeight();

//     Point endOfAxis(sketchLocation.x + currentWidth, sketchLocation.y + currentHeight);

//     Point zoomPoint;
//     zoomPoint.x = min(endOfAxis.x, max(sketchLocation.x, cursorPosition.x));
//     zoomPoint.y = min(endOfAxis.y, max(sketchLocation.y, cursorPosition.y));
    
//     float scaleRatio = static_cast<float>(targetScale) / _scale;
    
//     Point location;
//     location.x = zoomPoint.x - static_cast<int>((zoomPoint.x - sketchLocation.x) * scaleRatio);
//     location.y = zoomPoint.y - static_cast<int>((zoomPoint.y - sketchLocation.y) * scaleRatio);

//     _scale = targetScale;
    
//     move(location.x, location.y);
// }
// void PixelEditor::panning(Point fromCursor, Point toCursor){
//     int cursorDeltaX = sketchLocation.x - (fromCursor.x - toCursor.x);
//     int cursorDeltaY = sketchLocation.y - (fromCursor.y - toCursor.y);

//     move(cursorDeltaX, cursorDeltaY);
// }
// void PixelEditor::move(int x, int y){
//     Point initialPosition = getInitialPosition();

//     int minLeftOffset = initialPosition.x;
//     int maxLeftOffset = getViewportWidth() - getCurrentWidth() - minLeftOffset;

//     int minTopOffset = initialPosition.y;
//     int maxTopOffset = getViewportHeight() - getCurrentHeight() - minTopOffset;
    
//     sketchLocation.x = min(minLeftOffset, max(maxLeftOffset, x));
//     sketchLocation.y = min(minTopOffset, max(maxTopOffset, y));
// }

// int PixelEditor::getCurrentWidth(){ return _width*_scale;}
// int PixelEditor::getCurrentHeight(){ return _height*_scale; }

// Point PixelEditor::getInitialPosition(){
//     Point p;
//     p.x = floor((getViewportWidth() - (_width*getMinScale())) / 2);
//     p.y = floor((getViewportHeight() - (_height*getMinScale())) / 2);
//     return p;
// }

// int PixelEditor::getViewportWidth(){ return EM_ASM_INT({ return Module.canvas.clientWidth; }); }
// int PixelEditor::getViewportHeight(){ return EM_ASM_INT({ return Module.canvas.clientHeight; }); }

// int PixelEditor::getScale(){ return _scale;}
// int PixelEditor::getMinScale(){ return floor(max<unsigned int>(1, min(getViewportHeight()/_height, getViewportWidth()/_width))); }
// int PixelEditor::getMaxScale(){ return _width <= 16 || _height <= 16 ? 1 : getMinScale() + 10; }
// Point PixelEditor::getPosition(){ return sketchLocation; }

// unsigned int PixelEditor::getPixel(int index){
//     return getPixel(index, 0, getIndexFromActiveTile()+1);
// }
// unsigned int PixelEditor::getPixel(int index, int fromIndex, int toIndex){
//     unsigned int colorHex = 0;
//     if(toIndex > getAllTiles().size()) throw std::runtime_error("ToIndex excede o tamanho maximo de Frames.");

//     for(int layerIndex = fromIndex; layerIndex < toIndex; layerIndex++){
//         blending(colorHex, getTileByIndex(layerIndex)->getPixel(index));
//     }

//     return colorHex;
// }

// void PixelEditor::putPixel(int x, int y, unsigned int colorHex){
//     putPixel(x + y*_width, colorHex);
// }
// void PixelEditor::putPixel(int index, unsigned int colorHex){
//     screen[index] = colorHex;
// }

// void PixelEditor::hover(int x, int y){
//     int viewportWidth = getViewportWidth();
//     Point locationStart;
//     Point locationEnd;

//     if(!computeVisibleShape(sketchLocation.y + y*_scale, _scale, getViewportHeight(), locationStart.y,  locationEnd.y))
//         return;
    
//     if(!computeVisibleShape(sketchLocation.x + x*_scale, _scale, viewportWidth, locationStart.x,  locationEnd.x))
//         return;

//     int startOfBoundX = locationStart.x + locationStart.y *viewportWidth;

//     unsigned int topColorHex = 0x22222222;
//     unsigned int colorHex = screen[startOfBoundX];

//     swap_endian_uint32(&colorHex);
//     blending(colorHex, topColorHex);
//     swap_endian_uint32(&colorHex);

//     int visiblePixelHeight = (locationEnd.y - locationStart.y);
//     int visiblePixelWidth = (locationEnd.x - locationStart.x);
    
//     int endOfBoundY = startOfBoundX + (visiblePixelHeight * viewportWidth);
//     int endOfBoundX = startOfBoundX + visiblePixelWidth;

//     for(; startOfBoundX < endOfBoundX; startOfBoundX++){
//         for(int index = startOfBoundX; index < endOfBoundY; index+=viewportWidth){
//             screen[index] = colorHex;
//         }
//         endOfBoundY++;
//     }

//     renderScreen();
// }

// unsigned int PixelEditor::getDefaultColor(int x, int y){
//     return (x+y)%2 ? CHECKERED_LIGHT_COLOR_HEX : CHECKERED_DARK_COLOR_HEX;
// }
// unsigned int PixelEditor::getPixelWithPreview(int x, int y){
//     int index = x + y*_width;
//     return getPixelWithPreview(index);
// }

// unsigned int PixelEditor::getPixelWithPreview(int index){
//     unsigned int colorHEX = 0;
//     blending(colorHEX, getPixel(index, 0,  getIndexFromActiveTile()-1));

//     unsigned int colorPreview = previewLayer.getPixel(index);
//     blending(colorHEX, (colorPreview == 0) ? getActiveTile()->getPixel(index) : colorPreview);

//     blending(colorHEX, getPixel(index,  getIndexFromActiveTile()-1,  getIndexFromActiveTile()));

//     return colorHEX;
// }

// void PixelEditor::updatePixel(int x, int y){
//     int viewportWidth = getViewportWidth();

//     unsigned int colorHex = getDefaultColor(x, y);
//     blending(colorHex, getPixelWithPreview(x, y));

//     Point locationStart;
//     Point locationEnd;

//     if(!computeVisibleShape(sketchLocation.y + y*_scale, _scale, getViewportHeight(), locationStart.y,  locationEnd.y))
//         return;
    
//     if(!computeVisibleShape(sketchLocation.x + x*_scale, _scale, viewportWidth, locationStart.x,  locationEnd.x))
//         return;

//     int startOfBoundX = locationStart.x + locationStart.y *viewportWidth;

//     int visiblePixelHeight = (locationEnd.y - locationStart.y);
//     int visiblePixelWidth = (locationEnd.x - locationStart.x);
    
//     int endOfBoundY = startOfBoundX + (visiblePixelHeight * viewportWidth);
//     int endOfBoundX = startOfBoundX+visiblePixelWidth;

//     for(; startOfBoundX < endOfBoundX; startOfBoundX++){
//         for(int index = startOfBoundX; index < endOfBoundY; index+=viewportWidth){
//             screen[index] = colorHex;
//         }
//         endOfBoundY++;
//     }
    
// }

// void PixelEditor::blending(unsigned int& bottomColor, unsigned int topColor) {
//     const float alphaSrc = (topColor & 0xFF) / 255.0f;
//     const float alphaDst = 1.0f - alphaSrc;

//     bottomColor = 
//         (static_cast<int>(alphaSrc * ((topColor >> 24) & 0xFF) + alphaDst * ((bottomColor >> 24) & 0xFF)) << 24) |
//         (static_cast<int>(alphaSrc * ((topColor >> 16) & 0xFF) + alphaDst * ((bottomColor >> 16) & 0xFF)) << 16) |
//         (static_cast<int>(alphaSrc * ((topColor >> 8) & 0xFF) + alphaDst * ((bottomColor >> 8) & 0xFF)) << 8) |
//         (static_cast<int>(alphaSrc * (topColor & 0xFF) + alphaDst * (bottomColor & 0xFF)));
// }

// bool PixelEditor::computeVisibleShape(int originalAxis, int originalSize, int viewportSize, int& outStartAxis, int& outEndAxis){
//     if (originalAxis <= -originalSize || originalAxis >= viewportSize){
//         outEndAxis = 0;
//         outStartAxis = 0;
//         return false;
//     }

//     outStartAxis = std::max(0, originalAxis);
//     outEndAxis = std::min(viewportSize, originalAxis + originalSize);

//     return true;
// }

// void PixelEditor::renderScreen(){
//     EM_ASM({
//         const context = Module.canvas.getContext("2d");

//         const ptr = $0;
//         const length = $1;
//         const width = $2;
//         const height = $3;

//         const x = $4;
//         const y = $5;

//         const buffer = new Uint8ClampedArray(Module.HEAPU8.buffer, ptr, length*4);
//         const data = new ImageData(buffer, width);

//         context.clearRect(x, y, width, height);
//         context.putImageData(data, x, y);
//     }, screen, _length, _viewportWidth, _viewportHeight, 0, 0);
// }

// void PixelEditor::bringTileTo(size_t from, size_t to){
//     if (from == to || from >= tiles.size() || to >= tiles.size()) return;

//     if (from < to)
//         std::swap(to, from);

//     // if (from < to) {
//     //     std::rotate(tiles.begin() + from, tiles.begin() + from + 1, tiles.begin() + to + 1);
//     // } else {
//     // }
//     std::rotate(tiles.begin() + to, tiles.begin() + from, tiles.begin() + from + 1);
// }
// void PixelEditor::removeTile(int index){
//     typename vector<Frame*>::iterator it = tiles.begin();
//     advance(it, index);
//     tiles.erase(it);
// }
// void PixelEditor::addTile(Frame* tile){
//     tiles.emplace_back(tile);
// }
// vector<Frame*> PixelEditor::getAllTiles(){
//     return tiles;
// }
// Frame* PixelEditor::getTileByIndex(unsigned int index){
//     return tiles.at(index);
// }
// Frame* PixelEditor::getActiveTile(){
//     return getTileByIndex(active);
// }
// int PixelEditor::getIndexFromActiveTile(){
//     return active;
// }
// void PixelEditor::changeActiveTile(unsigned int index){
//     if(tiles.size() < index && index >= 0)
//         return;

//     active = index;
// }
// size_t PixelEditor::getNumberOfTiles(){
//     return tiles.size();
// }

// using namespace emscripten;

// EMSCRIPTEN_BINDINGS(pixel_editor_module){
//     register_vector<Frame*>("VectorFrame");

//     emscripten::class_<PixelEditor>("PixelEditor")
//         .constructor<unsigned int, unsigned int>()
//         .smart_ptr<std::shared_ptr<PixelEditor>>("shared_ptr<PixelEditor>")
        
//         .function("bringTileTo", &PixelEditor::bringTileTo)
//         .function("removeTile", &PixelEditor::removeTile)
//         .function("addTile", &PixelEditor::addTile, allow_raw_pointers())
//         .function("getAllTiles", &PixelEditor::getAllTiles, allow_raw_pointers())
//         .function("getTileByIndex", &PixelEditor::getTileByIndex, allow_raw_pointers())
//         .function("getActiveTile", &PixelEditor::getActiveTile, allow_raw_pointers())
//         .function("getIndexFromActiveTile", &PixelEditor::getIndexFromActiveTile)
//         .function("changeActiveTile", &PixelEditor::changeActiveTile)
//         .function("getNumberOfTiles", &PixelEditor::getNumberOfTiles)


//         .function("renderPreview", &PixelEditor::renderPreview)
//         .function("abortPreview", &PixelEditor::abortPreview)
//         .function("draw", &PixelEditor::draw)
//         .function("render", &PixelEditor::render)
//         .function("hover", &PixelEditor::hover);
// };
