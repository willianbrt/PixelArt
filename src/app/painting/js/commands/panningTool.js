import ICommand from "../interfaces/ICommand.js";

export default function PanningTool(sketch, fromCursorPosition, toCursorPosition){
    let _fromCursorPosition = fromCursorPosition;
    let _toCursorPosition = toCursorPosition;

    this.execute = ()=>{
        sketch.panning(_fromCursorPosition.x, _fromCursorPosition.y, _toCursorPosition.x, _toCursorPosition.y);
    }
    this.undo = ()=>{
        sketch.panning(_toCursorPosition.x, _toCursorPosition.y, _fromCursorPosition.x, _fromCursorPosition.y);
    }
}
PanningTool.prototype = ICommand.prototype;
PanningTool.prototype.constructor = ICommand;