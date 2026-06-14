#ifndef SELECTPASS_H
#define SELECTPASS_H

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include "../../../interfaces/IRenderPass/IRenderPass.h"


#include "../../../objects/EditorManager/EditorManager.h"
#include "../../../objects/Editor/Editor.h"
#include "../../../graphics/surface/Surface.h"
#include "../../../graphics/surface/Surface.h"
#include "../../../context/CursorContext/CursorContext.h"
#include "../../../context/ViewportContext/ViewportContext.h"
#include "../../Quad/Quad.h"
#include "../../Shader/Shader.h"
#include "../../Texture/Texture.h"

class SelectPass : public IRenderPass {
private:
    Surface* _surface;
    ViewportContext* _viewport;
    CursorContext* hover;
    Editor* editor;
    EditorManager* _manager;
    
    GLint texSizeLocation;
    GLint resizeHandleLocation;
    GLint rotateHandleLocation;
    GLint selectSizeLocation;
    GLint enabledLocation;
    
    bool initialized;
    Bounding _area;

    GLint cursorLocation;
    Texture texture;
    Quad quad;
    Shader shader;


public:
    SelectPass(EditorManager* manager, ViewportContext* viewport);
    ~SelectPass();

    void render();
};
#endif