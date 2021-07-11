#pragma once

/* ================ Настройки ================ */

#define WIDTH                   16                  //Кол-во пикселей по горизонтали
#define HEIGHT                  8                   //Кол-во пикселей по вертикали
#define LED_TYPE                WS2812              //Тип светодиодов в матрице
#define MATRIX_PIN              5                   //Пин, к которому подключена матрица
#define MODE_BUTTON_PIN         D1                  //Пин для кнопки смены основного режима
#define SUBMODE_BUTTON_PIN      D8                  //Пин для кнопки смены подрежима

#define LIGHT_SENSOR_PIN        A0                  //Пин, к которому подключен датчик освещенности
#define MIN_BRIGHTNESS          0                   //Минимальная яркость
#define MAX_BRIGHTNESS          20                  //Максимальная яркость
#define MIN_LIGHT_VALUE         70                 //Минимальное значение датчика
#define MAX_LIGHT_VALUE         800                 //Максимальное значение датчика
#define BRIGHTNESS_TIME         1000                //Время обновления и смены яркости

#define CONFIG_FILE_NAME        "/config.json"      //Имя конфигурационного файла

#define AP_SSID                 "MatrixClock OS"    //Имя WiFi сети, которая будет создана, при невозможности подключится к существующей
#define AP_PASS                 "12345678"          //Пароль для этой сети
#define MAX_CONNECTION_TIME     10000               //Максимальное время подключения к WiFi
#define WIFI_SSID               "SSID"              //Название сети WiFi, к которой будет подключатся контроллер
#define WIFI_PASS               "PASS"              //Пароль от этой сети

#define TIME_ZONE               3                   //Временная зона (в часах)
#define USE_NTP                 1                   //Использовать ли NTP для синхронизации времени
#define NTP_SERVER_ADDR         "ru.pool.ntp.org"   //Адрес NTP сервера
#define RESYNC_TIME             600000              //Период синхронизации часов (в миллисекундах)
#define MAX_WAIT_RESPONSE_TIME  500                 //Максимальное время ожидания ответа от сервера

#define OTA_HOSTNAME            "MatrixClock"       //Название устройства в сети
#define OTA_PASSWORD            ""                  //Пароль к устройству для обновления по воздуху

/* =========================================== */

/* Функция загрузки конфига */
void loadConfig();

/* Функция сохранения конфига */
void saveConfig();
