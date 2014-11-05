#include "display.h"

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

void writetoDisplayHelp(int value, int8_t dot, bool sign) {
  if ((sign && value > 199) || (!sign && value > 999)) {
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
  
  writetoDisplay(third, second, first);
}

void writetoDisplay(int value) {
  writetoDisplayHelp(value, -1, true);
}

void writetoDisplay(float value, char dec) {
  for (char i = 0; i < dec; i++) {
    value *= 10;
  }
  
  writetoDisplayHelp((int)value, (int8_t)dec, true);
}

