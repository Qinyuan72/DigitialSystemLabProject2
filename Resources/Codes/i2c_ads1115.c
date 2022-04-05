/*
 * i2c_lcd.c
 *
 * Created: 28/03/2017 21:01:04
 * Author : Ciaran
 *
 * Warning: not tested yet!!  
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define  F_CPU	16000000
#include <util/delay.h>

#define SLA 0x48
#define SLA_W (SLA<<1)
#define SLA_R (SLA<<1) | 0x01

#define CONVERSION_REG	0x00
#define CONFIG_REG		0x01

#define START			0x08 
#define MT_SLAW_ACK		0x18
#define MT_DATA_ACK		0x28
#define MT_SLAR_ACK		0x40
#define MR_DATA_ACK		0x50

#define ARBITRARY_VALUE 1000

void init_i2c(void);
void init_ads1115_continuous(void);
void Error_Trap(void);
unsigned int read_ads1115(void);


int main(void)
{
	int adcs1115_reading;
	
	DDRD = (1<<DDD7);			/* MS bit is output  */
	PORTD = 0;					/* Turn it off  */
	init_i2c();
    
	void init_ads1115_continuous(void);		/* Set ads1115 to continuous mode  */
	
	
	
	
    while (1) 
    {
		_delay_ms(2);				/* This is an arbitrary value to save polling for data ready from ads1115 */
		adcs1115_reading = read_ads1115();
		
		if (adcs1115_reading > ARBITRARY_VALUE)
			PORTD |= (1<<PORTD7);
		else
			PORTD &= ~(1<<PORTD7);
    }
}

/********************************************************************************************************/
/* Initialisation functions																				*/
/********************************************************************************************************/

void init_i2c(void)
{
	TWBR = 2;				/* 16/(16+2*TWBR*Prescale) = 16/(16+2*2*16) = 16/80 = 0.2MHz = 200kHz */
	TWSR = (2<<TWPS0);		/* TWPS1:TWPS0=1:0 => /16 as prescale value */
	
	/* Write to TWCR when you want to start transmission */
}


/********************************************************************************************************/
/* ADS1115 initialisation																				*/
/* From ADS1115 data sheet, quickstart:																	*/
/* To write to the configuration register to set the ADS111x to continuous-conversion mode and then		
read the conversion result, send the following bytes in this order:
1. Write to Config register:
– First byte: 0b10010000 (first 7-bit I2C address followed by a low R/W bit)
– Second byte: 0b00000001 (points to Config register)
– Third byte: 0b10000100 (MSB of the Config register to be written)
– Fourth byte: 0b10000011 (LSB of the Config register to be written)									*/
/* Note:																								*/
/* A more general purpose version would have the content of the config register as passed in parameters */
/********************************************************************************************************/


void init_ads1115_continuous()
{
	/* START Command  */
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		/* Issue the Start Command and reset TWINT, also enable TWEN  */
	
	while (!(TWCR &	(1<<TWINT)))				/* Wait for TWINT high to indicate Start sent */
		;
	
	if ((TWSR & 0xF8) != START)					/* All ok, ie no error? then proceed   */
		Error_Trap();
		
	TWDR = SLA_W;								/* Set up the I2C address of the ADS1115 + READ CMD   */
	TWCR = (1<<TWINT) | (1<<TWEN);				/* Now send it out							*/
	
	while (!(TWCR &	(1<<TWINT)))				/* Wait for TWINT high to indicate SLA_R sent */
		;
		
	if ((TWSR & 0xF8) != MT_SLAW_ACK)
		Error_Trap();	
	
	TWDR = CONFIG_REG;						/* This is the command byte in the I2C protocol - in this case it's the pointer reg */
	
	TWCR = (1<<TWINT) | (1<<TWEN);			/* And send it out */
	
	while (!(TWCR & (1<<TWINT)))			/* Wait for TWINT Flag set. Indicates that Pointer Reg has been transmitted, and
		;											ACK/NACK has been received.   */
	
	if ((TWSR & 0xF8) != MT_DATA_ACK)
		Error_Trap();
	
	TWDR =	0b10000100;						/* This is the MSB of the Config Register - see above and ADS1115 quickstart  */
	
	TWCR = (1<<TWINT) | (1<<TWEN);			/* And send it out */
	
	while (!(TWCR & (1<<TWINT)))			/* Wait for TWINT Flag set. Indicates that Pointer Reg has been transmitted, and
		;											ACK/NACK has been received.   */
	
	if ((TWSR & 0xF8) != MT_DATA_ACK)
		Error_Trap();
	
	TWDR =	0b10000011;						/* This is the LSB of the Config Register - see above and ADS1115 quickstart  */
	
	TWCR = (1<<TWINT) | (1<<TWEN);			/* And send it out */
	
	while (!(TWCR & (1<<TWINT)))			/* Wait for TWINT Flag set. Indicates that Pointer Reg has been transmitted, and
		;											ACK/NACK has been received.   */
	
	if ((TWSR & 0xF8) != MT_DATA_ACK)
		Error_Trap();
	
	TWCR = (1<<TWINT)| (1<<TWEN)|(1<<TWSTO);		/* Transmit STOP condition  */
}

void Error_Trap()
{
	while(1)
		;					/* Quick fix - really do something better ! */
}

/**********************************************************************************************************************************/
/* unsigned int read_ads1115()																									  */
/*																																  */
/* Read conversion data from ads1115. Recall this is in continuous mode, so we just read its data. The ADC will always just return */
/* the last good conversion result. So this is simply a set of i2c operations to read the conversion register of the ADS1115	  */
/* See ADS1115 Quickstart:																										  */
/* 2. Write to Address Pointer register:
– First byte: 0b10010000 (first 7-bit I2C address followed by a low R/W bit)
– Second byte: 0b00000000 (points to Conversion register)
3. Read Conversion register:
– First byte: 0b10010001 (first 7-bit I2C address followed by a high R/W bit)
– Second byte: the ADS111x response with the MSB of the Conversion register
– Third byte: the ADS111x response with the LSB of the Conversion register															*/
/**********************************************************************************************************************************/

unsigned int read_ads1115()
{
	unsigned int ads_data;

	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		/* Issue the Start Command and reset TWINT, also enable TWEN  */
	
	while (!(TWCR &	(1<<TWINT)))				/* Wait for TWINT high to indicate Start sent */
		;
	
	if ((TWSR & 0xF8) != START)
		Error_Trap();
		
	TWDR = SLA_W;								/* Set up the I2C address of the ADS1115   */
	TWCR = (1<<TWINT) | (1<<TWEN);				/* Now send it out							*/
	
	
	while (!(TWCR &	(1<<TWINT)))				/* Wait for TWINT Flag set. Indicates that SLA+W has been transmitted, and
		;											ACK/NACK has been received.   */
	
	if ((TWSR & 0xF8) != MT_SLAW_ACK)
		Error_Trap();	
	
	TWDR = CONVERSION_REG;						/* This is the command byte in the I2C protocol - in this case it's the pointer reg */
	
	TWCR = (1<<TWINT) | (1<<TWEN);			/* And send it out */
	
	while (!(TWCR & (1<<TWINT)))			/* Wait for TWINT Flag set. Indicates that Pointer Reg has been transmitted, and
		;											ACK/NACK has been received.   */
	
	if ((TWSR & 0xF8) != MT_DATA_ACK)
		Error_Trap();

/* Now read the conversion register, begin with a read command     */
	
	TWDR = SLA_R;								/* Set up the I2C address of the ADS1115   */
	TWCR = (1<<TWINT) | (1<<TWEN);				/* Now send it out							*/
	
	while (!(TWCR &	(1<<TWINT)))				/* Wait for TWINT Flag set. Indicates that SLA+W has been transmitted, and
		;											ACK/NACK has been received.   */
	
	if ((TWSR & 0xF8) != MT_SLAR_ACK)
		Error_Trap();	
	
		
		
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);		/* We are about to read back so enable ACK generation when data received */
	
	while (!(TWCR & (1<<TWINT)))			/* Wait for TWINT Flag set. Indicates that Pointer Reg has been transmitted, and
		;											ACK/NACK has been received.   */
	
	if ((TWSR & 0xF8) != MR_DATA_ACK)			/* Checks if data received correctly */
		Error_Trap();
		
	ads_data = TWDR;							/* Read the Conversion MSByte  */
	
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);		/* We are about to read back so enable ACK generation when data received */
	
	while (!(TWCR & (1<<TWINT)))			/* Wait for TWINT Flag set. Indicates that Pointer Reg has been transmitted, and
		;											ACK/NACK has been received.   */
	
	if ((TWSR & 0xF8) != MR_DATA_ACK)			/* Checks if data received correctly */
		Error_Trap();
	
	ads_data = (ads_data << 8) | TWDR;
		
	
	TWCR = (1<<TWINT)| (1<<TWEN)|(1<<TWSTO);		/* Transmit STOP condition  */

	return ads_data;	
}