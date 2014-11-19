#include "fly_sensor.h"
#include <math.h>

FlySensor::FlySensor(){
  flydurino = new Flydurino();
}

/**
 * @brief Convert two ACC values to an orientation
 *
 * In general you get two forces, either in (z,x), (z,y), or (x,y). Your task is
 * to translate these "2D-vector" into angle in degree, not rad! That's it ...
 */

float FlySensorAcc::convertACC(int16_t v1, int16_t v2){
  float a = fabs(v1) < fabs(v2) ? v1 : v2;
  float b = fabs(v1) > fabs(v2) ? v1 : v2;
  
  if (b == 0) {
    return 0;
  }
  
  return asin(a/b) * 180.0 / M_PI;
}

void FlySensorAcc::getMeasurement(void *value){
  float *uiValue = (float *) value;
  
  int16_t acc_x, acc_y, acc_z;
  flydurino->getAcceleration(&acc_x, &acc_y, &acc_z);

  uiValue[0] = convertACC(acc_z, acc_x);
  uiValue[1] = convertACC(acc_z, acc_y);
  uiValue[2] = convertACC(acc_x, acc_y);
}
