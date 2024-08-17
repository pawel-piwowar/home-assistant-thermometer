#ifndef PePe_Temp_Hum_Sensor
#define PePe_Temp_Hum_Sensor

#include <Arduino.h>

class Temp_Hum_Sensor {
  private:
  public:
    boolean init();
    boolean read_temp_hum(float *temp, float *hum);
};

#endif