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

    const offscreen = canvas.transferControlToOffscreen();
    const context = offscreen.getContext("2d", { willReadFrequently: true, desyncronized: true });

    let _scale = getMinScale();
    let sketchPositionX = getInitialPosition().x;
    let sketchPositionY = getInitialPosition().y;

    const buffer = new Uint8ClampedArray(sketchWidth*sketchHeight*4);
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

        renderSketch();
    }

    function putPixel(index, colorHex){
        buffer[index + INDEX_RED] = colorHex >> 24 & 0xFF;
        buffer[index + INDEX_GREEN] = colorHex >> 16 & 0xFF;
        buffer[index + INDEX_BLUE] = colorHex >> 8 & 0xFF;
        buffer[index + INDEX_ALFA] = colorHex >> 0 & 0xFF;
    }

    function renderSketch(){
        const viewportWidth = canvas.clientWidth;
        const viewportHeight = canvas.clientHeight;
        
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

        
        context.clearRect(0, 0, viewportWidth, viewportHeight);

        renderNativo(buffer, startVisibleX, endVisibleX, startVisibleY, endVisibleY);
        // renderLayers(startVisibleX, endVisibleX,  startVisibleY, endVisibleY);
    }

    function renderLayers(startVisibleX, endVisibleX,  startVisibleY, endVisibleY){
        listLayers.map(layer=>{
            render(layer.getBuffer(), startVisibleX, endVisibleX,  startVisibleY, endVisibleY);
        });
    }


    function render(originalBuffer, startVisibleX, endVisibleX, startVisibleY, endVisibleY){
        const visibleWidth = endVisibleX - startVisibleX;
        const visibleHeight = endVisibleY - startVisibleY;

        const bufferView = new Uint8ClampedArray(visibleWidth * visibleHeight * SIZE_PIXEL);
        
        let yIncrement = startVisibleY % _scale;
        let startXIncrement = startVisibleX % _scale;
        
        let startOriginalY = (startVisibleY - yIncrement) / _scale;
        let startOriginalX = (startVisibleX - startXIncrement) / _scale;
        let startIndex = (startOriginalY * sketchWidth + startOriginalX)*SIZE_PIXEL;
        let nextLine = sketchWidth * SIZE_PIXEL;
        
        let resizedIndex = 0;
        console.time("render")
        for (let y = 0; y < visibleHeight; y++) {
            let index = startIndex;
            let xIncrement = startXIncrement;
            
            for (let x = 0; x < visibleWidth; x++) {
                bufferView.set(originalBuffer.subarray(index, index+SIZE_PIXEL), resizedIndex);
                resizedIndex += SIZE_PIXEL;
                
                xIncrement++;
                if(xIncrement === _scale){
                    xIncrement = 0;
                    index += SIZE_PIXEL;
                }
            }
            
            yIncrement++;
            if(yIncrement === _scale){
                yIncrement = 0;
                startIndex += nextLine;
            }
        }
        console.timeEnd("render")

        var data = new ImageData(bufferView, visibleWidth, visibleHeight)

        context.putImageData(data, sketchPositionX + startVisibleX, sketchPositionY + startVisibleY)
    }

    function renderNativo(originalBuffer, startVisibleX, endVisibleX, startVisibleY, endVisibleY){

        console.time("render")
        
        let restoX = startVisibleX % _scale;
        let restoY = startVisibleY % _scale;

        let startOriginalY = Math.floor(startVisibleY / _scale);
        let startOriginalX = Math.floor(startVisibleX / _scale);

        let endOriginalY = Math.floor(endVisibleY / _scale);
        let endOriginalX = Math.floor(endVisibleX / _scale);

        let index = calcIndex(startOriginalX, startOriginalY, sketchWidth);
        let endindex = (endOriginalX * sketchWidth + endOriginalX)*SIZE_PIXEL;

        // console.log((startOriginalY * sketchWidth + startOriginalX)*SIZE_PIXEL)
        
        let positionY = sketchPositionY + startOriginalY;
        let positionX = sketchPositionX + startOriginalX;
        let compensationX = startOriginalX*SIZE_PIXEL;
        
        let largura = _scale;
        let altura = _scale;

        for (let originalY = startOriginalY; originalY < sketchHeight; originalY++) {

            for (let originalX = startOriginalX; originalX < sketchWidth; originalX++) {
                let ci = (originalY *sketchWidth + originalX) * SIZE_PIXEL
                
                if(index != ci)
                    console.error(index-ci)

                context.fillStyle = `rgba(${ originalBuffer.subarray(index, index+SIZE_PIXEL).join(",") })`;
                context.fillRect(positionX, positionY, largura, altura);
                
                index += SIZE_PIXEL;
                positionX += largura;
            }
            index += compensationX;

            positionX = sketchPositionX+startOriginalX;
            positionY += _scale;
        }
        console.timeEnd("render")
    }
    
    function getPixel(index){
        const startIndex = index;
        const endIndex = startIndex + SIZE_PIXEL;

        const colorHex = originalBuffer.slice(startIndex, endIndex).reduce((a,c)=>(a << 8n) | BigInt(c), 0n);
        return colorHex
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
    function isValidSize() { return (parseInt(sketchWidth) > 0 && parseInt(sketchWidth) <= 1200) || (parseInt(sketchHeight) > 0 && parseInt(sketchHeight) < 0) }


    function hoverCursor(cursor, brushStyle, brushSize){
        const pixelHover = tryGetPixelPosition(cursorPosition);

        const startIndex = calcIndex(x, y, _width);
        const endIndex = startIndex + SIZE_PIXEL;

        const colorHex = buffer.slice(startIndex, endIndex);

        const size = brushSize*_scale;
        
        context.fillStyle  = `rgba(${colorHex[0]}, ${colorHex[1]},${colorHex[2]},${colorHex[3]}}`;
        
        for(let b = 0; b <= brushStyle.length; b++){
            let x = b.x + pixelHover.x; 
            let y = b.y + pixelHover.y; 
            context.fillRect(x, y, size, size);
        }
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