#include <avr/io.h>
#include <essDisplay.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef DISPLAY_H
#define DISPLAY_H

/**
 * @brief Write integer values to display ...
 *
 * Try to implement a function that prints at least values between -199 and 999.
 * See also the datasheet for futher details.
 * 
 * @param value to show
 *
 * @see http://datasheet.octopart.com/LTM-8522HR-Lite-On-datasheet-96660.pdf
 */
void writetoDisplay(int value);

/**
 * @brief Write float values to display ...
 *
 * This can be implemented by using the upper function, or vice versa. The parameter
 * @p dec represents the position of the decimal point. 
 * @p dec == 0 -> (999.) 
 * @p dec == 1 -> (99.9)
 * @p dec == 2 -> (9.99)
 * 
 * @param value to show
 * @param dec imal point
 */
void writetoDisplay(float value, char dec=1);

#endif
