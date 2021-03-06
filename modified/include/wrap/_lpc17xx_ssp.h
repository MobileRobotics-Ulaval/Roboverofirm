/*******************************************************************************
 * @file
 * @purpose        
 * @version        0.1
 *------------------------------------------------------------------------------
 * Copyright (C) 2011 Gumstix Inc.
 * All rights reserved.
 *
 * Contributer(s):
 *   Neil MacMunn   <neil@gumstix.com>
 *------------------------------------------------------------------------------
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef ___LPC17XX_SSP_H__
#define ___LPC17XX_SSP_H__

#include "lpc_types.h"

int _SSP_LoopBackCmd(uint8_t * args);
int _SSP_GetRawIntStatus(uint8_t * args);
int _SSP_Init(uint8_t * args);
int _SSP_DMACmd(uint8_t * args);
int _SSP_GetDataSize(uint8_t * args);
int _SSP_Cmd(uint8_t * args);
int _SSP_IntConfig(uint8_t * args);
int _SSP_SlaveOutputCmd(uint8_t * args);
int _SSP_ClearIntPending(uint8_t * args);
int _SSP_ReadWrite(uint8_t * args);
int _SSP_DeInit(uint8_t * args);
int _SSP_GetStatus(uint8_t * args);
int _SSP_GetRawIntStatusReg(uint8_t * args);
int _SSP_ConfigStructInit(uint8_t * args);
int _SSP_ReceiveData(uint8_t * args);
int _SSP_GetIntStatus(uint8_t * args);
int _SSP_SendData(uint8_t * args);
int _SSP_CFG_Type_malloc(uint8_t * args);
int _SSP_CFG_Type_Databit(uint8_t * args);
int _SSP_CFG_Type_CPHA(uint8_t * args);
int _SSP_CFG_Type_CPOL(uint8_t * args);
int _SSP_CFG_Type_Mode(uint8_t * args);
int _SSP_CFG_Type_FrameFormat(uint8_t * args);
int _SSP_CFG_Type_ClockRate(uint8_t * args);
int _SSP_DATA_SETUP_Type_malloc(uint8_t * args);
int _SSP_DATA_SETUP_Type_tx_data(uint8_t * args);
int _SSP_DATA_SETUP_Type_tx_cnt(uint8_t * args);
int _SSP_DATA_SETUP_Type_rx_data(uint8_t * args);
int _SSP_DATA_SETUP_Type_rx_cnt(uint8_t * args);
int _SSP_DATA_SETUP_Type_length(uint8_t * args);
int _SSP_DATA_SETUP_Type_status(uint8_t * args);
int _SSP_TRANSFER_Type_malloc(uint8_t * args);

#endif
