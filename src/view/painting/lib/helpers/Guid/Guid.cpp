#include "Guid.h"

#include <emscripten.h>
#include <emscripten/bind.h>
Guid::Guid(){}
Guid::Guid(const std::string& str) {
    if (str.size() != 36)
        throw std::invalid_argument("Invalid UUID size");

    char buf[32];
    int j = 0;

    for (char c : str)
        if (c != '-') buf[j++] = c;

    high = hexToUint64(buf);
    low  = hexToUint64(buf + 16);

};
Guid::Guid(uint64_t high, uint64_t low) : high(high), low(low) {};

Guid Guid::generateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);
    
    return Guid(dis(gen), dis(gen));
};

uint64_t Guid::hexToUint64(const char* p)
{
    uint64_t v = 0;
    for (int i = 0; i < 16; i++)
    {
        char c = *p++;
        v <<= 4;

        if (c >= '0' && c <= '9') v |= (c - '0');
        else if (c >= 'a' && c <= 'f') v |= (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') v |= (c - 'A' + 10);
        else throw std::invalid_argument("Invalid hex char");
    }
    return v;
}


std::string Guid::toString() {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    ss << std::setw(8) << ((high >> 32) & 0xFFFFFFFF) << "-"
       << std::setw(4) << ((high >> 16) & 0xFFFF) << "-"
       << std::setw(4) << ((high) & 0xFFFF) << "-"
       << std::setw(4) << ((low >> 48) & 0xFFFF) << "-"
       << std::setw(12) << (low & 0xFFFFFFFFFFFF);

    return ss.str();
};

using namespace emscripten;
EMSCRIPTEN_BINDINGS(guid_module){
    class_<Guid>("Guid")
        .constructor<std::string>()
        .class_function("generateUUID", &Guid::generateUUID)
        .function("toString", &Guid::toString);
};