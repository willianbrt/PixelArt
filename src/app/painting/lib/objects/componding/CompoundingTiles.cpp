#include "CompoundingTiles.h"
#include <emscripten.h>
#include <emscripten/bind.h>
 
// template <typename T>
// inline void CompoundingTiles<T>::bringElement(size_t from, size_t to){
//     if (from == to || from >= tiles.size() || to >= tiles.size()) return;

//     if (from < to) {
//         std::rotate(tiles.begin() + from, tiles.begin() + from + 1, tiles.begin() + to + 1);
//     } else {
//         std::rotate(tiles.begin() + to, tiles.begin() + from, tiles.begin() + from + 1);
//     }
// }


// template <typename T>
// inline void CompoundingTiles<T>::remove(int index){
//     typename vector<T>::iterator it = tiles.begin();
//     advance(it, index);
//     tiles.erase(it);
// }

// template <typename T>
// inline void CompoundingTiles<T>::add(T tile){
//     tiles.emplace_back(tile);
// }

// template <typename T>
// inline vector<T>& CompoundingTiles<T>::getAll(){
//     return tiles;
// }

// template <typename T>
// inline T& CompoundingTiles<T>::getByIndex(unsigned int index){
//     return tiles.at(index);
// }

// template <typename T>
// inline T& CompoundingTiles<T>::getActive(){
//     return getByIndex(active);
// }

// template <typename T>
// inline int CompoundingTiles<T>::getActiveIndex(){
//     return active;
// }

// template <typename T>
// inline void CompoundingTiles<T>::changeActive(unsigned int index){
//     if(tiles.size() < index && index >= 0)
//     active = index;
// }

// template <typename T>
// template class CompoundingTiles<T>;

// using namespace emscripten;
// EMSCRIPTEN_BINDINGS(compounding_module){
//     class_<CompoundingTiles<val>>("CompoundingTiles")
//         .constructor<>()
//         .function("changeActive", &CompoundingTiles<val>::changeActive)
//         .function("getActiveIndex", &CompoundingTiles<val>::getActiveIndex)
//         .function("getByIndex", &CompoundingTiles<val>::getByIndex)
//         .function("getAll", &CompoundingTiles<val>::getAll)
//         .function("add", &CompoundingTiles<val>::add)
//         .function("remove", &CompoundingTiles<val>::remove)
//         .function("bringElement", &CompoundingTiles<val>::bringElement);
// };