let _frameViewModel;
let _isActiveFrame = false;
let _listLayer;

export function buildPaneLayers(frameViewModel){
    _frameViewModel = frameViewModel;

    _listLayer = document.getElementById("list-Layers");
    _listLayer.querySelectorAll(".layer")
             .forEach((e)=>e.remove());

    let layers = frameViewModel.getAllLayers();
    let _activeLayer = frameViewModel.getActiveLayer();
    for(let i = 0; i < layers.size(); i++){
        let layerElement = createLayerElement(layers.get(i));
        layerElement.setIsActive(layers.get(i).getID().toString() == _activeLayer.getID().toString());
        _listLayer.prepend(layerElement);
    }

    
    let btnAddLayer = document.getElementById("add-layer");
    let btnRemoveLayer = document.getElementById("remove-layer");
    let btnCloneLayer = document.getElementById("duplicate-layer");
    let btnMoveDown = document.getElementById("move-down-layer");
    let btnMoveUp = document.getElementById("move-up-layer");

    btnAddLayer.onclick = ()=> _frameViewModel.createLayer();
    btnRemoveLayer.onclick = ()=> _frameViewModel.removeActiveLayer();
    btnMoveDown.onclick = ()=> _frameViewModel.moveDownActiveLayer();
    btnMoveUp.onclick = ()=> _frameViewModel.moveUpActiveLayer();
    btnCloneLayer.onclick = ()=> _frameViewModel.duplicateActiveLayer();

    _frameViewModel.registerEvent("ADD_LAYER", onAddLayer);
    _frameViewModel.registerEvent("REMOVE_LAYER", onRemoveLayer);
    _frameViewModel.registerEvent("MOVE_LAYER_TO", onMoveLayerTo);
    _frameViewModel.registerEvent("CHANGE_ACTIVE_LAYER", onChangeActiveLayer);
}

function onAddLayer(event){
    const { layer, index, is_active } = event;
    if(!_isActiveFrame) return;

    let layerElement = createLayerElement(layer);

    let layers = _listLayer.querySelectorAll("div.layer");
    if(layers.length > 0)
        layers[layers.length-index].before(layerElement);
    else
        _listLayer.prepend(layerElement);
}
function onRemoveLayer(event){
    const { layer, index, is_active } = event;
    if(!_isActiveFrame) return;

    let layerElement = getLayerElementById(layer.getID().toString());

    if(!layerElement) return;

    layerElement.remove();
}
function onChangeActiveLayer(event){
    const { layer, index, is_active } = event;
    if(!_isActiveFrame) return;

    let layerElement = getLayerElementById(layer.getID().toString());
    _listLayerElements.push(layerElement);

    if(!layerElement)
        layerElement = createLayerElement(layer);

    setOpacity(layer.getOpacity());
}
function onMoveLayerTo(event){
    const { layer, index } = event;
    if(!_isActiveFrame) return;

    let layerElement = getLayerElementById(layer.getID().toString());
    if(!layerElement){
        layerElement = createLayerElement(layer);
        return;
    }

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
    return _listLayer.find((l)=> { if(l.dataset.id == id) return l;});
}
function createLayerElement(module, layer){
    let _isActive = false;
    
    let layerViewModel = new module.LayerViewModel();
    layerViewModel.registerEvent("OPACITY_LAYER", onOpacityLayer);
    layerViewModel.registerEvent("TOGGLE_HIDE_LAYER", onToggleHideLayer);
    layerViewModel.registerEvent("TOGGLE_LOCK_LAYER", onToggleLockLayer);
    layerViewModel.registerEvent("RENAME_LAYER", onRenameLayer);


    let layerElement = document.createElement("div");
    layerElement.classList.add("layer");
    layerElement.dataset.id = layer.getID().toString();
    layerElement.onclick = ()=> layerViewModel.setChangeActiveFrame(layer.getID());
    

    let nameLayer = document.createElement("div");
    nameLayer.className = "text";
    nameLayer.className = "name-layer";
    let h5 = document.createElement("h5");

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
        inpNameLayer.value = layer.getName();
        inpNameLayer.type = "text";

        nameLayer.replaceChild(inpNameLayer, h5);
        inpNameLayer.focus();
        
        inpNameLayer.addEventListener("blur", done);
        inpNameLayer.addEventListener("keypress", function(e){
            if(e.keyCode == 13)
                done();
        });

        function done(){
            if(inpNameLayer.value != "" && inpNameLayer.value != layer.getName()){
                layerViewModel.rename(inpNameLayer.value);
            }
        
            nameLayer.replaceChild(h5, inpNameLayer);
            editing = false;
        }
    };
    nameLayer.append(h5);
    updateName();

    
    let btnHideLayer = document.createElement("button");
    let iconHideLayer = document.createElement("i");
    btnHideLayer.className = "hide-layer";
    btnHideLayer.append(iconHideLayer);
    btnHideLayer.onclick = ()=> layerViewModel.setVisible(!layer.isVisible());
    updateIsVisible();
    
    let btnLockLayer = document.createElement("button");
    let iconLockLayer = document.createElement("i");
    btnLockLayer.className = "lock-layer";
    btnLockLayer.append(iconLockLayer);
    btnLockLayer.onclick = ()=> layerViewModel.setLock(!layer.isLock());
    updateIsLock();
    
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
                elementLast?.after(layerElement);
            } else {
                elementLast = _listLayer[0];
                elementLast?.classList.remove("before-indicator");
                areaListLayer.prepend(layerElement);
            }

            layerViewModel.moveLayerTo(layer.getID(), elementLast.dataset.index);
            
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

    function updateName(){
        h5.innerText = layer.getName();
    }
    function updateIsVisible(){
        if(layer.isVisible()){
            iconLockLayer.classList.remove("fa-eye-slash");
            iconLockLayer.classList.add("fa-eye");
        }else {
            iconLockLayer.classList.remove("fa-eye");
            iconLockLayer.classList.add("fa-eye-slash");
        }
    }
    function updateIsLock(){
        if(layer.isLock()){
            iconLockLayer.classList.remove("fa-unlock");
            iconLockLayer.classList.add("fa-lock");
        }else {
            iconLockLayer.classList.remove("fa-lock");
            iconLockLayer.classList.add("fa-unlock");
        }
    }
    function setIsActive(isActive){
        if(isActive){
            _listLayer.querySelectorAll("div.layer.active")
                     .forEach((l)=>l.classList.remove("active"));
            layerElement?.classList.toggle("active", true);
        }
        _isActive = isActive;
    }
    function updateOpacity(){
        inpOpacity.value = layer.getOpacity() * 100.0;
        document.querySelector("#opacity-label h5").innerText = "Transparência " + inpOpacity.value + "%";
    }

    function onOpacityLayer(event){
        const { layer, index, is_active } = event;
        if(!is_active) return;

        setOpacity(layer.getOpacity());
    }
    function onToggleLockLayer(event){
        const { layer, index, is_active } = event;
        if(!_isActiveFrame) return;
        
        let icon = layerElement.querySelector("i");
        if(layer.isLock()){
            icon.classList.replace("fa-unlock","fa-lock");
            return;
        }
        icon.classList.replace("fa-lock", "fa-unlock");
    }
    function onToggleHideLayer(event){
        const { layer, index, is_active } = event;
        if(!_isActiveFrame) return;

        let icon = layerElement.querySelector("i");

        if(layer.isVisible()){
            icon.classList.replace("fa-eye-slash", "fa-eye");
            layerElement.classList.toggle("hidden-layer", false);
            return;
        }

        icon.classList.replace("fa-eye", "fa-eye-slash");
        layerElement.classList.toggle("hidden-layer", true);
    }
    function onRenameLayer(event){
        const { layer, index } = event;
        if(!_isActiveFrame) return;

        const h5 = layerElement.querySelector("h5");
        h5.innerText = layer.getName();
    }

    return Object.assign(layerElement, {
        updateName,
        updateIsVisible,
        updateIsLock,
        setIsActive
    });
}

/*

function createLayerElement(layerViewModel, layer, index){
    let layers = _listLayer.querySelectorAll("div.layer");
    
    let layerElement = document.createElement("div");
    let nameLayer = document.createElement("div");
    let h5 = document.createElement("h5");
    let btnHideLayer = document.createElement("button");
    let btnLockLayer = document.createElement("button");
    let btnGrabLayer = document.createElement("button");
    
    layerElement.classList.add("layer");
    layerElement.dataset.id = layer.getID().toString();
    layerElement.dataset.index = index;
    nameLayer.className = "text";
    nameLayer.className = "name-layer";
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
        _listLayer.prepend(layerElement);
    
    layerElement.onclick = ()=> layerViewModel.setChangeActiveFrame(layer.getID());
    btnLockLayer.onclick = ()=> layerViewModel.setLock(!layer.isLock());
    btnHideLayer.onclick = ()=> layerViewModel.setVisible(!layer.isVisible());
    
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
        renameLayer(e);
    });

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
                elementLast?.after(layerElement);
            } else {
                elementLast = _listLayer[0];
                elementLast?.classList.remove("before-indicator");
                areaListLayer.prepend(layerElement);
            }

            layerViewModel.moveLayerTo(layer.getID(), elementLast.dataset.index);
            
            abort.abort();
        };

        window.addEventListener("mousemove", onTracking, {signal: abort.signal});
        window.addEventListener("touchmove", onTracking, {signal: abort.signal});
        
        window.addEventListener("mouseup", onRelease, { once: true });
        window.addEventListener("touchend", onRelease, { once: true });
        
    };
    function renameLayer(ev){
        if(editing) return;
        editing = true;

        let inpNameLayer = document.createElement("input");
        inpNameLayer.value = layer.getName();
        inpNameLayer.type = "text";

        nameLayer.replaceChild(inpNameLayer, h5);
        inpNameLayer.focus();
        
        inpNameLayer.addEventListener("blur", done);
        inpNameLayer.addEventListener("keypress", function(e){
            if(e.keyCode == 13)
                done();
        });

        function done(){
            if(inpNameLayer.value != "" && inpNameLayer.value != layer.getName()){
                layerViewModel.rename(inpNameLayer.value);
            }
        
            nameLayer.replaceChild(h5, inpNameLayer);
            editing = false;
        }
    };
}

*/