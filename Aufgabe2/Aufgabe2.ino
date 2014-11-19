#include <essDisplay.h>
#include "display.h"

#include <Wire.h>
#include <Flydurino.h>
#include "fly_sensor.h"
#include "ir_sensor.h"

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
IrSensor *ir;

void setup() {
  float version = 0.4;
    
  Serial.begin(9600);
  Serial.println("----------------------------------");
  Serial.println("PKES Wintersemester 2014/15"       );
  Serial.print  ("Vorlage 2. essBot - Version "      );
  Serial.println(version                             );
  Serial.println("----------------------------------");

  initDisplay();
  
  flyACC = new FlySensorAcc();
  ir     = new IrSensor(3);
  mode = 0;
}

void loop() {
  if(buttonPress(0)){
    mode = !mode;
  }
  
  if(mode){
    float acc[3];
    flyACC->getMeasurement(acc);
    Serial.print("Orientation; "); Serial.print(acc[0]); Serial.print(", "); Serial.print(acc[1]); Serial.print(", "); Serial.println(acc[2]);
    balanceDisplay(acc[0], acc[1]);
  } else {
    uint8_t distance;
    ir->getMeasurement(&distance);
    Serial.print("Distance IR: "); Serial.println(distance);
    writetoDisplay((int)distance);
  }  
}
