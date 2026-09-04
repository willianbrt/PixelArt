import { app } from "./app.js"
import HandlerEvents from './handlerEvents.js'

const handlerEvents = HandlerEvents(document.querySelector("canvas#painting"));

const thickness = document.querySelector("input[name='size']");
const hardness = document.querySelector("input[name='strength']");
hardness.value = 100;
// checked
var pixelPerfect = document.querySelector("#pixel-perfect input[type='checkbox']");
var mirrorX = document.querySelector("#mirror-x input[type='checkbox']");
var mirrorY = document.querySelector("#mirror-y input[type='checkbox']");
var fill = document.querySelector("#fill input[type='checkbox']");


export function buildPaneToolBar(){
    const toolViewModel = app.paneToolViewModel();
    const drawingSettings = app.drawingSettingsVM();
    const symmetrySettings = app.symmetrySettingsVM();
    thickness.onchange = (e)=>{ drawingSettings.setSize(parseInt(e.srcElement.value)); }
    hardness.onchange = (e)=>{ drawingSettings.setHardness(parseFloat(e.srcElement.value) / 100.0); }
    mirrorX.onchange = (e)=>{ symmetrySettings.setMirrorX(e.srcElement.checked); }
    mirrorY.onchange = (e)=>{ symmetrySettings.setMirrorY(e.srcElement.checked); }
    fill.onchange = (e)=>{ console.log(e.srcElement.checked); }
    symmetrySettings.setNTileX(3);
    symmetrySettings.setNTileY(3);

    // thickness.onchange = (e)=>{ drawingSettings.setColor(parseInt(e.srcElement.value)); }


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

    // handlerEvents.setScrollEvent(onZoomScrollStrategy(), false);
    // handlerEvents.setDoubleTouchEvent(onZoomDoubleTouchStrategy(), false);
    // handlerEvents.setDoubleTouchEvent(onZoomDoubleTouchStrategy(), false);
    // handlerEvents.setScrollEvent(onSizeStrategy(), true);
    // handlerEvents.setGenericButtonMousePressedEvent(onPanningStrategy());
    // handlerEvents.setLeftButtonMousePressedEvent(eraseStrategy());
    // handlerEvents.setRightButtonMousePressedEvent(onPanningStrategy());
    // handlerEvents.setMoveEvent((x,y)=>{console.log("m",x,y)});

    // btnPencil.click();
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
