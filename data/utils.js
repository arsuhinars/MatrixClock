
/**
 * Функция преобразования цвета из HSV в RGB пространство
 */
function hsvToRgb(h, s, v)
{
    var r, g, b;

    var i = Math.floor(h * 6);
    var f = h * 6 - i;
    var p = v * (1 - s);
    var q = v * (1 - f * s);
    var t = v * (1 - (1 - f) * s);

    switch (i % 6)
    {
    case 0: r = v, g = t, b = p; break;
    case 1: r = q, g = v, b = p; break;
    case 2: r = p, g = v, b = t; break;
    case 3: r = p, g = q, b = v; break;
    case 4: r = t, g = p, b = v; break;
    case 5: r = v, g = p, b = q; break;
    }

    r = Math.floor(r*255);
    g = Math.floor(g*255);
    b = Math.floor(b*255);

    return [r, g, b];
}

/**
 * Функция преобразования цвета из RGB в HSV пространство
 */
function rgbToHsv(r, g, b)
{
    r = r / 255
    g = g / 255
    b = b / 255

    var max = Math.max(r, g, b)
    var min = Math.min(r, g, b)
    var h, v = max

    var d = max - min
    var s = (max == 0) ? 0 : d / max

    if(max == min){
        h = 0
    }
    else {
        switch(max){
            case r: h = (g - b) / d + ((g < b) ? 6 : 0); break
            case g: h = (b - r) / d + 2; break
            case b: h = (r - g) / d + 4; break
        }
        h = h / 6
    }

    h = Math.floor(h*360)
    s = Math.floor(s*100)
    v = Math.floor(v*100)

    return [h, s, v]
}

/**
 * Функция плавного появления элемента.
 * @param element - ссылка на DOM объект
 * @param time - длительность анимации
*/
function fadeOutElement(element, time) {
    return new Promise((resolve, reject) => {
        element.style.display = ''
        let startTime = performance.now() - parseFloat(element.style.opacity) * time
        requestAnimationFrame(function animate(currentTime) {
            let passedTime = currentTime - startTime
            if (passedTime < time) {
                element.style.opacity = passedTime / time
                requestAnimationFrame(animate)
            }
            else {
                element.style.opacity = 1.0
                resolve()
            }
        })
    })
}

/**
 * Функция плавного исчезания элемента.
 * @param element - ссылка на DOM объект
 * @param time - длительность анимации
*/
function fadeInElement(element, time) {
    return new Promise((resolve, reject) => {
        let startTime = performance.now() - (1.0 - parseFloat(element.style.opacity)) * time
        requestAnimationFrame(function animate(currentTime) {
            let passedTime = currentTime - startTime 
            if (passedTime < time) {
                element.style.opacity = 1.0 - (passedTime / time)
                requestAnimationFrame(animate)
            }
            else {
                element.style.display = 'none'
                element.style.opacity = 0.0
                resolve()
            }
        })
    })
}

/* Функция преобразования часа и минуты в Unix время */
function convertTimeToUnix(hour, minute) {
    let timestamp = new Date(2021, 1, 1, hour, minute).getTime() / 1000
    return timestamp - new Date().getTimezoneOffset() * 60
}
