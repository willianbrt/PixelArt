
mergeInto(LibraryManager.library,{
    draw: draw,
    get_viewport_width: getViewportWidth,
    get_viewport_height: getViewportHeight,
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
function getViewportWidth() { return Module.canvas.clientWidth; }
function getViewportHeight(){ return Module.canvas.clientHeight; }
