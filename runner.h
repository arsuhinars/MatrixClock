#pragma once

#include <stdint.h>

//Типы событий игры
#define RUNNER_EVENT_START      0   //Событие запуска игры
#define RUNNER_EVENT_END        1   //Событие остановки игры
#define RUNNER_EVENT_DEAD       2   //Событие смерти/проигрыша игрока
#define RUNNER_EVENT_JUMP       3   //Событие прыжка игрока
#define RUNNER_EVENT_GOT_SCORE  4   //Событие получения одного очка

/* Функция обновления игры */
void updateRunner();

/* Функция для обновления демки игры */
void updateRunnerDemo();

/* Функция обработки входящий событий */
void handleRunnerEvent(uint8_t eventType);
