/*
 * USART1.c
 *
 * Created: 13-1-2019 15:33:16
 *  Author: jurgen
 *
 * the usart.c and .h are for the serial communication
 * all the basic settings are set in the .h file.
 * and all the transmit and receive functions are defined
 * in the .c file.
 * these files can be used in other projects as well.
 * only requires avr/io.h and avr/interrupt.h and a F_CPU define
 */ 

#include "USART1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* UDR0 interrupt
 * when data is in UDR0 and the interrupt 
 * is set send the data.
 */
ISR(USART_UDRE_vect)
{
	unsigned char tmptail;
	/* Check if all data is transmitted */
	if (UART_TxHead != UART_TxTail) 
	{
		/* Calculate buffer index */
		tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK;
		/* Store new index */
		UART_TxTail = tmptail;
		/* Start transmission */
		UDR0 = UART_TxBuf[tmptail];
	}
	else 
	{
		/* Disable UDRE interrupt */
		UCSR0B &= ~(1<<UDRIE0);
	}
}

/* RX interrupt 
 * reads the incoming rx data and 
 * places it in the rx buffer for the 
 * program to use the data
 */
ISR(USART_RX_vect)
{
	char data;
	unsigned char tmphead;
	/* Read the received data */
	data = UDR0;
	/* Calculate buffer index */
	tmphead = (UART_RxHead + 1) & UART_RX_BUFFER_MASK;
	/* Store new index */
	UART_RxHead = tmphead;
	if (tmphead == UART_RxTail) {
		/* ERROR! Receive buffer overflow */
	}
	/* Store received data in buffer */
	UART_RxBuf[tmphead] = data;
}

/* Initialize UART */
void InitUART(unsigned int ubrr_val)
{
	char x;
	/* Set the baud rate */
	UBRR0H = (unsigned char)(ubrr_val>>8);
	UBRR0L = (unsigned char)ubrr_val;
	/* Enable UART receiver and transmitter */
	UCSR0B = ((1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0));
	/* Flush receive buffer */
	x = 0;
	UART_RxTail = x;
	UART_RxHead = x;
	UART_TxTail = x;
	UART_TxHead = x;
}

/* receives a byte(char) from the rx buffer
 * if there is nothing in the buffer 
 * the function will wait till something gets in
 * or until the reset is pressed so it can break 
 * out of the while loop
 */
char ReceiveByte(void)
{
	unsigned char tmptail;
	/* Wait for incoming data */
	while (UART_RxHead == UART_RxTail)
	{
		if (reset)
		{
			break;
		}
	}
	/* Calculate buffer index */
	tmptail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;
	/* Store new index */
	UART_RxTail = tmptail;
	/* Return data */
	return UART_RxBuf[tmptail];
}

/* prepares a byte(char) buffer to send 
 * enables the UDRE interrupt to actually send the data
 */
void TransmitByte(char data)
{
	unsigned char tmphead;
	/* Calculate buffer index */
	tmphead = (UART_TxHead + 1) & UART_TX_BUFFER_MASK;
	/* Wait for free space in buffer */
	while (tmphead == UART_TxTail);
	/* Store data in buffer */
	UART_TxBuf[tmphead] = data;
	/* Store new index */
	UART_TxHead = tmphead;
	/* Enable UDRE interrupt */
	UCSR0B |= (1<<UDRIE0);
}

/* This function gets a string of characters from the USART.
 * The string is placed in the array pointed to by str.
 *
 * - This function uses the function ReceiveByte() to get a byte
 * from the UART.
 * - If the received byte is equal to '\n' (Line Feed),
 * the function returns.
 * - The array is terminated with ´\0´.
 */
void ReceiveString(char *str)
{
	uint8_t t = 0;
	while ((str[t] = ReceiveByte()) != '\n')
	{
		if (reset)
		{
			break;
		}
		t++;
	}
	str[t++] = '\n';
	str[t] = '\0';
}

/* Transmits a string of characters to the USART.
 * The string must be terminated with '\0'.
 *
 * - This function uses the function TransmitByte() to
 * transmit a byte via the UART
 * - Bytes are transmitted until the terminator
 * character '\0' is detected. Then the function returns.
 */
void TransmitString(char *str)
{
	while(*str)
	{
		TransmitByte(*str++);
	}
}

/* resets the usart buffer;
 * both the tx and rx buffers
 */
void resetUart1()
{
	char x = 0;
	UART_RxTail = x;
	UART_RxHead = x;
	UART_TxTail = x;
	UART_TxHead = x;
}

/* NOT WORKING
 * find a way to avoid using _delay in the code
 * instead use the register info to know when to send
 * new data to the transmitString() function
 */
void waitToPrint()
{
	//char _TMP = UCSR0A & (1<<UDRE0);
	//while( ( UCSR0A & ( 1 << UDRE0 ) ) == 0 ){}
}