import { app } from "./app.js"
import { modal, pagination } from "./elements.js"


let toolViewModel;
let brushSettings;
let drawingSettings;
let symmetrySettings;
let history;

const toolbar = {
    "drawing": [
        {
            name:"enable-pattern",
            value: false,
            label:"Pattern",
            type:"checkbox",
            icon: "texture",
            action: function(value){
                brushSettings.enablePattern(value);
                onUpdateBrush();
            }
        },
        {
            name:"solid-color",
            value: false,
            label:"Solid Color",
            type:"checkbox",
            icon: "format_color_fill",
            action: function(value){
                brushSettings.isSolidColor(value);
                onUpdateBrush();
            }
        },
        {
            name:"size",
            value:"",
            label:"Thinkeness",
            type:"number",
            unit:"px",
            min: 1, max:64,
            action: function(value){
                brushSettings.setSize(parseInt(value));
                onUpdateBrush();
            }
        },
        {
            name:"hardness",
            value:"",
            label:"Hardness",
            type:"number",
            min: 0, max:100,
            unit:"%",
            action: function(value){
                brushSettings.setHardness(parseFloat(value) / 100.0);
                onUpdateBrush();
            }
        }
    ],
    "tracking":[
        {
            name:"pixel-perfect",
            value: false,
            label:"Pixel Perfect",
            type:"checkbox",
            icon:"line_curve",
            action: function(value){
                brushSettings.setPixelPerfect(value);
                onUpdateBrush();
            }
        },
        {
            name:"spacing",
            value:"",
            label:"Spacing",
            type:"number",
            unit:"%",
            min: 0, max:100,
            action: function(value){
                brushSettings.setSpacing(parseInt(value));
                onUpdateBrush();
            }
        },
        {
            name:"stabilizer",
            value:"",
            label:"Stabilizer",
            type:"number",
            unit:"%",
            min: 0, max:100,
            action: function(value){
                brushSettings.setStabilizer(parseFloat(value) / 100.0);
                onUpdateBrush();
            }
        },
    ],
    "shape": [
        {
            name:"fill",
            value:"",
            label:"Fill",
            type:"checkbox",
            icon:"colors",
            action: function(value){
            }
        },
    ],
    "symmetry": [
        {
            name:"mirror-x",
            value: false,
            label:"Mirror X",
            type:"checkbox",
            icon:"flip",
            action: function(value){
                symmetrySettings.setMirrorX(value);
            }
        },
        {
            name:"mirror-y",
            value: false,
            label:"Mirror Y",
            type:"checkbox",
            icon:"flip",
            action: function(value){
                symmetrySettings.setMirrorY(value);
            }
        },
    ],

};

const toolList = {
    "brush":{
        icon: "../../../src/resource/img/pencil.png",
        toolbar:["drawing", "tracking", "symmetry"],
        onchange: ()=>{}
    },
    "eraser":{
        icon: "../../../src/resource/img/eraser.png",
        toolbar:["drawing", "tracking", "symmetry"],
        onchange: ()=>{}
    },
    "dropper":{
        icon: "../../../src/resource/img/dropper.png",
        toolbar:["dropper"],
        onchange: ()=>{}
    },
    "bucket":{
        icon: "../../../src/resource/img/bucket.png",
        toolbar:["drawing", "tracking", "symmetry"],
        onchange: ()=>{}
    },
    "line":{
        icon: "../../../src/resource/img/line.png",
        toolbar:["drawing", "symmetry"],
        onchange: ()=>{}
    },
    "square":{
        icon: "../../../src/resource/img/square.png",
        toolbar:["drawing", "shape", "symmetry"],
        onchange: ()=>{}
    },
    "circle":{
        icon: "../../../src/resource/img/circle.png",
        toolbar:["drawing", "shape", "symmetry"],
        onchange: ()=>{}
    },
    "select":{
        icon: "../../../src/resource/img/select.png",
        toolbar:["symmetry"],
        onchange: ()=>{
            createFloatingToolbar();
        }
    }
};
export async function buildPaneToolBar(){
    toolViewModel = app.paneToolViewModel();
    brushSettings = app.brushSettingsVM();
    drawingSettings = app.drawingSettingsVM();
    symmetrySettings = app.symmetrySettingsVM();

    buildModalBrush();

    const paneTool = document.querySelector("#pane-tool");
    const toolbarList = paneTool.querySelectorAll(".tool");
    toolbarList.forEach((e)=>{
        const onChangeTool = ()=>{
            const tool = toolList[e.dataset.tool];

            createToolOption(tool.toolbar);

            toolViewModel.setPressedTool(e.dataset.tool);

            paneTool.querySelector(".tool.active")?.classList.toggle("active", false);
            e.classList.toggle("active", true);

            tool.onchange();
        };

        e.onclick = onChangeTool;

        if(toolViewModel.getTool() == e.dataset.tool){
            e.classList.toggle("active", true);
            onChangeTool();
        }
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

async function buildModalBrush(){
    let patternRepository = await PatternRepository();
    
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


    let selectAlignMode = document.querySelector("#align-mode");
    selectAlignMode.onchange = (e)=>{ 
        brushSettings.setAlignMode(e.srcElement.value);
    };
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
function createToolOption(toolOptions) {
    const contentToolOptions = document.querySelector("#tool-options");
    contentToolOptions.innerHTML = "";
    
    let toolbarElem = document.createElement("div");
    toolbarElem.className = "tool-options";
    toolOptions.forEach((settings)=>{
        toolbar[settings].forEach(addTool);
    });
    
    contentToolOptions.appendChild(toolbarElem);
    
    function addTool({ id, name, icon, label, action, type, min, max, value, unit }) {
        let inputGroup = document.createElement("div");
        inputGroup.className = "input-group";
        
        let inputLabel = document.createElement("span");
        inputLabel.className = "input-label";
        inputLabel.innerText = label;
        
        const input = document.createElement("input");
        input.type = type;
        input.id = name;
        input.name = name;
        input.onchange = (e)=>{            
            action((input.type=="checkbox") ? input.checked : input.value);
        };

        if(type == "number" || type == "range"){
            input.min = min;
            input.max = max;
                
            let labelUnit = document.createElement("span");
            labelUnit.className = "input-label";
            labelUnit.innerText = unit;
            
            inputGroup.appendChild(inputLabel);
            inputGroup.appendChild(input);
            inputGroup.appendChild(labelUnit);
        }
        
        if(type == "checkbox"){
            let elemIcon = document.createElement("span");
            elemIcon.className = "material-symbols-outlined";
            elemIcon.innerText = icon;

            let labelCheckbox = document.createElement("label");
            labelCheckbox.className = "btn-choice";
            labelCheckbox.for = name;


            labelCheckbox.appendChild(elemIcon);
            labelCheckbox.appendChild(input);
            labelCheckbox.appendChild(inputLabel);
            inputGroup.appendChild(labelCheckbox);
        }


        toolbarElem.append(inputGroup);
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
        uniform int alignMode;
        uniform int fillMode;
        uniform int typeBrushTip;
        uniform bool isPatternEnable;
        uniform bool isSolidColor;
        
        out vec4 fragColor;
        
        vec4 getPatternPixel(sampler2D tex, vec2 texSize, float size){
            if(alignMode == 0 || alignMode == 1){ // BRUSHING  // LOCAL
                vec2 origemPattern = (resolution- texSize*size)*0.5;
                vec2 patternUV = fract((uv * resolution - origemPattern)/ (texSize*size));

                return texture(tex, patternUV);
            } else if(alignMode == 2){ // SOURCE
                return texture(tex, fract(uv / texSize));
            }
            return texture(tex, fract(uv / texSize));
        }

        void main(){
            float size = min(resolution.x/texShapeSize.x, resolution.y/texShapeSize.y);
            vec2 origem = (resolution- texShapeSize*size)*0.5;
            vec2 shapeUV = (uv * resolution - origem) / (texShapeSize * size);
            if(shapeUV.x < 0.0 || shapeUV.x >= 1.0 || shapeUV.y < 0.0 || shapeUV.y >= 1.0) {
                discard;
            }

            float shapeMask = texture(texShape, shapeUV).r*hardness;
            
            vec3 c = color.rgb;
            float patternMask = 1.0;
            if(isPatternEnable) {
                vec4 patternPixel;
                if(texPatternSize.x > 0.0 && texPatternSize.y > 0.0 ){
                    patternPixel = getPatternPixel(texPattern, texPatternSize, size);
                } else {
                    patternPixel = getPatternPixel(texShape, texShapeSize, size);
                }
                patternMask = patternPixel.r;

                if(!isSolidColor) {
                    c = patternPixel.agb;
                }
            } else{
                if(!isSolidColor && typeBrushTip != 0){
                    c = texture(texShape, shapeUV).agb;
                }
            }
            fragColor = vec4(c, shapeMask*patternMask);
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
    const alignModeLocation = gl.getUniformLocation(prog,"alignMode");
    const isSolidColorLocation = gl.getUniformLocation(prog,"isSolidColor");
    const isPatternEnableLocation = gl.getUniformLocation(prog,"isPatternEnable");
    const texPatternSizeLocation = gl.getUniformLocation(prog,"texPatternSize");
    const texShapeSizeLocation = gl.getUniformLocation(prog,"texShapeSize");
    const typeBrushTipLocation = gl.getUniformLocation(prog,"typeBrushTip");

    function draw(obj){
        texShape.bind();
        texShape.update(obj.shape);

        texPattern.bind();
        texPattern.update(obj.pattern);

        gl.uniform2f(resLocation, canvas.width,canvas.height);
        gl.uniform2f(texPatternSizeLocation, obj.pattern.width,obj.pattern.height);
        gl.uniform2f(texShapeSizeLocation, obj.shape.width,obj.shape.height);
        gl.uniform1f(hardnessLocation, obj.hardness/100.0);
        gl.uniform1i(fillModeLocation, obj.fillMode);
        gl.uniform1i(alignModeLocation, obj.alignMode);
        gl.uniform1f(isSolidColorLocation, obj.isSolidColor);
        gl.uniform1f(isPatternEnableLocation, obj.isPatternEnable);
        gl.uniform1i(typeBrushTipLocation, obj.typeBrushTip);

        const a = (obj.color & 0xFF) ;
        const b = (obj.color>>8 & 0xFF)
        const g = (obj.color>>16 & 0xFF);
        const r = (obj.color>>24 & 0xFF);
        gl.uniform4f(colorLocation, r,g,b,a);
            
        gl.clearColor(0, 0, 0, 0);
        gl.clear(gl.COLOR_BUFFER_BIT);

        gl.viewport(0, 0, canvas.width, canvas.height);

        gl.drawArrays(gl.TRIANGLES, 0, 8);
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

    const thickness = document.querySelector("input[name='size']");
    const hardness = document.querySelector("input[name='hardness']");
    const spacing = document.querySelector("input[name='spacing']");
    const stabilizer = document.querySelector("input[name='stabilizer']");
    const pixelPerfect = document.querySelector("input[name='pixel-perfect']");
    

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