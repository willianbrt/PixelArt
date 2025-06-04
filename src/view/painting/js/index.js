import ModulePixelEditor from '../build/graphics/PixelEditor.js'

let width = 100;
let height = 100;

window.onload = async ()=>{
    window.canvas = document.querySelector("canvas#painting");

    let {
            clientWidth: viewportWidth, 
            clientHeight: viewportHeight
        } = document.querySelector("#drawing-area");

    canvas.height = viewportHeight;
    canvas.width = viewportWidth;

    window.module = await ModulePixelEditor({
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
    console.log(module)
    window.editor = new module.Editor(width, height);
    // window.editor = new module.PixelEditor(width, height);

    
    let headerFrame = document.querySelector("#pane-footer .header");
    let btnAddFrame = document.getElementById("add-frame");
    let btnCloneFrame = document.getElementById("duplicate-frame");
    let btnMoveDownFrame = document.getElementById("move-down-frame");
    let btnMoveUpFrame = document.getElementById("move-up-frame");
    let btnRemoveFrame = document.getElementById("remove-frame");

    headerFrame.addEventListener("click", function(e){
        if(e.target.classList.contains("header"))
            this.parentNode.querySelector(".body").classList.toggle("hidden")
    });

    btnMoveDownFrame.addEventListener("click", ()=> moveDownFrame(activeFrame.getId()));
    btnMoveUpFrame.addEventListener("click", ()=> moveUpFrame(activeFrame.getId()));
    btnAddFrame.addEventListener("click", addFrame);
    btnRemoveFrame.addEventListener("click", ()=> removeFrame(activeFrame.getId()));
    btnCloneFrame.addEventListener("click", ()=> cloneFrame(activeFrame.getId()));
    
    
    let btnAddLayer = document.getElementById("add-layer");
    let btnCloneLayer = document.getElementById("duplicate-layer");
    let btnMoveDown = document.getElementById("move-down-layer");
    let btnMoveUp = document.getElementById("move-up-layer");
    let btnRemoveLayer = document.getElementById("remove-layer");

    inpOpacity.addEventListener("input", ()=> updateOpacityLayer(activeLayer.getId()));
    btnMoveDown.addEventListener("click", ()=> moveDownLayer(activeLayer.getId()));
    btnMoveUp.addEventListener("click", ()=> moveUpLayer(activeLayer.getId()));
    btnAddLayer.addEventListener("click", addLayer);
    btnRemoveLayer.addEventListener("click", ()=> removeLayer(activeLayer.getId()));
    btnCloneLayer.addEventListener("click", ()=> cloneLayer(activeLayer.getId()));
}

function updateOpacityLayer(){
    activeLayer.setOpacity(this.value);
}
function cloneLayer(id){
    let layer = activeLayer;
    layer.title = findTitle(activeLayer.getTitle());
    activeFrame.addLayer(layer);
}
function removeLayer(id){
    activeFrame.remove(activeLayer);
}
function moveUpLayer(id){
}
function moveDownLayer(id){
}
function addLayer(options){
}
function swapActiveLayer(){
    
}
