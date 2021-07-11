
#include <stdint.h>

#include "main.h"
#include "display.h"
#include "config.h"
#include "snake.h"
#include "runner.h"
#include "effects.h"

#include "text.h"

/* Настройки эффектов */
#define RAINBOW_WIDTH             3           //"Ширина" радуги
#define RAINBOW_STEP_TIME         20          //Время шага обновления радуги в микросекундах. Можно управлять скоростью анимации.

#define MATRIX_DENSITY            8           //Плотность линий
#define MATRIX_FADE_SPEED         102         //Скорость угасания линий
#define MATRIX_SPEED              100         //Скорость одного шага обновления матрицы в миллисекундах

#define SNOWFALL_DENSITY          10          //Плотность снежинок
#define SNOWFALL_SPEED            250         //Скорость одного шага обновления в миллисекундах

#define SPARKLES_DENSITY          3           //Плотность конфети
#define SPARKLES_SPEED            100         //Скорость анимации конфети
#define SPARKLES_FADE_SPEED       70          //Скорость угасания конфети

#define STARFALL_DENSITY          12          //Плотность звездопада
#define STARFALL_SPEED            100         //Скорость звездопада
#define STARFALL_FADE_SPEED       70          //Скорость угасания звездопада

//Эффект "огонь"
#define SPARKLES 1        //Отлетающие угольки вкл выкл
#define HUE_ADD 0         //Добавка цвета в огонь (от 0 до 230) - меняет весь цвет пламени

/* Функция отрисовки эффекта по его номеру */
void drawEffect(uint8_t effect) {
  switch (effect) {
    case 0: drawRunningRainbow(); break;
    case 1: drawMatrix(); break;
    case 2: drawSnowfall(); break;
    case 3: drawSparkles(); break;
    case 4: drawStarfall(); break;
    case 5: drawEqualizer(); break;
    case 6: cloudNoise(); break;
    case 7: lavaNoise(); break;
    case 8: rainbowNoise(); break;
    case 9: plasmaNoise(); break;
    case 10: oceanNoise(); break;
    case 11: fireRoutine(); break;
    case 12: updateSnakeDemo(); break;
    case 13: updateRunnerDemo(); break;
  }
}

/* Функция отрисовки бегущей радуги */
void drawRunningRainbow() {
  for (uint8_t x = 0; x < WIDTH; x++) {
    CRGB color = CHSV(millis() / RAINBOW_STEP_TIME + x * RAINBOW_WIDTH, 255, 255);
    for (uint8_t y = 0; y < HEIGHT; y++)
      setPixel(x, y, color);
  }
}

/* Эффект из фильма Матрица */
void drawMatrix() {
  static unsigned long timer = 0;
  if (millis() - timer < MATRIX_SPEED) {
    return;
  }
  timer = millis();
  
  for (uint8_t x = 0; x < WIDTH; x++) {
    //Смещаем все пиксели вниз
    for (uint8_t y = HEIGHT - 1; y > 0; y--) {
      setPixel(x, y, getPixelLazy(x, y - 1));
    }
    
    CRGB color = getPixelLazy(x, 0);
    if ((uint32_t)color == 0 &&
      !random(MATRIX_DENSITY) &&
      (uint32_t)getPixel(x - 1, 0) == 0 &&
      (uint32_t)getPixel(x + 1, 0) == 0) {
      //Генерируем первую линию
      color = CRGB::Green;
    } else {
      //Угасаем первую линию
      color = color.fadeToBlackBy(MATRIX_FADE_SPEED);
    }
    setPixel(x, 0, color);
  }
}

/* Эффект падающего снега */
void drawSnowfall() {
  static unsigned long timer = 0;
  if (millis() - timer < SNOWFALL_SPEED) {
    return;
  }
  timer = millis();
  
  for (uint8_t x = 0; x < WIDTH; x++) {
    //Смещаем все пиксели вниз
    for (uint8_t y = HEIGHT - 1; y > 0; y--) {
      setPixel(x, y, getPixelLazy(x, y - 1));
    }
    setPixel(x, 0, CRGB::Black);
    //Генерируем первую линию
    if ((uint32_t)getPixelLazy(x, 0) == 0 &&
      !random(SNOWFALL_DENSITY) &&
      (uint32_t)getPixel(x + 1, 0) == 0 &&
      (uint32_t)getPixel(x - 1, 0) == 0 &&
      (uint32_t)getPixelLazy(x, 1) == 0) {
      setPixel(x, 0, CRGB::White);
    }
  }
}

/* Эффект конфети */
void drawSparkles() {
  static unsigned long timer = 0;
  if (millis() - timer < SPARKLES_SPEED) {
    return;
  }
  timer = millis();
  
  for (uint8_t i = 0; i < SPARKLES_DENSITY; i++) {
    //Генерируем координату следующей частицы
    uint8_t x = random(WIDTH);
    uint8_t y = random(HEIGHT);
    //Создаем частицу
    if ((uint32_t)getPixelLazy(x, y) == 0)
      setPixel(x, y, CHSV(random(255), 255, 255));
  }
  //Затухаем пиксели
  for (uint8_t x = 0; x < WIDTH; x++) {
    for (uint8_t y = 0; y < HEIGHT; y++) {
      CRGB color = getPixelLazy(x, y);
      color.fadeToBlackBy(SPARKLES_FADE_SPEED);
      setPixel(x, y, color);
    }
  }
}

/* Эффект звездопада */
void drawStarfall() {
  static unsigned long timer = 0;
  if (millis() - timer < STARFALL_SPEED) {
    return;
  }
  timer = millis();
  
  //Смещаем звезды по диагонали
  for (uint8_t x = WIDTH - 1; x > 0; x--) {
    for (uint8_t y = HEIGHT - 1; y > 0; y--) {
      setPixel(x, y, getPixelLazy(x - 1, y - 1));
    }
  }

  for (uint8_t x = 0; x < WIDTH; x++) {
    CRGB color = getPixelLazy(x, 0);
    //Генерируем звезды на верхней линии
    if (x < WIDTH * 3 / 4 &&
      (uint32_t)color == 0 &&
      !random(STARFALL_DENSITY) &&
      (uint32_t)getPixel(x - 1, 0) == 0 &&
      (uint32_t)getPixel(x + 1, 0) == 0){
      color = CHSV(random(255), 255, 255);
    } else {  //Угасаем звезды на верхней линии
      color.fadeToBlackBy(STARFALL_FADE_SPEED);
    }
    setPixel(x, 0, color);
  }

  for (uint8_t y = 0; y < HEIGHT; y++) {
    CRGB color = getPixelLazy(0, y);
    //Генерируем звезды на левой линии
    if (y < HEIGHT * 3 / 4 &&
      (uint32_t)color == 0 &&
      random(STARFALL_DENSITY) &&
      (uint32_t)getPixel(0, y - 1) == 0 &&
      (uint32_t)getPixel(0, y + 1) == 0){
      color = CHSV(random(255), 255, 255);
    }
    //Угасаем звезды на левой линии
    else {
      color.fadeToBlackBy(STARFALL_FADE_SPEED);
    }
    setPixel(0, y, color);
  }
}

const uint8_t equalizerScales[WIDTH] = {
  175, 82, 133, 134, 227, 171, 150, 82, 102, 161, 134, 104, 227, 103, 161, 240
};

const uint16_t equalizerOffsets[WIDTH] = {
  1958, 6603, 5730, 1950, 7348, 2756, 3763, 1326, 569, 8153, 6413, 4925, 6558, 3189, 170, 4420
};

const uint8_t equalizerAmplitudes[WIDTH] = {
  255, 255, 255, 255, 236, 217, 199, 180, 180, 180, 192, 204, 217, 217, 217, 217
};

uint8_t equalizerMaximums[WIDTH] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

uint16_t equalizerMaxTimers[WIDTH] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Эффект симуляции работы эквалайзера */
void drawEqualizer() {
  static unsigned long timer = 0;
  
  CRGB columnsColor = CHSV(millis() / 100, 255, 255);
  
  FastLED.clear();
  for (uint8_t x = 0; x < WIDTH; x++) {
    uint8_t h = inoise8(equalizerScales[x] * x + millis() * 5 / 4 + equalizerOffsets[x]);

    h = scale8(scale8(HEIGHT, equalizerAmplitudes[x]), h);
    for (uint8_t y = HEIGHT - 1; y > HEIGHT - h; y--) {
      setPixel(x, y, columnsColor);
    }

    if (h > equalizerMaximums[x]) {
      equalizerMaximums[x] = h;
      equalizerMaxTimers[x] = 0;
    }

    setPixel(x, HEIGHT - equalizerMaximums[x], columnsColor);
  }

  if (millis() - timer > 100) {
    timer = millis();
    for (uint8_t x = 0; x < WIDTH; x++) {
      if (equalizerMaxTimers[x] < 5) {
        equalizerMaxTimers[x]++;
      } else {
        if (equalizerMaximums[x] > 0)
          equalizerMaximums[x]--;
      }
    }
  }
}

/* Эффект шума Перлина */
inline void perlinNoise(const CRGBPalette16 &palette, const uint8_t scale, const uint32_t moveSpeed, const uint32_t waveSpeed) {
  for (uint8_t x = 0; x < WIDTH; x++) {
    for (uint8_t y = 0; y < HEIGHT; y++) {
      uint8_t noise = inoise8(x * scale + millis() / moveSpeed, y * scale + millis() / moveSpeed, millis() / waveSpeed);
      setPixel(x, y, ColorFromPalette(palette, noise));
    }
  }
}

/* Эффект облачного шума */
void cloudNoise() {
  perlinNoise(CloudColors_p, 32, 8, 32);
}

/* Эффект лавового шума */
void lavaNoise() {
  perlinNoise(LavaColors_p, 32, 16, 4);
}

/* Эффект радужного шума */
void rainbowNoise() {
  perlinNoise(RainbowColors_p, 16, 32, 4);
}

/* Эффект плазмы */
void plasmaNoise() {
  perlinNoise(PartyColors_p, 16, 32, 4);
}

/* Эффект шума океана */
void oceanNoise() {
  perlinNoise(OceanColors_p, 32, 8, 4);
}

/* Эффект шума  */

// ********************** огонь **********************
unsigned char matrixValue[8][16];
unsigned char line[WIDTH];
int pcnt = 0;

//these values are substracetd from the generated values to give a shape to the animation
const unsigned char valueMask[8][16] PROGMEM = {
  {32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
  {64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 , 64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
  {96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 , 96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 },
  {128, 64 , 32 , 0  , 0  , 32 , 64 , 128, 128, 64 , 32 , 0  , 0  , 32 , 64 , 128},
  {160, 96 , 64 , 32 , 32 , 64 , 96 , 160, 160, 96 , 64 , 32 , 32 , 64 , 96 , 160},
  {192, 128, 96 , 64 , 64 , 96 , 128, 192, 192, 128, 96 , 64 , 64 , 96 , 128, 192},
  {255, 160, 128, 96 , 96 , 128, 160, 255, 255, 160, 128, 96 , 96 , 128, 160, 255},
  {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}
};

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
const unsigned char hueMask[8][16] PROGMEM = {
  {1 , 11, 19, 25, 25, 22, 11, 1 , 1 , 11, 19, 25, 25, 22, 11, 1 },
  {1 , 8 , 13, 19, 25, 19, 8 , 1 , 1 , 8 , 13, 19, 25, 19, 8 , 1 },
  {1 , 8 , 13, 16, 19, 16, 8 , 1 , 1 , 8 , 13, 16, 19, 16, 8 , 1 },
  {1 , 5 , 11, 13, 13, 13, 5 , 1 , 1 , 5 , 11, 13, 13, 13, 5 , 1 },
  {1 , 5 , 11, 11, 11, 11, 5 , 1 , 1 , 5 , 11, 11, 11, 11, 5 , 1 },
  {0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 , 0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 },
  {0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 , 0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 },
  {0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 }
};

// draw a frame, interpolating between 2 "key frames"
// @param pcnt percentage of interpolation
void drawFrame(int pcnt) {
  int nextv;

  //each row interpolates with the one before it
  for (unsigned char y = HEIGHT - 1; y > 0; y--) {
    for (unsigned char x = 0; x < WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x - 15;
      if (y < 8) {
        nextv =
          (((100.0 - pcnt) * matrixValue[y][newX]
            + pcnt * matrixValue[y - 1][newX]) / 100.0)
          - pgm_read_byte(&(valueMask[y][newX]));

        CRGB color = CHSV(
                       HUE_ADD + pgm_read_byte(&(hueMask[y][newX])), // H
                       255, // S
                       (uint8_t)max(0, nextv) // V
                     );

        setPixel(x, HEIGHT - 1 - y, color);
      } else if (y == 8 && SPARKLES) {
        if (random(0, 20) == 0 && (uint32_t)getPixel(x, HEIGHT - 2 - y) != 0) setPixel(x, HEIGHT - 1 - y, getPixel(x, HEIGHT - 2 - y));
        else setPixel(x, HEIGHT - 1 - y, 0);
      } else if (SPARKLES) {
        // старая версия для яркости
        if ((uint32_t)getPixel(x, HEIGHT - 2 - y) > 0)
          setPixel(x, HEIGHT - 1 - y, getPixel(x, HEIGHT - 2 - y));
        else setPixel(x, HEIGHT - 1 - y, 0);
      }
    }
  }

  //first row interpolates with the "next" line
  for (unsigned char x = 0; x < WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x - 15;
    CRGB color = CHSV(
                   HUE_ADD + pgm_read_byte(&(hueMask[0][newX])), // H
                   255,           // S
                   (uint8_t)(((100.0 - pcnt) * matrixValue[0][newX] + pcnt * line[newX]) / 100.0) // V
                 );
    setPixel(newX, HEIGHT - 1, color);
  }
}

//shift all values in the matrix up one row
void shiftUp() {
  for (uint8_t y = HEIGHT - 1; y > 0; y--) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x - 15;
      if (y > 7) continue;
      matrixValue[y][newX] = matrixValue[y - 1][newX];
    }
  }

  for (uint8_t x = 0; x < WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x - 15;
    matrixValue[0][newX] = line[newX];
  }
}

// Randomly generate the next line (matrix row)
void generateLine() {
  for (uint8_t x = 0; x < WIDTH; x++) {
    line[x] = random(64, 255);
  }
}

/* Эффект огня */
void fireRoutine() {
  static unsigned long timer = 0;
  if (millis() - timer < 10) {
    return;
  }
  timer = millis();
  
  if (pcnt >= 100) {
    shiftUp();
    generateLine();
    pcnt = 0;
  }
  drawFrame(pcnt);
  pcnt += 10;
}
