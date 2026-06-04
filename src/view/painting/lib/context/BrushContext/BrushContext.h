#ifndef BRUSHCONTEXT_H
#define BRUSHCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"
#include <unordered_map>
#include <vector>
#include <string>


struct Pattern{
    std::string name;
    std::vector<unsigned int> buffer;
    int width;
    int height;
};
class BrushContext {
private:

public:
    BrushContext();
    Pattern* selectedPattern;
    static std::vector<Pattern> pattern;
    std::vector<Pattern>::iterator findPattern(std::string name);
    void setActivePattern(std::string name);
    Pattern& getPattern(std::string name);
};

#endif
