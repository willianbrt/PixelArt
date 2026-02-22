import { app } from "./app.js"

import { buildPaneLayers } from "./paneLayer.js"

let _paneFramesViewModel;
let _listFrame;

export function buildPaneFrames(paneFramesViewModel){
    _paneFramesViewModel = paneFramesViewModel;
    
    _listFrame = document.getElementById("list-frames");
    _listFrame.innerHTML = "";

    for(let i = 0; i < _paneFramesViewModel.getNumberFrames(); i++){
        let frameElement = createFrameElement(_paneFramesViewModel.getFrameByIndex(i));
        _listFrame.append(frameElement);
    }
    
    let headerFrame = document.querySelector("#pane-footer .header");
    headerFrame.onclick = function(e){
        if(e.target.classList.contains("header"))
            this.parentNode.querySelector(".body").classList.toggle("hidden")
    };
    
    let btnAddFrame = document.getElementById("add-frame");
    let btnCloneFrame = document.getElementById("duplicate-frame");
    let btnMoveDownFrame = document.getElementById("move-down-frame");
    let btnMoveUpFrame = document.getElementById("move-up-frame");
    let btnRemoveFrame = document.getElementById("remove-frame");
    let btnFlipXFrame = document.getElementById("flip-x");
    let btnFlipYFrame = document.getElementById("flip-y");
    
    btnAddFrame.onclick = ()=> _paneFramesViewModel.createFrame();
    btnRemoveFrame.onclick = ()=> _paneFramesViewModel.removeActiveFrame();
    btnMoveDownFrame.onclick = ()=> _paneFramesViewModel.moveDownActiveFrame();
    btnMoveUpFrame.onclick = ()=> _paneFramesViewModel.moveUpActiveFrame();
    btnCloneFrame.onclick = ()=> _paneFramesViewModel.cloneActiveFrame();

    btnFlipXFrame.onclick = ()=> _paneFramesViewModel.flipXActiveFrame();
    btnFlipYFrame.onclick = ()=> _paneFramesViewModel.flipYActiveFrame();
    
    _paneFramesViewModel.registerEvent("ADD_FRAME", onAddFrame);
    _paneFramesViewModel.registerEvent("REMOVE_FRAME", onRemoveFrame);
    _paneFramesViewModel.registerEvent("MOVE_FRAME_TO", onMoveFrameTo);
    _paneFramesViewModel.registerEvent("CHANGE_ACTIVE_FRAME", onChangeActiveFrame);
}
function onAddFrame(frame, index){
    let frameElement = createFrameElement(frame);
    let frames = _listFrame.querySelectorAll("div.frame");
    
    if(frames.length > 0)
        frames[index-1].after(frameElement);
    else
        _listFrame.append(frameElement);
}
function onRemoveFrame(id){
    let frameElement = getFrameById(id);
    frameElement?.remove();
}
function onChangeActiveFrame(id){
    let frameElement = getFrameById(id);
    
    _listFrame.querySelectorAll("div.frame.active")
              .forEach((f)=>f.classList.remove("active"));
    frameElement?.classList.toggle("active", true);

    buildPaneLayers(app.paneLayersViewModel());
}
function onMoveFrameTo(id, index){
    let frames = _listFrame.querySelectorAll("div.frame");
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
function onFrameDraw(frameId){
    const frameElement = getFrameById(frameId);
    const frameCanvas = frameElement.querySelector("canvas");
    let gl = frameCanvas.getContext("webgl");
    gl.texSubImage2D(
        gl.TEXTURE_2D,
        0,
        0,
        0,
        width,
        height,
        gl.RGBA,
        gl.UNSIGNED_BYTE,
        buffer);
}
function getFrameById(id){
    return _listFrame.querySelector(`.frame[data-id="${id}"]`);
}
function createFrameElement(frame){
    const { id, buffer, width, height, isActive } = frame;
    
    let frameElement = document.createElement("div");
    let canvas = document.createElement("canvas");

    let gl = canvas.getContext("webgl");
    const tex = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, tex);
    gl.texParameteri(gl.TEXTURE_2D,
                    gl.TEXTURE_MIN_FILTER,
                    gl.NEAREST);

    gl.texParameteri(gl.TEXTURE_2D,
                    gl.TEXTURE_MAG_FILTER,
                    gl.NEAREST);
    gl.texImage2D(
        gl.TEXTURE_2D,
        0,
        gl.RGBA,
        width,
        height,
        0,
        gl.RGBA,
        gl.UNSIGNED_BYTE,
        buffer
    ); 

    frameElement.classList.add("frame");
    if(isActive)
        frameElement.classList.add("active");
    frameElement.dataset.id = id.toString();

    frameElement.append(canvas);
    
    frameElement.onclick = ()=>{
        _paneFramesViewModel.changeActiveFrame(frame.id);
    };

    return frameElement;
}