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

        brushPreview();
    }
    pixelPerfect.onchange = (e)=>{ brushSettings.setPixelPerfect(!brushSettings.getPixelPerfect()); }
    hardness.onchange = (e)=>{ brushSettings.setHardness(parseFloat(e.srcElement.value) / 100.0); }
    spacing.onchange = (e)=>{ brushSettings.setSpacing(parseInt(e.srcElement.value)); }
    stabilizer.onchange = (e)=>{ brushSettings.setStabilizer(parseFloat(e.srcElement.value) / 100.0); }
    mirrorX.onchange = (e)=>{ symmetrySettings.setMirrorX(e.srcElement.checked); }
    mirrorY.onchange = (e)=>{ symmetrySettings.setMirrorY(e.srcElement.checked); }
    fill.onchange = (e)=>{ console.log(e.srcElement.checked); }

    pixelPerfect.checked = brushSettings.getPixelPerfect(); 
    hardness.value = brushSettings.getHardness()*100;
    spacing.value = brushSettings.getSpacing();
    stabilizer.value = brushSettings.getStabilizer()*100;

    toolViewModel.setPressedTool("brush");
    brushPreview();

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
                brushPreview();

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
                    brushPreview();
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
                    brushPreview();
                }
            });
            return brush;
        }
    });





    let tabFill = document.querySelector("#tab-fill");
    let mode = {
        "solid": (e)=>{
            brushSettings.setFillMode("solid");

            let lastActivePane = tabFill.querySelector(".tab-select.active");
            lastActivePane?.classList.remove("active");
            
            let activePane = tabFill.querySelector("#pane-solid");
            activePane.classList.add("active");
        },
        "stamp": ()=>{
            brushSettings.setFillMode("stamp");

            let lastActivePane = tabFill.querySelector(".tab-select.active");
            lastActivePane?.classList.remove("active");
            
            let activePane = tabFill.querySelector("#pane-stamp");
            activePane.classList.add("active");
        },
        "pattern": ()=>{
            brushSettings.setFillMode("mask");

            let lastActivePane = tabFill.querySelector(".tab-select.active");
            lastActivePane?.classList.remove("active");

            let activePane = tabFill.querySelector("#pane-pattern");
            activePane.classList.add("active");
        },
    };

    let selectModeBrush = document.querySelector("#mode-brush");
    selectModeBrush.onchange = (e)=>{ mode[e.srcElement.value]() };
    mode[selectModeBrush.value]();


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
        buffer[i] = data.buffer[i+3];
        buffer[i+1] = data.buffer[i]; 
        buffer[i+2] = data.buffer[i+1]; 
        buffer[i+3] = data.buffer[i+2];
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



 function brushPreview(){
    const data = brushSettings.getBrush();
    let buffer = [];
    for(let i = 0; i < data.buffer.byteLength; i+=4){
        buffer[i]   = data.buffer[i+3];
        buffer[i+1] = data.buffer[i]; 
        buffer[i+2] = data.buffer[i+1]; 
        buffer[i+3] = data.buffer[i+2];
    }
    const imgData = new ImageData(new Uint8ClampedArray(buffer), data.width,data.height);


    let offPattern = document.createElement("canvas");
    offPattern.width = data.width;
    offPattern.height = data.height;
    const offContext = offPattern.getContext("2d");
    offContext.putImageData(imgData, 0, 0);

    
    let listPreviewCanvas = document.querySelectorAll(".brush-pattern-preview canvas");
    listPreviewCanvas.forEach((previewCanvas)=>{
        const transformActiveCanvas = fitContent({x: data.width, y: data.height}, {x: previewCanvas.width, y: previewCanvas.height }); 
        let ctxActiveCanvas = previewCanvas.getContext("2d");        
        ctxActiveCanvas.imageSmoothingEnabled = false;
        ctxActiveCanvas.drawImage(offPattern, transformActiveCanvas.pan.x, transformActiveCanvas.pan.y, transformActiveCanvas.resized.x,transformActiveCanvas.resized.y);
    });
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