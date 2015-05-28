#ifndef __LEDARFIRML_H__
#define __LEDARFIRML_H__

#include "lpc_types.h"
#include "extras.h"

#define i2cextern LPC_I2C2
#define VERSION 1

#define MAX_ST_I2C_RETRANSMISSIONS   3
#define	LEDDARONE_TRANSMIT_AND_RECEIVE	0x00
#define	LEDDARONE_FCR				0x02
#define	LEDDARONE_INIT				0x05

#define	LEDDARONE_IOCONTROL			0x0E

#define	LEDDARONE_LCR				0x03
#define	LEDDARONE_MCR				0x04
#define	LEDDARONE_RXLVL				0x09
#define	LEDDARONE_DLL				0x00
#define	LEDDARONE_DLH				0x01
#define	LEDDARONE_CRYSTAL_FREQUENCY	14745600
#define	LEDDARONE_BAUDRATE			115200
#define	LEDDARONE_CRYSTAL_div_BAUDRATE	128  // LEDDARONE_CRYSTAL_FREQUENCY / LEDDARONE_BAUDRATE


int writeRegisterLeddar(uint8_t address_i2c, uint8_t reg, uint8_t value);
int writeMultiDataRegisterLeddar(uint8_t address_i2c, uint8_t reg, unsigned char *aData, int aLength );
uint8_t readRegisterLeddar(uint8_t address_i2c, uint8_t reg, uint8_t *receive);
int _sendSerialOverI2C(uint8_t * args);
int _addDataToBuff(uint8_t * args);
int _readFIFO(uint8_t * args);
int _addByteToBuff(uint8_t * args);
int _sendBufferOverI2C(uint8_t * args);
int _receiveSerialOverI2C(uint8_t * args);

int _testLeddar(uint8_t * args);

int _resetDevice(uint8_t * args); 
int _setSerialBaudrate(uint8_t * args);
int _setLine(uint8_t * args);
int _getFIFOAvailableData(uint8_t * args);

#endif
