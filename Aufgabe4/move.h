#include "fly_Sensor.h"
#include "motor.h"

class Move
{
  public:
    
    Move(Motor *left, Motor *right);
  
    void rotate(float degrees);
    
    /**
    * distance in cm 
    */
    void drive(int distance);
    
 private:
   Motor *left, *right;
   FlySensorRot *flySensorRot;
   const uint16_t kRotateSpeed = 0x1B0;
};
