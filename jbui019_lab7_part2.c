/*	Author: Jarvis Bui
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "io.h"
#endif

enum States{start, d1, d2 ,d3, wait, waitR, victory, restart}state;
volatile unsigned char TimerFlag =0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}
void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
	
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
unsigned char CU;
unsigned char button;
void tick(){
	static unsigned char score = 5;
	switch(state){//transitions
		case start:
			PORTB = 0x00;
			state = d1;
			break;

		case d1:
			if(button == 0x01){
				if(score > 0){
					score--;
				}
				else{
					score = 0;
				}

				state = wait;
			}
			else{
				state = d2;
			}
			CU = 1;
			break;

		case d3:
			if(button == 0x01){
				if(score > 0){
					score--;
				}
				else{
					score = 0;
				}
				state = wait;
			}
			else{
				state = d2;
			}
			CU = 0;
			break;

		case d2:
			if(button == 0x01){
				if(score < 9){
					score++;
				}else{
					score = 9;
				}

				state = wait;
			}	
			else{
				if(CU == 0){
					state = d1;
				}
				else if(CU == 1){
					state = d3;
				}
	
			}
			break;

		case wait:
			if(button == 0x01){
				state = wait;
			}
			else{
				state = waitR;
			}
			break;

		case waitR:
			if(score >= 9){
				state = victory;
			}
			else if(button == 0x01){
				state = restart;
			}
			else{
				state = waitR;
			}
			break;
		
		case victory:
			if(button == 0x01){
				state = restart;
			}	
			else{
				state = victory;
			}
			break;

		case restart:
			if(button == 0x01){
				state = restart;
			}
			else{
				state = start;
			}
			break;

		default:
			state = start;
			break;
	}
	switch(state){//actions
		case start:
			LCD_DisplayString(1, "Score:");
			break;
		case d1:
			PORTB = 0x01;
			LCD_Cursor(7);
			LCD_WriteData(score + '0');
			break;
		case d2:
			PORTB = 0x02;
			LCD_Cursor(7);
                        LCD_WriteData(score + '0');

			break;
		case d3:
			PORTB = 0x04;
			LCD_Cursor(7);
                        LCD_WriteData(score + '0');

			break;

		case wait:
			LCD_Cursor(7);
			LCD_WriteData(score + '0');
			break;
		case waitR:
			break;
		case victory:
			LCD_DisplayString(1, "VICTORY");
			score = 0;
		case restart:
			break;
		
		default:
			break;
	}

}


int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	LCD_init();
	state = start;
	TimerSet(500);
	TimerOn();
	LCD_DisplayString(1, "Score:");	
    	while(1){
		button = ~PINA & 0x01;
		tick();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
	return 1;
}
