#pragma once

//Количество эффектов
#define EFFECTS_AMOUNT 14

//Список всех эффектов, которые могут быть использованы, как фон
const uint8_t backgroundEffects[] = {0, 6, 7, 8, 9, 10};

/* Функция отрисовки эффекта по его номеру */
void drawEffect(uint8_t effect);

void drawRunningRainbow();  /* Эффект бегущей радуги */
void drawMatrix();          /* Эффект матрицы */
void drawSnowfall();        /* Эффект падающего снега */
void drawSparkles();        /* Эффект конфети */
void drawStarfall();        /* Эффект звездопада */
void drawEqualizer();       /* Эффект эквалайзера */
void cloudNoise();          /* Эффект облачного шума */
void lavaNoise();           /* Эффект лавового шума */
void rainbowNoise();        /* Эффект радужного шума */
void plasmaNoise();         /* Эффект плазмы */
void oceanNoise();          /* Эффект шума океана */
void fireRoutine();         /* Эффект огня */
void infoTextRoutine();     /* Эффект с текстом с полезной информацией */
