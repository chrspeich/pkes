#include "ir_sensor.h"
#include <math.h>
#include <Arduino.h>
#include "util.h"

IrSensor::IrSensor(uint8_t channel) {
  mChannel = channel;
}

void IrSensor::configure(){
  analogReference(INTERNAL2V56);
}

void IrSensor::getMeasurement(void *value){
   int raw = analogRead(this->mChannel);
   
   // Datasheet GP2D120: http://datasheet.octopart.com/GP2D120XJ00F-Sharp-Microelectronics-datasheet-10606343.pdf
   // 12 is the slope of the graph in figure 5 in the datasheet (estimated, as they didn't specify it :/)
   
   // Distance formular from datasheet
//   float distance = 1/x + 0.42;

  float distance = 0.0;

  if (mChannel == 2) {
    float x = ((float)raw / 1024.0 * 2.56) / 12.0;
    distance = 1/x + 0.42;
  }
  else {
   float x = ((float)raw / 1024.0 * 2.56) / 19.333333333333;   
   distance = 1/x;
  }
   
   *(uint8_t*)value = (uint8_t)roundf(mean_push_get(this->mean, 5, distance));
}


