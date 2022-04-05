/*
 * Timer0_EX.c
 *
 * Created: 16/01/2020 16:44:48
 * Author : ciaran.macnamee
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t timecount0;


int main(void)
{
	//Set PortD to all outputs because LEDs are connected to this PORT
	DDRD = 0xff;	// 0xff = 0b11111111; all ones
	PORTD = 0x0;		// Initialise to all off
	
	timecount0 = 0;		// Initialise the overflow count. Note its scope
	TCCR0B = (5<<CS00);	// Set T0 Source = Clock (16MHz)/1024 and put Timer in Normal mode
	
	TCCR0A = 0;			// Not strictly necessary as these are the reset states but it's good
						// practice to show what you're doing
	TCNT0 = 61;			// Recall: 256-61 = 195 & 195*64us = 12.48ms, approx 12.5ms
	TIMSK0 = (1<<TOIE0);	// Enable Timer 0 interrupt
		
	sei();				// Global interrupt enable (I=1)
	
	
    while(1)
		{}			// Do nothing loop
}

ISR(TIMER0_OVF_vect)
{
	
	
	TCNT0 = 61;		//TCNT0 needs to be set to the start point each time
	++timecount0;	// count the number of times the interrupt has been reached
	
	if (timecount0 >= 40)	// 40 * 12.5ms = 500ms = 0.5s
	{
		PORTD = ~PORTD;		// Toggle all the bits
		timecount0 = 0;		// Restart the overflow counter
	}
}