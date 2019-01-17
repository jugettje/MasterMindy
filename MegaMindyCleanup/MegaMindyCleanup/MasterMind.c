/*
 * MasterMind.c
 *
 * Created: 13-1-2019 15:11:50
 *  Author: jurgen
 */ 

#include "MasterMind.h"
#include "USART1.h"
#include "JR_Macros.h"
#include <util/delay.h>
#include <stdio.h>
#include "infoLeds.h"

//Reset pin interrupt
ISR(PCINT0_vect)
{
	if (SWITCH_PRESSED)
	{
		//if the main loop was waiting for input
		if (resetPressed)
		{
			//break the input loop
			reset = 1;
		}
		//reset the history
		clearLists();
		//clear timer for LED
		turnTimer1Off();
		turnTimer2Off();
		//fill the secret code struct with random numbers
		generateRandomCode(&secretCode1);
		//for debugging: print the generated code
		TransmitString("\r\n\r\nDEBUG :: ");
		TransmitByte('0' + secretCode1.number1);
		TransmitByte('0' + secretCode1.number2);
		TransmitByte('0' + secretCode1.number3);
		TransmitByte('0' + secretCode1.number4);
		TransmitString("\r\n");
		//TransmitString("New game stared\r\n");
		
		//reset the turns
		turns = 12;
		//allow main program to run again
		resetPressed = 1;
	}
}

/*Timer 0 cmp A interrupt
 *use for the random generator
*/
ISR(TIMER0_COMPA_vect)
{
	if(cnt < 200)
	{
		cnt++;
	}
	else
	{
		cnt = 0;
		counter++;
	}
}

//initializes the registers to play the game
void initMasterMind()
{
	/* set interrupt to trigger when timer = OCIE0A
	 * set operation mode to ctc
	 * set the prescaler to 64
	 * freq = F_CPU / (2 * 64 * 250 )
	 * 1/freq = 1/500 s
	 * 200 loops for 1 puls so * 200
	 * 1/((1 / 500) * 200) = 2,5 Hz
	*/
	BIT_ON_IN_REG(TIMSK0, OCIE0A);
	BIT_ON_IN_REG(TCCR0A, WGM01);
	OCR0A = 249;
	BITS_2_ON_IN_REG(TCCR0B, CS10, CS11);
	
	//set interrupt on button
	BIT_ON_IN_REG(PCMSK0, PCINT7);
	BIT_ON_IN_REG(PCICR, PCIE0);
	//make led output
	BIT_ON_IN_REG(DDRB, DDB0);
	//make button input
	BIT_OFF_IN_REG(DDRB, DDB7);
}

//fills a mm_code_t struct with 4 random values
void generateRandomCode(mm_code_t * _code)
{
	srand(counter);
	_code->number1 = 1 + rand()%6;
	_code->number2 = 1 + rand()%6;
	_code->number3 = 1 + rand()%6;
	_code->number4 = 1 + rand()%6;
}

//checks if the user input is the same as the code
void checkCode()
{
	//make sure the mm_result_t struct currentResult is empty
	currentResult.correctLocations = 0;
	currentResult.correctNumbes = 0;
	//add 1 to correctLocations for every correct number on the correct place
	if(secretCode1.number1 == userInputCode.number1)
	{
		currentResult.correctLocations += 1;
	}
	if(secretCode1.number2 == userInputCode.number2)
	{
		currentResult.correctLocations += 1;
	}
	if(secretCode1.number3 == userInputCode.number3)
	{
		currentResult.correctLocations += 1;
	}
	if(secretCode1.number4 == userInputCode.number4)
	{
		currentResult.correctLocations += 1;
	}
	
	
	//add 1 to correctNumbers for every correct number on the wrong place
	// check 1 to 2, 3, 4
	if(userInputCode.number1 == secretCode1.number2)
	{
		currentResult.correctNumbes += 1;
	}
	if(userInputCode.number1 == secretCode1.number3)
	{
		currentResult.correctNumbes += 1;
	}
	if(userInputCode.number1 == secretCode1.number4)
	{
		currentResult.correctNumbes += 1;
	}
	
	// check 2 to 1, 3, 4
	if(userInputCode.number2 == secretCode1.number1)
	{
		currentResult.correctNumbes += 1;
	}
	if(userInputCode.number2 == secretCode1.number3)
	{
		currentResult.correctNumbes += 1;
	}
	if(userInputCode.number2 == secretCode1.number4)
	{
		currentResult.correctNumbes += 1;
	}
	
	// check 3 to 1, 2, 4
	if(userInputCode.number3 == secretCode1.number1)
	{
		currentResult.correctNumbes += 1;
	}
	if(userInputCode.number3 == secretCode1.number2)
	{
		currentResult.correctNumbes += 1;
	}
	if(userInputCode.number3 == secretCode1.number4)
	{
		currentResult.correctNumbes += 1;
	}
	
	// check 4 to 1, 2, 3
	if(userInputCode.number4 == secretCode1.number1)
	{
		currentResult.correctNumbes += 1;
	}
	if(userInputCode.number4 == secretCode1.number2)
	{
		currentResult.correctNumbes += 1;
	}
	if(userInputCode.number4 == secretCode1.number3)
	{
		currentResult.correctNumbes += 1;
	}
}

void printResult()
{
	resetLeds();
	//setLeds();
	setLedSetting();
	timer1_Frequency(5);
	TransmitString("amout of correct places: ");
	TransmitByte('0' + currentResult.correctLocations);
	TransmitString("\r\n");
	_delay_ms(100);
	TransmitString("amout of wrong placed correct numbers: ");
	TransmitByte('0' + currentResult.correctNumbes);
	TransmitString("\r\n");
	_delay_ms(100);
	turns --;
}

int getUserCode(const char * _inputString)
{
	int correctInput = 1;
	mm_code_t inputCode = {0, 0, 0, 0};
	if (isdigit(_inputString[0]))
	{
		if(_inputString[0] - '0' > 0 && _inputString[0] - '0' < 7)
		{
			inputCode.number1 = _inputString[0] - '0';
		}
		else
		{
			correctInput = 0;
		}
	}
	else
	{
		correctInput = 0;
	}
	
	if (isdigit(_inputString[1]))
	{
		if(_inputString[1] - '0' > 0 && _inputString[1] - '0' < 7)
		{
			inputCode.number2 = _inputString[1] - '0';
		}
		else
		{
			correctInput = 0;
		}
	}
	else
	{
		correctInput = 0;
	}
	
	if (isdigit(_inputString[2]))
	{
		if(_inputString[2] - '0' > 0 && _inputString[2] - '0' < 7)
		{
			inputCode.number3 = _inputString[2] - '0';
		}
		else
		{
			correctInput = 0;
		}
	}
	else
	{
		correctInput = 0;
	}
	
	if (isdigit(_inputString[3]))
	{
		if(_inputString[3] - '0' > 0 && _inputString[3] - '0' < 7)
		{
			inputCode.number4 = _inputString[3] - '0';
		}
		else
		{
			correctInput = 0;
		}
	}
	else
	{
		correctInput = 0;
	}
	
	if (!correctInput)
	{
		
		TransmitString("only use the numbers 1 <-> 6  \r\n");
		//TransmitString("try again\r\n");
	}
	userInputCode = inputCode;
	return correctInput;
}

void checkTurn()
{
	userCodeHistory[11 -turns ] = userInputCode;
	userResultHistory[11 - turns ].correctLocations = currentResult.correctLocations;
	userResultHistory[11 - turns ].correctNumbes = currentResult.correctNumbes;
	if(currentResult.correctLocations == 4)
	{
		timer2_Frequency(1);
		TransmitString("You won the game!!\r\n");
		_delay_ms(100);
		printHistory();
		char buffer[40];
		if(12 - turns == 1)
		{
			sprintf(buffer, "%s%d%s\r\n", "you did it in ", 12 - turns, " turn");
		}
		else
		{
			sprintf(buffer, "%s%d%s\r\n", "you did it in ", 12 - turns, " turns");
		}
		TransmitString(buffer);
		_delay_ms(100);
		TransmitString("press reset to start again\r\n");
		resetPressed = 0;
	}
	
	userInputCode.number1 = 0;
	userInputCode.number2 = 0;
	userInputCode.number3 = 0;
	userInputCode.number4 = 0;
	currentResult.correctLocations = 0;
	currentResult.correctNumbes = 0;
	
	if (turns == 0)
	{
		timer2_Frequency(2);
		TransmitString("\r\n");
		printHistory();
		_delay_ms(100);
		TransmitString("winning code was : ");
		TransmitByte('0' + secretCode1.number1);
		TransmitByte('0' + secretCode1.number2);
		TransmitByte('0' + secretCode1.number3);
		TransmitByte('0' + secretCode1.number4);
		TransmitString("\r\n");
		_delay_ms(100);
		TransmitString("you lose press reset to try again\r\n");
		resetPressed = 0;
	}
}

void clearLists()
{
	for(int i = 0; i < 12; i++)
	{
		userCodeHistory[i].number1 = 0;
		userCodeHistory[i].number2 = 0;
		userCodeHistory[i].number3 = 0;
		userCodeHistory[i].number4 = 0;
		userResultHistory[i].correctLocations = 0;
		userResultHistory[i].correctNumbes = 0;
	}
}

void printHistory()
{
	TransmitString("CP = correct placed, WP = wrong placed\r\n");
	_delay_ms(100);
	for(int i = 0; i < 12 - turns; i++)
	{
		char buffer[30];
		sprintf(buffer, "%s%d%s%d%d%d%d\r\n", "turn ", i + 1, ": \tyou played: ",
				userCodeHistory[i].number1, userCodeHistory[i].number2, userCodeHistory[i].number3, userCodeHistory[i].number4);
		TransmitString(buffer);
		_delay_ms(100);
	}
}

void printInfo()
{
	TransmitString(codeInput);
	TransmitString("\r\n");
	//ReceiveByte();
	checkCode();
	printResult();
	checkTurn();
	TransmitString("\r\n\r\n");
	//ReceiveByte();
}

void setLedSetting()
{
	pinStates[0]=0;
	pinStates[1]=0;
	pinStates[2]=0;
	pinStates[3]=0;
	int maxLEds = 4;
	int ledsLeft = maxLEds - currentResult.correctLocations;
	for(int i = 0; i < currentResult.correctLocations; i++)
	{
		pinStates[i]=1;
	}
	if(ledsLeft > 0)
	{
		for(int i = currentResult.correctLocations; i < (currentResult.correctLocations + currentResult.correctNumbes) && i < 4; i++)
		{
			pinStates[i]=2;
		}
	}
	char bufferSTR[10];
	sprintf(bufferSTR, "%d %d %d %d\r\n", pinStates[0], pinStates[1], pinStates[2], pinStates[3]);
	TransmitString(bufferSTR);
}