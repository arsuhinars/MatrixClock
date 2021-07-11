#pragma once

#include <FastLED.h>

#include "config.h"

/* Функция изменения цвета пикселя матрицы в координатах x и y на color */
#define setPixel(x, y, color) pixels[getPixelNumber(x, y)] = color

/* Функция для ленивого получения цвета пикселя в (x, y) */
#define getPixelLazy(x, y) pixels[getPixelNumber(x, y)]

//Массив светодиодов матрицы
extern CRGB pixels[WIDTH * HEIGHT];

extern uint8_t matrixBrightness;    //Текущая яркость матрицы
extern uint8_t brightnessMul;      //Множитель яркости, деленный на 255

/* Получить номер пикселя в массиве */
uint8_t getPixelNumber(uint8_t x, uint8_t y);

/* Получить цвет пикселя в координатах x, y с проверкой на предел экрана */
CRGB getPixel(uint8_t x, uint8_t y);

/* Функция инитилизации экрана */
void initDisplay();

/* Функция обновления дисплея и вывода всей графики */
void updateDisplay();
