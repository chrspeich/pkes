#include <avr/io.h>
#include <essDisplay.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef DISPLAY_H
#define DISPLAY_H

uint8_t displayMask(char val);

// todo
void writetoDisplay(int value);
// todo
void writetoDisplay(float value, char dec=1);

// todo
void balanceDisplay(double x, double y);

#endif
