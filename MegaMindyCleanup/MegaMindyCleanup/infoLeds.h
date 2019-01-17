/*
 * infoLeds.h
 *
 * Created: 13-1-2019 22:15:09
 *  Author: jurgen
 */ 


#ifndef INFOLEDS_H_
#define INFOLEDS_H_
#include "stdint.h"
#include <avr/io.h>

void initTimers();
void timer1_Frequency(uint8_t freq);
void timer2_Frequency(uint8_t freq);
void turnTimer1Off();
void turnTimer2Off();
void resetLeds();
void setLeds();


volatile char PORTB_Pins[4];
volatile char PINB_Pins[4];
volatile int LED_counter;

#endif /* INFOLEDS_H_ */