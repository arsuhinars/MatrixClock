#pragma once

#include <stdint.h>

#include "effects.h"

//Включен ли режим автоматической смены эффектов
extern bool effectAutoChangeFlag;

/* Отправить текущему подключенному пользователю данные об текущем эффекте */
void sendEffectModeData();

/* Функция изменения текущего эффекта */
void changeEffect(uint8_t effect);

/* Функция получения номера текущего эффекта */
uint8_t getCurrentEffect();

/* Функция обновления текущего эффекта */
void updateEffectMode();
