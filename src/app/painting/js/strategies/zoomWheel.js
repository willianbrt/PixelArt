import { IScroll } from "../interfaces/IHandlerEvents.js";
import { PositionHelper } from "../../../../scripts/common/position.js";

export default function ZoomWheel(sketch){
    this.scroll = (event)=>{
        if(event.deltaY < 0)
            sketch.zoomIn(PositionHelper.getPositionCursor(event));
        else
            sketch.zoomOut(PositionHelper.getPositionCursor(event));
    }
}
ZoomWheel.prototype = IScroll.prototype;
ZoomWheel.prototype.constructor = IScroll;