
#ifndef _portarch_h_
#define _portarch_h_

#include "mb.h"
#include "mbport.h"
#include "mbutils.h"
#include "cpu.h"
#include "USART1.h"
/* ----------------------- Defines ------------------------------------------*/
#define REG_COILS_START     1
#define REG_COILS_SIZE      16

#define REG_DISCRETE_START  1
#define REG_DISCRETE_SIZE   16

#define REG_INPUT_START     1
#define REG_INPUT_NREGS     3100

#define REG_HOLDING_START   1
#define REG_HOLDING_NREGS   160
extern USHORT   usRegInputBuf[REG_INPUT_NREGS];

extern unsigned char get_R_T_flag,get_R_I_flag;
/* ----------------------- Function ------------------------------------------*/
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode );

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete );


eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs );


eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode );

#endif
