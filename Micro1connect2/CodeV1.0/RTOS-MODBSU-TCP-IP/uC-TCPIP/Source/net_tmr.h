/*
*********************************************************************************************************
*                                              uC/TCP-IP
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2008; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/TCP-IP is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/TCP-IP in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to help
*               you experience uC/TCP-IP.  The fact that the source code is provided does
*               NOT mean that you can use it without paying a licensing fee.
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
*                                       NETWORK TIMER MANAGEMENT
*
* Filename      : net_tmr.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_TMR_MODULE
#define  NET_TMR_EXT
#else
#define  NET_TMR_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     NETWORK TIMER FLAG DEFINES
*********************************************************************************************************
*/

                                                                /* ------------------ NET TMR FLAGS ------------------- */
#define  NET_TMR_FLAG_NONE                        DEF_BIT_NONE
#define  NET_TMR_FLAG_USED                        DEF_BIT_00    /* Tmr cur used; i.e. NOT in free tmr pool.             */


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NETWORK TIMER TASK TIME DEFINES
*
* Note(s) : (1) Time constants based on NET_TMR_CFG_TASK_FREQ, NetTmr_TaskHandler()'s frequency [i.e. how 
*               often NetTmr_TaskHandler() is scheduled to run per second as implemented in NetOS_Tmr_Task()].  
*
*           (2) NET_TMR_CFG_TASK_FREQ  MUST NOT be configured as a floating-point frequency.
*
*           (3) NET_TMR_TIME_MAX  #define'd as longest time constant internally required by the network 
*               protocol suite.
*********************************************************************************************************
*/

#define  NET_TMR_TIME_0S                                                                   (  0L)

#define  NET_TMR_TIME_10MS                                                                (( 10L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_20MS                                                                (( 20L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_30MS                                                                (( 30L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_40MS                                                                (( 40L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_50MS                                                                (( 50L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_60MS                                                                (( 60L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_70MS                                                                (( 70L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_80MS                                                                (( 80L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_90MS                                                                (( 90L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)

#define  NET_TMR_TIME_100MS                                                               ((100L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_200MS                                                               ((200L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_300MS                                                               ((300L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_400MS                                                               ((400L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_500MS                                                               ((500L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_600MS                                                               ((600L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_700MS                                                               ((700L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_800MS                                                               ((800L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TMR_TIME_900MS                                                               ((900L  *  NET_TMR_CFG_TASK_FREQ)  /  DEF_TIME_NBR_mS_PER_SEC)

#define  NET_TMR_TIME_1S                                                                   (  1L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_2S                                                                   (  2L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_3S                                                                   (  3L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_4S                                                                   (  4L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_5S                                                                   (  5L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_6S                                                                   (  6L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_7S                                                                   (  7L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_8S                                                                   (  8L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_9S                                                                   (  9L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_10S                                                                  ( 10L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_15S                                                                  ( 15L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_20S                                                                  ( 20L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_25S                                                                  ( 25L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_30S                                                                  ( 30L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_35S                                                                  ( 35L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_40S                                                                  ( 40L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_45S                                                                  ( 45L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_50S                                                                  ( 50L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_55S                                                                  ( 55L  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_60S                                                                  ( 60L  *  NET_TMR_CFG_TASK_FREQ)
/*$PAGE*/
#define  NET_TMR_TIME_1M                                      (  1L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_2M                                      (  2L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_3M                                      (  3L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_4M                                      (  4L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_5M                                      (  5L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_6M                                      (  6L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_7M                                      (  7L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_8M                                      (  8L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_9M                                      (  9L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_10M                                     ( 10L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_15M                                     ( 15L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_20M                                     ( 20L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_25M                                     ( 25L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_30M                                     ( 30L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_35M                                     ( 35L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_40M                                     ( 40L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_45M                                     ( 45L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_50M                                     ( 50L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_55M                                     ( 55L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_60M                                     ( 60L  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)

#define  NET_TMR_TIME_1H          (  1L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_2H          (  2L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_3H          (  3L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_4H          (  4L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_5H          (  5L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_6H          (  6L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_7H          (  7L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_8H          (  8L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_9H          (  9L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_10H         ( 10L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_11H         ( 11L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TIME_12H         ( 12L  *  DEF_TIME_NBR_MIN_PER_HR  *  DEF_TIME_NBR_SEC_PER_MIN  *  NET_TMR_CFG_TASK_FREQ)


#define  NET_TMR_TIME_INFINITE_MS           DEF_INT_32U_MAX_VAL
#define  NET_TMR_TIME_INFINITE_SEC          DEF_INT_32U_MAX_VAL


#define  NET_TMR_TIME_MIN                   NET_TMR_TIME_0S
#define  NET_TMR_TIME_MAX                   NET_TMR_TIME_12H

#define  NET_TMR_TIME_TICK                    1
#define  NET_TMR_TIME_TICK_PER_SEC          NET_TMR_TIME_1S



#define  NET_TMR_TASK_PERIOD_SEC                                        NET_TMR_CFG_TASK_FREQ
#define  NET_TMR_TASK_PERIOD_mS            (DEF_TIME_NBR_mS_PER_SEC  /  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TASK_PERIOD_uS            (DEF_TIME_NBR_uS_PER_SEC  /  NET_TMR_CFG_TASK_FREQ)
#define  NET_TMR_TASK_PERIOD_nS            (DEF_TIME_NBR_nS_PER_SEC  /  NET_TMR_CFG_TASK_FREQ)


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NETWORK TIMER TYPE DEFINES
*
* Note(s) : (1) NET_TMR_TYPE_??? #define values specifically chosen as ASCII representations of the 
*               network timer types.  Memory displays of network timers will display the timer TYPEs 
*               with their chosen ASCII name.
*********************************************************************************************************
*/

                                                                /* ------------------ NET TMR TYPES ------------------- */
#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  NET_TMR_TYPE_NONE                        0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_TMR_TYPE_TMR                         0x544D5220    /* "TMR " in ASCII.                                     */

#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)
#define  NET_TMR_TYPE_NONE                        0x454E4F4E    /* "NONE" in ASCII.                                     */
#define  NET_TMR_TYPE_TMR                         0x20524D54    /* "TMR " in ASCII.                                     */

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  NET_TMR_TYPE_NONE                        0x4F4E454E    /* "NONE" in ASCII.                                     */
#define  NET_TMR_TYPE_TMR                         0x4D542052    /* "TMR " in ASCII.                                     */

#else                                                           /* Dflt CPU_WORD_SIZE_08.                               */
#define  NET_TMR_TYPE_NONE                        0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_TMR_TYPE_TMR                         0x544D5220    /* "TMR " in ASCII.                                     */

#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                  NETWORK TIMER QUANTITY DATA TYPE
*
* Note(s) : (1) NET_TMR_NBR_MAX  SHOULD be #define'd based on 'NET_TMR_QTY' data type declared.
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_TMR_QTY;                               /* Defines max qty of net tmrs to support.              */

#define  NET_TMR_NBR_MIN                                  1
#define  NET_TMR_NBR_MAX                DEF_INT_16U_MAX_VAL     /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                    NETWORK TIMER TICK DATA TYPE
*
* Note(s) : (1) 'NET_TMR_TICK' data type size determined by selecting the smaller data type size of 
*               'CPU_INT16U'/'CPU_INT32U' whose maximum unsigned value is NOT less than NET_TMR_CFG_TICK_TH.
*
*           (2) NET_TMR_TIME_INFINITE_TICK & NET_TMR_TIME_INFINITE  MUST be globally #define'd AFTER
*              'NET_TMR_TICK' data type declared.
*********************************************************************************************************
*/

#define  NET_TMR_CFG_TICK_TH            NET_TMR_TIME_MAX

                                                                /* Defines   max TMR tick cnts.                         */
#if     (NET_TMR_CFG_TICK_TH <=         DEF_INT_16U_MAX_VAL)

typedef  CPU_INT16U    NET_TMR_TICK;

#define  NET_TMR_TIME_INFINITE_TICK     DEF_INT_16U_MAX_VAL     /* Define as max unsigned val (see Note #2).            */


#else

typedef  CPU_INT32U    NET_TMR_TICK;

#define  NET_TMR_TIME_INFINITE_TICK     DEF_INT_32U_MAX_VAL     /* Define as max unsigned val (see Note #2).            */

#endif


#define  NET_TMR_TIME_INFINITE          NET_TMR_TIME_INFINITE_TICK


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NETWORK TIMER DATA TYPE
*
*                                    NET_TMR
*                                |-------------|
*                                | Timer Type  |
*                     Previous   |-------------|
*                      Timer <----------O      |
*                                |-------------|     Next
*                                |      O----------> Timer
*                                |-------------|                    ------------- 
*                                |      O-------------------------> |           |
*                                |-------------|       Object       |  Object   |
*                                |      O----------> Expiration     |   that    |
*                                |-------------|      Function      | requested |
*                                |   Current   |                    |   Timer   |
*                                | Timer value |                    |           |
*                                |-------------|                    -------------
*                                |    Flags    |
*                                |-------------|
*
*
* Note(s) : (1) Forced word-alignment at start of timer NOT required since first data member 'Type' is 
*               declared as 'CPU_INT32U'.
*********************************************************************************************************
*/

                                                                /* --------------------- NET TMR ---------------------- */
struct  net_tmr {
    NET_TYPE        Type;                                       /* Type cfg'd @ init : NET_TMR_TYPE_TMR.                */

    NET_TMR        *PrevPtr;                                    /* Ptr to PREV tmr.                                     */
    NET_TMR        *NextPtr;                                    /* Ptr to NEXT tmr.                                     */

    void           *Obj;                                        /* Ptr to obj  using TMR.                               */
    CPU_FNCT_PTR    Fnct;                                       /* Ptr to fnct used on obj when TMR expires.            */

    NET_TMR_TICK    TmrVal;                                     /* Cur tmr val (in NET_TMR_TICK ticks).                 */

    NET_TMR_QTY     ID;                                         /* Tmr id.                                              */
    CPU_INT16U      Flags;                                      /* Tmr flags.                                           */
};


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

NET_TMR_EXT  NET_TMR         NetTmr_Tbl[NET_TMR_CFG_NBR_TMR];
NET_TMR_EXT  NET_TMR        *NetTmr_PoolPtr;                    /* Ptr to pool of free net tmrs.                        */
NET_TMR_EXT  NET_STAT_POOL   NetTmr_PoolStat;

NET_TMR_EXT  NET_TMR        *NetTmr_TaskListHead;               /* Ptr to head of Tmr Task List.                        */
NET_TMR_EXT  NET_TMR        *NetTmr_TaskListPtr;                /* Ptr to cur     Tmr Task List tmr to update.          */


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void            NetTmr_Init                (NET_ERR       *perr);

void            NetTmr_TaskHandler         (void);


                                                                        /* ----------- TMR ALLOCATION FNCTS ----------- */
NET_TMR        *NetTmr_Get                 (void          *obj,
                                            CPU_FNCT_PTR   fnct,
                                            NET_TMR_TICK   time,
                                            CPU_INT16U     flags,
                                            NET_ERR       *perr);

void            NetTmr_Free                (NET_TMR       *ptmr);


                                                                        /* -------------- TMR API FNCTS --------------- */
void            NetTmr_Set                 (NET_TMR       *ptmr,
                                            CPU_FNCT_PTR   fnct,
                                            NET_TMR_TICK   time,
                                            NET_ERR       *perr);


                                                                        /* ------------- TMR STATUS FNCTS ------------ */
NET_STAT_POOL   NetTmr_PoolStatGet         (void);

void            NetTmr_PoolStatResetMaxUsed(void);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                      DEFINED IN OS'S  net_os.c
*********************************************************************************************************
*/

void            NetOS_Tmr_Init             (NET_ERR       *perr);       /* Create & start Tmr Task.                     */


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_TMR_CFG_NBR_TMR
#error  "NET_TMR_CFG_NBR_TMR          not #define'd in 'net_cfg.h'"
#error  "                       [MUST be  >= NET_TMR_NBR_MIN]     "
#error  "                       [     &&  <= NET_TMR_NBR_MAX]     "
#elif  ((NET_TMR_CFG_NBR_TMR < NET_TMR_NBR_MIN) || \
        (NET_TMR_CFG_NBR_TMR > NET_TMR_NBR_MAX))
#error  "NET_TMR_CFG_NBR_TMR    illegally #define'd in 'net_cfg.h'"
#error  "                       [MUST be  >= NET_TMR_NBR_MIN]     "
#error  "                       [     &&  <= NET_TMR_NBR_MAX]     "
#endif




#ifndef  NET_TMR_CFG_TASK_FREQ
#error  "NET_TMR_CFG_TASK_FREQ        not #define'd in 'net_cfg.h'"
#error  "                       [MUST be  > 0 Hz]                 "
#elif   (NET_TMR_CFG_TASK_FREQ < 1)
#error  "NET_TMR_CFG_TASK_FREQ  illegally #define'd in 'net_cfg.h'"
#error  "                       [MUST be  > 0 Hz]                 "
#endif

