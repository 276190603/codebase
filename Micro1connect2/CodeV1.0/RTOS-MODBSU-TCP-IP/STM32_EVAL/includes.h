/*
*********************************************************************************************************
*
*                                             EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                          MASTER INCLUDE FILE
*
*                                          Atmel AT91SAM7X
*                                                on the
*                                  Atmel AT91SAM7X-EK Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : BAN
*********************************************************************************************************
*/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <stdarg.h>

#include    "stm32f10x.h"
#include    "stm32f107.h"

#include    <ucos_ii.h>

#include    <cpu.h>
#include    <lib_def.h>
#include    <lib_mem.h>
#include    <lib_str.h>
#include    <lib_ascii.h>

#include    <app_cfg.h>
#include    "stm32_eval.h"
//#include    <bsp.h>

#include    "RS485.h"
#include    "bms.h"
#include    "USART1.H"
#include    "USART2.H"
#include    "USART3.H"
#include "UART5.h"
#include "DIPswitch.h"
#include "gpio_control.h"
#include    "portarch.h"
//#if (APP_CFG_TCPIP_EN > 0)
//#include    <net.h>
//#include    "portarch.h"
//#endif
void checkComm_TCP(void);
union float_to_u16
{
		float data32;
		unsigned short int data16[2];
};

extern unsigned char addard,Get_Rdata,Start_Rdata1,Start_Rdata2;
#endif
