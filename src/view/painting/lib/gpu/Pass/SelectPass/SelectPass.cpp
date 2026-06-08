#include "./SelectPass.h"

SelectPass::SelectPass(EditorManager* manager, ViewportContext* viewport){
    _manager = manager;
    _viewport = viewport;

    const char* fs = 
        R"(#version 300 es
        precision highp float;
        uniform sampler2D texs;
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
        uniform bool enabled;
        uniform vec2 selectSize;
        uniform vec2 resizeHandle[4];
        uniform vec2 rotateHandle[4];

        out vec4 fragColor;

        float hasEdge(vec2 pixelUV){
           float inside = step(0.0, pixelUV.x) * step(0.0, pixelUV.y) * step(pixelUV.x, 1.0) * step(pixelUV.y, 1.0);
           return step(0.003, texture(texs, pixelUV).r)*inside;
        }

        vec2 hyp(vec2 point, vec2 center){
           vec2 dir = center - point;
           float dist = length(dir);
           return vec2(dir / dist);
        }

        void drawCornerResize(vec2 pixel, float size, float thickness){
           pixel = pixel - size * 0.5;

           float inner = step(pixel.x, uv.x) * step(pixel.y, uv.y) * step(uv.x, pixel.x + size) * step(uv.y, pixel.y + size);
           float outer = step(pixel.x + thickness, uv.x) * step(pixel.y + thickness, uv.y) * step(uv.x, pixel.x + size - thickness) * step(uv.y, pixel.y + size - thickness);
           float border =  inner - outer;

           float hover = step(pixel.x, cursor.x) * step(pixel.y, cursor.y) * step(cursor.x, pixel.x + size) * step(cursor.y, pixel.y + size);

           vec4 background = mix(vec4(0.87,0.411,0.0,1.0), vec4(0.0, 1.0, 0.0,1.0), hover);
           fragColor = mix(fragColor, background, inner);
           fragColor = mix(fragColor, vec4(0.203,0.105,0.015,1.0), border);
        }

        void drawCornerRotate(vec2 pixel, float size, float thickness){
           float radius = size*0.5;
           float r2 = radius*radius;

           float radiusOuter = (radius-thickness);
           float ro2 = radiusOuter*radiusOuter;

           float sumOuter = 0.0;
           float sum = 0.0;

            float blur = 0.15;
            vec2 offsets[4];
            offsets[0] = vec2(-blur, -blur);
            offsets[1] = vec2( blur, -blur);
            offsets[2] = vec2(-blur,  blur);
            offsets[3] = vec2( blur,  blur);

            vec2 dh = cursor - pixel;
            float hover = step(dot(dh,dh), ro2);

            for(int i = 0; i < 4; i++){
                vec2 d = (uv + offsets[i]) - pixel;
                sum += step(dot(d,d), r2);
                sumOuter += step(dot(d,d), ro2);
            }
           float inside = sum / 4.0;
           float outside = sumOuter / 4.0;
           float border = inside - outside;

           vec4 background = mix(vec4(0.87,0.411,0.0,1.0), vec4(0.0, 1.0, 0.0,1.0), hover);
           fragColor = mix(fragColor, background, inside);
           fragColor = mix(fragColor, vec4(0.203,0.105,0.015,1.0), border);
        }
        void computeCorner(){
           float sizeResize = zoom.x*0.5;
           float sizeRotate = zoom.x*0.5;
           float thinkness = 2.0;

           for(int i = 0; i < 4; i++){
               drawCornerResize(resizeHandle[i], sizeResize, thinkness);
           }

           for(int i = 0; i < 4; i++){
               drawCornerRotate(rotateHandle[i], sizeRotate, thinkness);
           }

        }

        void main(){
           if(!enabled) discard;
           vec2 transformedUV = pixel / texSize;

           vec2 lineSize = 1.0/(zoom*selectSize);

           float mask = hasEdge(transformedUV);
           fragColor = mix(vec4(0.0), vec4(0.32,0.21,0.36,0.5), mask);

           vec2 px = vec2(lineSize.x, 0.0);
           vec2 py = vec2(0.0, lineSize.y);

           float left  = hasEdge(transformedUV - px);
           float down  = hasEdge(transformedUV - py);

           float edge =  abs(mask - down) + abs(mask - left);
           edge = clamp(edge, 0.0, 1.0);

           float coord = uv.x + uv.y;
           float speed = 1.0;
           float pattern = step(0.5, fract(coord / zoom.x + time * speed));
           vec3 antsColor = mix(vec3(1.0), vec3(0.0), pattern);

           fragColor = mix(fragColor, vec4(antsColor, edge), edge);

           computeCorner();
        })";
    

    

    
    quad.create();
    shader.create(fs);
    shader.use();

    // texture = glGetUniformLocation(shader.id(),"texs");
    texSizeLocation = glGetUniformLocation(shader.id(),"texSize");
    selectSizeLocation = glGetUniformLocation(shader.id(),"selectSize");
    enabledLocation = glGetUniformLocation(shader.id(),"enabled");
}
SelectPass::~SelectPass(){}

void SelectPass::init(){
    glUniformBlockBinding(shader.id(),  glGetUniformBlockIndex(shader.id(),  "GlobalData"),  0);
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        editor->getWidth(),
        editor->getHeight(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        preview
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    initialized = true;
}
void SelectPass::draw(){
    editor = _manager->getActiveEditor();
    if(!editor) return;
    if(!editor->getSurface()) return;
    
    if(preview != editor->preview()->getBuffer()){
        preview = editor->preview()->getBuffer();
        init();
    }

    SelectContext* select = editor->getSelectContext();    

    Bounding area = editor->preview()->getDirtyArea();
    if(area.start.x != INT_MAX && area.start.y != INT_MAX &&
        area.end.x != INT_MIN && area.end.y != INT_MIN){
        upload(area);
    }
    
    shader.use();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader.id(), "texs"), 1);

    quad.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

       
    CanvasSettings* canvasSettings = editor->getCanvasSettings();
    glUniform2fv(glGetUniformLocation(shader.id(),"resizeHandle[0]"), 4, select->getAllHandle(canvasSettings).data());
    glUniform2fv(glGetUniformLocation(shader.id(),"rotateHandle[0]"), 4, select->getAllRotateHandle(canvasSettings).data());
    glUniform2f(selectSizeLocation, editor->getWidth(), editor->getHeight());
    
    std::array<float, 8> r = select->getAllHandle(canvasSettings);
    glUniform2f(texSizeLocation, editor->getWidth(), editor->getHeight());
    glUniform1i(enabledLocation, select->enabled);

    quad.draw();
}
void SelectPass::upload(Bounding area){
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, editor->getWidth());
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
        preview
    );

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
}