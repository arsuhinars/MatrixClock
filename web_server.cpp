
#include <ESP8266WebServer.h>
#include "FS.h"

#include "main.h"
#include "effects_mode.h"
#include "clock.h"
#include "text.h"
#include "snake.h"
#include "runner.h"
#include "time.h"
#include "display.h"
#include "config.h"
#include "web_server.h"

//Номера изменяемых через сеть параметров
#define VAR_CLOCK_DIGITS_COLOR  0
#define VAR_AUTO_CHANGE_EFFECTS 1
#define VAR_CLOCK_ENABLE_BG     2
#define VAR_CLOCK_BG            3
#define VAR_AUTOBRIGHTNESS      4
#define VAR_BRIGHTNESS          5
#define VAR_WIFI_SSID           6
#define VAR_WIFI_PASS           7
#define VAR_TIME_ZONE           8
#define VAR_USE_NTP             9
#define VAR_NTP_ADDR            10
#define VAR_TIME                11
#define VAR_SNAKE_SPEED         12
#define VAR_SNAKE_BARRIER       13
#define VAR_RUNNING_LINE_TEXT   14
#define VAR_RUNNING_LINE_BG     15
#define VAR_RUNNING_LINE_SPEED  16

//HTTP сервер
ESP8266WebServer httpServer(80);

//WebSocket сервер
WebSocketsServer webSocket(81);

/* Функция отправки сообщения пользователю */
void sendMessage(uint8_t* data, size_t length) {
  webSocket.broadcastBIN(data, length);
}

/* Функция отправки события */
void sendEventMessage(uint8_t eventType) {
  uint8_t data[] = { CMD_SEND_EVENT, eventType };
  webSocket.broadcastBIN(data, sizeof(data));
}

/* Функция отправки отладочного сообщения */
void sendLog(String message) {
  size_t length = message.length() + 1;
  uint8_t* data = new uint8_t[length];
  data[0] = CMD_LOG;
  for (size_t i = 1; i < length; i++) {
    data[i] = message[i - 1];
  }
  webSocket.broadcastBIN(data, length);
  delete[] data;
}

/* Функция обработки команды получения переменной */
void handleGetVar(uint8_t* data, size_t length) {
  uint8_t* data_ = nullptr;
  size_t length_ = 0;
  
  switch (data[1]) {
    case VAR_CLOCK_DIGITS_COLOR: {
      data_ = new uint8_t[5];
      length_ = 5;
      data_[2] = clockDigitsColor.r;
      data_[3] = clockDigitsColor.g;
      data_[4] = clockDigitsColor.b;
      break;
    }
    case VAR_AUTO_CHANGE_EFFECTS: {
      data_ = new uint8_t[3];
      length_ = 3;
      data_[2] = effectAutoChangeFlag;
      break;
    }
    case VAR_CLOCK_ENABLE_BG: {
      data_ = new uint8_t[3];
      length_ = 3;
      data_[2] = clockEnableBackgrounds;
      break;
    }
    case VAR_CLOCK_BG: {
      data_ = new uint8_t[3];
      length_ = 3;
      data_[2] = clockBackground;
      break;
    }
    case VAR_AUTOBRIGHTNESS: {
      data_ = new uint8_t[3];
      length_ = 3;
      data_[2] = autoBrightness;
      break;
    }
    case VAR_BRIGHTNESS: {
      data_ = new uint8_t[3];
      length_ = 3;
      data_[2] = matrixBrightness;
      break;
    }
    case VAR_WIFI_SSID: {
      length_ = wifiSSID.length() + 2;
      data_ = new uint8_t[length_];
      for (int i = 2; i < length_; i++)
        data_[i] = wifiSSID[i - 2];
      break;
    }
    case VAR_WIFI_PASS: {
      length_ = wifiPass.length() + 2;
      data_ = new uint8_t[length_];
      for (int i = 2; i < length_; i++)
        data_[i] = wifiPass[i - 2];
      break;
    }
    case VAR_TIME_ZONE: {
      data_ = new uint8_t[3];
      length_ = 3;
      data_[2] = timeZone;
      break;
    }
    case VAR_USE_NTP: {
      data_ = new uint8_t[3];
      length_ = 3;
      data_[2] = useNTP;
      break;
    }
    case VAR_NTP_ADDR: {
      length_ = ntpServerAddr.length() + 2;
      data_ = new uint8_t[length_];
      for (int i = 2; i < length_; i++)
        data_[i] = ntpServerAddr[i - 2];
      break;
    }
    case VAR_TIME: {
      data_ = new uint8_t[6];
      length_ = 6;

      auto currentTime = getTime();
      data_[2] = currentTime >> 24;
      data_[3] = currentTime >> 16;
      data_[4] = currentTime >> 8;
      data_[5] = currentTime;
      break;
    }
    case VAR_SNAKE_SPEED: {
      data_ = new uint8_t[4];
      length_ = 4;
      data_[2] = snakeSpeed >> 8;
      data_[3] = snakeSpeed;
      break;
    }
    case VAR_SNAKE_BARRIER: {
      data_ = new uint8_t[3];
      length_ = 3;
      data_[2] = snakeBarrierEnabled;
      break;
    }
    case VAR_RUNNING_LINE_TEXT: {
      length_ = runningLineText.length() + 2;
      data_ = new uint8_t[length_];
      for (uint8_t i = 2; i < length_; i++)
        data_[i] = runningLineText[i - 2];
      break;
    }
    case VAR_RUNNING_LINE_BG: {
      data_ = new uint8_t[3];
      length_ = 3;
      data_[2] = runningLineBg;
      break;
    }
    case VAR_RUNNING_LINE_SPEED: {
      data_ = new uint8_t[3];
      length_ = 3;
      data[2] = runningLineSpeed;
      break;
    }
  }
  data_[0] = CMD_GET_VAR;
  data_[1] = data[1];
  webSocket.broadcastBIN(data_, length_);
  delete[] data_;
}

/* Функция обработки команды изменения переменной */
void handleSetVar(uint8_t* data, size_t length) {
  switch (data[1]) {
    case VAR_CLOCK_DIGITS_COLOR: clockDigitsColor = CRGB(data[2], data[3], data[4]); break;
    case VAR_AUTO_CHANGE_EFFECTS: effectAutoChangeFlag = data[2]; break;
    case VAR_CLOCK_ENABLE_BG: clockEnableBackgrounds = data[2]; break;
    case VAR_CLOCK_BG:  clockBackground = data[2]; break;
    case VAR_AUTOBRIGHTNESS: autoBrightness = data[2]; break;
    case VAR_BRIGHTNESS: matrixBrightness = data[2]; break;
    case VAR_WIFI_SSID: {
      char string[length - 1];
      for (int i = 2; i < length; i++)
        string[i - 2] = data[i];
      string[length - 2] = '\0';
      wifiSSID = string;
      break;
    }
    case VAR_WIFI_PASS: {
      char string[length - 1];
      for (int i = 2; i < length; i++)
        string[i - 2] = data[i];
      string[length - 2] = '\0';
      wifiPass = string;
      break;
    }
    case VAR_TIME_ZONE: timeZone = data[2]; break;
    case VAR_USE_NTP: useNTP = data[2]; break;
    case VAR_NTP_ADDR: {
      char string[length - 1];
      for (int i = 2; i < length; i++)
        string[i - 2] = data[i];
      string[length - 2] = '\0';
      ntpServerAddr = string;
      break;
    }
    case VAR_TIME: {
      unsigned long time = data[2] << 24;
      time |= data[3] << 16;
      time |= data[4] << 8;
      time |= data[5];
      setTime(time);
      break;
    }
    case VAR_SNAKE_SPEED:
      snakeSpeed = data[2] << 8;
      snakeSpeed |= data[3];
      break;
    case VAR_SNAKE_BARRIER: snakeBarrierEnabled = data[2]; break;
    case VAR_RUNNING_LINE_TEXT:
      char string[length - 1];
      for (int i = 2; i < length; i++)
        string[i - 2] = data[i];
      string[length - 2] = '\0';
      runningLineText = string;
      break;
    case VAR_RUNNING_LINE_BG: runningLineBg = data[2]; break;
    case VAR_RUNNING_LINE_SPEED: runningLineSpeed = data[2]; break;
  }
}

/* Функция обработки входящей команды */
void handleCommand(uint8_t* data, size_t length) {
  //Обрабатываем команду
  switch (data[0]) {
    //Команда изменения основного режима
    case CMD_CHANGE_MODE: changeMode(data[1]); break;
    //Команда изменения подрежима
    case CMD_CHANGE_SUBMODE:  {
      switch (getCurrentMode()) {
        case MODE_CLOCK: changeClockMode(data[1]); break;
        case MODE_EFFECTS: changeEffect(data[1]); break;
      }
      break;
    }
    //Команда получения параметра
    case CMD_GET_VAR: handleGetVar(data, length); break;
    //Команда изменения параметра
    case CMD_SET_VAR: handleSetVar(data, length); break;
    //Команда сохранения настроек
    case CMD_SAVE_SETTINGS: {
      saveConfig();
      uint8_t data[] = {CMD_SAVE_SETTINGS};
      webSocket.broadcastBIN(data, sizeof(data));
      break;
    }
    //Команда отправки события
    case CMD_SEND_EVENT: {
      switch (getCurrentMode()) {
        case MODE_GAME_SNAKE: handleSnakeEvent(data[1]); break;
        case MODE_GAME_RUNNER: handleRunnerEvent(data[1]); break;
      }
      break;
    }
  }
}

/* Получить тип контента по имени файла */
String getContentType(String filename) {
  if (filename.endsWith(".htm"))
    return "text/html";
  else if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".xml"))
    return "text/xml";
  else if (filename.endsWith(".pdf"))
    return "application/x-pdf";
  else if (filename.endsWith(".zip"))
    return "application/x-zip";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

/* Добавить страницу обработчик файлов */
void addFilePage(String filename, String route="") {
  if (!SPIFFS.exists(filename)) {
    Serial.print(F("Error! Can't add file page for "));
    Serial.print(filename);
    Serial.println(F(" because it doesn't exist"));
    return;
  }

  if (route.length() == 0)
    route = filename;

  Serial.print(F("Added file page for "));
  Serial.println(filename);
  httpServer.on(route, [filename]() {
    Serial.print(F("Request for "));
    Serial.print(filename);
    Serial.print(F(" from "));
    Serial.println(httpServer.client().remoteIP().toString());

    if (SPIFFS.exists(filename)) {
      File file = SPIFFS.open(filename, "r");
      httpServer.streamFile(file, getContentType(filename));
      file.close();
      return;
    }

    httpServer.send(404, "text/plain", "File not found");

    Serial.println(F("Error! This file doesn't exist"));
  });
}

/* Функция обработчик событий от WebSocket сервера */
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  IPAddress ip = webSocket.remoteIP(num);
  switch (type) {
    //Событие подключения
    case WStype_CONNECTED:
      Serial.print(F("Connection from "));
      Serial.print(ip.toString());
      Serial.print("[");
      Serial.print(num);
      Serial.println("]");

      //Отправляем пользователю данные о текущих режимах
      sendModeData();
      sendClockData();
      sendEffectModeData();
      break;
    //Событие отключения
    case WStype_DISCONNECTED:
      Serial.print("Client ");
      Serial.print(ip.toString());
      Serial.print("[");
      Serial.print(num);
      Serial.print("] ");
      Serial.println(F(" disconnected"));
      break;
    //Событие получения бинарного сообщения
    case WStype_BIN: handleCommand(payload, length); break;
  }
}

/* Функция инитилизации веб-сервера */
void initWebServer() {
  //Инитилизируем HTTP сервер
  httpServer.begin();
  addFilePage(F("/index.html"), "/");
  addFilePage(F("/style.css"));
  addFilePage(F("/main.js"));
  addFilePage(F("/utils.js"));
  addFilePage(F("/snake.js"));
  addFilePage(F("/runner.js"));
  addFilePage(F("/spectre.min.css"));
  addFilePage(F("/spectre-exp.min.css"));
  addFilePage(F("/icons.min.css"));

  //Инитилизируем WebSocket сервер
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  webSocket.enableHeartbeat(5000, 5000, 1);

  Serial.println(F("Web socket and HTTP servers has been started!"));
}

/* Функция обновления веб-сервера. Должна вызыватся в loop() */
void updateWebServer() {
  httpServer.handleClient();
  webSocket.loop();
}
