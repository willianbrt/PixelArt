import Graphics from '../../build/graphics/graphics.js'
import Layer from "./layer.js" 

const SIZE_PIXEL  = 4;
      
const CHECKERED_LIGHT_COLOR_HEX = 0xFFFFFFFF,
      CHECKERED_DARK_COLOR_HEX = 0xDDDDDDFF,
      CHECKERED_HOVER_LIGHT_COLOR_HEX = 0x555555FF,
      CHECKERED_HOVER_DARK_COLOR_HEX = 0x222222FF;

export default async function Sketch({
    canvas: canvas, 
    width: sketchWidth,
    height: sketchHeight
}){
    if(!isValidCanvas()) throw new Error("Objeto 'canvas' não encontrado.");
    if(!isValidSize()) throw new Error("Os parâmetros 'sketchWidth' e 'sketchHeight' devem conter uma valor entre 0 e 1200.");

    let graphics = await Graphics({
        wasmMemory: new WebAssembly.Memory({ initial: 256, maximum: 256 }),
    }).then(Module => {
        const { _build, _render, _freeBuffer, _putPixel, _getPixel, HEAPU8, _calculateIndex } = Module;
            
        function getPixel(x, y){
            return _getPixel(x, y) >>> 0;
        }

        const proto = Object.create(Graphics.prototype);
        proto.render = _render;
        proto.putPixel = _putPixel;
        proto.getPixel = getPixel;
        proto.free = _freeBuffer;
        proto.build = _build;
        proto.memory = HEAPU8;
        proto.calculateIndex = _calculateIndex;
        return Object.seal(proto);
    });

    const context = canvas.getContext("2d", { willReadFrequently: true });

    let _scale = getMinScale();
    let sketchPositionX = getInitialPosition().x;
    let sketchPositionY = getInitialPosition().y;
    
    const ptrBuffer = graphics.build(sketchWidth, sketchHeight);

    const listLayers = [];

    build();

    function build(){
        let isLightColor = true;
        let flagIsLightColor = isLightColor;
        for (let x = 0; x < sketchWidth; x++) {
            for (let y = 0; y < sketchHeight; y++) {
                let checkeredColorHEX = (isLightColor) ? CHECKERED_LIGHT_COLOR_HEX : CHECKERED_DARK_COLOR_HEX;

                graphics.putPixel(x, y, checkeredColorHEX);
                
                isLightColor = !isLightColor;
            }

            isLightColor = !flagIsLightColor;
            flagIsLightColor = isLightColor;
        }

        renderSketch();
    }

    function renderSketch(){
        const viewportWidth = canvas.clientWidth;
        const viewportHeight = canvas.clientHeight;
        
        context.clearRect(0, 0, viewportWidth, viewportHeight);

        const resizedWidth = getCurrentWidth();
        const resizedHeight = getCurrentHeight();
        
        let isNotVisibleX = sketchPositionX > viewportWidth || sketchPositionX < -resizedWidth;
        let isNotVisibleY = sketchPositionY > viewportHeight || sketchPositionY < -resizedHeight;
        
        if(isNotVisibleX || isNotVisibleY) return;
        
        let maxPositionX = viewportWidth - resizedWidth;
        let maxPositionY = viewportHeight - resizedHeight;
        
        let startVisibleX = (sketchPositionX < 0) ? -sketchPositionX : 0;
        let endVisibleX =  (sketchPositionX < maxPositionX) ? resizedWidth : resizedWidth - (sketchPositionX - maxPositionX);
        
        let startVisibleY = (sketchPositionY < 0) ? -sketchPositionY : 0;
        let endVisibleY = (sketchPositionY < maxPositionY) ? resizedHeight : resizedHeight - (sketchPositionY - maxPositionY);


        render(startVisibleX, endVisibleX,  startVisibleY, endVisibleY);
        renderLayers(startVisibleX, endVisibleX,  startVisibleY, endVisibleY);
    }

    function render(startVisibleX, endVisibleX,  startVisibleY, endVisibleY){
        let visibleWidth = endVisibleX - startVisibleX;
        let visibleHeight = endVisibleY - startVisibleY;

        const tempPtr = graphics.render(ptrBuffer, startVisibleX, endVisibleX, startVisibleY, endVisibleY, _scale);

        const bufferView = new Uint8ClampedArray(graphics.memory.buffer, tempPtr, visibleWidth*visibleHeight*SIZE_PIXEL);
        const data = new ImageData(bufferView, visibleWidth, visibleHeight);
        context.putImageData(data, sketchPositionX + startVisibleX, sketchPositionY + startVisibleY);

        graphics.free(tempPtr);
    }

    function renderLayers(startVisibleX, endVisibleX,  startVisibleY, endVisibleY){
        listLayers.map(layer=>{
            layer.render(startVisibleX, endVisibleX,  startVisibleY, endVisibleY);
        });
    }

    function zoomIn(cursorPosition){
        var targetScale = _scale + 1;
        
        if(targetScale > getMaxScale()) return;

        zoom(targetScale, cursorPosition);
    }

    function zoomOut(cursorPosition){
        var targetScale = _scale - 1;

        if(targetScale < getMinScale()) return;

        zoom(targetScale, cursorPosition);
    }

    function zoom(targetScale, cursorPosition){
        if(targetScale < 0) return;

        let currentWidth = getCurrentWidth();
        let currentHeight = getCurrentHeight();

        let endOfAxisX = sketchPositionX + currentWidth;
        let endOfAxisY = sketchPositionY + currentHeight;
        
        let zoomPointX = Math.min(endOfAxisX, Math.max(sketchPositionX, cursorPosition.x));
        let zoomPointY = Math.min(endOfAxisY, Math.max(sketchPositionY, cursorPosition.y));

        let x = zoomPointX - (zoomPointX - sketchPositionX) * (targetScale / _scale);
        let y = zoomPointY - (zoomPointY - sketchPositionY) * (targetScale / _scale);

        _scale = targetScale;

        moveTo({x, y});
    }

    function panning(fromCursorX, fromCursorY, toCursorX, toCursorY){
        let cursorDeltaX = fromCursorX - toCursorX;
        let cursorDeltaY = fromCursorY - toCursorY;

        moveTo({x: sketchPositionX-cursorDeltaX, y: sketchPositionY-cursorDeltaY})
    }

    function moveTo({x, y}){
        let initialPosition = getInitialPosition();

        let minLeftOffset = initialPosition.x;
        let maxLeftOffset = canvas.clientWidth - getCurrentWidth() - minLeftOffset;

        let minTopOffset = initialPosition.y;
        let maxTopOffset = canvas.clientHeight - getCurrentHeight() - minTopOffset;

        sketchPositionX = Math.min(minLeftOffset, Math.max(maxLeftOffset, Math.floor(x)));
        sketchPositionY = Math.min(minTopOffset, Math.max(maxTopOffset, Math.floor(y)));

        renderSketch();
    }

    function getMinScale(){ return Math.max(1, Math.min(Math.floor(canvas.clientHeight/sketchHeight),  Math.floor(canvas.clientWidth/sketchWidth))); }
    function getMaxScale(){ return sketchWidth <= 16 || sketchHeight <= 16 ? 1 : getMinScale() + 10; }
    function getCurrentHeight(){ return sketchHeight * _scale; }
    function getCurrentWidth(){ return sketchWidth * _scale; }

    function getInitialPosition(){ 
        return {
            x: Math.floor((canvas.clientWidth - (sketchWidth*getMinScale())) / 2),
            y: Math.floor((canvas.clientHeight - (sketchHeight*getMinScale())) / 2),
        }
    }

    function tryGetPixelPosition(cursorPosition){
        if(!isItInsideTheSketch(cursorPosition)) throw new Error("Cursor fora da area de desenho.");

        let cursorOffsetX = cursorPosition.x - sketchPositionX;
        let cursorOffsetY = cursorPosition.y - sketchPositionY;

        return {
            x: Math.floor(cursorOffsetX/_scale),
            y: Math.floor(cursorOffsetY/_scale),
        }
    }
    
    function isItInsideTheSketch(cursorPosition){ 
        let isItInsideTheSketchX = cursorPosition.x >= sketchPositionX && cursorPosition.x <= sketchPositionX + getCurrentWidth();
        let isItInsideTheSketchY = cursorPosition.y >= sketchPositionY && cursorPosition.y <= sketchPositionY + getCurrentHeight();

        return isItInsideTheSketchX && isItInsideTheSketchY;
    }
    
    
    function isValidCanvas() { return canvas.nodeName === 'CANVAS'}
    function isValidSize() { return (parseInt(sketchWidth) > 0 && parseInt(sketchWidth) <= 1200) || (parseInt(sketchHeight) > 0 && parseInt(sketchHeight) < 0) }

    function hover(cursorPosition, stencils, brushSize){
        const pixelHover = tryGetPixelPosition(cursorPosition);

        context.fillStyle = hexToRgba(((pixelHover.x + pixelHover.y) | 0x1) ? CHECKERED_HOVER_LIGHT_COLOR_HEX : CHECKERED_HOVER_DARK_COLOR_HEX);
        
        const size = brushSize*_scale;
        for(let stencil in stencils){
            let x = stencil.x + pixelHover.x;
            let y = stencil.y + pixelHover.y;
            context.fillRect(x, y, size, size);
        }
    }
    function brush(cursorPosition, stencils, brushSize){
        const pixelHover = tryGetPixelPosition(cursorPosition);
        const colorHex = graphics.getPixel(pixelHover.x, pixelHover.y);
        
        context.fillStyle = `#${ colorHex.toString(16) }`;
        
        const size = brushSize*_scale;
        for(let stencil in stencils){
            let x = stencil.x + pixelHover.x;
            let y = stencil.y + pixelHover.y;
            context.fillRect(x, y, size, size);
        }
    }


    function hexToRgba(colorHex){
        let red = colorHex >> 24 & 0xFF;
        let blue = colorHex >> 16 & 0xFF;
        let green = colorHex >> 8 & 0xFF;
        let alpha = colorHex & 0xFF;
        
        return `rgba(${red}, ${blue}, ${green}, ${alpha})`; 
    }

    function setCursor(cursorName){
        canvas.style.cursor = cursorName;
    }

    const proto = Object.create(Graphics.prototype);
    proto.render = renderSketch;
    proto.zoomIn = zoomIn;
    proto.zoomOut = zoomOut;
    proto.panning = panning;
    proto.moveTo = moveTo;
    proto.setCursor = setCursor;
    proto.isItInsideTheSketch = isItInsideTheSketch;
    proto.tryGetPixelPosition = tryGetPixelPosition;

    return Object.seal(proto);
};