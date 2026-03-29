#include "Renderer.h"

Renderer::Renderer(){
    createShader();
    createQuad();
    cache();
}

void Renderer::init(Surface* surface, Viewport* viewport){
    glGenTextures(1, &canvasTexture);
    glBindTexture(GL_TEXTURE_2D, canvasTexture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        surface->getWidth(),
        surface->getHeight(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        surface->getBuffer()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::uploadSurface(Bounding area, Surface* surface){
    glBindTexture(GL_TEXTURE_2D, canvasTexture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, surface->getWidth());
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
        surface->getBuffer()
    );

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
}

void Renderer::initCursorHover(HoverPreview* hover){
    glGenTextures(1, &canvasCursorHover);
    glBindTexture(GL_TEXTURE_2D, canvasCursorHover);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        hover->pattern->width,
        hover->pattern->height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        hover->pattern->buffer.data()
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::uploadCursorHover(HoverPreview* hover){
    glBindTexture(GL_TEXTURE_2D, canvasCursorHover);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        hover->pattern->width,
        hover->pattern->height,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        hover->pattern->buffer.data()
    );
}


void Renderer::draw(Surface* surface, Viewport* viewport){
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, canvasTexture);
    glUniform1i(texLocation, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);

    CanvasSettings* canvasSettings = viewport->getCanvasSettings();

    glUniform2f(resolutionLocation, viewport->getWidth(), viewport->getHeight());
    glUniform2f(positionLocation, canvasSettings->getSketchPosition().x, canvasSettings->getSketchPosition().y);
    glUniform1f(scaleLocation, canvasSettings->getScale());
    glUniform2f(texSizeLocation, surface->getWidth(), surface->getHeight());
    glUniform2f(repeatLocation, canvasSettings->getTilesX(), canvasSettings->getTilesY());
    glUniform2f(gridDivisionsLocation, canvasSettings->getGridDivisionsX(), canvasSettings->getGridDivisionsY());

    glUniform4f(lightColorLocation, 1,1,1,1);
    glUniform4f(darkColorLocation, 0.3f,0.3f,0.3f,1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Renderer::drawCursorHover(Surface* surface, HoverPreview* hover, Viewport* viewport){
    glUseProgram(programHover);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, canvasCursorHover);
    glUniform1i(texLocationH, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(posH);
    glVertexAttribPointer(posH, 2, GL_FLOAT, GL_FALSE, 0, 0);

    CanvasSettings* canvasSettings = viewport->getCanvasSettings();
    glUniform2f(resolutionLocationH, viewport->getWidth(), viewport->getHeight());
    glUniform2f(positionLocationH,  canvasSettings->getSketchPosition().x, canvasSettings->getSketchPosition().y);
    glUniform1f(scaleLocationH, canvasSettings->getScale());
    glUniform2f(texSizeLocationH, surface->getWidth(), surface->getHeight());

    Point cursor = viewport->getCursor();
    Point cursorCanvas = viewport->cursorToCanvas(cursor.x, cursor.y);

    glUniform2f(cursorLocation, cursorCanvas.x, cursorCanvas.y);
    glUniform2f(brushSizeLocation, hover->pattern->width, hover->pattern->height);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Renderer::createShader(){
    const char* vs =
    "precision highp float;"
    "attribute vec2 position;"
    "varying vec2 uv;"
    "void main(){"
    " uv = position * 0.5 + 0.5;"
    " uv = vec2(uv.x, 1.0 - uv.y);"
    " gl_Position = vec4(position,0.0,1.0);"
    "}";

    const char* fs =
    "precision highp float;"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "uniform vec2 texSize;"
    "uniform vec2 cursor;"
    "uniform vec2 brushSize;"
    "uniform vec2 gridDivisions;"
    "uniform vec4 lightColor;"
    "uniform vec4 darkColor;"
    "uniform float zoom;"
    "uniform vec2 pan;"
    "uniform vec2 resolution;"
    "uniform vec2 repeat;"
    "void main(){"
    "   vec4 color = vec4(0.0,0.0,0.0,0.0);"
    "   vec2 transformedUV = uv * resolution/texSize;"
    "   transformedUV -= pan / texSize;"
    "   transformedUV /= (zoom + repeat-1.0);"
    "   if (transformedUV.x >= 0.0 && transformedUV.x <= 1.0 && transformedUV.y >= 0.0 && transformedUV.y <= 1.0) {"
    "       transformedUV = fract(transformedUV * repeat);"
    "       vec4 textureColor = texture2D(tex, transformedUV).abgr;"
    "       color = mix(color, textureColor, textureColor.a);"
    
    "       vec2 divisions = min(gridDivisions, texSize);"
    "       vec2 pixel = transformedUV * texSize;"
    "       vec2 cellSize = texSize / divisions;"
    "       vec2 cellCoord = floor(pixel / cellSize);"
    
    "       float checker = mod(cellCoord.x + cellCoord.y, 2.0);"
    "       vec4 background = mix(lightColor, darkColor, checker);"
    "       color = mix(background, textureColor, textureColor.a);"
    
    "       vec2 gridPos = mod(pixel, cellSize);"
    "       float thickness = 0.01*zoom;"
    "       float line = step(gridPos.x, thickness) + step(gridPos.y, thickness);"
    "       line = clamp(line, 0.0, 1.0);"
    "       color = mix(color, vec4(0.0), line);"
    "   }"
    
    "   gl_FragColor = color;"
    "}";


    const char* fsHover =
    "precision highp float;"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "uniform vec2 texSize;"
    "uniform vec2 cursor;"
    "uniform vec2 brushSize;"
    "uniform float zoom;"
    "uniform vec2 pan;"
    "uniform vec2 resolution;"
    "void main(){"
    " vec2 transformedUV = uv * resolution/texSize;"
    "   transformedUV -= pan / texSize;"
    "   transformedUV /= zoom;"
    "   if(transformedUV.x < 0.0 || transformedUV.x > 1.0 || transformedUV.y<0.0 || transformedUV.y > 1.0) discard;"
    "   vec2 pixel = transformedUV * texSize;"
    "   vec2 hoverUV = (floor(pixel)-cursor)/brushSize + 0.5;"
    "   if(hoverUV.x < 0.0 || hoverUV.x >= 1.0 || hoverUV.y < 0.0 || hoverUV.y >= 1.0) discard;"
    "   vec4 color = texture2D(tex, hoverUV).abgr;"
    "   gl_FragColor = color;"
    "}";

    GLuint v = compile(GL_VERTEX_SHADER,vs);
    GLuint f = compile(GL_FRAGMENT_SHADER,fs);
    GLuint fh = compile(GL_FRAGMENT_SHADER,fsHover);

    program = glCreateProgram();
    glAttachShader(program,v);
    glAttachShader(program,f);
    glLinkProgram(program);

    programHover = glCreateProgram();
    glAttachShader(programHover,v);
    glAttachShader(programHover,fh);
    glLinkProgram(programHover);    
}

void Renderer::cache(){
    glUseProgram(program);
    pos = glGetAttribLocation(program,"position");
    texLocation = glGetUniformLocation(program,"tex");
    resolutionLocation = glGetUniformLocation(program,"resolution");
    positionLocation = glGetUniformLocation(program,"pan");
    scaleLocation = glGetUniformLocation(program,"zoom");
    texSizeLocation = glGetUniformLocation(program,"texSize");
    repeatLocation = glGetUniformLocation(program,"repeat");
    gridDivisionsLocation = glGetUniformLocation(program,"gridDivisions");
    lightColorLocation = glGetUniformLocation(program,"lightColor");
    darkColorLocation = glGetUniformLocation(program,"darkColor");

    glUseProgram(programHover);
    posH = glGetAttribLocation(programHover,"position");
    texLocationH = glGetUniformLocation(programHover,"tex");
    resolutionLocationH = glGetUniformLocation(programHover,"resolution");
    positionLocationH = glGetUniformLocation(programHover,"pan");
    scaleLocationH = glGetUniformLocation(programHover,"zoom");
    texSizeLocationH = glGetUniformLocation(programHover,"texSize");
    cursorLocation = glGetUniformLocation(programHover,"cursor");
    brushSizeLocation = glGetUniformLocation(programHover,"brushSize");
}

void Renderer::createQuad(){
    float quad[] = {
        -1,  1,
         1,  1,
        -1, -1,
         1, -1
    };

    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(quad),quad,GL_STATIC_DRAW);
}

GLuint Renderer::compile(GLenum type,const char* src){
    GLuint s = glCreateShader(type);
    glShaderSource(s,1,&src,nullptr);
    glCompileShader(s);

    GLint ok;
    glGetShaderiv(s,GL_COMPILE_STATUS,&ok);

    if(!ok){
        char log[512];
        glGetShaderInfoLog(s,512,nullptr,log);
        printf("Shader error: %s\n",log);
    }

    return s;
}