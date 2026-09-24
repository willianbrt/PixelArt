import { app } from "./app.js"
import {database} from "./repository.js"
import { modal, carousel } from "./elements.js"


const thickness = document.querySelector("input[name='size']");
const hardness = document.querySelector("input[name='strength']");
hardness.value = 100;
// checked
var pixelPerfect = document.querySelector("#pixel-perfect input[type='checkbox']");
var mirrorX = document.querySelector("#mirror-x input[type='checkbox']");
var mirrorY = document.querySelector("#mirror-y input[type='checkbox']");
var fill = document.querySelector("#fill input[type='checkbox']");

let modalBrush = modal({modalId: "modal-brush", triggerId: "btn-modal-brush"});
let shapePagination = carousel({
    id: "#shape-patterns",
    empty: ()=>{
        let elem = document.createElement("span");
        elem.className = "text";
        elem.innerText = "Nenhuma pincel encontrado.";
        return elem;
    },
    search: async (index,limit)=>{
        let table = await database.table("pattern");
        let data = await table.getAll(index*limit, limit);
        let size = await table.size();

        return {data, dataLength: size};
    },
    element: elemPagination
});

let customShapePagination = carousel({
    id: "#custom-patterns",
    empty: ()=>{
        let elem = document.createElement("span");
        elem.className = "text";
        elem.innerText = "Nenhum pincel encontrado.";
        return elem;
    },
    search: async (index,limit)=>{
        let table = await database.table("pattern");
        let data = await table.getAll(index*limit, limit);
        let size = await table.size();

        return {data, dataLength: size};
    },
    element: elemPagination
});
let maskPagination  = carousel({
    id: "#mask-patterns",
    empty: ()=>{
        let elem = document.createElement("span.text");
        elem.className = "text";
        elem.innerText = "Nenhuma máscara encontrada.";
        return elem;
    },
    search: async (index,limit)=>{
        let table = await database.table("pattern");
        let data = await table.getAll(index*limit, limit);
        let size = await table.size();

        return {data, dataLength: size};
    },
    element: elemPagination
});

export function buildPaneToolBar(){
    const toolViewModel = app.paneToolViewModel();
    const brushSettings = app.brushSettingsVM();
    const drawingSettings = app.drawingSettingsVM();
    const symmetrySettings = app.symmetrySettingsVM();
    thickness.onchange = (e)=>{ drawingSettings.setSize(parseInt(e.srcElement.value)); }
    hardness.onchange = (e)=>{ drawingSettings.setHardness(parseFloat(e.srcElement.value) / 100.0); }
    mirrorX.onchange = (e)=>{ symmetrySettings.setMirrorX(e.srcElement.checked); }
    mirrorY.onchange = (e)=>{ symmetrySettings.setMirrorY(e.srcElement.checked); }
    fill.onchange = (e)=>{ console.log(e.srcElement.checked); }
    
    // symmetrySettings.enabledTilingX(true);
    // symmetrySettings.enabledTilingY(true);
    // thickness.onchange = (e)=>{ drawingSetting   s.setColor(parseInt(e.srcElement.value)); }
    toolViewModel.setPressedTool("brush");

    const btnPencil = document.querySelector(".tool-pencil");
    btnPencil.addEventListener("click", function(e){
        toolViewModel.setPressedTool("brush");
        changeSelectTool.call(this);
    });
    const btnBrush = document.querySelector(".tool-brush");
    btnBrush.addEventListener("click", function(e){
        toolViewModel.setPressedTool("brush");
        changeSelectTool.call(this);
        console.log(brushSettings.getShape("line"))
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

function getPattern(jsPattern) {
    let cppPattern = new module.VectorVectorFloat();

    jsPattern.forEach(row => {
        let rowVec = new module.VectorFloat();
        row.forEach(v => rowVec.push_back(v*getWeight()));
        cppPattern.push_back(rowVec);
    });

    return cppPattern;
}


function elemPagination(data){
    const pageElementList = this;

    const imgData = new ImageData(new Uint8ClampedArray(data.buffer), data.width,data.height);
    const SIZE_DEST_CANVAS = {x:24, y: 24};
    let scale = Math.min(SIZE_DEST_CANVAS.x/data.width, SIZE_DEST_CANVAS.y/data.height);
    let resized = {x: data.width*scale, y: data.height*scale}
    let pan = {
        x: (SIZE_DEST_CANVAS.x - resized.x) / 2,
        y: (SIZE_DEST_CANVAS.y - resized.y) / 2
    };

    let offPattern = document.createElement("canvas");
    offPattern.width = data.width;
    offPattern.height = data.height;
    const offContext = offPattern.getContext("2d");
    offContext.putImageData(imgData, 0, 0);

    let canvasPattern = document.createElement("canvas");
    canvasPattern.width = SIZE_DEST_CANVAS.x;
    canvasPattern.height = SIZE_DEST_CANVAS.y;
    const context = canvasPattern.getContext("2d");
    context.imageSmoothingEnabled = false;
    context.drawImage(offPattern, pan.x, pan.y, resized.x,resized.y);

    let brushPattern = document.createElement("div");
    brushPattern.className = "brush-pattern";
    // if(activePattern == i)
    //     brushPattern.classList.add("active");

    brushPattern.append(canvasPattern);
    brushPattern.addEventListener("click", function(e){
        pageElementList.querySelector('.brush-pattern.active')?.classList.remove('active');
        this.classList.add("active");
    });
    return brushPattern;
}