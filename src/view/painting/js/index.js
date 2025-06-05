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

    // btnMoveDownFrame.addEventListener("click", ()=> moveDownFrame(activeFrame.getId()));
    // btnMoveUpFrame.addEventListener("click", ()=> moveUpFrame(activeFrame.getId()));
    btnAddFrame.addEventListener("click", ()=> editor.addFrame(new module.Frame(width, height)));
    btnRemoveFrame.addEventListener("click", ()=> editor.removeFrame(editor.getActiveFrame().getID()));
    // btnCloneFrame.addEventListener("click", ()=> cloneFrame(activeFrame.getId()));
    
    
    let btnAddLayer = document.getElementById("add-layer");
    let btnCloneLayer = document.getElementById("duplicate-layer");
    let btnMoveDown = document.getElementById("move-down-layer");
    let btnMoveUp = document.getElementById("move-up-layer");
    let btnRemoveLayer = document.getElementById("remove-layer");

    // let inpOpacity = document.querySelector("input[name='opacity-layer']");
    // inpOpacity.addEventListener("input", ()=> updateOpacityLayer(activeLayer.getId(), this.value));
    // btnMoveDown.addEventListener("click", ()=> moveDownLayer(activeLayer.getId()));
    // btnMoveUp.addEventListener("click", ()=> moveUpLayer(activeLayer.getId()));
    // btnAddLayer.addEventListener("click", addLayer);
    // btnRemoveLayer.addEventListener("click", ()=> removeLayer(activeLayer.getId()));
    // btnCloneLayer.addEventListener("click", ()=> cloneLayer(activeLayer.getId()));
}

window.add_layer = addLayer;
window.remove_layer = removeLayer;
window.swap_layer = swapActiveLayer;
window.move_layer_to = moveDownLayer;
window.move_down_layer = moveDownLayer;
window.move_up_layer = moveUpLayer;
window.clone_layer = cloneLayer;

window.add_frame = addFrame;
window.remove_frame = removeFrame;
window.change_active_frame = changeActiveFrame;
window.move_frame_to = moveFrameTo;
window.move_down_frame = moveDownFrame;
window.move_up_frame = moveUpFrame;


var ctrFrame = 1;
let activeFrame;

let listFrame = document.getElementById("list-frames");
function addFrame(frame){
    const id = frame.getID();

    let frameElement = document.createElement("div");
    let canvas = document.createElement("canvas");

    frameElement.classList.add("frame");
    frameElement.dataset.id = id.toString();

    frameElement.append(canvas);
    listFrame.prepend(frameElement);

    frameElement.addEventListener("click", ()=>changeActiveFrame(id));
}
function changeActiveFrame(id){
    let frameElement = getFrameById(id.toString());
    
    listFrame.querySelectorAll("div.frame.active")
             .forEach((f)=>f.classList.remove("active"));

    frameElement?.classList.toggle("active", true);
}
function moveFrameTo(frame, index){
    let frames = [...listFrame.querySelectorAll("div.frame")];
    let frameElement = listFrame.querySelector(`div.frame[data-id=${frame.getID().toString()}]`);
    let previousFrame = frames.indexOf(frameElement) - 1;

    if(previousFrame>= 0)
        frames[previousFrame].before(activeFrame);


    const children = parentNode.children;

    if (index < 0 || index > frames.length) {
        return;
    }

    if (index === children.length) {
        previousFrame.appendChild(newElement); // Append if index is at the end
    } else {
        previousFrame.insertBefore(newElement, children[index]);
    }
}
function moveDownFrame(frame){
    let frames = [...listFrame.querySelectorAll("div.frame")];
    let frameElement = getFrameById(frame.getID().toString());
    let previousFrame = frames.indexOf(frameElement) - 1;

    if(previousFrame>= 0)
        frames[previousFrame].before(frameElement);
}
function moveUpFrame(frame){
    let listFrame = document.getElementById("list-frames");
    let frames = [...listFrame.querySelectorAll("div.frame")];
    let nextFrame = frames.indexOf(activeFrame) + 1;

    if(nextFrame < frames.length)
        frames[nextFrame].after(activeFrame);
}
function removeFrame(id){
    let frameElement = getFrameById(id);
    frameElement.remove();
}
function getFrameById(id){
    return listFrame.querySelector(`.frame[data-id="${id}"]`);
}

/* LAYER */
let ctr = 1;
let activeLayer;

function updateOpacityLayer(){
    document.querySelector("#opacity-label h5").innerText = "Transparência " + this.value + "%"
    activeLayer.dataset.opacity = this.value;
}
function cloneLayer(){
    let title = findTitle(activeLayer.dataset.title);
    
    addLayer({
        title: title,
        ishidden: activeLayer.dataset.ishidden === "true",
        islock: activeLayer.dataset.islock === "true"
    });
}
function removeLayer(){
    let listLayer = document.getElementById("list-Layers");
    if(listLayer.querySelectorAll(".layer").length <= 1) return;

    let layers = [...listLayer.querySelectorAll(".layer")];
    let indexActiveLayer = layers.indexOf(activeLayer);
    activeLayer.remove();

    layers = [...listLayer.querySelectorAll(".layer")];

    if (layers.length === 0) return;

    let nextLayer;
    if (indexActiveLayer < layers.length) {
        nextLayer = layers[indexActiveLayer];
    } else {
        nextLayer = layers[layers.length - 1];
    }

    swapActiveLayer.call(nextLayer);
}
function moveUpLayer(){
    let listLayer = document.getElementById("list-Layers");
    let layers = [...listLayer.querySelectorAll(".layer")];
    let nextLayer = layers.indexOf(activeLayer) + 1;

    if(nextLayer < layers.length)
        layers[nextLayer].after(activeLayer);
}
function moveDownLayer(){
    let listLayer = document.getElementById("list-Layers");
    let layers = [...listLayer.querySelectorAll(".layer")];
    let previousLayer = layers.indexOf(activeLayer) - 1;

    if(previousLayer>= 0)
        layers[previousLayer].before(activeLayer);
}
function findTitle(find) {
    let listLayer = document.getElementById("list-Layers");
    const layers = [...listLayer.querySelectorAll(".layer")];
    let name = find;

    let layersFounds = 1;
    let cntr = 1;
    while(layers.length > cntr){
        const exists = layers.filter(e => e.dataset.title === name).length;

        if (exists >= 1) {
            name = `${name.replace(/\(\d+\)$/, '')}(${cntr})`;
        }

        cntr++;
    }

    return name;
}
function addLayer(options){
    console.log("add_layer")
    let listLayer = document.getElementById("list-Layers");
    let layer = document.createElement("div");
    let nameLayer = document.createElement("div");
    let h5 = document.createElement("h5");
    let btnHideLayer = document.createElement("button");
    let btnLockLayer = document.createElement("button");
    let btnGrabLayer = document.createElement("button");
    
    layer.classList.add("layer");
    layer.id = options.id ?? ctr;
    layer.dataset.islock = options.islock ?? false;
    layer.dataset.ishidden = options.ishidden ?? false;
    layer.dataset.title = options.title ?? findTitle("Layer " + ctr);
    layer.dataset.opacity = options.opacity ?? 100;

    if(listLayer.querySelectorAll(".layer").length == 0){
        swapActiveLayer.call(layer);
    }

    nameLayer.className = "text";
    nameLayer.className = "name-layer"
    h5.innerText = layer.dataset.title;

    btnHideLayer.className = "hide-layer";
    btnHideLayer.innerHTML = `<i class=\"fa ${options.ishidden ? "fa-eye-slash" : "fa-eye"}\"></i>`;

    btnLockLayer.className = "lock-layer";
    btnLockLayer.innerHTML = `<i class=\"fa ${options.islock ?  "fa-lock" : "fa-unlock" }\"></i>`;

    btnGrabLayer.className = "grab-layer";
    btnGrabLayer.innerHTML = "<i class=\"fa fa-grip-lines\"></i>";

    layer.append(btnHideLayer);
    nameLayer.append(h5);
    layer.append(nameLayer);
    layer.append(btnLockLayer);
    layer.append(btnGrabLayer);
    listLayer.prepend(layer);

    layer.addEventListener("click", swapActiveLayer);
    btnLockLayer.addEventListener("click", toggleLockLayer);
    btnHideLayer.addEventListener("click", toggleHideLayer);
    btnGrabLayer.addEventListener("mousedown", grabLayer);
    nameLayer.addEventListener("dblclick", renameLayer);
    
    function toggleLockLayer(){
        let isLocking = layer.getAttribute("data-islock") === "true";
        let icon = this.querySelector("i");

        if(isLocking){
            layer.dataset.islock = false;
            icon.classList.replace("fa-lock", "fa-unlock");
            return;
        }
        
        layer.dataset.islock = true;
        icon.classList.replace("fa-unlock", "fa-lock");
    }
    function toggleHideLayer(){
        let isHidden = layer.getAttribute("data-ishidden") === "true";
        let icon = this.querySelector("i");
        
        if(isHidden){
            layer.dataset.ishidden = false;
            icon.classList.replace("fa-eye-slash", "fa-eye");
            layer.classList.toggle("hidden-layer", false);
            return;
        }
    
        icon.classList.replace("fa-eye", "fa-eye-slash");
        layer.classList.toggle("hidden-layer", true);
        layer.dataset.ishidden = true;
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
            e.target?.closest(".layer").after(layer);
            abort.abort();
        }, { once: true });
    }
    function renameLayer(e){
        let inpNameLayer = document.createElement("input");
        inpNameLayer.value = layer.dataset.title;
        inpNameLayer.type = "text";

        nameLayer.replaceChild(inpNameLayer, h5);
        inpNameLayer.focus();
        inpNameLayer.addEventListener("blur", function(){
            if(inpNameLayer.value == "" || inpNameLayer.value == layer.dataset.title){
                nameLayer.replaceChild(h5, inpNameLayer);
                return;
            }

            layer.dataset.title = findTitle(inpNameLayer.value);
            h5.innerText = layer.dataset.title;
            nameLayer.replaceChild(h5, inpNameLayer);
        });
        inpNameLayer.addEventListener("keypress", function(e){
            if(e.keyCode == 13)
                inpNameLayer.blur();
        });
    }

    ctr++;
}
function swapActiveLayer(){
    let inpOpacity = document.querySelector("input[name='opacity-layer']");
    activeLayer?.classList.remove("active");
    this.classList.add("active");
    activeLayer = this;

    document.querySelector("#opacity-label h5").innerText = "Transparência " + activeLayer.dataset.opacity + "%"
    inpOpacity.value = activeLayer.dataset.opacity;
}