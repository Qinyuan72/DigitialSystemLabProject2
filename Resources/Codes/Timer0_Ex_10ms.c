/*
 * Timer0_Ex_10ms.c
 *
 * Created: 14/02/2022 22:01:16
 *
 *  Author: Ciaran
 *  Updated to use fixed size integers
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#define DLY_2_ms 131
/* We use 131 because we want Timer0 to count 125 counts, and 256-131 = 125  */
#define COUNT_FOR_10ms	5

uint16_t timecount0;


int main(void)
{
	/* Set PortD to all outputs because LEDs are connected to this PORT */
	DDRD = 0xff;	/* 0xff = 0b11111111; all ones						*/
	PORTD = 0;		/* Initialise to all off							*/
	
	timecount0 = 0;		/* Initialise the overflow count. Note its scope	*/
	TCCR0B = (4<<CS00);	/* Set T0 Source = Clock (16MHz)/256 and put Timer in Normal mode	*/
	
	TCCR0A = 0;			/* Not strictly necessary as these are the reset states but it's good	*/
						/* practice to show what you're doing									*/
	TCNT0 = DLY_2_ms;			/* Recall: 256-131 = 125                    		*/
	TIMSK0 = (1<<TOIE0);		/* Enable Timer 0 interrupt										*/
		
	sei();						/* Global interrupt enable (I=1)								*/
	
	
    while(1)
		;						/* Do nothing loop												*/
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = DLY_2_ms;		/*	TCNT0 needs to be set to the start point each time				*/
	++timecount0;			/* count the number of times the interrupt has been reached			*/
	
	if (timecount0 >= COUNT_FOR_10ms)	/* 5 * 2ms = 10ms									*/
	{
		PORTD = ~PORTD;		/* Toggle all the bits							*/
		timecount0 = 0;		/* Restart the overflow counter					*/
	}
}

