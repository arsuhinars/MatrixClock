#pragma once

#include <stdint.h>

#include <FastLED.h>

//Количество режимов часов
#define CLOCK_MODES_AMOUNT    2

extern uint8_t clockBackground;       //Номер текущего фона часов
extern bool clockEnableBackgrounds;   //Включить смену режима часов и фона
extern CRGB clockDigitsColor;         //Цвет цифр часов

/* Отправить текущему подключенному пользователю данные об текущем режиме часов */
void sendClockData();

/* Функция изменения текущего подрежима часов */
void changeClockMode(uint8_t mode);

/* Получить текущий режим часов */
uint8_t getClockMode();

/* Функция обновления и вывода часов на экран */
void updateClockMode();
