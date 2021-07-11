
#include "FS.h"

#include <ArduinoJson.h>

#include "main.h"
#include "display.h"
#include "time.h"
#include "clock.h"
#include "effects_mode.h"
#include "text.h"
#include "config.h"

/* Функция загрузки конфига */
void loadConfig() {
  //Если файл конфига не существует
  if (!SPIFFS.exists(CONFIG_FILE_NAME))
    return;
    
  //Открываем файл и загружаем оттуда конфиг
  File file = SPIFFS.open(CONFIG_FILE_NAME, "r");
  if (!file)  //Если произошла ошибка
    return;

  //Читаем буфер
  char* buff = new char[file.size()];
  file.readBytes(buff, file.size());

  //Парсим JSON конфиг
  StaticJsonDocument<256> json;
  auto error = deserializeMsgPack(json, buff);

  //Закрываем файл и освобождаем память
  file.close();
  delete[] buff;
  
  //Если произошла ошибка при парсинге файла
  if (error) {
    Serial.print(F("Error while reading config: "));
    Serial.println(error.c_str());
    return;
  }

  //Загружаем все значения
  if (json["mode"].is<uint8_t>())
    changeMode(json["mode"]);
  if (json["clockMode"].is<uint8_t>())
    changeClockMode(json["clockMode"]);
  if (json["effectMode"].is<uint8_t>())
    changeEffect(json["effectMode"]);
  matrixBrightness = json["brightness"] | matrixBrightness;
  autoBrightness = json["autoBrightness"] | autoBrightness;
  if (json["clockDigitsColor"].is<uint32_t>())
    clockDigitsColor = json["clockDigitsColor"].as<uint32_t>();
  clockEnableBackgrounds = json["clockEnableBgs"] | clockEnableBackgrounds;
  clockBackground = json["clockBackground"] | clockBackground;
  runningLineText = json["runningLineText"] | runningLineText;
  runningLineBg = json["runningLineBg"] | runningLineBg;
  runningLineSpeed = json["runningLineSpeed"] | runningLineSpeed;
  wifiSSID = json["ssid"] | wifiSSID;
  wifiPass = json["pass"] | wifiPass;
  timeZone = json["timeZone"] | timeZone;
  useNTP = json["useNTP"] | useNTP;
  ntpServerAddr = json["ntpServerAddr"] | ntpServerAddr;
}

/* Функция сохранения конфига */
void saveConfig() {
  //Создаем JSON документ и записываем в него значения
  StaticJsonDocument<256> json;
  json["mode"] = getCurrentMode();
  json["clockMode"] = getClockMode();
  json["effectMode"] = getCurrentEffect();
  json["brightness"] = matrixBrightness;
  json["autoBrightness"] = autoBrightness;
  json["clockDigitsColor"] = (uint32_t)(clockDigitsColor.r << 16 | clockDigitsColor.g << 8 | clockDigitsColor.b);
  json["clockEnableBgs"] = clockEnableBackgrounds;
  json["clockBackground"] = clockBackground;
  json["runningLineText"] = runningLineText;
  json["runningLineBg"] = runningLineBg;
  json["runningLineSpeed"] = runningLineSpeed;
  json["ssid"] = wifiSSID;
  json["pass"] = wifiPass;
  json["timeZone"] = timeZone;
  json["useNTP"] = useNTP;
  json["ntpServerAddr"] = ntpServerAddr;

  //Открываем файл для записи
  File file = SPIFFS.open(CONFIG_FILE_NAME, "w");
  if (!file) {
    Serial.println(F("Error! Failed to open config file to writing"));
    return;
  }

  //Сериализуем JSON документ и записываем результат в файл
  serializeMsgPack(json, file);
  file.close();
}
