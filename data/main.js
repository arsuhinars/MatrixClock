
//Длительность анимации перехода
const FADE_TIME = 200.0

//Номера команд
const CMD_CHANGE_MODE = 0
const CMD_CHANGE_SUBMODE = 1
const CMD_GET_VAR = 2
const CMD_SET_VAR = 3
const CMD_SAVE_SETTINGS = 4
const CMD_SEND_EVENT = 5
const CMD_LOG = 6

//Индексы переменных
const varIndexes = {
    clockBgColor: 0,
    autoChangeBg: 1,
    clockEnableBg: 2,
    clockBg: 3,
    autoBrightness: 4,
    brightness: 5,
    wifiSSID: 6,
    wifiPass: 7,
    timeZone: 8,
    useNTP: 9,
    ntpAddr: 10,
    time: 11,
    snakeSpeed: 12,
    snakeBarrier: 13,
    runningLineText: 14,
    runningLineBg: 15,
    runningLineSpeed: 16
}

let socket               //Объект WebSocket соединения

let clockModeSelect      //Элемент выбора подрежима часов
let bgModeSelect         //Элемент выбора подрежима фона

let loadingScreen        //Экран загрузки
let disconnectedScreen   //Экран с сообщенем об потери связи

let modeSelect           //Меню выбора основного режима

//Элементы настроек режима часов
let clockColorInput      //Ползунок выбора цвета циферблата
let clockBgEnabled       //Переключатель включенности смены режима фона
let clockBg              //Меню выбора текущего фона часов 

//Элементы настроек режима бегущей строки
let runningLineText
let runningLineBg
let runningLineSpeed

//Элементы меню настроек
let bgChangeFlag         //Флаг автоматической смены фона 
let brightness           //Ползунок выбора яркости элемента
let autoBrightness       //Флаг авто-подстройки яркости
let wifiSSID             //Поле для ввода имени точки доступа
let wifiPass             //Поле для ввода её пароля
let timeZone             //Поле для выбора временной зоны
let useNTP               //Флаг синхронизации времени по интернету
let ntpAddr              //Поле для ввода адреса NTP сервера
let currentHour          //Поле для выбора текущего часа
let currentMinute        //Поле для выбора текущей минуты
let setCurrentTimeBtn    //Кнопка для изменения времени на настоящее
let saveButton           //Кнопка сохранения настроек

//Список, содержащий объекты меню основных режимов
let modesMenus = []

/**
 * Функция открытия подключения к устройству
 */
function connect() {
    //socket = new WebSocket('ws://' + window.location.hostname + ':81')
    socket = new WebSocket('ws://' + '192.168.1.68' + ':81')
    socket.bufferType = "arraybuffer";
    socket.onopen = () => {
        //Скрываем экран загрузки
        fadeInElement(loadingScreen, FADE_TIME)
        
        //Отправляем команды на получение данных об переменных
        for (key in varIndexes)
            sendCommand(CMD_GET_VAR, varIndexes[key])
    }
    //При закрытии соединения
    socket.onclose = (event) => {
        //Показываем сообщение об потери соединения
        fadeInElement(loadingScreen, FADE_TIME).then(() => {
            fadeOutElement(disconnectedScreen, FADE_TIME)
            disconnectedScreen.querySelector('h4').innerHTML = event.wasClean ?
                'Соединение было принудительно разорвано.' :
                'Соединение было прервано.'
        })
    }
    //При получении сообщения
    socket.onmessage = (event) => {
        if (event.data instanceof Blob) {
            event.data.arrayBuffer().then((data) => {
                handleCommand(new Uint8Array(data))
            })
        }
    }
}

/**
 * Функция отправки команды на устройство.
*/
function sendCommand() {
    let byteArray = new Uint8Array(arguments.length)
    for (let i = 0; i < byteArray.length; i++) {
        byteArray[i] = arguments[i]
    }
    socket.send(byteArray)
}

/**
 * Функция обработки полученной команды
 * @param {Uint8Array} data 
 */
function handleCommand(data) {
    switch (data[0]) {
        //Получаем текущее состояние какого либо параметра 
        case CMD_GET_VAR: handleGetVar(data); break;
        //При изменении основного режима
        case CMD_CHANGE_MODE:
            modeSelect.selectedIndex = data[1]
            //Прячем все остальные меню
            for (let i = 0; i < modesMenus.length; i++)
                if (i != data[1])
                    fadeInElement(modesMenus[i], FADE_TIME)
            //После того, как все меню исчезнут, то показываем текущее меню
            setTimeout(() => {
                fadeOutElement(modesMenus[data[1]], FADE_TIME)
            }, FADE_TIME)
            break;
        //При изменении подрежима
        case CMD_CHANGE_SUBMODE:
            switch (modeSelect.selectedIndex) {
                //Подрежима часов
                case 0:
                    clockModeSelect.selectedIndex = data[1]
                    break;
                //Подрежима фона
                case 1:
                    bgModeSelect.selectedIndex = data[1]
                    break;
            }
            break;
        //Пришло событие
        case CMD_SEND_EVENT:
            switch (modeSelect.selectedIndex) {
                //Для игры змейки
                case 4:
                    snakeHandleEvent(data[1])
                    break;
                case 5:
                    runnerHandleEvent(data[1])
                    break;
            }
            break;
        //Событие сохранения настроек
        case CMD_SAVE_SETTINGS:
            saveButton.disabled = true
            break;
        //Команда для вывода текста в лог
        case CMD_LOG:
            message = ''
            for (let i = 1; i < data.length; i++) {
                message += String.fromCharCode(data[i])
            }
            console.log(message)
            break
    }
}

/* Функция обработки ответа от команды получения переменной */
function handleGetVar(data) {
    switch (data[1]) {
        case varIndexes.clockBgColor:
            clockColorInput.value = rgbToHsv(data[2], data[3], data[4])[0] / 360.0 * 1000
            clockColorInput.dispatchEvent(new Event('input'))
            break
        case varIndexes.autoChangeBg:
            bgChangeFlag.checked = data[2]
            break
        case varIndexes.clockEnableBg:
            clockBgEnabled.checked = data[2]
            clockBgEnabled.dispatchEvent(new Event('change'))
            break
        case varIndexes.clockBg:
            clockBg.selectedIndex = data[2]
            break
        case varIndexes.autoBrightness:
            autoBrightness.checked = data[2]
            autoBrightness.dispatchEvent(new Event('change'))
            break
        case varIndexes.brightness:
            brightness.value = data[2]
            break
        case varIndexes.wifiSSID:
            wifiSSID.value = ''
            for (let i = 2; i < data.length; i++)
                wifiSSID.value += String.fromCharCode(data[i])
            break
        case varIndexes.wifiPass:
            wifiPass.value = ''
            for (let i = 2; i < data.length; i++)
                wifiPass.value += String.fromCharCode(data[i])
            break
        case varIndexes.timeZone:
            timeZone.selectedIndex = data[2]
            break
        case varIndexes.useNTP:
            useNTP.checked = data[2]
            useNTP.dispatchEvent(new Event('change'))
            break
        case varIndexes.ntpAddr:
            ntpAddr.value = ''
            for (let i = 2; i < data.length; i++)
                ntpAddr.value += String.fromCharCode(data[i])
            break
        case varIndexes.time:
            let timestamp = data[2] << 24 | data[3] << 16 | data[4] << 8 | data[5]
            let date = new Date((timestamp + new Date().getTimezoneOffset() * 60) * 1000)
            currentHour.selectedIndex = date.getHours()
            currentMinute.selectedIndex = date.getMinutes()
            break
        case varIndexes.snakeSpeed:
            break
        case varIndexes.snakeBarrier:
            break
        case varIndexes.runningLineText:
            runningLineText.value = ''
            for (let i = 2; i < data.length; i++)
            runningLineText.value += String.fromCharCode(data[i])
            break
        case varIndexes.runningLineBg:
            runningLineBg.selectedIndex = data[2]
            break
        case varIndexes.runningLineSpeed:
            runningLineSpeed.value = data[2]
            break
    }
}

//Событие полной загрузки страницы
document.addEventListener('DOMContentLoaded', () => {
    //Добавляем меню основных режимов
    modesMenus = [
        document.querySelector('#clock-container'),
        document.querySelector('#background-container'),
        document.querySelector('#running-line-container'),
        document.querySelector('#paint-container'),
        document.querySelector('#snake-container'),
        document.querySelector('#runner-container')
    ]

    //Получаем все элементы
    loadingScreen = document.querySelector('#loading-screen')
    disconnectedScreen = document.querySelector('#disconnected-screen')

    //При нажатии на кнопку переподключения
    document.querySelector('#reconnect-button').onclick = () => {
        connect()
        fadeInElement(disconnectedScreen, FADE_TIME).then(() => 
            fadeOutElement(loadingScreen, FADE_TIME))
    }

    //При изменении основного режима
    modeSelect = document.querySelector('#mode-select')
    modeSelect.onchange = (e) => {
        //Отправляем команду на его изменение
        sendCommand(CMD_CHANGE_MODE, e.target.selectedIndex)
        saveButton.disabled = false
    }

    //При изменении режима часов
    clockModeSelect = document.querySelector('#clock-mode-select')
    clockModeSelect.onchange = (e) => {
        //Отправляем на устройство
        sendCommand(CMD_CHANGE_SUBMODE, e.target.selectedIndex)
        saveButton.disabled = false
    }

    //При изменении режима фона
    bgModeSelect = document.querySelector('#background-mode-select')
    bgModeSelect.onchange = (e) => {
        //Отправляем на устройство
        sendCommand(CMD_CHANGE_SUBMODE, e.target.selectedIndex)
        saveButton.disabled = false
    }

    //Кнопка открытия экрана настроек
    let settingsScreen = document.querySelector('#settings-screen')
    document.querySelector('#settings-button').onclick = () =>
        fadeOutElement(settingsScreen, FADE_TIME)
    //Кнопка возврата на основной экран
    document.querySelector('#back-button').onclick = () =>
        fadeInElement(settingsScreen, FADE_TIME)
    
    clockColorInput = document.querySelector('input[name=clock-fill-color]')
    clockColorInput.oninput = (e) => {   //При изменении цвета, отправляем его
        let clr = hsvToRgb(parseFloat(e.target.value) / 1000, 1, 1)
        e.target.style.setProperty('--thumb-color', `rgb(${clr[0]},${clr[1]},${clr[2]})`)
        sendCommand(CMD_SET_VAR, varIndexes.clockBgColor, clr[0], clr[1], clr[2])
        saveButton.disabled = false
    }

    clockBgEnabled = document.querySelector('input[name=clock-bg-enabled]')
    //Событие изменения режима смены часов и фона
    clockBgEnabled.onchange = (e) => {
        sendCommand(CMD_SET_VAR, varIndexes.clockEnableBg, e.target.checked)
        clockBg.disabled = e.target.checked
        saveButton.disabled = false
    }

    clockBg = document.querySelector('#clock-bg')
    //Событие изменения текущего фона часов
    clockBg.onchange = (e) => {
        sendCommand(CMD_SET_VAR, varIndexes.clockBg, e.target.selectedIndex)
        saveButton.disabled = false
    }

    //При изменении автоматической смены фонов
    bgChangeFlag = document.querySelector('input[name=bg-change-flag]')
    bgChangeFlag.onchange = (e) => {
        sendCommand(CMD_SET_VAR, varIndexes.autoChangeBg, e.target.checked)
        saveButton.disabled = false
    }

    //При изменении текста бегущей строки
    runningLineText = document.querySelector('input[name=running-line-text]')
    runningLineText.oninput = (e) => {
        //Формируем массив
        let byteArray = new Uint8Array(e.target.value.length + 2)
        byteArray[0] = CMD_SET_VAR
        byteArray[1] = varIndexes.runningLineText
        for (let i = 2; i < byteArray.length; i++)
            byteArray[i] = e.target.value.charCodeAt(i - 2)
        //Отправляем
        socket.send(byteArray)

        saveButton.disabled = false
    }

    //При изменении скорости бегущей строки
    runningLineSpeed = document.querySelector('input[name=running-line-speed]')
    runningLineSpeed.oninput = (e) => {
        sendCommand(CMD_SET_VAR, varIndexes.runningLineSpeed, parseInt(e.target.value))
        saveButton.disabled = false
    }

    //При изменении фона бегущей строки
    runningLineBg = document.querySelector('#running-line-bg')
    runningLineBg.onchange = (e) => {
        sendCommand(CMD_SET_VAR, varIndexes.runningLineBg, e.target.selectedIndex)
        saveButton.disabled = false
    }

    //При изменении значения ползунка яркости
    brightness = document.querySelector('input[name=brightness]')
    brightness.oninput = (e) => {
        sendCommand(CMD_SET_VAR, varIndexes.brightness, parseInt(e.target.value))
        saveButton.disabled = false
    }

    //При изменении автоматической подстройки яркости
    autoBrightness = document.querySelector('input[name=auto-brightness]')
    autoBrightness.onchange = (e) => {
        sendCommand(CMD_SET_VAR, varIndexes.autoBrightness, e.target.checked)
        brightness.disabled = e.target.checked
        saveButton.disabled = false
    }

    //При изменении имени точки доступа
    wifiSSID = document.querySelector('input[name=wifi-ssid]')
    wifiSSID.oninput = (e) => {
        //Формируем массив
        let byteArray = new Uint8Array(e.target.value.length + 2)
        byteArray[0] = CMD_SET_VAR
        byteArray[1] = varIndexes.wifiSSID
        for (let i = 2; i < byteArray.length; i++)
            byteArray[i] = e.target.value.charCodeAt(i - 2)
        //Отправляем
        socket.send(byteArray)

        saveButton.disabled = false
    }

    //При изменении пароля точки доступа
    wifiPass = document.querySelector('input[name=wifi-pass]')
    wifiPass.oninput = (e) => {
        //Формируем массив
        let byteArray = new Uint8Array(e.target.value.length + 2)
        byteArray[0] = CMD_SET_VAR
        byteArray[1] = varIndexes.wifiPass
        for (let i = 2; i < byteArray.length; i++)
            byteArray[i] = e.target.value.charCodeAt(i - 2)
        //Отправляем
        socket.send(byteArray)

        saveButton.disabled = false
    }

    timeZone = document.querySelector('#time-zone')
    //Добавляем все временные зоны
    for (let i = 0; i < 24; i++) {
        let option = document.createElement('option')
        option.innerHTML = 'UTC+' + i
        timeZone.append(option)
    }
    //Ловим событие изменения
    timeZone.onchange = (e) => {
        sendCommand(CMD_SET_VAR, varIndexes.timeZone, e.target.selectedIndex)
        saveButton.disabled = false
    }

    //При изменении флага синхронизации по интернету
    useNTP = document.querySelector('input[name=use-ntp]')
    useNTP.onchange = (e) => {
        sendCommand(CMD_SET_VAR, varIndexes.useNTP, e.target.checked)
        ntpAddr.disabled = !e.target.checked
        currentHour.disabled = e.target.checked
        currentMinute.disabled = e.target.checked
        setCurrentTimeBtn.disabled = e.target.checked
        saveButton.disabled = false
    }

    //При изменении адреса NTP сервера
    ntpAddr = document.querySelector('input[name=ntp-addr]')
    ntpAddr.oninput = (e) => {
        //Формируем массив
        let byteArray = new Uint8Array(e.target.value.length + 2)
        byteArray[0] = CMD_SET_VAR
        byteArray[1] = varIndexes.ntpAddr
        for (let i = 2; i < byteArray.length; i++)
            byteArray[i] = e.target.value.charCodeAt(i - 2)
        //Отправляем
        socket.send(byteArray)

        saveButton.disabled = false
    }

    currentHour = document.querySelector('#current-hour')
    //Добавляем в список все возможные значения часа
    for (let i = 0; i < 24; i++) {
        let option = document.createElement('option')
        option.innerHTML = i
        currentHour.append(option)
    }
    //Ловим событие изменения
    currentHour.onchange = (e) => {
        let time = convertTimeToUnix(currentHour.selectedIndex, currentMinute.selectedIndex)
        sendCommand(CMD_SET_VAR, varIndexes.time, time >> 24, time >> 16, time >> 8, time)

        saveButton.disabled = false
    }

    currentMinute = document.querySelector('#current-minute')
    //Добавляем в список все возможные значения часа
    for (let i = 0; i < 60; i++) {
        let option = document.createElement('option')
        option.innerHTML = i
        currentMinute.append(option)
    }
    //Ловим событие изменения
    currentMinute.onchange = (e) => {
        let time = convertTimeToUnix(currentHour.selectedIndex, currentMinute.selectedIndex)
        sendCommand(CMD_SET_VAR, varIndexes.time, time >> 24, time >> 16, time >> 8, time)

        saveButton.disabled = false
    }
    
    //Кнопка для изменения времени на настоящее
    setCurrentTimeBtn = document.querySelector('#set-current-time-btn')
    setCurrentTimeBtn.onclick = () => {
        let date = new Date()
        let timestamp = convertTimeToUnix(date.getHours(), date.getMinutes())
        sendCommand(CMD_SET_VAR, varIndexes.time, timestamp >> 24, timestamp >> 16, timestamp >> 8, timestamp)

        currentHour.selectedIndex = date.getHours()
        currentMinute.selectedIndex = date.getMinutes()

        saveButton.disabled = false
    }

    //Кнопка сохранения настроек
    saveButton = document.querySelector('#save-button')
    //При нажатии на кнопку сохранения настроек
    saveButton.onclick = () => sendCommand(CMD_SAVE_SETTINGS)
    //При потере фокуса кнопки
    // saveButton.onblur = (e) => {
    //     e.target.setAttribute('data-tooltip', '')
    // }

    //Подключаемся к устройству
    connect()
})