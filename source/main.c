/*	Author: jbui019
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

volatile unsigned char TimerFlag = 0;
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

enum States {Start, INIT, INC, DEC, WAIT, ZERO}state;

void tick(){
	static unsigned char i = 0;
	switch(state) // transitions
	{ 
		case Start:
			LCD_Cursor(1);
			LCD_WriteData('0');
			state = INIT; 
			break;
		
		
		
		case INIT:
			if((~PINA & 0x01) == 0x01){
				state = INC; 
			}
			else if((~PINA & 0x02) == 0x02){
				state = DEC; 
			}
			else if((~PINA & 0x03) == 0x03){
				state = ZERO; 
			}
			else{
				state = INIT; 
			}
			break;
		
		
		case INC:
			state = WAIT; 
			break;
		
		
		case DEC:
			state = WAIT; 
			break;
		
		
		case WAIT:
		
			if(((~PINA & 0x03) == 0x01) || ((~PINA & 0x03) == 0x02)){
				state = WAIT;
			}
			else if((~PINA & 0x03) == 0x03){
				state = ZERO; 
			}
			else{
				state = INIT;
			}
			break;
		
		
		case ZERO:
			if (((~PINA & 0x03) == 0x01) || ((~PINA & 0x03) == 0x02)) {
				state = ZERO; 
			}
			else{
				state = INIT; 
			}
		
			break;
		
		default:
			state = Start;
			break;
	}
	
	switch(state) //state actions
	{
		case Start:
			break;
			
		case INIT:
			break;
			
		case INC:
			if (i < 0x09) {
				i += 1;
			}

			else {
				i = i;
			}
			LCD_Cursor(1);
			LCD_WriteData(i + '0');
			break;
		
		
		case DEC:
			if (i > 0x00) {
				i -= 1;
			}

			else {
				i = i;
			}
			LCD_Cursor(1);
			LCD_WriteData(i +'0');
			break;

		
		case WAIT:
			break;
			
		case ZERO:
			i = 0;
			LCD_Cursor(1);
			LCD_WriteData(i +'0');
			break;
	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	LCD_init();
	TimerSet(1);
	TimerOn();
	
	LCD_DisplayString(1,"i");
    /* Insert your solution below */
    while (1) {
	tick();	
	while(!TimerFlag){}
	TimerFlag = 0;
    }
  
}
