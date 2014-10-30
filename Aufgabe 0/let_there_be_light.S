#define __SFR_OFFSET 0
#include "avr/io.h"

.global main

main: 
  ldi   r16, 0xFF     ; load value 255 into register 16, 0xFF=255=0o377=0b11111111 
  out   DDRB, r16     ; write content of register 16 out to data-direction-register of port b
  rcall led_on       ; call subfunction
loop:
  rcall sleep
  rcall led_on
  rcall sleep
  rcall led_off
  rjmp  loop          ; jump back to loop
led_on:
  ldi   r16, 0xFF     
  out   PORTB, r16    ; 
  ret
led_off:
  out   PORTB, r0     ; write content of register 0 (always 0) out to port b
  ret

sleep:
  ldi r17, 0x0A
_time_loop:
  ldi r18, 0xFF
_time_loop2:
  ldi r19, 0xFF
_time_loop3:
  nop
  SUBI r19, 0x01
  CPI r19, 0x00
  BRNE _time_loop3

  SUBI r18, 0x01
  CPI r18, 0x00
  BRNE _time_loop2

  SUBI r17, 0x01
  CPI r17, 0x00
  BRNE _time_loop

  ret
  
