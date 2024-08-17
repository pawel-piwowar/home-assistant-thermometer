#include "Temp_Hum_Sensor.h"
#include "Seeed_SHT35.h"

#define SERIAL Serial

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SDAPIN 20
#define SCLPIN 21
#define RSTPIN 7
#define SERIAL SerialUSB
#else
#define SDAPIN A4
#define SCLPIN A5
#define RSTPIN 2
#define SERIAL Serial
#endif

SHT35 sensor(SCLPIN);

boolean Temp_Hum_Sensor::init() {
  if (sensor.init()) {
    printf("sensor init failed!!!");
    return false;
  }
  return true;
}

boolean Temp_Hum_Sensor::read_temp_hum(float *temp, float *hum) {
  boolean result = sensor.read_meas_data_single_shot(HIGH_REP_WITH_STRCH, temp, hum);

  SERIAL.print("Temp : ");
  SERIAL.println(*temp);

  SERIAL.print("Hum : ");
  SERIAL.println(*hum);
  return result;
}