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
  float rotation[3];
  flySensorRot->getMeasurement(rotation);
  this->lastAngle = rotation[2];
}

void Move::drive(int distance)
{
  this->distance = distance;
  this->leftSpeed = 0x120;
  this->rightSpeed = 0x120;
  this->targetRightSpeed = 100;
  this->targetLeftSpeed = 100;
  this->i = 0;
  memset(this->leftMean, 0, 5);
  memset(this->rightMean, 0, 5);
   
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    this->beginLeftOdometrie = *this->leftOdometrie;
    this->beginRightOdometrie = *this->rightOdometrie;
  }
  
  this->lastLeftOdometrie = this->beginLeftOdometrie;
  this->lastRightOdometrie = this->beginRightOdometrie;
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
    
    double distanceTraveled = (((this->lastLeftOdometrie - this->beginLeftOdometrie) + (this->lastRightOdometrie - this->beginRightOdometrie))/2.0/120.0 * M_PI * 5)/10;
    
    if (distanceTraveled > this->distance) {
//      Serial.println("Done");
      this->distance = 0;
      this->left->move(0);
      this->right->move(0);
    }
    
    if (abs(left - this->targetLeftSpeed) > 5 && this->leftSpeed <= 0x360 && this->leftSpeed >= 0x100) {
      if (left < this->targetLeftSpeed) {
//        Serial.println("Increase left");
        this->leftSpeed += 10;
      }
      else {
//        Serial.println("Decrease left");
        this->leftSpeed -= 10;
      }
    }
    
    if (abs(right - this->targetRightSpeed) > 5 && this->rightSpeed <= 0x360 && this->rightSpeed >= 0x100) {
      if (right < this->targetRightSpeed) {
//        Serial.println("Increase right");
        this->rightSpeed += 10;
      }
      else {
//        Serial.println("Decrease right");
        this->rightSpeed -= 10;
      }
    }
    
//    Serial.print("Left: "); Serial.print(left); Serial.print(" ("); Serial.print(this->leftSpeed); Serial.print(") Right: "); Serial.print(right); Serial.print(" ("); Serial.print(this->rightSpeed); Serial.print(") Distance: ");Serial.println(distanceTraveled);
    this->left->move(this->leftSpeed);
    this->right->move(this->rightSpeed);
  }
  else if (this->remainingAngle) {
      float rotation[3];
      float angle;

      flySensorRot->getMeasurement(rotation);
      angle = rotation[2];
//      Serial.print(angle);Serial.print(":");
      this->remainingAngle -= angle - this->lastAngle;
      this->lastAngle = angle;
//      Serial.println(remainingAngle);
      //Slower at the end for precision.
      if (abs(remainingAngle) < 8) {
        //stop movement once done
//        if (abs(remainingAngle) < 6) {
          left->move(0);
          right->move(0);
          Serial.println("Done");
          this->remainingAngle = 0;
//        }
//        else {
//          Serial.println("Slow");
//          //slowly rotate right
//          if (remainingAngle > 0) {
//            right->move(-kRotateSpeed + 30);
//            left->move(kRotateSpeed - 30);
//          }
//          //slowly rotate left
//          else {
//            right->move(kRotateSpeed - 30);
//            left->move(-kRotateSpeed + 30);
//          }
//        }
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
