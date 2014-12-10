#include "ir_sensor.h"
#include <math.h>
#include <Arduino.h>

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
   float x = ((float)raw / 1024.0 * 2.56) / 12.0;
   // Distance formular from datasheet
   float distance = 1/x + 0.42;
   
   *(uint8_t*)value = (uint8_t)roundf(mean_push_get(this->mean, 5, distance));
}


