#include <essDisplay.h>
#include "display.h"

#include <Wire.h>
#include <Flydurino.h>
#include "fly_sensor.h"
#include "ir_sensor.h"
#include "motor.h"

bool __attribute__((OS_buttonPress)) buttonPress(bool button) {
  bool press = 0;
  asm volatile ("    CPI   %[btn], 1   \n\t"
                "    BREQ AButton      \n\t"
                "DButton:              \n\t"
                "    IN   r16, %[ping] \n\t"
                "    ANDI r16, 0x20    \n\t"
                "    JMP  CMP          \n\t"
                "AButton:              \n\t"
                "    IN   r16, %[pinf] \n\t"
                "    ANDI r16, 0x18    \n\t"
                "CMP:                  \n\t"
                "    CPI  r16, 0x0     \n\t"
                "    BRNE PRESSED      \n\t"
                "    LDI  %[result], 0 \n\t"
                "    JMP  END          \n\t"
                "PRESSED:              \n\t"
                "    LDI  %[result], 1 \n\t"
                "END:                  \n\t"
                "                      \n\t"
                
                : [result]"=r" (press)
                : [ping] "I" (_SFR_IO_ADDR(PING)), [pinf] "I" (_SFR_IO_ADDR(PINF)), [btn] "w" (button)
                : "r16"); 
  return press;
}

bool mode;
FlySensorAcc *flyACC;
FlySensorRot *flyROT;
IrSensor *ir1;
IrSensor *ir2;
Motor *left;
Motor *right;

void setup() {
  float version=0.4;
  
  Serial.begin(9600);
  Serial.println("----------------------------------");
  Serial.println("PKES Wintersemester 2014/15"       );
  Serial.print  ("Vorlage 3. essBot - Version "      );
  Serial.println(version                             );
  Serial.println("----------------------------------");

  initDisplay();
  
  flyACC = new FlySensorAcc();
  flyROT = new FlySensorRot();
  ir1    = new IrSensor(2);
  ir2    = new IrSensor(3);
  
  mode = 1;
  
  
  // TODO: 
  // 1. pwm-config for OC1A and OC3C
  //    use phase-correct pwm with 10bits, prescaler is optional
  // 2. set the data-direction for PWM-channels
  //    arduino digital pins 11 and 3 
  // 3. set the data-direction for motor direction pins
  //    located at arduino digital pin 12 and 13
 
  TCCR1A = (1 << WGM11) | (1 << WGM10) | (1 << COM1A1);  
  TCCR1B = (1 << CS12) | (1 << CS10);    //Control register for OC1A
  
  TCCR3A = (1 << WGM31) | (1 << WGM30) | (1 << COM3C1);
  TCCR3B = (1 << CS32) | (1 << CS30);     //Control register for OC3C
  
  DDRE = (1 << PORTE5);  //Pin 3
  DDRB = (1 << PORTB5) | (1 << PORTB6) | (1 << PORTB7);  //pins 11,12,13
  
  right = new Motor(&OCR3CH, &OCR3CL, &PORTB, 6, -1);
  left  = new Motor(&OCR1AH, &OCR1AL, &PORTB, 7, 1);
  
  right->move(0xFF);
  
}

void loop() {
  
  if(buttonPress(0)){
    mode = !mode;
  }
  
  if(mode){
    float rot[3];
    
    flyROT->getMeasurement(rot);
    writetoDisplay(rot[2], -1);
    
    Serial.print("z-rotation:"); Serial.println(rot[2]);
    
    // todo
    // rotate back in the oposite direction, see the video
    
  } else {
    // todo
    // perform a random walk, if an obstacle is detected ...
    // perform a random rotation and move forward (or backward)
  } 
  
}
