#ifndef GUID_H
#define GUID_H
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>
class Guid{
private:
uint64_t high;
uint64_t low;

Guid(uint64_t high, uint64_t low);

public:
    static Guid generateUUID();
    std::string toString();
};

#endif