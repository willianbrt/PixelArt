
#ifndef RENDERERPIPELINE_H
#define RENDERERPIPELINE_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include "../../objects/Editor/Editor.h"
#include "../../graphics/surface/Surface.h"
// #include "../../objects/CanvasSettings/CanvasSettings.h"
#include "../../objects/Viewport/Viewport.h"
#include "../../context/CursorContext/CursorContext.h"

#include "../../interfaces/IRenderPass/IRenderPass.h"
#include <vector>
#include "../Shader/Shader.h"

class RendererPipeline
{
public:
    RendererPipeline();
    void render();
    void registerPass(IRenderPass* pass);
private:
    vector<IRenderPass*> passes;
};
#endif