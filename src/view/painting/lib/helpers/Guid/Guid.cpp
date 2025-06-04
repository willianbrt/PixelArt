#include "Guid.h"

#include <emscripten.h>
#include <emscripten/bind.h>

Guid::Guid(uint64_t high, uint64_t low) : high(high), low(low) {};

Guid Guid::generateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);
    
    return Guid(dis(gen), dis(gen));
};

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
        .function("toString", &Guid::toString);
};