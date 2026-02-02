import ModulePixelEditor from '../build/PixelEditor.js'
import HandlerEvents from './handlerEvents.js'
import { PositionHelper } from "../../../scripts/common/position.js";
import { Chromatic, ColorFactory } from "./chromatic.js"
import "https://cdn.jsdelivr.net/npm/gif.js@0.2.0/dist/gif.min.js"

const DEFAULT_NAME_LAYER = "Layer";
let editor;

let listFrame = document.getElementById("list-frames");
let listLayer = document.getElementById("list-Layers");

const canvas = document.querySelector("canvas#painting");
const renderArea = document.querySelector("#render-area");
const drawingArea = document.querySelector("#drawing-area");
const handlerEvents = HandlerEvents(drawingArea);

let targetScale = 1;
const pattern = {
    dot: [[1]],
    brush_1: [
        [0.9,1,0.8],
        [0.7,1,0.5],
        [0.2,1,0.3],
    ],
}
var pattern_selected = "dot";

const lineSize = document.querySelector("input[name='size']");
const weight = document.querySelector("input[name='strength']");
weight.value = 100;

var isPixelPerfect = document.querySelector("#pixel-perfect input[type='checkbox']");
var isMirrorX = document.querySelector("#mirror-x input[type='checkbox']");
var isMirrorY = document.querySelector("#mirror-y input[type='checkbox']");
var isFill = document.querySelector("#fill input[type='checkbox']");


var dirtyFlag = {
    start:{
        x: 0,
        y:0
    },
    end:{
        x: 0,
        y:0
    }
};
let boundingSelectedArea = {
    start:{x:-1,y:-1},
    end:{x:-1,y:-1},
    getWidth: function(){
        return this.end.x - this.start.x;
    },
    getHeight: function(){
        return this.end.y - this.start.y;
    }
}

let ctx = canvas.getContext("2d");
ctx.beginPath();

let clipboard;
const channel = new BroadcastChannel("shared-buffer");
channel.onmessage = (e) => {
    if(e.data.action == "SET_CLIPBOARD"){
        clipboard = e.data.clipboard;
    }

    if(e.data.action == "REQUEST_CLIPBOARD"){
        channel.postMessage({ action:"SET_CLIPBOARD", clipboard: clipboard});
    }
};
let _shortcuts = {
    "contextmenu": ()=>{},
    "control": {
        "s": (event)=>{
            // exportAs("image/png");
            // exportAsGIF();
            save();
        },
        "o": (event)=>{
            importProject();
        },
        "v": ()=>{
            if(document.activeElement.tagName === "INPUT" ||
                document.activeElement.tagName === "TEXTAREA" ||
                document.activeElement.isContentEditable)
                return;
                let select = selectStrategy();
                handlerEvents.setRightButtonMousePressedEvent(select);
                select.paste();

                changeSelectTool.call(document.querySelector(".tool-select"));
        },
        "shift": {
            "alt": {

            }
        },
    }
};

window.addEventListener("keydown", function(event){
    let action;
    if (event.ctrlKey){
        action = _shortcuts.control;

        if (event.shiftKey){
            action = action.shift;

            if (event.altKey){
                action = _shortcuts.alt;
            }
        }
    }

    if(action?.hasOwnProperty(event.key.toLowerCase())){
        event.preventDefault();
        event.stopPropagation();
        action[event.key.toLowerCase()]();
    }
});
window.addEventListener("click", function(event){
    if(event.target.tagName !== "INPUT" &&
        event.target.tagName !== "TEXTAREA" &&
        !event.target.isContentEditable){
        document.activeElement.blur();
    }
});

window.onload = async ()=>{
    channel.postMessage({ action: "REQUEST_CLIPBOARD"});

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

    createProject(32, 10);
    editor.setNumberTiles(2,2);
    editor.render();

    let {
            offsetWidth: viewportWidth,
            offsetHeight: viewportHeight
        } = drawingArea;
    targetScale = Math.floor(Math.min(viewportWidth / canvas.clientWidth, viewportHeight / canvas.clientHeight));

    renderArea.style.position = `absolute`;
    renderArea.style.scale = targetScale;
    renderArea.style.left = `${ ( (viewportWidth - canvas.clientWidth*targetScale)/ 2  )}px`;
    renderArea.style.top = `${ ( (viewportHeight - canvas.clientHeight*targetScale)/ 2  ) }px`;

    buildPaneToolBar();
    buildPaneFrames();
    buildPaneLayers();
}
function buildPaneFrames(){
    let headerFrame = document.querySelector("#pane-footer .header");
    headerFrame.addEventListener("click", function(e){
        if(e.target.classList.contains("header"))
            this.parentNode.querySelector(".body").classList.toggle("hidden")
    });

    let btnAddFrame = document.getElementById("add-frame");
    let btnCloneFrame = document.getElementById("duplicate-frame");
    let btnMoveDownFrame = document.getElementById("move-down-frame");
    let btnMoveUpFrame = document.getElementById("move-up-frame");
    let btnRemoveFrame = document.getElementById("remove-frame");
    let btnFlipXFrame = document.getElementById("flip-x");
    let btnFlipYFrame = document.getElementById("flip-y");

    btnAddFrame.addEventListener("click", ()=>{
        let frame = new module.Frame();
        let layer = new module.Layer(DEFAULT_NAME_LAYER, width, height);
        frame.addLayer(layer);
        editor.addFrame(frame);
        editor.changeActiveFrame(frame.getID());
        addFrame(frame);
        changeActiveFrame(editor.getActiveFrame());
    });
    btnRemoveFrame.addEventListener("click", ()=> {
        const activeFrame = editor.getActiveFrame();
        const id = activeFrame.getID();
        editor.removeFrame(id);
        removeFrame(id);

        let frames = editor.getAllFrames();
        let n_frames = frames.size();

        if(n_frames == 0){
            let frame = new module.Frame();
            let layer = new module.Layer(DEFAULT_NAME_LAYER, width, height);
            frame.addLayer(layer);
            editor.addFrame(frame);
            addFrame(frame);
        }

        changeActiveFrame(editor.getActiveFrame());
    });
    btnMoveDownFrame.addEventListener("click", ()=> {
        const frameId = editor.getActiveFrame().getID();
        const i = Math.max(editor.getFrameIndex(frameId) - 1, 0) ;
        editor.bringFrameTo(frameId, i);
        editor.render();
        moveFrameTo(frameId, i);
    });
    btnMoveUpFrame.addEventListener("click", ()=> {
        const frameId = editor.getActiveFrame().getID();
        const i = Math.min(editor.getFrameIndex(frameId) + 1, editor.getAllFrames().size()-1);
        editor.bringFrameTo(frameId, i);
        editor.render();
        moveFrameTo(frameId, i);
    });
    btnCloneFrame.addEventListener("click", ()=> {
        let frame = editor.getActiveFrame();
        let cloneFrame = frame.clone();
        cloneFrame.setID( module.Guid.generateUUID());

        editor.addFrame(cloneFrame);

        let i = editor.getFrameIndex(frame.getID());
        editor.bringFrameTo(cloneFrame.getID(), i + 1);
        editor.changeActiveFrame(cloneFrame.getID());

        editor.render();

        addFrame(cloneFrame);
        changeActiveFrame(cloneFrame);
    });

    btnFlipXFrame.addEventListener("click", ()=>{
        const activeFrame = editor.getActiveFrame();
        activeFrame.flipX();
        editor.render();
    });

    btnFlipYFrame.addEventListener("click", ()=>{
        const activeFrame = editor.getActiveFrame();
        activeFrame.flipY();
        editor.render();
    });
}
function buildPaneLayers(){
    let inpOpacity = document.querySelector("input[name='opacity-layer']");
    inpOpacity.addEventListener("input", function() {
        const activeFrame = editor.getActiveFrame();
        const activeLayer = activeFrame.getActiveLayer();
        activeLayer.setOpacity(parseFloat(this.value / 100.0));
        document.querySelector("#opacity-label h5").innerText = "Transparência " + this.value + "%"
        editor.render();
    });

    let btnAddLayer = document.getElementById("add-layer");
    let btnRemoveLayer = document.getElementById("remove-layer");
    let btnCloneLayer = document.getElementById("duplicate-layer");
    let btnMoveDown = document.getElementById("move-down-layer");
    let btnMoveUp = document.getElementById("move-up-layer");

    btnAddLayer.addEventListener("click", ()=> {
        const layer = new module.Layer(findTitle(DEFAULT_NAME_LAYER), width, height);
        const frame = editor.getActiveFrame();
        frame.addLayer(layer);
        addLayer(layer);
        editor.render();
        updateFramePreview(editor.getActiveFrame());
    });
    btnRemoveLayer.addEventListener("click", ()=> {
        const frame = editor.getActiveFrame();
        const layer = frame.getActiveLayer();

        const layers = frame.getAllLayers();
        const n_layers = layers.size();

        if(n_layers == 0) return;

        frame.removeLayer(layer.getID());
        removeLayer(layer.getID());

        if(frame.getAllLayers().size() == 0){
            const layer = new module.Layer(findTitle(DEFAULT_NAME_LAYER), width, height);
            frame.addLayer(layer);
            addLayer(layer);
            frame.changeActiveLayer(layer.getID());
        }
        changeActiveLayer(frame.getActiveLayer());

        editor.render();
        updateFramePreview(editor.getActiveFrame());
    });
    btnMoveDown.addEventListener("click", ()=> {
        const activeFrame = editor.getActiveFrame();
        const activeLayer = activeFrame.getActiveLayer();
        const layerId = activeLayer.getID();
        const i = Math.max(activeFrame.getLayerIndex(layerId) - 1, 0);
        activeFrame.bringLayerTo(layerId, i);
        editor.render();
        moveLayerTo(layerId, i);
        updateFramePreview(editor.getActiveFrame());
    });
    btnMoveUp.addEventListener("click", ()=> {
        const activeFrame = editor.getActiveFrame();
        const activeLayer = activeFrame.getActiveLayer();
        const layerId = activeLayer.getID();
        const i = Math.min(activeFrame.getLayerIndex(layerId) + 1, activeFrame.getAllLayers().size()-1);
        activeFrame.bringLayerTo(layerId, i);
        editor.render();
        moveLayerTo(layerId, i);
        updateFramePreview(editor.getActiveFrame());
    });
    btnCloneLayer.addEventListener("click", ()=> {
        let frame = editor.getActiveFrame();
        const layer = frame.getActiveLayer();

        let cloneLayer = layer.clone();
        cloneLayer.setID(module.Guid.generateUUID());
        cloneLayer.setName(findTitle(cloneLayer.getName()));

        frame.addLayer(cloneLayer);

        let i = frame.getLayerIndex(layer.getID());
        frame.bringLayerTo(cloneLayer.getID(), i + 1);
        frame.changeActiveLayer(cloneLayer.getID());

        editor.render();

        addLayer(cloneLayer);
        changeActiveLayer(cloneLayer);
        updateFramePreview(editor.getActiveFrame());
    });
}

function createProject(width, height){
    editor?.delete();
    editor = new module.Editor(width, height);

    let frame = new module.Frame();
    let layer = new module.Layer(DEFAULT_NAME_LAYER, width, height);
    frame.addLayer(layer);
    editor.addFrame(frame);

    loadingProject(editor);
}

function loadingProject(){
    listFrame.replaceChildren();
    listLayer.replaceChildren();

    window.width = editor.getWidth();
    window.height = editor.getHeight();
    let {
            offsetWidth: viewportWidth,
            offsetHeight: viewportHeight
        } = drawingArea;

    canvas.width = width;
    canvas.height = height;

    targetScale = Math.floor(Math.min(viewportWidth / editor.getWidth(), viewportHeight / editor.getHeight()));

    renderArea.style.position = `absolute`;
    renderArea.style.scale = targetScale;
    renderArea.style.left = `${ ( (viewportWidth - width*targetScale)/ 2  )}px`;
    renderArea.style.top = `${ ( (viewportHeight - height*targetScale)/ 2  ) }px`;

    const frames = editor.getAllFrames();
    for(let i = 0; i < frames.size(); i++ ){
        addFrame(frames.get(i));
    }
    changeActiveFrame(editor.getActiveFrame());
}

function addFrame(frame){
    const id = frame.getID();
    let index = editor.getFrameIndex(frame.getID());

    let frames = listFrame.querySelectorAll("div.frame");
    let frameElement = document.createElement("div");
    let canvas = document.createElement("canvas");

    frameElement.classList.add("frame");
    frameElement.dataset.id = id.toString();

    frameElement.append(canvas);
    if(frames.length > 0)
        frames[index-1].after(frameElement);
    else
        listFrame.append(frameElement);

    frameElement.addEventListener("click", ()=>{
        editor.changeActiveFrame(id);
        changeActiveFrame(editor.getActiveFrame());
    });
    updateFramePreview(frame);
}
function changeActiveFrame(activeFrame){
    const activeLayer = activeFrame.getActiveLayer();

    let frameElement = getFrameById(activeFrame.getID().toString());
    listFrame.querySelectorAll("div.frame.active")
             .forEach((f)=>f.classList.remove("active"));
    frameElement?.classList.toggle("active", true);

    listLayer.querySelectorAll(".layer")
             .forEach((e)=>e.remove());

    let layers = activeFrame.getAllLayers();
    for(let i = 0; i < layers.size(); i++){
        addLayer(layers.get(i));
    }
    changeActiveLayer(activeLayer);
    editor.render();
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
    editor.render();
}
function removeFrame(id){
    let frameElement = getFrameById(id);
    frameElement.remove();
    editor.render();
}
function getFrameById(id){
    return listFrame.querySelector(`.frame[data-id="${id}"]`);
}

/* LAYER */
function changeActiveLayer(layer){
    let layerElement = getLayerById(layer.getID().toString());
    if(!layerElement)
        layerElement = addLayer(layer);

    listLayer.querySelectorAll("div.layer.active")
                .forEach((f)=>f.classList.remove("active"));
    layerElement?.classList.toggle("active", true);

    let inpOpacity = document.querySelector("input[name='opacity-layer']");
    inpOpacity.value = layer.getOpacity() * 100.0;
    document.querySelector("#opacity-label h5").innerText = "Transparência " + inpOpacity.value + "%"
}
function addLayer(layer){
    let activeFrame = editor.getActiveFrame();
    if(!activeFrame)
        return;

    if(!activeFrameContainLayer(layer.getID()))
        return;

    let index = activeFrame.getLayerIndex(layer.getID());

    let listLayer = document.getElementById("list-Layers");
    let layers = listLayer.querySelectorAll("div.layer");

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
    btnHideLayer.innerHTML = `<i class=\"fa ${layer.isVisible() ? "fa-eye" : "fa-eye-slash" }\"></i>`;

    btnLockLayer.className = "lock-layer";
    btnLockLayer.innerHTML = `<i class=\"fa ${layer.isLock() ? "fa-lock" : "fa-unlock" }\"></i>`;

    btnGrabLayer.className = "grab-layer";
    btnGrabLayer.innerHTML = "<i class=\"fa fa-grip-lines\"></i>";

    layerElement.append(btnHideLayer);
    nameLayer.append(h5);
    layerElement.append(nameLayer);
    layerElement.append(btnLockLayer);
    layerElement.append(btnGrabLayer);

    if(layers.length > 0)
        layers[layers.length-index].before(layerElement);
    else
        listLayer.prepend(layerElement);

    layerElement.addEventListener("click", ()=>{
        const frame = editor.getActiveFrame();
        frame.changeActiveLayer(layer.getID());
        changeActiveLayer(layer);
    });
    btnLockLayer.addEventListener("click", toggleLockLayer);
    btnHideLayer.addEventListener("click", toggleHideLayer);

    btnGrabLayer.addEventListener("mousedown", grabLayer);
    btnGrabLayer.addEventListener("touchstart", grabLayer);

    let editing = false;
    let dbl = false;
    nameLayer.addEventListener("dblclick", renameLayer);
    nameLayer.addEventListener("touchstart", (e)=>{
        if(e.touches.length > 1) return;

        if(!dbl){
            setTimeout(()=>{
                dbl = false;
            }, 250);
            dbl = true;
            return;
        }
        renameLayer(e)
    });

    function toggleLockLayer(){
        let icon = this.querySelector("i");

        layer.setLock(!layer.isLock());

        if(layer.isLock()){
            icon.classList.replace("fa-unlock","fa-lock");
            return;
        }
        icon.classList.replace("fa-lock", "fa-unlock");
    }
    function toggleHideLayer(){
        let icon = this.querySelector("i");

        layer.setVisible(!layer.isVisible());
        updateFramePreview(activeFrame);
        editor.render();

        if(layer.isVisible()){
            icon.classList.replace("fa-eye-slash", "fa-eye");
            layerElement.classList.toggle("hidden-layer", false);
            return;
        }

        icon.classList.replace("fa-eye", "fa-eye-slash");
        layerElement.classList.toggle("hidden-layer", true);
    }
    function grabLayer(e){
        let areaListLayer = document.getElementById("list-Layers");
        let listLayer = areaListLayer.querySelectorAll("#list-Layers .layer");
        let abort = new AbortController();
        e.preventDefault();

        let onTracking = (e)=> {
            let elementLast;
            listLayer.forEach(el => {
                let box  = el.getBoundingClientRect();

                el.classList.remove("after-indicator")
                el.classList.remove("before-indicator")

                if((e?.touches?.[0].clientY ?? e?.clientY) > box.y){
                    elementLast = el;
                }
            });

            if(!elementLast)
                listLayer[0]?.classList.add("before-indicator");
            else
                elementLast?.classList.add("after-indicator");
        }
        let onRelease = (e)=>{
            let elementLast;
            listLayer.forEach(el => {
                let box  = el.getBoundingClientRect();

                el.classList.remove("swap")
                if((e?.changedTouches?.[0].clientY ?? e?.clientY) > box.y){
                    elementLast = el;
                }
            });

            let activeFrame = editor.getActiveFrame();

            if(elementLast){
                elementLast?.classList.remove("after-indicator");
                elementLast?.after(layerElement);
            } else {
                elementLast = listLayer[0];
                elementLast?.classList.remove("before-indicator");
                areaListLayer.prepend(layerElement);
            }

            let indexDst = activeFrame.getLayerIndex(new module.Guid(elementLast.dataset.id));
            activeFrame.bringLayerTo(layer.getID(), indexDst);
            updateFramePreview(activeFrame);
            editor.render();

            abort.abort();
        }

        window.addEventListener("mousemove", onTracking, {signal: abort.signal});
        window.addEventListener("touchmove", onTracking, {signal: abort.signal});

        window.addEventListener("mouseup", onRelease, { once: true });
        window.addEventListener("touchend", onRelease, { once: true });

    }
    function renameLayer(e){
        if(editing) return;
        editing = true;

        let inpNameLayer = document.createElement("input");
        inpNameLayer.value = layer.getName();
        inpNameLayer.type = "text";

        nameLayer.replaceChild(inpNameLayer, h5);
        inpNameLayer.focus();

        function done(){
            if(inpNameLayer.value != "" && inpNameLayer.value != layer.getName()){
                let nome = findTitle(inpNameLayer.value);
                layer.setName(nome);
                h5.innerText = layer.getName();
            }

            nameLayer.replaceChild(h5, inpNameLayer);
            editing = false;
        }
        inpNameLayer.addEventListener("blur", done);
        inpNameLayer.addEventListener("keypress", function(e){
            if(e.keyCode == 13)
                done();
        });
    }
    return layerElement;
}
function removeLayer(id){
    let frameElement = getLayerById(id);
    frameElement.remove();
    editor.render();
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
    editor.render();
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

async function exportAsGIF(){
    let gifLoading = fetch('https://cdn.jsdelivr.net/npm/gif.js@0.2.0/dist/gif.worker.js')
    .then((response) => {
        if (!response.ok)
            throw new Error("Network response was not OK");
        return response.blob();
    }).then(workerBlob => {
        let gif = new GIF({
            workers: 2,
            workerScript: URL.createObjectURL(workerBlob),
            quality: 0,
            width: editor.getWidth()*15,
            height: editor.getHeight()*15
        });

        gif.on('finished', async function (blob) {
            let suggestedName = "abacadabra";
            const a = document.createElement("a");
            a.href = URL.createObjectURL(blob);
            a.download = suggestedName;
            a.click();
            URL.revokeObjectURL(a.href);
        });
        return gif;
    });

    gifLoading.then(gif => {
        let frames = editor.getAllFrames();
        for(let i = 0; i < frames.size(); i++){
            let frame = frames.get(i);
            const bufferJS = frame.getBufferJS();
            const buffer8 = new Uint8ClampedArray(
                bufferJS.buffer,
                bufferJS.byteOffset,
                bufferJS.byteLength);
            const imageData = new ImageData(buffer8, editor.getWidth(), editor.getHeight());
            const bitmap = createImageBitmap(imageData);
            gif.addFrame(imageData, { delay: 200, copy: true })
        }
        gif.render();
    });
}
async function exportAs(mimetype){
    let resizedWidth = editor.getWidth()*15;
    let resizedHeight = editor.getHeight()*15;

    const frame = editor.getActiveFrame();
    const bufferJS = frame.getBufferJS();
    const buffer8 = new Uint8ClampedArray(
        bufferJS.buffer,
        bufferJS.byteOffset,
        bufferJS.byteLength);
    const imageData = new ImageData(buffer8, editor.getWidth(), editor.getHeight());
    const bitmap = await createImageBitmap(imageData);

    const offscreenCanvas = new OffscreenCanvas(resizedWidth, resizedHeight);
    const ctx = offscreenCanvas.getContext("2d");
    ctx.imageSmoothingEnabled = false;

    ctx.drawImage(bitmap, 0, 0, resizedWidth, resizedHeight);

    offscreenCanvas.convertToBlob().then(blob =>{
        let suggestedName = "abacadabra";
        const a = document.createElement("a");
        a.href = URL.createObjectURL(blob);
        a.download = suggestedName;
        a.click();
        URL.revokeObjectURL(a.href);
    }, mimetype);

}
async function save(){
    const frames = editor.getAllFrames();
    let project = {
        application: "PRBT",
        author: "https://preebit.com",
        width: editor.getWidth(),
        height: editor.getHeight(),
        version: "0.0.0",
        n_frames: frames.size(),
        frames: []
    };

    for(let frameIndex = 0; frameIndex < frames.size(); frameIndex++){
        const frame = frames.get(frameIndex);
        const layers = frame.getAllLayers();

        let dataFrame = {
            id: frame.getID().toString(),
            duration: frame.getFrameDuration(),
            n_layers: layers.size(),
            layers: []
        };


        for(let layerIndex = 0; layerIndex < layers.size(); layerIndex++){
            const layer = layers.get(layerIndex);

            let dataLayer = {
                id: layer.getID().toString(),
                name: layer.getName(),
                opacity: layer.getOpacity(),
                is_lock: layer.isLock(),
                is_visible: layer.isVisible(),
                compression: 1,
                buffer: ""
            };
            let index = 0, cnt = 0;
            let flagColorLayer = layer.getPixelByIndex(index);

            while(index < layer.getLength()){
                let colorLayer = layer.getPixelByIndex(index);

                if(flagColorLayer != colorLayer){
                    dataLayer.buffer +=  cnt + ":" + flagColorLayer + ",";
                    cnt = 1;
                    flagColorLayer = colorLayer;
                } else {
                    cnt++;
                }
                index++;
            }
            dataLayer.buffer +=  cnt + ":" + flagColorLayer;
            dataFrame.layers.push(dataLayer);
        }
        project.frames.push(dataFrame);
    }


    const blob = new Blob([JSON.stringify(project)], { type: "application/json" });
    const a = document.createElement("a");
    a.href = URL.createObjectURL(blob);
    a.download = "output.preebit";
    a.click();
}
async function importProject(){
    const input = document.createElement("input");
    input.type = "file";
    input.accept = ".preebit";
    input.multiple = false;
    input.excludeAcceptAllOption = true;

    input.onchange = async () => {
        input.files[0].text().then(text => {
            const data = JSON.parse(text);
            if(data.application !== "PRBT") throw new Error("Formato inválido.");
            switch(data.version){
                case "0.0.0":
                    loadVersion000(data);
                break;
                default: throw new Error("Versão não identificada.");
            }
        });
    };

    input.click();
}
function loadVersion000(data){
    let local_editor = new module.Editor(data.width, data.height);

    for(let frameIndex = 0; frameIndex < data.frames.length; frameIndex++){
        const dataFrame = data.frames[frameIndex];
        let frame = new module.Frame();
        frame.setID(new module.Guid(dataFrame.id));

        if(dataFrame.n_layers !== dataFrame.layers.length) throw new Error("Arquivo corrompido.");

        for(let layerIndex = 0; layerIndex < dataFrame.layers.length; layerIndex++){
            const dataLayer = dataFrame.layers[layerIndex];

            let layer = new module.Layer(dataLayer.name, data.width, data.height);
            layer.setID(new module.Guid(dataLayer.id));
            layer.setLock(dataLayer.is_lock);
            layer.setVisible(dataLayer.is_visible);
            layer.setOpacity(dataLayer.opacity);

            let buffer = dataLayer.buffer.split(",");
            let index = 0;
            for(let i = 0; i < buffer.length; i++){
                let pixelInfo = buffer[i].split(":");
                for(let j = 0; j < pixelInfo[0]; j++){
                    layer.putPixelByIndex(Number(index) >>> 0, Number(pixelInfo[1]) >>> 0);
                    index++;
                }
            }

            frame.addLayer(layer);
        }
        local_editor.addFrame(frame);
    }
    // editor?.delete();
    editor = local_editor;

    loadingProject();
}
function updateFramePreview(frame){
    const width = editor.getWidth();
    const height = editor.getHeight();

    const id = frame.getID().toString();
    const bufferJS = frame.getBufferJS();
    const buffer8 = new Uint8ClampedArray(
        bufferJS.buffer,
        bufferJS.byteOffset,
        bufferJS.byteLength);
    const imageData = new ImageData(buffer8, width, height);

    let frameElement = listFrame.querySelector(`.frame[data-id="${id}"]`);
    let frameCanvas = frameElement.querySelector(`canvas`);
    frameCanvas.width = width;
    frameCanvas.height = height;

    let ctx = frameCanvas.getContext("2d");
    ctx.clearRect(0, 0, width, height);
    ctx.putImageData(imageData, 0, 0);
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
function getLineSize(){
    return parseInt(lineSize.value);
}
function getWeight(){
    return parseFloat(weight.value) / 100.0;
}

// Mock
let paintStrategy = () => {
    let activeFrame;
    let activeLayer;
    let flagToPoint = null;
    let brush;
    pattern_selected = "dot";

    return {
        onPressed: (point) => {
            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            point = cursorToPixel(point);
            brush = new module.Brush(
                point.x, point.y,
                point.x, point.y,
                getPattern(pattern[pattern_selected]),
                window.selectedColor,
                getLineSize(),
                isMirrorX.checked, isMirrorY.checked,
                2,2
            );
            editor.draw(brush);
            updateFramePreview(activeFrame);
            flagToPoint = point;
        },

        onTracking: (point) => {
            point = cursorToPixel(point);
            if (point.x == flagToPoint.x && point.y == flagToPoint.y) return;

            brush = new module.Brush(
                flagToPoint.x, flagToPoint.y,
                point.x, point.y,
                getPattern(pattern[pattern_selected]),
                window.selectedColor,
                parseInt(getLineSize()),
                isMirrorX.checked, isMirrorY.checked,
                2,2
            );
            editor.draw(brush);
            updateFramePreview(activeFrame);

            flagToPoint = point;
        },

        onRelease: () => {
            editor.render();
        }
    };
};
let brushStrategy = () => {
    let activeFrame;
    let activeLayer;
    let flagToPoint = null;
    let brush;
    pattern_selected = "brush_1";

    return {
        onPressed: (point) => {
            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            point = cursorToPixel(point);
            brush = new module.Brush(
                point.x, point.y,
                point.x, point.y,
                getPattern(pattern[pattern_selected]),
                window.selectedColor,
                getLineSize(),
                isMirrorX.checked, isMirrorY.checked,
                2,2
            );
            editor.draw(brush);
            updateFramePreview(activeFrame);


            flagToPoint = point;
        },

        onTracking: (point) => {
            point = cursorToPixel(point);
            if (point.x == flagToPoint.x && point.y == flagToPoint.y) return;

            brush = new module.Brush(
                flagToPoint.x, flagToPoint.y,
                point.x, point.y,
                getPattern(pattern[pattern_selected]),
                window.selectedColor,
                getLineSize(),
                isMirrorX.checked, isMirrorY.checked,
                2,2
            );
            editor.draw(brush);
            updateFramePreview(activeFrame);
            flagToPoint = point;
        },

        onRelease: () => {
            editor.render();
        }
    };
};
let eraseStrategy = () => {
    let activeFrame;
    let activeLayer;
    let flagToPoint = null;
    let erase;
    pattern_selected = "dot";

    return {
        onPressed: (point) => {
            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            point = cursorToPixel(point);

            erase = new module.Erase(
                point.x, point.y,
                point.x, point.y,
                getLineSize(),
                getWeight(), 
                isMirrorX.checked, isMirrorY.checked,
                2,2);
            editor.draw(erase);
            updateFramePreview(activeFrame);


            flagToPoint = point;
        },

        onTracking: (point) => {
            point = cursorToPixel(point);
            if (point.x == flagToPoint.x && point.y == flagToPoint.y) return;

            erase = new module.Erase(
                flagToPoint.x, flagToPoint.y,
                point.x, point.y,
                getLineSize(),
                getWeight(), 
                isMirrorX.checked, isMirrorY.checked,
                2,2);
            editor.draw(erase);
            updateFramePreview(activeFrame);

            flagToPoint = point;
        },

        onRelease: () => {
            editor.render();
        }
    };
};

let bucketStrategy = () => {
    let activeFrame;
    let activeLayer;
    return {
        onPressed: (point) => {
            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            point = cursorToPixel(point);

            let bucket = new module.Bucket(point.x, point.y, 
                window.selectedColor, 
                isMirrorX.checked, isMirrorY.checked,
                2,2);
            editor.draw(bucket);
            updateFramePreview(activeFrame);
        },

        onTracking: (point) => {},

        onRelease: () => {}
    };
};

let squareStrategy = () => {
    let activeFrame;
    let activeLayer;
    let squareTool;
    let flagToPoint = null;
    let startPoint = null;

    return {
        onPressed: (point) => {
            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            point = cursorToPixel(point);

            squareTool = new module.Square(point.x, point.y, point.x, point.y,
                isFill.checked, getLineSize(),
                window.selectedColor, 
                isMirrorX.checked, isMirrorY.checked,
                2,2);
            editor.preview(squareTool);
            startPoint = point;
            flagToPoint = point;
        },

        onTracking: (point) => {
            point = cursorToPixel(point);
            if (point.x == flagToPoint.x && point.y == flagToPoint.y) return;

            squareTool = new module.Square(
                startPoint.x, startPoint.y,
                point.x, point.y,
                isFill.checked, getLineSize(),
                window.selectedColor, 
                isMirrorX.checked, isMirrorY.checked,
                2,2);
            editor.preview(squareTool);

            flagToPoint = point;
        },

        onRelease: () => {
            editor.draw(squareTool);
            updateFramePreview(activeFrame);

            editor.render();
        }
    };
};

let circleStrategy = () => {
    let activeFrame;
    let activeLayer;
    let circleTool;
    let flagToPoint = null;
    let startPoint = null;

    return {
        onPressed: (point) => {
            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            point = cursorToPixel(point);

            circleTool = new module.Circle(point.x, point.y, point.x, point.y,
                isFill.checked, getLineSize(),
                window.selectedColor, 
                isMirrorX.checked, isMirrorY.checked,
                2,2);
                editor.preview(circleTool);
            startPoint = point;
            flagToPoint = point;
        },

        onTracking: (point) => {
            point = cursorToPixel(point);
            if (point.x == flagToPoint.x && point.y == flagToPoint.y) return;

            circleTool = new module.Circle(
                startPoint.x, startPoint.y,
                point.x, point.y,
                isFill.checked, getLineSize(),
                window.selectedColor, 
                isMirrorX.checked, isMirrorY.checked,
                2,2);
            editor.preview(circleTool);

            flagToPoint = point;
        },

        onRelease: () => {
            editor.draw(circleTool);
            updateFramePreview(activeFrame);

            editor.render();
        }
    };
};

let lineStrategy = () => {
    let activeFrame;
    let activeLayer;
    let lineTool;
    let flagToPoint = null;
    let startPoint = null;

    return {
        onPressed: (point) => {
            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            point = cursorToPixel(point);

            lineTool = new module.Line(
                point.x, point.y,
                point.x, point.y,
                window.selectedColor,
                getLineSize(), 
                isMirrorX.checked, isMirrorY.checked,
                2,2);
            editor.preview(lineTool);

            startPoint = point;
            flagToPoint = point;
        },

        onTracking: (point) => {
            point = cursorToPixel(point);
            if (point.x == flagToPoint.x && point.y == flagToPoint.y) return;

            lineTool = new module.Line(
                startPoint.x, startPoint.y,
                point.x, point.y,
                window.selectedColor,
                getLineSize(), 
                isMirrorX.checked, isMirrorY.checked,
                2,2);
            editor.preview(lineTool);

            flagToPoint = point;
        },

        onRelease: () => {
            editor.draw(lineTool);
            updateFramePreview(activeFrame);

            editor.render();
        }
    };
};

let dropperStrategy = () => {
    let activeFrame;
    let activeLayer;
    let factoryColor = ColorFactory();

    return {
        onPressed: (point) => {
            point = cursorToPixel(point);

            activeFrame = editor.getActiveFrame();
            activeLayer = activeFrame.getActiveLayer();

            let colorHex = activeLayer.getPixel(point.x, point.y);
            modalChromatic.setColor(factoryColor.buildByDecimal(colorHex));
        },
        onTracking: (point) => {
            point = cursorToPixel(point);

            let colorHex = activeLayer.getPixel(point.x, point.y);
            modalChromatic.setColor(factoryColor.buildByDecimal(colorHex));
        },
        onRelease: () => {}
    };
};
const ENUM_MARKER = {
    tl:0,
    bl:1,
    tr:2,
    br:3,
    rotate:4
}
function selectStrategy(){
    let activeFrame;
    let select;
    let intialPixel = null;

    const CORNER_TOOL_ROTATE = "rotate";
    const CORNER_TOOL_RESIZE = "resize";
    let cornerTool = CORNER_TOOL_RESIZE;

    pattern_selected = "dot";
    canvas.style.cursor = "crosshair";

    let toolbar;
    let selectedArea = document.querySelector("#selected-area");
    let markersElement = document.querySelectorAll("#selected-area .marker");

    _shortcuts.control.c = copy;

    let _onTracking = onTrackingBounding;
    let _onRelease = (point)=>{};
    let _onPressed = (point,event) => {
        selectedArea.style.position = "absolute"
        selectedArea.style.display = "block";

        intialPixel = cursorToPixel(point);

        if(select == null){
            createFloatingToolbar();
            return;
        }

        if(event.target.classList.contains("marker")){
            if(cornerTool == CORNER_TOOL_RESIZE)
                _onTracking = onTrackingResize.bind(event.target);
            else if(cornerTool == CORNER_TOOL_ROTATE)
                _onTracking = onTrackingRotate.bind(event.target);
            else if(cornerTool == CORNER_TOOL_CROP)
                _onTracking = onTrackingCrop.bind(event.target);

            return;
        }

        let corners = select.getDestinationCorners();
        if(isInsideRotatedBounding(intialPixel, corners)){
            _onTracking = onTrackingTranslate;
        } else if(!toolbar.contains(event.target)){
            done();
            _onTracking = onTrackingBounding;
        }
    };

    function onTrackingBounding(point){
        let pixel = cursorToPixel(point);

        if(intialPixel.x - pixel.x == 0 && intialPixel.y - pixel.y == 0){
            return;
        }
        activeFrame = editor.getActiveFrame();

        if(select !== null && select !== undefined) select.delete();

        select = new module.Selection(intialPixel.x, intialPixel.y,
            pixel.x, pixel.y, 
            activeFrame.getActiveLayer(), 
            true,
            isMirrorX.checked, isMirrorY.checked,
            2,2);
        editor.preview(select);

        if(toolbar === null || toolbar === undefined) createFloatingToolbar();

        drawMarkers();
    };
    function onTrackingResize(point){
        let pixel = cursorToPixel(point, true);
        let currentMarker = parseInt(this.dataset.marker);

        select.resize(currentMarker, pixel.x, pixel.y);
        editor.preview(select);

        drawMarkers();
        intialPixel = pixel;
    }

    function onTrackingTranslate(point){
        let pixel = cursorToPixel(point);

        select.translate(pixel.x - intialPixel.x, pixel.y - intialPixel.y);
        intialPixel = pixel;

        editor.preview(select);
        drawMarkers();
    }

    function onTrackingRotate(point){
        let pixel = cursorToPixel(point, true);

        let dstCenter = select.getCenter();

        let radBefore = PositionHelper.getRadBetweenTwoPoints(intialPixel, dstCenter);
        let radAfter  = PositionHelper.getRadBetweenTwoPoints(pixel, dstCenter);

        let deltaRad = radAfter - radBefore;
        intialPixel = pixel;

        select.rotate(deltaRad);
        editor.preview(select);
        drawMarkers();
    }

    function drawMarkers(){
        let _corners = select.getDestinationCorners();

        markersElement[ENUM_MARKER.tl].style.translate = "-50% -50%";
        markersElement[ENUM_MARKER.tr].style.translate = "-50% -50%";
        markersElement[ENUM_MARKER.bl].style.translate = "-50% -50%";
        markersElement[ENUM_MARKER.br].style.translate = "-50% -50%";

        markersElement[ENUM_MARKER.bl].style.left = `${(_corners.bottomLeft.x) }px`;
        markersElement[ENUM_MARKER.bl].style.top  = `${(_corners.bottomLeft.y) }px`;

        markersElement[ENUM_MARKER.tl].style.left = `${(_corners.topLeft.x) }px`;
        markersElement[ENUM_MARKER.tl].style.top  = `${(_corners.topLeft.y) }px`;

        markersElement[ENUM_MARKER.tr].style.left = `${(_corners.topRight.x) }px`;
        markersElement[ENUM_MARKER.tr].style.top  = `${(_corners.topRight.y) }px`;

        markersElement[ENUM_MARKER.br].style.left = `${(_corners.bottomRight.x) }px`;
        markersElement[ENUM_MARKER.br].style.top  = `${(_corners.bottomRight.y) }px`;

        ctx.fillStyle = "rgba(0,0,0,0.5)"
        ctx.beginPath();
        ctx.moveTo(_corners.topLeft.x, _corners.topLeft.y);
        ctx.lineTo(_corners.topRight.x, _corners.topRight.y);
        ctx.lineTo(_corners.bottomRight.x, _corners.bottomRight.y);
        ctx.lineTo(_corners.bottomLeft.x, _corners.bottomLeft.y);
        ctx.closePath();
        ctx.fill();

        selectedArea.style.position = ""
        selectedArea.style.display = "block"
        markersElement.forEach((e)=>{
            e.style.display = "block";
            e.style.width = 1 + "px";
            e.style.height = 1 + "px";
            e.style.scale =  0.5;
        });

    }


    function copy(){
        if(document.activeElement.tagName === "INPUT" ||
            document.activeElement.tagName === "TEXTAREA" ||
            document.activeElement.isContentEditable)
            return;
        if(select == null) return;

        let surface = select.copy();

        const buffer = new Uint8ClampedArray(module.HEAPU8.buffer, surface.getBufferPtr(), surface.getLength()*4);

        clipboard = { buffer: buffer.slice(), width: surface.getWidth() };
        channel.postMessage({ action: "SET_CLIPBOARD", clipboard: clipboard});

        surface.delete();
    }

    function paste(){
        if(!clipboard) return;

        activeFrame = editor.getActiveFrame();

        done();
        createFloatingToolbar();
        const { buffer, width } = clipboard;
        const height = buffer.length / 4 / width;

        let surface = new module.Surface(width, height);
        module.HEAPU8.set(buffer, surface.getBufferPtr());

        select = new module.Selection(0, 0, 
            width-1, height-1, 
            surface, false,
            isMirrorX.checked, isMirrorY.checked,
            2,2);
        editor.preview(select);
        drawMarkers();
    }

    function done(){
        selectedArea.style.display = "none";
        markersElement.forEach((e)=>{
            e.style.display = "none";
            e.style.width = 1 + "px";
            e.style.height = 1 + "px";
            e.style.scale =  0.5;
        });
        toolbar?.remove();
        toolbar = null;

        if(select != null){
            editor.draw(select);
            updateFramePreview(activeFrame);
            select.delete();
        }
        select = null;

        delete _shortcuts.control.c;
    }

    function createFloatingToolbar() {
        toolbar?.remove();

        toolbar = document.createElement("div");
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
                id: CORNER_TOOL_RESIZE,
                icon: "resize",
                label: "Redimensionar",
                eventClick: function(){
                    cornerTool = CORNER_TOOL_RESIZE;
                    toolbar.querySelectorAll("button")
                            .forEach((e)=> e.classList.remove("active"));
                    this.classList.add("active");
                }
            },
            {
                id: CORNER_TOOL_ROTATE,
                icon: "autorenew",
                label: "Rotacionar",
                eventClick: function(){
                    cornerTool = CORNER_TOOL_ROTATE;
                    toolbar.querySelectorAll("button")
                            .forEach((e)=> e.classList.remove("active"));
                    this.classList.add("active");
                }
            },
            {
                id: "copy",
                icon: "content_copy",
                label: "Copiar",
                eventClick: copy
            },
            {
                id: "paste",
                icon: "content_paste",
                label: "Colar",
                eventClick: paste
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

        drawingArea.appendChild(toolbar);

        function addTool({ id, icon, label, eventClick }) {
            const btn = document.createElement("button");
            if(cornerTool == id) btn.classList.add("active");
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

    return {
        onPressed: _onPressed,
        onTracking:(point)=>{
            _onTracking(point);
        },
        onRelease: _onRelease,
        dispatch: done,
        paste: paste,
        copy: copy,
        done: done
    };
};
function isInsideRotatedBounding(point, corners){
    function cross(p1, p2, p3){
        return (p1.x - p2.x) * (p3.y - p2.y) - (p1.y - p2.y) * (p3.x - p2.x);
    }
    if(corners === null) return false;

    let b1 = cross(point, corners.topLeft, corners.topRight) >= 0;
    let b2 = cross(point, corners.topRight, corners.bottomRight) >= 0;
    let b3 = cross(point, corners.bottomRight, corners.bottomLeft) >= 0;
    let b4 = cross(point, corners.bottomLeft, corners.topLeft) >= 0;

    return (b1 && b2 && b3 && b4) || (!b1 && !b2 && !b3 && !b4);
}


function cursorToPixel(point, middlePoint=false){
    let position = getPosition();

    let pixel = {
        x: (point.x - position.x) / targetScale,
        y: (point.y - position.y) / targetScale
    }
    if(middlePoint){
        pixel.x = Math.floor(pixel.x + 0.5);
        pixel.y = Math.floor(pixel.y + 0.5);
        return pixel;
    }
    pixel.x = Math.floor(pixel.x);
    pixel.y = Math.floor(pixel.y);

    return pixel;
}
function direction(from, to){
    if(from.y === to.y) return "H"
    if(from.x === to.x) return "V"
    return "D"
}

function cursorIsInsideSkecth(point){
    return point.x < width && point.y < height && point.x >= 0 && point.y >= 0
}

function hoverBrush(cursorPosition){
    cursorPosition = cursorToPixel(cursorPosition);

    editor.renderArea(dirtyFlag.start.x, dirtyFlag.start.y,
                      dirtyFlag.end.x, dirtyFlag.end.y);
    ctx.fillStyle = `rgba(${0},${0},${0},${0.6})`;

    ctx.moveTo(boundingSelectedArea.start.x , boundingSelectedArea.start.y);
    ctx.lineTo((boundingSelectedArea.end.x+1), boundingSelectedArea.start.y);
    ctx.lineTo((boundingSelectedArea.end.x+1), (boundingSelectedArea.end.y+1));
    ctx.lineTo(boundingSelectedArea.start.x ,   (boundingSelectedArea.end.y+1));
    ctx.closePath();
    ctx.fill();

    let patternSelected = pattern[pattern_selected];

    let startPixel = {
        x: Math.round(cursorPosition.x - ((patternSelected.length) / 2 * getLineSize())),
        y: Math.round(cursorPosition.y - ((patternSelected[0].length) / 2* getLineSize()))
    };

    let heightPattern = patternSelected.length*getLineSize();
    let widthPattern = patternSelected[0].length*getLineSize();

    let newAxisX = {StartAxis:0, EndAxis:0};
    let newAxisY = {StartAxis:0, EndAxis:0};

    if(!computeVisibleShape(startPixel.x, widthPattern, width, newAxisX) ||
       !computeVisibleShape(startPixel.y, heightPattern, height, newAxisY)){
        dirtyFlag.start.x = newAxisX.StartAxis;
        dirtyFlag.end.x = newAxisX.EndAxis;
        dirtyFlag.start.y = newAxisY.StartAxis;
        dirtyFlag.end.y = newAxisY.EndAxis;

        return;
    }

    dirtyFlag.start.x = newAxisX.StartAxis;
    dirtyFlag.end.x = newAxisX.EndAxis;
    dirtyFlag.start.y = newAxisY.StartAxis;
    dirtyFlag.end.y = newAxisY.EndAxis;

    let flagStartPixel = {
        x: dirtyFlag.start.x,
        y: dirtyFlag.start.y
    };

    let y = dirtyFlag.start.y;
    for(let i = 0; i < patternSelected.length; i++){
        let patternLine = patternSelected[i];

        let x = flagStartPixel.x;

        for(let j = 0; j < patternSelected[0].length; j++){
            let patternCell = patternLine[j];
            ctx.fillStyle = `rgba(${0},${0},${0},${patternCell*0.6})`;
            ctx.fillRect(x, y,getLineSize(),getLineSize());
            x += getLineSize();
        }
        y += getLineSize();
    }
}

function computeVisibleShape(originalAxis, originalSize, viewportSize, outNewAxis){
    if (originalAxis <= -originalSize || originalAxis >= viewportSize){
        outNewAxis.EndAxis = 0;
        outNewAxis.StartAxis = 0;
        return false;
    }

    outNewAxis.StartAxis = Math.max(0, originalAxis);
    outNewAxis.EndAxis = Math.min(viewportSize, originalAxis + originalSize);

    return true;
}

function buildPaneToolBar(){
    // canvas.addEventListener("mouseleave", function(e){
    //     editor.renderArea(dirtyFlag.start.x, dirtyFlag.start.y,
    //                     dirtyFlag.end.x, dirtyFlag.end.y);
    // });
    // handlerEvents.setMoveEvent(hoverBrush);


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
        handlerEvents.setRightButtonMousePressedEvent(dropperStrategy());
        changeSelectTool.call(this);
    });

    const buttonLine = document.querySelector(".tool-line");
    buttonLine.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(lineStrategy());
        changeSelectTool.call(this);
    });
    const buttonSquare = document.querySelector(".tool-square");
    buttonSquare.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(squareStrategy());
        changeSelectTool.call(this);
    });

    const buttonCircle = document.querySelector(".tool-circle");
    buttonCircle.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(circleStrategy());
        changeSelectTool.call(this);
    });

    const buttonBucket = document.querySelector(".tool-bucket");
    buttonBucket.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(bucketStrategy());
        changeSelectTool.call(this);
    });

    const buttonBrush = document.querySelector(".tool-brush");
    buttonBrush.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(brushStrategy());
        changeSelectTool.call(this);
    });
    const buttonSelect = document.querySelector(".tool-select");
    buttonSelect.addEventListener("click", function(e){
        handlerEvents.setRightButtonMousePressedEvent(selectStrategy());
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

    handlerEvents.setScrollEvent(onZoomScrollStrategy(), false);
    handlerEvents.setDoubleTouchEvent(onZoomDoubleTouchStrategy(), false);
    handlerEvents.setDoubleTouchEvent(onZoomDoubleTouchStrategy(), false);
    handlerEvents.setScrollEvent(onSizeStrategy(), true);
    handlerEvents.setGenericButtonMousePressedEvent(onPanningStrategy());
    handlerEvents.setLeftButtonMousePressedEvent(eraseStrategy());
    // handlerEvents.setRightButtonMousePressedEvent(onPanningStrategy());

    buttonPencil.click();
}
function changeSelectTool(){
    document.querySelector(".tool.active")?.classList.toggle("active", false);
    this.classList.toggle("active", true);
}
let onSizeStrategy = ()=> {
    return {
        onScrollUp: (cursor)=>{
            lineSize.value = Math.min(getLineSize() + 1, 20);
        },
        onScrollDown:(cursor)=>{
            lineSize.value = Math.max(getLineSize() - 1, 1);
        }
    }
}

let onZoomScrollStrategy = ()=> {
    return {
        onScrollUp: (cursor)=>{
            zoom(targetScale*1.1, cursor)
        },
        onScrollDown:(cursor)=>{
            zoom(targetScale/1.1, cursor);
        }
    }
}
let onZoomDoubleTouchStrategy = ()=> {
    let flagInitalDelta;
    let flagScale;
    let startPositionCursor;
    let cursor = {x:0, y:0};
    return {
        onPressed: (firstTouch, secondTouch)=>{
            cursor.x = (firstTouch.x + firstTouch.x) * 0.5;
            cursor.y = (firstTouch.y + firstTouch.y) * 0.5;

            const deltaX = (secondTouch.x - firstTouch.x);
            const deltaY = (secondTouch.y - firstTouch.y);
            const delta =  Math.hypot(deltaX, deltaY);
            flagInitalDelta = delta;
            flagScale = delta / flagInitalDelta;
            startPositionCursor = cursor;
        },
        onTracking: (firstTouch, secondTouch)=>{
            cursor.x = (firstTouch.x + firstTouch.x) * 0.5;
            cursor.y = (firstTouch.y + firstTouch.y) * 0.5;

            const deltaX = (secondTouch.x - firstTouch.x);
            const deltaY = (secondTouch.y - firstTouch.y);
            const delta = Math.hypot(deltaX, deltaY);
            let scale = delta / flagInitalDelta;

            zoom(targetScale * (1+scale-flagScale), cursor);

            let position = getPosition();

            let cursorDeltaX = cursor.x - startPositionCursor.x;
            let cursorDeltaY = cursor.y - startPositionCursor.y;

            moveTo({
                x: position.x+cursorDeltaX,
                y: position.y+cursorDeltaY
            });

            flagScale = scale;
        },
        onRelease: (firstTouch, secondTouch)=>{
        }
    }
}

function zoom(scale, positionCursor){
    if(scale < 1) return;

    let position = getPosition();

    let {
            offsetWidth: viewportWidth,
            offsetHeight: viewportHeight
        } = drawingArea;

    let x, y;
    if(canvas.clientWidth *scale <= viewportWidth){
        x = (viewportWidth - canvas.clientWidth*scale) * 0.5;
    }else{
        let currentWidth = canvas.clientWidth*targetScale;
        let endOfAxisX = position.x + currentWidth;
        let zoomPointX = Math.min(endOfAxisX, Math.max(position.x, positionCursor.x));
        x = zoomPointX - (zoomPointX - position.x) * (scale / targetScale);
    }

    if(canvas.clientHeight*scale <= viewportHeight){
        y = (viewportHeight - canvas.clientHeight*scale) * 0.5;
    } else {
        let currentHeight = canvas.clientHeight*targetScale;
        let endOfAxisY = position.y + currentHeight;
        let zoomPointY = Math.min(endOfAxisY, Math.max(position.y, positionCursor.y));
        y = zoomPointY - (zoomPointY - position.y) * (scale / targetScale);
    }


    targetScale = scale;
    renderArea.style.position = `absolute`;
    renderArea.style.scale = targetScale;

    moveTo({x,y});
}


let onPanningStrategy = ()=> {
    let startPositionCursor;
    return {
        onPressed: (cursor)=>{
            startPositionCursor = cursor;
            canvas.style.cursor = "grabbing";
        },
        onTracking:(cursor)=>{
            let positionCursor = cursor;
            let position = getPosition();

            let cursorDeltaX = positionCursor.x - startPositionCursor.x;
            let cursorDeltaY = positionCursor.y - startPositionCursor.y;

            moveTo({
                x: position.x+cursorDeltaX,
                y: position.y+cursorDeltaY
            });

            startPositionCursor = positionCursor;
        },
        onRelease:()=>{
            canvas.style.cursor = "";
        }
    }
}

function moveTo({x, y}){
    let {
            offsetWidth: viewportWidth,
            offsetHeight: viewportHeight
        } = drawingArea;

    let initialPosition = getInitialPosition();

    let currentWidth = canvas.clientWidth*targetScale;
    let currentHeight= canvas.clientHeight*targetScale;

    let minLeftOffset, minTopOffset;
    let maxLeftOffset , maxTopOffset;

    if(currentWidth <= viewportWidth){
        minLeftOffset = 0;
        maxLeftOffset = drawingArea.offsetWidth - currentWidth;
    } else {
        maxLeftOffset = initialPosition.x;
        minLeftOffset = drawingArea.offsetWidth - currentWidth - maxLeftOffset;
    }

    if(currentHeight <= viewportHeight){
        minTopOffset = 0;
        maxTopOffset = drawingArea.offsetHeight - currentHeight;
    } else {
        maxTopOffset = initialPosition.y;
        minTopOffset = drawingArea.offsetHeight - currentHeight - maxTopOffset;
    }

    renderArea.style.left = `${ Math.max(minLeftOffset, Math.min(maxLeftOffset,x))}px`;
    renderArea.style.top = `${ Math.max(minTopOffset, Math.min(maxTopOffset, y))}px`;
}
 function getInitialPosition(){
    let {
            offsetWidth: viewportWidth,
            offsetHeight: viewportHeight
        } = drawingArea;

    let p = {x:0,y:0}
    let minScale = Math.floor(Math.min(viewportWidth / width, viewportHeight / height));

    p.x = Math.floor((viewportWidth - (width*minScale)) / 2);
    p.y = Math.floor((viewportHeight - (height*minScale)) / 2);
    return p;
}

function getPosition(){
    return {
        x: renderArea.offsetLeft,
        y: renderArea.offsetTop
    }
}