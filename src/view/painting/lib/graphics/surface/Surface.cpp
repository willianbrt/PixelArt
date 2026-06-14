#include "Surface.h"
#define ASSERT_MSG(cond, fmt, ...) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, fmt "\n", __VA_ARGS__); \
            assert(cond); \
        } \
    } while(0)

    
Surface::Surface(unsigned int* buffer, int width, int height){
    setSize(width, height);
    _data = buffer;

}
Surface::Surface(int width, int height){
    setSize(width, height);
    allocBuffer();
}
Surface::~Surface(){
    free(_data);
}

uintptr_t Surface::getBufferPtr() {
    return reinterpret_cast<uintptr_t>(_data);
}
unsigned int* Surface::getBuffer() {
    return _data;
}
int Surface::getWidth() {
    return _width;
}
int Surface::getHeight() {
    return _height;
}
unsigned int Surface::getLength() {
    return _height*_width;
}
Surface* Surface::crop(Bounding bound){
    bound.start.x = std::min(_width, std::max(0 ,bound.start.x));
    bound.start.y = std::min(_height, std::max(0, bound.start.y));
    bound.end.x = std::min(_width, std::max(0, bound.end.x));
    bound.end.y = std::min(_height, std::max(0, bound.end.y));

    Surface* dirtSurface = new Surface(bound.getWidth(), bound.getHeight());
    
    int boundWidth = bound.getWidth();
    unsigned long length = boundWidth * sizeof(unsigned int);
    unsigned int* ptrDestination = dirtSurface->getBuffer();
    unsigned int* ptr = _data + (bound.start.x + _width*bound.start.y);
    unsigned int* ptrEnd = ptr + (_width*bound.end.y);

    for (unsigned int* index = ptr; index < ptrEnd; index += _width) {
        memcpy(ptrDestination, index, length);
        ptrDestination += boundWidth;
    }
    
    return dirtSurface;
}
unsigned int Surface::getPixel(int x, int y) {
    ASSERT_MSG(x < _width, "[getPixel] (%i, %i - %i)error: x maior que a width;", x, y, _width);
    ASSERT_MSG(x >= 0, "[getPixel] (%i, %i - %i)error: x menor que 0;", x, y, _width);

    ASSERT_MSG(y < _height, "[getPixel] (%i, %i - %i)error: y maior que a width;", x, y, _height);
    ASSERT_MSG(y >= 0, "[getPixel] (%i, %i - %i)error: y menor que 0;", x, y, _height);

    return getPixel(x + y*_width);
}
unsigned int Surface::getPixel(unsigned int index) {
    if (index < 0 || index >= _length)  return 0;

    ASSERT_MSG(index < _length, "[getPixel](%u)error: index maior que o buffer;", index);
    ASSERT_MSG(index >= 0, "[getPixel](%u)error: index menor que 0;", index);
    
    return _data[index];
}
void Surface::putPixel(int x, int y, unsigned int colorHex){
    ASSERT_MSG(x < _width, "[putPixel] (%i, %i - %i) error: x maior que a width;", x, y, _width);
    ASSERT_MSG(x >= 0, "[putPixel] (%i, %i - %i) error: x menor que 0;", x, y, _width);

    ASSERT_MSG(y < _height, "[putPixel] (%i, %i - %i) error: y maior que a width;", x, y, _height);
    ASSERT_MSG(y >= 0, "[putPixel] (%i, %i - %i) error: y menor que 0;", x, y, _height);

    putPixel(x + y*_width, colorHex);
}
bool Surface::isInsideSkecth(int x, int y) {
    return x < _width && y < _height && x >= 0 && y >= 0;
}
void Surface::putPixel(unsigned int index, unsigned int colorHex){
    if (index < 0 || index >= _length) return;

    ASSERT_MSG(index < _length, "[getPixel](%u)error: index maior que o buffer;",index);
    ASSERT_MSG(index >= 0, "[getPixel](%u)error: index menor que 0;", index);

    _data[index] = colorHex;
}
void Surface::setSize(int width, int height){
    _length = width*height;
    _width = width;
    _height = height;
    printf("hs: %i\n", _height);
}
void Surface::allocBuffer(){
    _data = (unsigned int*) malloc(_length*sizeof(unsigned int));

    if(!_data){
        free(_data);
        throw std::runtime_error("Impossível alocar memoria para layer");
    }
    
    memset(_data, 0, _length*sizeof(unsigned int));
}

void Surface::translation(Bounding bound, int deltaX, int deltaY){
    for(int y = bound.start.y; y < bound.end.y; y++){
        for(int x = bound.start.x; x < bound.end.x; x++){
            int index = x + _width*y;
            int newIndex = (x + deltaX) + _width*(y + deltaY);

            _data[newIndex] = _data[index];
            _data[index] = 0x0;
        }
    }
}
