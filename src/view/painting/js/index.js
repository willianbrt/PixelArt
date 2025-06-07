import ModulePixelEditor from '../build/graphics/PixelEditor.js'

let width = 100;
let height = 100;
let activeFrame;
let activeLayer;
const DEFAULT_NAME_LAYER = "Layer";

window.onload = async ()=>{
    window.canvas = document.querySelector("canvas#painting");

    let {
            clientWidth: viewportWidth, 
            clientHeight: viewportHeight
        } = document.querySelector("#drawing-area");

    canvas.height = viewportHeight;
    canvas.width = viewportWidth;

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

    let frame = new module.Frame(width, height);
    frame.addLayer(new module.Layer(DEFAULT_NAME_LAYER, width, height));
    window.editor.addFrame(frame);
    
    
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

    btnMoveDownFrame.addEventListener("click", ()=> editor.bringFrameBack(editor.getActiveFrame().getID()));
    btnMoveUpFrame.addEventListener("click", ()=> editor.bringFrameToFoward(editor.getActiveFrame().getID()));
    btnAddFrame.addEventListener("click", ()=> {
        let f = new module.Frame(width, height);
        let l = new module.Layer("Layer", width, height);
        f.addLayer(l);
        editor.addFrame(f);
        editor.changeActiveFrame(f.getID());
    });
    btnRemoveFrame.addEventListener("click", ()=> { editor.removeFrame(editor.getActiveFrame().getID()); });
    // btnCloneFrame.addEventListener("click", ()=> cloneFrame(activeFrame.getId()));
        // btnAddFrame.click();
        // btnAddFrame.click();
        // btnAddFrame.click();
        // btnAddFrame.click();
        // headerFrame.click();
    
    let btnAddLayer = document.getElementById("add-layer");
    let btnRemoveLayer = document.getElementById("remove-layer");
    let btnCloneLayer = document.getElementById("duplicate-layer");
    let btnMoveDown = document.getElementById("move-down-layer");
    let btnMoveUp = document.getElementById("move-up-layer");

    let inpOpacity = document.querySelector("input[name='opacity-layer']");

    btnAddLayer.addEventListener("click", ()=> editor.getActiveFrame().addLayer(new module.Layer(findTitle(DEFAULT_NAME_LAYER), width, height)));
    inpOpacity.addEventListener("input", function() {
        const activeFrame = editor.getActiveFrame();
        const activeLayer = activeFrame.getActiveLayer();
        activeLayer.setOpacity(parseInt(this.value));
        updateOpacityLayer(this.value)
    });
    btnRemoveLayer.addEventListener("click", ()=> {
        const activeFrame = editor.getActiveFrame();
        const activeLayer = activeFrame.getActiveLayer();
        activeFrame.removeLayer(activeLayer.getID());
    });
    // btnMoveDown.addEventListener("click", ()=> moveDownLayer(activeLayer.getId()));
    // btnMoveUp.addEventListener("click", ()=> moveUpLayer(activeLayer.getId()));
    // btnCloneLayer.addEventListener("click", ()=> cloneLayer(activeLayer.getId()));
}

window.add_layer = addLayer;
window.remove_layer = removeLayer;
window.change_active_layer = changeActiveLayer;
window.move_layer_to = moveLayerTo;

window.add_frame = addFrame;
window.remove_frame = removeFrame;
window.change_active_frame = changeActiveFrame;
window.move_frame_to = moveFrameTo;

let listFrame = document.getElementById("list-frames");
let listLayer = document.getElementById("list-Layers");
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
function removeLayer(id){    
    let frameElement = getLayerById(id);
    frameElement.remove();
}
function moveLayerTo(id, index){
    let layers = listLayer.querySelectorAll("div.layer");
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
function changeActiveLayer(layer){
    let layerElement = getLayerById(layer.getID().toString());
        
    if(!layerElement){
        // addLayer(layer);
        // console.log(getLayerById(layer.getID().toString()))
        console.log("change", layer.getID().toString())

    }

    listLayer.querySelectorAll("div.layer.active")
                .forEach((f)=>f.classList.remove("active"));
    layerElement?.classList.toggle("active", true);

    let inpOpacity = document.querySelector("input[name='opacity-layer']");
    document.querySelector("#opacity-label h5").innerText = "Transparência " + layer.getOpacity() + "%"
    inpOpacity.value = layer.getOpacity();
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


function addLayer(layer){
        console.log("add", layer.getID().toString())
    let activeFrame = editor.getActiveFrame();
    if(!activeFrame)
        return;

    const strIdLayer = layer.getID().toString();
    let has = false;
    let frames = activeFrame.getAllLayers();
    for(let i = 0; i < frames.size(); i++){
        if(frames.get(i).getID().toString() == strIdLayer){
            has = true;
            break;
        }
    }
    if(!has)
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
function getLayerById(id){
    return listLayer.querySelector(`.layer[data-id="${id}"]`);
}