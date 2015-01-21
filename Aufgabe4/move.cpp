#include "move.h"
#include <util/atomic.h>
#include "util.h"


Move::Move(Motor *left, volatile uint32_t* leftOdometrie, Motor *right, volatile uint32_t* rightOdometrie)
{
  this->left = left;
  this->right = right;
  this->flySensorRot = new FlySensorRot();
  this->leftOdometrie = leftOdometrie;
  this->rightOdometrie = rightOdometrie;
  this->distance = 0;
  this->remainingAngle = 0;
}  

void Move::rotate(float degrees)
{  
  this->remainingAngle = degrees;
  this->lastAngle = 0;
}

void Move::drive(int distance)
{
  this->distance = distance;
  this->leftSpeed = 0x180;
  this->rightSpeed = 0x180;
  this->lastLeftOdometrie = 0x0;
  this->lastRightOdometrie = 0x0;
  this->i = 0;
  memset(this->leftMean, 0, 5);
  memset(this->rightMean, 0, 5);
   
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    this->beginLeftOdometrie = *this->leftOdometrie;
    this->beginRightOdometrie = *this->rightOdometrie;
  }
}

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

bool Move::do_work()
{
  if (this->distance > 0) {
    uint32_t left, right;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      left = *this->leftOdometrie;
      right = *this->rightOdometrie;
    }
    
    uint32_t temp1 = left, temp2 = right;
    
    left -= this->lastLeftOdometrie;
    right -= this->lastRightOdometrie;
    this->lastLeftOdometrie = temp1;
    this->lastRightOdometrie = temp2;
    
    left = mean_push_get(this->leftMean, 5, left);
    right = mean_push_get(this->rightMean, 5, right);
    
    float distance = (this->lastLeftOdometrie - this->beginLeftOdometrie)/120.0 * M_PI * 5.0;
   
    if (distance > this->distance) {
      this->right->move(0);
      this->left->move(0);
      this->distance = 0;
      Serial.print("Done ");Serial.println(distance);
      return false;
    }
   
    float d = ((float)left - (float)right);
    
    if ((d > 0 && this->rightSpeed < 0x360) || (d < 0 && this->rightSpeed > 0x100)) {
      this->i += d * 0.8;
      this->rightSpeed += d + this->i;
      
      if (this->rightSpeed > 0x360) {
        this->rightSpeed = 0x360;
      }
    }
    else {
      this->i = 0;
    }
    
    Serial.print("Left: ");Serial.print(left);Serial.print(" (");Serial.print(this->leftSpeed);Serial.print(") Right: ");Serial.print(right);Serial.print(" (");Serial.print(this->rightSpeed);Serial.print(") Distance: ");Serial.print(distance);Serial.print(" D: ");Serial.print(d);Serial.print(" ");Serial.print(this->distance);Serial.print(" I ");Serial.println(this->i);

    this->left->move(this->leftSpeed);
    this->right->move(this->rightSpeed);
  }
  else if (this->remainingAngle) {
      float rotation[3];
      float angle;

      flySensorRot->getMeasurement(rotation);
      angle = rotation[2];
      Serial.print(angle);Serial.print(":");
      this->remainingAngle -= angle - this->lastAngle;
      this->lastAngle = angle;
      Serial.println(remainingAngle);
      //Slower at the end for precision.
      if (abs(remainingAngle) < 8) {
        //stop movement once done
        if (abs(remainingAngle) < 2) {
          left->move(0);
          right->move(0);
          Serial.println("Done");
          this->remainingAngle = 0;
        }
        else {
          Serial.println("Slow");
          //slowly rotate right
          if (remainingAngle > 0) {
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
      if (remainingAngle > 0) {
        right->move(-kRotateSpeed);
        left->move(kRotateSpeed);
      }
      //rotate left
      else {
        right->move(kRotateSpeed);
        left->move(-kRotateSpeed);
      }
  }
  else {
    this->right->move(0);
    this->left->move(0);
  }
  
  return this->distance > 0 || this->remainingAngle != 0;
}

void Move::stop()
{
  this->distance = 0;
  this->remainingAngle = 0;
  this->left->move(0);
  this->right->move(0);
}
