#include "fly_sensor.h"
#include "util.h"

FlySensor::FlySensor(){
  flydurino = new Flydurino();
}

/*************************************************************/

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

/*************************************************************/

FlySensorRot::FlySensorRot() : FlySensor(){  
  mRotation[0] = mRotation[1] = mRotation[2] = 0;
  configure();
  mTime = millis();  
}

/**
 * @brief offset-correction
 *
 * As many other sensors, also this gyroscope has a natural offset, in
 * dependence to the applied filter. Check the fly_rot.ino - example at:
 * File -> Examples -> essFlyduino 
 * Identify this offset and use it for further error-correction within
 * method "getMeasurement" ...
 *
 * @see http://www.invensense.com/mems/gyro/documents/PS-MPU-6000A-00v3.4.pdf
 */
void FlySensorRot::configure() {
  flydurino->setDLPFMode(6);
  
  mOffset[0]   = -117.40;
  mOffset[1]   = 78.25;
  mOffset[2]   = 26.5;
  Serial.println(flydurino->getFullScaleGyroRange());
}

/**
 * @brief Rotation in degrees ...
 *
 * Calculate the rotation angles according to x, y, and z. Keep in mind,
 * that this gyroscope is an incremental (over time) sensor... Have also
 * a look into the datasheet for the correct convertion.
 *
 * @see http://www.invensense.com/mems/gyro/documents/PS-MPU-6000A-00v3.4.pdf
 */
void FlySensorRot::getMeasurement(void *value){
  int16_t x, y, z;
  flydurino->getRotation(&x, &y, &z);

#if 1
  static double mean[3][20] = {{0}};
  
  Serial.print("Means: x="); Serial.print(mean_push_get(mean[0], 20, x));
  Serial.print(" y="); Serial.print(mean_push_get(mean[1], 20, y));
  Serial.print(" x="); Serial.println(mean_push_get(mean[2], 20, z));
#endif
  
  float* uiValue = (float*)value;
  uiValue[0] = ((float)x - mOffset[0])/131.0;
  uiValue[1] = ((float)y - mOffset[1])/131.0;
  uiValue[2] = ((float)z - mOffset[2])/131.0;
}
