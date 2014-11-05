#include <essDisplay.h>
#include "display.h"

/**
 * @brief Check if buttons are pressed or not...
 *
 * The functionality should be implemented in and only in inline assembly. Variable
 * @p press is used to store the return value (true if the appropriate button was
 * pressed). The input parameter @p button is only used to differentiate between
 * the two buttons on the developer shield...
 * 
 * Buttons are at (Ardino) digital pin 4 and analog pin 4, see also the PinMap con-
 * version.
 * 
 * @param button true stands for the upper button false for the other one ...
 * @return true if the button is pressed, otherwise false 
 *
 * @see loop()
 * @see http://hitboxgame.com/wordpress/wp-content/uploads/2014/05/PinMap2560sma_.png
 */
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


short int counter0 =0;
float counter1     =0;
bool  direction    =1;
bool  counter      =0;

void setup() {
  float version = 0.4;
    
  Serial.begin(9600);
  Serial.println("----------------------------------");
  Serial.println("PKES Wintersemester 2014/15"       );
  Serial.print  ("Vorlage 1. essBot - Version "      );
  Serial.println(version                             );
  Serial.println("----------------------------------");

   // Set PORTG and PORTF as input
  asm volatile ("    LDI  r16, 0x0   \n\t"
                "    OUT %0, r16    \n\t"
                "    OUT %1, r16    \n\t"
                : 
                : "I" (_SFR_IO_ADDR(DDRG)), "I" (_SFR_IO_ADDR(DDRF))
                : "r16"); 

  initDisplay();
}

/**
 * @brief Counting int and float values ...
 *
 * This loop is only used to implement the initial interface for your essBOT, the 7
 * segment display (plus one dot) and the two buttons. This is required later to
 * show the sensor measurements. Implement therefor the buttonPress() function  as
 * well as the two writetoDisplay(int) and writetoDisplay(float, char) functions,
 * which have to be implemented in display.cpp. Check if the display shows the same
 * results as received via the serial interface.
 *
 * @note http://www.atmel.com/images/doc2549.pdf
 */
void loop() {
  delay(250);
 
  if(buttonPress(0)) direction=!direction;
  
  if(buttonPress(1)) counter  =!counter;
  
  if(counter){
    counter1 += direction ? 0.1 : -0.1;
    writetoDisplay(counter1);
    Serial.print("Float: "); Serial.println(counter1);
  }
  else{
    writetoDisplay(direction ? (int)counter0++ : (int)counter0--);
    Serial.print("Int: "); Serial.println((int)counter0);
  }
}
