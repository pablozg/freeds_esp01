/*
  FreeDS_ESP01.ino - Firmware for Solax V2/3 connection
   
  Copyright (C) 2020 Pablo Zerón (https://github.com/pablozg/freeds)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

struct CONFIG
{
  boolean hasSSID;

  // DATOS SOLAX
  char ssid_esp01[30] = {};
  char password_esp01[30] = {};
  uint8_t version = 2;
} config;

uint32_t readTimer;

struct INVERTER
{
  float pv1c;              // Corriente string 1
  float pv2c;              // Corriente string 2
  float pv1v;              // Tension string 1
  float pv2v;              // Tension string 2
  float pw1;               // Potencia string 1
  float pw2;               // Potencia string 2
  float gridv;             // Tension de red
  float wsolar;            // Potencia solar
  float wtoday;            // Potencia solar diaria
  float wgrid;             // Potencia de red (Negativo: de red - Positivo: a red)
  float wtogrid;           // Potencia enviada a red
  float temperature;       // Temperatura Inversor
} inverter;

//// Definiciones Conexiones

ESP8266WiFiMulti WiFiMulti;

DynamicJsonDocument root(4096);

/////////////////////////////////////

void setup()
{
  Serial.begin(115200);

  ////////////////////// WATCHDOG //////////////////
  ESP.wdtDisable();
  /////////////////////////////////////////////////

  config.hasSSID = false;
  EEPROM.begin(sizeof(config));
  EEPROM.get(0, config);

  WiFi.mode(WIFI_STA);

  if (config.hasSSID) {
    WiFiMulti.addAP(config.ssid_esp01, "");
    Serial.printf("Connecting to SSID: %s\n", config.ssid_esp01);
  } else {
    Serial.printf("No SSID stored\n");
  }
  readTimer = millis();
}

void loop()
{
  //////// Watchdog ////////////
  ESP.wdtFeed();
  /////////////////////////////////////

  if (Serial.available()) {
    String Data;
    Data = Serial.readStringUntil('\n');

    if (Data.startsWith("SSID: ")){
      String SSID = Data.substring(Data.indexOf("SSID: ") + 6, Data.length());
      strcpy(config.ssid_esp01, SSID.c_str());
      config.hasSSID = true;
      Serial.printf("SSID received: %s\n", config.ssid_esp01);
      EEPROM.put(0, config);
      EEPROM.commit();
      WiFiMulti.addAP(config.ssid_esp01, "");
    } 

    if (Data.startsWith("DATAVERSION: ")){
      config.version = atoi(Data.substring(Data.indexOf("DATAVERSION: ") + 13, Data.length()).c_str());
      Serial.printf("Data version received: %d\n", config.version);
      EEPROM.put(0, config);
      EEPROM.commit();
    } 
  }

  if (WiFiMulti.run() == WL_CONNECTED)
  {    
    if (millis() - readTimer > 1500) {
      readTimer = millis();
      requestSolaxData();
    }
  } else {
    if (millis() - readTimer > 5000) {
      readTimer = millis();
      Serial.printf("Wifi not connected or lost\n");
    }
  }
} // End loop
