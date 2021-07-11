#pragma once

#include <Arduino.h>
#include <stdint.h>

//Фон для бегущей строки
extern uint8_t runningLineBg;

//Скорость бегущей строки
extern uint8_t runningLineSpeed;

//Текущий текст, выводимый в бегущей строке
extern String runningLineText;

/* Функция отрисовки символа в буфер для маски */
void drawChar(char symbol, int8_t x, int8_t y);

/* Функция отрисовки текста в точке (x, y) */
void drawText(String &text, int16_t x, uint8_t y);

/* Функция расчета длины текста на экране */
uint16_t calcTextLength(String &text);

/* Функция применения маски текста на текущем фоне */
void applyTextMask();

/* Функция обновления режима бегущей строки */
void updateRunningLine();
