/*Includes ------------------------------------------------------------------*/
#include <stdlib.h>

#include <WiFi.h>
#include <ArduinoHA.h>

#include "Epaper_digits.h"
#include "Temp_Hum_Sensor.h"

#define SLEEP_SECONDS 60
#define HUM_SHOW_SECONDS 2
#define USE_DISPLAY true
#define SEND_DATA_WITH_MQTT true
#define DEVICE_NAME "esp32-dev-epaper"
#define BATTERY_LEVEL_PIN 36

// WiFi
const char *my_ssid = "**REPLACE**";             // Enter your Wi-Fi name
const char *my_password = "**REPLACE**";  // Enter Wi-Fi password

// MQTT Broker
const char *my_mqtt_broker = "homeassistant.local";
const char *my_mqtt_username = "**REPLACE**";
const char *my_mqtt_password = "**REPLACE**";

WiFiClient wifiClient;
HADevice device(DEVICE_NAME);
HAMqtt mqtt(wifiClient, device);
HASensorNumber tempSensor("epaper-temperature", HASensorNumber::PrecisionP2);
HASensorNumber humSensor("epaper-humidity", HASensorNumber::PrecisionP1);
HASensorNumber batterySensorPercent("epaper-battery-level", HASensorNumber::PrecisionP0);
HASensorNumber batterySensorVolt("epaper-battery-volt", HASensorNumber::PrecisionP2);

const float BATTERY_VOLTAGE_MULTIPLIER = 2.75;
const float BATTERY_LEVEL_100_PERCENT = 4.15;
const float BATTERY_LEVEL_0_PERCENT = 3.00;

int uS_TO_S_FACTOR = 1000000;
RTC_DATA_ATTR float temp, hum;
EpaperBigDigits epaperBigDigits;
Temp_Hum_Sensor temp_Hum_Sensor;

void mqtt_setup() {
  device.setName(DEVICE_NAME);
  device.setSoftwareVersion("1.0.0");

  tempSensor.setIcon("mdi:thermometer");
  tempSensor.setName("Temp-mqtt");
  tempSensor.setUnitOfMeasurement("°C");

  humSensor.setIcon("mdi:water-percent");
  humSensor.setName("Hum-mqtt");
  humSensor.setUnitOfMeasurement("%");

  batterySensorPercent.setIcon("mdi:battery");
  batterySensorPercent.setName("Battery-Level-mqtt");
  batterySensorPercent.setUnitOfMeasurement("%");

  batterySensorVolt.setIcon("mdi:battery");
  batterySensorVolt.setName("Battery-Volt-mqtt");
  batterySensorVolt.setUnitOfMeasurement("V");
  mqtt.begin(my_mqtt_broker, my_mqtt_username, my_mqtt_password);
}

boolean wifi_reconnect() {
  if(WiFi.isConnected()){
    return true;
  }
  int connectionAttempts = 3;
  while (WiFi.status() != WL_CONNECTED && connectionAttempts >= 0) {
    Serial.println("Connecting to WiFi..");
    WiFi.begin(my_ssid, my_password);
    if (WiFi.isConnected()) {
      return true;
    }
    connectionAttempts--;
    delay(500);
  }
  return WiFi.isConnected();
}

int8_t get_battery_level_percent(float battery_level){
  if (battery_level > BATTERY_LEVEL_100_PERCENT){
    battery_level = BATTERY_LEVEL_100_PERCENT;
  }

  float battery_level_rel = battery_level - BATTERY_LEVEL_0_PERCENT;
  float divider = (BATTERY_LEVEL_100_PERCENT - BATTERY_LEVEL_0_PERCENT) / 100;
  int battery_level_perc = (int)(battery_level_rel / divider);

  Serial.print("Battery level %: ");
  Serial.println(battery_level_perc);
  return battery_level_perc;
}

void tempAndHum() {
  printf("show temp and hum \r\n");
  temp_Hum_Sensor.read_temp_hum(&temp, &hum);
  float battery_level_corrected = analogRead(BATTERY_LEVEL_PIN) * BATTERY_VOLTAGE_MULTIPLIER;
  float battery_vol = battery_level_corrected / 1000;
  int8_t battery_level_percent = get_battery_level_percent(battery_vol);
  if (USE_DISPLAY) {
    int x = 0;
    int y = 20;
    epaperBigDigits.clear_display();
    epaperBigDigits.drawFloat(x, y, hum, HUMIDITY);
    delay(HUM_SHOW_SECONDS * 1000);
    epaperBigDigits.drawFloat(x, y, temp, TEMPERATURE);
  }
  if (SEND_DATA_WITH_MQTT) {
    wifi_reconnect();
    mqtt.loop();
    tempSensor.setValue(temp);
    humSensor.setValue(hum);
    batterySensorPercent.setValue(battery_level_percent);
    batterySensorVolt.setValue(battery_vol);
  }
}

/* Entry point ----------------------------------------------------------------*/
void setup() {
  esp_sleep_enable_timer_wakeup(SLEEP_SECONDS * uS_TO_S_FACTOR);
  temp_Hum_Sensor.init();
  epaperBigDigits.digits_init();
  mqtt_setup();
}

/* The main loop -------------------------------------------------------------*/
void loop() {
  delay(200);
  tempAndHum();
  delay(200);
  esp_light_sleep_start();
}
