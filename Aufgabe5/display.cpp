#include "display.h"
#include "util.h"
#include <math.h>

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

double x_mean[5] = {0, 0, 0, 0, 0};
double y_mean[5] = {0, 0, 0, 0, 0};

void balanceDisplay(double x, double y){
  x = mean_push_get(x_mean, 5, x);
  y = mean_push_get(y_mean, 5, y);
  int v = 0xFF;
  int h = 0xFF;
  
  if (fabs(x) < 60.0/4.0) {
    h = 0;
  }
  else if (fabs(x) < 2.0 * 60.0/4.0) {
    h = copysignf(1, x);
  }
  else if (fabs(x) < 3.0 * 60.0/4.0) {
    h = copysignf(2, x);
  }
  else {
    h = copysignf(3, x);
  }
  
  if (fabs(y) < 60.0/2.0) {
    v = 0;
  }
  else {
    v = copysignf(1, x);
  }
  
  int first = 0;
  int second = 0;
  int third = 0;
  
  if (h == 0) {
    second |= 0b01101100;
  }
  else if (h == -1) {
    second |= 0b00001100;
  }
  else if (h == 1) {
    second |= 0b01100000;
  }
  else if (h == 3) {
    third |= 0b01100000;
  }
  else if (h == -3) {
    first |= 0b00001100;
  }
  else if (h == 2) {
    third |= 0b00001100;
  }
  else if (h == -2) {
    first |= 0b01100000;
  }
  
  if (v == 0) {
    first |= 0b00000010;
    second |= 0b00000010;
    third |= 0b00000010;
  }
  else if (v == -1) {
    first |= 0b00010000;
    second |= 0b00010000;
    third |= 0b00010000;
  }
  else if (v == 1) {
    first |= 0b10000000;
    second |= 0b10000000;
    third |= 0b10000000;
  }
  writetoDisplay(first, second, third);
}
