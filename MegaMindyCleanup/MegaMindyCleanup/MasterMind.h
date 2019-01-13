/*
 * MasterMind.h
 *
 * Created: 13-1-2019 15:12:02
 *  Author: jurgen
 */ 


#ifndef MASTERMIND_H_
#define MASTERMIND_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#include <time.h>

typedef struct mm_code_t
{
	int number1;
	int number2;
	int number3;
	int number4;
	
}mm_code_t;

typedef struct mm_result_t
{
	int correctLocations;
	int correctNumbes;
}mm_result_t;

mm_code_t secretCode1;
mm_code_t userInputCode;
mm_code_t userCodeHistory[12];
mm_result_t currentResult;
mm_result_t userResultHistory[12];

static volatile uint16_t cnt;
static volatile unsigned int counter;

void initMasterMind();
void generateRandomCode(mm_code_t * _code);
void checkCode();
void checkTurn();
void printResult();
int getUserCode(const char * _inputString);
void clearLists();
void printHistory();
void printInfo();


#endif /* MASTERMIND_H_ */