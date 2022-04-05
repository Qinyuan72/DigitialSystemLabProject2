/*
 * T1_PWMEX.c
 *
 * Created: 23/10/2013 20:11:14
 *  Author: Ciaran
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
	unsigned char last_switch;
	
	DDRB = 0b00000100;		/* Bit 2 output, rest inputs  */
	PORTB = 0b00000011;		/* Enable pull ups on PINB bits 1 & 0 
							   PINB bits 1 & 0 are used with pushbuttons  */
	TCCR1B = (2<<CS10);		/* use clk/8 as TC1 clock source */
	TCCR1A = ((2<<COM1B0)|(1<<WGM10)); /* Enable 8 bit PWM, Enable OC1B set on match while downcounting */
	
	last_switch = PINB & 0x03;  /* Mask out unused bits */
	OCR1B = 25;					/* Initial default value  */
	 
    while(1)
    {
		if ((PINB & 0x03) != last_switch)  /* The IF test is respond to change only */
		{
			last_switch = PINB & 0x03;
			switch (PINB & 0x03)
			{
				case 0: OCR1B = 25;		/* 10% of maxcount  */
				break;
				case 1: OCR1B = 51;		/* 20% of maxcount  */
				break;
				case 2: OCR1B = 102;	/* 30% of maxcount  */
				break;
				case 3: OCR1B = 153;	/* 60% of maxcount  */
				break;
				default: OCR1B = 51;		/* 20% of maxcount  */
				break;
				break;
			}
		}		 
    }
}