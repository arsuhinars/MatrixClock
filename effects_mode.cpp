
#include "main.h"
#include "display.h"
#include "web_server.h"
#include "fader.h"
#include "effects_mode.h"

#define EFFECTS_CHANGE_DELAY 10           //Время смены режимов в секундах
#define EFFECTS_FADE_TIME    1500         //Время перехода между эффектами

uint8_t currentEffect;                    //Текущий эффект
bool effectAutoChangeFlag = true;         //Включен ли режим автоматической смены режимов

uint8_t nextEffect = 0;                   //Следующий эффект
unsigned long effectChangeTime = 0;       //Время предыдущей смены эффекта
uint8_t changeEffectPart = 0;             //Этап анимации смены эффектов. 0 - закончили, 1 - угасание, 2 - нарастание

/* Отправить текущему подключенному пользователю данные об текущем эффекте */
void sendEffectModeData() {
  if (getCurrentMode() == MODE_EFFECTS) {
    uint8_t data[] = {CMD_CHANGE_SUBMODE, currentEffect};
    sendMessage(data, sizeof(data));
  }
}

/* Функция изменения текущего эффекта */
void changeEffect(uint8_t effect) {
  //Сбрасываем все значения
  currentEffect = effect;
  nextEffect = effect;
  brightnessMul = 255;
  changeEffectPart = 0;
  effectChangeTime = millis();
  FastLED.clear();

  sendEffectModeData();
}

/* Функция получения номера текущего эффекта */
uint8_t getCurrentEffect() {
  return currentEffect;
}

/* Функция обновления фона */
void updateEffectMode() {
  //Если основной режим был только что изменен на текущий
  if (isModeJustChanged) {
    changeEffect(currentEffect);
  }

  //Переключаем режимы эффектов
  if (secondaryButton.isClick()) {
    changeEffect((currentEffect + 1) % EFFECTS_AMOUNT);
  }
    
  //Если нужно изменять режим
  if (currentEffect != nextEffect && changeEffectPart == 0) {
    fadeOut(EFFECTS_FADE_TIME);
    changeEffectPart = 1;
  }

  //Этапы анимации смены эффектов
  switch (changeEffectPart) {
    //Этап угасания
    case 1:
      if (fadeStep()) {
        changeEffectPart = 2;
        currentEffect = nextEffect;
        sendEffectModeData();
        FastLED.clear();
        fadeIn(EFFECTS_FADE_TIME);
      }
      break;
    //Этап нарастания
    case 2:
      if (fadeStep()) {
        changeEffectPart = 0;
      }
      break;
  }

  //Если включен флаг автоматической смены эффектов
  if (effectAutoChangeFlag && changeEffectPart == 0) {
    //Если пришло время изменять режим
    if (millis() - effectChangeTime > EFFECTS_CHANGE_DELAY * 1000) {
      effectChangeTime = millis();
      nextEffect = random(EFFECTS_AMOUNT);  //Изменяем эффект на случайный
    }
  }

  //Рисуем текущий эффект
  drawEffect(currentEffect);
}
