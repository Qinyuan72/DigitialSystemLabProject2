/*
 * ADC_FR_EX2.c
 *
 * ADC Example using Free Running Mode
 * VREF = AVCC - simplest setup for us to use
 * 
 * Created: 3/11/2020 16:38:00, this program uses the background to display ADC, 
 * 14/02/2022, int8_t used for flag
 * Author : ciaran.macnamee
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#define THRESHOLD_VOLTAGE 512  /* This is the reading for 2.5V  */


#define ONE_EIGHTH_VOLTAGE 128
#define ONE_QUARTER_VOLTAGE 256
#define THREE_EIGHTH_VOLTAGE 384
#define HALF_VOLTAGE 512
#define FIVE_EIGHTH_VOLTAGE 640
#define THREE_QUARTER_VOLTAGE 768
#define SEVEN_EIGHTH_VOLTAGE 896

volatile int8_t new_adc_data_flag;
volatile uint16_t adc_reading;	/* Defined as a global here, because it's shared with main  */

int main(void)
{
	new_adc_data_flag = 0;
	
	DDRD =  0b11111111; /*(1 << DDD7)|(1 << DDD1)|(1 << DDD0);*/   /* If not set to outputs, bits 1 & 0 will turn on on the LED display */
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
		{
			if (new_adc_data_flag)
			{
				if (adc_reading < ONE_EIGHTH_VOLTAGE)
					PORTD = 0b00000000;
				else if (adc_reading < ONE_QUARTER_VOLTAGE)
					PORTD = 0b00000001;
				else if (adc_reading < THREE_EIGHTH_VOLTAGE)
					PORTD = 0b00000011;
				else if (adc_reading < HALF_VOLTAGE)
					PORTD = 0b00000111;
				else
					PORTD = 0b11111111;
				
				new_adc_data_flag = 0;
			}
			
		}
}

ISR (ADC_vect)	/* handles ADC interrupts  */
{
	adc_reading = ADC;   /* ADC is in Free Running Mode - you don't have to set up anything for 
						    the next conversion */

	new_adc_data_flag = 1;
}