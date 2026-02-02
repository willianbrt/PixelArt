export function Chromatic(options){
    const RAD_360 = 2 * Math.PI;
    const RAD_TO_DEG = 180 / Math.PI;
    const DEG_TO_RAD = Math.PI / 180;

    let _color = ColorFactory().buildByHSL(0, 100, 50);

    const canvas = document.getElementById("color-picker");
    const ctx = canvas.getContext("2d", { willReadFrequently: true });
    
    let hueMarkerPosition = { x:0, y:0 };
    let colorMarkerPosition = { x:0, y:0 };

    const width = canvas.width;
    const height = canvas.height;

    const padding = 5;
    const cx = width / 2;
    const cy = height / 2;
    const offset = 10;
    const outerRadius = Math.min(height, width)/2 - offset;
    const strokeWidth = outerRadius*0.1;
    const innerRadius = outerRadius - strokeWidth;
    const centerRadius = innerRadius + strokeWidth/2;
    const markerWidth = strokeWidth*0.3;
    const markerRadius = strokeWidth-markerWidth*2;
    
    let chromatic = buildChromatic(innerRadius-padding, {x: cx,y:cy}, 90);
    (()=>{
        setColor(_color);
        drawChromaticTriangle();
    })();

    let touchID;
    canvas.addEventListener("mousedown", (e)=>{ 
        e.preventDefault();
        let rect = canvas.getBoundingClientRect();
        let cursorX = e.clientX - rect.left;
        let cursorY = e.clientY - rect.top;

        eventPressPicker(cursorX, cursorY);
    });
    canvas.addEventListener("touchstart", (e)=>{ 
        e.preventDefault();

        if(e.touches.length > 1)
            return;

        touchID = e.changedTouches[0].identifier;
        let rect = canvas.getBoundingClientRect();
        let cursorX = e.touches[0].clientX - rect.left;
        let cursorY = e.touches[0].clientY - rect.top;

        eventPressPicker(cursorX, cursorY);
    });
    function eventPressPicker(cursorX, cursorY){
        const distance = Math.sqrt((cursorX - cx) ** 2 + (cursorY - cy) ** 2);

        if(distance >= innerRadius && distance <= outerRadius+offset)
        {
            onUpdateHue(cursorX, cursorY);
        }
        else if(chromatic.isInside({x:cursorX, y:cursorY}))
        {
            onUpdateColor(cursorX, cursorY);
        }
    }
    function onUpdateColor(cursorX, cursorY){
        let abort = new AbortController();
        
        setColorByPoint(cursorX, cursorY);

        window.addEventListener("mousemove", (e)=>{
            let rect = canvas.getBoundingClientRect();
            let cursorX = e.clientX - rect.left;
            let cursorY = e.clientY - rect.top;

            setColorByPoint(cursorX, cursorY);
        }, { signal: abort.signal });

        window.addEventListener("touchmove", (e)=>{
            let rect = canvas.getBoundingClientRect();
            let cursorX = e.touches[0].clientX - rect.left;
            let cursorY = e.touches[0].clientY - rect.top;
            setColorByPoint(cursorX, cursorY);
        }, { signal: abort.signal });

        window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
        window.addEventListener("touchend", (e)=>{
            for(let i = 0; i < e.changedTouches.length; i++){
                if(e.changedTouches[i].identifier == touchID){
                    abort.abort()
                }
            }
        }, {once:true});
        window.addEventListener("blur", ()=>{abort.abort()}, {once:true});
    }
    function onUpdateHue(cursorX, cursorY){
        let abort = new AbortController();
        setHueByPosition(cursorX, cursorY);
        
        window.addEventListener("mousemove", (e)=>{
            let rect = canvas.getBoundingClientRect();
            let cursorX = e.clientX - rect.left;
            let cursorY = e.clientY - rect.top;

            setHueByPosition(cursorX, cursorY);
            options?.onUpdateColor(_color);
        }, { signal: abort.signal });

        window.addEventListener("touchmove", (e)=>{
            let rect = canvas.getBoundingClientRect();
            let cursorX = e.touches[0].clientX - rect.left;
            let cursorY = e.touches[0].clientY - rect.top;

            setHueByPosition(cursorX, cursorY);
            options?.onUpdateColor(_color);
        }, { signal: abort.signal });
        

        window.addEventListener("mouseup", ()=>abort.abort(), {once:true});
        window.addEventListener("touchend", (e)=>{
            for(let i = 0; i < e.changedTouches.length; i++){
                if(e.changedTouches[i].identifier == touchID){
                    abort.abort()
                }
            }
        }, {once:true});

        window.addEventListener("blur", ()=>{abort.abort()}, {once:true});
    }
    function setHueByPosition(x, y){
        const dx = x - cx;
        const dy = y - cy;
        const rad = Math.atan2(dy, dx);
        const degree = (rad * RAD_TO_DEG + 360) % 360;

        hueMarkerPosition.x = Math.cos(rad) * centerRadius + cx;
        hueMarkerPosition.y = Math.sin(rad) * centerRadius + cy;

        _color = ColorFactory().buildByHSL(degree, _color.hsl.s,_color.hsl.l);
        chromatic.setColor(degree);

        requestAnimationFrame(drawChromaticTriangle);
    }
    function drawChromaticTriangle(){
        const imageData = new ImageData(chromatic.bounding.width, chromatic.bounding.height);
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

        ctx.clearRect(0, 0, width, height);
        ctx.putImageData(imageData, chromatic.bounding.min.x, chromatic.bounding.min.y);
        
        drawColorWheel();


        ctx.beginPath();
        ctx.lineWidth = markerWidth;
        ctx.strokeStyle  = "white";
        ctx.arc(colorMarkerPosition.x, colorMarkerPosition.y, markerRadius, 0, RAD_360);
        ctx.stroke();
        ctx.closePath();

        ctx.beginPath();
        ctx.lineWidth = markerWidth;
        ctx.strokeStyle  = "white";
        ctx.arc(hueMarkerPosition.x, hueMarkerPosition.y, markerRadius, 0, RAD_360);
        ctx.stroke();
        ctx.closePath();
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
        ctx.arc(cx, cy, outerRadius-strokeWidth/2, 0, RAD_360);
        ctx.stroke();
        ctx.closePath();
    }
    function setColorByPoint(x, y){
        colorMarkerPosition = chromatic.clampped(x, y);
        _color = chromatic.getColor(colorMarkerPosition);
        
        requestAnimationFrame(drawChromaticTriangle);
        options?.onUpdateColor(_color);
    }
    function setColor(color){
        try{
            _color = color;
            colorMarkerPosition = chromatic.getPositionColor(_color.hsl.s, _color.hsl.l);;

            const rad = _color.hsl.h * DEG_TO_RAD;
            
            hueMarkerPosition.x = Math.cos(rad) * centerRadius + cx;
            hueMarkerPosition.y = Math.sin(rad) * centerRadius + cy;

            chromatic.setColor(_color.hsl.h);
            requestAnimationFrame(drawChromaticTriangle);
        } catch(e){
            console.warn(e);
        }

        options?.onUpdateColor(_color);
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
    function buildByDecimal(bigint){
        const r =  bigint & 0xFF;
        const g = (bigint >>> 8) & 0xFF;
        const b = (bigint >>> 16) & 0xFF;
        
        return {
            rgb: {r, g, b},
            hsl: rgbToHsl(r, g, b),
            hex: rgbToHex(r,g,b),
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b),
            getRGBLittleEndian: getRGBLittleEndian(r,g,b)
        }
    }
    function buildByRGB(r,g,b){
        return {
            rgb: {r, g, b},
            hsl: rgbToHsl(r, g, b),
            hex: rgbToHex(r,g,b),
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b),
            getRGBLittleEndian: getRGBLittleEndian(r,g,b)
        }
    }
    function buildByHSL(h,s,l){
        const {r,g,b} = hslToRgb(h,s,l);
        
        return {
            rgb: {r,g,b},
            hsl: { h, s, l},
            hex: rgbToHex(r,g,b),
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b),
            getRGBLittleEndian: getRGBLittleEndian(r,g,b)
        };
    }
    function buildByHex(hex){
        const {r,g,b} = hexToRgb(hex);

        return {
            rgb: {r,g,b},
            hsl: rgbToHsl(r, g, b),
            hex: hex,
            hex16: rgbToHex16(r, g, b),
            hex32: rgbToHex32(r, g, b),
            getRGBLittleEndian: getRGBLittleEndian(r,g,b)
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
        if(hex == 0){
            return {r: 0, g: 0, b: 0};
        }
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
    function getRGBLittleEndian(r,g,b){
        return ((0xFF << 24) | (b << 16) | (g << 8) | (r)) >>> 0;
    }
    return {
        buildByDecimal,
        buildByRGB,
        buildByHSL,
        buildByHex
    }
}