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

void Renderer::initCursorHover(CursorContext* hover){
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

void Renderer::uploadCursorHover(CursorContext* hover){
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
    glUniform2f(cursorWorldLocation, viewport->getCursor().x, viewport->getCursor().y);
    glUniform1f(scaleLocation, canvasSettings->getScale());
    glUniform2f(texSizeLocation, surface->getWidth(), surface->getHeight());
    glUniform2f(repeatLocation, canvasSettings->getTilesX(), canvasSettings->getTilesY());
    glUniform2f(gridDivisionsLocation, canvasSettings->getGridDivisionsX(), canvasSettings->getGridDivisionsY());

    glUniform4f(lightColorLocation, 1,1,1,1);
    glUniform4f(darkColorLocation, 1.0f,1.0f,1.0f,0.3f);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Renderer::drawCursorHover(Surface* surface, CursorContext* hover, Viewport* viewport){
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
    glUniform2f(cursorWorldLocation, cursor.x, cursor.y);
    // glUniform2f(cursorLocation, cursor.x, cursor.y);
    glUniform2f(brushSizeLocation, hover->pattern->width, hover->pattern->height);

    float time = glfwGetTime();
    glUniform1f(glGetUniformLocation(programHover,"time"), time);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Renderer::createShader(){
    const char* vs =
    "precision highp float;"
    "attribute vec2 position;"
    "uniform vec2 resolution;"
    "varying vec2 uv;"
    "void main(){"
    " uv = position * 0.5 + 0.5;"
    " uv = vec2(uv.x, 1.0 - uv.y) * resolution;"
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
    "uniform vec2 repeat;"
    "uniform float time;"
    "uniform vec2 resolution;"
    "void main(){"
    "   vec4 color = vec4(0.0,0.0,0.0,0.0);"
    "   vec2 pixel = (uv - pan) / (zoom + repeat-1.0);"
    "   vec2 transformedUV = pixel / texSize;"
    "   vec2 thickness = 1.0/(zoom*texSize);"
    "   if(transformedUV.x < 0.0 || transformedUV.x > 1.0 || transformedUV.y<0.0 || transformedUV.y > 1.0) discard;"

    "   transformedUV = fract(transformedUV * repeat);"
    "   vec4 textureColor = texture2D(tex, transformedUV).abgr;"
    "   color = mix(color, textureColor, textureColor.a);"

    "   vec2 divisions = min(gridDivisions, texSize);"
    "   vec2 cellSize = texSize/divisions;"
    "   vec2 cellCoord = floor(pixel / cellSize);"

    "   float checker = mod(floor(cellCoord.x) + floor(cellCoord.y), 2.0);"
    "   vec4 background = mix(lightColor, darkColor, checker);"
    "   color = mix(background, textureColor, textureColor.a);"

    "   vec2 gridPos = mod(transformedUV, cellSize/texSize);"
    "   float line = step(gridPos.x, thickness.x) + step(gridPos.y, thickness.y);"
    "   line = clamp(line, 0.0, 1.0);"

    "   color = mix(color, vec4(0.0,0.0,0.0,1.0), line);"

    "   gl_FragColor = color;"
    "}";


    const char* fsHover =
    "precision highp float;"
    "uniform sampler2D tex;"
    "varying vec2 uv;"
    "uniform vec2 texSize;"
    "uniform vec2 cursor;"
    "uniform vec2 cursorWorld;"
    "uniform vec2 brushSize;"
    "uniform vec2 resolution;"
    "uniform float zoom;"
    "uniform vec2 pan;"
    "uniform vec2 tl;"
    "uniform vec2 bl;"
    "uniform vec2 tr;"
    "uniform vec2 br;"
    "uniform float time;"

    "float hasEdge(vec2 pixelUV){"
    "   float inside = step(0.0, pixelUV.x) * step(0.0, pixelUV.y) * step(pixelUV.x, 1.0) * step(pixelUV.y, 1.0);"
    "   return step(0.004, texture2D(tex, pixelUV).r)*inside;"
    "}"

    "void drawCornerResize(vec2 pixel, float size, float thickness){"
    "   pixel = pixel - size * 0.5;"

    "   float inner = step(pixel.x, uv.x) * step(pixel.y, uv.y) * step(uv.x, pixel.x + size) * step(uv.y, pixel.y + size);"
    "   float outer = step(pixel.x + thickness, uv.x) * step(pixel.y + thickness, uv.y) * step(uv.x, pixel.x + size - thickness) * step(uv.y, pixel.y + size - thickness);"
    "   float border =  inner - outer;"
    
    "   float hover = step(pixel.x, cursorWorld.x) * step(pixel.y, cursorWorld.y) * step(cursorWorld.x, pixel.x + size) * step(cursorWorld.y, pixel.y + size);"

    "   vec4 background = mix(vec4(0.87,0.411,0.0,1.0), vec4(0.0, 1.0, 0.0,1.0), hover);"
    "   gl_FragColor = mix(gl_FragColor, background, inner);"
    "   gl_FragColor = mix(gl_FragColor, vec4(0.203,0.105,0.015,1.0), border);"
    "}"
    

    "void drawCornerRotate(vec2 pixel, float size, float thickness){"
    "   float radius = size*0.5;"
    "   float r2 = radius*radius;"

    "   float radiusOuter = (radius-thickness);"
    "   float ro2 = radiusOuter*radiusOuter;"
    
    "   float sumOuter = 0.0;"
    "   float sum = 0.0;"

    "    float blur = 0.15;"
    "    vec2 offsets[4];"
    "    offsets[0] = vec2(-blur, -blur);"
    "    offsets[1] = vec2( blur, -blur);"
    "    offsets[2] = vec2(-blur,  blur);"
    "    offsets[3] = vec2( blur,  blur);"
        
    "    vec2 dh = cursorWorld - pixel;"
    "    float hover = step(dot(dh,dh), ro2);"

    "    for(int i = 0; i < 4; i++){"
    "        vec2 d = (uv + offsets[i]) - pixel;"
    "        sum += step(dot(d,d), r2);"
    "        sumOuter += step(dot(d,d), ro2);"
    "    }"
    "   float inside = sum / 4.0;"
    "   float outside = sumOuter / 4.0;"
    "   float border = inside - outside;"
    // #e06900
    // rgb(52, 27, 4)
    "   vec4 background = mix(vec4(0.87,0.411,0.0,1.0), vec4(0.0, 1.0, 0.0,1.0), hover);"
    "   gl_FragColor = mix(gl_FragColor, background, inside);"
    "   gl_FragColor = mix(gl_FragColor, vec4(0.203,0.105,0.015,1.0), border);"
    "}"
    "void computeCorner(){"
    "   float sizeResize = zoom*0.5;"
    "   float sizeRotate = zoom*0.5;"
    "   float thinkness = 2.0;"
    // "   drawCornerResize(tl * zoom);"
    // "   drawCornerResize(tr * zoom);"
    // "   drawCornerResize(bl * zoom);"
    // "   drawCornerResize(br * zoom);"
    "   drawCornerResize(vec2(0.0,0.0) * zoom, sizeResize, thinkness);"
    "   drawCornerResize(vec2(0.0,10.0) * zoom, sizeResize, thinkness);"
    "   drawCornerResize(vec2(10.0,10.0) * zoom, sizeResize, thinkness);"
    "   drawCornerResize(vec2(10.0,0.0) * zoom, sizeResize, thinkness);"
    "   float offset = 1.0;"

    "   drawCornerRotate((vec2(0.0,0.0)+offset) * zoom, sizeRotate, thinkness);"
    "   drawCornerRotate((vec2(0.0+offset, 10.0-offset)) * zoom, sizeRotate, thinkness);"
    "   drawCornerRotate((vec2(10.0-offset, 0.0+offset)) * zoom, sizeRotate, thinkness);"
    "   drawCornerRotate((vec2(10.0,10.0) - offset) * zoom, sizeRotate, thinkness);"
    "}"
    
    "void main(){"
    "   vec2 pixel = (uv - pan) / zoom;"
    "   vec2 transformedUV = pixel / texSize;"
    // "   if(transformedUV.x < (0.0 - lineSize.x) || transformedUV.x > (1.0 + lineSize.x) || transformedUV.y < (0.0 - lineSize.y) || transformedUV.y > (1.0 + offset.y)) discard;"

    "   vec2 lineSize = 1.0/(zoom*brushSize);"

    "   vec2 centered = floor(brushSize*0.5);"
    "   vec2 pixelHover = (pixel-cursor) + centered;"
    "   vec2 hoverUV = pixelHover / brushSize;"

    "   float mask = hasEdge(hoverUV);"
    "   gl_FragColor = mix(vec4(0.0), vec4(0.32,0.21,0.36,0.5), mask);"

    "   vec2 px = vec2(lineSize.x, 0.0);"
    "   vec2 py = vec2(0.0, lineSize.y);"

    "   float left  = hasEdge(hoverUV - px);"
    "   float down  = hasEdge(hoverUV - py);"

    "   float edge =  abs(mask - down) + abs(mask - left);"
    "   edge = clamp(edge, 0.0, 1.0);"

    "   float coord = uv.x + uv.y;"
    "   float speed = 1.0;"
    "   float pattern = step(0.5, fract(coord / zoom + time * speed));"
    "   vec3 antsColor = mix(vec3(1.0), vec3(0.0), pattern);"

    "   gl_FragColor = mix(gl_FragColor, vec4(antsColor, edge), edge);"

    "   computeCorner();"
    "}";

    const char* fsSelect =
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
    "   vec2 pixel = (uv - pan) / zoom;"
    "   vec2 transformedUV = pixel / texSize;"
    "   if(transformedUV.x < 0.0 || transformedUV.x > 1.0 || transformedUV.y < 0.0 || transformedUV.y > 1.0) discard;"

    "   vec2 pixelHover = (pixel-cursor) + floor(brushSize*0.5);"
    "   vec2 hoverUV = pixelHover / brushSize;"
    "   if(hoverUV.x < 0.0 || hoverUV.x >= 1.0 || hoverUV.y < 0.0 || hoverUV.y >= 1.0) discard;"

    "   vec4 color = texture2D(tex, hoverUV).abgr;"
    "   gl_FragColor = color;"
    "}";

    GLuint v = compile(GL_VERTEX_SHADER,vs);
    GLuint f = compile(GL_FRAGMENT_SHADER,fs);
    GLuint fh = compile(GL_FRAGMENT_SHADER,fsHover);
    GLuint fsl = compile(GL_FRAGMENT_SHADER,fsSelect);

    program = glCreateProgram();
    glAttachShader(program,v);
    glAttachShader(program,f);
    glLinkProgram(program);

    programHover = glCreateProgram();
    glAttachShader(programHover,v);
    glAttachShader(programHover,fh);
    glLinkProgram(programHover);

    programSelect = glCreateProgram();
    glAttachShader(programSelect,v);
    glAttachShader(programSelect,fsl);
    glLinkProgram(programSelect);
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
    cursorWorldLocation = glGetUniformLocation(programHover,"cursorWorld");

    glUseProgram(programSelect);
    posSelect = glGetAttribLocation(programSelect,"position");
    texLocationSelect = glGetUniformLocation(programSelect,"tex");
    resolutionLocationSelect = glGetUniformLocation(programSelect,"resolution");
    positionLocationSelect = glGetUniformLocation(programSelect,"pan");
    scaleLocationSelect = glGetUniformLocation(programSelect,"zoom");
    texSizeLocationSelect = glGetUniformLocation(programSelect,"texSize");
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