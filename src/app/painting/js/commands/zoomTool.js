import ICommand from "../interfaces/ICommand.js";

export default function ZoomTool(draft, cursorPosition, deltaY){
    let _cursorPosition = cursorPosition;
    let _deltaY = deltaY;
    let position = draft.getPosition();

    this.execute = ()=>{
        if(deltaY < 0){
            draft.zoomIn(_cursorPosition);
        } else {
            draft.zoomOut();
        }
    }

    this.undo = ()=>{
        if(_deltaY < 0){
            draft.zoomIn(_cursorPosition);
        } else {
            draft.zoomOut();
        }

        draft.moveTo(position);
    }
}
ZoomTool.prototype = ICommand.prototype;
ZoomTool.prototype.constructor = ICommand;