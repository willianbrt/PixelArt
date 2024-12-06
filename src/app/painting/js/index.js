var clientWidth;
var clientHeight;

window.onload = ()=>{
    const canvas = document.querySelector("#paintingCanvas");
    const context = canvas.getContext("2d");
    
    const canvasHeight = canvas.clientHeight;
    const canvasWidth = canvas.clientWidth;

    var buffer = [clientWidth][clientHeight];    

    canvas.addEventListener("mousedown", onMouseDown);
    canvas.addEventListener("mouseup", onMouseUp);
    canvas.addEventListener("mousemove", onMouseMove);
    canvas.addEventListener("wheel", zoom);
    canvas.addEventListener("auxclick", onWhellClick);
};


function onMouseMove(event){
    // TODO: IMPLEMENTAR FUNÇÃO
}

function onWhellClick(event){
    const WHELL_BUTTON = 1;
    if(event.button == WHELL_BUTTON){
        // TODO: IMPLEMENTAR AÇÃO
    }
}

function zoomIn(x, y) {
    // TODO: IMPLEMENTAR FUNÇÃO
}
function zoomOut(x, y) {
    // TODO: IMPLEMENTAR FUNÇÃO
}
function zoom(event){

    // TODO: IMPLEMENTAR FUNÇÃO
}

function move(x, y){
    // TODO: IMPLEMENTAR FUNÇÃO
}


function onMouseDown(event){
    // TODO: IMPLEMENTAR FUNÇÃO
}
function onMouseUp(event){
    // TODO: IMPLEMENTAR FUNÇÃO
}