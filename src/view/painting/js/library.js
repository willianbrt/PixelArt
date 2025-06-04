
mergeInto(LibraryManager.library,{
    draw: draw,
    add_layer: addLayer,
    remove_layer: removeLayer,
    swap_layer: swapActiveLayer,
    move_down_layer: moveDownLayer,
    move_up_layer: moveUpLayer,
    clone_layer: cloneLayer,

    add_frame: addFrame,
    remove_frame: removeFrame,
    swap_frame: swapActiveFrame,
    move_down_frame: moveDownFrame,
    move_up_frame: moveUpFrame,
    clone_frame: cloneFrame,
});



function draw(screen, length, viewportWidth, viewportHeight, x, y){
    const context = Module.canvas.getContext("2d");

    const ptr = screen;
    const width = viewportWidth;
    const height = viewportHeight;

    const buffer = new Uint8ClampedArray(Module.HEAPU8.buffer, ptr, length*4);
    const data = new ImageData(buffer, width);

    context.clearRect(x, y, width, height);
    context.putImageData(data, x, y);
}

/* FRAMES */
var ctrFrame = 1;
let activeFrame;

function addFrame(options){
    let listFrame = document.getElementById("list-frames");
    let frame = document.createElement("div");
    let canvas = document.createElement("canvas");
    
    frame.classList.add("frame");
    canvas.id = options.id ?? ctrFrame;

    if(listFrame.querySelectorAll(".frame").length == 0){
        swapActiveFrame.call(frame);
    }

    frame.append(canvas);
    listFrame.prepend(frame);

    frame.addEventListener("click", swapActiveFrame);
    ctrFrame++;
}
function swapActiveFrame(){
    activeFrame?.classList.remove("active");
    this.classList.add("active");
    activeFrame = this;
}
function moveDownFrame(){
    let listFrame = document.getElementById("list-frames");
    let frames = [...listFrame.querySelectorAll("div.frame")];
    let previousFrame = frames.indexOf(activeFrame) - 1;

    if(previousFrame>= 0)
        frames[previousFrame].before(activeFrame);
}
function moveUpFrame(){
    let listFrame = document.getElementById("list-frames");
    let frames = [...listFrame.querySelectorAll("div.frame")];
    let nextFrame = frames.indexOf(activeFrame) + 1;

    if(nextFrame < frames.length)
        frames[nextFrame].after(activeFrame);
}
function removeFrame(){
    let listFrame = document.getElementById("list-frames");
    if(listFrame.querySelectorAll("div.frame").length <= 1) return;

    let frames = [...listFrame.querySelectorAll(".frame")];
    let indexActiveFrame = frames.indexOf(activeFrame);
    activeFrame.remove();

    frames = [...listFrame.querySelectorAll("div.frame")];

    if (frames.length === 0) return;

    let nextFrame;
    if (indexActiveFrame < frames.length) {
        nextFrame = frames[indexActiveFrame];
    } else {
        nextFrame = frames[frames.length - 1];
    }

    swapActiveFrame.call(nextFrame);
}
function cloneFrame(){
    addFrame({});
}


/* LAYER */
let ctr = 1;
let activeLayer;

function updateOpcaityLayer(){
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
