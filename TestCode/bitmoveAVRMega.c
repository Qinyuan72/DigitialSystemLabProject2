/*
 * Timer0_Ex_Static_Var.c
 *
 * Created: 14/02/2022 21:55:44
 * Author : ciaran.macnamee
 */ 

/* This version of Timer0 Example uses a static local variable for timecount0. Note that static local variables are 
 * initialised to 0 by default, and thast they retain their values between invocation */

#include <avr/io.h>
#include <avr/interrupt.h>

    int PORTV = 1;
    int move_leds_counter = 0;
    int derection = 0;


int main(void)
{
	//Set PortD to all outputs because LEDs are connected to this PORT
	DDRD = 0xff;	// 0xff = 0b11111111; all ones
	PORTD = 0x0;		// Initialise to all off
	
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
	static uint16_t timecount0;		/* Automatically initialised to zero on first use. */
	
	TCNT0 = 61;		//TCNT0 needs to be set to the start point each time
	++timecount0;	// count the number of times the interrupt has been reached
	
	if (timecount0 >= 40)	// 40 * 12.5ms = 500ms = 0.5s
	{
		timecount0 = 0;		// Restart the overflow counter
		        ++move_leds_counter;
		        if (derection == 0)
		        {
			        PORTV = PORTV << 1;
					PORTD = PORTV;

		        }

		        else
		        {
			        // move the bit right
			        PORTV = PORTV >> 1;
					PORTD = PORTV;
		        }

		        if (move_leds_counter == 7)
		        {
			        move_leds_counter = 0;
					
			        derection = ~derection;
		        }
		}
}