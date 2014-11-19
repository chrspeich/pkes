#include "sensor.h"
#include <stdint.h>

class IrSensor : public Sensor {
  public:
    IrSensor(uint8_t channel);
    void getMeasurement(void* value);
  private:
    void configure();
    
    uint8_t mChannel;
    double mean[5];
};

