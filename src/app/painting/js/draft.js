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

    const context = _canvas.getContext("2d", { willReadFrequently: true, imageSmoothingEnabled: false });
    
    let _scale = getMinScale();
    let _sketchPositionX = calcStartPosition(_canvas.clientWidth, getCurrentWidth());
    let         _sketchPositionY = calcStartPosition(_canvas.clientHeight, getCurrentHeight());
    
    let imageData = new ImageData(getCurrentWidth(), getCurrentHeight());

    _canvas.addEventListener("mouseout", (event)=>{ leaveHover(); });
    _canvas.addEventListener("mousemove", (event)=>{ onHover(); });

    let flagRowHover, flagColumnHover;
    let onHover = (cursorX, cursorY)=>{
        let cursorOffsetX = cursorX - _sketchPositionX;
        let cursorOffsetY = cursorY - _sketchPositionY;
        
        let currentWidth = getCurrentWidth();
        let currentHeight = getCurrentHeight();


        leaveHover();

        if(cursorOffsetX < 0 || cursorOffsetX > currentWidth ||             cursorOffsetY < 0 || cursorOffsetY > currentHeight)
            return;


        let col = Math.floor(cursorOffsetY / _scale);
        let row = Math.floor(cursorOffsetX / _scale);
        
        const dX = row*_scale;
        const dY = col*_scale;
        
        for (let iX = 0; iX < _scale; iX++) {
            for (let iY = 0; iY < _scale; iY++) {
                const resizedIndex = calcIndex(iX + dX, iY + dY, currentWidth);
                
                let checkeredColorHEX = (!(row&0x1) && col&0x1) || (!(col&0x1) && row&0x1) ? CHECKERED_HOVER_LIGHT_COLOR_HEX : CHECKERED_HOVER_DARK_COLOR_HEX;
                
                imageData.data[resizedIndex * SIZE_PIXEL + INDEX_RED] = checkeredColorHEX >> 8*INDEX_RED & 0xFF;
                imageData.data[resizedIndex * SIZE_PIXEL + INDEX_GREEN] = checkeredColorHEX >> 8*INDEX_GREEN & 0xFF;
                imageData.data[resizedIndex * SIZE_PIXEL + INDEX_BLUE] = checkeredColorHEX >> 8*INDEX_BLUE & 0xFF;
                imageData.data[resizedIndex * SIZE_PIXEL + INDEX_ALFA] = checkeredColorHEX >> 8*INDEX_ALFA & 0xFF;
            }
        }

                flagRowHover = row;
        flagColumnHover = col;

        context.clearRect(0, 0, _canvas.clientWidth, _canvas.clientHeight);
        context.putImageData(imageData, Math.floor(_sketchPositionX), Math.floor(_sketchPositionY));
    }

    let leaveHover = ()=>{
        const flagDX = flagRowHover*_scale;
        const flagDY = flagColumnHover*_scale;
        
        for (let iX = 0; iX < _scale; iX++) {
            for (let iY = 0; iY < _scale; iY++) {
                const resizedIndex = calcIndex(iX + flagDX, iY + flagDY, getCurrentWidth());
            
                let checkeredColorHEX = (!(flagRowHover&0x1) && flagColumnHover&0x1) || (!(flagColumnHover&0x1) && flagRowHover&0x1) ? CHECKERED_LIGHT_COLOR_HEX : CHECKERED_DARK_COLOR_HEX;
                
                imageData.data[resizedIndex * SIZE_PIXEL + INDEX_RED] = checkeredColorHEX >> 8*INDEX_RED & 0xFF;
                imageData.data[resizedIndex * SIZE_PIXEL + INDEX_GREEN] = checkeredColorHEX >> 8*INDEX_GREEN & 0xFF;
                imageData.data[resizedIndex * SIZE_PIXEL + INDEX_BLUE] = checkeredColorHEX >> 8*INDEX_BLUE & 0xFF;
                imageData.data[resizedIndex * SIZE_PIXEL + INDEX_ALFA] = checkeredColorHEX >> 8*INDEX_ALFA & 0xFF;
            }
        }

        context.clearRect(0, 0, _canvas.clientWidth, _canvas.clientHeight);
        context.putImageData(imageData, Math.floor(_sketchPositionX), Math.floor(_sketchPositionY));
    }

    let zoomIn = (cursorX, cursorY)=>{
        var targetScale = _scale-1;
        zoom(targetScale, cursorX, cursorY);
    }
    let zoomOut = (cursorX, cursorY)=>{
        var targetScale = _scale+1;
        zoom(targetScale, cursorX, cursorY);
    }
    
    let zoom = (scale, cursorX, cursorY)=>{
        if(scale < getMinScale() || scale > getMaxScale()) return;

        let currentWidth = _sketchWidth * _scale;
        let currentHeight = _sketchHeight * _scale;
        
        if(scale > _scale){ 
            let lastPixelPositionX = _sketchPositionX + currentWidth;
            let lastPixelPositionY = _sketchPositionY + currentHeight;
            
            cursorX = Math.min(lastPixelPositionX, Math.max(_sketchPositionX, cursorX));
            cursorY = Math.min(lastPixelPositionY, Math.max(_sketchPositionY, cursorY));
            
            _sketchPositionX = cursorX - (cursorX - _sketchPositionX) * (scale / _scale);
            _sketchPositionY = cursorY - (cursorY - _sketchPositionY) * (scale / _scale);
        }
        else
        {
            let minScale = getMinScale();

            let minLeftOffset = calcStartPosition(_canvas.clientWidth, _sketchWidth*minScale);
            let maxLeftOffset = _canvas.clientWidth - currentWidth - minLeftOffset;
            
            let minTopOffset = calcStartPosition(_canvas.clientHeight, _sketchHeight*minScale);
            let maxTopOffset = _canvas.clientHeight - currentHeight - minTopOffset;

            let interpolatedX = _sketchPositionX - (_sketchWidth*scale - currentWidth);
            let interpolatedY = _sketchPositionY - (_sketchHeight*scale - currentHeight);
            
            _sketchPositionX = Math.min(minLeftOffset, Math.max(maxLeftOffset, interpolatedX));
            _sketchPositionY = Math.min(minTopOffset, Math.max(maxTopOffset, interpolatedY));
        }

        _scale = scale;

        renderFrame();
    }
    
    let panning = (fromCursorX, fromCursorY, toCursorX, toCursorY)=>{
        let currentWidth = _sketchWidth * _scale;
        let currentHeight = _sketchHeight * _scale;

        let minLeftOffset = calcStartPosition(_canvas.clientWidth, _sketchWidth*getMinScale()),
            maxLeftOffset = _canvas.clientWidth - currentWidth - minLeftOffset;

        let minTopOffset = calcStartPosition(_canvas.clientHeight, _sketchHeight*getMinScale()),
            maxTopOffset = _canvas.clientHeight - currentHeight - minTopOffset;

        let cursorDeltaX = fromCursorX - toCursorX;
        let cursorDeltaY = fromCursorY - toCursorY;
        
        _sketchPositionX = Math.min(minLeftOffset, Math.max(maxLeftOffset, _sketchPositionX-cursorDeltaX));
        _sketchPositionY = Math.min(minTopOffset, Math.max(maxTopOffset, _sketchPositionY-cursorDeltaY));
        
        context.clearRect(0, 0, _canvas.clientHeight, _canvas.clientHeight);
        context.putImageData(imageData, Math.floor(_sketchPositionX), Math.floor(_sketchPositionY));
    }
    
    let renderFrame = ()=>{
        let newWidth = getCurrentWidth();
        let newHeight = getCurrentHeight();
        
        let newImageData = new ImageData(newWidth, newHeight);
        
        for (let x = 0; x < _sketchWidth; x++) {
            for (let y = 0; y < _sketchHeight; y++) {
                                const dX = x*_scale;
                const dY = y*_scale;
                
                for (let iX = 0; iX < _scale; iX++) {
                    for (let iY = 0; iY < _scale; iY++) {
                        const resizedIndex = calcIndex(iX + dX, iY + dY, newWidth);
                    
                        let checkeredColorHEX = (!(y&0x1) && x&0x1) || (!(x&0x1) && y&0x1) ? CHECKERED_LIGHT_COLOR_HEX : CHECKERED_DARK_COLOR_HEX;
                        
                        newImageData.data[resizedIndex * SIZE_PIXEL + INDEX_RED] = checkeredColorHEX >> 8*INDEX_RED & 0xFF;
                        newImageData.data[resizedIndex * SIZE_PIXEL + INDEX_GREEN] = checkeredColorHEX >> 8*INDEX_GREEN & 0xFF;
                        newImageData.data[resizedIndex * SIZE_PIXEL + INDEX_BLUE] = checkeredColorHEX >> 8*INDEX_BLUE & 0xFF;
                        newImageData.data[resizedIndex * SIZE_PIXEL + INDEX_ALFA] = checkeredColorHEX >> 8*INDEX_ALFA & 0xFF;
                    }
                }
            }
        }
        imageData = newImageData;
        context.clearRect(0, 0, _canvas.clientWidth, _canvas.clientHeight);
        context.putImageData(imageData, Math.floor(_sketchPositionX), Math.floor(_sketchPositionY));
    }

    let draw = ()=> {
                requestAnimationFrame(renderFrame); 
    }

    
    let getScale = ()=> {
        return _scale;
    }
    
    function calcIndex(row, col, lengthRow){ return col * lengthRow + row; }
    function getMinScale(){ return Math.max(1, Math.min(Math.floor(_canvas.clientHeight/_sketchHeight),  Math.floor(_canvas.clientWidth/_sketchWidth))); }
    function getMaxScale(){ return getMinScale() + 10; }
function getCurrentHeight(){ return _sketchHeight * _scale; }
    function getCurrentWidth(){ return _sketchWidth * _scale; }
    
    function calcStartPosition(sizeOfParent, sizeOfChild){ return Math.floor((sizeOfParent - sizeOfChild) / 2); }
    
    function isValidCanvas() { return _canvas.nodeName === 'CANVAS'}
    function isValidSize() { return (parseInt(_sketchWidth) > 0 && parseInt(_sketchWidth) < 1200) || (parseInt(_sketchHeight) > 0 && parseInt(_sketchHeight) < 0) }
    
    return {
        draw,
        onHover,
        zoom,
        zoomIn,
        zoomOut,
        panning,
        getScale
    }
}