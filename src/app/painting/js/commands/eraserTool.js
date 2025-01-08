import ICommand from "../interfaces/ICommand.js";

export default function EraserTool(){
    this.execute = ()=>{}
    this.undo = ()=>{}
}
EraserTool.prototype = Object.create(ICommand.prototype);
EraserTool.prototype.constructor = ICommand;