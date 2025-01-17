#ifndef LAYER
#define LAYER

#include <emscripten.h>
#include <emscripten/bind.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <cstring>

#include "pixel.cpp"

using namespace std;

class Layer{
public:
    unsigned int* _buffer;
    unsigned int _width;
    unsigned int _height;
    bool _isVisible = true;
    string _name;
    
    Layer(string name, unsigned int width, unsigned int height){
        _buffer = (unsigned int*) malloc(width*height*sizeof(unsigned int));
        if(!_buffer){
            free(_buffer);
            throw runtime_error("Impossível alocar memoria para layer");
        }
        
        memset(_buffer, 0, width*height*sizeof(unsigned int));
        _width = width;
        _name = name;
    }
    
    ~Layer(){
        free(_buffer);
    }
    
    bool isVisible(){
        return _isVisible;
    }

    void visible(bool isVisible){
        _isVisible = isVisible;
    }

    string getName() {    
        return _name;
    }

    unsigned int getPixel(unsigned int x, unsigned int y) {    
        return getPixel(calculateIndex(x, y, _width));
    }

    unsigned int getPixel(unsigned int index, bool littleEndian = false) {    
        unsigned int colorHEX = _buffer[0];
        
        EM_ASM({
            console.log(($0 >>> 0).toString(16));
        }, colorHEX);

        return colorHEX;
    }

    void putPixel(unsigned int x, unsigned int y, unsigned int colorHEX) {
        putPixel(calculateIndex(x, y, _width), colorHEX);
    }

    void putPixel(unsigned int index, unsigned int colorHEX) {
        _buffer[index] = colorHEX;
    }

    unsigned int* getBuffer() {
        return _buffer;
    }

    unsigned int calculateIndex(unsigned int x, unsigned int y, unsigned int width) {
        return y * width + x;
    }
    



    Layer(const Layer&) = delete;
    Layer& operator=(const Layer&) = delete;

    Layer(Layer&& other) noexcept
        : _buffer(other._buffer), _width(other._width), _height(other._height), _name(std::move(other._name)) {
        other._buffer = nullptr;
    }

    Layer& operator=(Layer&& other) noexcept {
        if (this != &other) {
            free(_buffer);
            _buffer = other._buffer;
            _width = other._width;
            _height = other._height;
            _name = std::move(other._name);
            other._buffer = nullptr;
        }
        return *this;
    }

    
};

// EMSCRIPTEN_BINDINGS(scene_module){
//     emscripten::class_<layers::Layer>("Layer")
//                 .constructor<unsigned int, unsigned int>()
//                 // .function("getPixel", &layers::Layer::getPixel)
//                 // .function("putPixel", &layers::Layer::putPixel)
//                 // .function("getBuffer", &layers::Layer::getBuffer)
//                 ;
// };
#endif