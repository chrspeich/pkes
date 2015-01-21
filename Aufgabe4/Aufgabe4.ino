#include <essDisplay.h>
#include "display.h"

#include <Wire.h>
#include <Flydurino.h>
#include "ir_sensor.h"
#include "motor.h"
#include "move.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

volatile uint32_t motorATicks = 0; 
volatile uint32_t motorBTicks = 0;

ISR(INT4_vect)
{
  motorATicks++;
}

ISR(PCINT0_vect)
{
//  if (EIFR & (1 << 4)) {
    motorBTicks++;
//  }
}

bool __attribute__((OS_buttonPress)) buttonPress(bool button) {
  bool press = 0;
  
  if (button == 0) {
    press = (PING & 0x20);
    
    if (press) {
      delay(200);
      press = (PING & 0x20);
    }
  }
  else {
    press = (PINF & 0x18) != 0;
    if (press) {
          Serial.println("Press A");

      delay(200);
      press = (PINF & 0x18) != 0;
      if (press)
      Serial.println("Press B");
    }
  }
  
  return press;
}

bool mode;
FlySensorAcc *flyACC;
FlySensorRot *flyROT;
IrSensor *ir1;
IrSensor *ir2;
Motor *left;
Motor *right;
Move* move;
bool rotating = false;
bool rotateBack = false;
int rotatingDirection = 1;
uint8_t step = 0;


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
  TCCR1B = (1 << CS11) | (1 << CS10);    //Control register for OC1A
  
  TCCR3A = (1 << WGM31) | (1 << WGM30) | (1 << COM3C1);
  TCCR3B = (1 << CS31);// | (1 << CS30);     //Control register for OC3C
  
  DDRE |= (1 << PORTE5);  //Pin 3
  DDRB |= (1 << PORTB5) | (1 << PORTB6) | (1 << PORTB7);  //pins 11,12,13
  
  right = new Motor(&OCR3CH, &OCR3CL, &PORTB, 6, 1);
  left  = new Motor(&OCR1AH, &OCR1AL, &PORTB, 7, -1);
  move = new Move(right, &motorATicks, left, &motorBTicks);
  
  sei();
  EIMSK |= (1 << 4);  // Demask INT4
  PCICR |= (1 << 0);  // Enable change PC
  PCMSK0 |= (1 << 4); // Demask PCINT4
  EICRB |= (1 << 1);  // any edge of PCINT4
  
  step = 0;
}

static const uint8_t kDigits[] = {
  /* 0 => ABCDEF  */ 0b11111100,
  /* 1 => BC      */ 0b01100000,
  /* 2 => ABDEG   */ 0b11011010,
  /* 3 => ABCDG   */ 0b11110010,
  /* 4 => FGBC    */ 0b01100110,
  /* 5 => AFGCD   */ 0b10110110,
  /* 6 => ACFEDCG */ 0b10111110,
  /* 7 => ABC     */ 0b11100000,
  /* 8 => ABCDEFG */ 0b11111110,
  /* 9 => ABCDFG  */ 0b11110110,
};

void writetoDisplayHelp1(bool rotating, int value, int8_t dot, bool sign) {
  if ((sign && value > 999) || (!sign && value > 199)) {
    writetoDisplay(0b00000010, 0b11111100, 0b00000010);
    return;
  }
  else if ((sign && value < -199) || (!sign && value < 0)) {
    writetoDisplay(0b00000010, 0b01111100, 0b00000010);
    return;
  }
  
  bool minus = false;
  
  if (value < 0) {
    minus = true;
    value = -value;
  }
  
  uint8_t first = kDigits[value % 10];
  value /= 10;
  uint8_t second = kDigits[value % 10];
  value /= 10;
  uint8_t third = kDigits[value % 10];
  
  if (dot == 0) {
    first |= 0x1;
  }
  else if (dot == 1) {
    second |= 0x1;
  }
  else if (dot == 2) {
    third |= 0x1;
  }
  
  if (third == kDigits[0] && dot < 2) {
    third = 0;
    if (second == kDigits[0] && dot < 1) {
      second = 0;
    }
  }
  
  if (minus) {
     third |= 0b00000010; 
  }
  
  if (!rotating) {
    third = 0b01101100;
  }
  else {
    third = 0b10010000;
  }
  
  writetoDisplay(third, second, first);
}

const uint16_t kRotateSpeed = 0x1B0;

uint32_t lastLeft = 0x0;
uint32_t last;

void loop() {
  
//  if(buttonPress(0)){
//    mode = !mode;
//    Serial.println("Press!");
//  }
//  
//  uint32_t aTicks;
//  uint32_t bTicks;
//  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
//    aTicks = motorATicks;
//    bTicks = motorBTicks;
//  }
//  uint32_t diff = bTicks - lastLeft;
//  lastLeft = bTicks;
//  Serial.print("Diff ");Serial.println(diff);
//  left->move(0x140);
//  right->move(0x240);
//  left->move(0x240);
  uint32_t time = millis();
  
  float acc[3];
  flyACC->getMeasurement(&acc);
//  Serial.print("1: ");Serial.print(acc[0]);Serial.print("2: ");Serial.print(acc[1]);Serial.print("3: ");Serial.println(acc[2]);
  
//  if (fabs(acc[2]) > 80) {
//    Serial.println("Stop");
//    move->stop();
//  }
  
  if (time - last > 200) {
    if (!move->do_work()) {
  
      if (step == 0) {
        if (buttonPress(1)) {
          move->drive(500);
          step++;
        }
      }
      else if (step == 1) {
        move->rotate(180);
        step++;
      }
      else if (step == 2) {
        move->drive(500);
        step=0;
      }
    }
    last = time;
  }
  return;
  
}
