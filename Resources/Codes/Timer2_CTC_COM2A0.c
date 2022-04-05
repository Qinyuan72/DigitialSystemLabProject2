/*
 * Timer2_CTC_COM2A0.c
 *
 * Created: 07/10/2021 15:26:53
 * Author : Ciaran.MacNamee
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>


int main(void)
{
	DDRB = (1<<DDB3);  /* OC2A is shared with PORTB bit 3  */
	
	TCCR2B = (7<<CS20);		/* Set T2 Source = Clock (16MHz)/1024, this would be (5<<CS00) for Timer0 */
	
	TCCR2A = (1<<COM2A0)|(1<<WGM21);			/* Toggle OC2A on compare match, CTC Mode (Clear Timer on Match)  */
	
	OCR2A = 250;			/* 250*64us = 16ms exactly */
	
	/* Note: no interrupts - PORTB bit 3 will toggle at 12.48ms without any extra software, and the CTC ensures that the count will always
	   be up to 250  */
	
    while (1) 
    {
    }
}
