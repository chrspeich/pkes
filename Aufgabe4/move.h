#include "fly_Sensor.h"
#include "motor.h"

class Move
{
  public:
    
    Move(Motor *left, volatile uint32_t* leftOdometrie, Motor *right, volatile uint32_t* rightOdometrie);
  
    void rotate(float degrees);
    
    /**
    * distance in cm 
    */
    void drive(int distance);
    
    void stop();
    
    // Returns true if more work needs to be done,
    // false if work is done.
    bool do_work();
 private:
   Motor *left, *right;
   float* rotation;
   volatile uint32_t* leftOdometrie;
   volatile uint32_t* rightOdometrie;
   uint32_t lastLeftOdometrie;
   uint32_t lastRightOdometrie;
   double leftMean[5];
   double rightMean[5];
   int16_t rightSpeed;
   int16_t leftSpeed;
   float i;
   
   uint32_t distance;
   uint32_t beginLeftOdometrie;
   uint32_t beginRightOdometrie;
   FlySensorRot *flySensorRot;
   const uint16_t kRotateSpeed = 0x1B0;
   float remainingAngle;
   float lastAngle;
};
