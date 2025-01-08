import { Layer } from "./layer.js";


const SIZE_PIXEL  = 4, 
      INDEX_RED   = 0, 
      INDEX_GREEN = 1, 
      INDEX_BLUE  = 2, 
      INDEX_ALFA  = 3;

const CHECKERED_LIGHT_COLOR_HEX = 0xFFFFFFFF,
      CHECKERED_DARK_COLOR_HEX = 0xDDDDDDFF,
      CHECKERED_HOVER_LIGHT_COLOR_HEX = 0x555555FF,
      CHECKERED_HOVER_DARK_COLOR_HEX = 0x222222FF;


export function Draft(options) {
    let {
        canvas: _canvas,
        sketchWidth: _sketchWidth,
        sketchHeight:_sketchHeight 
    } = options;
    
    if(!isValidCanvas()) throw new Error("Objeto 'canvas' não encontrado.");
    if(!isValidSize()) throw new Error("Os parâmetros 'sketchWidth' e 'sketchHeight' devem conter uma valor entre 0 e 1200.");
    
    const context = _canvas.getContext("2d");

    _canvas.addEventListener("mouseout", (event)=>{ leaveHover(); });
    
    let _scale = getMinScale();

    let buffer = new Uint8ClampedArray(_sketchWidth * _sketchHeight * 4);

    let _sketchPositionX = calcStartPosition(_canvas.clientWidth, getCurrentWidth());
    let _sketchPositionY = calcStartPosition(_canvas.clientHeight, getCurrentHeight());
    
    let layerList = [];

    let flagRowHover, flagColumnHover;
    let onHover = (cursorX, cursorY)=>{
        let cursorOffsetX = cursorX - _sketchPositionX;
        let cursorOffsetY = cursorY - _sketchPositionY;

        let isItInsideTheSketchX = cursorOffsetX < 0 || cursorOffsetX > getCurrentWidth();
        let isItInsideTheSketchY = cursorOffsetY < 0 || cursorOffsetY > getCurrentHeight();

        leaveHover();

        if(isItInsideTheSketchX || isItInsideTheSketchY)
            return;


        let bufferHover = new Uint8ClampedArray(_scale*_scale*4);
        
        flagRowHover = Math.floor(cursorOffsetX / _scale);
        flagColumnHover= Math.floor(cursorOffsetY / _scale);
        
        let checkeredColorHEX = (!(flagRowHover&0x1) && flagColumnHover&0x1) || (!(flagColumnHover&0x1) && flagRowHover&0x1) ? CHECKERED_HOVER_LIGHT_COLOR_HEX : CHECKERED_HOVER_DARK_COLOR_HEX;
        
        for (let index = 0; index < _scale*_scale; index++) {            
            bufferHover[index * SIZE_PIXEL + INDEX_RED] = checkeredColorHEX >> 8*INDEX_RED & 0xFF;
            bufferHover[index * SIZE_PIXEL + INDEX_GREEN] = checkeredColorHEX >> 8*INDEX_GREEN & 0xFF;
            bufferHover[index * SIZE_PIXEL + INDEX_BLUE] = checkeredColorHEX >> 8*INDEX_BLUE & 0xFF;
            bufferHover[index * SIZE_PIXEL + INDEX_ALFA] = checkeredColorHEX >> 8*INDEX_ALFA & 0xFF;
        }

        const dX = flagRowHover*_scale;
        const dY = flagColumnHover*_scale;
         
    };

    let leaveHover = ()=>{
        render();
    };

    let zoomIn = (cursorX, cursorY)=>{
        var targetScale = _scale+1;

        if(targetScale > getMaxScale()) return;

        let currentWidth = getCurrentWidth();
        let currentHeight = getCurrentHeight();

        let lastPixelPositionX = _sketchPositionX + currentWidth;
        let lastPixelPositionY = _sketchPositionY + currentHeight;
        
        let zoomPointX = Math.min(lastPixelPositionX, Math.max(_sketchPositionX, cursorX));
        let zoomPointY = Math.min(lastPixelPositionY, Math.max(_sketchPositionY, cursorY));

        let x = zoomPointX - (zoomPointX - _sketchPositionX) * (targetScale / _scale);
        let y = zoomPointY - (zoomPointY - _sketchPositionY) * (targetScale / _scale);
        setPositionTo(x, y);
        
        _scale = targetScale;
        
        render();
    };

    let zoomOut = ()=>{
        var targetScale = _scale-1;

        if(targetScale < getMinScale()) return;

        let currentWidth = getCurrentWidth();
        let currentHeight = getCurrentHeight();

        let x = _sketchPositionX - (_sketchWidth*targetScale - currentWidth);
        let y = _sketchPositionY - (_sketchHeight*targetScale - currentHeight);
        setPositionTo(x, y);
        
        _scale = targetScale;

        render();
    };
    
    let panning = (fromCursorX, fromCursorY, toCursorX, toCursorY)=>{
        let cursorDeltaX = fromCursorX - toCursorX;
        let cursorDeltaY = fromCursorY - toCursorY;
        
        setPositionTo(_sketchPositionX-cursorDeltaX, _sketchPositionY-cursorDeltaY);
        render();
    };

    let setPositionTo = (x, y)=>{
        x = parseInt(x);
        y = parseInt(y);

        let minLeftOffset = calcStartPosition(_canvas.clientWidth, _sketchWidth*getMinScale());
        let maxLeftOffset = _canvas.clientWidth - getCurrentWidth() - minLeftOffset;

        let minTopOffset = calcStartPosition(_canvas.clientHeight, _sketchHeight*getMinScale());
        let maxTopOffset = _canvas.clientHeight - getCurrentHeight() - minTopOffset;

        _sketchPositionX = Math.min(minLeftOffset, Math.max(maxLeftOffset, x));
        _sketchPositionY = Math.min(minTopOffset, Math.max(maxTopOffset, y));
    };
    
    

    let render = ()=>{
        context.clearRect(0, 0,_canvas.clientWidth, _canvas.clientHeight);

        renderCanvas();
        // renderLayers();
    };
    
    let renderCanvas = ()=>{
        buildGrid();
        renderBuffer(buffer);
    }
    let buildGrid = ()=>{
        for (let x = 0; x < _sketchWidth; x++) {
            for (let y = 0; y < _sketchHeight; y++) {
                const index = calcIndex(x, y, _sketchWidth)*SIZE_PIXEL;
            
                let checkeredColorHEX = (!(y&0x1) && x&0x1) || (!(x&0x1) && y&0x1) ? CHECKERED_LIGHT_COLOR_HEX : CHECKERED_DARK_COLOR_HEX;
                
                buffer[index + INDEX_RED] = checkeredColorHEX >> 8*INDEX_RED & 0xFF;
                buffer[index + INDEX_GREEN] = checkeredColorHEX >> 8*INDEX_GREEN & 0xFF;
                buffer[index + INDEX_BLUE] = checkeredColorHEX >> 8*INDEX_BLUE & 0xFF;
                buffer[index + INDEX_ALFA] = checkeredColorHEX >> 8*INDEX_ALFA & 0xFF;
            }
        }
    };
    

    let renderLayers = ()=>{
        layerList.forEach(layer=>{
            const layerBuffer = layer.getBuffer();
            render(layerBuffer);
        });
    }

    let renderBuffer = (originalBuffer)=>{
        let resizedWidth = getCurrentWidth();
        let resizedHeight = getCurrentHeight();
        let viewWidth = _canvas.clientWidth;
        let viewHeight = _canvas.clientHeight;
        
        
        if(_sketchPositionX > viewWidth || _sketchPositionX < -resizedWidth) return;
        if(_sketchPositionY > viewHeight || _sketchPositionY < -resizedHeight) return;

        let maxPositionX = viewWidth - resizedWidth;
        let maxPositionY = viewHeight - resizedHeight;
        
        let startVisibleX = (_sketchPositionX < 0) ? -_sketchPositionX : 0;
        let endVisibleX =  resizedWidth - ((_sketchPositionX < maxPositionX) ? 0 : (_sketchPositionX - maxPositionX));

        let startVisibleY = (_sketchPositionY < 0) ? -_sketchPositionY : 0;
        let endVisibleY = resizedHeight - ((_sketchPositionY < maxPositionY) ? 0 : (_sketchPositionY - maxPositionY));


        let visibleWidth = endVisibleX - startVisibleX;
        let visibleHeight = endVisibleY - startVisibleY;
        
        let tempBuffer = new Uint8ClampedArray(visibleWidth*visibleHeight*SIZE_PIXEL);
        
        for (let x = startVisibleX; x < endVisibleX; x++) {
            const dX = Math.floor(x / _scale);

            for (let y = startVisibleY; y < endVisibleY; y++) {
                const dY = Math.floor(y / _scale);
                
                const index = dY * _sketchWidth + dX;
                
                const red = originalBuffer[index* SIZE_PIXEL + INDEX_RED];
                const green = originalBuffer[index* SIZE_PIXEL + INDEX_GREEN];
                const blue = originalBuffer[index * SIZE_PIXEL + INDEX_BLUE];
                const alpha = originalBuffer[index * SIZE_PIXEL + INDEX_ALFA];
                
                const resizedIndex = (y-startVisibleY) * visibleWidth + (x-startVisibleX);
                tempBuffer[resizedIndex * SIZE_PIXEL + INDEX_RED] = red;
                tempBuffer[resizedIndex * SIZE_PIXEL + INDEX_GREEN] = green;
                tempBuffer[resizedIndex * SIZE_PIXEL + INDEX_BLUE] = blue;
                tempBuffer[resizedIndex * SIZE_PIXEL + INDEX_ALFA] = alpha;
            }
        }
        const data = new ImageData(tempBuffer, visibleWidth, visibleHeight);
        context.putImageData(data, _sketchPositionX-startVisibleX, _sketchPositionY-startVisibleY);
    }

    let addLayer = (layer)=>{
        if(!(layer instanceof Layer)) throw new Error("Objeto inválido. Insira uma Layer válida.");

        layerList.push(layer);
    };
    let removeLayer = (index)=>{
        if(parseInt(index) < 0) throw new Error("Parâmetro fora do escopo.\nDigite um index maior do que 0.");

        layerList.splice(index, 1);
    };

    function getScale(){ return _scale; }
    function getMinScale(){ return Math.max(1, Math.min(Math.floor(_canvas.clientHeight/_sketchHeight),  Math.floor(_canvas.clientWidth/_sketchWidth))); }
    function getMaxScale(){ return getMinScale() + 10; }
    function getCurrentHeight(){ return _sketchHeight * getScale(); }
    function getCurrentWidth(){ return _sketchWidth * getScale(); }

    function calcIndex(row, col, lengthRow){ return col * lengthRow + row; }
    function calcStartPosition(sizeOfParent, sizeOfChild){ return Math.floor((sizeOfParent - sizeOfChild) / 2); }
    
    function isValidCanvas() { return _canvas.nodeName === 'CANVAS'}
    function isValidSize() { return (parseInt(_sketchWidth) > 0 && parseInt(_sketchWidth) < 1200) || (parseInt(_sketchHeight) > 0 && parseInt(_sketchHeight) < 0) }

    const proto = Object.create(Layer.prototype);
    proto.render = render;
    proto.onHover = onHover;
    proto.zoomIn = zoomIn;
    proto.zoomOut = zoomOut;
    proto.panning = panning;
    proto.getScale = getScale;
    proto.getCurrentHeight = getCurrentWidth;
    proto.addLayer = addLayer;
    proto.removeLayer = removeLayer;

    return Object.seal(proto);
}