import Module from '../build/graphics/PixelEditor.js';
import ModulePixelEditor from '../build/graphics/PixelEditor.js'
import HandlerEvents from './handlerEvents.js'

let width = 32;
let height = 32;
const DEFAULT_NAME_LAYER = "Layer";
let targetScale = 1;

let listFrame = document.getElementById("list-frames");
let listLayer = document.getElementById("list-Layers");

const canvas = document.querySelector("canvas#painting");
const handlerEvents = HandlerEvents(canvas);

window.add_layer = addLayer;
window.remove_layer = removeLayer;
window.change_active_layer = changeActiveLayer;
window.move_layer_to = moveLayerTo;

window.add_frame = addFrame;
window.remove_frame = removeFrame;
window.change_active_frame = changeActiveFrame;
window.move_frame_to = moveFrameTo;

window.onload = async ()=>{
    let {
            offsetWidth: viewportWidth, 
            offsetHeight: viewportHeight
        } = document.querySelector("#drawing-area");

    canvas.width = width;
    canvas.height = height;

    targetScale = Math.min(viewportWidth / width, viewportHeight / height);
    canvas.style.position = `absolute`;
    canvas.style.transform = `scale(${targetScale})`;
    canvas.style.left = `${ ( (viewportWidth - width*targetScale)/ 2  )}px`;
    canvas.style.top = `${0}`;

    window.module = await ModulePixelEditor({
        canvas,
        preRun: function() {
            console.log("WASM module is starting...");
        },
        postRun: function() {
            console.log("WASM module has completed...");
        },
        onRuntimeInitialized: () =>{
            console.log("MODULE INITIALIZED")
        }
    });

    window.editor = new module.Editor(width, height);

    let frame = new module.Frame();
    frame.addLayer(new module.Layer(DEFAULT_NAME_LAYER, width, height));
    editor.addFrame(frame);
    editor.render();
    
    let headerFrame = document.querySelector("#pane-footer .header");
    headerFrame.addEventListener("click", function(e){
        if(e.target.classList.contains("header"))
            this.parentNode.querySelector(".body").classList.toggle("hidden")
    });
    
    buildToolBar();
    
    let btnAddFrame = document.getElementById("add-frame");
    let btnCloneFrame = document.getElementById("duplicate-frame");
    let btnMoveDownFrame = document.getElementById("move-down-frame");
    let btnMoveUpFrame = document.getElementById("move-up-frame");
    let btnRemoveFrame = document.getElementById("remove-frame");
    
    btnAddFrame.addEventListener("click", ()=>{
        let f = new module.Frame();
        let l = new module.Layer(DEFAULT_NAME_LAYER, width, height);
        f.addLayer(l);
        editor.addFrame(f);
        editor.changeActiveFrame(f.getID());
    });
    btnRemoveFrame.addEventListener("click", ()=> {
        const activeFrame = editor.getActiveFrame();
        editor.removeFrame(activeFrame.getID());
    });
    btnMoveDownFrame.addEventListener("click", ()=> editor.bringFrameBack(editor.getActiveFrame().getID()));
    btnMoveUpFrame.addEventListener("click", ()=> editor.bringFrameToFoward(editor.getActiveFrame().getID()));
    btnCloneFrame.addEventListener("click", ()=> {
        const activeFrame = editor.getActiveFrame();
        activeFrame.cloneFrame(activeFrame.getID());
    });

    let inpOpacity = document.querySelector("input[name='opacity-layer']");
    inpOpacity.addEventListener("input", function() {
        const activeFrame = editor.getActiveFrame();
        const activeLayer = activeFrame.getActiveLayer();
        activeLayer.setOpacity(parseInt(this.value));
        updateOpacityLayer(this.value)
    });

    let btnAddLayer = document.getElementById("add-layer");
    let btnRemoveLayer = document.getElementById("remove-layer");
    let btnCloneLayer = document.getElementById("duplicate-layer");
    let btnMoveDown = document.getElementById("move-down-layer");
    let btnMoveUp = document.getElementById("move-up-layer");

    btnAddLayer.addEventListener("click", ()=> editor.getActiveFrame().addLayer(new module.Layer(findTitle(DEFAULT_NAME_LAYER), width, height)));
    btnRemoveLayer.addEventListener("click", ()=> {
        const activeFrame = editor.getActiveFrame();
        const activeLayer = activeFrame.getActiveLayer();
        activeFrame.removeLayer(activeLayer.getID());
    });
    btnMoveDown.addEventListener("click", ()=> {
        const activeFrame = editor.getActiveFrame();
        const activeLayer = activeFrame.getActiveLayer();
        activeFrame.bringLayerBack(activeLayer.getID());
    });
    btnMoveUp.addEventListener("click", ()=> {
        const activeFrame = editor.getActiveFrame();
        const activeLayer = activeFrame.getActiveLayer();
        activeFrame.bringLayerToFoward(activeLayer.getID());
    });
    btnCloneLayer.addEventListener("click", ()=> {
        const activeFrame = editor.getActiveFrame();
        const activeLayer = activeFrame.getActiveLayer();
        
        activeFrame.cloneLayer(activeLayer.getID());
    });
}

function addFrame(frame){
    const id = frame.getID();

    let frameElement = document.createElement("div");
    let canvas = document.createElement("canvas");

    frameElement.classList.add("frame");
    frameElement.dataset.id = id.toString();

    frameElement.append(canvas);
    listFrame.append(frameElement);

    frameElement.addEventListener("click", ()=> editor.changeActiveFrame(id));
}
function changeActiveFrame(id){
    const activeFrame = editor.getActiveFrame();
    const activeLayer = activeFrame.getActiveLayer();
    const activeLayerID =  activeLayer.getID().toString();

    let frameElement = getFrameById(id.toString());
    listFrame.querySelectorAll("div.frame.active")
             .forEach((f)=>f.classList.remove("active"));
    frameElement?.classList.toggle("active", true);
    
    listLayer.querySelectorAll(".layer")
             .forEach((e)=>e.remove());

    let layers = activeFrame.getAllLayers();
    for(let i = 0; i < layers.size(); i++){
        addLayer(layers.get(i));

        if(activeLayerID ==  layers.get(i).getID().toString())
        {
            changeActiveLayer(layers.get(i));
        }
    }
}
function moveFrameTo(id, index){
    const listFrame = document.getElementById("list-frames");
    let frames = listFrame.querySelectorAll("div.frame");
    let frameElement = getFrameById(id.toString());

    if (frameElement === frames[index] || index < 0 || index >= frames.length) {
        return;
    }
    
    if (frameElement.compareDocumentPosition(frames[index]) & Node.DOCUMENT_POSITION_FOLLOWING) {
        frames[index].after(frameElement);
    } else {
        frames[index].before(frameElement);
    }
}
function removeFrame(id){
    let frameElement = getFrameById(id);
    frameElement.remove();
}
function getFrameById(id){
    return listFrame.querySelector(`.frame[data-id="${id}"]`);
}

/* LAYER */
function updateOpacityLayer(value){
    document.querySelector("#opacity-label h5").innerText = "Transparência " + value + "%"
}
function changeActiveLayer(layer){
    let layerElement = getLayerById(layer.getID().toString());
    if(!layerElement)
        layerElement = addLayer(layer);

    listLayer.querySelectorAll("div.layer.active")
                .forEach((f)=>f.classList.remove("active"));
    layerElement?.classList.toggle("active", true);

    let inpOpacity = document.querySelector("input[name='opacity-layer']");
    document.querySelector("#opacity-label h5").innerText = "Transparência " + layer.getOpacity() + "%"
    inpOpacity.value = layer.getOpacity();
}
function addLayer(layer){
    let activeFrame = editor.getActiveFrame();
    if(!activeFrame)
        return;

    if(!activeFrameContainLayer(layer.getID()))
        return;

    let listLayer = document.getElementById("list-Layers");
    let layerElement = document.createElement("div");
    let nameLayer = document.createElement("div");
    let h5 = document.createElement("h5");
    let btnHideLayer = document.createElement("button");
    let btnLockLayer = document.createElement("button");
    let btnGrabLayer = document.createElement("button");
    
    layerElement.classList.add("layer");
    layerElement.dataset.id = layer.getID().toString();
    nameLayer.className = "text";
    nameLayer.className = "name-layer"
    h5.innerText = layer.getName();

    btnHideLayer.className = "hide-layer";
    btnHideLayer.innerHTML = `<i class=\"fa ${layer.isVisible() ? "fa-eye-slash" : "fa-eye"}\"></i>`;

    btnLockLayer.className = "lock-layer";
    btnLockLayer.innerHTML = `<i class=\"fa ${layer.isLock() ?  "fa-lock" : "fa-unlock" }\"></i>`;

    btnGrabLayer.className = "grab-layer";
    btnGrabLayer.innerHTML = "<i class=\"fa fa-grip-lines\"></i>";

    layerElement.append(btnHideLayer);
    nameLayer.append(h5);
    layerElement.append(nameLayer);
    layerElement.append(btnLockLayer);
    layerElement.append(btnGrabLayer);
    listLayer.prepend(layerElement);

    layerElement.addEventListener("click", ()=>editor.getActiveFrame().changeActiveLayer(layer.getID()));
    btnLockLayer.addEventListener("click", toggleLockLayer);
    btnHideLayer.addEventListener("click", toggleHideLayer);
    btnGrabLayer.addEventListener("mousedown", grabLayer);
    nameLayer.addEventListener("dblclick", renameLayer);
    
    function toggleLockLayer(){
        let icon = this.querySelector("i");

        if(layer.isLock()){
            icon.classList.replace("fa-lock", "fa-unlock");
            return;
        }
        icon.classList.replace("fa-unlock", "fa-lock");
    }
    function toggleHideLayer(){
        let icon = this.querySelector("i");
        
        if(layer.isLock()){
            icon.classList.replace("fa-eye-slash", "fa-eye");
            layerElement.classList.toggle("hidden-layer", false);
            return;
        }
    
        icon.classList.replace("fa-eye", "fa-eye-slash");
        layerElement.classList.toggle("hidden-layer", true);
    }
    function grabLayer(e){
        let listLayer = document.getElementById("list-Layers");
        let abort = new AbortController();
        e.preventDefault();
    
        window.addEventListener("mousemove", (e)=> {
            listLayer.querySelectorAll(".layer").forEach(el => el.classList.remove("swap"));
            e.target.closest(".layer")?.classList.add("swap");
        },{signal: abort.signal});
        window.addEventListener("mouseup", (e)=>{
            e.target?.closest(".layer").classList.remove("swap");
            e.target?.closest(".layer").after(layerElement);
            abort.abort();
        }, { once: true });
    }
    function renameLayer(e){
        let inpNameLayer = document.createElement("input");
        inpNameLayer.value = layer.getName();
        inpNameLayer.type = "text";

        nameLayer.replaceChild(inpNameLayer, h5);
        inpNameLayer.focus();
        inpNameLayer.addEventListener("blur", function(){
            if(inpNameLayer.value == "" || inpNameLayer.value == layer.getName()){
                nameLayer.replaceChild(h5, inpNameLayer);
                return;
            }
            let nome = findTitle(inpNameLayer.value);
            layer.setName(nome);
            h5.innerText = layer.getName();
            nameLayer.replaceChild(h5, inpNameLayer);
        });
        inpNameLayer.addEventListener("keypress", function(e){
            if(e.keyCode == 13)
                inpNameLayer.blur();
        });
    }
    return layerElement;
}
function removeLayer(id){    
    let frameElement = getLayerById(id);
    frameElement.remove();
}
function moveLayerTo(id, index){
    if(!activeFrameContainLayer(id))
        return;

    let layers = listLayer.querySelectorAll("div.layer");
    index = layers.length - index - 1;
    let layerElement = getLayerById(id.toString());

    if (layerElement === layers[index] || index < 0 || index >= layers.length) {
        return;
    }
    
    if (layerElement.compareDocumentPosition(layers[index]) & Node.DOCUMENT_POSITION_FOLLOWING) {
        layers[index].after(layerElement);
    } else {
        layers[index].before(layerElement);
    }
}
function getLayerById(id){
    return listLayer.querySelector(`.layer[data-id="${id}"]`);
}
function activeFrameContainLayer(layerID){
    let activeFrame = editor.getActiveFrame();
    const strIdLayer = layerID.toString();
    
    let frames = activeFrame.getAllLayers();
    for(let i = 0; i < frames.size(); i++){
        if(frames.get(i).getID().toString() == strIdLayer){
            return true;
        }
    }
    return false;
}
function findTitle(find) {
    let name = find;
    let cntr = 1;
    while(hasLayerWithName(name)){
        name = `${name.replace(/\(\d+\)$/, '')}(${cntr})`;
        cntr++;
    }

    return name;
}
function hasLayerWithName(name){
    let layers = editor.getActiveFrame().getAllLayers();
    for(let i = 0; i < layers.size(); i++){
        if(layers.get(i).getName() === name){
            return true;
        }
    }
    return false;
}



// Mock
function direction(from, to){
    if(from.y === to.y) return "H"
    if(from.x === to.x) return "V"
    return "D"
}
function distance(from, to){
    return {
        dx:to.x - from.x,
        dy:to.y - from.y
    }
}
let paintStrategy = () => {
    let activeFrame;
    let activeLayer;
    let flagFromPoint = null;
    let flagToPoint = null;
    let flagDir = null;
    let line;

    return {
        onPressed: (point) => {
            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            point = cursorToPixel(point);

            line = new module.Line(
                activeLayer,
                point.x, point.y,
                point.x, point.y,
                window.selectedColor
            );
            line.draw();
            editor.render();


            flagFromPoint = point;
            flagToPoint = point;
            flagDir = direction(flagFromPoint, flagToPoint);
        },

        onTracking: (point) => {
            point = cursorToPixel(point);
            if (point.x == flagToPoint.x && point.y == flagToPoint.y) return;
            
            let dir = direction(flagToPoint, point);
            if (flagDir !== dir) {
                const flagDelta = distance(flagFromPoint, flagToPoint);
                const delta = distance(flagToPoint, point);

                activeLayer.putPixel(flagToPoint.x, flagToPoint.y, 0);
                
                // flagToPoint.y += Math.sign(delta.dy);
                // flagToPoint.x += Math.sign(delta.dx);
                flagFromPoint = flagToPoint;
                
                // if(dir == "D" && Math.abs(flagDelta.dy) >= 1){
                //     activeLayer.putPixel(flagToPoint.x, flagToPoint.y, 0);
                //     flagToPoint.y += Math.sign(delta.dy);
                //     flagToPoint.x += Math.sign(delta.dx);
                //     flagFromPoint = flagToPoint;
                // }

                // if(dir === "H" && Math.abs(flagDelta.dy) >= 1){
                //     activeLayer.putPixel(flagToPoint.x, flagToPoint.y, 0);
                //     flagToPoint.x += Math.sign(delta.dx);
                //     flagFromPoint = flagToPoint;
                // }
            }


            const line = new module.Line(
                activeLayer,
                flagToPoint.x, flagToPoint.y,
                point.x, point.y,
                window.selectedColor
            );
            line.draw();
            
            // activeLayer.putPixel(flagToPoint.x, flagToPoint.y, 0x0000FFFF);
            // activeLayer.putPixel(point.x, point.y, 0xFF0000FF);

            editor.render();

            flagToPoint = point;
            flagDir = dir;
        },

        onRelease: () => {
            editor.render();
        }
    };
};
let eraseStrategy = () => {
    let activeFrame;
    let activeLayer;
    let flagFromPoint = null;
    let flagToPoint = null;
    let flagDir = null;
    let line;

    return {
        onPressed: (point) => {
            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            point = cursorToPixel(point);

            line = new module.Line(
                activeLayer,
                point.x, point.y,
                point.x, point.y,
                0x0
            );
            line.draw();
            editor.render();


            flagFromPoint = point;
            flagToPoint = point;
            flagDir = direction(flagFromPoint, flagToPoint);
        },

        onTracking: (point) => {
            point = cursorToPixel(point);
            if (point.x == flagToPoint.x && point.y == flagToPoint.y) return;
            
            let dir = direction(flagToPoint, point);
            if (flagDir !== dir) {
                const flagDelta = distance(flagFromPoint, flagToPoint);
                const delta = distance(flagToPoint, point);

                activeLayer.putPixel(flagToPoint.x, flagToPoint.y, 0);
                
                // flagToPoint.y += Math.sign(delta.dy);
                // flagToPoint.x += Math.sign(delta.dx);
                flagFromPoint = flagToPoint;
                
                // if(dir == "D" && Math.abs(flagDelta.dy) >= 1){
                //     activeLayer.putPixel(flagToPoint.x, flagToPoint.y, 0);
                //     flagToPoint.y += Math.sign(delta.dy);
                //     flagToPoint.x += Math.sign(delta.dx);
                //     flagFromPoint = flagToPoint;
                // }

                // if(dir === "H" && Math.abs(flagDelta.dy) >= 1){
                //     activeLayer.putPixel(flagToPoint.x, flagToPoint.y, 0);
                //     flagToPoint.x += Math.sign(delta.dx);
                //     flagFromPoint = flagToPoint;
                // }
            }


            const line = new module.Line(
                activeLayer,
                flagToPoint.x, flagToPoint.y,
                point.x, point.y,
                0x0
            );
            line.draw();
            
            // activeLayer.putPixel(flagToPoint.x, flagToPoint.y, 0x0000FFFF);
            // activeLayer.putPixel(point.x, point.y, 0xFF0000FF);

            editor.render();

            flagToPoint = point;
            flagDir = dir;
        },

        onRelease: () => {
            editor.render();
        }
    };
};

let squareStrategy = () => {
    let activeFrame;
    let activeLayer;

    let markerTopLeft     = document.querySelector(".marker#m-top-left");
    let markerTopRight    = document.querySelector(".marker#m-top-right");
    let markerBottomLeft  = document.querySelector(".marker#m-bottom-left");
    let markerBottomRight = document.querySelector(".marker#m-bottom-right");

    return {
        onPressed: (point) => {
            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            point = cursorToPixel(point);

            markerTopLeft.addEventListener("mousedown", moveMarker);
            markerTopRight.addEventListener("mousedown", moveMarker);
            markerBottomLeft.addEventListener("mousedown", moveMarker);
            markerBottomRight.addEventListener("mousedown", moveMarker);
        },
        onTracking: (point) => {
            point = cursorToPixel(point);
            if (point.x == flagToPoint.x && point.y == flagToPoint.y) return;
            

        },
        onRelease: () => {
            editor.render();
        }
    };
    function moveMarker(point){

    }
};

function cursorToPixel(point){
    return {
        x: Math.floor(point.x / targetScale),
        y: Math.floor(point.y / targetScale),
    }
}


function buildToolBar(){
    const buttonPencil = document.querySelector(".tool-pencil");
    buttonPencil.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(paintStrategy());
        changeSelectTool.call(this);
    });
    
    const buttonEraser = document.querySelector(".tool-eraser");
    buttonEraser.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(eraseStrategy());
        changeSelectTool.call(this);
    });
    
    const buttonDropper = document.querySelector(".tool-dropper");
    buttonDropper.addEventListener("click", function(e){
        changeSelectTool.call(this);
    });
    
    const buttonLine = document.querySelector(".tool-line");
    buttonLine.addEventListener("click", function(e){
        changeSelectTool.call(this);
    });
    
    
    const buttonSquare = document.querySelector(".tool-square");
    buttonSquare.addEventListener("click", function(e){
        changeSelectTool.call(this);
    });
    
    const buttonCircle = document.querySelector(".tool-circle");
    buttonCircle.addEventListener("click", function(e){
        changeSelectTool.call(this);
    });
    
    const buttonBucket = document.querySelector(".tool-bucket");
    buttonBucket.addEventListener("click", function(e){
        changeSelectTool.call(this);
    });
    
    
    const buttonBrush = document.querySelector(".tool-brush");
    buttonBrush.addEventListener("click", function(e){
        changeSelectTool.call(this);
    });
    
    const buttonUndo = document.querySelector("#undo");
    buttonUndo.addEventListener("click", function(e){
        history.undo();
    });
    
    const buttonRedo = document.querySelector("#redo");
    buttonRedo.addEventListener("click", function(e){
        history.redo();
    });
    
    handlerEvents.setLeftButtonMousePressedEvent(eraseStrategy());
    buttonPencil.click();

    function changeSelectTool(){
        document.querySelector(".tool.active")?.classList.toggle("active", false);
        this.classList.toggle("active", true);
    }
}