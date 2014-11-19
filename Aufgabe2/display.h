#include <avr/io.h>
#include <essDisplay.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef DISPLAY_H
#define DISPLAY_H

void writetoDisplay(int value);

void writetoDisplay(float value, char dec=1);

/**
 * @brief Realization of a spirit level (according to the example video)
 *
 *
 * As a simple five-finger exercise ... you can choose your parameters freely ;)
 * 
 * @param x orientation
 * @param y orientation
 */
void balanceDisplay(double x, double y);

#endif
