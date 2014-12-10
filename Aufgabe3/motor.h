#include <arduino.h>

#ifndef MOTOR_H
#define MOTOR_H

class Motor{
public:
  /**
  * @brief a cpp is not alway reqired ...
  */
  Motor(volatile uint8_t* highByte, volatile uint8_t* lowByte, volatile uint8_t* dir, uint8_t pin, int8_t adapt){
       m_High = highByte;
       m_Low  = lowByte;
       m_DirPort = dir;
       m_DirPin  = pin;
       m_DirAdapt= adapt;
    };
    
  /**
  * @brief turning the wheels...
  *
  * iSpeed is used to define the speed (or in other words, the PWM compare-value) of the wheel and the direction
  * a positive value should move the whell forward and a negative backward ... 
  *
  */
  void move(int16_t iSpeed){
       
  };
private:
  volatile uint8_t * m_High;
  volatile uint8_t * m_Low;
  volatile uint8_t * m_DirPort;
  uint8_t m_DirPin;
  int8_t  m_DirAdapt;
};

#endif
