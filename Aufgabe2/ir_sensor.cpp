#include "ir_sensor.h"
#include "util.h"
#include <Arduino.h>

/**
 * @brief Configure the analog-channel
 *
 * The Infrared sensors are connected to (arduino) analog pin 2 and 3.
 * Use it wise during the initialization ;)
 */

IrSensor::IrSensor(uint8_t channel) {
  mChannel = channel;
  configure();
}

/**
 * @brief General Analog-Read configuration
 *
 * Configure the reference-voltage according to your SHARP-Infrared distance sensor.
 * See therefore also the specific sensor-type (marked at the top of the sensor).
 *
 * Keep in mind that the configuration is set globally.
 *
 * @see http://arduino.cc/de/Reference/AnalogReference
 */

void IrSensor::configure(){
  analogReference(INTERNAL2V56);
}

/**
 * @brief Read in distance measurements
 *
 * Read in the analog sensor value and try to linearize the measurements accordingly.
 * Choose therefor one of the methods presented in the lecture and store the result in @p *value.
 *
 * @param *value should point to an external integer of float, see the fly_sensor example
 *
 * @see http://arduino.cc/de/Reference/AnalogRead
 */

void IrSensor::getMeasurement(void *value){
   int raw = analogRead(this->mChannel);
   
   // Datasheet GP2D120: http://datasheet.octopart.com/GP2D120XJ00F-Sharp-Microelectronics-datasheet-10606343.pdf
   // 12 is the slope of the graph in figure 5 in the datasheet (estimated, as they didn't specify it :/)
   float x = ((float)raw / 1024.0 * 2.56) / 12.0;
   // Distance formular from datasheet
   float distance = 1/x + 0.42;
   
   *(uint8_t*)value = (uint8_t)roundf(mean_push_get(this->mean, 5, distance));
}


