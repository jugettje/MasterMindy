/*
 * infoLeds.c
 *
 * Created: 13-1-2019 22:15:20
 *  Author: jurgen
 */ 
#include "infoLeds.h"
#include "JR_Macros.h"
#include "Global_Var.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include "MasterMind.h"

volatile char PORTB_Pins[] = {PORTB0, PORTB1, PORTB2, PORTB3};
volatile char PINB_Pins[4] = {PINB0, PINB1, PINB2, PINB3};

ISR(TIMER2_COMPA_vect)
{
	LED_TOGGLE;
}

ISR(TIMER1_COMPA_vect)
{
	for(int i = LED_counter; i < userResultHistory[12 - turns ].correctLocations + userResultHistory[11 - turns ].correctNumbes && i < 4; i++)
	{
		//PINB |= (1<<PINB_Pins[k]);
		BIT_FLIP_IN_REG(PORTB, PORTB_Pins[i]);
	}
}

void initTimers()
{
	LED_counter = 0;
	BITS_5_ON_IN_REG(DDRB, DDB0, DDB1, DDB2, DDB3, DDB5);
	/*
	*	TCCR1B (Timer \ Counter1 Control Register 1 B) 16.11.2
	*	TCCR1B Bit 4, 3 and TCCR1A Bit 1, 0 to select the Mode of Operation 
	*	we make bit 3 of TCCR1B high for the code (0b0100) CTC mode
	*/
	BIT_ON_IN_REG(TCCR1B, WGM12);	
	
	BIT_ON_IN_REG(TCCR2A, WGM21);
	
	/*
	*	TIMSK1 (Timer / Counter1 Interrupt Mask Register) 16.11.8
	*	Bit 1 (OCIE1A [Timer / Counter1 Output Compare A Match Interrupt Enable])
	*	Turn this bit on to trigger the interrupt if output compare A is high 
	*/
	BIT_ON_IN_REG(TIMSK1, OCIE1A);
	
	BIT_ON_IN_REG(TIMSK2, OCIE2A);
	
	//init timer 2
}

void timer2_Frequency(uint8_t freq)
{
	/*
	*	TCCR1B (Timer \ Counter1 Control Register 1 B) 16.11.2
	*	Bit 2, 1, 0 for Clock Select
	*	We make bit 2 (CS12) high for the code (0b100) prescaler 256
	*/
	BIT_ON_IN_REG(TCCR1B, CS12);	
	
		/*
	*	OCR1A (Timer / Counter 1 Output Compare Register A) 16.11.5
	*	Formula for frequency (16.9.2)
	*	[F_CPU = Fclk_I/O], [N = Prescaler]
	*	Freq = F_CPU / 2 * N * (1 + OCR1A)
	*	Change formula to calculate OCR1A with a given Freq
	*	OCR1A = F_CPU / (Freq * 2 * N) - 1
	*/
	OCR1A = (F_CPU/(freq*2*256)-1);
}

void turnTimer2Off()
{
	BIT_OFF_IN_REG(TCCR1B, CS12);
	OCR1A = 0;
}

void timer1_Frequency(uint8_t freq)
{
	/*
	*	TCCR1B (Timer \ Counter1 Control Register 1 B) 16.11.2
	*	Bit 2, 1, 0 for Clock Select
	*	We make bit 2 (CS12) high for the code (0b100) prescaler 256
	*/
	BITS_2_ON_IN_REG(TCCR2B, CS22, CS21);	
	
		/*
	*	OCR1A (Timer / Counter 1 Output Compare Register A) 16.11.5
	*	Formula for frequency (16.9.2)
	*	[F_CPU = Fclk_I/O], [N = Prescaler]
	*	Freq = F_CPU / 2 * N * (1 + OCR1A)
	*	Change formula to calculate OCR1A with a given Freq
	*	OCR1A = F_CPU / (Freq * 2 * N) - 1
	*/
	OCR2A = (F_CPU/(freq*2*256)-1);
}

void turnTimer1Off()
{
	BITS_2_OFF_IN_REG(TCCR2B, CS22, CS21);
	OCR2A = 0;
	LED_OFF;
}

void resetLeds()
{
	for (int i = 0; i < 4; i++)
	{
		PIN_X_OFF(PORTB, PORTB_Pins[i]);
	}
	LED_counter = 0;
}

void setLeds()
{
	for(int i = LED_counter; i < userResultHistory[12 - turns ].correctLocations; i++)
	{
		PIN_X_ON(PORTB, PORTB_Pins[i]);
		LED_counter = i + 1;
	}
}