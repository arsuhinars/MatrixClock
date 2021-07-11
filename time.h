#pragma once

extern uint8_t timeZone;            //Временная зона
extern bool useNTP;                 //Использовать NTP для синхронизации времени
extern String ntpServerAddr;        //Адрес NTP сервера

/* Функция инитилизации компонента синхронизации времени */
void initTime();

/* Функция обновления времени(в нужный момент синхронизируется с NTP сервером) */
void updateTime();

/* Функция немедленой синхронизации с сервером */
void syncTime();

/* Функция изменения текущего времени */
void setTime(unsigned long time);

/* Функция получения текущего времени */
unsigned long getTime();

/* Получить час из UTC времени */
uint8_t getHour(unsigned long time);

/* Получить минуту из UTC времени */
uint8_t getMinute(unsigned long time);

/* Получить секунды из UTC времени */
uint8_t getSecond(unsigned long time);
