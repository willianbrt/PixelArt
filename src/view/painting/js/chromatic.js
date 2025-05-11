import { rotate, getDelta } from "./dot.js"

const canvas = document.getElementById("color-picker");
const ctx = canvas.getContext("2d");

const width = canvas.width;
const height = canvas.height;

const cx = width / 2;
const cy = height / 2;
const outerRadius = Math.min(height, width)/2;
const strokeWidth = 10;
const innerRadius = outerRadius - strokeWidth;

const _lado = Math.sqrt(3) * (innerRadius);
const _altura = Math.sqrt(3)*_lado/2;
const _rotateDeg = 90;

let triangle = buildTriangle(_lado, _altura, {x: cx,y:cy}, _rotateDeg);

let _hue = 0;
let hueMarker = document.querySelector(".marker#hue");
let colorMarker = document.querySelector(".marker#color");

colorMarker.style.top = `${triangle.vertexA.y-colorMarker.offsetHeight/2}px`;
colorMarker.style.left = `${triangle.vertexA.x-colorMarker.offsetWidth/2}px`;

drawColorWheel();
drawChromaticTriangle();
setHueDeg(_hue);

function setHueDeg(hue){
    setHueRad(hue * Math.PI / 180);
}
function setHueRad(hue){
    let centerRadius = innerRadius + strokeWidth/2;
    const markerX = Math.cos(hue) * centerRadius + cx;
    const markerY = Math.sin(hue) * centerRadius + cy;

    hueMarker.style.top = `${markerY - hueMarker.offsetHeight/2}px`;
    hueMarker.style.left = `${markerX - hueMarker.offsetWidth/2}px`;

    _hue = hue * 180 / Math.PI;

    triangle.setHueColor(_hue);
    requestAnimationFrame(()=>drawChromaticTriangle());
}

function buildTriangle(lado, altura, centerPosition, rotateDeg){
    let vertexA = {
        x: centerPosition.x - lado/2,
        y: centerPosition.y + (innerRadius)*Math.sin(30*Math.PI/180),
        color: [0, 0, 0]
    };
    
    let vertexB = { x: vertexA.x + lado, y: vertexA.y, color: [0, 0, 0] };
    let vertexC = { x: centerPosition.x, y: vertexA.y - altura, color: [255, 255, 255] };

    let centroide = {
        x: (vertexA.x + vertexB.x + vertexC.x) / 3,
        y: (vertexA.y + vertexB.y + vertexC.y) / 3,
    };

    rotate(vertexA, rotateDeg, centroide);
    rotate(vertexB, rotateDeg, centroide);
    rotate(vertexC, rotateDeg, centroide);

    let _area = calculateArea(vertexA, vertexB, vertexC);
    
    function setHueColor(hue){
        hue = ((hue % 360) + 360) % 360;
        const X = Math.round((1 - Math.abs((hue / 60) % 2 - 1)) * 255);

        if (hue < 60) {
            vertexA.color = [255, X, 0];
        } else if (hue < 120) {
            vertexA.color = [X, 255, 0];
        } else if (hue < 180) {
            vertexA.color = [0, 255, X];
        } else if (hue < 240) {
            vertexA.color = [0, X, 255];
        } else if (hue < 300) {
            vertexA.color = [X, 0, 255];
        } else {
            vertexA.color = [255, 0, X];
        }
    }

    function getColor(point){
        const [ alpha, beta, gamma] = getBarycentric(point);

        if (alpha < 0 || beta < 0 || gamma < 0) throw new Error("Point out of bounds");
        
        const r = alpha * vertexA.color[0] + beta * vertexB.color[0] + gamma * vertexC.color[0];
        const g = alpha * vertexA.color[1] + beta * vertexB.color[1] + gamma * vertexC.color[1];
        const b = alpha * vertexA.color[2] + beta * vertexB.color[2] + gamma * vertexC.color[2];

        return [r, g, b];
    }

    function getBarycentric(point){
        let areaABP = calculateArea(vertexA, vertexB, point);
        let areaBCP = calculateArea(vertexB, vertexC, point);
        let areaCAP = calculateArea(vertexC, vertexA, point);

        const alpha = areaABP / _area;
        const beta = areaBCP / _area;
        const gamma = areaCAP / _area;

        return [alpha, beta, gamma];
    }
    
    function calculateArea(vertexA, vertexB, point){
        const ab = {
            x: vertexB.x - vertexA.x,
            y: vertexB.y - vertexA.y
        };
        const ap = {
            x: point.x - vertexA.x,
            y: point.y - vertexA.y
        };
        return ab.x*ap.y - ab.y*ap.x;
    }

    function getBounding(){
        const min = {
                x:  Math.floor(Math.min(Math.min(vertexA.x, vertexB.x), vertexC.x)),
                y:  Math.floor(Math.min(Math.min(vertexA.y, vertexB.y), vertexC.y))
        };
        const max = {
                x:  Math.ceil(Math.max(Math.max(vertexA.x, vertexB.x), vertexC.x)),
                y:  Math.ceil(Math.max(Math.max(vertexA.y, vertexB.y), vertexC.y))
        };
        return {
            min,
            max,
            width: max.x - min.x,
            height: max.y - min.y
        }
    }

    function isInside(x, y){
        const [alpha, beta, gamma] = triangle.getBarycentric({x, y});
        return (alpha >= 0 && beta >= 0 && gamma >= 0)
    }
    
    return {
        vertexA,
        vertexB,
        vertexC,
        centroide, 
        AB: getDelta(vertexA, vertexB),
        BC: getDelta(vertexB, vertexC),
        CA: getDelta(vertexC, vertexA),
        area: _area,
        bounding: getBounding(),
        getColor,
        getBarycentric,
        setHueColor,
        calculateArea,
        isInside,
    }
}

function drawChromaticTriangle(){
    const imageData = ctx.getImageData(triangle.bounding.min.x, triangle.bounding.min.y, triangle.bounding.width, triangle.bounding.height);
    const data = imageData.data;

    for (let y = triangle.bounding.min.y; y < triangle.bounding.max.y; y++) {
        for (let x = triangle.bounding.min.x; x < triangle.bounding.max.x; x++) {
            try {
                const [r, g, b] = triangle.getColor({ x, y });
                
                const index = ((y-triangle.bounding.min.y) * (triangle.bounding.width) + (x-triangle.bounding.min.x)) * 4;
                data[index] = r;
                data[index + 1] = g;
                data[index + 2] = b;
                data[index + 3] = 255;
            } catch(e){}
        }
    }

    ctx.clearRect(triangle.bounding.min.x, triangle.bounding.min.y, triangle.bounding.width, triangle.bounding.height);
    ctx.putImageData(imageData, triangle.bounding.min.x, triangle.bounding.min.y);
}

canvas.addEventListener("mousedown", function(e){
    let x = e.clientX - canvas.getBoundingClientRect().left;
    let y = e.clientY - canvas.getBoundingClientRect().top;

    const distance = Math.sqrt((x - cx) ** 2 + (y - cy) ** 2);
    let tolerancia = 0;

    if(distance >= innerRadius - tolerancia && distance <= outerRadius + tolerancia)
    {
        cursorMoveOntoCircle();
    }
    else if(triangle.isInside(x,y))
    {
        cursorMoveOntoTriangle();
    } 
});

/* CIRCLE CHROMATIC */

hueMarker.addEventListener("mousedown", function(e){
    e.preventDefault();
    cursorMoveOntoCircle();
});

function cursorMoveOntoCircle(){
    let abort = new AbortController();
    const rect = canvas.getBoundingClientRect();

    window.addEventListener("mousemove", function(ev){
        ev.preventDefault();

        let cursorX = ev.clientX - rect.left;
        let cursorY = ev.clientY - rect.top;

        const dx = cursorX - cx;
        const dy = cursorY - cy;

        setHueRad(Math.atan2(dy, dx));

    }, { signal: abort.signal });

    window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
}

function drawColorWheel(){
    let gradient = ctx.createConicGradient(0, cx, cy);
    let endColor = 0;

    const INCREMENT_END_COLOR = 1/360;
    for (let angle = 0; angle < 360; angle += 1) {
        gradient.addColorStop(endColor, `hsl(${angle}, 100%, 50%)`);
        endColor += INCREMENT_END_COLOR;
    }

    ctx.beginPath();
    ctx.lineWidth = strokeWidth;
    ctx.strokeStyle = gradient;
    ctx.arc(cx, cy, outerRadius-strokeWidth/2, 0, 2 * Math.PI);
    ctx.stroke();
    ctx.closePath();
}

/* Triangle CHROMATIC */
colorMarker.addEventListener("mousedown", function(e){
    e.preventDefault();
    cursorMoveOntoTriangle();
});

function cursorMoveOntoTriangle(){
    let abort = new AbortController();
    const rect = canvas.getBoundingClientRect();

    window.addEventListener("mousemove", function(ev){
        ev.preventDefault();

        const x = ev.clientX - rect.left;
        const y = ev.clientY - rect.top;
        
        const clamped = clampToTriangle(x, y);

        colorMarker.style.left = `${clamped.x - colorMarker.offsetWidth / 2}px`;
        colorMarker.style.top = `${clamped.y - colorMarker.offsetHeight / 2}px`;
    }, { signal: abort.signal });

    window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
}

function clampToTriangle(x, y) {
    const p = { x, y };
    
    if(triangle.isInside(x, y)) return p;

    const ab = closestPointOnSegment(p, triangle.vertexA, triangle.vertexB);
    const bc = closestPointOnSegment(p, triangle.vertexB, triangle.vertexC);
    const ca = closestPointOnSegment(p, triangle.vertexC, triangle.vertexA);

    const dAB = distanceSquared(p, ab);
    const dBC = distanceSquared(p, bc);
    const dCA = distanceSquared(p, ca);

    if (dAB <= dBC && dAB <= dCA) return ab;
    if (dBC <= dCA) return bc;
    return ca;
}

function closestPointOnSegment(p, a, b) {
    const abx = b.x - a.x;
    const aby = b.y - a.y;
    const t = ((p.x - a.x) * abx + (p.y - a.y) * aby) / (abx * abx + aby * aby);

    const clampedT = Math.max(0, Math.min(1, t));

    return {
        x: a.x + clampedT * abx,
        y: a.y + clampedT * aby
    };
}

function distanceSquared(p1, p2) {
    const dx = p1.x - p2.x;
    const dy = p1.y - p2.y;
    return dx * dx + dy * dy;
}
