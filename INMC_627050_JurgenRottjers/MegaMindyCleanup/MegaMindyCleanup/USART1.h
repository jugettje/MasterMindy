/*
 * USART1.h
 *
 * Created: 13-1-2019 15:33:06
 *  Author: jurgen
 *
 * the usart.c and .h are for the serial communication
 * all the basic settings are set in the .h file.
 * and all the transmit and receive functions are defined
 * in the .c file.
 * these files can be used in other projects aswell.
 * only requires avr/io.h and avr/interrupt.h
 * this file uses Global_Var.h just for the F_CPU
 * if use elsewhere remove this and set own CPU speed.
 */ 


#ifndef USART1_H_
#define USART1_H_

#include "Global_Var.h"


#define FOSC F_CPU // Clock speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

/* UART Buffer Defines */
#define UART_RX_BUFFER_SIZE 32 /* 2,4,8,16,32,64,128 or 256 bytes */
#define UART_TX_BUFFER_SIZE 32 /* 2,4,8,16,32,64,128 or 256 bytes */
#define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)

#if (UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK)
	#error RX buffer size is not a power of 2
#endif

#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)

#if (UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK)
	#error TX buffer size is not a power of 2
#endif

/* Static Variables */
static char UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile char UART_RxHead;
static volatile char UART_RxTail;
static char UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile char UART_TxHead;
static volatile char UART_TxTail;

/* Prototypes */
void InitUART(unsigned int ubrr_val);
char ReceiveByte(void);
void TransmitByte(char data);
void ReceiveString(char *str);
void TransmitString(char *str);
void resetUart1();
//void waitToPrint();

#endif /* USART1_H_ */