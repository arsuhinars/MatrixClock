
#include "display.h"
#include "main.h"
#include "web_server.h"
#include "runner.h"

#define GAME_SPEED          200           //Базовая скорость игры
#define MIN_GAME_SPEED      50            //Минимальная скорость игры
#define MAX_OBJECTS         4             //Максимальное количество объектов на экране
#define PLAYER_COLOR        CRGB::Blue    //Цвет игрока
#define OBJECT_COLOR        CRGB::Green   //Цвет препятствия
#define PLAYER_HEIGHT       2             //Высота игрока
#define PLAYER_OFFSET       2             //Смещение игрока от левого края экрана
#define PLAYER_GRAVITY      1             //Ускорение свободного падения
#define JUMP_SPEED          9
#define FRAME_TIME          50            //Время обновления кадра

//Структура и массив объектов
struct object {
  bool spawned;       //Был ли данный объект заспавнен
  uint8_t position;   //Позиция объекта
  uint8_t height;     //Его высота
} runnerObjects[MAX_OBJECTS];

//Позиция игрока по Y
int8_t runnerPositionY = 0;
//Текущая скорость игрока по вертикали
int8_t runnerSpeedY = 0;

//Запущена ли игра
bool isRunnerStarted = false;

//Задержка между шагами игры
uint16_t runnerStepDelay = GAME_SPEED;

/* Функция обновления игры */
void updateRunner() {
  //Если был только что изменен режим, то сбрасываем игру
  if (isModeJustChanged) {
    handleRunnerEvent(RUNNER_EVENT_END);
  }
  
  //Обновляем по таймеру
  static unsigned long frameUpdateTimer = 0;
  if (millis() - frameUpdateTimer < FRAME_TIME) {
    return;
  }
  frameUpdateTimer = millis();

  //Очищаем экран
  FastLED.clear();

  static unsigned long gameUpdateTimer = 0;
  if (isRunnerStarted && millis() - gameUpdateTimer > runnerStepDelay) {
    gameUpdateTimer = millis();
    
    //Обновляем каждый из объектов
    for (uint8_t i = 0; i < MAX_OBJECTS; i++) {
      //Если объект пока не был создан
      if (!runnerObjects[i].spawned) {
        continue;
      }
      
      //Если объект коснулся края экрана
      if (runnerObjects[i].position == 0) {
        runnerObjects[i].spawned = false;
        continue;
      }

      //Сдвигаем объект влево
      runnerObjects[i].position -= 1;
    }

    //Счетчик для спавна объектов
    static uint8_t spawnCounter = 0;
    static uint8_t nextSpawnValue = 5;
    spawnCounter++;
  
    if (spawnCounter > nextSpawnValue) {
      //Ищем несозданый объект
      for (uint8_t i = 0; i < MAX_OBJECTS; i++) {
        if (runnerObjects[i].spawned) {
          continue;
        }
  
        //Создаем объект
        runnerObjects[i].spawned = true;
        runnerObjects[i].position = WIDTH - 1;
        runnerObjects[i].height = random(1, 4);
  
        //Обновляем счетчик
        spawnCounter = 0;
        nextSpawnValue = random(8, 15);
  
        break;
      }
    }
    
    //Счетчик шагов
    static uint8_t counter = 0;
    counter++;
    
    //Каждые сто шагов даем игроку очко и ускоряем игру
    if (counter > 100) {
      counter = 0;
      handleRunnerEvent(RUNNER_EVENT_GOT_SCORE);
      runnerStepDelay -= 15;
      if (runnerStepDelay < MIN_GAME_SPEED) {
        runnerStepDelay = MIN_GAME_SPEED;
      }
    }
  }

  //Рисуем каждый из объектов
  for (uint8_t i = 0; i < MAX_OBJECTS; i++) {
    if (!runnerObjects[i].spawned) {
      continue;
    }
    for (uint8_t j = 0; j < runnerObjects[i].height; j++) {
      setPixel(runnerObjects[i].position, HEIGHT - j - 1, OBJECT_COLOR);
    }
  }

  if (isRunnerStarted) {
    //Симулируем силу тяжести
    runnerSpeedY -= PLAYER_GRAVITY;
    //Перемещаем игрока
    runnerPositionY += runnerSpeedY;
  
    //Если игрок упал на землю
    if (runnerPositionY < 0) {
      runnerPositionY = 0;
      runnerSpeedY = 0;
    }
    
    //Проверяем на столкновения
    if ((uint32_t)getPixel(PLAYER_OFFSET, HEIGHT - 1 - runnerPositionY / 8) != 0) {
      handleRunnerEvent(RUNNER_EVENT_DEAD);
    }
  }

  //Рисуем игрока
  for (uint8_t i = 0; i < PLAYER_HEIGHT; i++) {
    setPixel(PLAYER_OFFSET, HEIGHT - 1 - i - runnerPositionY / 8, PLAYER_COLOR);
  }
}

/* Функция для обновления демки игры */
void updateRunnerDemo() {
  updateRunner();

  //Запускаем игру, если она не запущена
  if (!isRunnerStarted) {
    handleRunnerEvent(RUNNER_EVENT_START);
  }

  //В демо режиме заставляем игру работать со стандартной скоростью
  //runnerStepDelay = GAME_SPEED;

  //Проверяем область перед игроком
  for (uint8_t x = PLAYER_OFFSET + 1; x < PLAYER_OFFSET + 3; x++) {
    if ((uint32_t)getPixelLazy(x, HEIGHT - 1) != 0) {
      handleRunnerEvent(RUNNER_EVENT_JUMP);
      break;
    }
  }
}

/* Функция обработки входящий событий */
void handleRunnerEvent(uint8_t eventType) {
  switch (eventType) {
    case RUNNER_EVENT_START:
      //Удаляем все объекты
      for (uint8_t i = 0; i < MAX_OBJECTS; i++) {
        runnerObjects[i].spawned = false;
      }
      isRunnerStarted = true;
      runnerSpeedY = 0;
      runnerPositionY = 0;
      runnerStepDelay = GAME_SPEED;
      sendEventMessage(RUNNER_EVENT_START);
      break;
    case RUNNER_EVENT_END:
      isRunnerStarted = false;
      sendEventMessage(RUNNER_EVENT_END);
      break;
    case RUNNER_EVENT_DEAD:
      isRunnerStarted = false;
      sendEventMessage(RUNNER_EVENT_DEAD);
      break;
    case RUNNER_EVENT_JUMP:
      if (runnerPositionY == 0) {
        runnerSpeedY = JUMP_SPEED;
      }
      break;
    case RUNNER_EVENT_GOT_SCORE:
      sendEventMessage(RUNNER_EVENT_GOT_SCORE);
      break;
  }
}
