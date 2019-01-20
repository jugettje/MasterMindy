/*
 * MegaMindyCleanup.c
 *
 * Created: 13-1-2019 15:04:51
 * Author : jurgen
 */ 

#include <avr/io.h>
#include "USART1.h"
#include "Global_Var.h"
#include <avr/interrupt.h>
#include "MasterMind.h"
#include <time.h>
#include "JR_Macros.h"
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include "infoLeds.h"

int main(void)
{
	initMasterMind();
	InitUART(MYUBRR);
	initTimers();
	sei();
	TransmitString("DEBUG: initialising done\r\n");
	
	turns = 12;
	resetPressed = 0;
	reset = 0;
	//char _TMP = UCSR0A;
	//while (!_TMP !=  !(1<<TXC0));
	TransmitString("Press reset to start the game...\r\n");
    while (1) 
    {
		//don't start the game until reset is pressed
		if(resetPressed)
		{
			//print some basic information
			_delay_ms(100);
			resetUart1();
			TransmitString("@------------------@\r\n");
			sprintf(formatString, "%s%d\r\n", "turns left : ", turns);
			TransmitString(formatString);
			_delay_ms(100);
			TransmitString("input code: ");
			//make sure reset is not set
			//if it is set you can't get input
			reset = 0;
			
			//recieve a string
			do
			{
				ReceiveString(codeInput);
				if (reset) //break if reset is pressed
				{
					break;
				}
			} while (!getUserCode(codeInput)); //check the user input
			
			//if reset was not pressed while waiting for input
			if(!reset)
			{
				//print all the info of the turn and store the info
				printInfo();
			}
		}
    }
}


