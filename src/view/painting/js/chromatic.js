import { rotate, getDelta } from "./dot.js"

const canvas = document.getElementById("color-picker");
const ctx = canvas.getContext("2d", { willReadFrequently: true });

const width = canvas.width;
const height = canvas.height;

const cx = width / 2;
const cy = height / 2;
const outerRadius = Math.min(height, width)/2;
const strokeWidth = 10;
const innerRadius = outerRadius - strokeWidth;

const padding = 5;
const _rotateDeg = 90;
let triangle = buildTriangle(innerRadius-padding, {x: cx,y:cy}, _rotateDeg);
let primaryColor = [0,0,0];
let secondaryColor = [255,255,255];
let colorMarkerPosition = triangle.vertexC.x;

let _hue = 0;
let hueMarker = document.querySelector(".marker#hue");
let colorMarker = document.querySelector(".marker#color");
let inpColorHex = document.querySelector("input[name=hex]");

drawColorWheel();
setHueDeg(_hue);

document.querySelector(".work-color.secondary-color")
.addEventListener("click", (e)=>{
    let elemPrimaryColor = e.target.parentNode.querySelector(".work-color.primary-color");
    let elemSecondaryColor = e.target;

    let temp = primaryColor;
    primaryColor = secondaryColor;
    secondaryColor = temp;

    elemPrimaryColor.style.background = `rgb(${ secondaryColor[0] }, ${secondaryColor[1]}, ${secondaryColor[2]})`;
    elemSecondaryColor.style.background = `rgb(${ primaryColor[0] }, ${primaryColor[1]}, ${primaryColor[2]})`;
});

function setColorByPosition(x, y){
    colorMarkerPosition = triangle.clampToTriangle(x, y);
    const [r,g,b] = triangle.getColor(colorMarkerPosition);
    primaryColor = [r,g,b];
    inpColorHex.value = `rgb(${r},${g},${b})`;

    let selectedColor = document.querySelector(".work-color.primary-color");
    selectedColor.style.background = `rgb(${r},${g},${b})`;

    colorMarker.style.left = `${(colorMarkerPosition.x - colorMarker.offsetWidth / 2)}px`;
    colorMarker.style.top = `${(colorMarkerPosition.y - colorMarker.offsetHeight / 2)}px`;
}
inpColorHex.addEventListener("input", function(e){
    let p = triangle.setColor([255,0,0])
    console.log(e)
});

function setHueByPosition(x, y){
    const dx = x - cx;
    const dy = y - cy;

    setHueRad(Math.atan2(dy, dx));
}
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
    requestAnimationFrame(() => drawChromaticTriangle());

    try{
        setColorByPosition(colorMarkerPosition.x, colorMarkerPosition.y);
    }
    catch(e)
    {
        setColorByPosition(triangle.vertexC.x,triangle.vertexC.y);
    }
}

function buildTriangle(radius, centerPosition, rotateDeg){
    const lado = Math.sqrt(3) * (radius);
    const altura = Math.sqrt(3)*lado/2;

    let vertexA = {
        x: centerPosition.x - lado/2,
        y: centerPosition.y + (radius)*Math.sin(30*Math.PI/180),
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

    let detABC = determinant(vertexA, vertexB, vertexC);
    
    function setColor([r,g,b]){
        const A = triangle.vertexB.color;
        const B = triangle.vertexC.color;
        const C = triangle.vertexA.color;
        
        const mat = [
            [A[0] - C[0], B[0] - C[0]],
            [A[1] - C[1], B[1] - C[1]],
            [A[2] - C[2], B[2] - C[2]]
        ];

        const target = [
            r - C[0],
            g - C[1],
            b - C[2]
        ];

        const { alpha, beta } = solveLeastSquares(mat, target);
        const gamma = 1 - alpha - beta;

        if (alpha < 0 || beta < 0 || gamma < 0) throw new Error("Out of Bounds");

        // Obter coordenada cartesiana do ponto
        const px = alpha * triangle.vertexA.x + beta * triangle.vertexB.x + gamma * triangle.vertexC.x;
        const py = alpha * triangle.vertexA.y + beta * triangle.vertexB.y + gamma * triangle.vertexC.y;

        // Atualiza marcador
        colorMarker.style.left = `${px - colorMarker.offsetWidth / 2}px`;
        colorMarker.style.top = `${py - colorMarker.offsetHeight / 2}px`;

        return { px, py };

    }
    function solveLeastSquares(mat, target) {
        const xtx = [
            [0, 0],
            [0, 0]
        ];
        const xty = [0, 0];

        for (let i = 0; i < 3; i++) {
            xtx[0][0] += mat[i][0] * mat[i][0];
            xtx[0][1] += mat[i][0] * mat[i][1];
            xtx[1][0] += mat[i][1] * mat[i][0];
            xtx[1][1] += mat[i][1] * mat[i][1];

            xty[0] += mat[i][0] * target[i];
            xty[1] += mat[i][1] * target[i];
        }

        const det = xtx[0][0] * xtx[1][1] - xtx[0][1] * xtx[1][0];
        if (Math.abs(det) < 1e-6) {
            throw new Error("Sistema singular");
        }

        const inv = [
            [xtx[1][1] / det, -xtx[0][1] / det],
            [-xtx[1][0] / det, xtx[0][0] / det]
        ];

        const alpha = inv[0][0] * xty[0] + inv[0][1] * xty[1];
        const beta = inv[1][0] * xty[0] + inv[1][1] * xty[1];

        return { alpha, beta };
    }
    
    function setHueColor(hue){
        hue = ((hue % 360) + 360) % 360;
        const X = Math.round((1 - Math.abs((hue / 60) % 2 - 1)) * 255);
        let color;
        if (hue < 60) {
            color = [255, X, 0];
        } else if (hue < 120) {
            color = [X, 255, 0];
        } else if (hue < 180) {
            color = [0, 255, X];
        } else if (hue < 240) {
            color = [0, X, 255];
        } else if (hue < 300) {
            color = [X, 0, 255];
        } else {
            color = [255, 0, X];
        }
        vertexA.color = color;
        return color;
    }

    function getColor(point){
        const [ alpha, beta, gamma] = getBarycentric(point);

        if (!isInside([ alpha, beta, gamma])) throw new Error("Point out of bounds");
        
        const r = alpha * vertexA.color[0] + beta * vertexB.color[0] + gamma * vertexC.color[0];
        const g = alpha * vertexA.color[1] + beta * vertexB.color[1] + gamma * vertexC.color[1];
        const b = alpha * vertexA.color[2] + beta * vertexB.color[2] + gamma * vertexC.color[2];
        
        const edge = Math.min(alpha, beta, gamma);
        const smoothingThreshold = 0.01;
        const a = 255 * smoothstep(0, smoothingThreshold, edge);

        return [Math.ceil(r), Math.ceil(g), Math.ceil(b), Math.ceil(a)];
    }

    function getBarycentric(point){
        let detABP = determinant(vertexA, vertexB, point);
        let detBCP = determinant(vertexB, vertexC, point);
        let detCAP = determinant(vertexC, vertexA, point);

        const alpha = detABP / detABC;
        const beta = detBCP / detABC;
        const gamma = detCAP / detABC ;

        return [alpha, beta, gamma];
    }
    
    function determinant(vertexA, vertexB, point){
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
            x:  Math.floor(Math.min(vertexA.x, vertexB.x, vertexC.x)),
            y:  Math.floor(Math.min(vertexA.y, vertexB.y, vertexC.y))
        };
        const max = {
            x:  Math.ceil(Math.max(vertexA.x, vertexB.x, vertexC.x)),
            y:  Math.ceil(Math.max(vertexA.y, vertexB.y, vertexC.y))
        };
        return {
            min,
            max,
            width: max.x - min.x,
            height: max.y - min.y
        }
    }

    function isInside([alpha, beta, gamma], epsilon = 1e-6){
        const sum = alpha + beta + gamma;
        return (
            alpha >= -epsilon &&
            beta >= -epsilon &&
            gamma >= -epsilon &&
            sum >= 1 - epsilon &&
            sum <= 1 + epsilon
        );
    }

    function clampToTriangle(x, y) {
        const p = { x, y };
        
        if(triangle.isInside(getBarycentric({x,y}))) return p;

        const ab = closestPointOnSegment(p, vertexA, vertexB);
        const bc = closestPointOnSegment(p, vertexB, vertexC);
        const ca = closestPointOnSegment(p, vertexC, vertexA);

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
    
    return {
        vertexA,
        vertexB,
        vertexC,
        centroide, 
        AB: getDelta(vertexA, vertexB),
        BC: getDelta(vertexB, vertexC),
        CA: getDelta(vertexC, vertexA),
        bounding: getBounding(),
        getColor,
        getBarycentric,
        setHueColor,
        calculateArea: determinant,
        isInside,
        clampToTriangle,
        closestPointOnSegment,
        distanceSquared,
        setColor
    }
}

function drawChromaticTriangle(){
    const imageData = ctx.getImageData(triangle.bounding.min.x, triangle.bounding.min.y, triangle.bounding.width, triangle.bounding.height);
    const data = imageData.data;

    for (let y = triangle.bounding.min.y; y < triangle.bounding.max.y; y++) {
        for (let x = triangle.bounding.min.x; x < triangle.bounding.max.x; x++) {
            try {
                const [r, g, b, a] = triangle.getColor({ x, y });

                const index = ((y-triangle.bounding.min.y) * (triangle.bounding.width) + (x-triangle.bounding.min.x)) * 4;
                data[index] = r;
                data[index + 1] = g;
                data[index + 2] = b;
                data[index + 3] = a;
            } catch(e){}
        }
    }
    ctx.clearRect(triangle.bounding.min.x, triangle.bounding.min.y, triangle.bounding.width, triangle.bounding.height);
    ctx.putImageData(imageData, triangle.bounding.min.x, triangle.bounding.min.y);
}
function smoothstep(edge0, edge1, x) {
    const t = Math.min(Math.max((x - edge0) / (edge1 - edge0), 0), 1);
    return t * t * (3 - 2 * t);
}

canvas.addEventListener("mousedown", function(e){
    e.preventDefault();
    let cursorX = e.clientX - canvas.getBoundingClientRect().left;
    let cursorY = e.clientY - canvas.getBoundingClientRect().top;

    const distance = Math.sqrt((cursorX - cx) ** 2 + (cursorY - cy) ** 2);
    let tolerancia = 0;

    if(distance >= innerRadius - tolerancia && distance <= outerRadius + tolerancia)
    {
        setHueByPosition(cursorX, cursorY);
        moveCursorWithinColorWheel();
    }
    else if(triangle.isInside(triangle.getBarycentric({x:cursorX, y:cursorY})))
    {
        setColorByPosition(cursorX, cursorY);
        moveCursorWithinChromaticTriangle();
    } 
});

hueMarker.addEventListener("mousedown", function(e){
    e.preventDefault();
    moveCursorWithinColorWheel();
});

function moveCursorWithinColorWheel(){
    let abort = new AbortController();
    const rect = canvas.getBoundingClientRect();

    window.addEventListener("mousemove", function(ev){
        ev.preventDefault();

        let cursorX = ev.clientX - rect.left;
        let cursorY = ev.clientY - rect.top;

        setHueByPosition(cursorX, cursorY);
    }, { signal: abort.signal });

    window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
    window.addEventListener("blur", ()=>{abort.abort()}, {once:true});
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

colorMarker.addEventListener("mousedown", function(e){
    e.preventDefault();
    moveCursorWithinChromaticTriangle();
});

function moveCursorWithinChromaticTriangle(){
    let abort = new AbortController();
    const rect = canvas.getBoundingClientRect();

    window.addEventListener("mousemove", function(ev){
        ev.preventDefault();

        const x = ev.clientX - rect.left;
        const y = ev.clientY - rect.top;
        
        setColorByPosition(x, y);
    }, { signal: abort.signal });

    window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
    window.addEventListener("blur", ()=>{abort.abort()}, {once:true}); 
}

