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

#include <stdio.h>  /* Required for sprintf */
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char qcntr = 0,sndcntr = 0;   /*indexes into the queue*/
unsigned char queue[50];       /*character queue*/ 



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
   char buffer[60];  /* similar size to queue */
   
   Init_USART();
   sei(); /*global interrupt enable */
   while (1)         
   {
      if (UCSR0A & (1<<RXC0)) /*check for character received*/
      {
         ch = UDR0;    /*get character sent from PC*/
        chooseChar(ch,buffer);
      } 
   }
   return 1; 
} 

void chooseChar(char ch, char *buffer){
	 switch (ch)
	 {
		 case 'f':
		 case 'F':
		 sprintf(buffer, "Set Direction of motor spin to Forward");
		 sendmsg(buffer); /*send first message*/
		 break;
		
		 case 'r':
		 case 'R':
		 sprintf(buffer, "Set Direction of motor spin to Reverse");
		 sendmsg(buffer); /*send first message*/
		 break;
		
		 case 'b':
		 case 'B':
		 sprintf(buffer, "Stop motor irrespective of the motor speed previously selected � ie Turn on Brake");
		 sendmsg(buffer); /*send first message*/
		 break;
		 
		  case 'g':
		  case 'G':
		  sprintf(buffer, "Start Motor");
		  sendmsg(buffer); /*send first message*/
		  break;
		 
		  case '0':
		  case '9':
		  sprintf(buffer, ": Set DC motor speed");
		  sendmsg(buffer); /*send first message*/
		  break; 
		  
		  case 's':
		  case 'S':
		  sprintf(buffer, " Report the current value of the OCR2B register");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 't':
		  case 'T':
		  sprintf(buffer, "Report the period of the 555 Timer in microseconds.");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'l':
		  case 'L':
		  sprintf(buffer, " Report the time taken by the low pulse of the 555 Timer signal in microseconds");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'h':
		  case 'H':
		  sprintf(buffer, " Report the time taken by the high pulse of the 555 Timer signal in microseconds");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'c':
		  case 'C':
		  sprintf(buffer, "Continuously report the Timer input period in microseconds.");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'e':
		  case 'E':
		  sprintf(buffer, " Stop continuous reporting of Timer input.");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'a':
		  case 'A':
		  sprintf(buffer, " Report the ADC conversion result. This is the ADC value.");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'v':
		  case 'V':
		  sprintf(buffer, "Report the ADC conversion result in mV. You must convert the ADC value to mV. You must also display the ADC source.");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'm':
		  case 'M':
		  sprintf(buffer, "Continuously report the ADC conversion result in mV. You must convert the ADC value to mV. You must also display the ADC source.");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'n':
		  case 'N':
		  sprintf(buffer, " Stop continuous reporting of ADC input.");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'w':
		  case 'W':
		  sprintf(buffer, "Toggle the LED bit 4 at 125ms and move the servomotor to its next position");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'u':
		  case 'U':
		  sprintf(buffer, "Stop toggling LED bit 4 and stop moving the servomotor");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'd':
		  case 'D':
		  sprintf(buffer, " Report to the user the state (in hex) of PORTD outputs.");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'p':
		  case 'P':
		  sprintf(buffer, ": Report to the user the state (in hex) of PINB inputs.");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'i':
		  case 'I':
		  sprintf(buffer, " Select ADC14, Internal 1.1V BG ref as the ADC input. This means changing the ADMUX register and the change must be done in the ADC ISR.");
		  sendmsg(buffer); /*send first message*/
		  break;
		  
		  case 'j':
		  case 'J':
		  sprintf(buffer, ": Select ADC2 as the ADC input. This means changing the ADMUX register and the change must be done in the ADC ISR.");
		  sendmsg(buffer); /*send first message*/
		  break;
	 }
}
/*this interrupt occurs whenever the */
/*USART has completed sending a character*/
ISR(USART_TX_vect)
{
   /*send next character and increment index*/
   if (qcntr != sndcntr)  
      UDR0 = queue[sndcntr++]; 
} 
