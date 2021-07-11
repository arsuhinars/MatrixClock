#pragma once

#include <Arduino.h>
#include <stdint.h>

#include <GyverButton.h>

/* Вспомогательная функция для взятия остатка с учетом отрицательности числа */
#define mod(a, b) ((b) + (a) % (b)) % (b)

//Количество основных режимов
#define MODE_COUNT  2

//Номера основных режимов
#define MODE_CLOCK          0
#define MODE_EFFECTS        1
#define MODE_RUNNING_LINE   2
#define MODE_PAINT          3
#define MODE_GAME_SNAKE     4
#define MODE_GAME_RUNNER    5

//Создаем объекты для кнопок
extern GButton mainButton;
extern GButton secondaryButton;

extern bool autoBrightness;     //Режим автоподстройки яркости
extern bool isModeJustChanged;  //Изменился ли только-что основной режим
extern String wifiSSID;         //Имя сети, к которой будем подключаться
extern String wifiPass;         //Пароль от этой сети

/* Функция инитилизации основного компонента */
void initMain();

/* Функция обновления основного компонента */
void updateMain();

/* Отправить текущему подключенному пользователю данные об текущем основном режиме */
void sendModeData();

/* Функция смены текущего основного режима */
void changeMode(uint8_t mode);

/* Функция для получения номера текущего режима */
uint8_t getCurrentMode();
