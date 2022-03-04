/*
 * Lab3 part F
 *
 * Created: 3/1/2022 1:23:30 PM
 * Author : Ross Miller
 */ 

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"
#include <stdlib.h>
#include <stdio.h>

volatile int mode = 1;
volatile unsigned long overf;
volatile int tr;
volatile unsigned long edge = 0;
char String[25];
void Initialize(){
	cli();
	DDRD |= (1<<DDD5);
	PORTD |= (1<<PORTD5);
	TCCR0B |= (1<<CS00);
	TCCR0B |= (1<<CS01);
	TCCR0B &= ~(1<<CS02);
	TCCR0A |= (1<<WGM00);
	TCCR0B |= (1<<WGM02);
	TCCR0A |= (1<<COM0A0);
	DDRB &= ~(1<<DDB0); 
	PORTB &= ~(1<<PORTB0);
	DDRD |= (1<<DDD7);
	PORTD &= ~(1<<PORTD7);
	TCCR1B |= (1<<CS10);
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12);
	TCCR1B |= (1<<ICES1);
	TIMSK1 |= (1<<ICIE1);
	TIMSK1 |= (1<<TOIE1);
	OCR1A = 15000;
	TCCR0A |= (1<<COM0B0);
	TCCR0A |= (1<<COM0B1);
	overf = 0;
	trig = 0;
	DDRB &= ~(1<<DDB1);
	PORTB &= ~(1<<PORTB1);
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<PCINT1);
	PRR &= ~(1<<PRADC);
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);
	ADCSRA |= (1<<ADPS0);
	ADCSRA |= (1<<ADPS1);
	ADCSRA |= (1<<ADPS2);
	ADMUX &= ~(1<<MUX0);
	ADMUX &= ~(1<<MUX1);
	ADMUX &= ~(1<<MUX2);
	ADMUX &= ~(1<<MUX3);
	ADCSRA |= (1<<ADATE);
	ADCSRB &= ~(1<<ADTS0);
	ADCSRB &= ~(1<<ADTS1);
	ADCSRB &= ~(1<<ADTS2);
	DIDR0 |= (1<<ADC0D);
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
	sei();
}
ISR(PCINT0_vect){
	if(PINB & (1<<PINB1)){ 
		mode = 1;
	}
	else{
		mode = 0;
	}
}
ISR(TIMER1_COMPA_vect){
	unsigned long a = OCR1A + 15000;
	PORTD &= ~(1<<PORTD7);
	TCCR1B |= (1<<ICES1);
	overf = 0;
	tr = 0;
	if(a < 65535){
		a=a;
	} else {
		a -= 65535;
	}
}
ISR(TIMER1_OVF_vect){
	overf++;
}
ISR(TIMER1_CAPT_vect) {
	unsigned long inputCapt = ICR1;
	if(tr == 1) {
		TCCR1B &= ~(1<<ICES1);
		tr++;
		} else {
		unsigned long period = (unsigned long)(overf * 65535) + inputCapt - edge;  
		unsigned long distance = (period * 64 * 170 * 100) / ( F_CPU );  
		sprintf(String, "distance is %d cm \n", distance);
		UART_putstring(String);
		sprintf(String, "In mode %d\n", mode);
		UART_putstring(String);
		if (mode == 1){
			if (distance <= 25) OCR0A = 60;
			else if (distance > 25 && distance <= 50) OCR0A = 53;
			else if (distance > 75 && distance <= 100) OCR0A = 48;
			else if (distance > 125 && distance <= 150) OCR0A = 45;
			else if (distance > 175 && distance <= 200) OCR0A = 40;
			else if (distance > 225 && distance <= 250) OCR0A = 36;
			else if (distance > 275 &&distance <= 300) OCR0A = 32;
			else OCR0A = 30;
			} else {
		OCR0A = (unsigned int)((-38/100)*distance)+59.7;  
		}
		tr = 0;
		TCCR1B |= (1<<ICES1);
	}
	unsigned long a = ICR1 + 15000;
	OCR1A = a < 65535 ? a: a - 65535;
	overf = 0;
	edge = inputCapt;
	if(ADC<100){
		OCR0B = OCR0A*.55;
	}
	else if(ADC >= 100 && ADC <200){
		OCR0B = OCR0A*.50;
	}
	else if(ADC >= 200 && ADC <300){
		OCR0B = OCR0A*.45;
	}
	else if(ADC >= 300 && ADC <400){
		OCR0B = OCR0A*.4;
	}
	else if(ADC >= 400 && ADC <500){
		OCR0B = OCR0A*.35;
	}
	else if(ADC >= 500 && ADC <600){
		OCR0B = OCR0A*.3;
	}
	else if(ADC >= 600 && ADC <700){
		OCR0B = OCR0A*.25;
	}
	else if(ADC >= 700 && ADC <800){
		OCR0B = OCR0A*.2;
	}
	else if(ADC >= 800 && ADC <900){
		OCR0B = OCR0A*.15;
	}
	else if(ADC >= 900 && ADC <1000){
		OCR0B = OCR0A*.1;
	}
	else{
		OCR0B = OCR0A*.05;
	}
	
}

int main(void)
{
	Initialize();
	UART_init(BAUD_PRESCALER);
	while (1)
	{
		if (tr == 0) {
			PORTD |= (1<<PORTD7);
			_delay_us(10);
			PORTD &= ~(1<<PORTD7);
			tr = 1;
		}
	}
}

