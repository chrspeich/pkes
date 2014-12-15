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
bool rotating = false;
int rotatingDirection = 1;

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
  TCCR1B = (1 << CS12);    //Control register for OC1A
  
  TCCR3A = (1 << WGM31) | (1 << WGM30) | (1 << COM3C1);
  TCCR3B = (1 << CS32);     //Control register for OC3C
  
  DDRE = (1 << PORTE5);  //Pin 3
  DDRB = (1 << PORTB5) | (1 << PORTB6) | (1 << PORTB7);  //pins 11,12,13
  
  right = new Motor(&OCR3CH, &OCR3CL, &PORTB, 6, -1);
  left  = new Motor(&OCR1AH, &OCR1AL, &PORTB, 7, 1);
}

void loop() {
  
  if(buttonPress(0)){
    mode = !mode;
    Serial.println("Press!");
  }
  
  if(mode){
    left->move(0);
    right->move(0);
    float rot[3];
    
    flyROT->getMeasurement(rot);
    writetoDisplay(rot[2], -1);
    
    Serial.print("z-rotation:"); Serial.println(rot[2]);
    
    // todo
    // rotate back in the oposite direction, see the video
    
  } else {
    uint8_t ir1Distance, ir2Distance;
    ir1->getMeasurement(&ir1Distance);
    ir2->getMeasurement(&ir2Distance);
    
    if (ir1Distance < 10 || ir1Distance > 60 || ir2Distance < 10 || ir2Distance > 60) {
      return;
    }
    
    Serial.print("ir1: "); Serial.print(ir1Distance);  Serial.print("ir2: "); Serial.println(ir2Distance);
    
    if (rotating || ir1Distance < 25 || ir2Distance < 25) {
      Serial.println("rotating");
      writetoDisplay(0b11111110, 0, 0);
      rotating = true;
      
//      if (rotatingDirection == 0) {
//        if (ir1Distance < ir2Distance) {
//          rotatingDirection = 1;
//        }
//        else {
//          rotatingDirection = -1;
//        }
//      }
      
      if (rotatingDirection > 0) {
        right->move(-0x1FF);
        left->move(0x1FF);
      }
      else {
        right->move(0x1FF);
        left->move(-0x1FF);
      }
      
      if (ir1Distance > 30 && ir2Distance > 30) {
        rotating = false;
        rotatingDirection = 1;
      }
    }
    
    if (!rotating) {
      writetoDisplay(0, 0, 0b11111110);
       
      Serial.println("moving");
      right->move(0x300);
      left->move(0x300);
    }
  } 
  
}
