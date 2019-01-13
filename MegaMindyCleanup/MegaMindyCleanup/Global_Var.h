/*
 * Global_Var.h
 *
 * Created: 13-1-2019 15:08:15
 *  Author: jurgen
 */ 


#ifndef GLOBAL_VAR_H_
#define GLOBAL_VAR_H_

#include <avr/io.h>

#define F_CPU 16000000UL // 16Mhz
#define SWITCH_PRESSED	!(PINB & (1<<PINB7))

char codeInput[10];
char formatString[20];

volatile int turns;
volatile int resetPressed;
volatile int reset;

#endif /* GLOBAL_VAR_H_ */