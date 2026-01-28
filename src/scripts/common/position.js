export let PositionHelper = Object.freeze({
    getDelta: (pointA, pointB)=>{
        return {
            x: pointB.x - pointA.x,
            y: pointB.y - pointA.y
        }
    },
    getRadBetweenTwoPoints: (pointA, pointB) => {
        let dx = pointB.x - pointA.x;
        let dy = pointB.y - pointA.y;
        return Math.atan2(dy, dx);
    },
    rotateByDeg: (point, deg, eixo = {x:0,y:0}) => {
        let rad = deg * Math.PI/180;
        return rotateByRad(point, rad, eixo);
    },
    rotateByRad: (point, rad, eixo = {x:0,y:0}) => {
        let {x, y} = point;
        x -= eixo.x;
        y -= eixo.y;

        return {
            x: x * Math.cos(rad) - y * Math.sin(rad) + eixo.x,
            y: x * Math.sin(rad) + y * Math.cos(rad) + eixo.y,
        }
    },
    getPositionCursor: (positionX, positionY,elem)=>{
        let elementPosition = elem.getBoundingClientRect();
        
        return Object.freeze({
            x: Math.floor(positionX - elementPosition.left),
            y: Math.floor(positionY - elementPosition.top) 
        });
    }
});