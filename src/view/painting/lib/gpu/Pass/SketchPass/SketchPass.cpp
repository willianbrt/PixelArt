#include "./SketchPass.h"

SketchPass::SketchPass(){
    
}
SketchPass::SketchPass(EditorManager* manager, ViewportContext* viewport){
    _manager = manager;
    _viewport = viewport;
    const char* fs =
        R"(#version 300 es
        precision highp float;

        uniform sampler2D tex;

        in vec2 uv;
        in vec2 pixel;

        layout(std140) uniform GlobalData {
            vec2 resolution;
            vec2 cursor;
            vec2 cursorLocation;
            vec2 pan;
            vec2 zoom;
            vec2 repeat;
            float time;
        };

        uniform vec2 texSize;
        uniform vec2 gridDivisions;
        uniform vec4 lightColor;
        uniform vec4 darkColor;

        out vec4 fragColor;

        void main()
        {
            vec2 transformedUV = pixel / texSize;
            
            if(transformedUV.x < 0.0 || transformedUV.x > 1.0 || transformedUV.y < 0.0 || transformedUV.y > 1.0) discard;
            
            vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
            vec2 thickness = 1.0 / (zoom * texSize);
            transformedUV = fract(transformedUV * repeat);

            vec4 textureColor = texture(tex, transformedUV).abgr;
            color = mix(color, textureColor, textureColor.a);

            vec2 divisions = min(gridDivisions, texSize);
            vec2 cellSize = texSize / divisions;
            vec2 cellCoord = floor(pixel / cellSize);

            float checker = mod(floor(cellCoord.x) + floor(cellCoord.y), 2.0);
            vec4 background = mix(lightColor, darkColor, checker);
            color = mix(background, textureColor, textureColor.a);

            vec2 gridPos = mod(transformedUV, cellSize / texSize);

            float line = step(gridPos.x, thickness.x) + step(gridPos.y, thickness.y);
            line = clamp(line, 0.0, 1.0);

            fragColor = mix(color, vec4(0.0, 0.0, 0.0, 1.0), line);
        })";

    quad.create();
    shader.create(fs);
    shader.use();

    globalUBO.create();
    blockIndex = glGetUniformBlockIndex(shader.id(),  "GlobalData");
    printf("GlobalData index = %u\n", blockIndex);

    glUniformBlockBinding(shader.id(), blockIndex,  0);

    // glGetActiveUniformBlockiv(
    //     shader.id(),
    //     blockIndex,
    //     GL_UNIFORM_BLOCK_DATA_SIZE,
    //     &blockSize
    // );
    // printf("Block size = %d\n", blockSize);


    texture = glGetUniformLocation(shader.id(),"tex");
    texSizeLocation = glGetUniformLocation(shader.id(),"texSize");
    gridDivisionsLocation = glGetUniformLocation(shader.id(),"gridDivisions");
    lightColorLocation = glGetUniformLocation(shader.id(),"lightColor");
    darkColorLocation = glGetUniformLocation(shader.id(),"darkColor");
}
SketchPass::~SketchPass(){}

void SketchPass::init(){
    if (glIsTexture(texture)) {
        glDeleteTextures(1, &texture);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        _surface->getWidth(),
        _surface->getHeight(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _surface->getBuffer()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void SketchPass::draw(){
    editor = _manager->getActiveEditor();
    if(!editor) return;
    _surface = editor->getSurface();
    if(!_surface) return;
    if(!glIsTexture(texture)) init();

    Bounding area = editor->preview()->getDirtyArea();
    if(area.start.x != INT_MAX
        && area.start.y != INT_MAX
        && area.end.x != INT_MIN
        && area.end.y != INT_MIN){
            editor->compose();
            upload(area);
        }
    
    shader.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader.id(),"tex"), 0);
    
    quad.bind();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);

    CanvasSettings* canvasSettings = editor->getCanvasSettings();
    Point panning = canvasSettings->getSketchPosition();
    Point cursorLocation = canvasSettings->cursorToCanvas(_viewport->cursorX, _viewport->cursorY);

    GlobalData data;
    data.resolution[0] = _viewport->width;
    data.resolution[1] = _viewport->height;
    data.cursor[0] = _viewport->cursorX;
    data.cursor[1] = _viewport->cursorY;
    data.cursorLocation[0] = cursorLocation.x;
    data.cursorLocation[1] = cursorLocation.y;
    data.pan[0] = panning.x;
    data.pan[1] = panning.y;
    data.repeat[0] = canvasSettings->getTilesX();
    data.repeat[1] = canvasSettings->getTilesY();
    data.zoom[0] = canvasSettings->getScale();
    data.zoom[1] = canvasSettings->getScale();
    data.time = glfwGetTime();

    // glBindBuffer(GL_UNIFORM_BUFFER, globalUBO);
    // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GlobalData), &data);
    glBindBuffer(GL_UNIFORM_BUFFER, globalUBO.id());
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);

    glUniform2f(gridDivisionsLocation, canvasSettings->getGridDivisionsX(), canvasSettings->getGridDivisionsY());
    
    glUniform2f(texSizeLocation, _surface->getWidth(), _surface->getHeight());
    glUniform4f(lightColorLocation, 1,1,1,1);
    glUniform4f(darkColorLocation, 1.0f,1.0f,1.0f,0.3f);

    quad.draw();
}

void SketchPass::upload(Bounding area){
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, _surface->getWidth());
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, area.start.x);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, area.start.y);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        area.start.x,
        area.start.y,
        area.getWidth(),
        area.getHeight(),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _surface->getBuffer()
    );

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
}