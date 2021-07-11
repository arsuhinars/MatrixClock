
//Типы событий игры
const runnerEvents = {
    start: 0,
    end: 1,
    dead: 2,
    jump: 3,
    got_score: 4,
}

let runnerScore = 0         //Текущий счет
let runnerRecord = 0          //Рекорд

let runnerMainMenu          //Главное меню
let runnerPlayMenu          //Меню игры
let runnerLoseMessage       //Элемент сообщения о проигрыше

let runnerScoreText         //Текст, отображающий количество очков игрока
let runnerLoseScoreText     //Количество очков на экране проигрыша
let runnerRecordText        //Элемент рекорда игрока

/* Функция обновления текстов */
function runnerUpdateTexts() {
    runnerScoreText.innerHTML = 'Ваш счет: ' + runnerScore
    runnerRecordText.innerHTML = 'Рекорд: ' + runnerRecord
    runnerLoseScoreText.innerHTML = runnerScoreText.innerHTML
}

/* Функция обработки входящих событий */
function runnerHandleEvent(eventType) {
    switch (eventType) {
        case runnerEvents.start:
            runnerScore = 0
            fadeInElement(runnerMainMenu, FADE_TIME).then(() => {
                fadeOutElement(runnerPlayMenu, FADE_TIME)
            })
            break
        case runnerEvents.end:
            fadeInElement(runnerPlayMenu, FADE_TIME).then(() => {
                runnerLoseMessage.style.display = 'none'
                runnerUpdateTexts()
                fadeOutElement(runnerMainMenu, FADE_TIME)
            })
            break
        case runnerEvents.dead:
            if (runnerScore > runnerRecord) {
                runnerRecord = runnerScore
                localStorage.setItem('runnerRecord', runnerRecord)
            }
            fadeInElement(runnerPlayMenu, FADE_TIME).then(() => {
                runnerLoseMessage.style.display = ''
                runnerUpdateTexts()
                fadeOutElement(runnerMainMenu, FADE_TIME)
            })
            break
        case runnerEvents.got_score:
            runnerScore += 100
            runnerUpdateTexts()
            break
    }
}

//Событие полной загрузки страницы
document.addEventListener('DOMContentLoaded', () => {
    //Получаем все элементы
    runnerMainMenu = document.querySelector('#runner-main-menu')
    runnerPlayMenu = document.querySelector('#runner-play-menu')
    runnerLoseMessage = document.querySelector('#runner-lose-msg')

    runnerScoreText = document.querySelector('#runner-score-text')
    runnerLoseScoreText = document.querySelector('#runner-lose-score-text')
    runnerRecordText = document.querySelector('#runner-record-text')

    let startButton = document.querySelector('#runner-start-btn')
    let jumpButton = document.querySelector('#runner-jump-btn')

    //Скрываем сообщение о проигрыше
    runnerLoseMessage.style.display = 'none'

    //Добавляем слушатель кнопке запуска игры
    startButton.onclick = () => {
        sendCommand(CMD_SEND_EVENT, runnerEvents.start)
    }
    //Добавляем слушатель кнопке прыжка
    jumpButton.onclick = () => {
        sendCommand(CMD_SEND_EVENT, runnerEvents.jump)
    }

    //Загружаем значение рекорда из памяти
    runnerRecord = parseInt(localStorage.getItem('runnerRecord'))
    if (isNaN(runnerRecord)) {
        runnerRecord = 0
    }
})