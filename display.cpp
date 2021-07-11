
#include "config.h"
#include "display.h"

//Массив светодиодов матрицы
CRGB pixels[WIDTH * HEIGHT];

uint8_t matrixBrightness = 25;  //Текущая яркость матрицы
uint8_t brightnessMul = 255;    //Множитель яркости, деленный на 255

/* Получить номер пикселя в массиве */
uint8_t getPixelNumber(uint8_t x, uint8_t y) {
  static const uint8_t halfWidth = WIDTH / 2;  //Половина ширины
  static const uint8_t halfPixelsCount = WIDTH * HEIGHT / 2;  //Кол-во пикселей в половине экрана
  if (x < halfWidth) {  //Если точка в левой части экрана
    return x + y * halfWidth;
  } else {  //Если точка в правой части экрана
    return halfPixelsCount + (x - halfWidth) + y * halfWidth;
  }
}

/* Получить цвет пикселя в координатах x, y с проверкой на пределы */
CRGB getPixel(uint8_t x, uint8_t y) {
  return (x < WIDTH && y < HEIGHT) ? getPixelLazy(x, y) : CRGB::Black;
}

/* Функция инитилизации матрицы */
void initDisplay() {
  FastLED.addLeds<LED_TYPE, MATRIX_PIN, GRB>(pixels, WIDTH * HEIGHT).setCorrection(TypicalLEDStrip);
  pinMode(13, OUTPUT);
}

/* Функция обновления матрицы и вывода всей графики */
void updateDisplay() {
  //Исправляем мерцания на низкой яркости
  for (uint8_t i = 0; i < WIDTH * HEIGHT; i++) {
    if (pixels[i].r < 25 &&
      pixels[i].g < 25 &&
      pixels[i].b < 25)
      pixels[i] = CRGB::Black;
  }

  //Выводим всю графику
  FastLED.setBrightness(scale8(matrixBrightness, brightnessMul));
  FastLED.show();
}
