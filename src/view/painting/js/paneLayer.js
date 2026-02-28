import { app } from "./app.js"

let _paneLayersViewModel;
let _listLayer;
let _layers = [];

let inpOpacity;
export function buildPaneLayers(paneLayersViewModel){
    _paneLayersViewModel = paneLayersViewModel;

    inpOpacity = document.querySelector("input[name='opacity-layer']");

    ["mousedown", "touchstart"].forEach((eventType)=> function() {
        inpOpacity.addEventListener(eventType, function() {
            _paneLayersViewModel.beginChangeActiveLayerOpacity();
        });
    });
    inpOpacity.addEventListener("input", function() {
        _paneLayersViewModel.onChangeActiveLayerOpacity(parseFloat(this.value / 100.0));
    });
    ["mouseup", "touchend"].forEach((eventType)=> function() {
        inpOpacity.addEventListener(eventType, function() {
            _paneLayersViewModel.endChangeActiveLayerOpacity();
        });
    });

    _listLayer = document.getElementById("list-Layers");

    _layers = _layers.filter((e)=> e.element.remove());
    for(let i = 0; i < _paneLayersViewModel.getNumberLayers(); i++){
        onAddLayer(_paneLayersViewModel.getLayerByIndex(i), i);
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
    let layerObject = createLayerElement(layer);

    let layers = _listLayer.querySelectorAll("div.layer");
    
    if(_layers.length > 0)
        layers[layers.length-index].before(layerObject.element);
    else
        _listLayer.prepend(layerObject.element);

    _layers.splice(index, 0, layerObject);
}
function onRemoveLayer(id){
    let layerObject = getLayerElementById(id);
    if(!!layerObject) layerObject.element.remove();

    const index = _layers.indexOf(layerObject);
    if(index > -1) _layers.splice(index, 1);
}
function onChangeActiveLayer(id){
    let layerObject = getLayerElementById(id);

    _layers.forEach((l)=>l.setIsActive(false));

    layerObject.setIsActive(true);
}
function onMoveLayerTo(id, index){
    let layerObject = getLayerElementById(id);

    const layerElement = layerObject.element;
    if (layerElement.compareDocumentPosition(_layers[index].element) & Node.DOCUMENT_POSITION_FOLLOWING) {
        _layers[index].element.after(layerElement);
    } else {
        _layers[index].element.before(layerElement);
    }

    const flag = _layers.splice(_layers.indexOf(layerObject), 1)[0];
    _layers.splice(index, 0, flag);
}
function getLayerElementById(id){
    return _layers.find((l) => l.getID() == id);
}


function createLayerElement(layer){
    let _layer = layer;
    
    let layerViewModel = app.layerViewModel(_layer.id);
    layerViewModel.registerEvent("OPACITY_LAYER", setOpacityLayer);
    layerViewModel.registerEvent("IS_VISIBLE_LAYER", setVisibilityLayer);
    layerViewModel.registerEvent("IS_LOCK_LAYER", setLockLayer);
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
        
        inpNameLayer.addEventListener("keydown", function(e){
            if(e.keyCode == 13){
                inpNameLayer.blur();
            }
        });
        inpNameLayer.addEventListener("blur", ()=> {
            layerViewModel.setName(inpNameLayer.value);
            nameLayer.replaceChild(h5, inpNameLayer);
            editing = false;
        });
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
    
    setOpacityLayer(_layer.opacity);
    setLockLayer(_layer.isLock);
    setVisibilityLayer(_layer.isVisible);
    setNameLayer(_layer.name);
    setIsActive(_layer.isActive);

    function setOpacityLayer(opacity){
        _layer.opacity = opacity;
        updateOpacityRange();
    }
    function setLockLayer(isLock){
        _layer.isLock = isLock;
        if(_layer.isLock){
            iconLockLayer.className = "fa fa-lock";
            return;
        }
        iconLockLayer.className = "fa fa-unlock";
    }
    function setVisibilityLayer(isVisible){
        _layer.isVisible = isVisible;

        if(_layer.isVisible){
            iconHideLayer.className = "fa fa-eye";
            return;
        }

        iconHideLayer.className = "fa fa-eye-slash";
    }
    function setNameLayer(name){
        _layer.name = name;
        h5.innerText = _layer.name;
    }
    function setIsActive(isActive){
        _layer.isActive = isActive;
        layerElement?.classList.toggle("active", isActive);
        if(isActive)
            setOpacityLayer(_layer.opacity);
    }
    function updateOpacityRange(){
        if(!_layer.isActive) return;

        inpOpacity.value = _layer.opacity * 100.0;
        document.querySelector("#opacity-label h5").innerText = "Transparência " + inpOpacity.value + "%";
    }
    return Object.freeze({
        element: layerElement,
        setIsActive,
        getID:()=>{ return _layer.id; }
    });
}