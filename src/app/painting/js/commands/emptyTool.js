import ICommand from "../interfaces/ICommand.js";

export default function EmptyTool(){
    this.execute = ()=>{}
    this.undo = ()=>{}
}
EmptyTool.prototype = Object.create(ICommand.prototype);
EmptyTool.prototype.constructor = ICommand;