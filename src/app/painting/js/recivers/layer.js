const SIZE_PIXEL = 4;

export default function Layer({ 
    name: _name,
    width: _width,
    heigth: _height,
    sketch: _sketch,
    isVisible: _isVisible
}) {
    let buffer = new Uint8ClampedArray(_width*_height * 4);


    function fill(cursorPosition, colorHex){
        let modifiedPixels = [];
        
        const { startX, startY } = _sketch.tryGetPixelPosition(cursorPosition);
        const pixelStart = getPixel(startX, startY);

        if(pixelStart.colorHex == colorHex)
            return DrawingBuilder([]);

        let queue = [];
        queue.push({x: startX, y: startY});
        
        while(queue.length != 0) {
            let { x, y } = queue.shift();
            const currentPixel = getPixel(x, y);

            const isItOutsideTheAxisX = x < 0 || x > _width;
            const isItOutsideTheAxisY =  y < 0 || y > _height;

            if(isItOutsideTheAxisX || isItOutsideTheAxisY || currentPixel.colorHex != pixelStart.colorHex)
                continue;

            modifiedPixels.push(Pixel(x, y, colorHex));
            
            queue.push(Pixel(x+1, y));
            queue.push(Pixel(x-1, y));
            queue.push(Pixel(x, y+1));
            queue.push(Pixel(x, y-1));
        }


        return DrawingBuilder(modifiedPixels);
    }
    
    function drawRetangle(fromCursorPosition, toCursorPosition, colorHex){            
        const fromPixelPosition = _sketch.tryGetPixelPosition(fromCursorPosition);
        const toPixelPosition = _sketch.tryGetPixelPosition(toCursorPosition);

        let largura = (toPixelPosition.x > fromPixelPosition.x) ? toPixelPosition.x - fromPixelPosition.x : fromPixelPosition.x - toPixelPosition.x;
        let altura = (toPixelPosition.y > fromPixelPosition.y) ? toPixelPosition.y - fromPixelPosition.y : fromPixelPosition.y - toPixelPosition.y;

        return drawLine(fromPixelPosition.x, fromPixelPosition.y, colorHex)
                    .addPoint(altura, 270)
                    .addPoint(largura, 180)
                    .addPoint(altura, 270)
                    .build();
    }
    
    function drawLine(from, to, colorHex){
        let modifiedPixels = [];
        let lineStroke = 1;

        let resultDrawLine;
        if(to.x - from.x < to.y - from.y){
            resultDrawLine =drawHorizontalLine(from, to, colorHex).build();
        }
        else{
            resultDrawLine = drawVerticalLine(from, to, colorHex).build();
        }

        modifiedPixels.concat(resultDrawLine.getModifiedPixels());

        return { 
            addPoint: (comprimento, deg)=> {
                let nextPoint;
                nextPoint.x = Math.abs((comprimento + to.x) * Math.tan(deg));
                nextPoint.y = Math.abs((comprimento + to.y) * Math.tan(deg));
                
                let previousPoint = to;

                if(nextPoint.y < from.y){
                    previousPoint.y -= lineStroke;
                }
                else if(nextPoint.y > from.y)
                {
                    nextPoint.y += lineStroke;
                }

                if(nextPoint.x < from.x){
                    nextPoint.x -= lineStroke;
                }
                else if(nextPoint.x > from.x)
                {
                    nextPoint.y += lineStroke;
                }


                let resultDrawLine = drawLine(previousPoint, nextPoint, colorHex);
                const draw = resultDrawLine.build(); 

                modifiedPixels.concat(draw.getModifiedPixels());

                return resultDrawLine;
            },
            build: DrawingBuilder(modifiedPixels),
        };
    }

    function drawHorizontalLine(from, to, colorHex){
        let modifiedPixels = [];

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
                modifiedPixels.push(Pixel(x, y, colorHex));
                
                if (D >= 0){
                    y+=dir;
                    D -= 2*dx;
                } else
                    D += 2*dy;
            }
        }

        return DrawingBuilder(modifiedPixels);
    }

    function drawVerticalLine(from, to, colorHex){
        let modifiedPixels = [];
        
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
                modifiedPixels.push(Pixel(x, y, colorHex));
                
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

    function putPixel(x, y, colorHex){
        let modifiedPixels = [];
        const index = calcIndex(x, y, _width);

        buffer[index + INDEX_RED] = colorHex >> 24 & 0xFF;
        buffer[index + INDEX_GREEN] = colorHex >> 16 & 0xFF;
        buffer[index + INDEX_BLUE] = colorHex >> 8 & 0xFF;
        buffer[index + INDEX_ALFA] = colorHex >> 0 & 0xFF;

        return DrawingBuilder(modifiedPixels);
    }


    let getBuffer = ()=>{
        return buffer;
    }

    let renderPreview = ()=>{
        let bufferTemp = new Uint8ClampedArray(_width*_height * 4);
        bufferTemp.set(buffer.data, 0);
        // bufferView.set(originalBuffer.subarray(index, index+SIZE_PIXEL), resizedIndex);
        return bufferTemp;
    }

    function getPixel(x, y){
        const startIndex = calcIndex(x, y, _width);
        const endIndex = startIndex + SIZE_PIXEL;

        const colorHex = buffer.slice(startIndex, endIndex).reduce((a,c)=>(a << 8n) | BigInt(c), 0n);

        return {
            x,
            y,
            colorHex
        }
    }
    function putPixel(x, y, colorHex){
        const index = calcIndex(x, y, _width);

        buffer[index + INDEX_RED] = colorHex >> 24 & 0xFF;
        buffer[index + INDEX_GREEN] = colorHex >> 16 & 0xFF;
        buffer[index + INDEX_BLUE] = colorHex >> 8 & 0xFF;
        buffer[index + INDEX_ALFA] = colorHex >> 0 & 0xFF;
    }

    function render(){
        tempPixel.map(p=>{
            context.fillRect(cursorOffsetX, cursorOffsetY, _scale, _scale);
        });
    }

    function calcIndex(row, col, lengthRow){ return (col * lengthRow + row) * SIZE_PIXEL; }


    const proto = Object.create(Layer.prototype);
    proto.getBuffer = getBuffer;
    proto.startDrawing = pencil;
    proto.putPixel = putPixel;

    return Object.seal(proto);
}

function DrawingBuilder(modifiedPixels){
    let tempPixel = modifiedPixels;

    return {
        getModifiedPixels: ()=>{
            return tempPixel;
        },
        applyChanges: ()=>{
            tempPixel.map(p=>{
                p.oldColor = buffer.slice(p.index, p.index+SIZE_PIXEL)
                                    .reduce((a,c)=>(a << 8n) | BigInt(c), 0n);

                putPixel(p.x, p.y, p.colorHex);
            });
            
            tempPixel.splice(0);
            _sketch.render();
        }
    }
}

function Pixel(x, y, colorHex){
    return {
        x,
        y,
        colorHex,
    }
}
function Drawing(){

}