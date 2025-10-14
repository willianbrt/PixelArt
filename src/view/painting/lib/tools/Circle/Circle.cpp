#include "Circle.h"

Circle::Circle(Layer& layer, unsigned int x, unsigned int y, unsigned int newColorHex) : _layer(layer){
    _point = Point(x,y);
    _newColorHex = newColorHex;
}
void Circle::draw(unsigned int cx_u, unsigned int cy_u, unsigned int r_u) {
    // converter para int para suportar coordenadas negativas durante a computação
    int cx = static_cast<int>(cx_u);
    int cy = static_cast<int>(cy_u);
    int r  = static_cast<int>(r_u);

    if (r <= 0) return;

    int x = 0;
    int y = r;
    int d = 1 - r; // decisão inicial (midpoint)

    // helper para plotar com checagem de coordenadas >= 0
    auto safePut = [this](int px, int py, unsigned int color) {
        if (px < 0 || py < 0) return; // evita cast inválido para unsigned que causaria wrap
        // Se quiser checar limites máximos da layer, adicione aqui (ex.: px < _width ...)
        _layer.putPixel(static_cast<unsigned int>(px),
                        static_cast<unsigned int>(py),
                        color);
    };

    while (x <= y) {
        // 8 simetrias
        safePut(cx + x, cy + y, _newColorHex);
        safePut(cx - x, cy + y, _newColorHex);
        safePut(cx + x, cy - y, _newColorHex);
        safePut(cx - x, cy - y, _newColorHex);
        safePut(cx + y, cy + x, _newColorHex);
        safePut(cx - y, cy + x, _newColorHex);
        safePut(cx + y, cy - x, _newColorHex);
        safePut(cx - y, cy - x, _newColorHex);

        // atualizar decisão
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

// void Circle::draw(unsigned int cx, unsigned int cy, unsigned int r){
// void Circle::draw(unsigned int cx_u, unsigned int cy_u, unsigned int r_u) {
    
//     int cx = static_cast<int>(cx_u);
//     int cy = static_cast<int>(cy_u);
//     int r  = static_cast<int>(r_u);

//     int x = 0;
//     int y = -r;
//     int p = -r;

//     const unsigned int _width = 32;
//     const unsigned int _height = 32;
//     while(x < -y) {

//         if(p > 0){
//             y+=1;
//             p+=2*(x+y)+1;
//         }else{
//             p+=2*x+1;
//         }
//         // if (cx + x < _width && cy + y < _height && cx + x >= 0 && cy + y >= 0)
//         //     _layer.putPixel(cx + x, cy + y, _newColorHex);

//         _layer.putPixel(cx + _point.x, cy + _point.y, _newColorHex);
//         _layer.putPixel(cx - _point.x, cy + _point.y, _newColorHex);
//         _layer.putPixel(cx + _point.x, cy - _point.y, _newColorHex);
//         _layer.putPixel(cx - _point.x, cy - _point.y, _newColorHex);
//         _layer.putPixel(cx + _point.y, cy + _point.x, _newColorHex);
//         _layer.putPixel(cx + _point.y, cy - _point.x, _newColorHex);
//         _layer.putPixel(cx - _point.y, cy + _point.x, _newColorHex);
//         _layer.putPixel(cx - _point.y, cy - _point.x, _newColorHex);
//         x+=1;
//     }
// }


using namespace emscripten;

EMSCRIPTEN_BINDINGS(circle_module){
    class_<Circle>("Circle")
        .constructor<Layer&, unsigned int,unsigned int,unsigned int>()
        .smart_ptr<std::shared_ptr<Circle>>("shared_ptr<Circle>")
        .function("draw", &Circle::draw);
};
