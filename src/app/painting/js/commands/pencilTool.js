import { ICommandPressed } from "../interfaces/ICommand.js";

export default function PencilTool(sketch, layer, arrayPosition, colorHex){
    // if(!(layer instanceof Layer)) throw new Error("Objeto inválido. Insira uma Layer válida.");

    let modifiedPixels = [];
    let draw;

    this.execute = ()=>{

        for(position in arrayPosition){
            draw = pencil(position.from, position.to, colorHex);
        }
        
        modifiedPixels = draw.applyChanges();

        sketch.render();
    }

    this.undo = ()=>{
        for(pixel in modifiedPixels){
            layer.putPixel(pixel.x, pixel.y, pixel.oldColor);
        }

        sketch.render();
    }
}
PencilTool.prototype = ICommand.prototype;
PencilTool.prototype.constructor = ICommand;