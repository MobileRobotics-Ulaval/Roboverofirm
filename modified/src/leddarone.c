

#include "table.h"
#include "return.h"
#include "base64.h"

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_can.h"
#include "lpc17xx_pwm.h"

#define SERIAL_BUF_LEN 2048
unsigned char serial_buf[SERIAL_BUF_LEN];
size_t len_serial_buf = 0;

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
 * Decode base64 data receive from overo and 
 * add it to a local buffer.
 */
int _addDataToBuff(uint8_t * args){ 
	char*            arg_ptr;
	char* 			 b64_buff = 0;
	unsigned char*   tmp_raw_buff;
	unsigned int     index;
	uint32_t         len_b64;
	size_t         	 len_raw;

	b64_buff = strtok(NULL, " ");
	if(b64_buff == NULL)
	    return 1;

	//sprintf((char*)str, "len:%d str:%s\r\n", strlen(b64_buff), b64_buff);
	//writeUSBOutString(str);

	// Decode base64 to raw binary data
	tmp_raw_buff = base64_decode(b64_buff,
                  				 strlen(b64_buff),
                  				 &len_raw);
	if(tmp_raw_buff == NULL)
		return 1;

	// The local buffer must not overflow
	if(len_serial_buf + len_raw >= SERIAL_BUF_LEN)
		return 1;

	// Copy temporary buffer to the end of the local buffer
	memcpy(serial_buf + len_serial_buf, 
		   tmp_raw_buff, 
		   len_raw);

	len_serial_buf += len_raw;


	//sprintf((char*)str, "len:%d %2.2X %2.2X %2.2X\r\n", len_raw, tmp_raw_buff[0], tmp_raw_buff[1], tmp_raw_buff[2]);
	//writeUSBOutString(str);

	// Free temporary buffer
	free(tmp_raw_buff);

	return 0;
}

/*
 * Read all of the FIFO, encode it in base64, send it back to the overo
 */
int _readFIFO(uint8_t * args){ 
	char*            arg_ptr;
	unsigned int     index;
	uint32_t         address;
	size_t 			 rev_len;
	//unsigned char*	 rev_buff;
	int 			 i;
	size_t 		     b64_len;
	char* 			 b64_buff;
	unsigned char 	 rev_buff[256]; // Can not be larger than 64

	arg_ptr = strtok(NULL, " ");
	if(arg_ptr == NULL)
	    return 1;

	address = strtoul(arg_ptr, NULL, 16);

	rev_len = readRegisterLeddar(address, LEDDARONE_RXLVL);

	/*if(rev_len == 0){
		sprintf((char*)str, "\r\n");
		writeUSBOutString(str);
		writeUSBOutString("0\r\n");
		return 0;
	}//*/

	//rev_buff = (unsigned char *)malloc(rev_len);
	//if(rev_buff == NULL)
	//	return 1;

	for(i = 0; i < rev_len; i++)
		rev_buff[i] = readRegisterLeddar(address, LEDDARONE_TRANSMIT_AND_RECEIVE);
	
	b64_buff = base64_encode(rev_buff,	   
                  			 rev_len,
                  			 &b64_len);
	if(b64_buff == NULL)
		return 1;

	sprintf((char*)str, "%X %s \r\n", b64_len, b64_buff);
	//memcpy(str, b64_buff, b64_len);
	//str[b64_len] = '\r';
	//str[b64_len + 1] = '\n';
	writeUSBOutString(str);

	// Send debug string
	//sprintf((char*)str, "%x\r\n", (unsigned int)b64_len);
	//writeUSBOutString(str);

	free(b64_buff);
	//free(rev_buff);

	return  0;
}

/*
 * Add byte to a buffer before send it
 */
int _addByteToBuff(uint8_t * args){ 
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

	serial_buf[len_serial_buf++] = arguments[0];

	// Wrap around, should use error handling of some kind
	if(len_serial_buf >= SERIAL_BUF_LEN)
		len_serial_buf = 0;

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

	int res = writeMultiDataRegisterLeddar(arguments[0], LEDDARONE_TRANSMIT_AND_RECEIVE, serial_buf, len_serial_buf);
	len_serial_buf = 0;

	return res;
}

/**
	Receive a byte from the serial RX FIFO of the SC16IS750
*/
int _receiveSerialOverI2C(uint8_t * args){ 
	char*            arg_ptr;
	unsigned int     index;
	uint32_t         arguments[2];
	uint8_t          receive_buffer;
	char* 	     str;

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

/**
	Test the connectivity of the SC16IS750
*/
int _testLeddar(uint8_t * args){ 
	char*            arg_ptr;
	unsigned int     index;
	uint32_t         arguments[1];
	uint8_t          good_parameter;
	uint8_t          received;
	char* 	     	 str;

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

/**
	Reset state of the SC16IS750
*/
int _resetDevice(uint8_t * args){ 
	char*            arg_ptr;
	uint32_t         arguments;

	arg_ptr = strtok(NULL, " ");
	if(arg_ptr == NULL)
	    return 1;

	arguments = strtoul(arg_ptr, NULL, 16);

	uint8_t address_i2c = arguments;

	int temp_iocontrol = readRegisterLeddar(address_i2c, LEDDARONE_IOCONTROL);
	temp_iocontrol |= 0x08; // Reset Flag
	writeRegisterLeddar(address_i2c, LEDDARONE_IOCONTROL, temp_iocontrol); 

	// Reset the size of the local buffer
	len_serial_buf = 0; 

	return 0;
}

int _setSerialBaudrate(uint8_t * args){ 
	char*            arg_ptr;
	uint8_t 		 address_i2c;
	
	arg_ptr = strtok(NULL, " ");
	if(arg_ptr == NULL)
	    return 1;

	address_i2c = strtoul(arg_ptr, NULL, 16);

	int prescaler;
	if ( (readRegisterLeddar(address_i2c, LEDDARONE_MCR)&0x80) == 0) { 
		prescaler = 1;
	} 
	else {
		prescaler = 4;
	}

	uint16_t divisor = (LEDDARONE_CRYSTAL_div_BAUDRATE/prescaler)/16;
	uint8_t tmp_lcr = readRegisterLeddar(address_i2c, LEDDARONE_LCR);
	tmp_lcr |= 0x80;
	writeRegisterLeddar(address_i2c, LEDDARONE_LCR, tmp_lcr);
	//write to DLL
	writeRegisterLeddar(address_i2c, LEDDARONE_DLL, (uint8_t)divisor);
	//write to DLH
	writeRegisterLeddar(address_i2c, LEDDARONE_DLH, (uint8_t)(divisor>>8));
	tmp_lcr &= 0x7F;
	writeRegisterLeddar(address_i2c, LEDDARONE_LCR, tmp_lcr); 

	// Set Fifo to enable
	uint8_t tmp_fcr = readRegisterLeddar(address_i2c, LEDDARONE_LCR);
	tmp_fcr |= 0x01;
	writeRegisterLeddar(address_i2c, LEDDARONE_FCR, tmp_fcr); 

	return 0;
}

/**
	Set serial configuration of the SC16IS750
*/
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

/**
	Return number of the byte in the receive FIFO of the SC16IS750
*/
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
