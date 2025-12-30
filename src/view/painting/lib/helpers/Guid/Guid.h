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
static uint64_t hexToUint64(const char* p);

public:
    Guid(const std::string& str);
    static Guid generateUUID();
    std::string toString();
};

#endif