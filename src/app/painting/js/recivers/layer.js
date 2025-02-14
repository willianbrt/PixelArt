import Graphics from '../../build/graphics/graphics.js'
const SIZE_PIXEL = 4;

export default async function Layer({ 
    name: _name,
    width: _width,
    heigth: _height,
    sketch: _sketch,
    isVisible: _isVisible
}) {
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

    const ptrBuffer = graphics.build(_width, _height);
    graphics.putPixel(0, 0, 0xFF0000FF)
    
    
    function fill(cursorPosition, newColorHex){
        let modifiedPixels = listPixelModifications();
        
        const { startX, startY } = _sketch.tryGetPixelPosition(cursorPosition);
        const startColorHEX = graphics.getPixel(startX, startY);

        if(startColorHEX == newColorHex)
            return DrawingBuilder([]);

        let queue = [];
        queue.push({x: startX, y: startY});
        
        while(queue.length != 0) {
            let { x, y } = queue.shift();
            const currentColorHEX = graphics.getPixel(x, y);

            const isItOutsideTheAxisX = x < 0 || x > _width;
            const isItOutsideTheAxisY =  y < 0 || y > _height;

            if(isItOutsideTheAxisX || isItOutsideTheAxisY || currentColorHEX != startColorHEX)
                continue;

            modifiedPixels.listPixelModifications(x, y, newColorHex);
            
            queue.push({x: x+1, y: y});
            queue.push({x: x-1, y: y});
            queue.push({x: x,   y: y+1});
            queue.push({x: x,   y: y-1});
        }


        return DrawingBuilder(modifiedPixels);
    }
    
    function drawRetangle(fromCursorPosition, toCursorPosition, newColorHex){            
        const fromPixelPosition = _sketch.tryGetPixelPosition(fromCursorPosition);
        const toPixelPosition = _sketch.tryGetPixelPosition(toCursorPosition);

        let largura = (toPixelPosition.x > fromPixelPosition.x) ? toPixelPosition.x - fromPixelPosition.x : fromPixelPosition.x - toPixelPosition.x;
        let altura = (toPixelPosition.y > fromPixelPosition.y) ? toPixelPosition.y - fromPixelPosition.y : fromPixelPosition.y - toPixelPosition.y;

        return drawLine(fromPixelPosition.x, fromPixelPosition.y, newColorHex)
                    .addPoint(altura, 270)
                    .addPoint(largura, 180)
                    .addPoint(altura, 270)
                    .build();
    }
    
    function drawLine(from, to, colorHex){
        let modifiedPixels = listPixelModifications();
        let lineStroke = 1;

        let resultDrawLine;
        if(to.x - from.x < to.y - from.y){
            resultDrawLine = drawHorizontalLine(from, to, colorHex).build();
        }
        else{
            resultDrawLine = drawVerticalLine(from, to, colorHex).build();
        }

        modifiedPixels.appendModification(resultDrawLine.getChanges());

        return { 
            addPoint: (comprimento, deg)=> {
                let nextPointX = Math.abs((comprimento + to.x) * Math.tan(deg));
                let nextPointY = Math.abs((comprimento + to.y) * Math.tan(deg));
                
                let previousPoint = to;

                if(nextPointY < from.y){
                    previousPoint.y -= lineStroke;
                }
                else if(nextPointY > from.y)
                {
                    nextPointY += lineStroke;
                }

                if(nextPointX < from.x){
                    previousPoint.x -= lineStroke;
                }
                else if(nextPointX > from.x)
                {
                    nextPointY += lineStroke;
                }


                let resultDrawLine = drawLine(previousPoint, {x:nextPointX, y:nextPointY }, colorHex);
                const draw = resultDrawLine.build(); 

                modifiedPixels.appendModification(draw.getChanges());

                return resultDrawLine;
            },
            build: DrawingBuilder(modifiedPixels),
        };
    }

    function drawHorizontalLine(from, to, newColorHex){
        let modifiedPixels = listPixelModifications();

        if(to.x > from.x){
            [to.x, from.x] = [from.x, to.x];
            [to.y, from.y] = [from.y, to.y];
        }
        
        let dx = to.x - from.x;
        let dy = to.y - from.y;

        let dir = (dy < 0) ? -1 : 0;
        dy *= dir;

        if(dx != 0){
            let D = 2*dy - dx;
            let y = from.y;
            
            for(let x = from.x; x = to.x; x++){
                modifiedPixels.addPixelModification(x, y, newColorHex);
                
                if (D >= 0){
                    y+=dir;
                    D -= 2*dx;
                } else
                    D += 2*dy;
            }
        }

        return DrawingBuilder(modifiedPixels);
    }

    function drawVerticalLine(from, to, newColorHex){
        let modifiedPixels = listPixelModifications();
        
        if(to.y > from.y){
            [to.x, from.x] = [from.x, to.x];
            [to.y, from.y] = [from.y, to.y];
        }
        
        let dx = to.x - from.x;
        let dy = to.y - from.y;

        let dir = (dx < 0) ? -1 : 0;
        dx *= dir;

        if(dy != 0){
            let D = 2*dx - dy;
            let x = from.x;
            
            for(let y = from.y; y = to.y; y++){
                modifiedPixels.addPixelModification(x, y, newColorHex);
                
                if (D > 0){
                    x+=dir;
                    D -= 2*dy;
                } else
                    D += 2*dx;
            }
        }

        return DrawingBuilder(modifiedPixels);
    }

    function pencil(fromCursorPosition, toCursorPosition, colorHex, branchSize){
        const fromPixelPosition = _sketch.tryGetPixelPosition(fromCursorPosition);
        const toPixelPosition = _sketch.tryGetPixelPosition(toCursorPosition);

        return drawLine(fromPixelPosition, toPixelPosition, colorHex).build();
    }

    function putPixel(x, y, newColorHex){
        let modifiedPixels = listPixelModifications();
        
        modifiedPixels.addPixelModification(x, y, newColorHex);

        return DrawingBuilder(modifiedPixels);
    }

    function getBuffer(){
        return new Uint8ClampedArray(graphics.memory.buffer, ptrBuffer, _width*_height*SIZE_PIXEL);
    }
    function getVisibleData(startVisibleX, endVisibleX,  startVisibleY, endVisibleY, scale){
        let visibleWidth = endVisibleX - startVisibleX;
        let visibleHeight = endVisibleY - startVisibleY;
        const tempPtr = graphics.render(ptrBuffer, startVisibleX, endVisibleX, startVisibleY, endVisibleY, scale);
        
        const bufferView = new Uint8ClampedArray(graphics.memory.buffer, tempPtr, visibleWidth*visibleHeight*SIZE_PIXEL)
        
        graphics.free(tempPtr);
        // return bufferView;

        return new ImageData(bufferView, visibleWidth, visibleHeight, {});
    }
    function listPixelModifications(){
        let list = [];

        return {
            appendModification: (newList)=>{
                list.concat(newList);
            },
            addPixelModification: (x, y, newColorHex)=>{
                list.push({
                    x,
                    y,
                    newColorHex,
                });
            },
            getListModifiedPixels:()=>{
                return list;
            }
        }
    }

    function DrawingBuilder(modifiedPixels){
        const tempPixel = modifiedPixels.getListModifiedPixels();
    
        return {
            getChanges: ()=>{
                return tempPixel;
            },
            applyChanges: ()=>{
                tempPixel.map(p=>{
                    p.oldColor = graphics.getPixel(p.x, p.y);

                    graphics.putPixel(p.x, p.y, p.newColorHex);
                });
                
                tempPixel.splice(0);
                _sketch.render();
            }
        }
    }

    let _opacity = 0.1;
    function getOpacity(){
        return _opacity ?? 1;
    }

    function setOpacity(opacity){
        return opacity;
    }

    return Object.seal({
        getVisibleData: getVisibleData,
        pencil: pencil,
        putPixel:putPixel,
        drawLine: drawLine,
        drawRetangle: drawRetangle,
        fill: fill,
        getOpacity: getOpacity,
        getBuffer: getBuffer,
    });
}