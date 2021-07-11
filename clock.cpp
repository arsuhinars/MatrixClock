
#include "main.h"
#include "web_server.h"
#include "time.h"
#include "display.h"
#include "effects.h"
#include "fader.h"
#include "config.h"
#include "clock.h"

#define DIGIT_CHANGE_STEP   100     //Время шага анимации смены цифр (время в миллисекундах
#define CHANGE_MODE_DELAY   15000   //Время смены режима фона и часов (в миллисекундах)
#define CLOCK_FADE_TIME     1500    //Время перехода между режимами

uint8_t clockMode = 0;                  //Текущий режим часов
uint8_t clockBackground = 0;            //Номер текущего фона часов
bool clockEnableBackgrounds = true;     //Включить смену режима часов и фона
CRGB clockDigitsColor = CRGB::Red;      //Цвет заливки

unsigned long lastChangeClockMode = 0;  //Время последней смены режимов
uint8_t clockModeChangingPart = 0;      //Этап анимации смены режимов. 0 - закончили, 1 - угасание, 2 - нарастание
bool isBackgroundShowing = false;       //Показывается ли в данный момент фон вместо часов

//Шрифт цифр 3x5 для часов
static const uint8_t font_3x5[][3] PROGMEM = {
  { 0b11111, 0b10001, 0b11111 }, //0
  { 0b01001, 0b11111, 0b00001 }, //1
  { 0b10111, 0b10101, 0b11101 }, //2
  { 0b10101, 0b10101, 0b11111 }, //3
  { 0b11100, 0b00100, 0b11111 }, //4
  { 0b11101, 0b10101, 0b10111 }, //5
  { 0b11111, 0b10101, 0b10111 }, //6
  { 0b10000, 0b10000, 0b11111 }, //7
  { 0b11111, 0b10101, 0b11111 }, //8
  { 0b11101, 0b10101, 0b11111 }  //9
};

//Буфер, хранящий какие пиксели нужно сохранить
static bool savePixels[WIDTH * HEIGHT];

/* Отправить текущему подключенному пользователю данные об текущем режиме часов */
void sendClockData() {
  if (getCurrentMode() == MODE_CLOCK) {
    uint8_t data[] = {CMD_CHANGE_SUBMODE, clockMode};
    sendMessage(data, sizeof(data));
  }
}

/* Функция изменения текущего подрежима часов */
void changeClockMode(uint8_t mode) {
  clockMode = mode;
  lastChangeClockMode = millis();
  clockModeChangingPart = 0;
  isBackgroundShowing = false;
  FastLED.clear();

  sendClockData();
}

/* Получить текущий режим часов */
uint8_t getClockMode() {
  return clockMode;
}

/* Нарисовать число в булевом буфере экрана */
void drawDigit(byte digit, int8_t X, int8_t Y) {
  if (digit > 9) return;
  for (int8_t x = 0; x < 3; x++) {
    byte thisByte = pgm_read_byte(&(font_3x5[digit][x]));
    for (int8_t y = 0; y < 5; y++)
      if (X + x >= 0 && X + x < WIDTH && Y + y >= 0 && Y + y < HEIGHT)
          savePixels[(X + x) + (Y + y) * WIDTH] = (thisByte << y) & 0b10000;
  }
}

/* Функция отрисовки цифр */
void drawDigits() {
  //Обнуляем массив пикселей
  for (uint8_t i = 0; i < WIDTH * HEIGHT; i++)
      savePixels[i] = false;

  //Получаем текущий час и минуту
  byte hour = getHour(getTime());
  byte minute = getMinute(getTime());

  //Текущая цифра
  static byte digit1 = 0, digit2 = 0, digit3 = 0, digit4 = 0;
  static byte animStep = 0;   //Шаг анимации (от 0 до 5)

  //Ищем следующие цифры
  byte nextDigit1 = hour / 10, nextDigit2 = hour % 10;
  byte nextDigit3 = minute / 10, nextDigit4 = minute % 10;

  //Нужно ли проигрывать анимацию
  bool playAnim = false;

  //Первая цифра
  if (digit1 == nextDigit1) {
    drawDigit(digit1, 0, 1);
  }
  else {
    playAnim = true;
    drawDigit(digit1, 0, 1 + animStep);
    drawDigit(nextDigit1, 0, -5 + animStep);
    for (uint8_t x = 0; x < 3; x++) {
      savePixels[x] = false;  //Очищаем область сверху
      for (uint8_t y = 6; y < 8; y++)
        savePixels[x + y * WIDTH] = false;  //Очищаем область снизу 
    }
  }

  //Вторая цифра
  if (digit2 == nextDigit2) {
    drawDigit(digit2, 4, 1);
  }
  else {
    playAnim = true;
    drawDigit(digit2, 4, 1 + animStep);
    drawDigit(nextDigit2, 4, -5 + animStep);
    for (uint8_t x = 4; x < 7; x++) {
      savePixels[x] = false;  //Очищаем область сверху
      for (uint8_t y = 6; y < 8; y++)
        savePixels[x + y * WIDTH] = false;  //Очищаем область снизу
    }
  }

  //Третья цифра
  if (digit3 == nextDigit3) {
    drawDigit(digit3, 9, 1);
  }
  else {
    playAnim = true;
    drawDigit(digit3, 9, 1 + animStep);
    drawDigit(nextDigit3, 9, -5 + animStep);
    for (uint8_t x = 9; x < 12; x++) {
      savePixels[x] = false;  //Очищаем область сверху
      for (uint8_t y = 6; y < 8; y++)
        savePixels[x + y * WIDTH] = false;  //Очищаем область снизу
    }
  }

  //Четвертая цифра
  if (digit4 == nextDigit4) {
    drawDigit(digit4, 13, 1);
  }
  else {
    playAnim = true;
    drawDigit(digit4, 13, 1 + animStep);
    drawDigit(nextDigit4, 13, -5 + animStep);
    for (uint8_t x = 13; x < 16; x++) {
      savePixels[x] = false;  //Очищаем область сверху
      for (uint8_t y = 6; y < 8; y++)
        savePixels[x + y * WIDTH] = false;  //Очищаем область снизу
    }
  }

  static unsigned long timer = 0; //Счетчик таймера
  if (playAnim && millis() - timer > DIGIT_CHANGE_STEP) {
    //Делаем шаг анимации
    animStep++;
    timer = millis();
    if (animStep > 6) {
      animStep = 0;
      digit1 = nextDigit1;
      digit2 = nextDigit2;
      digit3 = nextDigit3;
      digit4 = nextDigit4;
    }
  }
  
  //Делаем ненужные пиксели черными
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      if (!savePixels[x + y * WIDTH]) {
        setPixel(x, y, CRGB::Black);
      }
    }
  }
}

/* Функция обновления и вывода часов на экран */
void updateClockMode() {
  //Если основной режим был только что изменен на часы
  if (isModeJustChanged) {
    changeClockMode(clockMode);
  }

  //Переключаем режимы часов
  if (secondaryButton.isClick()) {
    changeClockMode((clockMode + 1) % CLOCK_MODES_AMOUNT);
  }

  //Если включен режим смены часов и фона
  if (clockEnableBackgrounds && millis() - lastChangeClockMode > CHANGE_MODE_DELAY && clockModeChangingPart == 0) {
    lastChangeClockMode = millis();
    clockModeChangingPart = 1;
    fadeOut(CLOCK_FADE_TIME);
  }

  //Этапы анимации смены режимов
  switch (clockModeChangingPart) {
    case 1:
      if (fadeStep()) {
        clockModeChangingPart = 2;
        isBackgroundShowing = !isBackgroundShowing;
        clockBackground = isBackgroundShowing ? random(EFFECTS_AMOUNT) : random(sizeof(backgroundEffects));
        FastLED.clear();
        fadeIn(CLOCK_FADE_TIME);
      }
      break;
    case 2:
      if (fadeStep()) {
        clockModeChangingPart = 0;
      }
      break;
  }

  //Если в данный момент включен режим фона
  if (isBackgroundShowing && clockEnableBackgrounds) {
    drawEffect(clockBackground);
    return;
  }
  
  switch (clockMode) {
    case 0:
      //Заливаем экран цветом
      for (uint8_t i = 0; i < WIDTH * HEIGHT; i++) {
          pixels[i] = clockDigitsColor;
      }
      break;
    case 1:
      if (clockBackground >= sizeof(backgroundEffects)) {
        clockBackground = 0;
      }
      //Рисуем фон
      drawEffect(backgroundEffects[clockBackground]);
      break;
    default: 
      clockMode = 0;
      break;
  }
  drawDigits();
}
