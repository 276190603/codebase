
#include "portarch.h"
//#include "24cxx.h"
//#include "includes.h"
/* ------------------------------ variables ---------------------------------*/
USHORT   usRegCoilsStart = REG_COILS_START;
UCHAR    ucRegCoilsBuf[REG_COILS_SIZE / 8];

USHORT   usRegDiscreteStart = REG_DISCRETE_START;
UCHAR    ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8];

USHORT   usRegInputStart = REG_INPUT_START;
USHORT   usRegInputBuf[REG_INPUT_NREGS];

USHORT   usRegHoldingStart = REG_HOLDING_START;
USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

CPU_CHAR IP[20] = {"192.168.1.21"};
CPU_CHAR NETMASK[20] = {"255.255.255.0"};
CPU_CHAR GW[20] = {"192.168.1.1"};
/* ----------------------- CBFunction ---------------------------------------*/

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iNCoils = (int)usNCoils;
    unsigned short  usBitOffset;

    /* Check if we have registers mapped at this block. */
    if( ( usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );
        switch ( eMode )
        {
                /* Read current values and pass to protocol stack. */
            case MB_REG_READ:
                while( iNCoils > 0 )
                {
                    *pucRegBuffer++ =
                        xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                        ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ) );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;

                /* Update current register values. */
            case MB_REG_WRITE:
                while( iNCoils > 0 )
                {
                    xMBUtilSetBits( ucRegCoilsBuf, usBitOffset, 
                                    ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
                                    *pucRegBuffer++ );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/* ----------------------- CBFunction ---------------------------------------*/

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    short           iNDiscrete = ( short )usNDiscrete;
    unsigned short  usBitOffset;

    /* Check if we have registers mapped at this block. */
    if( ( usAddress >= REG_DISCRETE_START ) &&
        ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - usRegDiscreteStart );
    
        /* Read current values and pass to protocol stack. */
        while( iNDiscrete > 0 )
        {
             *pucRegBuffer++ =
             xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
                             ( unsigned char )( iNDiscrete > 8 ? 8 : iNDiscrete ) );
             iNDiscrete -= 8;
             usBitOffset += 8;
        }
     }
     else
     {
         eStatus = MB_ENOREG;
     }
     return eStatus;
}

/* ----------------------- CBFunction ---------------------------------------*/

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

/* ----------------------- CBFunction ---------------------------------------*/
//	union float_to_u16 data_1;
//	union float_to_u16 data_2;
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegInputBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegInputBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
//                switch(iRegIndex)
//                {
//                case 2330://一通道立即测内阻  不测
//                  get_R_T_flag = 1;
//                  break;
//                  
//                  
//                default:
//                  break;
//                }
               setRegisterVal_tcpip(iRegIndex-1,usRegInputBuf[iRegIndex])     ;       
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
