#ifndef IOBSERVABLELIST_H
#define IOBSERVABLELIST_H

#include <vector>

template<class T>
class IObservableList{
private:
    std::vector<T> list;
public:
    IObservableList<T>();
    // virtual void onChanged() = 0;
    void add(T item) = 0;
    void remove(int index) = 0;
    void moveIndexTo(int originalIndex, int destIndex) = 0;
    void onChanged() = 0;
};

#endif