import { app } from "./app.js"
import { modal, pagination } from "./elements.js"


const thickness = document.querySelector("input[name='size']");
const hardness = document.querySelector("input[name='hardness']");
const spacing = document.querySelector("input[name='spacing']");
const stabilizer = document.querySelector("input[name='stabilizer']");

// checked
var pixelPerfect = document.querySelector("#pixel-perfect input[type='checkbox']");
var mirrorX = document.querySelector("#mirror-x input[type='checkbox']");
var mirrorY = document.querySelector("#mirror-y input[type='checkbox']");
var fill = document.querySelector("#fill input[type='checkbox']");
var enablePattern = document.querySelector("#enable-pattern ");

let toolViewModel;
let brushSettings;
let drawingSettings;
let symmetrySettings;

export async function buildPaneToolBar(){
    toolViewModel = app.paneToolViewModel();
    brushSettings = app.brushSettingsVM();
    drawingSettings = app.drawingSettingsVM();
    symmetrySettings = app.symmetrySettingsVM();

    let patternRepository = await PatternRepository();
    
    thickness.onchange = async (e)=>{
        brushSettings.setSize(parseInt(e.srcElement.value));
        onUpdateBrush();
    }
    pixelPerfect.onchange = (e)=>{
        brushSettings.setPixelPerfect(!brushSettings.getPixelPerfect());
        onUpdateBrush();
    }
    hardness.onchange = (e)=>{
        brushSettings.setHardness(parseFloat(e.srcElement.value) / 100.0);
        onUpdateBrush();
    }
    spacing.onchange = (e)=>{
        brushSettings.setSpacing(parseInt(e.srcElement.value));
        onUpdateBrush();
    }
    stabilizer.onchange = (e)=>{
        brushSettings.setStabilizer(parseFloat(e.srcElement.value) / 100.0);
        onUpdateBrush();
    }
    enablePattern.onchange = (e)=>{
        brushSettings.enablePattern(e.srcElement.checked);
        onUpdateBrush();
    }
    mirrorX.onchange = (e)=>{
        symmetrySettings.setMirrorX(e.srcElement.checked);
        onUpdateBrush();
    }
    mirrorY.onchange = (e)=>{
        symmetrySettings.setMirrorY(e.srcElement.checked);
        onUpdateBrush();
    }
    fill.onchange = (e)=>{
    }

    toolViewModel.setPressedTool("brush");
    onUpdateBrush();

    var modalBrush = modal({modalId: "modal-brush", triggerId: "btn-modal-brush"});
    var shapePagination = pagination({
        id: "#shape-patterns",
        data: {
            data: [
                {brushTip: "square", icon: "brush_tip_square.png"},
                {brushTip: "circle", icon: "brush_tip_circle.png"},
                {brushTip: "line", icon: "brush_tip_line.png"}
            ],
            dataLength: 3,
        },
        element: function(data) {
            const pageElementList = this;
            let brushPattern = document.createElement("div");
            brushPattern.className = "brush-pattern";

            let brushView = document.createElement("img");
            brushView.className = "material-symbols-outlined";
            brushView.src = "./../../resource/img/" + data.icon;

            brushPattern.append(brushView);
            brushPattern.addEventListener("click", function(e){
                brushSettings.setShape(data.brushTip, 0xFF00FFFF);
                onUpdateBrush();

                pageElementList.querySelector('.brush-pattern.active')?.classList.remove('active');
                this.classList.add("active");
            });

            return brushPattern;
        }
    });

    var customShapePagination = pagination({
        id: "#custom-patterns",
        empty: ()=>{
            let elem = document.createElement("span");
            elem.className = "text";
            elem.innerText = "Nenhum pincel encontrado.";
            return elem;
        },
        search: async (index,limit)=>{
            return {
                data: await patternRepository.getAll(index*limit, limit),
                dataLength: await patternRepository.getSize()
            };
        },
        element: function(data){
            const brush = elemPagination("custom-patterns", {
                data,
                onchange: ()=>{
                    const patternPtr = app.malloc(data.buffer);
                    const size = data.buffer.byteLength/4;
                    brushSettings.setCustomShape(patternPtr, size, data.width);
                    onUpdateBrush();
                }
            });
            return brush;
        }
    });
    var maskPagination  = pagination({
        id: "#mask-patterns",
        empty: ()=>{
            let elem = document.createElement("span.text");
            elem.className = "text";
            elem.innerText = "Nenhuma máscara encontrada.";
            return elem;
        },
        search: async (index,limit)=>{
            return {
                data: await patternRepository.getAll(index*limit, limit),
                dataLength: await patternRepository.getSize()
            };
        },
        element: function (data){
            const brush = elemPagination("mask-patterns", {
                data,
                onchange: ()=>{
                    const patternPtr = app.malloc(data.buffer);
                    const size = data.buffer.byteLength/4;
                    brushSettings.setPattern(patternPtr, size, data.width);
                    onUpdateBrush();
                }
            });
            return brush;
        }
    });


    let mode = {
        "solid": (e)=>{
            brushSettings.setFillMode("solid");
            onUpdateBrush();
        },
        "stamp": ()=>{
            brushSettings.setFillMode("stamp");
            onUpdateBrush();
        },
        "preserve-opacity": ()=>{
            brushSettings.setFillMode("preserve-opacity");
            onUpdateBrush();
        },
    };

    let selectModeBrush = document.querySelector("#mode-brush");
    selectModeBrush.onchange = (e)=>{ mode[e.srcElement.value]() };


    let selectAlignMode = document.querySelector("#align-mode");
    selectAlignMode.onchange = (e)=>{ 
        brushSettings.setAlignMode(e.srcElement.value);
    };
    

    const btnPencil = document.querySelector(".tool-pencil");
    btnPencil.addEventListener("click", function(e){
        toolViewModel.setPressedTool("brush");
        changeSelectTool.call(this);
    });
    const btnBrush = document.querySelector(".tool-brush");
    btnBrush.addEventListener("click", function(e){
        toolViewModel.setPressedTool("brush");
        changeSelectTool.call(this);
    });
    const btnEraser = document.querySelector(".tool-eraser");
    btnEraser.addEventListener("click", function(e){
        toolViewModel.setPressedTool("eraser");
        changeSelectTool.call(this);
    });
    const btnDropper = document.querySelector(".tool-dropper");
    btnDropper.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(dropperStrategy());
        changeSelectTool.call(this);
    });

    const btnLine = document.querySelector(".tool-line");
    btnLine.addEventListener("click", function(e){
        toolViewModel.setPressedTool("line");
        changeSelectTool.call(this);
    });
    const btnSquare = document.querySelector(".tool-square");
    btnSquare.addEventListener("click", function(e){
        toolViewModel.setPressedTool("square");
        changeSelectTool.call(this);
    });

    const btnCircle = document.querySelector(".tool-circle");
    btnCircle.addEventListener("click", function(e){
        toolViewModel.setPressedTool("circle");
        changeSelectTool.call(this);
    });

    const btnBucket = document.querySelector(".tool-bucket");
    btnBucket.addEventListener("click", function(e){
        toolViewModel.setPressedTool("bucket");
        changeSelectTool.call(this);
    });

    const btnSelect = document.querySelector(".tool-select");
    btnSelect.addEventListener("click", function(e){
        toolViewModel.setPressedTool("select");
        changeSelectTool.call(this);
        createFloatingToolbar();
    });

    const btnUndo = document.querySelector("#undo");
    btnUndo.addEventListener("click", function(e){
        history.undo();
    });

    const btnRedo = document.querySelector("#redo");
    btnRedo.addEventListener("click", function(e){
        history.redo();
    });
}

function changeSelectTool(){
    document.querySelector(".tool.active")?.classList.toggle("active", false);
    this.classList.toggle("active", true);
}

function createFloatingToolbar() {
    document.querySelector(".floating-toolbar")?.remove();
    let toolbar = document.createElement("div");
    toolbar.className = "floating-toolbar";

    const grip = document.createElement("div");
    grip.className = "grip";

    const gripIcon = document.createElement("span");
    gripIcon.className = "material-symbols-outlined";
    gripIcon.textContent = "drag_indicator";

    grip.appendChild(gripIcon);

    const content = document.createElement("div");
    content.className = "floating-toolbar-content";

    const buttons = [
        {
            id: "resize",
            icon: "crop",
            label: "Cortar",
            eventClick: function(){
                toolbar.querySelectorAll("button")
                        .forEach((e)=> e.classList.remove("active"));
                this.classList.add("active");
            }
        },
        {
            id: "copy",
            icon: "content_copy",
            label: "Copiar",
        },
        {
            id: "paste",
            icon: "content_paste",
            label: "Colar",
        },
        {
            id: "tile-brush",
            icon: "brush",
            label: "Pincel",
            eventClick: function(){
            }
        }
    ];

    buttons.forEach(addTool);

    toolbar.appendChild(grip);
    toolbar.appendChild(content);

    document.querySelector("#drawing-area").appendChild(toolbar);
    function addTool({ id, icon, label, eventClick }) {
        const btn = document.createElement("button");

        btn.id = id;
        btn.classList.add("select-tool");
        btn.addEventListener("click", eventClick);
        btn.addEventListener("touchstart", eventClick);

        const span = document.createElement("span");
        span.className = "material-symbols-outlined";
        span.textContent = icon;

        btn.appendChild(span);
        btn.append(label);

        content.appendChild(btn);
    }
}


function elemPagination(parent, options){
    const { data, canvasWidth = 24, canvasHeight = 24, onchange } = options;

    const transform = fitContent({x: data.width, y: data.height}, {x: canvasWidth, y: canvasWidth});
    
    let buffer = [];
    for(let i = 0; i < data.buffer.byteLength; i+=4){

        buffer[i]   = data.buffer[i+1];
        buffer[i+1] = data.buffer[i+2]; 
        buffer[i+2] = data.buffer[i+3];
        buffer[i+3] = data.buffer[i];
    }
    
    const imgData = new ImageData(new Uint8ClampedArray(buffer), data.width,data.height);

    let offPattern = document.createElement("canvas");
    offPattern.width = data.width;
    offPattern.height = data.height;
    const offContext = offPattern.getContext("2d");
    offContext.putImageData(imgData, 0, 0);

    let canvasPattern = document.createElement("canvas");
    canvasPattern.width = canvasWidth;
    canvasPattern.height = canvasHeight;
    const context = canvasPattern.getContext("2d");
    context.imageSmoothingEnabled = false;
    context.clearRect(0,0, canvasPattern.width, canvasPattern.height);
    context.drawImage(offPattern, transform.pan.x, transform.pan.y, transform.resized.x, transform.resized.y);

    let brushPattern = document.createElement("div");
    brushPattern.className = "brush-pattern";

    brushPattern.append(canvasPattern);
    brushPattern.addEventListener("click", function(){
        onchange(data);
        
        const pageElementList = document.getElementById(parent);
        pageElementList.querySelector('.brush-pattern.active')?.classList.remove('active');
        this.classList.add("active");
    });

    return brushPattern;
}


function brushPreviewRender(canvas){
    const gl = canvas.getContext("webgl2");

    const vs = `#version 300 es
    in vec2 pos;
    out vec2 uv;

    void main() {
        uv = pos * 0.5 + 0.5;
        uv = vec2(uv.x, 1.0 - uv.y);
        gl_Position = vec4(pos, 0.0, 1.0);
    }`;
    const fs = `#version 300 es
        precision highp float;
        uniform sampler2D texPattern;
        uniform sampler2D texShape;
        in vec2 uv;
        in vec2 pixel;
        
        uniform vec2 resolution;
        uniform vec2 texPatternSize;
        uniform vec2 texShapeSize;
        uniform vec2 brushSize;
        uniform vec4 color;
        uniform float hardness;
        uniform int fillMode;
        
        out vec4 fragColor;
        void main(){
            float size = min(resolution.x/texShapeSize.x, resolution.y/texShapeSize.y);
            vec2 origem = (resolution- texShapeSize*size)*0.5;
            vec2 shapeUV = (uv * resolution - origem) / (texShapeSize * size);
            if(shapeUV.x < 0.0 || shapeUV.x >= 1.0 || shapeUV.y < 0.0 || shapeUV.y >= 1.0) {
                discard;
            }

            float shapeMask = texture(texShape, shapeUV).r*hardness;

            vec3 c;
            if(fillMode == 0){
                c = vec3(color.r,color.g,color.b);
                fragColor = vec4(c, shapeMask);
            }
            else if(fillMode == 1) {
            
                vec2 origemPattern = (resolution- texPatternSize*size)*0.5;
                vec2 patternUV = fract((uv * resolution - origem)/ (texPatternSize*size));

                float patternMask = texture(texPattern, patternUV).r;

                // c = vec3(color.r,color.g,color.b);
                // c = texture(texPattern, patternUV).agb;
                c = vec3(texture(texPattern, patternUV).b,0.0,0.0);
                fragColor = vec4(c, shapeMask*patternMask);
            }
            else if(fillMode == 2) {
                c = texture(texShape, shapeUV).agb;
                fragColor = vec4(c, shapeMask);
            }
        }`;

    function shader(type, src){
        let s = gl.createShader(type);
        gl.shaderSource(s, src);
        gl.compileShader(s);
        return s;
    }
    
    function texture(id, name){
        const tex = gl.createTexture();
        const location = gl.getUniformLocation(prog, name);

        return {
            update: (surface)=>{
                if(!surface.buffer) return;

                gl.activeTexture(gl.TEXTURE0 + id);
                gl.bindTexture(gl.TEXTURE_2D, tex);

                gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
                gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

                gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
                gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

            
                gl.texImage2D(
                    gl.TEXTURE_2D,
                    0,
                    gl.RGBA,
                    surface.width,
                    surface.height,
                    0,
                    gl.RGBA,
                    gl.UNSIGNED_BYTE,
                    surface.buffer
                );
            },
            bind: () => {
                gl.activeTexture(gl.TEXTURE0 + id);
                gl.bindTexture(gl.TEXTURE_2D, tex);

                gl.uniform1i(location, id);
            }
        }
    }
    
    let prog = gl.createProgram();
    gl.attachShader(prog, shader(gl.VERTEX_SHADER, vs));
    gl.attachShader(prog, shader(gl.FRAGMENT_SHADER, fs));
    gl.linkProgram(prog);
    gl.useProgram(prog);
    
    
    const buf = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buf);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
        -1,-1,  1,-1,  -1,1,
        -1,1,   1,-1,   1,1
    ]), gl.STATIC_DRAW);


    
    const loc = gl.getAttribLocation(prog, "pos");
    gl.enableVertexAttribArray(loc);
    gl.vertexAttribPointer(loc, 2, gl.FLOAT, false, 0, 0);
    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
    
    gl.viewport(0,0,canvas.width,canvas.height);

    const texShape = texture(0, "texShape");
    const texPattern = texture(1, "texPattern");
    
    const resLocation = gl.getUniformLocation(prog,"resolution");
    const colorLocation = gl.getUniformLocation(prog,"color");
    const hardnessLocation = gl.getUniformLocation(prog,"hardness");
    const fillModeLocation = gl.getUniformLocation(prog,"fillMode");
    const texPatternSizeLocation = gl.getUniformLocation(prog,"texPatternSize");
    const texShapeSizeLocation = gl.getUniformLocation(prog,"texShapeSize");

    function draw(obj){
        console.log(obj.fillMode)
        gl.uniform2f(resLocation, canvas.width,canvas.height);
        gl.uniform2f(texPatternSizeLocation, obj.pattern.width,obj.pattern.height);
        gl.uniform2f(texShapeSizeLocation, obj.shape.width,obj.shape.height);
        gl.uniform1f(hardnessLocation, obj.hardness/100.0);
        gl.uniform1i(fillModeLocation, obj.fillMode);

        const a = (obj.color & 0xFF) ;
        const b = (obj.color>>8 & 0xFF)
        const g = (obj.color>>16 & 0xFF);
        const r = (obj.color>>24 & 0xFF);
        gl.uniform4f(colorLocation, r,g,b,a);

        texShape.bind();
        texShape.update(obj.shape);
        texPattern.bind();
        texPattern.update(obj.pattern);

        gl.drawArrays(gl.TRIANGLES, 0, 6);
    }
    return {draw};
}



var renderPreview = [];
let listPreviewCanvas = document.querySelectorAll(".brush-pattern-preview canvas");
listPreviewCanvas.forEach((previewCanvas)=>{
    renderPreview.push(brushPreviewRender(previewCanvas));
});

function onUpdateBrush(){
    const brush = brushSettings.getBrush();

    pixelPerfect.checked = brush.pixelPerfect; 
    hardness.value = brush.hardness;
    spacing.value = brush.spacing;
    stabilizer.value = brush.stabilizer;
    thickness.value = brush.size;
    
    renderPreview.forEach((render)=>{ render.draw(brush); });
}
function fitContent(source, dest){
    let scale = Math.min(dest.x/source.x, dest.y/source.y);
    let resized = {x: source.x*scale, y: source.y*scale};
    let pan = {
        x: (dest.x - resized.x) / 2,
        y: (dest.y - resized.y) / 2
    }
    return {
        scale,
        resized,
        pan
    }
}

async function PatternRepository(){
    let table = await app.database.table("pattern");

    let pattern = {
        put: async function(id, buffer, width, height, is_system){
            return await table.put({id, buffer, width, height, is_system});
        },
        remove: async function(id){
            const elem = await table.findBy("id", parseInt(id));
            if(elem.is_system)
                throw { source: "[PatternFactory]", description: "Object of System."};

            table.delete(elem.id);
        },
        get: async function(id){
            return await table.getById(parseInt(id));
        },
        getAll: async (index, limit)=>{ return await table.getAll(index*limit, limit); },
        getSize: async ()=>{ return await table.size(); }
    };

    try{
        pattern.put(1, new Uint8Array([154,0,0,0, 255,0,0,0, 90,0,0,0, 255,0,0,0, 154,0,0,0, 154,0,0,0]), 2, 3, true);
        pattern.put(2, new Uint8Array([154,0,255,33, 255,0,255,35, 90,0,255,10, 255,0,255,10, 154,0,255,10, 154,0,255,10]), 2, 3, true);

    } catch(e){
        console.error(e);
    }

    return pattern;
}