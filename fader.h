#pragma once

/* Функция для плавного затухания яркости */
void fadeOut(unsigned long time);

/* Функция плавного нарастания яркости */
void fadeIn(unsigned long time);

/* 
 *  Функция обновления затухания/нарастания. Необходимо постоянно опрашивать.
 *  Возвращает истинну, когда анимация закончится.
*/
bool fadeStep();