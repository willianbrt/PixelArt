export function Chromatic(options){
    const RAD_TO_DEG = 180 / Math.PI;
    const DEG_TO_RAD = Math.PI / 180;

    let _color = ColorFactory().buildByHSL(0, 100, 50);

    const canvas = document.getElementById("color-picker");
    const ctx = canvas.getContext("2d", { willReadFrequently: true });
    
    let hueMarker = document.querySelector(".marker#hue");
    let colorMarker = document.querySelector(".marker#color");

    const width = canvas.width;
    const height = canvas.height;

    const padding = 5;
    const cx = width / 2;
    const cy = height / 2;
    const outerRadius = Math.min(height, width)/2;
    const strokeWidth = parseInt(hueMarker.offsetWidth) || 0;
    const innerRadius = outerRadius - strokeWidth;
    const centerRadius = innerRadius + strokeWidth/2;
    
    let chromatic = buildChromatic(innerRadius-padding, {x: cx,y:cy}, 90);
    (()=>{
        setColor(_color);
        drawColorWheel();
        drawChromaticTriangle();
    })();

    hueMarker.addEventListener("mousedown", onUpdateHue);
    colorMarker.addEventListener("mousedown", onUpdateColor);
    
    canvas.addEventListener("mousedown", function(e){
        e.preventDefault();
        let cursorX = e.clientX - canvas.getBoundingClientRect().left;
        let cursorY = e.clientY - canvas.getBoundingClientRect().top;

        const distance = Math.sqrt((cursorX - cx) ** 2 + (cursorY - cy) ** 2);

        if(distance >= innerRadius && distance <= outerRadius)
        {
            onUpdateHue(e);
        }
        // else if(chromatic.isInside(chromatic.getBarycentric({x:cursorX, y:cursorY})))
        else if(chromatic.isInside({x:cursorX, y:cursorY}))
        {
            onUpdateColor(e);
        }
    });

    function onUpdateColor(e){
        e.preventDefault();
        let abort = new AbortController();
        const rect = canvas.getBoundingClientRect();

        let x = e.clientX - rect.left;
        let y = e.clientY - rect.top;
        
        setColorByPoint(x, y);
        
        window.addEventListener("mousemove", function(event){
            event.preventDefault();

            x = event.clientX - rect.left;
            y = event.clientY - rect.top;
            
            setColorByPoint(x, y);
        }, { signal: abort.signal });

        window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
        window.addEventListener("blur", ()=>{abort.abort()}, {once:true}); 
    }
    function onUpdateHue(e){
        e.preventDefault();
        
        let abort = new AbortController();
        const rect = canvas.getBoundingClientRect();
    
        let cursorX = e.clientX - rect.left;
        let cursorY = e.clientY - rect.top;
        setHueByPosition(cursorX, cursorY);

        window.addEventListener("mousemove", function(ev){
            ev.preventDefault();

            cursorX = ev.clientX - rect.left;
            cursorY = ev.clientY - rect.top;
            setHueByPosition(cursorX, cursorY);
            options?.onUpdateColor(_color);
        }, { signal: abort.signal });

        window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
        window.addEventListener("blur", ()=>{abort.abort()}, {once:true});
    }
    function setHueByPosition(x, y){
        const dx = x - cx;
        const dy = y - cy;
        const rad = Math.atan2(dy, dx);
        const degree = (rad * RAD_TO_DEG + 360) % 360;

        const markerX = Math.cos(rad) * centerRadius + cx;
        const markerY = Math.sin(rad) * centerRadius + cy;

        hueMarker.style.top = `${markerY - hueMarker.offsetHeight/2}px`;
        hueMarker.style.left = `${markerX - hueMarker.offsetWidth/2}px`;

        _color = ColorFactory().buildByHSL(degree, _color.hsl.s,_color.hsl.l);
        chromatic.setColor(degree);

        requestAnimationFrame(() => drawChromaticTriangle());
    }
    function drawChromaticTriangle(){
        const imageData = ctx.getImageData(chromatic.bounding.min.x, chromatic.bounding.min.y, chromatic.bounding.width, chromatic.bounding.height);
        const data = imageData.data;

        for (let y = chromatic.bounding.min.y; y < chromatic.bounding.max.y; y++) {
            for (let x = chromatic.bounding.min.x; x < chromatic.bounding.max.x; x++) {
                try {
                    const {r, g, b} = chromatic.getColor({ x, y }).rgb;

                    const index = ((y-chromatic.bounding.min.y) * (chromatic.bounding.width) + (x-chromatic.bounding.min.x)) * 4;
                    data[index] = r;
                    data[index + 1] = g;
                    data[index + 2] = b;
                    data[index + 3] = 255;
                } catch(e){
                    continue;
                }
            }
        }
        ctx.clearRect(chromatic.bounding.min.x, chromatic.bounding.min.y, chromatic.bounding.width, chromatic.bounding.height);
        ctx.putImageData(imageData, chromatic.bounding.min.x, chromatic.bounding.min.y);
    }
    
    function setColorByPoint(x, y){
        let colorMarkerPosition = chromatic.clampped(x, y);
        _color = chromatic.getColor(colorMarkerPosition);
        
        colorMarker.style.left = `${(colorMarkerPosition.x - colorMarker.offsetWidth/2)}px`;
        colorMarker.style.top = `${(colorMarkerPosition.y - colorMarker.offsetHeight/2)}px`;

        options?.onUpdateColor(_color);
    }
    function setColor(color){
        try{
            _color = color;
            const position = chromatic.getPositionColor(_color.hsl.s, _color.hsl.l);

            colorMarker.style.left = `${(position.x - colorMarker.offsetWidth / 2)}px`;
            colorMarker.style.top = `${(position.y - colorMarker.offsetHeight / 2)}px`;

            const rad = _color.hsl.h * DEG_TO_RAD;
            
            const markerX = Math.cos(rad) * centerRadius + cx;
            const markerY = Math.sin(rad) * centerRadius + cy;

            hueMarker.style.top = `${markerY - hueMarker.offsetHeight/2}px`;
            hueMarker.style.left = `${markerX - hueMarker.offsetWidth/2}px`;

            chromatic.setColor(_color.hsl.h);
            requestAnimationFrame(() => drawChromaticTriangle());
        } catch(e){
            console.warn(e);
        }

        options?.onUpdateColor(_color);
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

    function buildChromatic(radius, centerPosition){
        let colorFactory = ColorFactory();
        let targetColor = colorFactory.buildByHSL(0, 100, 50);

        function setColor(hue) {
            targetColor = colorFactory.buildByHSL(hue, 100, 50);
        }

        function getPositionColor(saturation, lightness) {
            const bounding = getBounding();
            
            const x = saturation/100*bounding.width + bounding.min.x;
            const y = (1-lightness/100)*bounding.height + bounding.min.y;

            return clampped(x, y);
        }

        function getColor(point){
            const bounding = getBounding();
            const u = (point.x - bounding.min.x) / (bounding.width); // 0 a 1 no eixo X
            const v = (point.y - bounding.min.y) / (bounding.height); // 0 a 1 no eixo Y

            const w0 = (1 - u) * (1 - v); // canto superior esquerdo
            const w1 = u * (1 - v);       // canto superior direito
            const w2 = u * v;             // canto inferior direito
            const w3 = (1 - u) * v;       // canto inferior esquerdo

            const h = Math.round(targetColor.hsl.h);
            const s = Math.round(100*w1 + 100*w2);
            const l = Math.round(100*w0 + 100*w1);

            return colorFactory.buildByHSL(h,s,l);
        }

        function isInside({x, y}){
            const bounding = getBounding();
            return x >= bounding.min.x && x <= bounding.max.x
                && y >= bounding.min.y && y <= bounding.max.y;
        }
    
        function clampped(x, y) {
            const bounding = getBounding();

            return{
                x: Math.max(bounding.min.x, Math.min(bounding.max.x, x)),
                y: Math.max(bounding.min.y, Math.min(bounding.max.y, y))
            }
        }

        function getBounding(){
            const offsetCenter = (radius * Math.sqrt(2)) / 2;
            const min = {
                x:  Math.floor(centerPosition.x - offsetCenter),
                y:  Math.floor(centerPosition.y - offsetCenter)
            };
            const max = {
                x:  Math.floor(centerPosition.x + offsetCenter),
                y:  Math.floor(centerPosition.y + offsetCenter)
            };
            return {
                min,
                max,
                width: max.x - min.x,
                height: max.y - min.y
            }
        }
        return {
            bounding: getBounding(),
            getColor,
            isInside,
            clampped,
            setColor,
            getPositionColor,
        }
    }

    return {
        setColor
    }
}

export function ColorFactory(){
    function buildByRGB(r,g,b){
        return {
            rgb: {r, g, b},
            hsl: rgbToHsl(r, g, b),
            hex: rgbToHex(r,g,b),
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b)
        }
    }
    function buildByHSL(h,s,l){
        const {r,g,b} = hslToRgb(h,s,l);
        
        return {
            rgb: {r,g,b},
            hsl: { h, s, l},
            hex: rgbToHex(r,g,b),
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b)
        };
    }
    function buildByHex(hex){
        const {r,g,b} = hexToRgb(hex);

        return {
            rgb: {r,g,b},
            hsl: rgbToHsl(r, g, b),
            hex: hex,
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b)
        }
    }
    
    function rgbToHex(r, g, b) {
        return  "#" + rgbToHex16(r, g, b).toString(16).padStart(6, "0");
    }
    function rgbToHex16(r, g, b) {
        return  (r << 16 | g << 8 | b);
    }
    function rgbToHex32(r, g, b) {
        return ((r << 24) | (g << 16) | (b << 8) | (0xFF)) >>> 0;
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
            s: +(s * 100).toFixed(0),
            l: +(l * 100).toFixed(0)
        };
    }
    function hexToRgb(hex) {
        hex = hex.replace(/^#/, "").slice(0, 6);
        if (!(/^[0-9A-Fa-f]{6}$/.test(hex)) && !(/^[0-9A-Fa-f]{3}$/.test(hex))) {
            throw new Error("Formato do hexadecimal inválido.");
        }
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

    return {
        buildByRGB,
        buildByHSL,
        buildByHex
    }
}