
#include <stdint.h>

#include "display.h"
#include "fader.h"

uint8_t fadeMode = 0;             //Текущий режим. 0 - затухание, 1 - нарастание
unsigned long fadeStart = 0;      //Время начала анимации
unsigned long fadeDuration = 0;   //Длительность анимации

/* Функция для плавного затухания яркости */
void fadeOut(unsigned long time) {
  fadeMode = 0;
  fadeDuration = time;
  fadeStart = millis();
}

/* Функция плавного нарастания яркости */
void fadeIn(unsigned long time) {
  fadeMode = 1;
  fadeDuration = time;
  fadeStart = millis();
}

/* 
 *  Функция обновления затухания/нарастания. Необходимо постоянно опрашивать.
 *  Возвращает истинну, когда анимация закончится.
*/
bool fadeStep() {
  //Если анимация закончилась
  if (millis() - fadeStart >= fadeDuration) {
    return true;
  }
  
  if (fadeMode == 0) {
    brightnessMul = 255 - (millis() - fadeStart) * 255 / fadeDuration;
  } else {
    brightnessMul = (millis() - fadeStart) * 255 / fadeDuration;
  }
  return false;
}
