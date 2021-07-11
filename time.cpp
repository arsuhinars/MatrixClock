
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "config.h"
#include "time.h"

//Объект UDP соединения
WiFiUDP udp;

unsigned long currentTime = 0;            //Текущее время с 1 января 1970 года (Unix время) с учетом временной зоны
uint8_t timeZone = TIME_ZONE;             //Текущая временная зона
bool useNTP = USE_NTP;                    //Использовать NTP для синхронизации времени
String ntpServerAddr = NTP_SERVER_ADDR;   //Адрес NTP сервера

//Время последней синхронизации (Unix время)
unsigned long lastSyncTime = 0;
//Время последней синхронизации времени (с момента запуска устройства)
unsigned long lastSyncMillis = 0;

/* Функция инитилизации компонента синхронизации времени */
void initTime() {
  //Открываем NTP порт для прослушивания ответа от сервера
  udp.begin(123);
  
  //Сразу же синхронизируем время
  //syncTime();
}

/* Функция обновления времени(в нужный момент синхронизируется с NTP сервером) */
void updateTime() {
  //Время прошедшее с прошлой синхронизации часов
  unsigned long passedTime = millis() - lastSyncMillis;
  
  //Проверяем, прошло ли достаточно времени с прошлой синхронизации
  if (useNTP && passedTime >= RESYNC_TIME) {
    syncTime();
  } else {
    //Изменяем текущее время в соотвествии со временем, прошедшим с прошлой синхронизации
    currentTime = lastSyncTime + (passedTime / 1000);
  }
}

/* Функция немедленой синхронизации с сервером */
void syncTime() {
  static const uint16_t NTP_PACKET_SIZE = 48;  //Размер NTP пакета
  IPAddress ntpServerIp;                  //IP адрес NTP сервера

  //Получаем IP адрес сервера
  WiFi.hostByName(ntpServerAddr.c_str(), ntpServerIp);

  //Буфер, хранящий текущий пакет
  byte packet[NTP_PACKET_SIZE] = {0};

  //Формируем запрос
  packet[0] = 0b00100011; //Индикатор коррекции, номер версии и режим
  //Оставляем остальные байты по нулям

  //Очищаем буфер от предыдущих ответов на наши запросы
  while (udp.parsePacket() > 0) {
    udp.flush();
  }

  //Отправляем пакет
  udp.beginPacket(ntpServerIp, 123);
  udp.write(packet, NTP_PACKET_SIZE);
  udp.endPacket();

  //Ждем ответа
  unsigned long waitBeginTime = millis(); //Время начала ожидания
  while (millis() - waitBeginTime < MAX_WAIT_RESPONSE_TIME) {
    int size = udp.parsePacket(); //Получаем размер полученного пакета
    if (size >= NTP_PACKET_SIZE) { //Если был получен пакет с допустимым размером
      udp.read(packet, NTP_PACKET_SIZE);  //Получаем пакет
      currentTime = (unsigned long)packet[40] << 24;
      currentTime |= (unsigned long)packet[41] << 16;
      currentTime |= (unsigned long)packet[42] << 8;
      currentTime |= (unsigned long)packet[43];
      currentTime -= 2208988800UL;
      currentTime += timeZone * 3600UL;
      
      setTime(currentTime);

      Serial.println(F("NTP response was received!"));
      Serial.print(F("Current time: "));
      Serial.print(getHour(currentTime));
      Serial.print(":");
      Serial.print(getMinute(currentTime));
      Serial.print(":");
      Serial.println(getSecond(currentTime));
      Serial.println();
      return;
    }
  }

  setTime(currentTime);
  
  Serial.println(F("Error! There is no NTP response"));
  Serial.println();
}

/* Функция изменения текущего времени */
void setTime(unsigned long time) {
  lastSyncTime = time;
  lastSyncMillis = millis();
}

/* Функция получения текущего времени */
unsigned long getTime() {
  return currentTime;
}

/* Получить час из UTC времени */
uint8_t getHour(unsigned long time) {
  return time % 86400 / 3600;
}

/* Получить минуту из UTC времени */
uint8_t getMinute(unsigned long time) {
  return time % 3600 / 60;
}

/* Получить секунды из UTC времени */
uint8_t getSecond(unsigned long time) {
  return time % 60;
}
