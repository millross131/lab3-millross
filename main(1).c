/*
 * Lab 3
 *
 * Created: 2/27/2022 3:12:13 PM
 * Author : Ross Miller
 */ 
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"

char String[25];

//char String[25];
volatile int overf = 0;
volatile int trig;
volatile int length = 0;
volatile unsigned long edge = 0;

void Initialize(){
	cli();
	DDRB &= ~(1<<DDB0); 
	PORTB &= ~(1<<PORTB0);
	DDRD |= (1<<DDD7);
	PORTD &= ~(1<<PORTD7);
	TCCR1B |= (1<<CS10);
	TCCR1B |= (1<<CS11);
	TCCR1B |= (1<<ICES1);
	TIMSK1 |= (1<<ICIE1);
	TIMSK1 |= (1<<TOIE1);
	OCR1A = 15000;
	overf = 0;
	trig = 0;
	sei();
	}
	
ISR(TIMER1_CAPT_vect){

	unsigned long inputCapt = ICR1;
	if(trig == 1) {
		TCCR1B &= ~(1<<ICES1);
		trig++;
		} else {
		unsigned long period = (unsigned long)(overf * 65535) + inputCapt - edge;
		unsigned long length = (period * 64 * 170 * 100) / ( F_CPU );
		sprintf(String, "distance is %d\n", length);
		UART_putstring(String);	
		trig = 0;
		TCCR1B |= (1<<ICES1);
	}
	unsigned long a = ICR1 + 15000;
	OCR1A = a < 65535 ? a: a - 65535;
	overf = 0;
	edge = inputCapt;
}
ISR(TIMER1_COMPA_vect){
	unsigned long a = OCR1A + 15000;
	PORTD &= ~(1<<PORTD7);
	TCCR1B |= (1<<ICES1);
	overf = 0;
	trig = 0;
	OCR1A = a < 65535 ? a: a - 65535;
}
ISR(TIMER1_OVF_vect){
	overf++;
}

int main(void)
{
    Initialize();
	UART_init(BAUD_PRESCALER);

    while (1) 
    {
		if (trig == 0) {
			PORTD |= (1<<PORTD7);
			_delay_us(10);
			PORTD &= ~(1<<PORTD7);
			trig = 1;
    }
}

}
