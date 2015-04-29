// *****************************************************************************
// Module..: SerialDemo
//
/// \file    Modbus.h
///
/// \brief   Declarations for the Modbus layer of the demo.
///
// Copyright (c) 2014 LeddarTech Inc. All rights reserved.
// Information contained herein is or may be confidential and proprietary to
// LeddarTech inc. Prior to using any part of the software development kit
// accompanying this notice, you must accept and agree to be bound to the
// terms of the LeddarTech Inc. license agreement accompanying this file.
// *****************************************************************************

#ifndef _MOBDUS_H_
#define _MOBDUS_H_

//#include "OS.h"
#include "leddarone.h"

// These definitions assumes 8-bit chars, 16-bit shorts and 32-bit ints.
// If your platform uses different sizes you will have to modify them.
typedef unsigned char  LtBool;
typedef unsigned char  LtByte;
typedef int            LtResult;
typedef unsigned short LtU16;
typedef short          Lt16;
typedef unsigned int   LtU32;

#define LT_TRUE   1
#define LT_FALSE  0

#define LT_PROTOCOL_ERROR   (-5)
#define LT_BAD_CRC          (-4)
#define LT_TIMEOUT          (-3)
#define LT_INVALID_ARGUMENT (-2)
#define LT_ERROR            (-1)
#define LT_SUCCESS            0

#define LT_PARITY_NONE  0
#define LT_PARITY_ODD   1
#define LT_PARITY_EVEN  2

#define LT_INVALID_HANDLE     (-1)
#define LT_MAX_PORT_NAME_LEN  24

typedef int LtHandle;

#define MODBUS_MAX_PAYLOAD 252
#define MODBUS_SERVER_ID   0x11

int _mobusInteract(uint8_t * args);

LtResult WriteToSerialPort( LtHandle aHandle, LtByte *aData, int aLength);
LtResult ReadFromSerialPort( LtHandle aHandle, LtByte *aData, int aMaxLength);


LtBool
ModbusConnected( void );

LtResult
ModbusSend( LtByte aFunction, LtByte *aBuffer, LtByte aLength );

LtResult
ModbusReceive( LtByte *aBuffer );

LtResult
ModbusConnect( char *aPortName, LtByte aAddress );

void
ModbusDisconnect( void );

LtResult
ModbusReadInputRegisters( LtU16 aNo, LtU16 aCount, LtU16 *aValue );

LtResult
ModbusReadHoldingRegister( LtU16 aNo, LtU16 *aValue );

LtResult
ModbusWriteRegister( LtU16 aNo, LtU16 aValue );

#endif
