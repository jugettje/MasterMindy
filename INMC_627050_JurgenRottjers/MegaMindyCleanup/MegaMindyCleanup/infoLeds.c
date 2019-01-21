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

//initialize the infoLeds variables
volatile char PORTB_Pins[4] = {PORTB0, PORTB1, PORTB2, PORTB3};
volatile char PINB_Pins[4] = {PINB0, PINB1, PINB2, PINB3};
volatile int pinStates[4] = {0,0,0,0};

/* timer 2 interrupt. will trigger at a given frequency timer2_Frequency(int setting)
 * loops trough the PORTB and pinstates arrays to set leds.
 */
ISR(TIMER1_COMPA_vect)
{
	for(int i = 0; i < 4; i++)
	{
		switch (pinStates[i])
		{
		case 0: // index i of PORTB pins array needs to be turned off
			BIT_OFF_IN_REG(PORTB, PORTB_Pins[i]);
		break;
		case 1: // index i of PORTB pins array needs to be turned on
			BIT_ON_IN_REG(PORTB, PORTB_Pins[i]);
		break;
		case 2: // index i of PORTB pins array needs to blink (5hz)
			BIT_FLIP_IN_REG(PORTB, PORTB_Pins[i]);
		break;
		}
	}
}

//timer 2 comp A interrupt to make the led blink at 20hz or 1hz
//timer 2 matches A at 30hz so the isr will be called every 30hz
ISR(TIMER2_OVF_vect)
{
	static double count = 0;
	if(count < 30 && LED_setting == 1)
	{
		count ++;
	}
	else if (LED_setting == 1)
	{
		LED_TOGGLE;
		count = 0;
	}
	
	if(count < 1.5 && LED_setting == 2)
	{
		count += 0.75;
	}
	else if (LED_setting == 2)
	{
		LED_TOGGLE;
		count = 0;
	}
}

//set the registers. if not initialized nothing will work properly 
void initTimers()
{
	LED_counter = 0;
	LED_setting = 0;
	BITS_5_ON_IN_REG(DDRB, DDB0, DDB1, DDB2, DDB3, DDB5);
	/*
	*	TCCR1B (Timer \ Counter1 Control Register 1 B) 16.11.2
	*	TCCR1B Bit 4, 3 and TCCR1A Bit 1, 0 to select the Mode of Operation 
	*	we make bit 3 of TCCR1B high for the code (0b0100) CTC mode
	*/
	BIT_ON_IN_REG(TCCR1B, WGM12);	
	
	/*
	*	TIMSK1 (Timer / Counter1 Interrupt Mask Register) 16.11.8
	*	Bit 1 (OCIE1A [Timer / Counter1 Output Compare A Match Interrupt Enable])
	*	Turn this bit on to trigger the interrupt if output compare A is high 
	*/
	BIT_ON_IN_REG(TIMSK1, OCIE1A);
	
	//let timer 2 interrupt if it overflows
	BIT_ON_IN_REG(TIMSK2, TOIE2);
}

//sets a frequency to timer 1 and turns it on (5hz for assignment)
void timer1_Frequency(uint8_t freq)
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

//turns on timer 2 and you can choose a frequency
// 1 for 1hz, 2 for 20hz
void timer2_Frequency(int setting)
{
	/*
	*	TCCR1B (Timer \ Counter1 Control Register 1 B) 16.11.2
	*	Bit 2, 1, 0 for Clock Select
	*	We make bit 2 (CS12), bit 1 (CS11) and bit 0 (CS10)
	*	high for the code (0b111) prescaler 1024
	*/
	BITS_3_ON_IN_REG(TCCR2B, CS22, CS21, CS20);	
	LED_setting = setting;
}

//reset timer 1
void turnTimer1Off()
{
	//reset prescaler, reset comp register A and turn off the leds
	BIT_OFF_IN_REG(TCCR1B, CS12);
	OCR1A = 0;
	resetLeds();
}

//reset timer 2
void turnTimer2Off()
{
	//reset the prescaler so the timer stops. and turn the led off
	BITS_3_OFF_IN_REG(TCCR2B, CS22, CS21, CS20);
	LED_OFF;
	LED_setting = 0;
}

//turn of all the leds in the PORTB pins array
void resetLeds()
{
	for (int i = 0; i < 4; i++)
	{
		PIN_X_OFF(PORTB, PORTB_Pins[i]);
	}
	LED_counter = 0;
}
