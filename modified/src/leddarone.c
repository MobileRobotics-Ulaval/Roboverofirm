

#include "table.h"
#include "return.h"

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_can.h"
#include "lpc17xx_pwm.h"


unsigned char buffer[1024];
size_t lenBuff = 0;

int writeRegisterLeddar(uint8_t address_i2c, uint8_t reg, uint8_t value){
	I2C_M_SETUP_Type setup;
	Status           result;
	uint8_t          transmit_buffer[2];
	uint8_t          receive_buffer;

	setup.sl_addr7bit         = address_i2c >> 1;
	setup.retransmissions_max = MAX_ST_I2C_RETRANSMISSIONS;

	/* Reg Address*/
	// The address format is XAAACCX where X is unused, A is the reg address and C is unused
	transmit_buffer[0] = reg << 3; 
	/* Value */
	transmit_buffer[1] = value;

	setup.tx_data   = transmit_buffer;
	setup.tx_length = 2;
	setup.rx_data   = &receive_buffer;
	setup.rx_length = 0;

	result = I2C_MasterTransferData(i2cextern, &setup, I2C_TRANSFER_POLLING);
	if(result == ERROR)
		return 1;

	return 0;
}

int writeMultiDataRegisterLeddar(uint8_t address_i2c, uint8_t reg, unsigned char *aData, int aLength ){
	I2C_M_SETUP_Type setup;
	Status           result;
	uint8_t          transmit_buffer[1 + aLength];
	uint8_t          receive_buffer;

	setup.sl_addr7bit         = address_i2c >> 1;
	setup.retransmissions_max = MAX_ST_I2C_RETRANSMISSIONS;

	/* Reg Address*/
	// The address format is XAAACCX where X is unused, A is the reg address and C is unused
	transmit_buffer[0] = reg << 3; 
	/* Copy the uchar array to the buffer */
	memcpy(&(transmit_buffer[1]), aData, aLength);

	setup.tx_data   = transmit_buffer;
	setup.tx_length = 1 + aLength;
	setup.rx_data   = &receive_buffer;
	setup.rx_length = 0;

	result = I2C_MasterTransferData(i2cextern, &setup, I2C_TRANSFER_POLLING);
	if(result == ERROR)
		return 1;

	return 0;
}

uint8_t readRegisterLeddar(uint8_t address_i2c, uint8_t reg){
	I2C_M_SETUP_Type setup;
	Status           result;
	uint8_t          transmit_buffer;
	uint8_t          receive_buffer;

	setup.sl_addr7bit         = address_i2c >> 1;
	setup.retransmissions_max = MAX_ST_I2C_RETRANSMISSIONS;

	/* Reg Address*/
	// The address format is XAAACCX where X is unused, A is the reg address and C is unused
	transmit_buffer = reg << 3;

	setup.tx_data   = &transmit_buffer;
	setup.tx_length = 1;
	setup.rx_data   = &receive_buffer;
	setup.rx_length = 1;

	result = I2C_MasterTransferData(i2cextern, &setup, I2C_TRANSFER_POLLING);
	if(result == ERROR)
		return 1;
	
	return receive_buffer;
}

/*
 * Send a byte via serial data via I2C with the SC16IS750
 */
int _sendSerialOverI2C(uint8_t * args){ 
	char*            arg_ptr;
	unsigned int     index;
	uint32_t         arguments[3];

	for(index = 0; index < 3; index++)
	{
		arg_ptr = strtok(NULL, " ");
		if(arg_ptr == NULL)
		    return 1;

		arguments[index] = strtoul(arg_ptr, NULL, 16);
	}

	return writeRegisterLeddar(arguments[0], arguments[1], arguments[2]);
}

/*
 * Add byte to a buffer before send it
 */
int _addBytesToBuff(uint8_t * args){ 
	char*            arg_ptr;
	unsigned int     index;
	uint32_t         arguments[2];

	for(index = 0; index < 1; index++)
	{
		arg_ptr = strtok(NULL, " ");
		if(arg_ptr == NULL)
		    return 1;

		arguments[index] = strtoul(arg_ptr, NULL, 16);
	}

	buffer[lenBuff++] = arguments[0];

	// Wrap around, should use error handling of some kind
	if(lenBuff >= 1024)
		lenBuff = 0;

	return 0;
}

/*
 * Send buffer via serial via I2C with the SC16IS750
 */
int _sendBufferOverI2C(uint8_t * args){ 
	char*            arg_ptr;
	unsigned int     index;
	uint32_t         arguments[2];

	for(index = 0; index < 1; index++)
	{
		arg_ptr = strtok(NULL, " ");
		if(arg_ptr == NULL)
		    return 1;

		arguments[index] = strtoul(arg_ptr, NULL, 16);
	}

	int res = writeMultiDataRegisterLeddar(arguments[0], LEDDARONE_TRANSMIT_AND_RECEIVE, buffer, lenBuff);
	lenBuff = 0;

	return res;
}

int _receiveSerialOverI2C(uint8_t * args){ 
	char*            arg_ptr;
	unsigned int     index;
	uint32_t         arguments[2];
	uint8_t          receive_buffer;
	char* 	     str;
	//i2c_smbus_write_word_data(file, commande, (argument << 8) | info1) >= 0
	for(index = 0; index < 2; index++)
	{
		arg_ptr = strtok(NULL, " ");
		if(arg_ptr == NULL)
		    return 1;

		arguments[index] = strtoul(arg_ptr, NULL, 16);
	}
	receive_buffer = readRegisterLeddar(arguments[0], arguments[1]);

	sprintf((char*)str, "%x\r\n", receive_buffer);
	writeUSBOutString(str);

	return 0;
}


int _testLeddar(uint8_t * args){ 
	char*            arg_ptr;
	unsigned int     index;
	uint32_t         arguments[1];
	uint8_t          good_parameter;
	uint8_t          received;
	char* 	     	 str;
	//i2c_smbus_write_word_data(file, commande, (argument << 8) | info1) >= 0
	for(index = 0; index < 1; index++)
	{
		arg_ptr = strtok(NULL, " ");
		if(arg_ptr == NULL)
		    return 1;

		arguments[index] = strtoul(arg_ptr, NULL, 16);
	}

	// We save the current parameter of the LCR
	good_parameter = readRegisterLeddar(arguments[0], LEDDARONE_LCR);

	// Write dummy data
	writeRegisterLeddar(arguments[0], LEDDARONE_LCR, 0x4);

	// Check what is writing in the register
	received = readRegisterLeddar(arguments[0], LEDDARONE_LCR);

	if(received != 0x4){
		sprintf((char*)str, "%x\r\n", 1);
	}
	else{
		sprintf((char*)str, "%x\r\n", 0);
	}

	// Rewrite the register to the old value
	writeRegisterLeddar(arguments[0], LEDDARONE_LCR, good_parameter);

	writeUSBOutString(str);

	return 0;
}

int _resetDevice(uint8_t * args){ 
	char*            arg_ptr;
	uint32_t         arguments;

	arg_ptr = strtok(NULL, " ");
	if(arg_ptr == NULL)
	    return 1;

	arguments = strtoul(arg_ptr, NULL, 16);

	uint8_t address_i2c = arguments;

	int temp_iocontrol = readRegisterLeddar(address_i2c, LEDDARONE_IOCONTROL);
	temp_iocontrol |= 0x08;
	writeRegisterLeddar(address_i2c, LEDDARONE_IOCONTROL, temp_iocontrol); 

	return 0;
}

int _setSerialBaudrate(uint8_t * args){ 
	char*            arg_ptr;
	uint32_t         arguments;
	
	arg_ptr = strtok(NULL, " ");
	if(arg_ptr == NULL)
	    return 1;

	arguments = strtoul(arg_ptr, NULL, 16);

	uint8_t address_i2c = arguments;

	int prescaler;
	if ( (readRegisterLeddar(address_i2c, LEDDARONE_MCR)&0x80) == 0) { 
		prescaler = 1;
	} 
	else {
		prescaler = 4;
	}

	uint16_t divisor = (LEDDARONE_CRYSTAL_div_BAUDRATE/prescaler)/16;
	uint8_t temp_lcr = readRegisterLeddar(address_i2c, LEDDARONE_LCR);
	temp_lcr |= 0x80;
	writeRegisterLeddar(address_i2c, LEDDARONE_LCR, temp_lcr);
	//write to DLL
	writeRegisterLeddar(address_i2c, LEDDARONE_DLL, (uint8_t)divisor);
	//write to DLH
	writeRegisterLeddar(address_i2c, LEDDARONE_DLH, (uint8_t)(divisor>>8));
	temp_lcr &= 0x7F;
	writeRegisterLeddar(address_i2c, LEDDARONE_LCR, temp_lcr); 

	// Set Fifo to enable
	uint8_t temp_fcr = readRegisterLeddar(address_i2c, LEDDARONE_LCR);
	temp_fcr |= 0x01;
	writeRegisterLeddar(address_i2c, LEDDARONE_FCR, temp_fcr); 

	return 0;
}


int _setLine(uint8_t * args){ 	
	char*            arg_ptr;
	uint32_t         arguments;
	
	arg_ptr = strtok(NULL, " ");
	if(arg_ptr == NULL)
	    return 1;

	arguments = strtoul(arg_ptr, NULL, 16);

	uint8_t address_i2c = arguments;

	uint8_t temp_lcr;
	temp_lcr = readRegisterLeddar(address_i2c, LEDDARONE_LCR);
	temp_lcr &= 0xC0; //Clear the lower six bit of LCR (LCR[0] to LCR[5])
	temp_lcr |= 0x03; // Set word at 8 bits
	writeRegisterLeddar(address_i2c, LEDDARONE_LCR, temp_lcr);

	return 0;
}

int _getFIFOAvailableData(uint8_t * args){
	char*            arg_ptr;
	unsigned int     index;
	uint32_t         arguments[1];
	char* 	     	 str;
	//i2c_smbus_write_word_data(file, commande, (argument << 8) | info1) >= 0
	for(index = 0; index < 1; index++)
	{
		arg_ptr = strtok(NULL, " ");
		if(arg_ptr == NULL)
		    return 1;

		arguments[index] = strtoul(arg_ptr, NULL, 16);
	}

	// We save the current parameter of the LCR
	return readRegisterLeddar(arguments[0], LEDDARONE_RXLVL);
}
