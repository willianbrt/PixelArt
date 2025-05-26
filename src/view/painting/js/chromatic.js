s
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

let inpColorR = document.querySelector("input[name=r]");
let inpColorG = document.querySelector("input[name=g]");
let inpColorB = document.querySelector("input[name=b]");

let inpColorH = document.querySelector("input[name=h]");
let inpColorS = document.querySelector("input[name=s]");
let inpColorL = document.querySelector("input[name=l]");

inpColorR.addEventListener("input", updateRGB);
inpColorG.addEventListener("input", updateRGB);
inpColorB.addEventListener("input", updateRGB);

inpColorH.addEventListener("input", updateHSL);
inpColorS.addEventListener("input", updateHSL);
inpColorL.addEventListener("input", updateHSL);

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
    primaryColor = triangle.getColor(colorMarkerPosition);

    const [r,g,b] = primaryColor;

    updateInputColors(r, g, b);

    colorMarker.style.left = `${(colorMarkerPosition.x - colorMarker.offsetWidth / 2)}px`;
    colorMarker.style.top = `${(colorMarkerPosition.y - colorMarker.offsetHeight / 2)}px`;
}
inpColorHex.addEventListener("input", function(e){
    let color = hexToRgb(this.value);
    let p = triangle.setColor([color.r, color.g, color.b]);
    setColorByPosition(p.x, p.y);
});

function updateHSL(){
    if(inpColorH.value > 359)
        inpColorH.value = inpColorH.value.slice(1);

    if(inpColorS.value > 100)
        inpColorS.value = 100;

    if(inpColorL.value > 100)
        inpColorL.value = 100;

    if(inpColorH.value < 0)
        inpColorH.value = 0;

    if(inpColorS.value < 0)
        inpColorS.value = 0;

    if(inpColorL.value < 0)
        inpColorL.value = 0;

    let h = Number(inpColorH.value);
    let s = Number(inpColorS.value);
    let l = Number(inpColorL.value);
    let { r, g, b } = hslToRgb(h, s, l);
    try{
        setHueDeg(h);
        let p = triangle.setColor([r, g, b]);
        setColorByPosition(p.x, p.y)
    } catch(e){
        
    }
}
function updateRGB(){
    let r = inpColorR.value;
    let g = inpColorG.value;
    let b = inpColorB.value;
    try{
        let p = triangle.setColor([r, g, b]);
        setColorByPosition(p.x, p.y)
    } catch(e){
        
    }
}
function updateInputColors(r, g, b){
    const colorHSL = rgbToHsl(r, g, b);
    const colorHex = rgbToHex(r, g, b);

    inpColorHex.value = colorHex;
    inpColorR.value = r;
    inpColorG.value = g;
    inpColorB.value = b;
    inpColorH.value = colorHSL.h;
    inpColorS.value = colorHSL.s;
    inpColorL.value = colorHSL.l;

    let selectedColor = document.querySelector(".work-color.primary-color");
    selectedColor.style.background = colorHex;
}
function rgbToHex(r, g, b) {
    return "#" + [r, g, b]
        .map(x => {
            const hex = x.toString(16);
            return hex.length === 1 ? "0" + hex : hex;
        })
        .join("");
}
function rgbToHsl(r, g, b) {
    r /= 255;
    g /= 255;
    b /= 255;

    const max = Math.max(r, g, b);
    const min = Math.min(r, g, b);
    const delta = max - min;

    let h = 0, s = 0, l = (max + min) / 2;

    if (delta !== 0) {
        s = delta / (1 - Math.abs(2 * l - 1));

        switch (max) {
            case r:
                h = ((g - b) / delta) % 6;
                break;
            case g:
                h = (b - r) / delta + 2;
                break;
            case b:
                h = (r - g) / delta + 4;
                break;
        }

        h *= 60;
        if (h < 0) h += 360;
    }

    return {
        h: Math.round(h),
        s: +(s * 100).toFixed(1),
        l: +(l * 100).toFixed(1)
    };
}
function hexToRgb(hex) {
    hex = hex.replace(/^#/, "");

    // Atende formatos curtos como #abc
    if (hex.length === 3) {
        hex = hex.split("").map(c => c + c).join("");
    }

    const bigint = parseInt(hex, 16);
    const r = (bigint >> 16) & 255;
    const g = (bigint >> 8) & 255;
    const b = bigint & 255;

    return { r, g, b };
}
function hslToRgb(h, s, l) {
    s /= 100;
    l /= 100;

    const c = (1 - Math.abs(2 * l - 1)) * s;
    const x = c * (1 - Math.abs((h / 60) % 2 - 1));
    const m = l - c / 2;

    let r = 0, g = 0, b = 0;

    if (0 <= h && h < 60) {
        [r, g, b] = [c, x, 0];
    } else if (60 <= h && h < 120) {
        [r, g, b] = [x, c, 0];
    } else if (120 <= h && h < 180) {
        [r, g, b] = [0, c, x];
    } else if (180 <= h && h < 240) {
        [r, g, b] = [0, x, c];
    } else if (240 <= h && h < 300) {
        [r, g, b] = [x, 0, c];
    } else if (300 <= h && h < 360) {
        [r, g, b] = [c, 0, x];
    }

    r = Math.round((r + m) * 255);
    g = Math.round((g + m) * 255);
    b = Math.round((b + m) * 255);

    return { r, g, b };
}



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

    colorMarkerPosition = triangle.setHueColor(_hue);
    requestAnimationFrame(() => drawChromaticTriangle());

    console.log(colorMarkerPosition.x, colorMarkerPosition.y);
    try{
        setColorByPosition(colorMarkerPosition.x, colorMarkerPosition.y);
    }
    catch(e)
    {
        // setColorByPosition(triangle.vertexC.x,triangle.vertexC.y);
        // console.log(e)
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

