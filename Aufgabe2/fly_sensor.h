#include <Wire.h>
#include <Flydurino.h>
#include "sensor.h"
#include <stdint.h>
#include <arduino.h>

class FlySensor : public Sensor {
  public:
    FlySensor();
  protected:    
    Flydurino *flydurino;
};


class FlySensorAcc : public FlySensor {
  public:
    FlySensorAcc() : FlySensor() {};
    void getMeasurement(void* value);  
  private:
    float convertACC(int16_t v1, int16_t v2);
};
