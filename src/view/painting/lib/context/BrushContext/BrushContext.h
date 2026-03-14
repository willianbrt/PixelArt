#ifndef BRUSHCONTEXT_H
#define BRUSHCONTEXT_H
#include "../../interfaces/IToolContext/IToolContext.h"
#include <unordered_map>
#include <vector>
#include <string>


class BrushContext : IToolContext{
public:
    struct Pattern{
        std::string name;
        std::vector<std::vector<float>> buffer;
    };

    std::vector<Pattern> lpattern = {
            {"dot", {
                {1.0f}
            }},
            {"brush_1", {
                {0.9f, 1.0f, 0.8f},
                {0.7f, 1.0f, 0.5f},
                {0.2f, 1.0f, 0.3f},
            }}
        };
    std::unordered_map<std::string, std::vector<std::vector<float>>> pattern = {
            {"dot", {
                {1.0f}
            }},
            {"brush_1", {
                {0.9f, 1.0f, 0.8f},
                {0.7f, 1.0f, 0.5f},
                {0.2f, 1.0f, 0.3f},
            }},
            {"brush_2", {
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
            }},
        };
    std::string selectedPattern;
};

#endif
