#include <Wire.h>
#include <Flydurino.h>
#include "sensor.h"
#include <stdint.h>
#include <arduino.h>

/***********************************************/
class FlySensor : public Sensor {
  public:
    FlySensor();
    //virtual void getMeasurement(void* value);
  protected:    
    Flydurino *flydurino;
};


/***********************************************/
class FlySensorAcc : public FlySensor {
  public:
    FlySensorAcc() : FlySensor() {};
    void getMeasurement(void* value);  
  private:
    float convertACC(int16_t v1, int16_t v2);
};


/***********************************************/
class FlySensorRot : public FlySensor {
  public:
    FlySensorRot();
    void getMeasurement(void* value);  
    void reset();
  private:
    void configure();
  
    float mOffset[3];
    float mRotation[3];
    unsigned long mTime;
    
};
