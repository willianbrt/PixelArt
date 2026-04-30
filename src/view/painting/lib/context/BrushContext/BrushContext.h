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
    std::vector<Pattern> pattern = {
            {
                "dot",
                {
                    0x000000FF 
                }, 1, 1
            },
            {
                "brush_1",
                {
                    0x00000000, 0x00000022, 0x00000022, 0x00000033,
                    0x00000033, 0x00000055, 0x00000000, 0x00000022,
                    0x00000033, 0x00000055, 0x00000088, 0x00000088,
                    0x00000022, 0x00000000, 0x00000033, 0x00000022,
                    0x00000000, 0x00000022, 0x00000022, 0x00000033
                }, 4, 5
            }
        };
    std::vector<Pattern>::iterator findPattern(std::string name);
    void setActivePattern(std::string name);
    Pattern& getPattern(std::string name);
};

#endif
