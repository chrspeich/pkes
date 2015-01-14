#include "fly_Sensor.h"

class Move
{
  public:
    
    Move(Motor *left, Motor *right);
  
    void rotate(int degrees);
    
    /**
    * distance in cm 
    */
    void drive(int distance);
    
 private:
   Motor *left, *right;
   float* rotation;
};
