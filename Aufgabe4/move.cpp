#include "move.h"
#include <util/atomic.h>


Move::Move(Motor *left, volatile uint32_t* leftOdometrie, Motor *right, volatile uint32_t* rightOdometrie)
{
  this->left = left;
  this->right = right;
  this->flySensorRot = new FlySensorRot();
  this->leftOdometrie = leftOdometrie;
  this->rightOdometrie = rightOdometrie;
  this->distance = 0;
}  

void Move::rotate(float degrees)
{
  float[3] rotation;
  float angle;

  while(true){
  flySensorRot->getMeasurement(rotation);
  angle = rotation[2];
  float degreesToMove = degrees - angle;
      
      //Slower at the end for precision.
      if (abs(degreesToMove) < 8) {
        //stop movement once done
        if (abs(degreesToMove) < 2) {
          left->move(0);
          right->move(0);
          break;
        }
        else {
          //slowly rotate right
          if (degreesToMove > 0) {
            right->move(-kRotateSpeed + 20);
            left->move(kRotateSpeed - 20);
          }
          //slowly rotate left
          else {
            right->move(kRotateSpeed - 20);
            left->move(-kRotateSpeed + 20);
          }
        }
      }
      //rotate right
      if (degreesToMove > 0) {
        right->move(-kRotateSpeed);
        left->move(kRotateSpeed);
      }
      //rotate left
      else {
        right->move(kRotateSpeed);
        left->move(-kRotateSpeed);
      }
  }
}

void Move::drive(int distance)
{
  this->distance = distance;
   
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    this->beginLeftOdometrie = *this->leftOdometrie;
    this->beginRightOdometrie = *this->rightOdometrie;
  }
}

bool Move::do_work()
{
  if (this->distance > 0) {
    uint32_t left, right;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      left = *this->leftOdometrie;
      right = *this->rightOdometrie;
    }
    
    left -= this->beginLeftOdometrie;
    right -= this->beginRightOdometrie;
    
    float distance = right/120 * M_PI * 5;
   
    if (distance > this->distance) {
      this->right->move(0);
      this->left->move(0);
      this->distance = 0;
      Serial.println("Done");
      return false;
    }
   
    float d = ((float)left - (float)right)/100;
       
    Serial.print("Left: ");Serial.print(left);Serial.print(" Right: ");Serial.print(right);Serial.print(" Distance: ");Serial.print(distance);Serial.print(" D: ");Serial.println(d);

    this->right->move(0x240 - 0x50 * d);
    this->left->move(0x240 + 0x50 * d);
  }
  
  return this->distance > 0;
}
