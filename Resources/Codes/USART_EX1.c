/*
 * USART_EX1.c
 *
 * Updated: 02/11/2020 13:40:54
 *
 * This program assumes a clock frequency of 16 MHz
 * Baud rate of 9600 used
 * Based on Barnett Example Fig 2-40
 *
 * Created: 17/10/2011 01:09:43
 *  Author: Ciaran MacNamee
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char qcntr = 0,sndcntr = 0;   /*indexes into the queue*/
unsigned char queue[50];       /*character queue*/ 

/*message arrays*/
char msg1[] = {"That was an a."};
char msg2[] = {"That was a b, not an a."};
char msg3[] = {"That was neither b nor a."};

void Init_USART(void)
{
	UCSR0A	= 0x00;				/* Not necessary  */
	
	UCSR0B	= (1<<RXEN0)|(1<<TXEN0)|(1<<TXCIE0);	  /*enable receiver, transmitter and transmit interrupt, 0x58;*/
	UBRR0	= 103;  /*baud rate = 9600, USART 2X = 0 so UBRR0 = ((16*10^6)/(16*9600))-1 = 103.167, rounded to 103 */
	
	/* NB: the default state of UCSR0C is 0b00000110; which selects 8 bit, no parity, 1 stop bit */
	/* Don't be tempted to set it to all zeros - you will select 5 bit data word */
}
   
/*this function loads the queue and */
/*starts the sending process*/
void sendmsg (char *s)
{
   qcntr = 0;    /*preset indices*/
   sndcntr = 1;  /*set to one because first character already sent*/
   queue[qcntr++] = 0x0d;   /*put CRLF into the queue first*/
   queue[qcntr++] = 0x0a;
   while (*s) 
      queue[qcntr++] = *s++;   /*put characters into queue*/
   UDR0 = queue[0];  /*send first character to start process*/
}

int main(void)
{  
   char ch;  /* character variable for received character*/ 
   Init_USART();
   sei(); /*global interrupt enable */
   while (1)         
   {
      if (UCSR0A & (1<<RXC0)) /*check for character received*/
      {
         ch = UDR0;    /*get character sent from PC*/
         switch (ch)
         {
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

/*this interrupt occurs whenever the */
/*USART has completed sending a character*/
ISR(USART_TX_vect)
{
   /*send next character and increment index*/
   if (qcntr != sndcntr)  
      UDR0 = queue[sndcntr++]; 
} 

