#ifndef BRUSHCONTEXT_H
#define BRUSHCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"
#include <unordered_map>
#include <vector>
#include <string>


struct Pattern{
    std::string name;
    std::vector<std::vector<float>> buffer;
    int width;
    int height;
};
class BrushContext : IToolContext{
private:

public:
    std::string selectedPattern;
    std::vector<Pattern> pattern = {
            {
                "dot",
                {
                    {1.0f}
                }, 1, 1
            },
            {
                "brush_1",
                {
                    {0.9f, 1.0f, 0.8f},
                    {0.7f, 1.0f, 0.5f},
                    {0.2f, 1.0f, 0.3f}
                }, 3, 3
            }
        };
    
    std::vector<Pattern>::iterator findPattern(std::string name);
    void setActivePattern(std::string name);
    Pattern getPattern(std::string name);
};

#endif
