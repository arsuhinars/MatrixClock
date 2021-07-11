
#include <ArduinoOTA.h>

#include "config.h"
#include "display.h"
#include "web_server.h"
#include "snake.h"
#include "runner.h"
#include "clock.h"
#include "effects_mode.h"
#include "text.h"
#include "main.h"

uint8_t currentMode = MODE_CLOCK; //Текущий режим при запуске
bool autoBrightness = false;      //Режим автоподстройки яркости
bool isModeJustChanged = false;   //Изменился ли только-что основной режим
String wifiSSID = WIFI_SSID;      //Имя сети, к которой будем подключаться
String wifiPass = WIFI_PASS;      //Пароль от этой сети

//Создаем объекты для кнопок
GButton mainButton(MODE_BUTTON_PIN);
GButton secondaryButton(SUBMODE_BUTTON_PIN);

/* Функция инитилизации основного компонента */
void initMain() {
  
}

/* Основная функция обновления */
void updateMain() {
  //Яркость к которой нужно изменять текущую
  static uint8_t targetBrightness = matrixBrightness;
  //Таймер обновления яркости
  static unsigned long brightnessTimer = 0;
  
  //Если включен режим авто-подстройки яркости
  if (autoBrightness) {
    //Если пришло время обновлять яркость
    if (millis() - brightnessTimer > BRIGHTNESS_TIME) {
      brightnessTimer = millis();
      int16_t val = analogRead(LIGHT_SENSOR_PIN);
      val = map(val, MIN_LIGHT_VALUE, MAX_LIGHT_VALUE, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
      targetBrightness = (uint8_t)constrain(val, 0, 255);
    }
    //Плавно изменяем яркость
    matrixBrightness = matrixBrightness + (int16_t)(targetBrightness - matrixBrightness) * (int16_t)(millis() - brightnessTimer) /  (int16_t)BRIGHTNESS_TIME;
  }

  //Обновляем кнопки
  mainButton.tick();
  secondaryButton.tick();

  //Если была нажата кнопка смены режима
  if (mainButton.isClick()) {
    changeMode((currentMode + 1) % MODE_COUNT);
  }

  //Обновляем текущий режим
  switch (currentMode) {
    case MODE_CLOCK: updateClockMode(); break;            //Режим часов
    case MODE_EFFECTS: updateEffectMode(); break;         //Режим фоновой заставки
    case MODE_RUNNING_LINE: updateRunningLine(); break;   //Режим вывода бегущей строки
    case MODE_PAINT: break;                               //Режим рисования
    case MODE_GAME_SNAKE: updateSnake(); break;           //Режим игры "Змейка"
    case MODE_GAME_RUNNER: updateRunner(); break;         //Режим игры раннер
    default: currentMode = 0; break;    //На случай, если режим выйдет за пределы доступного
  }
  updateDisplay();     //Обновляем картинку

  //Переключаем состояние флага
  isModeJustChanged = false;
}

/* Отправить текущему подключенному пользователю данные об текущем основном режиме */
void sendModeData() {
  uint8_t data[] = {CMD_CHANGE_MODE, currentMode};
  sendMessage(data, sizeof(data));
}

/* Функция смены текущего основного режима */
void changeMode(uint8_t mode) {
  currentMode = mode;
  isModeJustChanged = true;
  brightnessMul = 255;
  FastLED.clear();

  sendModeData();
}

/* Функция для получения номера текущего режима */
uint8_t getCurrentMode() {
  return currentMode;
}
