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
void timer2_Frequency(int setting);
void turnTimer1Off();
void turnTimer2Off();
void resetLeds();
void setLeds();
void setLedSetting();


volatile char PORTB_Pins[4];
volatile char PINB_Pins[4];
volatile int pinStates[4];
volatile int LED_counter;
volatile int LED_setting;

#endif /* INFOLEDS_H_ */