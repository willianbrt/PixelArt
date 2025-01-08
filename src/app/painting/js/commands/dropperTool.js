import ICommand from "../interfaces/ICommand.js";

export default function DropperTool(){
    this.execute = ()=>{}
    this.undo = ()=>{}
}
DropperTool.prototype = Object.create(ICommand.prototype);
DropperTool.prototype.constructor = ICommand;