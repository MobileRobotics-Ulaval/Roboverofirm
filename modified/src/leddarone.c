

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
	else
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
	else
		return 0;
}

uint8_t readRegisterLeddar(uint8_t address_i2c, uint8_t reg, uint8_t *receive_buffer){
	I2C_M_SETUP_Type setup;
	Status           result;
	uint8_t          transmit_buffer;

	setup.sl_addr7bit         = address_i2c >> 1;
	setup.retransmissions_max = MAX_ST_I2C_RETRANSMISSIONS;

	/* Reg Address*/
	// The address format is XAAACCX where X is unused, A is the reg address and C is unused
	transmit_buffer = reg << 3;

	setup.tx_data   = &transmit_buffer;
	setup.tx_length = 1;
	setup.rx_data   = receive_buffer;
	setup.rx_length = 1;

	result = I2C_MasterTransferData(i2cextern, &setup, I2C_TRANSFER_POLLING);
	if(result == ERROR)
		return 1;
	else
		return 0;
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
	char* 			 b64_buff = 0;
	unsigned char    tmp_raw_buff[256];
	size_t         	 len_raw;
	//uint32_t         len_b64;

	b64_buff = strtok(NULL, " ");
	if(b64_buff == NULL)
	    return 1;

	//sprintf((char*)str, "len:%d str:%s\r\n", strlen(b64_buff), b64_buff);
	//writeUSBOutString(str);

	// Decode base64 to raw binary data
	base64_decode(b64_buff,
				  tmp_raw_buff,
  				  strlen(b64_buff),
  				  &len_raw);

	// The local buffer must not overflow
	if(len_serial_buf + len_raw >= SERIAL_BUF_LEN){
		//free(tmp_raw_buff);
		return 1;
	}

	// Copy temporary buffer to the end of the local buffer
	memcpy(serial_buf + len_serial_buf, 
		   tmp_raw_buff, 
		   len_raw);

	len_serial_buf += len_raw;

	// Free temporary buffer
	//free(tmp_raw_buff);

	return 0;
}

/*
 * Read all of the FIFO, encode it in base64, send it back to the overo
 */
int _readFIFO(uint8_t * args){ 
	char*            arg_ptr;
	unsigned int     index;
	uint32_t         address;
	int 			 i;

	uint8_t 		 error;
	uint8_t			 rev_len;
	size_t 		     b64_len;
	unsigned char 	 rev_buff[64]; // Can not be larger than 64
	char 			 b64_buff[256];

	arg_ptr = strtok(NULL, " ");
	if(arg_ptr == NULL)
	    return 1;

	address = strtoul(arg_ptr, NULL, 16);

	error = readRegisterLeddar(address, LEDDARONE_RXLVL, &rev_len);
	if(error > 0){
		writeUSBOutString((uint8_t)"0  \r\n");
		return 1;
	}

	// The following line is way more efficient, but it's better to wait
	// for additionnal data than checking the FIFO non-stop
	//if(rev_len == 0)
	//	writeUSBOutString("0  \r\n");

	for(i = 0; i < rev_len; i++)
		error += readRegisterLeddar(address, LEDDARONE_TRANSMIT_AND_RECEIVE, &(rev_buff[i]));
	if(error > 0){
		writeUSBOutString((uint8_t)"0  \r\n");
		return 1;
	}
	
	 base64_encode(rev_buff,
	 			   b64_buff,	   
      			   rev_len,
      			   &b64_len);
	//f(b64_buff == NULL)
	//	return 1;

	// Add a end of string caracter at the end of the string
	b64_buff[b64_len] = 0;

	sprintf((char*)str, "%x %s \r\n", (unsigned int)b64_len, b64_buff);
	//memcpy(str, b64_buff, b64_len);
	//str[b64_len] = '\r';
	//str[b64_len + 1] = '\n';
	writeUSBOutString(str);

	// /* DEBUG PRINT DONT FORGET TO DELETE BOTH MCU AND CURRENT CODE*/
	// sprintf((char*)str, "This is the len%x\r\n", (unsigned int)rev_len);
	// writeUSBOutString(str);

	//free(b64_buff);
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

	//sprintf((char*)str, "len:%x %2.2X %2.2X %2.2X\r\n", len_serial_buf, serial_buf[0], serial_buf[1], serial_buf[2]);
	//writeUSBOutString(str);


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
	char* 	     	 str;

	for(index = 0; index < 2; index++)
	{
		arg_ptr = strtok(NULL, " ");
		if(arg_ptr == NULL)
		    return 1;

		arguments[index] = strtoul(arg_ptr, NULL, 16);
	}
	int res = readRegisterLeddar(arguments[0], arguments[1], &receive_buffer);

	sprintf((char*)str, "%x\r\n", receive_buffer);
	writeUSBOutString(str);

	return res;
}

/**
	Test the connectivity of the SC16IS750
*/
int _testLeddar(uint8_t * args){ 
	char*            arg_ptr;
	uint32_t         address;
	uint8_t          good_parameter;
	uint8_t          received;
	uint8_t          response;
	uint8_t          error;
	char* 	     	 str;

	arg_ptr = strtok(NULL, " ");
	if(arg_ptr == NULL)
	    return 1;

	address = strtoul(arg_ptr, NULL, 16);

	// We save the current parameter of the LCR
	error = readRegisterLeddar(address, LEDDARONE_LCR, &good_parameter);
	if(error != 0){
		writeUSBOutString("0\r\n");
		return 1;
	}
	else{
		writeUSBOutString("1\r\n");
		return 0;
	}
	/*
	// Write dummy data
	error += writeRegisterLeddar(address, LEDDARONE_LCR, 0x4);
	if(error != 0){
		writeUSBOutString("0\r\n");
		return 1;
	}
	// Check what is writing in the register
	error += readRegisterLeddar(address, LEDDARONE_LCR, &received);
	if(error != 0){
		writeUSBOutString("0\r\n");
		return 1;
	}

	// Rewrite the register to the old value
	error += writeRegisterLeddar(address, LEDDARONE_LCR, good_parameter);
	if(error != 0){
		writeUSBOutString("0\r\n");
		return 1;
	}

	if(received == 0x4)
		response = 1;
	else
		response = 0;
	sprintf((char*)str, "%x\r\n", response); 

	writeUSBOutString(str);

	return 0;
	*/
}

/**
	Reset state of the SC16IS750
*/
int _resetDevice(uint8_t * args){ 
	char*            arg_ptr;
	uint32_t         arguments;
	uint8_t 	temp_iocontrol;

	arg_ptr = strtok(NULL, " ");
	if(arg_ptr == NULL)
	    return 1;

	arguments = strtoul(arg_ptr, NULL, 16);

	uint8_t address_i2c = arguments;

	readRegisterLeddar(address_i2c, LEDDARONE_IOCONTROL, &temp_iocontrol);
	temp_iocontrol |= 0x08; // Reset Flag
	writeRegisterLeddar(address_i2c, LEDDARONE_IOCONTROL, temp_iocontrol); 

	// Reset the size of the local buffer
	len_serial_buf = 0; 

	return 0;
}

int _setSerialBaudrate(uint8_t * args){ 
	char*            arg_ptr;
	uint8_t 		 address_i2c;
	uint8_t 		 receive, tmp_lcr, tmp_fcr;
	
	arg_ptr = strtok(NULL, " ");
	if(arg_ptr == NULL)
	    return 1;

	address_i2c = strtoul(arg_ptr, NULL, 16);

	int prescaler;
	readRegisterLeddar(address_i2c, LEDDARONE_MCR, &receive);
	if ((receive & 0x80) == 0) { 
		prescaler = 1;
	} 
	else {
		prescaler = 4;
	}

	uint16_t divisor = (LEDDARONE_CRYSTAL_div_BAUDRATE/prescaler)/16;
	readRegisterLeddar(address_i2c, LEDDARONE_LCR, &tmp_lcr);
	tmp_lcr |= 0x80;
	writeRegisterLeddar(address_i2c, LEDDARONE_LCR, tmp_lcr);
	//write to DLL
	writeRegisterLeddar(address_i2c, LEDDARONE_DLL, (uint8_t)divisor);
	//write to DLH
	writeRegisterLeddar(address_i2c, LEDDARONE_DLH, (uint8_t)(divisor>>8));
	tmp_lcr &= 0x7F;
	writeRegisterLeddar(address_i2c, LEDDARONE_LCR, tmp_lcr); 

	// Set Fifo to enable
	readRegisterLeddar(address_i2c, LEDDARONE_LCR, &tmp_fcr);
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
	readRegisterLeddar(address_i2c, LEDDARONE_LCR, &temp_lcr);
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
	uint8_t          fifo_len;
	//i2c_smbus_write_word_data(file, commande, (argument << 8) | info1) >= 0
	for(index = 0; index < 1; index++)
	{
		arg_ptr = strtok(NULL, " ");
		if(arg_ptr == NULL)
		    return 1;

		arguments[index] = strtoul(arg_ptr, NULL, 16);
	}

	readRegisterLeddar(arguments[0], LEDDARONE_RXLVL, &fifo_len);
	return fifo_len;
}
