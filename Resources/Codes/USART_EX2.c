/*
 * UART_EX2.c
 * Updated 13 Nov 2020
 *
 * This program assumes a clock frequency of 8MHz
 * Baud rate of 9600 used to keep dowm error
 * Based on Barnett Example Fig 2-40
 *
 * This program uses UDRIE0 (UDR0 Empty Interrupts
 *
 * Created: 17/10/2011 01:09:43
 *  Author: Ciaran MacNamee
 */ 
void sendmsg (char *s);
void init_USART(void);

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char qcntr = 0,sndcntr = 0;   /*indexes into the que*/
unsigned char queue[50];       /*character queue*/

/*message arrays*/
char msg1[] = {"That was an a or an A."};
char msg2[] = {"That was a b, not an a."};
char msg3[] = {"That was neither b nor a."};




int main(void)
{
	char ch;  /* character variable for received character*/
	
	init_USART();
	

	sei(); /*global interrupt enable */

	while (1)
	{
		if (UCSR0A & (1<<RXC0)) /*check for character received*/
		{
			ch = UDR0;    /*get character sent from PC*/
			switch (ch)
			{
				case 'f':
				case 'F':
					PORTB = PORTB | 0b00010000;
					break;
				case 'A':
				case 'a':
					sendmsg(msg1); /*send first message*/
				break;
				case 'b':
					sendmsg(msg2); /*send second message*/
				break;
				default:
					sendmsg(msg3); /*send default message*/
			}
		}
	}
	return 1;
}

void init_USART()
{
	UCSR0A	= 0x00;
	UCSR0B	= (1<<RXEN0) | (1<<TXEN0) ;  /*enable receiver, transmitter and NO transmit interrupt*/
										/* | (1<<TXC0) */
	UBRR0	= 103;  /*baud rate = 9600, USART 2X = 0 so UBRR0 = ((16*10^6)/(16*9600))-1 = 103.167, rounded to 103 */
	
	/* NB: the default state of UCSR0C is 0b00000110; which selects 8 bit, no parity, 1 stop bit */
	/* Don't be tempted to set it to all zeros - you will select 5 bit data word */
}


/************************************************************************************/
/* USART sendmsg function															*/
/*this function loads the queue and													*/
/*starts the sending process														*/
/************************************************************************************/

void sendmsg (char *s)
{
	qcntr = 0;    /*preset indices*/
	sndcntr = 1;  /*set to one because first character already sent*/
	
	queue[qcntr++] = 0x0d;   /*put CRLF into the queue first*/
	queue[qcntr++] = 0x0a;
	while (*s)
		queue[qcntr++] = *s++;   /*put characters into queue*/
		

	UDR0 = queue[0];  /*send first character to start process*/
	UCSR0B	= (1<<RXEN0) | (1<<TXEN0) | (1<<UDRIE0);  /* Now enable UDRIE0 because we have something to send */
}

/********************************************************************************/
/* Interrupt Service Routines													*/
/********************************************************************************/

/*this interrupt occurs whenever the UDR0 register is empty */


ISR(USART_UDRE_vect)
{
	/*send next character and increment index*/
	if (qcntr != sndcntr)
		UDR0 = queue[sndcntr++];
	else
		UCSR0B	= (1<<RXEN0) | (1<<TXEN0) ;  /* Turn off UDRIE0 */
	
	/* In the else case, we turn off UDRIE0, because we have no more data to send */
	/* If the interrupt is enabled, we will get UDRE interrupts constantly with no data to send */
		
}