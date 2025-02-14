#ifndef COMPONDING_H
#define COMPONDING_H

#include <vector>
#include <algorithm>

#include "../../interfaces/ITile/ITile.h"

using namespace std;

template <typename T = ITile>
class CompoundingTiles {
protected:
    vector<T> tiles;
    int active = 0;
    
public:
    virtual void bringTileTo(size_t from, size_t to) = 0;
    virtual void removeTile(int index) = 0;
    virtual void addTile(T tile) = 0;
    virtual vector<T>& getAllTiles() = 0;
    virtual T& getTileByIndex(unsigned int index) = 0;
    virtual T& getActiveTile() = 0;
    virtual int getIndexFromActiveTile() = 0;
    virtual void changeActiveTile(unsigned int index) = 0;
};
// #include "CompoundingTiles.tpp"
#endif