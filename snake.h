#pragma once

#include <stdint.h>

//Типы входящих команд
#define SNAKE_EVENT_START 0
#define SNAKE_EVENT_END   1
#define SNAKE_EVENT_LOSE  2
#define SNAKE_EVENT_UP    3
#define SNAKE_EVENT_RIGHT 4
#define SNAKE_EVENT_DOWN  5
#define SNAKE_EVENT_LEFT  6
#define SNAKE_EVENT_EAT   7

//Скорость обновления змейки (в миллисекундах)
extern uint16_t snakeSpeed;

//Есть ли барьер на карте
extern bool snakeBarrierEnabled;

/* Функция обновления змейки */
void updateSnake();

/* Функция обновления демо режима  */
void updateSnakeDemo();

/* Функция обработчик входных команд */
void handleSnakeEvent(uint8_t eventType);
