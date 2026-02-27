import { app } from "./app.js"

let _paneLayersViewModel;
let _listLayer;

export function buildPaneLayers(paneLayersViewModel){
    _paneLayersViewModel = paneLayersViewModel;

    _listLayer = document.getElementById("list-Layers");
    _listLayer.querySelectorAll(".layer")
             .forEach((e)=>e.remove());

    for(let i = 0; i < _paneLayersViewModel.getNumberLayers(); i++){
        let layerElement = createLayerElement(_paneLayersViewModel.getLayerByIndex(i));
        
        _listLayer.append(layerElement);
    }
    
    let btnAddLayer = document.getElementById("add-layer");
    let btnRemoveLayer = document.getElementById("remove-layer");
    let btnCloneLayer = document.getElementById("duplicate-layer");
    let btnMoveDown = document.getElementById("move-down-layer");
    let btnMoveUp = document.getElementById("move-up-layer");

    btnAddLayer.onclick = ()=> _paneLayersViewModel.createLayer();
    btnRemoveLayer.onclick = ()=> _paneLayersViewModel.removeActiveLayer();
    btnMoveDown.onclick = ()=> _paneLayersViewModel.moveDownActiveLayer();
    btnMoveUp.onclick = ()=> _paneLayersViewModel.moveUpActiveLayer();
    btnCloneLayer.onclick = ()=> _paneLayersViewModel.cloneActiveLayer();

    _paneLayersViewModel.registerEvent("ADD_LAYER", onAddLayer);
    _paneLayersViewModel.registerEvent("REMOVE_LAYER", onRemoveLayer);
    _paneLayersViewModel.registerEvent("MOVE_LAYER_TO", onMoveLayerTo);
    _paneLayersViewModel.registerEvent("CHANGE_ACTIVE_LAYER", onChangeActiveLayer);
}

function onAddLayer(layer, index){
    let layerElement = createLayerElement(layer);

    let layers = _listLayer.querySelectorAll("div.layer");
    
    if(layers.length > 0)
        layers[layers.length-index].before(layerElement);
    else
        _listLayer.prepend(layerElement);
}
function onRemoveLayer(id){
    let layerElement = getLayerElementById(id);
    if(!layerElement) return;
    layerElement.remove();
}
function onChangeActiveLayer(id){
    let layerElement = getLayerElementById(id);

    _listLayer.querySelectorAll("div.layer.active")
                .forEach((l)=>l.classList.remove("active"));
    layerElement?.classList.toggle("active", true);
}
function onMoveLayerTo(id, index){
    let layerElement = getLayerElementById(id);

    let layers = _listLayer.querySelectorAll("div.layer");
    index = layers.length - index - 1;
    if (layerElement === layers[index] || index < 0 || index >= layers.length) {
        return;
    }

    if (layerElement.compareDocumentPosition(layers[index]) & Node.DOCUMENT_POSITION_FOLLOWING) {
        layers[index].after(layerElement);
    } else {
        layers[index].before(layerElement);
    }
}
function getLayerElementById(id){
    return _listLayer.querySelector(`.layer[data-id="${id}"]`);
}

function createLayerElement(layer){
    let _layer = layer;
    
    let layerViewModel = app.layerViewModel(_layer.id);
    layerViewModel.registerEvent("OPACITY_LAYER", setOpacityLayer);
    layerViewModel.registerEvent("TOGGLE_HIDE_LAYER", setVisibilityLayer);
    layerViewModel.registerEvent("TOGGLE_LOCK_LAYER", setLockLayer);
    layerViewModel.registerEvent("RENAME_LAYER", setNameLayer);

    let layerElement = document.createElement("div");
    layerElement.dataset.id = _layer.id;
    layerElement.classList.add("layer");
    layerElement.onclick = ()=> _paneLayersViewModel.changeActiveLayer(_layer.id);
    

    let nameLayer = document.createElement("div");
    nameLayer.className = "text";
    nameLayer.className = "name-layer";
    let h5 = document.createElement("h5");
    h5.innerText = _layer.name;

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
        renameLayer(e);
    });
    function renameLayer(){
        if(editing) return;
        editing = true;

        let inpNameLayer = document.createElement("input");
        inpNameLayer.value = layer.name;
        inpNameLayer.type = "text";

        nameLayer.replaceChild(inpNameLayer, h5);
        inpNameLayer.focus();
        
        inpNameLayer.addEventListener("blur", done);
        inpNameLayer.addEventListener("keypress", function(e){
            if(e.keyCode == 13)
                done();
        });

        function done(){
            if(inpNameLayer.value != "" && inpNameLayer.value != layer.name){
                layerViewModel.rename(inpNameLayer.value);
            }
        
            nameLayer.replaceChild(h5, inpNameLayer);
            editing = false;
        }
    };
    nameLayer.append(h5);

    let btnHideLayer = document.createElement("button");
    let iconHideLayer = document.createElement("i");
    iconHideLayer.classList.add("fa");
    btnHideLayer.className = "hide-layer";
    btnHideLayer.append(iconHideLayer);
    btnHideLayer.onclick = ()=> layerViewModel.setIsVisible(!_layer.isVisible);
    
    let btnLockLayer = document.createElement("button");
    let iconLockLayer = document.createElement("i");
    iconLockLayer.classList.add("fa");
    btnLockLayer.className = "lock-layer";
    btnLockLayer.append(iconLockLayer);
    btnLockLayer.onclick = ()=> layerViewModel.setIsLock(!_layer.isLock);

    let btnGrabLayer = document.createElement("button");
    btnGrabLayer.className = "grab-layer";
    btnGrabLayer.innerHTML = "<i class=\"fa fa-grip-lines\"></i>";
    btnGrabLayer.addEventListener("mousedown", grabLayer);
    btnGrabLayer.addEventListener("touchstart", grabLayer);
    function grabLayer(e){
        let areaListLayer = document.getElementById("list-Layers");
        let _listLayer = areaListLayer.querySelectorAll("#list-Layers .layer");
        let abort = new AbortController();
        e.preventDefault();

        let onTracking = (e)=> {
            let elementLast;
            _listLayer.forEach(el => {
                let box  = el.getBoundingClientRect();

                el.classList.remove("after-indicator");
                el.classList.remove("before-indicator");

                if((e?.touches?.[0].clientY ?? e?.clientY) > box.y){
                    elementLast = el;
                }
            });

            if(!elementLast)
                _listLayer[0]?.classList.add("before-indicator");
            else
                elementLast?.classList.add("after-indicator");
        };
        let onRelease = (e)=>{
            let elementLast;
            _listLayer.forEach(el => {
                let box  = el.getBoundingClientRect();

                el.classList.remove("swap");
                if((e?.changedTouches?.[0].clientY ?? e?.clientY) > box.y){
                    elementLast = el;
                }
            });
            
            if(elementLast){
                elementLast?.classList.remove("after-indicator");
            } else {
                elementLast = _listLayer[0];
                elementLast?.classList.remove("before-indicator");
            }

            _paneLayersViewModel.moveLayerTo(_layer.id, elementLast.dataset.id);
            
            abort.abort();
        };

        window.addEventListener("mousemove", onTracking, {signal: abort.signal});
        window.addEventListener("touchmove", onTracking, {signal: abort.signal});
        
        window.addEventListener("mouseup", onRelease, { once: true });
        window.addEventListener("touchend", onRelease, { once: true });
        
    };
    
    layerElement.append(btnHideLayer);
    layerElement.append(nameLayer);
    layerElement.append(btnLockLayer);
    layerElement.append(btnGrabLayer);
    
    let inpOpacity = document.querySelector("input[name='opacity-layer']");
    inpOpacity.addEventListener("input", function() {
        layerViewModel.setOpacityLayer(parseFloat(this.value / 100.0));
    });

    setOpacityLayer(layer);
    setLockLayer(layer);
    setVisibilityLayer(layer);
    setNameLayer(layer);

    function setOpacityLayer(layer){
        _layer.opacity = layer.opacity;
        inpOpacity.value = _layer.opacity * 100.0;
        document.querySelector("#opacity-label h5").innerText = "Transparência " + inpOpacity.value + "%";
    }
    function setLockLayer(layer){
        _layer.isLock = layer.isLock;
        if(_layer.isLock){
            iconLockLayer.className = "fa fa-lock";
            return;
        }
        iconLockLayer.className = "fa fa-unlock";
    }
    function setVisibilityLayer(layer){
        _layer.isVisible = layer.isVisible;

        if(_layer.isVisible){
            iconHideLayer.className = "fa fa-eye";
            return;
        }

        iconHideLayer.className = "fa fa-eye-slash";
    }
    function setNameLayer(layer){
        _layer.name = layer.name;
        h5.innerText = _layer.name;
    }


    return Object.assign(layerElement);
}