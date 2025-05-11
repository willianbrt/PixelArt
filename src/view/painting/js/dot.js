// export function dot(x, y){
//     this.x = x;
//     this.y = y;
//     this.rotate = (deg, eixo = {x:0,y:0})=>{
//         let {x, y} = this;
//         x -= eixo.x;
//         y -= eixo.y;
        
//         let rad = deg * Math.PI/180;
//         point.x = x * Math.cos(rad) - y * Math.sin(rad) + eixo.x;
//         point.y = x * Math.sin(rad) + y * Math.cos(rad) + eixo.y;
//     }
// }

export function rotate(point, deg, eixo = {x:0,y:0}){
    let {x, y} = point;
    x -= eixo.x;
    y -= eixo.y;
    
    let rad = deg * Math.PI/180;
    point.x = x * Math.cos(rad) - y * Math.sin(rad) + eixo.x;
    point.y = x * Math.sin(rad) + y * Math.cos(rad) + eixo.y;
}
export function getDelta(pointA, pointB){
    return {
        x: pointB.x - pointA.x,
        x: pointB.y - pointA.y
    }
}