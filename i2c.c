#include "i2c.h"

#include <avr/io.h>

void I2CInit(void)
{
	TWBR = 18;
	TWSR = (1<<TWPS0);	/* Prescaler = 4, SCL=16000000/(16+2*18*4)=100000Hz */
	TWCR |= (1<<TWEN);	/* Enable TWI */
	return;
}

void I2CStart(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA);	/* Start */
	while (!(TWCR & (1<<TWINT)));				/* Wait for TWINT */
	return;
}

void I2CStop(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);	/* Stop */
	while (TWCR & (1<<TWSTO));					/* Wait for TWSTO */
	return;
}

uint8_t I2CWriteByte(uint8_t data)
{
	TWDR = data;
	TWCR = (1<<TWEN) | (1<<TWINT);	/* Start data transfer */

	while (!(TWCR & (1<<TWINT)));	/* Wait for finish */

	if (TWSR_STA != 0x18 && TWSR_STA != 0x28 && TWSR_STA != 0x40)
		return 1;
	return 0;
}

uint8_t I2CReadByte(uint8_t *data, uint8_t ack)
{
	if (ack)
		TWCR |= (1<<TWEA);
	else
		TWCR &= ~(1<<TWEA);
	TWCR |= (1 << TWINT);

	while (!(TWCR & (1<<TWINT)));	/* Wait for finish */

	if (TWSR_STA != 0x58 && TWSR_STA != 0x50)
		return 1;

	*data = TWDR;
	return 0;
}

uint8_t I2CWrite(uint8_t device, uint8_t address, uint8_t data)
{
	I2CStart();
	if (I2CWriteByte(device))
		return 1;
	if (I2CWriteByte(address))
		return 1;
	if (I2CWriteByte(data))
		return 1;
	I2CStop();
	return 0;
}

uint8_t I2CRead(uint8_t device, uint8_t address, uint8_t *data)
{
	I2CStart();
	if (I2CWriteByte(device))
		return 1;
	if (I2CWriteByte(address))
		return 1;
	I2CStart();
	if (I2CWriteByte(device + 1))
		return 1;
	if (I2CReadByte(data, 0))
		return 1;
	I2CStop();
	return 0;
}