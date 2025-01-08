import Graphics from '../../build/graphics/graphics.js'
import Layer from "./layer.js" 

const SIZE_PIXEL  = 4, 
      INDEX_RED   = 0, 
      INDEX_GREEN = 1, 
      INDEX_BLUE  = 2, 
      INDEX_ALFA  = 3;

      
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

    const context = canvas.getContext("2d", { willReadFrequently: true });
    let _scale = getMinScale();
    let sketchPositionX = getInitialPosition().x;
    let sketchPositionY = getInitialPosition().y;

    const originalBuffer = new Uint8ClampedArray(sketchWidth*sketchHeight*4);
    const listLayers = [];
    
    build();

    function build(){
        for (let x = 0; x < sketchWidth; x++) {
            for (let y = 0; y < sketchHeight; y++) {
                const index = calcIndex(x, y, sketchWidth)*SIZE_PIXEL;
            
                let checkeredColorHEX = (!(y&0x1) && x&0x1) || (!(x&0x1) && y&0x1) ? CHECKERED_LIGHT_COLOR_HEX : CHECKERED_DARK_COLOR_HEX;
                
                putPixel(index, checkeredColorHEX);
            }
        }

        render();
    }

    function putPixel(index, colorHex){
        originalBuffer[index + INDEX_RED] = colorHex >> 24 & 0xFF;
        originalBuffer[index + INDEX_GREEN] = colorHex >> 16 & 0xFF;
        originalBuffer[index + INDEX_BLUE] = colorHex >> 8 & 0xFF;
        originalBuffer[index + INDEX_ALFA] = colorHex >> 0 & 0xFF;
    }

    function render(){
        context.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
        renderBuffer(originalBuffer);
        renderLayers();
    }
    
    function renderBuffer(){
        const viewportWidth = canvas.clientWidth;
        const viewportHeight = canvas.clientHeight;
        
        const resizedWidth = getCurrentWidth();
        const resizedHeight = getCurrentHeight();
        
        let isVisibleX = sketchPositionX > viewportWidth || sketchPositionX < -resizedWidth;
        let isVisibleY = sketchPositionY > viewportHeight || sketchPositionY < -resizedHeight;
        
        if(isVisibleX || isVisibleY) return;
        
        let maxPositionX = viewportWidth - resizedWidth;
        let maxPositionY = viewportHeight - resizedHeight;
        
        let startVisibleX = (sketchPositionX < 0) ? -sketchPositionX : 0;
        let endVisibleX =  (sketchPositionX < maxPositionX) ? resizedWidth : resizedWidth - (sketchPositionX - maxPositionX);
        
        let startVisibleY = (sketchPositionY < 0) ? -sketchPositionY : 0;
        let endVisibleY = (sketchPositionY < maxPositionY) ? resizedHeight : resizedHeight - (sketchPositionY - maxPositionY);
        
        let visibleWidth = endVisibleX - startVisibleX;
        let visibleHeight = endVisibleY - startVisibleY;
        
        const bufferView = new Uint8ClampedArray(visibleWidth * visibleHeight *4);
        
        for (let x = startVisibleX; x < endVisibleX; x++) {
            const dX = Math.floor(x / _scale);
            
            for (let y = startVisibleY; y < endVisibleY; y++) {
                const dY = Math.floor(y / _scale);
                
                const index = (dY * sketchWidth + dX)*SIZE_PIXEL;
                const resizedIndex = ((y-startVisibleY) * visibleWidth + (x-startVisibleX))*SIZE_PIXEL;

                bufferView.set(originalBuffer.subarray(index, index+SIZE_PIXEL), resizedIndex);
            }
        }
        
        
        var data = new ImageData(bufferView, visibleWidth, visibleHeight)
        context.putImageData(data, sketchPositionX + startVisibleX, sketchPositionY + startVisibleY)
    }

    function renderLayers(){
        listLayers.map(layer=>{
            layer.getBuffer();
        });
    }

    function zoomIn(cursorPosition){
        var targetScale = _scale+1;
        
        if(targetScale > getMaxScale()) return;

        zoom(targetScale, cursorPosition);
    }

    function zoomOut(cursorPosition){
        var targetScale = _scale-1;

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


        // zoomPointX - (zoomPointX * targetScale / _scale) + (sketchPositionX * targetScale / _scale)

        // se zoom point == sketchPosition: moveTo = sketchPositionX
        // x = sketchPositionX - (sketchPositionX * targetScale / _scale) + (sketchPositionX * targetScale / _scale)
        // x = sketchPositionX
        
        // se zoom point == endOfAxis: moveTo = sketchPositionX+_width*(_scale−targetScale)
        // x = (sketchPositionX + _width * _scale) - ((sketchPositionX + _width * _scale) * targetScale / _scale) + (sketchPositionX * targetScale / _scale)
        // x = sketchPositionX+_width*(_scale−targetScale)
        

        // se zoom point == cursorPosition: moveTo = sketchPositionX+_width*(_scale−targetScale)
        // x = sketchPositionX - (cursorPosition.x * targetScale / _scale) + (sketchPositionX * targetScale / _scale)

        _scale = targetScale;

        moveTo({x, y});
    }

    function panning(fromCursorX, fromCursorY, toCursorX, toCursorY){
        let cursorDeltaX = fromCursorX - toCursorX;
        let cursorDeltaY = fromCursorY - toCursorY;

        moveTo({x: sketchPositionX-cursorDeltaX, y: sketchPositionY-cursorDeltaY})
    }

    function moveTo({x, y}){
        x = parseInt(x);
        y = parseInt(y);
        
        let initialPosition = getInitialPosition();

        let minLeftOffset = initialPosition.x;
        let maxLeftOffset = canvas.clientWidth - getCurrentWidth() - minLeftOffset;

        let minTopOffset = initialPosition.y;
        let maxTopOffset = canvas.clientHeight - getCurrentHeight() - minTopOffset;

        sketchPositionX = Math.min(minLeftOffset, Math.max(maxLeftOffset, x));
        sketchPositionY = Math.min(minTopOffset, Math.max(maxTopOffset, y));

        render();
    }

    function getMinScale(){ return Math.max(1, Math.min(Math.floor(canvas.clientHeight/sketchHeight),  Math.floor(canvas.clientWidth/sketchWidth))); }
    function getMaxScale(){ return getMinScale() + 10; }
    function getCurrentHeight(){ return sketchHeight * _scale; }
    function getCurrentWidth(){ return sketchWidth * _scale; }

    function calcIndex(row, col, lengthRow){ return col * lengthRow + row; }
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
    function isValidSize() { return (parseInt(sketchWidth) > 0 && parseInt(sketchWidth) < 1200) || (parseInt(_sketchHeight) > 0 && parseInt(_sketchHeight) < 0) }


    function setCursor(cursorName){
        canvas.style.cursor = cursorName;
    }

    const proto = Object.create(Graphics.prototype);
    proto.render = render;
    proto.zoomIn = zoomIn;
    proto.zoomOut = zoomOut;
    proto.panning = panning;
    proto.moveTo = moveTo;
    proto.setCursor = setCursor;
    proto.isItInsideTheSketch = isItInsideTheSketch;
    proto.tryGetPixelPosition = tryGetPixelPosition;

    return Object.seal(proto);
};