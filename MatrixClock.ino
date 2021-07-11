
#include "FS.h"
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "main.h"
#include "time.h"
#include "web_server.h"
#include "display.h"

IPAddress AP_localIP(192, 168, 1, 1);     //Локальный IP адрес устройства в режиме точки доступа
IPAddress AP_gateway(192, 168, 1, 1);     //Шлюз
IPAddress AP_subnet(255, 255, 255, 0);    //Маска подсети

void setup() {
  Serial.begin(115200);
  Serial.println("\n");

  SPIFFS.begin();   //Инитилизируем файловую систему
  loadConfig();     //Загружаем конфигурационный файл
  initDisplay();    //Инитилизируем компонент экрана

  //Переключаем WiFi в режим клиента и подключаемся к сети
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPass);
  Serial.print(F("Connecting to "));
  Serial.println(wifiSSID);

  initMain();                 //Инитилизируем основной компонент
  initTime();                 //Инитилизируем компонент для синхронизации времени
  initWebServer();            //Инитилизируем веб-сервер

  randomSeed(ESP.random());

  //Инитилизируем обновление по воздуху
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
}

void loop() {
  //Подключены ли в данный момент к WiFi
  static bool isWiFiConnected = false;

  //Если слишком долго не можем подключиться к сети
  if (!isWiFiConnected && WiFi.status() != WL_CONNECTED && millis() > MAX_CONNECTION_TIME) {
    isWiFiConnected = true;

    Serial.print(F("Unable connect to "));
    Serial.print(wifiSSID);
    Serial.print(F(". Creating hot spot "));
    Serial.println(AP_SSID);

    //Создаем собственную сеть
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    WiFi.softAPConfig(AP_localIP, AP_gateway, AP_subnet);
    
    //Запускаем обновление по воздуху
    ArduinoOTA.begin();

    Serial.print("Local IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.println();
  }
  //Если только что подключились
  if (!isWiFiConnected && WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;

    Serial.print(F("Done, IP address: "));
    Serial.println(WiFi.localIP());
    Serial.println();

    ArduinoOTA.begin();   //Запускаем обновление по воздуху
    syncTime();           //Принудительно синхронизируем время
  }

  ArduinoOTA.handle();
  updateTime();       //Обновляем время
  updateWebServer();  //Обновляем веб-сервер
  updateMain();
}
