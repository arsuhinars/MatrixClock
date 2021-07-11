
//Типы событий игры
const snakeEvents = {
    start: 0,
    end: 1,
    lose: 2,
    up: 3,
    right: 4,
    down: 5,
    left: 6,
    eat: 7
}

//Список сложностей игры
const snakeDifficulties = [
    {
        //Легкая сложность
        speed: 300,
        barrierEnabled: false
    }, {
        //Средняя сложность
        speed: 200,
        barrierEnabled: false
    }, {
        //Максимальная сложность
        speed: 200,
        barrierEnabled: true
    }
]

let snakeScore = 0                  //Текущий счет
let snakeRecord = 0                 //Рекорд

let snakeMainMenu       //Главное меню
let snakePlayMenu       //Меню игры
let snakeLoseMessage    //Меню проигрыша

let snakeScoreText      //Текст, показывающий количество набранных очков
let snakeLoseScoreText
let snakeRecordText     //Текст, показывающий рекорд игрока

/* Функция обновления значения текстов */
function snakeUpdateTexts() {
    snakeScoreText.innerHTML = 'Ваш счет: ' + snakeScore
    snakeLoseScoreText.innerHTML = 'Ваш счет: ' + snakeScore
    snakeRecordText.innerHTML = 'Рекорд: ' + snakeRecord
}

/* Функция обработки входящих событий */
function snakeHandleEvent(eventType) {
    switch (eventType) {
        case snakeEvents.start:
            snakeScore = 0
            snakeUpdateTexts()
            fadeInElement(snakeMainMenu, FADE_TIME)
            setTimeout(() =>
                fadeOutElement(snakePlayMenu, FADE_TIME), FADE_TIME)
            break
        case snakeEvents.end:
            snakeLoseMessage.style.display = 'none'
            fadeInElement(snakePlayMenu, FADE_TIME).then(() =>
                fadeOutElement(snakeMainMenu, FADE_TIME))
            break
        case snakeEvents.lose:
            if (snakeScore > snakeRecord) {
                snakeRecord = snakeScore
                localStorage.setItem('snakeRecord', snakeRecord)
            }
            snakeUpdateTexts()
            fadeInElement(snakePlayMenu, FADE_TIME).then(() => {
                snakeLoseMessage.style.display = ''
                fadeOutElement(snakeMainMenu, FADE_TIME)
            })
            break
        case snakeEvents.eat:
            snakeScore += 1
            snakeUpdateTexts()
            break
    }
}

//Событие полной загрузки страницы
document.addEventListener('DOMContentLoaded', () => {
    //Получаем все элементы страницы
    let startButton = document.querySelector('#snake-start-btn')
    let stick = document.querySelector('#snake-stick')
    let stickPoint = document.querySelector('#snake-stick .stick-point')
    let difficultySelect = document.querySelector('#snake-difficulty')
    let barrierWarning = document.querySelector('#snake-barrier-warning')

    snakeMainMenu = document.querySelector('#snake-main-menu')
    snakePlayMenu = document.querySelector('#snake-play-menu')
    snakeLoseMessage = document.querySelector('#snake-lose-msg')

    snakeScoreText = document.querySelector('#snake-score-text')
    snakeLoseScoreText = document.querySelector('#snake-lose-score-text')
    snakeRecordText = document.querySelector('#snake-record-text')

    //Скрываем сообщение о проигрыше
    snakeLoseMessage.style.display = 'none'
    
    //Добавляем обработчики событий
    difficultySelect.onchange = (e) => {
        localStorage.setItem('snakeDifficulty', e.target.selectedIndex)
        barrierWarning.style.display = snakeDifficulties[e.target.selectedIndex].barrierEnabled ? '' : 'none'
    }
    startButton.onclick = () => {
        let speed = snakeDifficulties[difficultySelect.selectedIndex].speed

        sendCommand(CMD_SET_VAR, varIndexes.snakeSpeed, speed >> 8, speed)
        sendCommand(CMD_SET_VAR, varIndexes.snakeBarrier, snakeDifficulties[difficultySelect.selectedIndex].barrierEnabled)
        sendCommand(CMD_SEND_EVENT, snakeEvents.start)
    }

    //Номер текущего указателя для стика
    let currentPointerId = -1

    //При нажатии на стик
    stick.addEventListener('pointerdown', (e) => {
        if (currentPointerId == -1) {
            stick.setPointerCapture(e.pointerId)
            currentPointerId = e.pointerId
        }
    })
    //При перемещении внутри стика
    stick.addEventListener('pointermove', (e) => {
        if (currentPointerId == e.pointerId) {
            //Находим радиус стика
            let stickRadius = stick.offsetWidth / 2
            //Находим смещение курсора относительно центра
            let x = e.pageX - stick.offsetLeft - stickRadius
            let y = e.pageY - stick.offsetTop - stickRadius
            
            //Находим расстояние от центра до курсора
            let distance = Math.sqrt(x * x + y * y) / stickRadius

            //Если ушли за пределы
            if (distance > 1.0) {
                x /= distance
                y /= distance
            }
            
            //Смещаем точку внутри стика
            stickPoint.style.left = x + 'px'
            stickPoint.style.top = y + 'px'
        
            //Находим угол
            let angle = Math.atan2(y, -x) * 180.0 / Math.PI + 180

            //Двигаем змейку в соотвествии с направлением
            if (angle > 315 || angle <= 45) {
                sendCommand(CMD_SEND_EVENT, snakeEvents.right)
            } else if (angle > 225) {
                sendCommand(CMD_SEND_EVENT, snakeEvents.down)
            } else if (angle > 135) {
                sendCommand(CMD_SEND_EVENT, snakeEvents.left)
            } else if (angle > 45) {
                sendCommand(CMD_SEND_EVENT, snakeEvents.up)
            }
        }
    })
    //При отпускании стика
    stick.addEventListener('pointerup', (e) => {
        if (currentPointerId == e.pointerId) {
            stick.releasePointerCapture(e.pointerId)
            currentPointerId = -1

            stickPoint.style.left = '0'
            stickPoint.style.top = '0'
        }
    })

    //Загружаем сохраненные значения
    snakeRecord = parseInt(localStorage.getItem('snakeRecord'))
    difficultySelect.selectedIndex = parseInt(localStorage.getItem('snakeDifficulty'))
    difficultySelect.dispatchEvent(new Event('change'))

    //Если рекорд не был загружен, то присваиваем стандартное значение
    if (isNaN(snakeRecord)) {
        snakeRecord = 0
    }
})