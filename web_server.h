#pragma once

//Типы команд
#define CMD_CHANGE_MODE       0
#define CMD_CHANGE_SUBMODE    1
#define CMD_GET_VAR           2
#define CMD_SET_VAR           3
#define CMD_SAVE_SETTINGS     4
#define CMD_SEND_EVENT        5
#define CMD_LOG               6

#include <WebSocketsServer.h>

/* Функция отправки сообщения пользователю */
void sendMessage(uint8_t* data, size_t length);

/* Функция отправки события */
void sendEventMessage(uint8_t eventType);

/* Функция отправки отладочного сообщения */
void sendLog(String message);

/* Функция инитилизации веб-сервера */
void initWebServer();

/* Функция обновления веб-сервера. Должна вызыватся в loop() */
void updateWebServer();
