#ifndef BRUSHCONTEXT_H
#define BRUSHCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"
#include <unordered_map>
#include <vector>
#include <string>

class BrushContext : IToolContext{
public:
    std::unordered_map<std::string, std::vector<std::vector<float>>> pattern = {
            {"dot", {
                {1.0f}
            }},
            {"brush_1", {
                {0.9f, 1.0f, 0.8f},
                {0.7f, 1.0f, 0.5f},
                {0.2f, 1.0f, 0.3f},
            }}
        };
    std::string selectedPattern;
};

#endif
