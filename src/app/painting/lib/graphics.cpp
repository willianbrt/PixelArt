#ifndef SKETCH
#define SKETCH

#include <emscripten.h>
#include <emscripten/bind.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>

#include "pixel.cpp"
#include "layers.cpp"

const unsigned int CHECKERED_LIGHT_COLOR_HEX = 0xFFFFFFFF;
const unsigned int CHECKERED_DARK_COLOR_HEX = 0xDDDDDDFF;
const unsigned int CHECKERED_HOVER_LIGHT_COLOR_HEX = 0x555555FF;
const unsigned int CHECKERED_HOVER_DARK_COLOR_HEX = 0x222222FF;
const int MAX_LAYERS = 10;

using namespace std;
class Scene{
public:
    Scene(unsigned int width, unsigned int height) {
        _scene = (unsigned int*)malloc(width*height * sizeof(unsigned int));
        if (!_scene){
            free(_scene);
            throw "Erro ao alocar memória.";
        }

        _width = width;
        _height = height;
        position = getInitialPosition();
        _scale = getMinScale();
        
        addLayer("Layer 1");
    }
    ~Scene(){
        free(_scene);
    }

    void render(){
        int viewportWidth = getViewportWidth(); 
        int viewportHeight = getViewportHeight();
        
        int resizedWidth = getCurrentWidth();
        int resizedHeight = getCurrentHeight();
        
        bool isVisibleX = position.x < viewportWidth || position.x > -resizedWidth;
        bool isVisibleY = position.y < viewportHeight || position.y > -resizedHeight;

        if(!isVisibleX || !isVisibleY) return;

        unsigned int maxPositionX = viewportWidth - resizedWidth;
        unsigned int maxPositionY = viewportHeight - resizedHeight;

        unsigned int startVisibleX = (position.x < 0) ? -position.x : 0;
        unsigned int endVisibleX =  (position.x < maxPositionX) ? resizedWidth : resizedWidth - (position.x - maxPositionX);
        unsigned int startVisibleY = (position.y < 0) ? -position.y : 0;
        unsigned int endVisibleY = (position.y < maxPositionY) ? resizedHeight : resizedHeight - (position.y - maxPositionY);

        render(startVisibleX, endVisibleX, startVisibleY, endVisibleY);
    }

    void render(int startVisibleX, int endVisibleX, int startVisibleY, int endVisibleY){
        unsigned int visibleWidth = endVisibleX - startVisibleX;
        unsigned int visibleHeight = endVisibleY - startVisibleY;

        int length = visibleWidth*visibleHeight;

        unsigned int* viewBuffer = (unsigned int*)malloc(length * sizeof(unsigned int));
        if (!viewBuffer){
            free(viewBuffer);
            throw "Erro ao alocar memória.";
        }
        
        composeScene();

        int startOriginalY = (startVisibleY) / _scale;
        int startOriginalX = (startVisibleX) / _scale;

        int startOriginalIndex = (startOriginalY * _width + startOriginalX);
        int resizedIndex = 0;

        int yIncrement = startVisibleY - startOriginalY;
        int flagXIncrement = startVisibleX - startOriginalX;

        for (int y = 0; y < visibleHeight; y++) {
            int originalIndex = startOriginalIndex;
            int xIncrement = flagXIncrement;
            
            for (int x = 0; x < visibleWidth; x++) {
                viewBuffer[resizedIndex] = _scene[originalIndex];
                swap_endian_uint32(&viewBuffer[resizedIndex]);

                resizedIndex++;
                xIncrement++;
                if(xIncrement == _scale){
                    xIncrement = 0;
                    originalIndex++;
                }
            }
            
            yIncrement++;
            if(yIncrement == _scale){
                yIncrement = 0;
                startOriginalIndex += _width;
            }
        }

        int x = position.x + startVisibleX;
        int y = position.y + startVisibleY;
        renderCanvas(viewBuffer, length, visibleWidth, x, y);

        free(viewBuffer);
    }

    unsigned int blending(unsigned int topColor, unsigned int bottomColor){
        const Color dst = Color(bottomColor);
        const Color src = Color(topColor);

        const float factorAlphaSrc = src.alpha / 255.0f;

        const unsigned char outRed   = factorAlphaSrc*src.red   + (1.0f - factorAlphaSrc)*dst.red;
        const unsigned char outGreen = factorAlphaSrc*src.green + (1.0f - factorAlphaSrc)*dst.green;
        const unsigned char outBlue  = factorAlphaSrc*src.blue  + (1.0f - factorAlphaSrc)*dst.blue;
        const unsigned char outAlpha = dst.alpha;
        
        return outRed << 24 | outGreen << 16 | outBlue << 8 | outAlpha;
    }

    int calcStartPosition(){ return 0; }
    int getCurrentWidth(){ return _width*_scale; }
    int getCurrentHeight(){ return _height*_scale;}

    void addLayer(string name){
        layers.emplace_back(name, _width, _height);
        
        // unsigned int color = layers[0].getPixel(2);
        // string namel = layers[0].getName();
        // EM_ASM({
        //     console.log(UTF8ToString($1), ($0 >>> 0).toString(16));
        // }, color, &namel);
    }
    vector<Layer>& getAllLayers(){
        return layers;
    }
    Layer& getLayer(unsigned int index){
        return layers[index];
    }
    Layer& getActiveLayer(){
        return getLayer(activeLayer);
    }

private:
    void composeScene(){
        int index = 0;
        
        for(int x = 0; x < _width; x++){
            for(int y = 0; y < _height; y++){
                updatePixelScene(index);
                index++;
            }
        }
    }

    void updateScene(const unsigned int* listIndex, const unsigned int length){
        for(int i = 0; i < length; i++) {
            unsigned int index = listIndex[i];
            updatePixelScene(index);
        }
    }

    void updatePixelScene(unsigned int index){
        unsigned int bottomColor = getDefaultColor(index);

        for(Layer& layer : layers){
            if(!layer.isVisible()){ continue; }

            unsigned int topColor = layer.getPixel(index);

            bottomColor = blending(topColor, bottomColor);
        }
        
        _scene[index] = bottomColor;
    }

    unsigned int getDefaultColor(unsigned int x, unsigned int y){
        unsigned color = ((x+y) & 0x1) ? CHECKERED_LIGHT_COLOR_HEX : CHECKERED_DARK_COLOR_HEX;
        return color;
    }

    unsigned int getDefaultColor(unsigned int index){
        unsigned int x = index / _width;
        unsigned int y = index - x*_width;
        
        return getDefaultColor(x, y);
    }

    void renderCanvas(unsigned int* buffer, int length, int width, int x, int y){
        EM_ASM({
            const canvas = Module.canvas;
            const context = canvas.getContext("2d");

            context.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);

            const ptr = $0;
            const length = $1;
            const width = $2;

            const x = $3;
            const y = $4;

            const buffer = new Uint8ClampedArray(Module.HEAPU8.buffer, ptr, length*4);

            console.log(ptr, length, x, y);
            console.log(buffer);

            const data = new ImageData(buffer, width);
            context.putImageData(data, x, y);
        }, buffer, length, width, x, y);
    }

    unsigned int getViewportHeight(){
        return EM_ASM_INT({
            const canvas = Module.canvas;
            return canvas.clientHeight;
        });
    }  
    unsigned int getViewportWidth(){
        return EM_ASM_INT({
            const canvas = Module.canvas;
            return canvas.clientWidth;
        });
    }
    
    int getMinScale(){ return floor(max<unsigned int>(1, min(getViewportHeight()/_height, getViewportWidth()/_width))); }
    int getMaxScale(){ return _width <= 16 || _height <= 16 ? 1 : getMinScale() + 10; }

    Point getInitialPosition(){
        Point p = Point();
        
        p.x = floor((getViewportWidth() - (_width*getMinScale())) / 2);
        p.y = floor((getViewportHeight() - (_height*getMinScale())) / 2);
        return p;
    }

    unsigned int* _scene;
    unsigned int _width;
    unsigned int _height;
    unsigned int _scale = 1;
    Point position;
    unsigned int activeLayer = 1;
    vector<Layer> layers;
};

using namespace emscripten;
EMSCRIPTEN_BINDINGS(scene_module){
    class_<Scene>("Scene")
                .constructor<unsigned int, unsigned int>()
                .function("render", select_overload<void()>(&Scene::render))
                // .function("getBuffer", &Scene::teste, emscripten::allow_raw_pointers())
                // .function("getBuffer", &Scene::teste, emscripten::allow_raw_pointers())
                .function("blending", &Scene::blending)
                ;
};
#endif