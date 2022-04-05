/*
 * ADC_FR_EX.c
 *
 * ADC Example using Free Running Mode
 * VREF = AVCC - simplest setup for us to use
 * If ADC reading greater than a Threshold Voltage set PORTD, bit 7
 * Otherwise clear this bit
 * Created: 16/01/2020 16:38:00, this updated version uses uints as in C99
 * Author : ciaran.macnamee
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#define THRESHOLD_VOLTAGE 600  /* This is an arbitrary choice here  */



int main(void)
{
	DDRD = (1 << DDD7);
	PORTD = 0;  /* Initialise PORTD and Clear Bit 7  */
	
	ADMUX = ((1<<REFS0) | (0 << ADLAR) | (0<<MUX0));  /* AVCC selected for VREF, ADC0 as ADC input  */
	ADCSRA = ((1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADIE)|(5<<ADPS0));
										/* Enable ADC, Start Conversion, Auto Trigger enabled, 
										   Interrupt enabled, Prescale = 32  */
	ADCSRB = (0<<ADTS0); /* Select AutoTrigger Source to Free Running Mode 
						    Strictly speaking - this is already 0, so we could omit the write to
						    ADCSRB, but this is included here so the intent is clear */
	
	sei(); //global interrupt enable
	
    while(1)
		;  /* Once again an empty loop  */
}

ISR (ADC_vect)	/* handles ADC interrupts  */
{
	uint16_t adc_reading;	/* Defined as a local here. Would any other scope be ok?  */
	
	adc_reading = ADC;   /* ADC is in Free Running Mode - you don't have to set up anything for 
						    the next conversion */

	if (adc_reading > THRESHOLD_VOLTAGE)
		PORTD = PORTD | (1<<PORTD7);	/* Sets bit 7 leaves other bits as were  */
	else
		PORTD = PORTD & (~(1<<PORTD7));
}