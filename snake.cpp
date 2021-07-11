
#include "display.h"
#include "main.h"
#include "web_server.h"
#include "config.h"
#include "snake.h"

//Цвет тела змейки
#define SNAKE_COLOR CRGB(0x0400ff);
//Цвет яблока
#define APPLE_COLOR CRGB(0xa200ff);

//Начальная длина змейки
#define START_SNAKE_LENGTH 3

//Текущее направление движения змейки
//0 - вверх
//1 - вправо
//2 - вниз
//3 - влево
uint8_t snakeDir = 0;
//Будущая позиция змейки
uint8_t snakeNextDir = 0;

//Позиции клеток занимаемые телом змейки
//Каждая элемент массива, стоящий на четном месте - это координата по горизонтали от левого края
//Стоящий на нечетном месте - координата по вертикали от верхнего края
uint8_t snakeBody[WIDTH * HEIGHT * 2];
//Текущая длина змейки
uint16_t snakeLength = 0;

//Координата яблока
uint8_t appleX = 0, appleY = 0;

bool isSnakeStarted = false;        //Запущена ли игра
uint16_t snakeSpeed = 200;          //Скорость обновления змейки (в миллисекундах)
bool snakeBarrierEnabled = false;   //Есть ли барьер на карте

/* Проверить горизонтальную линию на наличие столкновений */
bool snakeCheckLineX(int8_t y, int8_t start, int8_t end) {
  for (int8_t x = start; x <= end; x++) {
    if ((uint32_t)getPixel(mod(x, WIDTH), mod(y, HEIGHT)) != 0) {
      return true;
    }
  }
  return false;
}

/* Проверить вертикальную линию на наличие столкновений */
bool snakeCheckLineY(int8_t x, int8_t start, int8_t end) {
  for (int8_t y = start; y <= end; y++) {
    if ((uint32_t)getPixel(mod(x, WIDTH), mod(y, HEIGHT)) != 0) {
      return true;
    }
  }
  return false;
}

/* Функция спавна яблока */
void snakeSpawnApple() {
  uint8_t freeX = 0, freeY = 0;
  //Ищем свободную точку и спавним там яблоко
  for (uint8_t x = 0; x < WIDTH; x++) {
    for (uint8_t y = 0; y < HEIGHT; y++) {
      if ((uint32_t)getPixel(x, y) == 0) {
        freeX = x; freeY = y;
        if (!random(32)) {
          appleX = x; appleY = y;
          return;
        }
      }
    }
  }
  appleX = freeX;
  appleY = freeY;
}

/* Функция запуска игры */
void snakeStart() {
  //Сбрасываем параметры
  snakeLength = START_SNAKE_LENGTH;
  snakeDir = 1;
  snakeNextDir = 1;
  isSnakeStarted = true;

  //Координаты головы в начале игры
  static uint8_t headX = WIDTH / 2, headY = HEIGHT / 2;
  //Создаем тело змейки
  for (uint8_t i = 0; i < START_SNAKE_LENGTH; i++) {
    snakeBody[i * 2] = headX - i;
    snakeBody[i * 2 + 1] = headY;
  }

  snakeSpawnApple();
}

/* Функция обновления змейки */
void updateSnake() {
  //Если был только что изменен режим, то сбрасываем игру
  if (isModeJustChanged) {
    handleSnakeEvent(SNAKE_EVENT_END);
  }

  //Обновляем по таймеру
  static unsigned long timer = 0;
  if (millis() - timer < snakeSpeed)
    return;
  timer = millis();

  //Очищаем экран
  FastLED.clear();

  bool isAppleEaten = false;
  if (isSnakeStarted) {
    //Если змейка съела яблоко
    isAppleEaten = (snakeBody[0] == appleX && snakeBody[1] == appleY);
    if (isAppleEaten) {
      snakeLength += 1; //то увеличиваем её длину
    }
      
    //Смещаем все клетки змейки к голове
    for (uint16_t i = snakeLength - 1; i > 0; i--) {
      snakeBody[i * 2] = snakeBody[(i - 1) * 2];
      snakeBody[i * 2 + 1] = snakeBody[(i - 1) * 2 + 1];
    }
    
    snakeDir = snakeNextDir;
    
    //Двигаем голову по направлению
    switch (snakeDir) {
      case 0: //Движение вверх
        //Если змейка уперлась в стенку
        if (snakeBody[1] == 0) {
          if (snakeBarrierEnabled) {
            handleSnakeEvent(SNAKE_EVENT_LOSE);
          } else { 
            snakeBody[1] = HEIGHT - 1;
          }
        }
        else
          snakeBody[1] -= 1;
        break;
      case 1: //Движение вправо
        //Если змейка уперлась в стенку
        if (snakeBody[0] == WIDTH - 1) {
          if (snakeBarrierEnabled) {
            handleSnakeEvent(SNAKE_EVENT_LOSE);
          } else {
            snakeBody[0] = 0;
          }
        }
        else
          snakeBody[0] += 1;
        break;
      case 2: //Движение вниз
        //Если змейка уперлась в стенку
        if (snakeBody[1] == HEIGHT - 1) {
          if (snakeBarrierEnabled) {
            handleSnakeEvent(SNAKE_EVENT_LOSE);
          } else {
            snakeBody[1] = 0;
          }
        }
        else
          snakeBody[1] += 1;
        break;
      case 3: //Движение влево
        //Если змейка уперлась в стенку
        if (snakeBody[0] == 0) {
          if (snakeBarrierEnabled) {
            handleSnakeEvent(SNAKE_EVENT_LOSE);
          } else {
            snakeBody[0] = WIDTH - 1;
          }
        }
        else
          snakeBody[0] -= 1;
        break;
    }

    for (uint16_t i = 1; i < snakeLength; i++) {
      //Если змейка съела сама себя
      if (snakeBody[0] == snakeBody[i * 2] && snakeBody[1] == snakeBody[i * 2 + 1]) {
        handleSnakeEvent(SNAKE_EVENT_LOSE);
        break;
      }
    }
  }

  //Отрисовываем змейку
  for (uint8_t i = 0; i < snakeLength; i++) {
    uint8_t x = snakeBody[i * 2];
    uint8_t y = snakeBody[i * 2 + 1];
    setPixel(x, y, SNAKE_COLOR);
  }

  //Спавним новое яблоко
  if (isAppleEaten) {
    snakeSpawnApple();
    handleSnakeEvent(SNAKE_EVENT_EAT);
  }

  //Рисуем яблоко
  setPixel(appleX, appleY, APPLE_COLOR);
}

/* Функция обновления демо режима */
void updateSnakeDemo() {
  //Обновляем и рисуем змейку
  updateSnake();

  //В демо режиме игра всегда запущена
  if (!isSnakeStarted) {
    snakeSpeed = 200;
    snakeBarrierEnabled = false;
    snakeStart();
  }

  //Радиус видимости у змейки
  static uint8_t snakeViewRadius = 2;

  //Следующая сторона, в которую поползет змейка
  uint8_t _nextSnakeDir = snakeDir;

  //Проверяем на столкновения
  switch (snakeDir) {
    //Змейка движется вверх
    case 0:
      //Проверяем вверху змейки
      if (snakeCheckLineY(snakeBody[0], snakeBody[1] - snakeViewRadius, snakeBody[1] - 1)) {
        //Если сверху есть препятствие, то изменяем направление змейки
        _nextSnakeDir = random(2) ? 1 : 3;
      }
      break;
    //Змейка движется направо
    case 1:
      //Проверяем справа от змейки
      if (snakeCheckLineX(snakeBody[1], snakeBody[0] + 1, snakeBody[0] + snakeViewRadius)) {
        _nextSnakeDir = random(2) ? 0 : 2;
      }
      break;
    //Змейка движется вниз
    case 2:
      //Проверяем внизу змейки
      if (snakeCheckLineY(snakeBody[0], snakeBody[1] + 1, snakeBody[1] + snakeViewRadius)) {
        _nextSnakeDir = random(2) ? 1 : 3;
      }
      break;
    //Змейка движется налево
    case 3:
      //Проверяем слева от змейки
      if (snakeCheckLineX(snakeBody[1], snakeBody[0] - snakeViewRadius, snakeBody[0] - 1)) {
        _nextSnakeDir = random(2) ? 0 : 2;
      }
      break;
  }

  if (_nextSnakeDir != snakeDir) {
    //Проверяем новую сторону для движения на препятствия
    switch (_nextSnakeDir) {
      case 0:
        if (snakeCheckLineY(snakeBody[0], snakeBody[1] - snakeViewRadius, snakeBody[1] - 1)) {
          _nextSnakeDir = (_nextSnakeDir + 2) % 4;  //Иначе меняем на противоположную
        }
        break;
      case 1:
        if (snakeCheckLineX(snakeBody[1], snakeBody[0] + 1, snakeBody[0] + snakeViewRadius)) {
          _nextSnakeDir = (_nextSnakeDir + 2) % 4;  //Иначе меняем на противоположную
        }
        break;
      case 2:
        if (snakeCheckLineY(snakeBody[0], snakeBody[1] + 1, snakeBody[1] + snakeViewRadius)) {
          _nextSnakeDir = (_nextSnakeDir + 2) % 4;  //Иначе меняем на противоположную
        }
        break;
      case 3:
        if (snakeCheckLineX(snakeBody[1], snakeBody[0] - snakeViewRadius, snakeBody[0] - 1)) {
          _nextSnakeDir = (_nextSnakeDir + 2) % 4;  //Иначе меняем на противоположную
        }
        break;
    }
    snakeViewRadius = random(1, 4);
  }

  //Двигаемся в новую сторону
  handleSnakeEvent(SNAKE_EVENT_UP + _nextSnakeDir);

  //Делаем проверку на нахождение рядом с яблоком
  if (snakeBody[0] == appleX) {
    if (appleY < snakeBody[1]) {
      //Проверяем на препятствия между яблоком и змейкой
      if (!snakeCheckLineY(snakeBody[0], appleY + 1, snakeBody[1] - 1)) {
        handleSnakeEvent(SNAKE_EVENT_UP);
      }
    } else if (appleY > snakeBody[1]) {
      if (!snakeCheckLineY(snakeBody[0], snakeBody[1] + 1, appleY - 1)) {
        handleSnakeEvent(SNAKE_EVENT_DOWN);
      }
    }
  }
  else if (snakeBody[1] == appleY) {
    if (appleX > snakeBody[0]) {
      if (!snakeCheckLineX(snakeBody[1], snakeBody[0] + 1, appleX - 1)) {
        handleSnakeEvent(SNAKE_EVENT_RIGHT);
      }
    } else if (appleX < snakeBody[0]) {
      if (!snakeCheckLineX(snakeBody[1], appleX + 1, snakeBody[0] - 1)) {
        handleSnakeEvent(SNAKE_EVENT_LEFT);
      }
    }
  }
}

/* Функция обработчик входных команд */
void handleSnakeEvent(uint8_t eventType) {
  switch (eventType) {
    //Событие запуска игры
    case SNAKE_EVENT_START:
      snakeStart();
      sendEventMessage(SNAKE_EVENT_START);
      break;
    //Событие проигрыша
    case SNAKE_EVENT_LOSE:
      isSnakeStarted = false;
      sendEventMessage(SNAKE_EVENT_LOSE);
      break;
    //События окончания игры
    case SNAKE_EVENT_END:
      isSnakeStarted = false;
      sendEventMessage(SNAKE_EVENT_END);
      break;
    //События нажатия кнопки вверх
    case SNAKE_EVENT_UP:
      if (snakeDir != 2)
        snakeNextDir = 0;
      break;
    //События нажатия кнопки вправо
    case SNAKE_EVENT_RIGHT:
      if (snakeDir != 3)
        snakeNextDir = 1;
      break;
    //События нажатия кнопки вниз
    case SNAKE_EVENT_DOWN:
      if (snakeDir != 0)
        snakeNextDir = 2;
      break;
    //События нажатия кнопки влево
    case SNAKE_EVENT_LEFT:
      if (snakeDir != 1)
        snakeNextDir = 3;
      break;
    //Событие поедания яблока
    case SNAKE_EVENT_EAT:
      sendEventMessage(SNAKE_EVENT_EAT);
      break;
  }
}
