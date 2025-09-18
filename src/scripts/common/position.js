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
        rotateByRad(point, rad, eixo);
    },
    rotateByRad: (point, rad, eixo = {x:0,y:0}) => {
        let {x, y} = point;
        x -= eixo.x;
        y -= eixo.y;

        point.x = x * Math.cos(rad) - y * Math.sin(rad) + eixo.x;
        point.y = x * Math.sin(rad) + y * Math.cos(rad) + eixo.y;
    },
    getPositionCursor: (event,elem)=>{
        let elementPosition = elem.getBoundingClientRect();
        // let elementPosition = event.srcElement.getBoundingClientRect();
        
        return Object.freeze({
            x: Math.floor(event.clientX - elementPosition.left),
            y: Math.floor(event.clientY - elementPosition.top) 
        });
    }
});