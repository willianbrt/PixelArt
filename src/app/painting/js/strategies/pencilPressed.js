import { IPressed } from "../interfaces/IHandlerEvents.js";

export default function PencilPressed(sketch, layer){
    this.pressed = (event)=>{
        layer.drawLine();
    }

    this.tracking = (event)=>{
        
    }

    this.released = (event)=>{
        
    }
}
PencilPressed.prototype = IPressed.prototype;
PencilPressed.prototype.constructor = IPressed;