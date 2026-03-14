
mergeInto(LibraryManager.library,{
    renderCanvas: renderCanvas,
    clear: clear,
    get_viewport_width: getViewportWidth,
    get_viewport_height: getViewportHeight,
});

function renderCanvas(
    projectWidth, projectHeight,
    screen, length,
    viewportWidth, viewportHeight,
    x, y,
    nRows, nCols){
    const canvas = Module.canvas || document.getElementById("canvas");

    if(!canvas){
        console.error("Canvas não encontrado");
        return;
    }

    canvas.width = projectWidth*nRows;
    canvas.height = projectHeight*nCols;

    const context = canvas.getContext("2d");
    const ptr = screen;
    const width = viewportWidth;
    const height = viewportHeight;

    const buffer = new Uint8ClampedArray(HEAPU8.buffer, ptr, length*4);
    const data = new ImageData(buffer, width);

    for(let r = 0; r < nRows; r++){
        for(let c = 0; c < nCols; c++){
            context.clearRect(x + width*r, y + height*c, width, height);
            context.putImageData(data, x + width*r, y + height*c);
        }
    }
}

function clear(viewportWidth, viewportHeight, x, y){
    const context = Module.canvas.getContext("2d");
    context.clearRect(x, y, viewportWidth, viewportHeight);
}
function getViewportWidth() { return Module.canvas.clientWidth; }
function getViewportHeight(){ return Module.canvas.clientHeight; }
