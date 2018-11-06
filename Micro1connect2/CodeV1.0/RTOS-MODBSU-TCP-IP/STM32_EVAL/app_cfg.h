/*
*********************************************************************************************************
*
*                                             EXAMPLE CODE
*
*                          (c) Copyright 2003-2008; Micrium, Inc.; Weston, FL
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
*                                      APPLICATION CONFIGURATION
*
*                                          Atmel AT91SAM7X
*                                                on the
*                                  Atmel AT91SAM7X-EK Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.01
* Programmer(s) : BAN
*                 FT                  
*********************************************************************************************************
*/

#ifndef __APP_CFG_H__
#define __APP_CFG_H__

/*
*********************************************************************************************************
*                                   ADDITIONAL uC/MODULE ENABLES
*********************************************************************************************************
*/

#define  APP_CFG_TCPIP_EN                 DEF_ENABLED

/*
*********************************************************************************************************
*                                        TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_PRIO               3
#define  APP_CFG_TASK_USER_IF_PRIO             4
#define  APP_CFG_TASK_JOY_PRIO                 5
#define  APP_CFG_TASK_PROBE_STR_PRIO           6
#define  APP_CFG_TASK_PHY_PRIO                 7
#define  APP_CFG_TASK_MODBUS_PRIO              8
//#define  APP_CFG_TASK_ALARM_PRIO               9

#define  KSD_CFG_TASK_LED_PRIO                 9
#define  KSD_CFG_TASK_LED_ID                   9

#define  NET_OS_CFG_IF_RX_TASK_PRIO           10
#define  NET_OS_CFG_TMR_TASK_PRIO             11

#define  OS_PROBE_TASK_PRIO                   12
#define  OS_PROBE_TASK_ID                     13

#define  OS_TASK_TMR_PRIO              (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                        TASK STACK SIZES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_STK_SIZE         256
#define  APP_CFG_TASK_USER_IF_STK_SIZE       256

#define  NET_OS_CFG_TMR_TASK_STK_SIZE        128
#define  APP_CFG_TASK_PHY_STK_SIZE           128
#define  APP_CFG_TASK_MODBUS_STK_SIZE       512
//#define  APP_CFG_TASK_ALARM_STK_SIZE       128

#define  KSD_CFG_TASK_LED_STK_SIZE           256

#define  NET_OS_CFG_IF_RX_TASK_STK_SIZE     256

/*
*********************************************************************************************************
*                                      uC/OS-II DCC CONFIGURATION
*********************************************************************************************************
*/

#define  OS_CPU_ARM_DCC_EN                     1

/*
*********************************************************************************************************
*                                      uC/LIB CONFIGURATION
*********************************************************************************************************
*/

#define  uC_CFG_OPTIMIZE_ASM_EN                1

/*
*********************************************************************************************************
*                                          BSP CONFIGURATION
*********************************************************************************************************
*/

#define  BSP_CFG_SER_COMM_EN             DEF_ENABLED
#define  BSP_CFG_SER_COMM_SEL            BSP_SER_COMM_UART_DBG

/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                       0
#define  TRACE_LEVEL_INFO                      1
#define  TRACE_LEVEL_DEBUG                     2

#define  BSP_CFG_TRACE_LEVEL            TRACE_LEVEL_OFF
#define  BSP_CFG_TRACE                  printf          

#define  BSP_TRACE_INFO(x)            ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_INFO)  ? (void)(BSP_CFG_TRACE x) : (void)0)
#define  BSP_TRACE_DBG(x)             ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_DBG)   ? (void)(BSP_CFG_TRACE x) : (void)0)

#define  APP_CFG_TRACE_LEVEL            TRACE_LEVEL_INFO
#define  APP_CFG_TRACE                  BSP_Ser_Printf          

#define  APP_TRACE_INFO(x)            ((APP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_INFO)  ? (void)(APP_CFG_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)             ((APP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_DBG)   ? (void)(APP_CFG_TRACE x) : (void)0)

#endif
