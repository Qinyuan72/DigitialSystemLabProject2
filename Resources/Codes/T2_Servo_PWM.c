/*
 * T2_Servo_PWM_Example.c
 *
 * Created: 15/03/2019 10:58:36
 * Author : ciaran.macnamee
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#define START_POSITION 15
#define NO_OF_STEPS 16

#define DELAY_ONE_SEC 1000000

int main(void)
{
	unsigned char servo_num;
	
	DDRB = 0b00001000;		/* Bit 3 output, OC2A output rest inputs  */
	DDRD 
	TCCR2B = (7<<CS20);		/* use clk/1024 as TC2 clock source */
	TCCR2A = ((2<<COM2A0)|(2<<COM2B0)|(3<<WGM20)); /* Enable Fast PWM Mode, TOP = 0xff, Enable OC2A set on match while downcounting */
	
	OCR2A = START_POSITION;					/* Initial default value  */
	_delay_us(DELAY_ONE_SEC);
	
	while(1)
	{
		for (servo_num = START_POSITION; servo_num < (START_POSITION + NO_OF_STEPS); servo_num++)
		{
			OCR2A = servo_num;
			_delay_us(DELAY_ONE_SEC);
		}
		
		for (servo_num = (START_POSITION + NO_OF_STEPS -1); servo_num >= START_POSITION; servo_num--)
		{
			OCR2A = servo_num;
			_delay_us(DELAY_ONE_SEC);
		}
		
	}
}