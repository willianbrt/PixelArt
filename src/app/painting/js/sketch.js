const SIZE_PIXEL  = 4, 
      INDEX_RED   = 0, 
      INDEX_GREEN = 1, 
      INDEX_BLUE  = 2, 
      INDEX_ALFA  = 3;

const CHECKERED_LIGHT_COLOR_HEX = 0xFFFFFFFF,
      CHECKERED_DARK_COLOR_HEX = 0xDDDDDDFF,
      CHECKERED_HOVER_LIGHT_COLOR_HEX = 0xDDDDDDFF,
      CHECKERED_HOVER_DARK_COLOR_HEX = 0xDDDDDDFF;

export class Sketch {
    constructor(ctx, x, y, width, height, scalePX = 5){
        if(ctx === undefined) throw new Error("Contexto não definido.");
        if(scalePX < 1) throw new Error("O Valor de scaleX deve ser maior do que 1.");

        this.ctx = ctx;
        this.width = width;
        this.height = height;
        this.x = x;
        this.y = y;
        this.scalePX = scalePX;
        this.imageData = new ImageData(this.width, this.height);
        this.isMoviment = false;
    }
    
    draw(){
        const tempBuffer = this.imageData.data;
        
        for (let x = 0; x < this.width; x++) {
            for (let y = 0; y < this.height; y++) {
                let checkeredColorHEX = (!(y&0x1) && x&0x1) || (!(x&0x1) && y&0x1) ? CHECKERED_LIGHT_COLOR_HEX : CHECKERED_DARK_COLOR_HEX;
                
                const index = this.calcIndex(x, y, this.width); 
                tempBuffer[index * SIZE_PIXEL + INDEX_RED] = checkeredColorHEX >> 8*INDEX_RED & 0xFF;
                tempBuffer[index * SIZE_PIXEL + INDEX_GREEN] = checkeredColorHEX >> 8*INDEX_GREEN & 0xFF;
                tempBuffer[index * SIZE_PIXEL + INDEX_BLUE] = checkeredColorHEX >> 8*INDEX_BLUE & 0xFF;
                tempBuffer[index * SIZE_PIXEL + INDEX_ALFA] = checkeredColorHEX >> 8*INDEX_ALFA & 0xFF;
            }
        }
        
        this.ctx.clearRect(this.x, this.y-1, this.width, this.height+1);
        this.ctx.putImageData(this.imageData, this.x, this.y);
        this.ctx.save();
    }

    move(startPointX, startPointY, x, y){
        this.ctx.clearRect(this.x, this.y-1, this.width, this.height+1);
        console.log(x - startPointX, y - startPointY)
        
        let minX = 0, maxX = 0;
        let minY = 0, maxY = 0;
        
        if(this.width*this.scalePX > this.ctx.canvas.width)
            minX = this.ctx.canvas.width - (this.width*this.scalePX)
        
        if(this.height*this.scalePX > this.ctx.canvas.height)
            minY = this.ctx.canvas.height - (this.height*this.scalePX)
        

        this.x = Math.max(minX, Math.min(this.ctx.canvas.width - (this.width*this.scalePX), this.x - (x - startPointX)));
        this.y = Math.max(minY, Math.min(this.ctx.canvas.height - (this.height*this.scalePX), this.y - (y - startPointY)));



        this.zoom(this.scalePX, this.x, this.y);
        this.ctx.save();
    }

    zoom(scalePX, cursorX, cursorY){
        let newImageData = new ImageData(this.width*scalePX, this.height*scalePX);
        
        for (let x = 0; x < this.width; x++) {
            for (let y = 0; y < this.height; y++) {
                const index = this.calcIndex(x, y, this.width); 
                
                const dX = x*scalePX;
                const dY = y*scalePX;
                
                for (let iX = dX; iX < (dX+scalePX); iX++) {
                    for (let iY = dY; iY < (dY+scalePX); iY++) {
                        const newIndex = this.calcIndex(iX, iY, newImageData.width);
                        
                        newImageData.data[newIndex*SIZE_PIXEL + INDEX_RED] = this.imageData.data[index*SIZE_PIXEL + INDEX_RED] - 255;
                        newImageData.data[newIndex*SIZE_PIXEL + INDEX_GREEN] = this.imageData.data[index*SIZE_PIXEL + INDEX_GREEN];
                        newImageData.data[newIndex*SIZE_PIXEL + INDEX_BLUE] = this.imageData.data[index*SIZE_PIXEL + INDEX_BLUE] + 255;
                        newImageData.data[newIndex*SIZE_PIXEL + INDEX_ALFA] = this.imageData.data[index*SIZE_PIXEL + INDEX_ALFA];
                    }
                }
            }
        }


        // this.x -=(cursorX - this.x)*(scalePX-1);
        // this.y -=(cursorY - this.y)*(scalePX-1);

        this.x -=((cursorX - this.x) * (scalePX -  this.scalePX)) / this.scalePX ;
        this.y -=((cursorY - this.y) * (scalePX -  this.scalePX)) / this.scalePX ;


        this.ctx.clearRect(this.x, this.y-1, this.width*this.scalePX, (this.height+1)*this.scalePX);
        this.scalePX = scalePX;
        this.ctx.putImageData(newImageData, Math.floor(this.x), Math.floor(this.y));
        
    }
    paint(cursorX, cursorY){
        let newImageData = this.imageData;
        const index = cursorY * this.width  + cursorX;
        newImageData.data[index*SIZE_PIXEL + INDEX_RED] = 255;
        newImageData.data[index*SIZE_PIXEL + INDEX_GREEN] = 0;
        newImageData.data[index*SIZE_PIXEL + INDEX_BLUE] = 0;
        newImageData.data[index*SIZE_PIXEL + INDEX_ALFA] = 255;
        this.ctx.reset();
        this.ctx.putImageData(newImageData, 0, 0);
    }
    calcIndex = (row, col, lengthRow)=>{
        return row * lengthRow + col;
    }
}