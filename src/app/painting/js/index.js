var clientWidth = 4;
var clientHeight = 4;

var canvas;
var context;

const SIZE_PIXEL = 4;
const INDEX_RED   = 0;
const INDEX_GREEN = 1;
const INDEX_BLUE  = 2;
const INDEX_ALFA  = 3;


window.onload = ()=>{
    canvas = document.querySelector("#paintingCanvas");
    context = canvas.getContext("2d");

    if(canvas === undefined || context === undefined )
        throw new Error("Area de pintura não encontrada");

    const canvasHeight = canvas.clientHeight;
    const canvasWidth = canvas.clientWidth;
    
    var colorHEX = 0xff0000ff;
    draw(getMiddlePoint(canvasWidth, clientWidth), getMiddlePoint(canvasHeight, clientHeight), clientWidth, clientHeight, colorHEX);
    resize(context.getImageData(0, 0, canvas.width, canvas.height), 10, 10, 316, 316);

    canvas.addEventListener("mousedown", onMouseDown);
    canvas.addEventListener("mouseup", onMouseUp);
    canvas.addEventListener("mousemove", onMouseMove);
    canvas.addEventListener("wheel", onScroll);
    canvas.addEventListener("auxclick", onWhellClick);
};

function draw(startX, startY, width, height, colorHEX){
    let imageData = new ImageData(width, height);
    const buffer = imageData.data;

    for (let x = 0; x < width; x++) {
        for (let y = 0; y < height; y++) {
            const index = (y * width + x) * 4;
            buffer[index + INDEX_RED]   = colorHEX >> 8*INDEX_RED   & 0xFF;
            buffer[index + INDEX_GREEN] = colorHEX >> 8*INDEX_GREEN & 0xFF;
            buffer[index + INDEX_BLUE]  = colorHEX >> 8*INDEX_BLUE  & 0xFF;
            buffer[index + INDEX_ALFA]  = colorHEX >> 8*INDEX_ALFA  & 0xFF;
        }
    }

    context.putImageData(imageData, startX, startY);
}

async function resize(imageData, scaleX, scaleY, cursorX, cursorY){
    if(scaleX < 1)
        throw new Error("O Valor de scaleX deve ser maior do que 1.");

    if(scaleY < 1)
        throw new Error("O Valor de scaleY deve ser maior do que 1.");

    const originalHeight = imageData.height;
    const originalWidth = imageData.width;
    
    let newImageData = new ImageData(originalWidth*scaleX, originalHeight*scaleY);
  
    for (let x = 0; x < originalWidth; x++) {
        for (let y = 0; y < originalHeight; y++) {
            const index = calcIndex(x, y, originalHeight);

            const dX = x*scaleX;
            const dY = y*scaleY;

            for (let iX = 0; iX < scaleX; iX++) {
                for (let iY = 0; iY < scaleY; iY++) {
                    const newIndex = (dY + iY) + (dX + iX)*newImageData.width;
                    
                    newImageData.data[newIndex*SIZE_PIXEL + INDEX_RED]    = imageData.data[index*SIZE_PIXEL + INDEX_RED];
                    newImageData.data[newIndex*SIZE_PIXEL + INDEX_GREEN]  = imageData.data[index*SIZE_PIXEL + INDEX_GREEN];
                    newImageData.data[newIndex*SIZE_PIXEL + INDEX_BLUE]   = imageData.data[index*SIZE_PIXEL + INDEX_BLUE];
                    newImageData.data[newIndex*SIZE_PIXEL + INDEX_ALFA]   = imageData.data[index*SIZE_PIXEL + INDEX_ALFA];
                }
            }
        }
    }

    let middlePointX = originalHeight / 2;
    let middlePointY = originalWidth / 2;

    let cursorDisplacementX = middlePointX - cursorX;
    let cursorDisplacementY = middlePointY - cursorY;

    let displacementX = cursorDisplacementX + middlePointX * (1-scaleX);
    let displacementY = cursorDisplacementY + middlePointY * (1-scaleY);
    
    context.putImageData(newImageData, Math.floor(displacementX), Math.floor(displacementY));
}

function calcIndex(i, j, sizeColumn){
    return i*sizeColumn + j;
}

function getMiddlePoint(parentSize, childrenSize){
    return Math.round((parentSize - childrenSize) / 2);
}

function onMouseMove(event){
    // TODO: IMPLEMENTAR FUNÇÃO
}

function onWhellClick(event){
    const WHELL_BUTTON = 1;
    if(event.button == WHELL_BUTTON){
        // TODO: IMPLEMENTAR AÇÃO
    }
}
function onScroll(event){
    let positionCursorX = event.clientX;
    let positionCursorY = event.clientY;
    let offsetHeight = event.srcElement.offsetHeight;
    let offsetLeft = event.srcElement.offsetLeft;
    let offsetTop = event.srcElement.offsetTop;
    let offsetWidth = event.srcElement.offsetWidth;

    // TODO: IMPLEMENTAR FUNÇÃO
}

function zoomIn(x, y) {
    // TODO: IMPLEMENTAR FUNÇÃO
}
function zoomOut(x, y) {
    // TODO: IMPLEMENTAR FUNÇÃO
}
function zoom(event){
    // TODO: IMPLEMENTAR FUNÇÃO
    
    let scale = 0;
    scale += event.deltaY;
    console.info("Zoom:", scale)

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