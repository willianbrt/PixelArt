import ICommand from "../interfaces/ICommand.js";

export default function LineTool(){
    this.execute = ()=>{}
    this.undo = ()=>{}
}
LineTool.prototype = Object.create(ICommand.prototype);
LineTool.prototype.constructor = ICommand;