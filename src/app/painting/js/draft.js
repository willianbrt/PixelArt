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
    _canvas.addEventListener("mouseout", (event)=>{ leaveHover(); });
    
    let _scale = getMinScale();
    let imageData = new ImageData(getCurrentWidth(), getCurrentHeight());

    let _sketchPositionX = calcStartPosition(_canvas.clientWidth, getCurrentWidth());
    let _sketchPositionY = calcStartPosition(_canvas.clientHeight, getCurrentHeight());

    let flagRowHover, flagColumnHover;
    let onHover = (cursorX, cursorY)=>{
        let cursorOffsetX = cursorX - _sketchPositionX;
        let cursorOffsetY = cursorY - _sketchPositionY;

        let currentWidth = getCurrentWidth();
        let currentHeight = getCurrentHeight();

        leaveHover();

        if(cursorOffsetX < 0 || cursorOffsetX > currentWidth || cursorOffsetY < 0 || cursorOffsetY > currentHeight)
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
    };

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
        renderFrame();
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
        renderFrame();
    };
    
    
    let panning = (fromCursorX, fromCursorY, toCursorX, toCursorY)=>{
        let cursorDeltaX = fromCursorX - toCursorX;
        let cursorDeltaY = fromCursorY - toCursorY;
        
        setPositionTo(_sketchPositionX-cursorDeltaX, _sketchPositionY-cursorDeltaY);

        context.clearRect(0, 0, _canvas.clientHeight, _canvas.clientHeight);
        context.putImageData(imageData, Math.floor(_sketchPositionX), Math.floor(_sketchPositionY));
    };

    let setPositionTo = (x, y)=>{
        let minLeftOffset = calcStartPosition(_canvas.clientWidth, _sketchWidth*getMinScale());
        let maxLeftOffset = _canvas.clientWidth - getCurrentWidth() - minLeftOffset;

        let minTopOffset = calcStartPosition(_canvas.clientHeight, _sketchHeight*getMinScale());
        let maxTopOffset = _canvas.clientHeight - getCurrentHeight() - minTopOffset;

        _sketchPositionX = Math.min(minLeftOffset, Math.max(maxLeftOffset, x));
        _sketchPositionY = Math.min(minTopOffset, Math.max(maxTopOffset, y));
    };
    
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
    };

    let draw = ()=>{
        requestAnimationFrame(renderFrame);
    };

    function getScale(){ return _scale; }
    function getMinScale(){ return Math.max(1, Math.min(Math.floor(_canvas.clientHeight/_sketchHeight),  Math.floor(_canvas.clientWidth/_sketchWidth))); }
    function getMaxScale(){ return getMinScale() + 10; }
    function getCurrentHeight(){ console.log(getScale()); return _sketchHeight * getScale(); }
    function getCurrentWidth(){ return _sketchWidth * getScale(); }

    function calcIndex(row, col, lengthRow){ return col * lengthRow + row; }
    function calcStartPosition(sizeOfParent, sizeOfChild){ return Math.floor((sizeOfParent - sizeOfChild) / 2); }
    
    function isValidCanvas() { return _canvas.nodeName === 'CANVAS'}
    function isValidSize() { return (parseInt(_sketchWidth) > 0 && parseInt(_sketchWidth) < 1200) || (parseInt(_sketchHeight) > 0 && parseInt(_sketchHeight) < 0) }
    
    return {
        draw,
        onHover,
        zoomIn,
        zoomOut,
        panning,
        getScale,
        getCurrentHeight,
        getCurrentWidth,
    }
}