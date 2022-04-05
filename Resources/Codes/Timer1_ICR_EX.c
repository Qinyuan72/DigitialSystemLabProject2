/*
 * Timer1_ICR_EX.c
 *
 * Created: 28/02/2020 11:44:59
 * Author : ciaran.macnamee
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t timecount;				// Extends TCNT of Timer1
uint16_t start_edge, end_edge;		// globals for times. Why global vars?
unsigned long clocks;					// count of clocks in the pulse

int main(void)
{
		
	// set PORTD to all outputs
	DDRD = 0xFF;
	
	TCCR1A = 0; // Disable all o/p waveforms
	TCCR1B=((1<<ICNC1) | (1<<ICES1) | (2<<CS10));		// Noise Canceller on, Rising Edge, CLK/8 (2MHz) T1 source
	TIMSK1 = ((1<<ICIE1) | (1 << TOIE1));				// Enable T1 OVF, T1 Input Cap Interrupt
	
	sei();
	start_edge = 0;
	timecount = 0;							// Initialise timecount (omitted earlier)
	
    while(1)
		;	// Do nothing Loop
		
	return 1;
}

ISR(TIMER1_OVF_vect)	// See iom169pa.h
{
	++timecount;		// Inc overflow counter on interrupt
}

ISR(TIMER1_CAPT_vect)
{
	end_edge = ICR1;	// The C compiler reads two 8bit regs for us
	
	clocks = ((unsigned long)timecount * 65536) + (unsigned long)end_edge - (unsigned long)start_edge;
	PORTD = (clocks/2000);		// Result in milliseconds
	
	timecount = 0;					// Clear timecount for next time around
	start_edge = end_edge;			// We're counting rising to rising, so this end = next start
									// Save its time for next time through here
	
}