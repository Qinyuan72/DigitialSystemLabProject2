/*
 * USART_EX1_2020.c
 *
 * Updated: 25/11/2020 11:40:54
 *
 * This program assumes a clock frequency of 16 MHz
 * Baud rate of 9600 used
 * Based on Barnett Example Fig 2-40
 * This version uses sprintf
 *
 * Created: 17/10/2011 01:09:43
 *  Author: Ciaran MacNamee
 */

#include <stdio.h> /* Required for sprintf */
#include <avr/io.h>
#include <avr/interrupt.h>
#define DLY_2_ms 61
#define COUNT_FOR_10ms 10

volatile int8_t new_adc_data_flag; // ISR take action flag
volatile int8_t new_input_capture_data_flag;
volatile int8_t new_timer_data_flag;

unsigned int adc_reading; /* Defined as a global here, because it's shared with main  */
int adc_reading_signed;
int t_period;
int t_period_high;
int t_period_low;

uint16_t timecount0;
uint16_t timecount1;

volatile uint8_t servo_flag;
volatile uint8_t servo_direction_flag;

volatile uint8_t ADC_report_flag;
volatile uint8_t TIMER_report_flag;
volatile int8_t ADC_select_flag;
unsigned char qcntr = 0, sndcntr = 0; /*indexes into the queue*/
unsigned char queue[60];			  /*character queue*/
char buffer[60];					  /* similar size to queue */
unsigned int ADC_VCF;				  // ADC_Votalge_Covertion_Factor 4.8828125
unsigned int ADC_VR;				  // ADC_Votalge_Reading

void initialisationSection()
{
	DDRD = (1 << DDD3) | (1 << DDD4) | (1 << DDD6) | (1 << DDD7); // Set PORTD bit 3, 4, 6 and 7 to outputs.
	DDRB = (1 << DDB3) | (1 << DDB1);							  // set PORTB Bit 3 to output

	TIMSK0 = (1 << TOIE0);							  // Timer/Counter 0 Overflow interrupts.
	TCCR0B = (1 << CS02) | (0 << CS01) | (1 << CS00); // CLKIO/1024
	TCNT0 = 61;										  // 256-61 = 195 & 195*64us = 12.48ms= 12.5ms

	TIMSK1 = (1 << ICIE1) | (1 << TOIE1);											// Timer/Counter 1 set for input capture and rollover interrupts.
	TCCR1B = (0 << CS12) | (1 << CS11) | (0 << CS10) | (0 << ICNC1) | (0 << ICES1); // set input capture for falling edge, noise canceler off, CLKIO/8
	// TCCR0A = 0;

	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20) | (0 << WGM22);									  // CLKIO/1024, Fast PWM 0xFF Timer/Counter 2
	TCCR2A = (1 << WGM21) | (1 << WGM20) | (1 << COM2B1) | (0 << COM2B0) | (1 << COM2A1) | (0 << COM2A0); // Fast PWM 0xFF Timer/Counter 2, Clear OC2B on Compare Match, set OC2B at BOTTOM
}
void Init_USART(void)
{
	UCSR0A = 0x00; /* Not necessary  */

	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << TXCIE0); /*enable receiver, transmitter and transmit interrupt, 0x58;*/
	UBRR0 = 103;										  /*baud rate = 9600, USART 2X = 0 so UBRR0 = ((16*10^6)/(16*9600))-1 = 103.167, rounded to 103 */

	/* NB: the default state of UCSR0C is 0b00000110; which selects 8 bit, no parity, 1 stop bit */
	/* Don't be tempted to set it to all zeros - you will select 5 bit data word */
}

/*this function loads the queue and */
/*starts the sending process*/
void sendmsg(char *s)
{
	qcntr = 0;			   /*preset indices*/
	sndcntr = 1;		   /*set to one because first character already sent*/
	queue[qcntr++] = 0x0d; /*put CRLF into the queue first*/
	queue[qcntr++] = 0x0a;
	while (*s)
		queue[qcntr++] = *s++; /*put characters into queue*/
	UDR0 = queue[0];		   /*send first character to start process*/
}

void ADC_initialization()
{
	new_adc_data_flag = 0;
	ADMUX = ((1 << REFS0) | (0 << ADLAR) | (1 << MUX1));							  /* AVCC selected for VREF, ADC0 as ADC input  */
	ADCSRA = ((1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (7 << ADPS0)); // Enable ADC, Start Conversion, Auto Trigger enabled, Interrupt enabled, Prescale = 128
	ADCSRB = (1 << ADTS2);
}

int main(void)
{
	char ch; /* character variable for received character*/

	OCR2A = 15; // Set an initial value so it will be always in range.

	ADC_initialization();
	Init_USART();
	initialisationSection();
	sei(); /*global interrupt enable */
	while (1)
	{
		if (UCSR0A & (1 << RXC0)) /*check for character received*/
		{
			ch = UDR0; /*get character sent from PC*/
			chooseChar(ch, buffer);
		}
		if (new_input_capture_data_flag)
		{
			if (TIMER_report_flag)
			{

				if (new_timer_data_flag)
				{
					if (qcntr == sndcntr)
					{
						sprintf(buffer, "The period of the 555 Timer in microseconds %i", t_period);
						sendmsg(buffer); /*send first message*/
						new_timer_data_flag = 0;
					}
				}
			}
			else if (ADC_report_flag)
			{
				if (new_adc_data_flag)
				{
					if (qcntr == sndcntr)
					{
						sprintf(buffer, "This is the ADC value %i", adc_reading);
						sendmsg(buffer); /*send first message*/
					}
					new_adc_data_flag = 0;
				}
			}
		}
		/*
		if (){
			if the continuous timer value display is selected, report new timer value to the user on the USART
			Clear the New Input Capture data flag
		}
		else Test New ADC data flag to see if new ADC data has been captured.
			if yes{
				if continuous ADC display is selected, report new ADC voltage to the user on the USART
				clear the ADC data flag
			}
		*/
	}
	return 1;
}

void chooseChar(char ch, char *buffer)
{
	ADC_VCF = 5;
	switch (ch)
	{
	case 'f':
	case 'F':
		sprintf(buffer, "Set Direction of motor spin to Forward");
		sendmsg(buffer); /*send first message*/
		PORTB = PORTB & (~(1 << PORTB4));
		break;

	case 'r':
	case 'R':
		sprintf(buffer, "Set Direction of motor spin to Reverse");
		sendmsg(buffer); /*send first message*/
		PORTB = PORTB | ((1 << PORTB4));
		break;

	case 'b':
	case 'B':
		sprintf(buffer, "Turn on Brake");
		sendmsg(buffer); /*send first message*/
		PORTB = PORTB | ((1 << PORTB1));
		break;

	case 'g':
	case 'G':
		sprintf(buffer, "Start Motor");
		sendmsg(buffer); /*send first message*/
		PORTB = PORTB & (~(1 << PORTB1));
		break;

	case '0':
		OCR2B = 0;
		sprintf(buffer, ": Set DC motor speed to %i ", OCR2B);
		sendmsg(buffer); /*send first message*/
		break;

	case '1':
		OCR2B = 25;
		sprintf(buffer, ": Set DC motor speed to %i", OCR2B);
		sendmsg(buffer); /*send first message*/
		break;

	case '2':
		OCR2B = 51;
		sprintf(buffer, ": Set DC motor speed %i %", OCR2B);
		sendmsg(buffer); /*send first message*/
		break;

	case '3':
		OCR2B = 77;
		sprintf(buffer, ": Set DC motor speed %i %", OCR2B);
		sendmsg(buffer); /*send first message*/
		break;

	case '4':
		sprintf(buffer, ": Set DC motor speed %i %", OCR2B);
		sendmsg(buffer); /*send first message*/
		OCR2B = 102;
		break;

	case '5':
		OCR2B = 128;
		sprintf(buffer, ": Set DC motor speed %i", OCR2B);
		sendmsg(buffer); /*send first message*/
		break;

	case '6':
		OCR2B = 153;
		sprintf(buffer, ": Set DC motor speed %i", OCR2B);
		sendmsg(buffer); /*send first message*/
		break;

	case '7':
		OCR2B = 179;
		sprintf(buffer, ": Set DC motor speed %i", OCR2B);
		sendmsg(buffer); /*send first message*/
		break;

	case '8':
		OCR2B = 204;
		sprintf(buffer, ": Set DC motor speed %i", OCR2B);
		sendmsg(buffer); /*send first message*/
		break;

	case '9':
		OCR2B = 230;
		sprintf(buffer, ": Set DC motor speed %i", OCR2B);
		sendmsg(buffer); /*send first message*/
		break;
	case 's':
	case 'S':
		sprintf(buffer, "OCR2B register: %i", OCR2B); // The current value of the OCR2B register is
		sendmsg(buffer);							  /*send first message*/
		break;

	case 't':
	case 'T':
		sprintf(buffer, "555 Timer period %ius", t_period); // The period of the 555 Timer in microseconds %ius
		sendmsg(buffer);									/*send first message*/
		break;

	case 'l':
	case 'L':
		sprintf(buffer, "555 Timer period Low pause %ius", t_period_low);
		sendmsg(buffer); /*send first message*/
		break;

	case 'h':
	case 'H':
		sprintf(buffer, "555 Timer period high pause %ius", t_period_high);
		sendmsg(buffer); /*send first message*/
		break;

	case 'c':
	case 'C':
		sprintf(buffer, "Timer Report Continuously"); // Continuously
		sendmsg(buffer);							  /*send first message*/
		TIMER_report_flag = 1;
		break;

	case 'e':
	case 'E':
		sprintf(buffer, " Stop continuous reporting of Timer input.");
		sendmsg(buffer); /*send first message*/
		TIMER_report_flag = 0;
		break;

	case 'a':
	case 'A':
		sprintf(buffer, "This is the ADC value %i", adc_reading);
		sendmsg(buffer); /*send first message*/
		break;

	case 'v':
	case 'V':
		// adc_reading_signed = (int)adc_reading;
		ADC_VR = adc_reading_signed * ADC_VCF;
		sprintf(buffer, "Report the ADC conversion result in mV. .%i", ADC_VR);
		sendmsg(buffer); /*send first message*/
		break;

	case 'm':
	case 'M':
		sprintf(buffer, "Continuously report the ADC conversion."); // Continuously report the ADC conversion result in mV. You must convert the ADC value to mV.
		sendmsg(buffer);											/*send first message*/
		ADC_report_flag = 1;
		break;

	case 'n':
	case 'N':
		sprintf(buffer, " Stop continuous reporting of ADC input.");
		sendmsg(buffer); /*send first message*/
		ADC_report_flag = 0;
		break;

	case 'w':
	case 'W':
		sprintf(buffer, "Toggle the LED bit 4 at 125ms and move the servomotor to its next position");
		sendmsg(buffer); /*send first message*/
		servo_flag = 1;
		OCR2A = 15;
		break;

	case 'u':
	case 'U':
		sprintf(buffer, "Stop toggling LED bit 4 and stop moving the servomotor");
		sendmsg(buffer); /*send first message*/
		// OCR2A = 30;
		servo_flag = 0;
		break;

	case 'd':
	case 'D':
		sprintf(buffer, " Report to the user the state (in hex) of PORTD outputs.%x", PORTD);
		sendmsg(buffer); /*send first message*/
		break;

	case 'p':
	case 'P':
		sprintf(buffer, ": Report to the user the state (in hex) of PINB inputs.%x", PINB);
		sendmsg(buffer); /*send first message*/
		break;

	case 'i':
	case 'I':
		ADC_select_flag = 1;
		sprintf(buffer, " Select ADC14, Internal 1.1V BG ref as the ADC input. ");
		sendmsg(buffer); /*send first message*/
		break;

	case 'j':
	case 'J':
		ADC_select_flag = 0;
		sprintf(buffer, ": Select ADC2 as the ADC input. ");
		sendmsg(buffer); /*send first message*/
		break;
	}
	new_adc_data_flag = 0;
}
/*this interrupt occurs whenever the */
/*USART has completed sending a character*/
ISR(USART_TX_vect)
{
	/*send next character and increment index*/
	if (qcntr != sndcntr)
		UDR0 = queue[sndcntr++];
}

ISR(ADC_vect) /* handles ADC interrupts  */
{
	if (ADC_select_flag)
	{
		ADMUX = (0 << MUX0)| (0 << MUX1)| (1 << MUX2); /* AVCC selected for VREF, ADC0 as ADC input  */
	}
	else
	{
		ADMUX = (0 << MUX0)| (1 << MUX1)| (0 << MUX2); // select 2/4 ??Q.
	}

	adc_reading = ADC; /* ADC is in Free Running Mode - you don't have to set up anything for the next conversion */
	adc_reading_signed = (int)adc_reading;

	if (ADC > 717)
	{ // work out the threshold value needed
		PORTD = PORTD | (1 << PORTD6);
	}
	else
		PORTD = PORTD & (~(1 << PORTD6));
	new_adc_data_flag = 1;
}

ISR(TIMER0_OVF_vect)
{
	static uint8_t servo_counter;
	static uint8_t servo_count_down;

	TCNT0 = DLY_2_ms; /*  TCNT0 needs to be set to the start point each time        */
	++timecount0;	  /* count the number of times the interrupt has been reached      */
	if (timecount0 >= COUNT_FOR_10ms)
	{ /* 10 x 12.48ms = 124.8ms */
		timecount0 = 0;

		if (servo_flag)
		{
			PORTD = PORTD ^ (1 << PORTD4);
			if (servo_counter > 35) // 35 & 12 is the limiter controlling OAC2A
				servo_direction_flag = 1;
			if (servo_counter < 12)
				servo_direction_flag = 0;
			if (servo_direction_flag)
				OCR2A = servo_counter--;
			else
				OCR2A = servo_counter++;
		}
	}
}

ISR(TIMER1_OVF_vect)
{
	// TCNT0 = 61;               /*  TCNT0 needs to be set to the start point each time        */
	++timecount1; /* count the number of times the interrupt has been reached      */
}
ISR(TIMER1_CAPT_vect)
{
	static uint16_t start_edge, end_edge; /* Use static local variables here */
	unsigned long Time_Period;
	unsigned long clocks; /* count of clocks in the pulse - not needed outside the
	ISR, so make it local */
	end_edge = ICR1;
	uint32_t Time_Period_Low, Time_Period_High;
	if (TCCR1B & 1 << ICES1)
	{
		clocks = ((unsigned long)timecount1 * 65536) + (unsigned long)end_edge - (unsigned long)start_edge;
		Time_Period_Low = (clocks / 2); /* Result is in microseconds */
		TCCR1B = TCCR1B & 0b10111111;	// TCCR1B =TCCR1B | (~(1<<ICES1))
	}
	else
	{
		clocks = ((unsigned long)timecount1 * 65536) + (unsigned long)end_edge - (unsigned long)start_edge;
		Time_Period_High = (clocks / 2); /* Result is in microseconds */
		TCCR1B = TCCR1B | 0b01000000;	 // TCCR1B =TCCR1B | (~(1<<ICES1))
	}
	Time_Period = Time_Period_High + Time_Period_Low;
	t_period = Time_Period;
	t_period_high = Time_Period_High;
	t_period_low = Time_Period_Low;

	timecount1 = 0; // Clear timecount for next time around
	new_input_capture_data_flag = 1;
	start_edge = end_edge; // We're counting rising to rising, so this end = next start
	// Save its time for next time through here
	if (Time_Period > 150000)
	{
		PORTD = PORTD | (1 << PORTD6);
	}
	else
	{
		PORTD = PORTD & (~(1 << PORTD6));
	}
	new_timer_data_flag = 1;
}