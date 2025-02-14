import { IPressed } from "../interfaces/IHandlerEvents.js";
import PanningTool from "../commands/panningTool.js";
import { PositionHelper } from "../../../../scripts/common/position.js";

export default function PanningPressed(sketch){
    let flagCursor = { x: 0, y: 0 };
    // sketch.setCursor("grab");
    this.pressed = (event)=>{
        // sketch.setCursor("grabbing");

        flagCursor = PositionHelper.getPositionCursor(event);
    }

    this.tracking = (event)=>{
        let currentCursor = PositionHelper.getPositionCursor(event);
        
        sketch.panning(flagCursor.x, flagCursor.y, currentCursor.x, currentCursor.y);

        flagCursor = currentCursor;
    }

    this.released = (event)=>{
        // sketch.setCursor("grab");
    }
}
PanningPressed.prototype = IPressed.prototype;
PanningPressed.prototype.constructor = IPressed;