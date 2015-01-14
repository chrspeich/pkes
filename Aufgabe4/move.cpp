#include "move.h"



Move::Move(Motor *left,Motor *right)
{
  this->left = left;
  this->right = right;
  this->flySensorRot = new FlySensorRot();
}  

void Move::rotate(float degrees)
{
  float[3] rotation;
  flySensorRot->getMeasurement(rotation);
  
  float angle = rotation[2];
  float degreesToMove = degrees - angle;
      
      //Slower at the end for precision.
      if (abs(degreesToMove) < 8) {
        //stop movement once done
        if (abs(degreesToMove) < 2) {
          left->move(0);
          right->move(0);
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

void Move::drive(int distance)
{
  
  
  
  
}
