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
*                                   NETWORK OPERATING SYSTEM LAYER
*
*                                          Micrium uC/OS-II
*
* Filename      : net_os.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Assumes uC/OS-II V2.86 is included in the project build.
*
*                 (2) REQUIREs the following uC/OS-II features to be ENABLED :
*
*                         ------- FEATURE --------    ---------- MINIMUM CONFIGURATION FOR NET/OS PORT ----------
*
*                     (a) OS Events                   OS_MAX_EVENTS >= NET_OS_NBR_EVENTS (see this 'net_os.h
*                                                                                             OS OBJECT DEFINES')
*
*                     (b) Semaphores                                   NET_OS_NBR_SEM    (see Note #2a)
*                         (1) OS_SEM_EN                   Enabled
*                         (2) OS_SEM_SET_EN               Enabled
*                         (3) OS_SEM_PEND_ABORT_EN        Enabled
*
*                     (c) Multi-Pend
*                         (1) OS_EVENT_MULTI_EN           Enabled -- required only if NET_SOCK_CFG_SEL_EN
*                                                             is DEF_ENABLED
*
*                     (d) Time Delay
*                         (1) OS_TIME_DLY_HMSM_EN         Enabled -- required only if NET_SOCK_CFG_SEL_EN
*                                                             is DEF_ENABLED
*
*********************************************************************************************************
* Notice(s)     : (1) The Institute of Electrical and Electronics Engineers and The Open Group, have given 
*                     us permission to reprint portions of their documentation.  Portions of this text are 
*                     reprinted and reproduced in electronic form from the IEEE Std 1003.1, 2004 Edition, 
*                     Standard for Information Technology -- Portable Operating System Interface (POSIX), 
*                     The Open Group Base Specifications Issue 6, Copyright (C) 2001-2004 by the Institute 
*                     of Electrical and Electronics Engineers, Inc and The Open Group.  In the event of any 
*                     discrepancy between these versions and the original IEEE and The Open Group Standard, 
*                     the original IEEE and The Open Group Standard is the referee document.  The original 
*                     Standard can be obtained online at http://www.opengroup.org/unix/online.html.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_OS_MODULE
#include  <net.h>


/*$PAGE*/
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*
* Note(s) : (1) (a) Signals & locks are implemented using binary   semaphores.
*
*               (b) Queues          are implemented using counting semaphores.
*********************************************************************************************************
*/

                                                                        /* --------------- TASK STACKS ---------------- */
static  OS_STK     NetOS_Tmr_TaskStk[NET_OS_CFG_TMR_TASK_STK_SIZE];
static  OS_STK     NetOS_IF_RxTaskStk[NET_OS_CFG_IF_RX_TASK_STK_SIZE];


                                                                        /* ----------------- SIGNALS ------------------ */
static  OS_EVENT  *NetOS_InitSignalPtr;
static  OS_EVENT  *NetOS_NIC_TxRdySignalPtr;

#if    (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
static  OS_EVENT  *NetOS_TxSuspendSignalPtr;
#endif

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  OS_EVENT  *NetOS_Sock_ConnReqSignalPtr[NET_SOCK_CFG_NBR_SOCK];
static  OS_EVENT  *NetOS_Sock_ConnAcceptQ_SignalPtr[NET_SOCK_CFG_NBR_SOCK];
static  OS_EVENT  *NetOS_Sock_ConnCloseSignalPtr[NET_SOCK_CFG_NBR_SOCK];
#endif
#endif


                                                                        /* ------------------ LOCKS ------------------- */
static  OS_EVENT  *NetOS_LockPtr;


                                                                        /* ------------------ QUEUES ------------------ */
static  OS_EVENT  *NetOS_IF_RxQ_SignalPtr;

#ifdef  NET_TCP_MODULE_PRESENT
static  OS_EVENT  *NetOS_TCP_RxQ_SignalPtr[NET_TCP_CFG_NBR_CONN];
static  OS_EVENT  *NetOS_TCP_TxQ_SignalPtr[NET_TCP_CFG_NBR_CONN];
#endif

#ifdef  NET_SOCK_MODULE_PRESENT
static  OS_EVENT  *NetOS_Sock_RxQ_SignalPtr[NET_SOCK_CFG_NBR_SOCK];
#endif


                                                                        /* ------------------ TIMERS ------------------ */
#ifdef  NET_OS_TIMEOUT_PRESENT

#if    (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
static  INT16U     NetOS_TxSuspendTimeout_tick;
#endif

#ifdef  NET_TCP_MODULE_PRESENT
static  INT16U     NetOS_TCP_RxQ_TimeoutTbl_tick[NET_TCP_CFG_NBR_CONN];
static  INT16U     NetOS_TCP_TxQ_TimeoutTbl_tick[NET_TCP_CFG_NBR_CONN];
#endif

#ifdef  NET_SOCK_MODULE_PRESENT
static  INT16U     NetOS_Sock_RxQ_TimeoutTbl_tick[NET_SOCK_CFG_NBR_SOCK];

#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  INT16U     NetOS_Sock_ConnReqTimeoutTbl_tick[NET_SOCK_CFG_NBR_SOCK];
static  INT16U     NetOS_Sock_ConnAcceptQ_TimeoutTbl_tick[NET_SOCK_CFG_NBR_SOCK];
static  INT16U     NetOS_Sock_ConnCloseTimeoutTbl_tick[NET_SOCK_CFG_NBR_SOCK];
#endif
#endif

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                                        /* ---- NETWORK TIMER MANAGEMENT FUNCTIONS ---- */
static  void        NetOS_Tmr_Task           (void         *p_data);


                                                                        /* ---- NETWORK INTERFACE LAYER FUNCTIONS ----- */
static  void        NetOS_IF_RxTask          (void         *p_data);


                                                                        
#ifdef  NET_OS_TIMEOUT_PRESENT                                          /* - NETWORK/OS TIMEOUT CALCULATION FUNCTIONS - */

static  INT16U      NetOS_TimeoutCalc_OS_tick(CPU_INT32U    timeout_ms,
                                              CPU_BOOLEAN   lim_max,
                                              NET_ERR      *perr);

static  CPU_INT32U  NetOS_TimeoutCalc_ms     (INT16U        timeout_os_tick);

#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                          NETWORK FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            NetOS_Init()
*
* Description : (1) Perform network/OS initialization :
*
*                   (a) Implement  network initialization signal by creating a counting semaphore.
*
*                       (1) Initialize network initialization signal with no signal by setting the 
*                           semaphore count to 0 to block the semaphore.
*
*                   (b) Implement global network lock by creating a binary semaphore.
*
*                       (1) Initialize network lock as released by setting the semaphore count to 1.
*
*                   (c) Implement  network transmit suspend signal by creating a counting semaphore.
*
*                       (1) Initialize network transmit suspend signal with no signal by setting the 
*                           semaphore count to 0 to block the semaphore.
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                         Network/OS initialization successful.
*
*                               NET_OS_ERR_INIT_SIGNAL                  Network    initialization signal 
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_SIGNAL_NAME             Network    initialization signal name
*                                                                           NOT successfully configured.
*
*                               NET_OS_ERR_INIT_LOCK                    Network    lock           signal 
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_LOCK_NAME               Network    lock           signal name
*                                                                           NOT successfully configured.
*
*                               NET_OS_ERR_INIT_TX_SUSPEND              Network  transmit suspend signal
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_TX_SUSPEND_NAME         Network  transmit suspend signal name
*                                                                           NOT successfully configured.
*                               NET_OS_ERR_INIT_TX_SUSPEND_TIMEOUT      Network  transmit suspend signal timeout
*                                                                           NOT successfully configured.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetOS_Init (NET_ERR  *perr)
{
#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    INT8U    os_err;
#endif
#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
    NET_ERR  net_err;
#endif


                                                            /* -------------- INITIALIZE NETWORK SIGNAL --------------- */
    NetOS_InitSignalPtr = OSSemCreate((INT16U)0);           /* Create network initialization signal (see Note #1a).     */
    if (NetOS_InitSignalPtr == (OS_EVENT *)0) {
       *perr = NET_OS_ERR_INIT_SIGNAL;
        return;
    }

#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    OSEventNameSet((OS_EVENT *) NetOS_InitSignalPtr,
                   (INT8U    *) NET_INIT_NAME,
                   (INT8U    *)&os_err);
    if (os_err !=  OS_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_SIGNAL_NAME;
        return;
    }
#endif


                                                            /* --------------- INITIALIZE NETWORK LOCK ---------------- */
    NetOS_LockPtr = OSSemCreate((INT16U)1);                 /* Create network lock (see Note #1b).                      */
    if (NetOS_LockPtr == (OS_EVENT *)0) {
       *perr = NET_OS_ERR_INIT_LOCK;
        return;
    }

#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    OSEventNameSet((OS_EVENT *) NetOS_LockPtr,
                   (INT8U    *) NET_LOCK_NAME,
                   (INT8U    *)&os_err);
    if (os_err !=  OS_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_LOCK_NAME;
        return;
    }
#endif


                                                            /* --------- INITIALIZE NETWORK TRANSMIT SUSPEND ---------- */
#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
    NetOS_TxSuspendSignalPtr = OSSemCreate((INT16U)0);      /* Create network transmit suspend signal (see Note #1c).   */
    if (NetOS_TxSuspendSignalPtr == (OS_EVENT *)0) {
       *perr = NET_OS_ERR_INIT_TX_SUSPEND;
        return;
    }
                                                            /* Initialize net transmit suspend signal timeout value.    */
    NetOS_TxSuspendTimeoutSet(NET_CFG_TX_SUSPEND_TIMEOUT_MS, &net_err);
    if (net_err != NET_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_TX_SUSPEND_TIMEOUT;
        return;
    }

#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    OSEventNameSet((OS_EVENT *) NetOS_TxSuspendSignalPtr,
                   (INT8U    *) NET_TX_SUSPEND_NAME,
                   (INT8U    *)&os_err);
    if (os_err !=  OS_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_TX_SUSPEND_NAME;
        return;
    }
#endif
#endif


   *perr = NET_OS_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetOS_InitWait()
*
* Description : Wait on signal indicating network initialization is complete.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Initialization signal     received.
*                               NET_OS_ERR_INIT                 Initialization signal NOT received.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler(),
*               NetTmr_TaskHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network initialization signal MUST be acquired--i.e. MUST wait for access; do NOT timeout.
*
*                   Failure to acquire signal will prevent network task(s) from running.
*********************************************************************************************************
*/

void  NetOS_InitWait (NET_ERR  *perr)
{
    INT8U  os_err;


    OSSemPend((OS_EVENT *) NetOS_InitSignalPtr,                 /* Wait until network initialization completes ...      */
              (INT16U    ) 0,                                   /* ... without timeout (see Note #1).                   */
              (INT8U    *)&os_err);

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_OS_ERR_NONE;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        case OS_ERR_PEND_ABORT:
        case OS_ERR_TIMEOUT:
        default:
            *perr = NET_OS_ERR_INIT;
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetOS_InitSignal()
*
* Description : Signal that network initialization is complete.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Network initialization     successfully signaled.
*                               NET_OS_ERR_INIT_SIGNALD         Network initialization NOT successfully signaled.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network initialization MUST be signaled--i.e. MUST signal without failure.
*
*                   Failure to signal will prevent network task(s) from running.
*********************************************************************************************************
*/

void  NetOS_InitSignal (NET_ERR  *perr)
{
    INT8U  os_err;


    os_err = OSSemPost(NetOS_InitSignalPtr);                    /* Signal network initialization complete.              */
    if (os_err !=  OS_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_SIGNALD;
    }

    *perr = NET_OS_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetOS_Lock()
*
* Description : Acquire mutually exclusive access to network protocol suite.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Network access     acquired.
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network access MUST be acquired--i.e. MUST wait for access; do NOT timeout.
*
*                   Failure to acquire network access will prevent network task(s)/operation(s)
*                   from functioning.
*********************************************************************************************************
*/

void  NetOS_Lock (NET_ERR  *perr)
{
    INT8U  os_err;


    OSSemPend((OS_EVENT *) NetOS_LockPtr,                       /* Acquire network access ...                           */
              (INT16U    ) 0,                                   /* ... without timeout (see Note #1).                   */
              (INT8U    *)&os_err);

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_OS_ERR_NONE;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        case OS_ERR_PEND_ABORT:
        case OS_ERR_TIMEOUT:
        default:
            *perr = NET_OS_ERR_LOCK;
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetOS_Unlock()
*
* Description : Release mutually exclusive access to network protocol suite.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network access MUST be released--i.e. MUST unlock access without failure.
*
*                   Failure to release network access will prevent network task(s)/operation(s)
*                   from functioning.
*********************************************************************************************************
*/

void  NetOS_Unlock (void)
{
   (void)OSSemPost(NetOS_LockPtr);                              /* Release network access.                              */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_TxSuspendWait()
*
* Description : Wait on network transmit suspend signal.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Net_TxSuspend().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network transmit suspend waits until :
*
*                   (a) Signaled
*                   (b) Timed out
*                   (c) Any OS fault occurs
*
*               (2) Implement timeout with OS-dependent functionality.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
void  NetOS_TxSuspendWait (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  timeout_os_tick;
    INT8U   os_err;


    CPU_CRITICAL_ENTER();
    timeout_os_tick = NetOS_TxSuspendTimeout_tick;
    CPU_CRITICAL_EXIT();
                                                                /* Wait on network transmit suspend signal.               */
    OSSemPend(NetOS_TxSuspendSignalPtr, timeout_os_tick, &os_err);

   (void)&os_err;                                               /* See Note #1c.                                          */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_TxSuspendSignal()
*
* Description : Signal network transmit suspend.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Net_TxSuspendSignal().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Failure to signal network transmit suspend signal will cause network transmit suspends
*                   to timeout.
*
*                   See also 'NetOS_TxSuspendWait()  Note #1b'.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
void  NetOS_TxSuspendSignal (void)
{
   (void)OSSemPost(NetOS_TxSuspendSignalPtr);                   /* Signal network transmit suspend.                     */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_TxSuspendTimeoutSet()
*
* Description : Set network transmit suspend timeout value.
*
* Argument(s) : timeout_ms      Timeout value (in milliseconds).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_NONE                    Network transmit suspend timeout successfully set.
*
*                                                               --- RETURNED BY NetOS_TimeoutCalc_OS_tick() : ----
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_Init(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) 'NetOS_TxSuspendTimeout_tick' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
void  NetOS_TxSuspendTimeoutSet (CPU_INT32U   timeout_ms,
                                 NET_ERR     *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  timeout_os_tick;

                                                                /* Calculate timeout value (in OS ticks).               */
    timeout_os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, DEF_NO, perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
                                                                /* Set network transmit suspend timeout value ...       */
    NetOS_TxSuspendTimeout_tick = timeout_os_tick;              /* ... (in OS ticks).                                   */
    CPU_CRITICAL_EXIT();
    

   *perr = NET_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetOS_TxSuspendTimeoutGet_ms()
*
* Description : Get network transmit suspend timeout value.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_NONE                    Network transmit suspend timeout 
*                                                                   successfully returned.
*
* Return(s)   : Network transmit suspend timeout value (in milliseconds).
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) 'NetOS_TxSuspendTimeout_tick' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
CPU_INT32U  NetOS_TxSuspendTimeoutGet_ms (NET_ERR  *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      timeout_os_tick;
    CPU_INT32U  timeout_ms;


    CPU_CRITICAL_ENTER();
                                                                /* Get network transmit suspend timeout value ...       */
    timeout_os_tick = NetOS_TxSuspendTimeout_tick;              /* ... (in OS ticks).                                   */
    CPU_CRITICAL_EXIT();

    timeout_ms      = NetOS_TimeoutCalc_ms(timeout_os_tick);    /* Calculate timeout value (in milliseconds).           */

   *perr            = NET_ERR_NONE;

    return (timeout_ms);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                 NETWORK TIMER MANAGEMENT FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          NetOS_Tmr_Init()
*
* Description : (1) Perform Timer/OS initialization :
*
*                   (a) Create Network Timer Task
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Network timer/OS initialization successful.
*                               NET_OS_ERR_INIT_TMR_TASK        Network timer task      NOT successfully
*                                                                   initialized.
*                               NET_OS_ERR_INIT_TMR_TASK_NAME   Network timer task name NOT successfully
*                                                                   configured.
*
* Return(s)   : none.
*
* Caller(s)   : NetTmr_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetOS_Tmr_Init (NET_ERR  *perr)
{
    INT8U  os_err;

                                                                    /* Create network timer task.                       */
#if (OS_TASK_CREATE_EXT_EN == 1)

#if (OS_STK_GROWTH == 1)
    os_err = OSTaskCreateExt((void (*)(void *)) NetOS_Tmr_Task,
                             (void          * ) 0,
                             (OS_STK        * )&NetOS_Tmr_TaskStk[NET_OS_CFG_TMR_TASK_STK_SIZE - 1],
                             (INT8U           ) NET_OS_CFG_TMR_TASK_PRIO,
                             (INT16U          ) NET_OS_CFG_TMR_TASK_PRIO,
                             (OS_STK        * )&NetOS_Tmr_TaskStk[0],
                             (INT32U          ) NET_OS_CFG_TMR_TASK_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
    os_err = OSTaskCreateExt((void (*)(void *)) NetOS_Tmr_Task,
                             (void          * ) 0,
                             (OS_STK        * )&NetOS_Tmr_TaskStk[0],
                             (INT8U           ) NET_OS_CFG_TMR_TASK_PRIO,
                             (INT16U          ) NET_OS_CFG_TMR_TASK_PRIO,
                             (OS_STK        * )&NetOS_Tmr_TaskStk[NET_OS_CFG_TMR_TASK_STK_SIZE - 1],
                             (INT32U          ) NET_OS_CFG_TMR_TASK_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#endif

#else

#if (OS_STK_GROWTH == 1)
    os_err = OSTaskCreate((void (*)(void *)) NetOS_Tmr_Task,
                          (void          * ) 0,
                          (OS_STK        * )&NetOS_Tmr_TaskStk[NET_OS_CFG_TMR_TASK_STK_SIZE - 1],
                          (INT8U           ) NET_OS_CFG_TMR_TASK_PRIO);
#else
    os_err = OSTaskCreate((void (*)(void *)) NetOS_Tmr_Task,
                          (void          * ) 0,
                          (OS_STK        * )&NetOS_Tmr_TaskStk[0],
                          (INT8U           ) NET_OS_CFG_TMR_TASK_PRIO);
#endif

#endif

    if (os_err !=  OS_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_TMR_TASK;
        return;
    }


#if (OS_TASK_NAME_SIZE >= NET_TASK_NAME_SIZE_MAX)
    OSTaskNameSet((INT8U  ) NET_OS_CFG_TMR_TASK_PRIO,
                  (INT8U *) NET_TMR_TASK_NAME,
                  (INT8U *)&os_err);
    if (os_err !=  OS_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_TMR_TASK_NAME;
        return;
    }
#endif


   *perr = NET_OS_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetOS_Tmr_Task()
*
* Description : OS-dependent shell task to schedule & run Timer Task handler.
*
*               (1) Shell task's primary purpose is to schedule & run NetTmr_TaskHandler(); shell task
*                   should run NetTmr_TaskHandler() at NET_TMR_CFG_TASK_FREQ rate forever (i.e. shell 
*                   task should NEVER exit).
*
*
* Argument(s) : p_data      Pointer to task initialization data (required by uC/OS-II).
*
* Return(s)   : none.
*
* Created by  : NetOS_Tmr_Init().
*
* Note(s)     : (2) Assumes OS_TICKS_PER_SEC frequency is greater than NET_TMR_CFG_TASK_FREQ.  Otherwise,
*                   timer task scheduling rate will NOT be correct.
*********************************************************************************************************
*/

static  void  NetOS_Tmr_Task (void  *p_data)
{
    INT16U  dly;


   (void)&p_data;                                               /* Prevent compiler warning.                            */

    dly = OS_TICKS_PER_SEC / NET_TMR_CFG_TASK_FREQ;             /* Delay task at NET_TMR_CFG_TASK_FREQ rate.            */

    while (DEF_ON) {
        OSTimeDly(dly);

        NetTmr_TaskHandler();
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                  NETWORK INTERFACE CARD FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          NetOS_NIC_Init()
*
* Description : (1) Perform NIC/OS initialization :
*
*                   (a) Implement NIC transmit ready signal by creating a binary semaphore.
*
*                       Initialize transmit ready signal based on configuration :
*
*                       (1) NET_NIC_CFG_TX_RDY_INIT_VAL configured to initialize the NIC-transmit-ready
*                           OS object with one of the following values :
*
*                           (A) 0, for NIC driver's that implement the NIC's transmit-ready status using
*                                  the Transmit Empty    interrupt.
*
*                           (B) 1, for NIC driver's that implement the NIC's transmit-ready status using 
*                                  the Transmit Complete interrupt.
*
*                       See also 'net_cfg.h  NETWORK INTERFACE CARD CONFIGURATION  Note #2'.
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                     NIC/OS initialization successful.
*                               NET_OS_ERR_INIT_NIC_TX_RDY          NIC transmit ready signal NOT successfully
*                                                                       initialized.
*                               NET_OS_ERR_INIT_NIC_TX_RDY_NAME     NIC transmit ready name   NOT successfully
*                                                                       configured.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_Init().
*
*               This function is a network protocol suite to network interface controller (NIC)  function &
*               SHOULD be called only by appropriate network interface controller initialization function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetOS_NIC_Init (NET_ERR  *perr)
{
#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    INT8U  os_err;
#endif

                                                                /* Create transmit ready signal (see Note #1a).         */
    NetOS_NIC_TxRdySignalPtr = OSSemCreate((INT16U)NET_NIC_CFG_TX_RDY_INIT_VAL);        
    if (NetOS_NIC_TxRdySignalPtr == (OS_EVENT *)0) {
       *perr = NET_OS_ERR_INIT_NIC_TX_RDY;
        return;
    }

#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    OSEventNameSet((OS_EVENT *) NetOS_NIC_TxRdySignalPtr,
                   (INT8U    *) NET_NIC_TX_RDY_NAME,
                   (INT8U    *)&os_err);
    if (os_err !=  OS_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_NIC_TX_RDY_NAME;
        return;
    }
#endif


   *perr = NET_OS_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_NIC_TxRdyWait()
*
* Description : Wait on NIC transmit ready signal.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_NIC_ERR_NONE                        NIC transmit ready signal     received.
*                               NET_NIC_ERR_TX_RDY_SIGNAL_TIMEOUT       NIC transmit ready signal NOT received
*                                                                           by timeout.
*                               NET_NIC_ERR_TX_RDY_SIGNAL_FAULT         NIC transmit ready signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx(),
*               NetIF_Char_Tx().
*
*               This function is a network interface (IF) to network interface controller (NIC) function 
*               & SHOULD be called only by appropriate network interface function(s).
*
* Note(s)     : (1) (a) If timeouts NOT desired, wait on NIC transmit ready signal forever (i.e. do NOT exit).
*
*                   (b) If timeout      desired, return NET_NIC_ERR_TX_RDY_SIGNAL_TIMEOUT error on transmit 
*                       ready timeout.  Implement timeout with OS-dependent functionality.
*********************************************************************************************************
*/

void  NetOS_NIC_TxRdyWait (NET_ERR  *perr)
{
    INT8U  os_err;


    OSSemPend((OS_EVENT *) NetOS_NIC_TxRdySignalPtr,            /* Wait on NIC transmit ready signal ...                */
              (INT16U    ) 0,                                   /* ... preferably without timeout (see Note #1a).       */
              (INT8U    *)&os_err);

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_NIC_ERR_NONE;
             break;


        case OS_ERR_TIMEOUT:
            *perr = NET_NIC_ERR_TX_RDY_SIGNAL_TIMEOUT;          /* See Note #1b.                                        */
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        case OS_ERR_PEND_ABORT:
        default:
            *perr = NET_NIC_ERR_TX_RDY_SIGNAL_FAULT;
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_NIC_TxRdySignal()
*
* Description : Signal that NIC transmit is ready.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_TxISR_Handler().
*
*               This function is a network protocol suite to network interface controller (NIC) function
*               & SHOULD be called only by appropriate network interface controller function(s).
*
* Note(s)     : (1) #### NIC transmit ready MUST be signaled--i.e. MUST signal without failure.
*
*                   Failure to signal NIC transmit ready will prevent NIC from transmitting packets.
*********************************************************************************************************
*/

void  NetOS_NIC_TxRdySignal (void)
{
    INT8U  os_err;


    os_err = OSSemPost(NetOS_NIC_TxRdySignalPtr);               /* Signal NIC transmit ready.                           */

   (void)&os_err;                                               /* See Note #1.                                         */
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                  NETWORK INTERFACE LAYER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           NetOS_IF_Init()
*
* Description : (1) Perform network interface/OS initialization :
*
*                   (a) (1) Implement network interface receive queue by creating a counting semaphore.
*
*                           (A) Initialize network interface receive queue with no received packets by 
*                               setting the semaphore count to 0 to block the semaphore.
*
*                       (2) Create Network Interface Receive Task
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                     Network interface/OS initialization
*                                                                           successful.
*                               NET_OS_ERR_INIT_IF_RX_Q             Network interface receive queue signal
*                                                                       NOT successfully initialized.
*                               NET_OS_ERR_INIT_IF_RX_Q_NAME        Network interface receive queue name
*                                                                       NOT successfully configured.
*                               NET_OS_ERR_INIT_IF_RX_TASK          Network interface receive task
*                                                                       NOT successfully initialized.
*                               NET_OS_ERR_INIT_IF_RX_TASK_NAME     Network interface receive task  name
*                                                                       NOT successfully configured.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Init(),
*               NetIF_Char_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetOS_IF_Init (NET_ERR  *perr)
{
    INT8U  os_err;


                                                            /* Create network interface receive queue (see Note #1a1).  */
    NetOS_IF_RxQ_SignalPtr = OSSemCreate((INT16U)0);
    if (NetOS_IF_RxQ_SignalPtr == (OS_EVENT *)0) {
       *perr = NET_OS_ERR_INIT_IF_RX_Q;
        return;
    }

#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    OSEventNameSet((OS_EVENT *) NetOS_IF_RxQ_SignalPtr,
                   (INT8U    *) NET_IF_RX_Q_NAME,
                   (INT8U    *)&os_err);
    if (os_err !=  OS_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_IF_RX_Q_NAME;
        return;
    }
#endif


                                                            /* Create network interface receive task.                   */
#if (OS_TASK_CREATE_EXT_EN == 1)

#if (OS_STK_GROWTH == 1)
    os_err = OSTaskCreateExt((void (*)(void *)) NetOS_IF_RxTask,
                             (void          * ) 0,
                             (OS_STK        * )&NetOS_IF_RxTaskStk[NET_OS_CFG_IF_RX_TASK_STK_SIZE - 1],
                             (INT8U           ) NET_OS_CFG_IF_RX_TASK_PRIO,
                             (INT16U          ) NET_OS_CFG_IF_RX_TASK_PRIO,
                             (OS_STK        * )&NetOS_IF_RxTaskStk[0],
                             (INT32U          ) NET_OS_CFG_IF_RX_TASK_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
    os_err = OSTaskCreateExt((void (*)(void *)) NetOS_IF_RxTask,
                             (void          * ) 0,
                             (OS_STK        * )&NetOS_IF_RxTaskStk[0],
                             (INT8U           ) NET_OS_CFG_IF_RX_TASK_PRIO,
                             (INT16U          ) NET_OS_CFG_IF_RX_TASK_PRIO,
                             (OS_STK        * )&NetOS_IF_RxTaskStk[NET_OS_CFG_IF_RX_TASK_STK_SIZE - 1],
                             (INT32U          ) NET_OS_CFG_IF_RX_TASK_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#endif

#else

#if (OS_STK_GROWTH == 1)
    os_err = OSTaskCreate((void (*)(void *)) NetOS_IF_RxTask,
                          (void          * ) 0,
                          (OS_STK        * )&NetOS_IF_RxTaskStk[NET_OS_CFG_IF_RX_TASK_STK_SIZE - 1],
                          (INT8U           ) NET_OS_CFG_IF_RX_TASK_PRIO);
#else
    os_err = OSTaskCreate((void (*)(void *)) NetOS_IF_RxTask,
                          (void          * ) 0,
                          (OS_STK        * )&NetOS_IF_RxTaskStk[0],
                          (INT8U           ) NET_OS_CFG_IF_RX_TASK_PRIO);
#endif

#endif

    if (os_err !=  OS_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_IF_RX_TASK;
        return;
    }


#if (OS_TASK_NAME_SIZE >= NET_TASK_NAME_SIZE_MAX)
    OSTaskNameSet((INT8U  ) NET_OS_CFG_IF_RX_TASK_PRIO,
                  (INT8U *) NET_IF_RX_TASK_NAME,
                  (INT8U *)&os_err);
    if (os_err !=  OS_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_IF_RX_TASK_NAME;
        return;
    }
#endif


   *perr = NET_OS_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_IF_RxTask()
*
* Description : OS-dependent shell task to schedule & run Network Interface Receive Task handler.
*
*               (1) Shell task's primary purpose is to schedule & run NetIF_RxTaskHandler() forever;
*                   (i.e. shell task should NEVER exit).
*
*
* Argument(s) : p_data      Pointer to task initialization data (required by uC/OS-II).
*
* Return(s)   : none.
*
* Created by  : NetOS_IF_Init().
*
* Note(s)     : (2) To prevent deadlocking any lower priority task(s), network tasks SHOULD delay for a 
*                   (brief) time after any network task handlers exit.
*********************************************************************************************************
*/

static  void  NetOS_IF_RxTask (void  *p_data)
{
   (void)&p_data;                                               /* Prevent compiler warning.                            */

    NetIF_RxTaskHandlerInit();
    while (DEF_ON) {
        NetIF_RxTaskHandler();
        OSTimeDly(1);                                           /* Dly for lower prio task(s) [see Note #2].            */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_IF_RxTaskWait()
*
* Description : Wait on network interface receive queue for NIC receive signal.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 Signal received from NIC receive ISR.
*                               NET_IF_ERR_RX_Q_EMPTY           Network interface receive queue empty.
*                               NET_IF_ERR_RX_Q_SIGNAL_FAULT    Network interface receive queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) (a) If timeouts NOT desired, wait on network interface receive queue until signaled
*                       (i.e. do NOT exit).
*
*                   (b) If timeout      desired, return NET_IF_ERR_RX_Q_EMPTY error on receive queue 
*                       timeout.  Implement timeout with OS-dependent functionality.
*********************************************************************************************************
*/

void  NetOS_IF_RxTaskWait (NET_ERR  *perr)
{
    INT8U  os_err;


    OSSemPend((OS_EVENT *) NetOS_IF_RxQ_SignalPtr,              /* Wait on network interface receive queue ...          */
              (INT16U    ) 0,                                   /* ... preferably without timeout (see Note #1a).       */
              (INT8U    *)&os_err);

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_IF_ERR_NONE;
             break;


        case OS_ERR_TIMEOUT:
            *perr = NET_IF_ERR_RX_Q_EMPTY;                      /* See Note #1b.                                        */
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        case OS_ERR_PEND_LOCKED:
        case OS_ERR_PEND_ABORT:
        default:
            *perr = NET_IF_ERR_RX_Q_SIGNAL_FAULT;
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_IF_RxTaskSignal()
*
* Description : Signal network interface receive task of NIC receive ISR.
*
* Argument(s) : psignal     Pointer to signal value to post to network interface receive queue.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 Network interface receive queue successfully
*                                                                   signaled.
*                               NET_IF_ERR_RX_Q_FULL            Network interface receive queue full.
*                               NET_IF_ERR_RX_Q_SIGNAL_FAULT    Network interface receive queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NIC driver receive ISR handler(s).
*
*               This function is a network protocol suite to network interface controller (NIC) function
*               & SHOULD be called only by appropriate network interface controller function(s).
*
* Note(s)     : (1) To balance network receive versus transmit packet loads for certain network connection
*                   types (e.g. stream-type connections), network receive & transmit packets SHOULD be 
*                   handled in an APPROXIMATELY balanced ratio.
*
*                   (b) To implement network receive versus transmit load balancing :
*
*                       (1) The availability of network receive packets MUST be managed at the network 
*                           interface layer :
*
*                           (A) Increment the number of available network receive packets queued for
*                               each network packet received.
*
*                   See also 'net.h  NETWORK RECEIVE PACKET MACRO'S  Note #1'.
*********************************************************************************************************
*/

void  NetOS_IF_RxTaskSignal (NET_ERR  *perr)
{
#if ((NET_CFG_LOAD_BAL_EN     == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif
    INT8U   os_err;


    os_err = OSSemPost(NetOS_IF_RxQ_SignalPtr);                 /* Signal network interface receive queue.              */

    switch (os_err) {
        case OS_ERR_NONE:
                                                                /* Increment number of queued receive packets ...       */
             NET_RX_PKT_INC();                                  /* ... available (see Note #1b1A).                      */
            *perr = NET_IF_ERR_NONE;
             break;


        case OS_SEM_OVF:
            *perr = NET_IF_ERR_RX_Q_FULL;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_IF_ERR_RX_Q_SIGNAL_FAULT;
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                          INTERNET CONTROL MESSAGE PROTOCOL LAYER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        NetOS_ICMP_TxMsgReq()
*
* Description : Transmit ICMP Request Message.
*
*               (1) NetOS_ICMP_TxMsgReq() is the correct API function for network & end-user applications to
*                   transmit ICMP Request Messages (see also 'net_icmp.c  NetICMP_TxMsgReq()  Note #3').
*
*               (2) See 'net_icmp.h  ICMP MESSAGE TYPES & CODES' for supported ICMP message types & codes.
*
*
* Argument(s) : type        ICMP Request Message type (see Note #2) :
*
*                               NET_ICMP_MSG_TYPE_ECHO_REQ
*                               NET_ICMP_MSG_TYPE_TS_REQ
*                               NET_ICMP_MSG_TYPE_ADDR_MASK_REQ
*
*               code        ICMP Request Message code (see Note #2).
*
*               addr_src    Source      IP address.
*
*               addr_dest   Destination IP address.
*
*               TOS         Specific TOS to transmit ICMP/IP packet
*                               (see 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
*
*               TTL         Specific TTL to transmit ICMP/IP packet
*                               (see 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES') :
*
*                               NET_IP_HDR_TTL_MIN        1     minimum TTL transmit value
*                               NET_IP_HDR_TTL_MAX      255     maximum TTL transmit value
*                               NET_IP_HDR_TTL_DFLT             default TTL transmit value
*                               NET_IP_HDR_TTL_NONE       0     replace with default TTL
*
*               flags       Flags to select transmit options; bit-field flags logically OR'd :
*
*                               NET_IP_FLAG_NONE                No  IP transmit flags selected.
*                               NET_IP_FLAG_TX_DONT_FRAG        Set IP 'Don't Frag' flag.
*
*               popts       Pointer to one or more IP options configuration data structures
*                               (see 'net_ip.h  IP HEADER OPTION CONFIGURATION DATA TYPES') :
*
*                               NULL                            NO IP transmit options configuration.
*                               NET_IP_OPT_CFG_ROUTE_TS         Route &/or Internet Timestamp options configuration.
*                               NET_IP_OPT_CFG_SECURITY         Security options configuration
*                                                                   (see 'net_ip.h  Note #1f').
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               ------ RETURNED BY NetICMP_TxMsgReq() : ------
*                               NET_ICMP_ERR_NONE               ICMP Request Message successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : ICMP Request Message's Identification (ID) & Sequence Numbers, if NO errors.
*
*               NULL                   Identification (ID) & Sequence Numbers, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (3) (a) RFC #792 states that the Identifier field is an "aid in matching [requests] and
*                       replies, may be zero ... For example, the identifier might be used like a port
*                       in TCP or UDP to identify a session" (Sections 'Echo or Echo Reply Message :
*                       Identifier, Description' & 'Timestamp or Timestamp Reply Message : Identifier,
*                       Description').
*
*                   (b) Use uC/OS-II task priority number as ICMP Request Message Identification field.
*
*               (4) #### ICMP Receive Error/Reply Messages NOT yet implemented.
*
*                   See also 'net_icmp.c  NetICMP_Rx()  Note #4'.
*********************************************************************************************************
*/
/*$PAGE*/
NET_ICMP_REQ_ID_SEQ  NetOS_ICMP_TxMsgReq (CPU_INT08U    type,
                                          CPU_INT08U    code,
                                          NET_IP_ADDR   addr_src,
                                          NET_IP_ADDR   addr_dest,
                                          NET_IP_TOS    TOS,
                                          NET_IP_TTL    TTL,
                                          CPU_INT16U    flags,
                                          void         *popts,
                                          void         *p_data,
                                          CPU_INT16U    data_len,
                                          NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR               cpu_sr;
#endif
    CPU_INT16U           id;
    NET_ICMP_REQ_ID_SEQ  id_seq;


    CPU_CRITICAL_ENTER();
    id     = (CPU_INT16U)OSTCBCur->OSTCBPrio;                   /* Set task prio as ICMP Req Msg id (see Note #3b).     */
    CPU_CRITICAL_EXIT();

    id_seq =  NetICMP_TxMsgReq((CPU_INT08U  )type,
                               (CPU_INT08U  )code,
                               (CPU_INT16U  )id,
                               (NET_IP_ADDR )addr_src,
                               (NET_IP_ADDR )addr_dest,
                               (NET_IP_TOS  )TOS,
                               (NET_IP_TTL  )TTL,
                               (CPU_INT16U  )flags,
                               (void       *)popts,
                               (void       *)p_data,
                               (CPU_INT16U  )data_len,
                               (NET_ERR    *)perr);

    return (id_seq);
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                            TRANSMISSION CONTROL PROTOCOL LAYER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          NetOS_TCP_Init()
*
* Description : (1) Perform TCP/OS initialization :
*
*                   (a) Create TCP connection receive  queue binary semaphores :
*
*                       (1) Initialize TCP connection receive queue binary semaphores with no received 
*                           packets  by setting the semaphore count to 0.
*                       (2) Initialize TCP connection receive queue timeout values.
*
*                   (b) Create TCP connection transmit queue binary semaphores :
*
*                       (1) Initialize TCP connection transmit queue binary semaphores with no transmit
*                           permissions by setting the semaphore count to 0.
*                       (2) Initialize TCP connection transmit queue timeout values.
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                     TCP/OS initialization successful.
*                               NET_OS_ERR_INIT_TCP_RX_Q            TCP receive  queue(s)         NOT 
*                                                                       successfully initialized.
*                               NET_OS_ERR_INIT_TCP_RX_Q_TIMEOUT    TCP receive  queue timeout(s) NOT 
*                                                                       successfully configured.
*                               NET_OS_ERR_INIT_TCP_TX_Q            TCP transmit queue(s)         NOT 
*                                                                       successfully initialized.
*                               NET_OS_ERR_INIT_TCP_TX_Q_TIMEOUT    TCP transmit queue timeout(s) NOT 
*                                                                       successfully configured.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_Init (NET_ERR  *perr)
{
    OS_EVENT          **psignal;
    NET_TCP_CONN_QTY    i;
    NET_ERR             net_err;

                                                            /* Initialize TCP connection queues (see Note #1).          */

    psignal = &NetOS_TCP_RxQ_SignalPtr[0];
    for (i = 0; i < NET_TCP_CFG_NBR_CONN; i++) {               
       *psignal = OSSemCreate((INT16U)0);                   /* Create     TCP connection receive queue semaphores.      */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_TCP_RX_Q;
             return;
        }
        psignal++;
                                                            /* Initialize TCP connection receive queue timeout values.  */
        NetOS_TCP_RxQ_TimeoutDflt(i, &net_err);
        if (net_err != NET_TCP_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_TCP_RX_Q_TIMEOUT;
            return;
        }
    }


    psignal = &NetOS_TCP_TxQ_SignalPtr[0];
    for (i = 0; i < NET_TCP_CFG_NBR_CONN; i++) {               
       *psignal = OSSemCreate((INT16U)0);                   /* Create     TCP connection transmit queue semaphores.     */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_TCP_TX_Q;
             return;
        }
        psignal++;
                                                            /* Initialize TCP connection transmit queue timeout values. */
        NetOS_TCP_TxQ_TimeoutDflt(i, &net_err);
        if (net_err != NET_TCP_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_TCP_TX_Q_TIMEOUT;
            return;
        }
    }


   *perr = NET_OS_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetOS_TCP_RxQ_Clr()
*
* Description : Clear TCP connection receive queue signal.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to clear receive queue signal.
*               -----------     Argument validated in NetTCP_RxAppData(),
*                                                     NetTCP_RxPktConnHandlerRxQ_AppData(),
*                                                     NetTCP_ConnFreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue signal 
*                                                                   successfully cleared.
*                               NET_TCP_ERR_RX_Q_SIGNAL_CLR     TCP connection receive queue signal
*                                                                   NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_RxAppData(),
*               NetTCP_RxPktConnHandlerRxQ_AppData(),
*               NetTCP_ConnFreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_RxQ_Clr (NET_TCP_CONN_ID   conn_id_tcp,
                         NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_TCP_RxQ_SignalPtr[conn_id_tcp];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear TCP connection receive queue signal.           */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_TCP_ERR_NONE;                       /* ... return NO error.                                 */
             } else {
                *perr = NET_TCP_ERR_RX_Q_SIGNAL_CLR;
             }
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_TCP_ERR_RX_Q_SIGNAL_CLR;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetOS_TCP_RxQ_Wait()
*
* Description : Wait on TCP connection receive queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to wait on receive queue.
*               -----------     Argument checked in NetTCP_RxAppData().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue   non-empty.
*                               NET_TCP_ERR_RX_Q_EMPTY          TCP connection receive queue still empty by
*                                                                   timeout.
*                               NET_TCP_ERR_RX_Q_SIGNAL_ABORT   TCP connection receive queue signal aborted;
*                                                                   TCP connection closed/aborted.
*                               NET_TCP_ERR_RX_Q_SIGNAL_FAULT   TCP connection receive queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_RxAppData().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) (a) If timeouts NOT desired, wait on TCP connection receive queue forever
*                       (i.e. do NOT exit).
*
*                   (b) If timeout      desired, return NET_TCP_ERR_RX_Q_EMPTY error on TCP connection 
*                       receive queue timeout.  Implement timeout with OS-dependent functionality.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_RxQ_Wait (NET_TCP_CONN_ID   conn_id_tcp,
                          NET_ERR          *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR     cpu_sr;
#endif
    OS_EVENT  *psignal;
    INT16U     timeout_os_tick;
    INT8U      os_err;


    psignal         = NetOS_TCP_RxQ_SignalPtr[conn_id_tcp];
    CPU_CRITICAL_ENTER();
    timeout_os_tick = NetOS_TCP_RxQ_TimeoutTbl_tick[conn_id_tcp];
    CPU_CRITICAL_EXIT();
    OSSemPend((OS_EVENT *) psignal,                             /* Wait on TCP connection receive queue ...             */
              (INT16U    ) timeout_os_tick,                     /* ... with configured timeout (see Note #1b).          */
              (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_ERR_TIMEOUT:
            *perr = NET_TCP_ERR_RX_Q_EMPTY;                     /* See Note #1b.                                        */
             break;


        case OS_ERR_PEND_ABORT:
            *perr = NET_TCP_ERR_RX_Q_SIGNAL_ABORT;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_TCP_ERR_RX_Q_SIGNAL_FAULT;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_TCP_RxQ_Signal()
*
* Description : Signal TCP connection receive queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to signal receive queue.
*               -----------     Argument validated in NetTCP_RxAppData(),
*                                                     NetTCP_RxPktConnHandlerRxQ_AppData().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue successfully
*                                                                   signaled.
*                               NET_TCP_ERR_RX_Q_FULL           TCP connection receive queue full.
*                               NET_TCP_ERR_RX_Q_SIGNAL_FAULT   TCP connection receive queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_RxAppData(),
*               NetTCP_RxPktConnHandlerRxQ_AppData().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_RxQ_Signal (NET_TCP_CONN_ID   conn_id_tcp,
                            NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_TCP_RxQ_SignalPtr[conn_id_tcp];
    os_err  = OSSemPost(psignal);                               /* Signal TCP connection receive queue.                 */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_SEM_OVF:
            *perr = NET_TCP_ERR_RX_Q_FULL;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_TCP_ERR_RX_Q_SIGNAL_FAULT;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_TCP_RxQ_Abort()
*
* Description : Abort wait on TCP connection receive queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to abort wait on socket receive queue.
*               -----------     Argument validated in NetTCP_ConnFreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                Wait on TCP connection receive queue 
*                                                                   successfully aborted.
*                               NET_TCP_ERR_RX_Q_ABORT          TCP connection receive queue abort failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_ConnFreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_RxQ_Abort (NET_TCP_CONN_ID   conn_id_tcp,
                           NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_TCP_RxQ_SignalPtr[conn_id_tcp];
   (void)OSSemPendAbort((OS_EVENT *) psignal,                   /* Abort wait on TCP connection receive queue ...       */
                        (INT8U     ) OS_PEND_OPT_BROADCAST,     /* ... for ALL waiting tasks.                           */
                        (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
        case OS_ERR_PEND_ABORT:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_TCP_ERR_RX_Q_ABORT;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_TCP_RxQ_TimeoutDflt()
*
* Description : Set TCP connection receive queue to configured-default timeout value.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to set receive queue configured-default
*   `           -----------         timeout.
*
*                               Argument checked in NetSock_CfgTimeoutRxQ_Dflt(),
*                                                   NetTCP_ConnClr(),
*                                                   NetOS_TCP_Init().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue configured-
*                                                                   default timeout successfully set.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutRxQ_Dflt(),
*               NetTCP_ConnClr(),
*               NetOS_TCP_Init().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutRxQ_Dflt()'.
*
* Note(s)     : (1) NetOS_TCP_RxQ_TimeoutDflt() is called by network protocol suite function(s) &
*                   may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutRxQ_Dflt()  Note #2'.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_RxQ_TimeoutDflt (NET_TCP_CONN_ID   conn_id_tcp,
                                 NET_ERR          *perr)
{
    CPU_INT32U  timeout_ms;
                                                                /* Set TCP connection receive queue timeout ...         */
                                                                /* ... to configured-default        timeout value.      */
    timeout_ms = NET_TCP_CFG_TIMEOUT_CONN_RX_Q_MS;

    NetOS_TCP_RxQ_TimeoutSet(conn_id_tcp, timeout_ms, perr);
    if (*perr != NET_TCP_ERR_NONE) {
         return;
    }

   *perr = NET_TCP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_TCP_RxQ_TimeoutSet()
*
* Description : Set TCP connection receive queue timeout value.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to set receive queue timeout.
*               -----------     Argument checked in NetSock_CfgTimeoutRxQ_Set(),
*                                                   NetOS_TCP_RxQ_TimeoutDflt().
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue timeout
*                                                                   successfully set.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutRxQ_Set(),
*               NetOS_TCP_RxQ_TimeoutDflt().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutRxQ_Set()'.
*
* Note(s)     : (1) NetOS_TCP_RxQ_TimeoutSet() is called by network protocol suite function(s) & may be
*                   called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutRxQ_Set()  Note #2'.
*
*               (2) 'NetOS_TCP_RxQ_TimeoutTbl_tick[]' variables MUST ALWAYS be accessed exclusively in 
*                    critical sections.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_RxQ_TimeoutSet (NET_TCP_CONN_ID   conn_id_tcp,
                                CPU_INT32U        timeout_ms,
                                NET_ERR          *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  timeout_os_tick;

                                                                /* Calculate timeout value (in OS ticks).               */
    timeout_os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, DEF_NO, perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
                                                                /* Set TCP connection receive queue timeout value ...   */
                                                                /* ... (in OS ticks).                                   */
    NetOS_TCP_RxQ_TimeoutTbl_tick[conn_id_tcp] = timeout_os_tick;
    CPU_CRITICAL_EXIT();

   *perr = NET_TCP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_TCP_RxQ_TimeoutGet_ms()
*
* Description : Get TCP connection receive queue timeout value.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to get receive queue timeout.
*               -----------     Argument checked in NetSock_CfgTimeoutRxQ_Get_ms().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue timeout 
*                                                                   successfully returned.
*
* Return(s)   : TCP connection receive queue network timeout value :
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : NetSock_CfgTimeoutRxQ_Get_ms().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutRxQ_Get_ms()'.
*
* Note(s)     : (1) NetOS_TCP_RxQ_TimeoutGet_ms() is called by network protocol suite function(s) & may 
*                   be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutRxQ_Get_ms()  Note #3'.
*
*               (2) 'NetOS_TCP_RxQ_TimeoutTbl_tick[]' variables MUST ALWAYS be accessed exclusively in 
*                    critical sections.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
CPU_INT32U  NetOS_TCP_RxQ_TimeoutGet_ms (NET_TCP_CONN_ID   conn_id_tcp,
                                         NET_ERR          *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      timeout_os_tick;
    CPU_INT32U  timeout_ms;


    CPU_CRITICAL_ENTER();
                                                                /* Get TCP connection receive queue timeout value ...   */
                                                                /* ... (in OS ticks).                                   */
    timeout_os_tick = NetOS_TCP_RxQ_TimeoutTbl_tick[conn_id_tcp];
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms      = NetOS_TimeoutCalc_ms(timeout_os_tick);    /* Calculate timeout value (in milliseconds).           */

   *perr            = NET_TCP_ERR_NONE;

    return (timeout_ms);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetOS_TCP_TxQ_Clr()
*
* Description : Clear TCP connection transmit queue signal.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to clear transmit queue signal.
*               -----------     Argument validated in NetTCP_TxConnAppData(),
*                                                     NetTCP_ConnFreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue signal 
*                                                                   successfully cleared.
*                               NET_TCP_ERR_TX_Q_SIGNAL_CLR     TCP connection transmit queue signal 
*                                                                   NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_TxConnAppData(),
*               NetTCP_ConnFreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_TxQ_Clr (NET_TCP_CONN_ID   conn_id_tcp,
                         NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_TCP_TxQ_SignalPtr[conn_id_tcp];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear TCP connection transmit queue signal.          */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_TCP_ERR_NONE;                       /* ... return NO error.                                 */
             } else {
                *perr = NET_TCP_ERR_TX_Q_SIGNAL_CLR;
             }
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_TCP_ERR_TX_Q_SIGNAL_CLR;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_TCP_TxQ_Wait()
*
* Description : Wait on TCP connection transmit queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to wait on transmit queue.
*               -----------     Argument checked in NetTCP_TxConnAppData().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue NOT full.
*                               NET_TCP_ERR_TX_Q_FULL           TCP connection transmit queue still full
*                                                                   by timeout.
*                               NET_TCP_ERR_TX_Q_SIGNAL_ABORT   TCP connection transmit queue signal aborted;
*                                                                   TCP connection closed/aborted.
*                               NET_TCP_ERR_TX_Q_SIGNAL_FAULT   TCP connection transmit queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_TxConnAppData().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) (a) If timeouts NOT desired, wait on TCP connection transmit queue forever
*                       (i.e. do NOT exit).
*
*                   (b) If timeout      desired, return NET_TCP_ERR_TX_Q_FULL error on TCP connection 
*                       transmit queue timeout.  Implement timeout with OS-dependent functionality.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_TxQ_Wait (NET_TCP_CONN_ID   conn_id_tcp,
                          NET_ERR          *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR     cpu_sr;
#endif
    OS_EVENT  *psignal;
    INT16U     timeout_os_tick;
    INT8U      os_err;


    psignal         = NetOS_TCP_TxQ_SignalPtr[conn_id_tcp];
    CPU_CRITICAL_ENTER();
    timeout_os_tick = NetOS_TCP_TxQ_TimeoutTbl_tick[conn_id_tcp];
    CPU_CRITICAL_EXIT();
    OSSemPend((OS_EVENT *) psignal,                             /* Wait on TCP connection transmit queue ...            */
              (INT16U    ) timeout_os_tick,                     /* ... with configured timeout (see Note #1b).          */
              (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_ERR_TIMEOUT:
            *perr = NET_TCP_ERR_TX_Q_FULL;                      /* See Note #1b.                                        */
             break;


        case OS_ERR_PEND_ABORT:
            *perr = NET_TCP_ERR_TX_Q_SIGNAL_ABORT;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_TCP_ERR_TX_Q_SIGNAL_FAULT;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_TCP_TxQ_Signal()
*
* Description : Signal TCP connection transmit queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to signal transmit queue.
*               -----------     Argument validated in NetTCP_TxConnWinSizeHandlerCfgd().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue successfully
*                                                                   signaled.
*                               NET_TCP_ERR_TX_Q_SIGNAL_FAULT   TCP connection transmit queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_TxConnWinSizeHandlerCfgd().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_TxQ_Signal (NET_TCP_CONN_ID   conn_id_tcp,
                            NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_TCP_TxQ_SignalPtr[conn_id_tcp];
    os_err  = OSSemPost(psignal);                               /* Signal TCP connection transmit queue.                */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_SEM_OVF:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_TCP_ERR_TX_Q_SIGNAL_FAULT;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_TCP_TxQ_Abort()
*
* Description : Abort wait on TCP connection transmit queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to abort wait on socket transmit queue.
*               -----------     Argument validated in NetTCP_ConnFreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                Wait on TCP connection transmit queue 
*                                                                   successfully aborted.
*                               NET_TCP_ERR_TX_Q_ABORT          TCP connection transmit queue abort failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_ConnFreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_TxQ_Abort (NET_TCP_CONN_ID   conn_id_tcp,
                           NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_TCP_TxQ_SignalPtr[conn_id_tcp];
   (void)OSSemPendAbort((OS_EVENT *) psignal,                   /* Abort wait on TCP connection transmit queue ...      */
                        (INT8U     ) OS_PEND_OPT_BROADCAST,     /* ... for ALL waiting tasks.                           */
                        (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
        case OS_ERR_PEND_ABORT:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_TCP_ERR_TX_Q_ABORT;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_TCP_TxQ_TimeoutDflt()
*
* Description : Set TCP connection transmit queue to configured-default timeout value.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to set transmit queue configured-default
*   `           -----------         timeout.
*
*                               Argument checked in NetSock_CfgTimeoutTxQ_Dflt(),
*                                                   NetTCP_ConnClr(),
*                                                   NetOS_TCP_Init().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue configured-
*                                                                   default timeout successfully set.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutTxQ_Dflt(),
*               NetTCP_ConnClr(),
*               NetOS_TCP_Init().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutTxQ_Dflt()'.
*
* Note(s)     : (1) NetOS_TCP_TxQ_TimeoutDflt() is called by network protocol suite function(s) &
*                   may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutTxQ_Dflt()  Note #3'.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_TxQ_TimeoutDflt (NET_TCP_CONN_ID   conn_id_tcp,
                                 NET_ERR          *perr)
{
    CPU_INT32U  timeout_ms;
                                                                /* Set TCP connection transmit queue timeout ...        */
                                                                /* ... to configured-default         timeout value.     */
    timeout_ms = NET_TCP_CFG_TIMEOUT_CONN_TX_Q_MS;

    NetOS_TCP_TxQ_TimeoutSet(conn_id_tcp, timeout_ms, perr);
    if (*perr != NET_TCP_ERR_NONE) {
         return;
    }

   *perr = NET_TCP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_TCP_TxQ_TimeoutSet()
*
* Description : Set TCP connection transmit queue timeout value.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to set transmit queue timeout.
*               -----------     Argument checked in NetSock_CfgTimeoutTxQ_Set(),
*                                                   NetOS_TCP_TxQ_TimeoutDflt().
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue timeout
*                                                                   successfully set.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutTxQ_Set(),
*               NetOS_TCP_TxQ_TimeoutDflt().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutTxQ_Set()'.
*
* Note(s)     : (1) NetOS_TCP_TxQ_TimeoutSet() is called by network protocol suite function(s) & may be
*                   called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutTxQ_Set()  Note #3'.
*
*               (2) 'NetOS_TCP_TxQ_TimeoutTbl_tick[]' variables MUST ALWAYS be accessed exclusively in 
*                    critical sections.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_TxQ_TimeoutSet (NET_TCP_CONN_ID   conn_id_tcp,
                                CPU_INT32U        timeout_ms,
                                NET_ERR          *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  timeout_os_tick;

                                                                /* Calculate timeout value (in OS ticks).               */
    timeout_os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, DEF_NO, perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
                                                                /* Set TCP connection transmit queue timeout value ...  */
                                                                /* ... (in OS ticks).                                   */
    NetOS_TCP_TxQ_TimeoutTbl_tick[conn_id_tcp] = timeout_os_tick;
    CPU_CRITICAL_EXIT();

   *perr = NET_TCP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_TCP_TxQ_TimeoutGet_ms()
*
* Description : Get TCP connection transmit queue timeout value.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to get transmit queue timeout.
*               -----------     Argument checked in NetSock_CfgTimeoutTxQ_Get_ms().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue timeout 
*                                                                   successfully returned.
*
* Return(s)   : TCP connection transmit queue network timeout value :
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : NetSock_CfgTimeoutTxQ_Get_ms().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutTxQ_Get_ms()'.
*
* Note(s)     : (1) NetOS_TCP_TxQ_TimeoutGet_ms() is called by network protocol suite function(s) & may 
*                   be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutTxQ_Get_ms()  Note #4'.
*
*               (2) 'NetOS_TCP_TxQ_TimeoutTbl_tick[]' variables MUST ALWAYS be accessed exclusively in 
*                    critical sections.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
CPU_INT32U  NetOS_TCP_TxQ_TimeoutGet_ms (NET_TCP_CONN_ID   conn_id_tcp,
                                         NET_ERR          *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      timeout_os_tick;
    CPU_INT32U  timeout_ms;


    CPU_CRITICAL_ENTER();
                                                                /* Get TCP connection transmit queue timeout value ...  */
                                                                /* ... (in OS ticks).                                   */
    timeout_os_tick = NetOS_TCP_TxQ_TimeoutTbl_tick[conn_id_tcp];
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms      = NetOS_TimeoutCalc_ms(timeout_os_tick);    /* Calculate timeout value (in milliseconds).           */

   *perr            = NET_TCP_ERR_NONE;

    return (timeout_ms);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                   NETWORK SOCKET LAYER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          NetOS_Sock_Init()
*
* Description : (1) Perform socket/OS initialization :
*
*                   (a) Create socket receive queue semaphores :
*
*                       (1) Initialize socket receive queue semaphores with no received packets by setting 
*                           the semaphore count to 0.
*                       (2) Initialize socket receive queue timeout values.
*
*                   (b) (1) Implement the following socket connection signals ... :
*
*                           (A) Socket connection request
*                           (B) Socket connection accept
*                           (C) Socket connection close
*
*                       (2) ... by creating binary semaphores :
*
*                           (A) Initialize socket connection signals as NOT signaled by setting the binary 
*                               semaphore counts to 0.
*                           (B) Initialize socket connection signal timeout values.
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                         Socket/OS initialization successful.
*                               NET_OS_ERR_INIT_SOCK_RX_Q               Socket receive queue(s)
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_SOCK_RX_Q_TIMEOUT       Socket receive queue timeout(s)
*                                                                           NOT successfully configured.
*                               NET_OS_ERR_INIT_SOCK_CONN               Socket connection request signal(s)
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_SOCK_CONN_TIMEOUT       Socket connection request signal timeout(s)
*                                                                           NOT successfully configured.
*                               NET_OS_ERR_INIT_SOCK_ACCEPT             Socket connection accept  signal(s)
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_SOCK_ACCEPT_TIMEOUT     Socket connection accept  signal timeout(s)
*                                                                           NOT successfully configured.
*                               NET_OS_ERR_INIT_SOCK_CLOSE              Socket connection close   signal(s)
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_SOCK_CLOSE_TIMEOUT      Socket connection close   signal timeout(s)
*                                                                           NOT successfully configured.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_Init (NET_ERR  *perr)
{
    OS_EVENT      **psignal;
    NET_SOCK_QTY    i;
    NET_ERR         net_err;


                                                                /* Initialize socket receive queues     (see Note #1a). */
    psignal = &NetOS_Sock_RxQ_SignalPtr[0];
    for (i = 0; i < NET_SOCK_CFG_NBR_SOCK; i++) {               
       *psignal = OSSemCreate((INT16U)0);                       /* Create     socket receive queue semaphores.          */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_SOCK_RX_Q;
             return;
        }
        psignal++;
                                                                /* Initialize socket receive queue timeout values.      */
        NetOS_Sock_RxQ_TimeoutDflt(i, &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_SOCK_RX_Q_TIMEOUT;
            return;
        }
    }



#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                                                                /* Initialize socket connection signals (see Note #1b). */

                                                                /* Initialize socket connection request signal.         */
    psignal = &NetOS_Sock_ConnReqSignalPtr[0];
    for (i = 0; i < NET_SOCK_CFG_NBR_SOCK; i++) {
       *psignal = OSSemCreate((INT16U)0);                       /* Create     socket connection signal semaphores.      */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_SOCK_CONN;
             return;
        }
        psignal++;
                                                                /* Initialize socket connection signal timeout values.  */
        NetOS_Sock_ConnReqTimeoutDflt(i, &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_SOCK_CONN_TIMEOUT;
            return;
        }
    }

                                                                /* Initialize socket connection accept  signal.         */
    psignal = &NetOS_Sock_ConnAcceptQ_SignalPtr[0];
    for (i = 0; i < NET_SOCK_CFG_NBR_SOCK; i++) {
       *psignal = OSSemCreate((INT16U)0);                       /* Create     socket connection signal semaphores.      */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_SOCK_ACCEPT;
             return;
        }
        psignal++;
                                                                /* Initialize socket connection signal timeout values.  */
        NetOS_Sock_ConnAcceptQ_TimeoutDflt(i, &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_SOCK_ACCEPT_TIMEOUT;
            return;
        }
    }

                                                                /* Initialize socket connection close   signal.         */
    psignal = &NetOS_Sock_ConnCloseSignalPtr[0];
    for (i = 0; i < NET_SOCK_CFG_NBR_SOCK; i++) {
       *psignal = OSSemCreate((INT16U)0);                       /* Create     socket connection signal semaphores.      */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_SOCK_CLOSE;
             return;
        }
        psignal++;
                                                                /* Initialize socket connection signal timeout values.  */
        NetOS_Sock_ConnCloseTimeoutDflt(i, &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_SOCK_CLOSE_TIMEOUT;
            return;
        }
    }
#endif


   *perr = NET_OS_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_Sock_RxQ_Clr()
*
* Description : Clear socket receive queue signal.
*
*               (1) Socket receive queue signals apply to the following socket type(s)/protocol(s) :
*
*                   (a) Datagram
*                       (1) UDP
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to clear receive queue signal.
*               -------     Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket receive queue signal successfully cleared.
*                               NET_SOCK_ERR_RX_Q_SIGNAL_CLR    Socket receive queue signal NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_RxQ_Clr (NET_SOCK_ID   sock_id,
                          NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_Sock_RxQ_SignalPtr[sock_id];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear socket receive queue signal.                   */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_SOCK_ERR_NONE;                      /* ... return NO error.                                 */
             } else {
                *perr = NET_SOCK_ERR_RX_Q_SIGNAL_CLR;
             }
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_RX_Q_SIGNAL_CLR;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_Sock_RxQ_Wait()
*
* Description : Wait on socket receive queue.
*
*               (1) Socket receive queue signals apply to the following socket type(s)/protocol(s) :
*
*                   (a) Datagram
*                       (1) UDP
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to wait on receive queue.
*               -------     Argument checked in NetSock_RxDataHandlerDatagram().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket receive queue non-empty.
*                               NET_SOCK_ERR_RX_Q_EMPTY             Socket receive queue still empty by
*                                                                       timeout.
*                               NET_SOCK_ERR_RX_Q_SIGNAL_ABORT      Socket receive queue signal aborted;
*                                                                       socket closed/aborted.
*                               NET_SOCK_ERR_RX_Q_SIGNAL_FAULT      Socket receive queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_RxDataHandlerDatagram().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) (a) If timeouts NOT desired, wait on socket receive queue forever (i.e. do NOT exit).
*
*                   (b) If timeout      desired, return NET_SOCK_ERR_RX_Q_EMPTY error on socket receive 
*                       queue timeout.  Implement timeout with OS-dependent functionality.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_RxQ_Wait (NET_SOCK_ID   sock_id,
                           NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR     cpu_sr;
#endif
    OS_EVENT  *psignal;
    INT16U     timeout_os_tick;
    INT8U      os_err;


    psignal         = NetOS_Sock_RxQ_SignalPtr[sock_id];
    CPU_CRITICAL_ENTER();
    timeout_os_tick = NetOS_Sock_RxQ_TimeoutTbl_tick[sock_id];
    CPU_CRITICAL_EXIT();
    OSSemPend((OS_EVENT *) psignal,                             /* Wait on socket receive queue ...                     */
              (INT16U    ) timeout_os_tick,                     /* ... with configured timeout (see Note #1b).          */
              (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TIMEOUT:
            *perr = NET_SOCK_ERR_RX_Q_EMPTY;                    /* See Note #2b.                                        */
             break;


        case OS_ERR_PEND_ABORT:
            *perr = NET_SOCK_ERR_RX_Q_SIGNAL_ABORT;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_SOCK_ERR_RX_Q_SIGNAL_FAULT;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_Sock_RxQ_Signal()
*
* Description : Signal socket receive queue.
*
*               (1) Socket receive queue signals apply to the following socket type(s)/protocol(s) :
*
*                   (a) Datagram
*                       (1) UDP
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to signal receive queue.
*               -------     Argument validated in NetSock_RxPktDemux(),
*                                                 NetSock_RxDataHandlerDatagram().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket receive queue successfully signaled.
*                               NET_SOCK_ERR_RX_Q_FULL              Socket receive queue full.
*                               NET_SOCK_ERR_RX_Q_SIGNAL_FAULT      Socket receive queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_RxPktDemux(),
*               NetSock_RxDataHandlerDatagram().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_RxQ_Signal (NET_SOCK_ID   sock_id,
                             NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_RxQ_SignalPtr[sock_id];
    os_err  = OSSemPost(psignal);                               /* Signal socket receive queue.                         */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_SEM_OVF:
            *perr = NET_SOCK_ERR_RX_Q_FULL;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_RX_Q_SIGNAL_FAULT;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_Sock_RxQ_Abort()
*
* Description : Abort wait on socket receive queue.
*
*               (1) Socket receive queue signals apply to the following socket type(s)/protocol(s) :
*
*                   (a) Datagram
*                       (1) UDP
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to abort wait on socket receive 
*               -------         queue.
*
*                           Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Wait on socket receive queue successfully 
*                                                                   aborted.
*                               NET_SOCK_ERR_RX_Q_ABORT         Socket receive queue abort failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_RxQ_Abort (NET_SOCK_ID   sock_id,
                            NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_RxQ_SignalPtr[sock_id];
   (void)OSSemPendAbort((OS_EVENT *) psignal,                   /* Abort wait on socket receive queue ...               */
                        (INT8U     ) OS_PEND_OPT_BROADCAST,     /* ... for ALL waiting tasks.                           */
                        (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
        case OS_ERR_PEND_ABORT:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_RX_Q_ABORT;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_Sock_RxQ_TimeoutDflt()
*
* Description : Set socket receive queue to configured-default timeout value.
*
*               (1) Socket receive queue timeouts apply to the following socket type(s)/protocol(s) :
*
*                   (a) Datagram
*                       (1) UDP
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to set receive queue configured-
*               -------         default timeout.
*
*                           Argument checked in NetSock_CfgTimeoutRxQ_Dflt(),
*                                               NetSock_Clr(),
*                                               NetOS_Sock_Init().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket receive queue configured-default timeout
*                                                                   successfully set.
*
*                                                               -- RETURNED BY NetOS_TimeoutCalc_OS_tick() : --
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutRxQ_Dflt(),
*               NetSock_Clr(),
*               NetOS_Sock_Init().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutRxQ_Dflt()'.
*
* Note(s)     : (2) NetOS_Sock_RxQ_TimeoutDflt() is called by network protocol suite function(s) &
*                   may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutRxQ_Dflt()  Note #2'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_RxQ_TimeoutDflt (NET_SOCK_ID   sock_id,
                                  NET_ERR      *perr)
{
    CPU_INT32U  timeout_ms;
                                                                /* Set socket receive queue  timeout ...                */
                                                                /* ... to configured-default timeout value.             */
    timeout_ms = NET_SOCK_CFG_TIMEOUT_RX_Q_MS;

    NetOS_Sock_RxQ_TimeoutSet(sock_id, timeout_ms, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return;
    }

   *perr = NET_SOCK_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_Sock_RxQ_TimeoutSet()
*
* Description : Set socket receive queue timeout value.
*
*               (1) Socket receive queue timeouts apply to the following socket type(s)/protocol(s) :
*
*                   (a) Datagram
*                       (1) UDP
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set receive queue timeout.
*               -------         Argument checked in NetSock_CfgTimeoutRxQ_Set(),
*                                                   NetOS_Sock_RxQ_TimeoutDflt().
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket receive queue timeout successfully set.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : --
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutRxQ_Set(),
*               NetOS_Sock_RxQ_TimeoutDflt().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutRxQ_Set()'.
*
* Note(s)     : (2) NetOS_Sock_RxQ_TimeoutSet() is called by network protocol suite function(s) & may be 
*                   called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutRxQ_Set()  Note #2'.
*
*               (3) 'NetOS_Sock_RxQ_TimeoutTbl_tick[]' variables MUST ALWAYS be accessed exclusively in 
*                    critical sections.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_RxQ_TimeoutSet (NET_SOCK_ID   sock_id,
                                 CPU_INT32U    timeout_ms,
                                 NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  timeout_os_tick;

                                                                /* Calculate timeout value (in OS ticks).               */
    timeout_os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, DEF_NO, perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
                                                                /* Set socket receive queue timeout value ...           */
    NetOS_Sock_RxQ_TimeoutTbl_tick[sock_id] = timeout_os_tick;  /* ... (in OS ticks).                                   */
    CPU_CRITICAL_EXIT();

   *perr = NET_SOCK_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetOS_Sock_RxQ_TimeoutGet_ms()
*
* Description : Get socket receive queue timeout value.
*
*               (1) Socket receive queue timeouts apply to the following socket type(s)/protocol(s) :
*
*                   (a) Datagram
*                       (1) UDP
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get receive queue timeout.
*               -------     Argument checked in NetSock_CfgTimeoutRxQ_Get_ms().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket receive queue timeout successfully
*                                                                   returned.
*
* Return(s)   : Socket receive queue network timeout value  :
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : NetSock_CfgTimeoutRxQ_Get_ms().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutRxQ_Get_ms()'.
*
* Note(s)     : (2) NetOS_Sock_RxQ_TimeoutGet_ms() is called by network protocol suite function(s) & may 
*                   be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutRxQ_Get_ms()  Note #3'.
*
*               (3) 'NetOS_Sock_RxQ_TimeoutTbl_tick[]' variables MUST ALWAYS be accessed exclusively in 
*                    critical sections.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
CPU_INT32U  NetOS_Sock_RxQ_TimeoutGet_ms (NET_SOCK_ID   sock_id,
                                          NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      timeout_os_tick;
    CPU_INT32U  timeout_ms;


    CPU_CRITICAL_ENTER();
                                                                /* Get socket receive queue timeout value ...           */
    timeout_os_tick = NetOS_Sock_RxQ_TimeoutTbl_tick[sock_id];  /* ... (in OS ticks).                                   */
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms      = NetOS_TimeoutCalc_ms(timeout_os_tick);    /* Calculate timeout value (in milliseconds).           */

   *perr            = NET_TCP_ERR_NONE;

    return (timeout_ms);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_Sock_ConnReqClr()
*
* Description : Clear socket connection request signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to clear connection request signal.
*               -------     Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection request signal successfully cleared.
*                               NET_SOCK_ERR_CONN_SIGNAL_CLR    Socket connection request signal NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqClr (NET_SOCK_ID   sock_id,
                             NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnReqSignalPtr[sock_id];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear socket connection request signal.              */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_SOCK_ERR_NONE;                      /* ... return NO error.                                 */
             } else {
                *perr = NET_SOCK_ERR_CONN_SIGNAL_CLR;
             }
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_CLR;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetOS_Sock_ConnReqWait()
*
* Description : Wait on socket connection request signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to wait on connection request signal.
*               -------     Argument checked in NetSock_ConnHandlerStream().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection request successfully
*                                                                       signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT    Socket connection request NOT signaled
*                                                                       by timeout.
*                               NET_SOCK_ERR_CONN_SIGNAL_ABORT      Socket connection request signal aborted;
*                                                                       socket closed/aborted.
*                               NET_SOCK_ERR_CONN_SIGNAL_FAULT      Socket connection request signal fault.

*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnHandlerStream().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) (a) If timeouts NOT desired, wait on socket connection request signal forever
*                       (i.e. do NOT exit).
*
*                   (b) If timeout      desired, return NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT error on socket 
*                       connection request timeout.  Implement timeout with OS-dependent functionality.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqWait (NET_SOCK_ID   sock_id,
                              NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR     cpu_sr;
#endif
    OS_EVENT  *psignal;
    INT16U     timeout_os_tick;
    INT8U      os_err;


    psignal         = NetOS_Sock_ConnReqSignalPtr[sock_id];
    CPU_CRITICAL_ENTER();
    timeout_os_tick = NetOS_Sock_ConnReqTimeoutTbl_tick[sock_id];
    CPU_CRITICAL_EXIT();
    OSSemPend((OS_EVENT *) psignal,                             /* Wait on socket connection request signal ...         */
              (INT16U    ) timeout_os_tick,                     /* ... with configured timeout (see Note #1b).          */
              (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TIMEOUT:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT;           /* See Note #1b.                                        */
             break;


        case OS_ERR_PEND_ABORT:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_ABORT;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_FAULT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_Sock_ConnReqSignal()
*
* Description : Signal socket that connection request complete; socket now connected.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to signal connection request complete.
*               -------     Argument checked in NetSock_ConnSignalReq().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection request successfully 
*                                                                       signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL_FAULT      Socket connection request signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnSignalReq().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqSignal (NET_SOCK_ID   sock_id,
                                NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnReqSignalPtr[sock_id];
    os_err  = OSSemPost(psignal);                               /* Signal socket connection.                            */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_SEM_OVF:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_FAULT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetOS_Sock_ConnReqAbort()
*
* Description : Abort wait on socket connection request signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to abort wait on connection request 
*               -------         signal.
*
*                           Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Wait on socket connection request signal 
*                                                                   successfully aborted.
*                               NET_SOCK_ERR_CONN_ABORT         Socket connection request signal abort
*                                                                   failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqAbort (NET_SOCK_ID   sock_id,
                               NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnReqSignalPtr[sock_id];
   (void)OSSemPendAbort((OS_EVENT *) psignal,                   /* Abort wait on socket connection request signal ...   */
                        (INT8U     ) OS_PEND_OPT_BROADCAST,     /* ... for ALL waiting tasks.                           */
                        (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
        case OS_ERR_PEND_ABORT:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_CONN_ABORT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetOS_Sock_ConnReqTimeoutDflt()
*
* Description : Set socket connection request signal configured-default timeout value.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to set connection request signal
*               -------         configured-default timeout.
*
*                           Argument checked in NetSock_CfgTimeoutConnReqDflt(),
*                                               NetSock_Clr(),
*                                               NetOS_Sock_Init().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection request signal configured-
*                                                                   default timeout successfully set.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutConnReqDflt(),
*               NetSock_Clr(),
*               NetOS_Sock_Init().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutConnReqDflt()'.
*
* Note(s)     : (1) NetOS_Sock_ConnReqTimeoutDflt() is called by network protocol suite function(s)
*                   & may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutConnReqDflt()  Note #2'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqTimeoutDflt (NET_SOCK_ID   sock_id,
                                     NET_ERR      *perr)
{
    CPU_INT32U  timeout_ms;
                                                                /* Set socket connection request timeout ...            */
                                                                /* ... to configured-default     timeout value.         */
    timeout_ms = NET_SOCK_CFG_TIMEOUT_CONN_REQ_MS;

    NetOS_Sock_ConnReqTimeoutSet(sock_id, timeout_ms, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return;
    }

   *perr = NET_SOCK_ERR_NONE;
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetOS_Sock_ConnReqTimeoutSet()
*
* Description : Set socket connection request signal timeout value.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set connection request 
*               -------             signal timeout.
*
*                               Argument checked in NetSock_CfgTimeoutConnReqSet(),
*                                                   NetOS_Sock_ConnReqTimeoutDflt().
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection request signal timeout
*                                                                   successfully set.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutConnReqSet(),
*               NetOS_Sock_ConnReqTimeoutDflt().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutConnReqSet()'.
*
* Note(s)     : (1) NetOS_Sock_ConnReqTimeoutSet() is called by network protocol suite function(s) & 
*                   may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutConnReqSet()  Note #2'.
*
*               (2) 'NetOS_Sock_ConnReqTimeoutTbl_tick[]' variables MUST ALWAYS be accessed exclusively 
*                    in critical sections.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqTimeoutSet (NET_SOCK_ID   sock_id,
                                    CPU_INT32U    timeout_ms,
                                    NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  timeout_os_tick;

                                                                /* Calculate timeout value (in OS ticks).               */
    timeout_os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, DEF_NO, perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
                                                                /* Set socket connection request timeout value ...      */
                                                                /* ... (in OS ticks).                                   */
    NetOS_Sock_ConnReqTimeoutTbl_tick[sock_id] = timeout_os_tick;
    CPU_CRITICAL_EXIT();

   *perr = NET_SOCK_ERR_NONE;
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetOS_Sock_ConnReqTimeoutGet_ms()
*
* Description : Get socket connection request signal timeout value.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get connection request signal
*               -------         timeout.
*
*                           Argument checked in NetSock_CfgTimeoutConnReqGet_ms().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection request signal timeout 
*                                                                   successfully returned.
*
* Return(s)   : Socket connection request network timeout value  :
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : NetSock_CfgTimeoutConnReqGet_ms().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutConnReqGet_ms()'.
*
* Note(s)     : (1) NetOS_Sock_ConnReqTimeoutGet_ms() is called by network protocol suite function(s) & 
*                   may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutConnReqGet_ms()  Note #3'.
*
*               (2) 'NetOS_Sock_ConnReqTimeoutTbl_tick[]' variables MUST ALWAYS be accessed exclusively 
*                    in critical sections.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
CPU_INT32U  NetOS_Sock_ConnReqTimeoutGet_ms (NET_SOCK_ID   sock_id,
                                             NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      timeout_os_tick;
    CPU_INT32U  timeout_ms;


    CPU_CRITICAL_ENTER();
                                                                /* Get socket connection request timeout value ...      */
                                                                /* ... (in OS ticks).                                   */
    timeout_os_tick = NetOS_Sock_ConnReqTimeoutTbl_tick[sock_id];
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms      = NetOS_TimeoutCalc_ms(timeout_os_tick);    /* Calculate timeout value (in milliseconds).           */

   *perr            = NET_TCP_ERR_NONE;

    return (timeout_ms);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_Sock_ConnAcceptQ_Clr()
*
* Description : Clear socket connection accept queue signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to clear connection accept 
*               -------         queue signal.
*
*                           Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection accept queue signal 
*                                                                   successfully cleared.
*                               NET_SOCK_ERR_CONN_SIGNAL_CLR    Socket connection accept queue signal 
*                                                                   NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnAcceptQ_Clr (NET_SOCK_ID   sock_id,
                                  NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnAcceptQ_SignalPtr[sock_id];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear socket connection accept queue signal.         */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_SOCK_ERR_NONE;                      /* ... return NO error.                                 */
             } else {
                *perr = NET_SOCK_ERR_CONN_SIGNAL_CLR;
             }
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_CLR;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_Sock_ConnAcceptQ_Wait()
*
* Description : Wait on socket connection accept queue signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to wait on connection accept 
*               -------         queue signal.
*
*                           Argument checked in NetSock_Accept().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection accept queue successfully
*                                                                       signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT    Socket connection accept queue NOT signaled
*                                                                       by timeout.
*                               NET_SOCK_ERR_CONN_SIGNAL_ABORT      Socket connection accept queue signal aborted;
*                                                                       socket closed/aborted.
*                               NET_SOCK_ERR_CONN_SIGNAL_FAULT      Socket connection accept queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Accept().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) (a) If timeouts NOT desired, wait on socket connection accept queue signal forever
*                       (i.e. do NOT exit).
*
*                   (b) If timeout      desired, return NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT error on socket 
*                       connection accept queue timeout.  Implement timeout with OS-dependent functionality.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnAcceptQ_Wait (NET_SOCK_ID   sock_id,
                                   NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR     cpu_sr;
#endif
    OS_EVENT  *psignal;
    INT16U     timeout_os_tick;
    INT8U      os_err;


    psignal         = NetOS_Sock_ConnAcceptQ_SignalPtr[sock_id];
    CPU_CRITICAL_ENTER();
    timeout_os_tick = NetOS_Sock_ConnAcceptQ_TimeoutTbl_tick[sock_id];
    CPU_CRITICAL_EXIT();
    OSSemPend((OS_EVENT *) psignal,                             /* Wait on socket connection accept queue signal ...    */
              (INT16U    ) timeout_os_tick,                     /* ... with configured timeout (see Note #1b).          */
              (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TIMEOUT:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT;           /* See Note #1b.                                        */
             break;


        case OS_ERR_PEND_ABORT:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_ABORT;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_FAULT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetOS_Sock_ConnAcceptQ_Signal()
*
* Description : Signal socket that connection request received; socket now available in accept queue.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to signal connection accept queue.
*               -------     Argument checked in NetSock_ConnSignalAccept().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection accept queue successfully 
*                                                                       signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL_FAULT      Socket connection accept queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnSignalAccept().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnAcceptQ_Signal (NET_SOCK_ID   sock_id,
                                     NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnAcceptQ_SignalPtr[sock_id];
    os_err  = OSSemPost(psignal);                               /* Signal socket accept queue.                          */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_SEM_OVF:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_FAULT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetOS_Sock_ConnAcceptQ_Abort()
*
* Description : Abort wait on socket connection accept queue signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to abort wait on connection 
*               -------         accept queue signal.
*
*                           Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Wait on socket connection accept queue
*                                                                   signal successfully aborted.
*                               NET_SOCK_ERR_CONN_ABORT         Socket connection request accept queue
*                                                                   signal abort failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnAcceptQ_Abort (NET_SOCK_ID   sock_id,
                                    NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnAcceptQ_SignalPtr[sock_id];
   (void)OSSemPendAbort((OS_EVENT *) psignal,                   /* Abort wait on socket connection accept queue signal  */
                        (INT8U     ) OS_PEND_OPT_BROADCAST,     /* ... for ALL waiting tasks.                           */
                        (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
        case OS_ERR_PEND_ABORT:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_CONN_ABORT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                NetOS_Sock_ConnAcceptQ_TimeoutDflt()
*
* Description : Set socket connection accept queue signal configured-default timeout value.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to set connection accept queue
*               -------          signal configured-default timeout.
*
*                           Argument checked in NetSock_CfgTimeoutConnAcceptDflt(),
*                                               NetSock_Clr(),
*                                               NetOS_Sock_Init().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection accept queue signal configured-
*                                                                   default timeout successfully set.
*
*                                                               --- RETURNED BY NetOS_TimeoutCalc_OS_tick() : ---
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutConnAcceptDflt(),
*               NetSock_Clr(),
*               NetOS_Sock_Init().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutConnAcceptDflt()'.
*
* Note(s)     : (1) Despite inconsistency with NetSock_CfgTimeoutConnAccept???() functions, 
*                   NetOS_Sock_ConnAcceptQ_TimeoutDflt() includes 'Q_' for consistency with other 
*                   NetOS_Sock_ConnAcceptQ_???() functions.
*
*               (2) NetOS_Sock_ConnAcceptQ_TimeoutDflt() is called by network protocol suite function(s)
*                   & may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutConnAcceptDflt()  Note #2'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnAcceptQ_TimeoutDflt (NET_SOCK_ID   sock_id,
                                          NET_ERR      *perr)
{
    CPU_INT32U  timeout_ms;
                                                                /* Set socket connection accept queue timeout ...       */
                                                                /* ... to configured-default          timeout value.    */
    timeout_ms = NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_MS;

    NetOS_Sock_ConnAcceptQ_TimeoutSet(sock_id, timeout_ms, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return;
    }

   *perr = NET_SOCK_ERR_NONE;
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetOS_Sock_ConnAcceptQ_TimeoutSet()
*
* Description : Set socket connection accept queue signal timeout value.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set connection accept 
*               -------             queue signal timeout.
*
*                               Argument checked in NetSock_CfgTimeoutConnAcceptSet(),
*                                                   NetOS_Sock_ConnAcceptQ_TimeoutDflt().
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection accept queue signal timeout
*                                                                   successfully set.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutConnAcceptSet(),
*               NetOS_Sock_ConnAcceptQ_TimeoutDflt().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutConnAcceptSet()'.
*
* Note(s)     : (1) Despite inconsistency with NetSock_CfgTimeoutConnAccept???() functions, 
*                   NetOS_Sock_ConnAcceptQ_TimeoutSet() includes 'Q_' for consistency with other
*                   NetOS_Sock_ConnAcceptQ_???() functions.
*
*               (2) NetOS_Sock_ConnAcceptQ_TimeoutSet() is called by network protocol suite function(s)
*                   & may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutConnAcceptSet()  Note #2'.
*
*               (3) 'NetOS_Sock_ConnAcceptQ_TimeoutTbl_tick[]' variables MUST ALWAYS be accessed 
*                    exclusively in critical sections.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnAcceptQ_TimeoutSet (NET_SOCK_ID   sock_id,
                                         CPU_INT32U    timeout_ms,
                                         NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  timeout_os_tick;

                                                                /* Calculate timeout value (in OS ticks).               */
    timeout_os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, DEF_NO, perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
                                                                /* Set socket connection accept queue timeout value ... */
                                                                /* ... (in OS ticks).                                   */
    NetOS_Sock_ConnAcceptQ_TimeoutTbl_tick[sock_id] = timeout_os_tick;
    CPU_CRITICAL_EXIT();

   *perr = NET_SOCK_ERR_NONE;
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                               NetOS_Sock_ConnAcceptQ_TimeoutGet_ms()
*
* Description : Get socket connection accept queue signal timeout value.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get connection accept queue 
*               -------         signal timeout.
*
*                           Argument checked in NetSock_CfgTimeoutConnAcceptGet_ms().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection accept queue signal 
*                                                                   timeout successfully returned.
*
* Return(s)   : Socket connection accept network timeout value :
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : NetSock_CfgTimeoutConnAcceptGet_ms().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutConnAcceptGet_ms()'.
*
* Note(s)     : (1) Despite inconsistency with NetSock_CfgTimeoutConnAccept???() functions, 
*                   NetOS_Sock_ConnAcceptQ_TimeoutGet_ms() includes 'Q_' for consistency with other 
*                   NetOS_Sock_ConnAcceptQ_???() functions.
*
*               (2) NetOS_Sock_ConnAcceptQ_TimeoutGet_ms() is called by network protocol suite function(s)
*                   & may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutConnAcceptGet_ms()  Note #3'.
*
*               (3) 'NetOS_Sock_ConnAcceptQ_TimeoutTbl_tick[]' variables MUST ALWAYS be accessed 
*                    exclusively in critical sections.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
CPU_INT32U  NetOS_Sock_ConnAcceptQ_TimeoutGet_ms (NET_SOCK_ID   sock_id,
                                                  NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      timeout_os_tick;
    CPU_INT32U  timeout_ms;


    CPU_CRITICAL_ENTER();
                                                                /* Get socket connection accept queue timeout ...       */
                                                                /* ... value (in OS ticks).                             */
    timeout_os_tick = NetOS_Sock_ConnAcceptQ_TimeoutTbl_tick[sock_id];
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms      = NetOS_TimeoutCalc_ms(timeout_os_tick);    /* Calculate timeout value (in milliseconds).           */

   *perr            = NET_TCP_ERR_NONE;

    return (timeout_ms);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetOS_Sock_ConnCloseClr()
*
* Description : Clear socket connection close signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to clear connection close signal.
*               -------     Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection close signal successfully cleared.
*                               NET_SOCK_ERR_CONN_SIGNAL_CLR    Socket connection close signal NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnCloseClr (NET_SOCK_ID   sock_id,
                               NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnCloseSignalPtr[sock_id];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear socket connection close signal.                */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_SOCK_ERR_NONE;                      /* ... return NO error.                                 */
             } else {
                *perr = NET_SOCK_ERR_CONN_SIGNAL_CLR;
             }
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_CLR;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_Sock_ConnCloseWait()
*
* Description : Wait on socket connection close signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to wait on connection close signal.
*               -------     Argument checked in NetSock_CloseHandlerStream().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection close successfully
*                                                                       signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT    Socket connection close NOT signaled
*                                                                       by timeout.
*                               NET_SOCK_ERR_CONN_SIGNAL_ABORT      Socket connection close signal aborted;
*                                                                       socket closed/aborted.
*                               NET_SOCK_ERR_CONN_SIGNAL_FAULT      Socket connection close signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CloseHandlerStream().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) (a) If timeouts NOT desired, wait on socket connection close signal forever
*                       (i.e. do NOT exit).
*
*                   (b) If timeout      desired, return NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT error on socket 
*                       connection request timeout.  Implement timeout with OS-dependent functionality.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnCloseWait (NET_SOCK_ID   sock_id,
                                NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR     cpu_sr;
#endif
    OS_EVENT  *psignal;
    INT16U     timeout_os_tick;
    INT8U      os_err;


    psignal         = NetOS_Sock_ConnCloseSignalPtr[sock_id];
    CPU_CRITICAL_ENTER();
    timeout_os_tick = NetOS_Sock_ConnCloseTimeoutTbl_tick[sock_id];
    CPU_CRITICAL_EXIT();
    OSSemPend((OS_EVENT *) psignal,                             /* Wait on socket connection close signal ...           */
              (INT16U    ) timeout_os_tick,                     /* ... with configured timeout (see Note #1b).          */
              (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TIMEOUT:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT;           /* See Note #1b.                                        */
             break;


        case OS_ERR_PEND_ABORT:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_ABORT;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_FAULT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_Sock_ConnCloseSignal()
*
* Description : Signal socket that connection close complete; socket connection now closed.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to signal connection close complete.
*               -------     Argument checked in NetSock_ConnSignalClose().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection close successfully 
*                                                                       signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL_FAULT      Socket connection close signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnSignalClose().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnCloseSignal (NET_SOCK_ID   sock_id,
                                  NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnCloseSignalPtr[sock_id];
    os_err  = OSSemPost(psignal);                               /* Signal socket close.                                 */

    switch (os_err) {
        case OS_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_SEM_OVF:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_FAULT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_Sock_ConnCloseAbort()
*
* Description : Abort wait on socket connection close signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to abort wait on connection close 
*               -------         signal.
*
*                           Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Wait on socket connection close signal 
*                                                                   successfully aborted.
*                               NET_SOCK_ERR_CONN_ABORT         Socket connection request close abort
*                                                                   failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnCloseAbort (NET_SOCK_ID   sock_id,
                                 NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnCloseSignalPtr[sock_id];
   (void)OSSemPendAbort((OS_EVENT *) psignal,                   /* Abort wait on socket connection close signal ...     */
                        (INT8U     ) OS_PEND_OPT_BROADCAST,     /* ... for ALL waiting tasks.                           */
                        (INT8U    *)&os_err);                       

    switch (os_err) {
        case OS_ERR_NONE:
        case OS_ERR_PEND_ABORT:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        default:
            *perr = NET_SOCK_ERR_CONN_ABORT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetOS_Sock_ConnCloseTimeoutDflt()
*
* Description : Set socket connection close signal configured-default timeout value.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to set connection close signal
*               -------         configured-default timeout.
*
*                           Argument checked in NetSock_CfgTimeoutConnCloseDflt(),
*                                               NetSock_Clr(),
*                                               NetOS_Sock_Init().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetOS_Sock_ConnCloseTimeoutSet() : -
*                               NET_SOCK_ERR_NONE               Socket connection close signal configured-
*                                                                   default timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutConnCloseDflt(),
*               NetSock_Clr(),
*               NetOS_Sock_Init().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutConnCloseDflt()'.
*
* Note(s)     : (1) NetOS_Sock_ConnCloseTimeoutDflt() is called by network protocol suite function(s)
*                   & may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutConnCloseDflt()  Note #2'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnCloseTimeoutDflt (NET_SOCK_ID   sock_id,
                                       NET_ERR      *perr)
{
    CPU_INT32U  timeout_ms;
                                                                /* Set socket connection close timeout ...              */
                                                                /* ... to configured-default   timeout value.           */
    timeout_ms = NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_MS;

    NetOS_Sock_ConnCloseTimeoutSet(sock_id, timeout_ms, perr);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetOS_Sock_ConnCloseTimeoutSet()
*
* Description : Set socket connection close signal timeout value.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set connection close 
*               -------             signal timeout.
*
*                               Argument checked in NetSock_CfgTimeoutConnCloseSet(),
*                                                   NetOS_Sock_ConnCloseTimeoutDflt().
*
*               timeout_ms      Timeout value (in milliseconds).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection close signal timeout
*                                                                   successfully set.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CfgTimeoutConnCloseSet(),
*               NetOS_Sock_ConnCloseTimeoutDflt().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutConnCloseSet()'.
*
* Note(s)     : (1) NetOS_Sock_ConnCloseTimeoutSet() is called by network protocol suite function(s) & 
*                   may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutConnCloseSet()  Note #2'.
*
*               (2) 'NetOS_Sock_ConnCloseTimeoutTbl_tick[]' variables MUST ALWAYS be accessed exclusively 
*                    in critical sections.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnCloseTimeoutSet (NET_SOCK_ID   sock_id,
                                      CPU_INT32U    timeout_ms,
                                      NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  timeout_os_tick;

                                                                /* Calculate timeout value (in OS ticks).               */
    timeout_os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, DEF_NO, perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
                                                                /* Set socket connection close timeout value ...        */
                                                                /* ... (in OS ticks).                                   */
    NetOS_Sock_ConnCloseTimeoutTbl_tick[sock_id] = timeout_os_tick;
    CPU_CRITICAL_EXIT();

   *perr = NET_SOCK_ERR_NONE;
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetOS_Sock_ConnCloseTimeoutGet_ms()
*
* Description : Get socket connection close signal timeout value.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get connection close signal
*               -------         timeout.
*
*                           Argument checked in NetSock_CfgTimeoutConnCloseGet_ms().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection close signal timeout 
*                                                                   successfully returned.
*
* Return(s)   : Socket connection close network timeout value :
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : NetSock_CfgTimeoutConnCloseGet_ms().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).  See 'net_sock.c  NetSock_CfgTimeoutConnCloseGet_ms()'.
*
* Note(s)     : (1) NetOS_Sock_ConnCloseTimeoutGet_ms() is called by network protocol suite function(s)
*                   & may be called either with OR without the global network lock already acquired.
*
*                   See also 'net_sock.c  NetSock_CfgTimeoutConnCloseGet_ms()  Note #3'.
*
*               (2) 'NetOS_Sock_ConnCloseTimeoutTbl_tick[]' variables MUST ALWAYS be accessed exclusively 
*                    in critical sections.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
CPU_INT32U  NetOS_Sock_ConnCloseTimeoutGet_ms (NET_SOCK_ID   sock_id,
                                               NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      timeout_os_tick;
    CPU_INT32U  timeout_ms;


    CPU_CRITICAL_ENTER();
                                                                /* Get socket connection close timeout value ...        */
                                                                /* ... (in OS ticks).                                   */
    timeout_os_tick = NetOS_Sock_ConnCloseTimeoutTbl_tick[sock_id];
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms      = NetOS_TimeoutCalc_ms(timeout_os_tick);    /* Calculate timeout value (in milliseconds).           */

   *perr            = NET_TCP_ERR_NONE;

    return (timeout_ms);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_Sock_SelWait()
*
* Description : (1) Wait on multiple socket events for available resources &/or operations :
*
*                   (a) Calculate socket event  wait OS timeout values
*                   (b) Prepare   socket events wait
*                   (c) Wait on   socket events :
*                       (1) Demultiplex ready socket events
*                       (2) Re-signal consumed OS event(s), if event(s) NOT aborted
*
*
*               (2) Socket events checked for available socket resources &/or operations via their 
*                   respective OS events :
*
*                   (a) Requested socket events are mapped to their respective socket resources &/or 
*                       operations via a corresponding OS events arrays :
*
*                       (1) (A) A socket events table lists requested socket or connection ID numbers/
*                               handle identifiers & their respective socket event operations.
*
*                           (B) An OS events table maps socket events & their respective operations to 
*                               their corresponding OS events.
*
*                       (2) (A) There is a one-to-one correspondence of socket event table entries to 
*                               OS event table entries at identical indices.  In other words, an OS 
*                               event that is ready at some index readies the socket event at the same 
*                               index in the socket event table.
*
*                           (B) Although a socket event maps to exactly one resource &/or operation & 
*                               therefore exactly one OS event (see Note #2a2A); a socket resource &/or 
*                               operation & corresponding OS event MAY map to more than one socket event.  
*                               Therefore, an OS event that is ready may correspond to more than socket 
*                               event table entry.
*
*                       (3) (A) The tables are zero-based indexed; i.e. indices range from '0' to 'N - 1' 
*                               entries.
*
*                           (B) The tables are terminated with a NULL table entry values at index 'N'.
*
*                       (4) The size of the socket events table & OS events table is configured by 
*                           NET_SOCK_CFG_SEL_NBR_EVENTS_MAX (see Note #2b2).  Note that one additional 
*                           table entry is added for a terminating NULL table entry at maximum table 
*                           index 'NET_SOCK_CFG_SEL_NBR_EVENTS_MAX' (see Note #2a3B).
*
*                   (b) (1) 'sock_events_nbr_cfgd' indicates the number of socket events to configure 
*                            operations to wait on.
*
*                       (2) (A) NET_SOCK_CFG_SEL_NBR_EVENTS_MAX configures the maximum number of socket 
*                               events/operations, & therefore the maximum number of OS events, to wait 
*                               on.
*
*                           (B) This value is used to declare the size of the socket events & OS events 
*                               tables (see Note #2a3).
*
*                   See also 'net_sock.c  NetSock_Sel()  Note #4'.
*
*
*                                                    Socket Events Table                                         OS Events Table
*                                                     (see Note #2a1A)                                           (see Note #2a1B)
*
*                                       Socket ID       Connection ID          Type                              OS Event Pointers
*       -----          -----       ---------------------------------------------------------                  ----------------------
*         ^              ^         |      ID #0      |      ID #0      |      Type #0      | <--------------> |      Event #0      |
*         |              |         |-----------------|-----------------|-------------------|                  |--------------------|
*         |              |         |      ID #1      |      ID #1      |      Type #1      | <--------------> |      Event #1      |
*         |              |         |-----------------|-----------------|-------------------|                  |--------------------|
*         |                        |      ID #2      |      ID #2      |      Type #2      | <--------------> |      Event #2      |
*         |          Number of     |-----------------|-----------------|-------------------|        ^         |--------------------|
*         |          Socket/OS     |        .        |        .        |         .         |        |         |         .          |
*                    Events to     |        .        |        .        |         .         |        |         |         .          |
*      Maximum        wait on      |        .        |        .        |         .         |        |         |         .          |
*     Number of   (see Note #2b1)  |-----------------|-----------------|-------------------|                  |--------------------|
*     Socket/OS                    |   ID #(N - 3)   |   ID #(N - 3)   |   Type #(N - 3)   |    One-to-one    |   Event #(N - 3)   |
*     Events to          |         |-----------------|-----------------|-------------------|  correspondence  |--------------------|
*      wait on           |         |   ID #(N - 2)   |   ID #(N - 2)   |   Type #(N - 2)   |     of table     |   Event #(N - 2)   |
*  (see Note #2b2)       |         |-----------------|-----------------|-------------------|  entry indices   |--------------------|
*                        v         |   ID #(N - 1)   |   ID #(N - 1)   |   Type #(N - 1)   | (see Note #2a2A) |   Event #(N - 1)   |
*         |            -----       |-----------------|-----------------|-------------------|                  |--------------------|
*         |                        |        .        |        .        |         .         |                  |         .          |
*         |                        |        .        |        .        |         .         |                  |         .          |
*         |                        |        .        |        .        |         .         |                  |         .          |
*         |                        |-----------------|-----------------|-------------------|                  |--------------------|
*         v                        | ID #(N_max - 1) | ID #(N_max - 1) | Type #(N_max - 1) |                  | Event #(N_max - 1) |
*       -----                      |-----------------|-----------------|-------------------|                  |--------------------|
*                                  |       NONE      |       NONE      |       NONE        | <--------------> |        NULL        |
*                                  ---------------------------------------------------------         |        ----------------------
*                                                                                                    |
*                                                                                            Terminating NULL
*                                                                                              table entry
*                                                                                            (see Note #2a3B)
*
*
*$PAGE*
* Argument(s) : sock_events_nbr_cfgd    Number of socket events to wait on (see Note #2b1).
*               --------------------    Argument validated in NetSock_Sel().
*
*               psock_events_cfg        Pointer to socket events table to configure wait on available socket 
*               ----------------            resources &/or operations (see Note #2a1A).
*
*                                       Argument validated in NetSock_Sel().
*
*               psock_events_rtn        Pointer to socket events table to return available socket events.
*               ----------------        Argument validated in NetSock_Sel().
*
*               timeout_sec             Timeout value :
*
*                                           NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) 
*                                                                           value desired.
*
*                                           In number of      seconds, otherwise.
*
*               timeout_us              Timeout value :
*
*                                           NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) 
*                                                                           value desired.
*
*                                           In number of microseconds, otherwise.
*
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket events successfully checked for available
*                                                                   resources &/or operations; check return value 
*                                                                   for the number of ready socket events with 
*                                                                   available resources &/or operations.
*
*                               NET_SOCK_ERR_TIMEOUT            NO socket events ready or available by timeout.
*                               NET_SOCK_ERR_INVALID_EVENT      Invalid socket event.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
* Return(s)   : Number of socket events with available resources &/or operations, if any & NO errors.
*
*               0,                                                                otherwise.
*
* Caller(s)   : NetSock_Sel().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (3) (a) (1) If timeouts NOT desired, wait on multiple socket events forever 
*                           (i.e. do NOT exit).
*
*                       (2) If timeout      desired, return NET_SOCK_ERR_TIMEOUT error on socket events
*                           timeout.  Implement timeout with OS-dependent functionality.
*
*                   (b) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that :
*                                
*                       (1) "If the 'timeout' parameter ('ptimeout') is not ... null ... it specifies a 
*                            maximum interval to wait for the selection to complete."
*
*                           (A) "Implementations may place limitations on the maximum timeout interval 
*                                supported" :
*
*                               (1) "All implementations shall support a maximum timeout interval of at 
*                                    least 31 days."
*
*                                   (a) However, since maximum timeout interval values are dependent on 
*                                       the specific OS implementation; a maximum timeout interval can 
*                                       NOT be guaranteed.
*
*                               (2) "If the 'timeout' argument specifies a timeout interval greater than 
*                                    the implementation-defined maximum value, the maximum value shall be 
*                                    used as the actual timeout value."
*
*                           (B) "Implementations may also place limitations on the granularity of timeout 
*                                intervals" :
*
*                               (1) "If the requested 'timeout' interval requires a finer granularity 
*                                    than the implementation supports, the actual timeout interval shall 
*                                    be rounded up to the next supported value."
*
*                       (2) "If the 'timeout' parameter ('ptimeout') is ... null ... then the call to 
*                            ... select() shall block indefinitely until at least one descriptor meets 
*                            the specified criteria."
*
*                       See also 'net_sock.c  NetSock_Sel()  Note #3b3'.
*
*               (4) Since datagram-type sockets typically never wait on transmit operations, no socket 
*                   event need be configured to wait on datagram-type socket transmit operations or
*                   transmit errors.
*
*                   See also 'net_sock.c  NetSock_IsRdyTxDatagram()            Note #3'
*                          & 'net_sock.c  NetSock_SelDescHandlerErrDatagram()  Note #4'.
*********************************************************************************************************
*/
/*$PAGE*/
#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
NET_SOCK_EVENT_QTY  NetOS_Sock_SelWait (NET_SOCK_EVENT_QTY   sock_events_nbr_cfgd,
                                        NET_SOCK_EVENT_TBL  *psock_events_cfg,
                                        NET_SOCK_EVENT_TBL  *psock_events_rtn,
                                        CPU_INT32U           timeout_sec,
                                        CPU_INT32U           timeout_us,
                                        NET_ERR             *perr)
{
    CPU_INT32U             timeout_sec_tot;
    CPU_INT32U             timeout_ms_sec;
    CPU_INT32U             timeout_ms_us;
    CPU_INT32U             timeout_ms_tot;
    INT16U                 timeout_os_tick;
    NET_SOCK_EVENT_QTY     sock_events_nbr_rem;
    NET_SOCK_EVENT_QTY     sock_events_nbr_rdy;
    NET_SOCK_EVENT_TBL    *psock_event;
    NET_SOCK_EVENT_TBL    *psock_event_rtn;
    NET_SOCK_EVENT_TYPE    sock_event_type;
    CPU_BOOLEAN            sock_event_avail;
    NET_SOCK_ID            sock_id;
    NET_CONN_ID            conn_id;
    OS_EVENT              *psignal;
    OS_EVENT             **pos_events;
    OS_EVENT             **pos_events_rdy;
    OS_EVENT              *pos_event;
    OS_EVENT              *pos_event_rdy;
    OS_EVENT              *os_events[NET_SOCK_CFG_SEL_NBR_EVENTS_MAX + 1];      /* See Notes #2a1B & #2a4.              */
    OS_EVENT              *os_events_rdy[NET_SOCK_CFG_SEL_NBR_EVENTS_MAX + 1];
    void                  *os_event_msgs[NET_SOCK_CFG_SEL_NBR_EVENTS_MAX + 1];
    CPU_INT16U             os_events_ix;
    INT16U                 os_events_nbr_rdy;
    INT8U                  os_err;
    NET_ERR                net_err;


                                                                    /* ------------- CALCULATE OS TIMEOUT ------------- */
    if ((timeout_sec != NET_TMR_TIME_INFINITE) &&                   /* If timeout specified,                         .. */
        (timeout_us  != NET_TMR_TIME_INFINITE)) {
                                                                    /* .. calculate timeout (in milliseconds) :      .. */
                                                                    /* .. calculate timeout's total      second value;  */
        timeout_sec_tot = timeout_sec + (timeout_us / DEF_TIME_NBR_uS_PER_SEC);
        if (timeout_sec_tot >= timeout_sec) {                       /* .. if NO seconds-timeout integer overflow,    .. */
                                                                    /* .. calculate timeout's total millisecond value,  */
                                                                    /* .. rounded to next millisecond (see Note #3b1B1) */
            timeout_ms_sec =   timeout_sec_tot * DEF_TIME_NBR_mS_PER_SEC;
            timeout_ms_us  = ((timeout_us      % DEF_TIME_NBR_uS_PER_SEC) + ((DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC) - 1))
                                                                          /  (DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC);
            timeout_ms_tot =   timeout_ms_sec  + timeout_ms_us;

            if (timeout_ms_tot < timeout_sec_tot) {                 /* .. if milliseconds-timeout integer overflow,  .. */
                timeout_ms_tot = NET_OS_TIMEOUT_MAX_mS;             /* ..   limit to maximum timeout (see Note #3b1A).  */
            }

        } else {                                                    /* Else limit to maximum timeout (see Note #3b1A).  */
            timeout_ms_tot = NET_OS_TIMEOUT_MAX_mS;
        }

    } else {                                                        /* Else configure infinite timeout (see Note #3b2). */
        timeout_ms_tot = NET_TMR_TIME_INFINITE;
    }

                                                                    /* Calculate OS timeout value (in OS ticks).        */
    timeout_os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms_tot, DEF_YES, &net_err);
    if (net_err != NET_OS_ERR_NONE) {                               /* If OS timeout configuration err, ...             */
        timeout_os_tick = NET_OS_TIMEOUT_MAX_TICK;                  /* ... set to maximum OS timeout (see Note #3b1A2). */
    }


/*$PAGE*/
                                                                    /* --------- PREPARE SOCKET/OS EVENT WAIT --------- */
     sock_events_nbr_rem =   sock_events_nbr_cfgd;
    psock_event          =  psock_events_cfg;
    pos_events           = &os_events[0];
                                                                    /* Prepare ALL socket events for OS events wait.    */
    while ((psock_event->EventType != NET_SOCK_EVENT_NONE) &&
            (sock_events_nbr_rem > 0)) {

        sock_id = (NET_SOCK_ID)psock_event->SockID;
        conn_id = (NET_CONN_ID)psock_event->ConnID;

        psignal = (OS_EVENT  *)0;
        switch (psock_event->EventType) {                           /* Demultiplex socket event to appropriate OS event.*/
            case NET_SOCK_EVENT_SOCK_RX:
            case NET_SOCK_EVENT_SOCK_ERR_RX:
                 psignal = NetOS_Sock_RxQ_SignalPtr[sock_id];
                 break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
            case NET_SOCK_EVENT_SOCK_ACCEPT:
            case NET_SOCK_EVENT_SOCK_ERR_ACCEPT:
                 psignal = NetOS_Sock_ConnAcceptQ_SignalPtr[sock_id];
                 break;


            case NET_SOCK_EVENT_SOCK_CONN:
            case NET_SOCK_EVENT_SOCK_ERR_CONN:
                 psignal = NetOS_Sock_ConnReqSignalPtr[sock_id];
                 break;


            case NET_SOCK_EVENT_SOCK_CLOSE:
            case NET_SOCK_EVENT_SOCK_ERR_CLOSE:
                 psignal = NetOS_Sock_ConnCloseSignalPtr[sock_id];
                 break;
#endif


            case NET_SOCK_EVENT_TRANSPORT_RX:
            case NET_SOCK_EVENT_TRANSPORT_ERR_RX:
#if    (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
#ifdef  NET_TCP_MODULE_PRESENT
                 psignal = NetOS_TCP_RxQ_SignalPtr[conn_id];
#endif
#endif
                 break;


            case NET_SOCK_EVENT_TRANSPORT_TX:
            case NET_SOCK_EVENT_TRANSPORT_ERR_TX:
#if    (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
#ifdef  NET_TCP_MODULE_PRESENT
                 psignal = NetOS_TCP_TxQ_SignalPtr[conn_id];
#endif
#endif
                 break;


            case NET_SOCK_EVENT_NONE:
            case NET_SOCK_EVENT_SOCK_TX:                                /* See Note #4.                                 */
            case NET_SOCK_EVENT_SOCK_ERR_TX:
            default:
                (void)&conn_id;                                         /* Prevent possible compiler warning.           */
                *perr =  NET_SOCK_ERR_INVALID_EVENT;
                 return (0);                                            /* Prevent 'break NOT reachable' warning.       */
        }

        if (psignal != (OS_EVENT *)0) {                                 /* If OS event available, ...                   */
           *pos_events = psignal;                                       /* ... prepare for socket/OS event wait.        */

        } else {
           *perr =  NET_SOCK_ERR_INVALID_EVENT;
            return (0);
        }

        sock_events_nbr_rem--;
        psock_event++;
        pos_events++;
    }

                                                                        /* Append terminating NULL table value ...      */
   *pos_events = (OS_EVENT *)0;                                         /* ... (see Note #2a3B).                        */


/*$PAGE*/
                                                                        /* ---- WAIT ON MULTIPLE SOCKET/OS EVENTS ----- */
    os_events_nbr_rdy = OSEventPendMulti((OS_EVENT **)&os_events[0],
                                         (OS_EVENT **)&os_events_rdy[0],
                                         (void     **)&os_event_msgs[0],
                                         (INT16U     ) timeout_os_tick,
                                         (INT8U     *)&os_err);
    switch (os_err) {
        case OS_ERR_NONE:
        case OS_ERR_PEND_ABORT:
             if (os_events_nbr_rdy > 0) {                               /* If any OS event(s) ready,                 .. */
                 sock_events_nbr_rdy =  (NET_SOCK_EVENT_QTY)0;
                 sock_events_nbr_rem =  (NET_SOCK_EVENT_QTY)os_events_nbr_rdy;
                 psock_event_rtn     =   psock_events_rtn;
                 pos_events_rdy      =  &os_events_rdy[0];
                 pos_event_rdy       = *pos_events_rdy;
                 while ((pos_event_rdy != (OS_EVENT *)0) &&             /* .. search every ready  OS event           .. */
                        (sock_events_nbr_rem > 0)) {

                      os_events_ix =   0;
                     pos_events    =  &os_events[0];
                     pos_event     = *pos_events;
                                                                        /* .. from ALL configured OS events (see #2a2C) */
                     while ((pos_event != (OS_EVENT *)0) &&
                             (os_events_ix < sock_events_nbr_cfgd)) {

                         if (pos_event == pos_event_rdy) {              /* .. & if ready OS event found, demultiplex .. */
                                                                        /* .. to appropriate return socket event(s).    */
                             psock_event = &psock_events_cfg[os_events_ix];
                             switch (psock_event->EventType) {
                                 case NET_SOCK_EVENT_SOCK_RX:
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                                 case NET_SOCK_EVENT_SOCK_ACCEPT:
                                 case NET_SOCK_EVENT_SOCK_CONN:
                                 case NET_SOCK_EVENT_SOCK_CLOSE:
#endif
                                 case NET_SOCK_EVENT_TRANSPORT_RX:
                                 case NET_SOCK_EVENT_TRANSPORT_TX:
                                      sock_event_avail =  DEF_YES;
                                      sock_event_type  = (os_err == OS_ERR_NONE) ? psock_event->EventType
                                                                                 : NET_SOCK_EVENT_ERR;
                                      break;


                                 case NET_SOCK_EVENT_SOCK_ERR_RX:
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                                 case NET_SOCK_EVENT_SOCK_ERR_ACCEPT:
                                 case NET_SOCK_EVENT_SOCK_ERR_CONN:
                                 case NET_SOCK_EVENT_SOCK_ERR_CLOSE:
#endif
                                 case NET_SOCK_EVENT_TRANSPORT_ERR_RX:
                                 case NET_SOCK_EVENT_TRANSPORT_ERR_TX:
                                                                        /* If an OS event-ready error returned, ...     */
                                                                        /* ... return socket event as ready with error. */
                                      sock_event_avail = (os_err != OS_ERR_NONE) ? DEF_YES : DEF_NO;
                                      sock_event_type  =  psock_event->EventType;
                                      break;


                                 case NET_SOCK_EVENT_NONE:
                                 case NET_SOCK_EVENT_SOCK_TX:           /* See Note #4.                                 */
                                 case NET_SOCK_EVENT_SOCK_ERR_TX:
                                 default:
                                      sock_event_avail = DEF_NO;
                                      sock_event_type  = NET_SOCK_EVENT_NONE;
                                      break;
                             }

                             if (sock_event_avail == DEF_YES) {         /* If available, return socket event as ready.  */
                                 psock_event_rtn->EventType =  sock_event_type;
                                 psock_event_rtn->SockID    = psock_event->SockID;
                                 psock_event_rtn->ConnID    = psock_event->ConnID;
                                 psock_event_rtn++;
                                 sock_events_nbr_rdy++;
                             }

                             if (os_err == OS_ERR_NONE) {               /* If NO OS event-ready error, ...              */
                                 if (pos_event != (OS_EVENT *)0) {
                                    (void)OSSemPost(pos_event);         /* ... re-signal consumed OS event.             */
                                 }
                             }
                         }

                          os_events_ix++;
                         pos_events++;
                         pos_event = *pos_events;
                     }

                     pos_events_rdy++;
                     pos_event_rdy = *pos_events_rdy;
                     sock_events_nbr_rem--;
                 }
             }
/*$PAGE*/
                                                                        /* Append terminating NULL table values ...     */
             psock_event_rtn->EventType = NET_SOCK_EVENT_NONE;          /* ... (see Note #2a3B).                        */
             psock_event_rtn->SockID    = NET_SOCK_ID_NONE;
             psock_event_rtn->ConnID    = NET_CONN_ID_NONE;
             break;


        case OS_ERR_TIMEOUT:
            *perr =  NET_SOCK_ERR_TIMEOUT;
             return (0);                                                /* Prevent 'break NOT reachable' warning.       */


        case OS_ERR_PEVENT_NULL:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEND_ISR:
        case OS_ERR_PEND_LOCKED:
        default:
            *perr =  NET_SOCK_ERR_CONN_FAIL;
             return (0);                                                /* Prevent 'break NOT reachable' warning.       */
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (sock_events_nbr_rdy);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                              NETWORK/OS TIMEOUT & TIME DELAY FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     NetOS_TimeoutCalc_OS_tick()
*
* Description : Calculate OS timeout value.
*
* Argument(s) : timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               lim_max         Indicate whether to limit timeout to maximum OS timeout value (in OS ticks) :
*
*                                   DEF_NO                      Do NOT limit timeout to maximum OS timeout 
*                                                                         value; instead return error.
*
*                                   DEF_YES                            Limit timeout to maximum OS timeout 
*                                                                         value without error.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : OS timeout value (in OS ticks), if NO errors.
*
*               0,                              otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) (a) (1) uC/OS-II timeout value of '0' implements NO timeout -- i.e. timeouts wait 
*                           forever (see 'uC/OS-II  os_sem.c  OSSemPend()  timeout').
*
*                       (2) To ensure that a non-zero, finite timeout is implemented; MUST force at
*                           least one OS tick.
*
*                   (b) (1) (A) Compare 'timeout_ms'   to maximum timeout value, in milliseconds.
*
*                           (B) Compare 'timeout_tick' to maximum value of OS's timeout-tick data type.
*
*                       (2) If an exclusive maximum-value comparison -- i.e. comparison checks for
*                           greater-than-but-not-equal-to -- is used, 'timeout_tick' MUST be declared 
*                           as an integer data type with a greater resolution -- i.e. greater number 
*                           of bits -- than the OS's timeout-tick data type.
*********************************************************************************************************
*/
/*$PAGE*/
#ifdef  NET_OS_TIMEOUT_PRESENT
static  INT16U  NetOS_TimeoutCalc_OS_tick (CPU_INT32U    timeout_ms,
                                           CPU_BOOLEAN   lim_max,
                                           NET_ERR      *perr)
{
    CPU_INT32U  timeout_tick;                                   /* See Note #1b2.                                       */
    INT16U      timeout_os_tick;


    if (timeout_ms != NET_TMR_TIME_INFINITE) {                  /* If timeout specified, ...                            */
                                                                /* ... calculate timeout value (in OS ticks).           */
        if (timeout_ms   > NET_OS_TIMEOUT_MAX_mS) {             /* If timeout in milliseconds > maximum (see Note #1b1A)*/
            if (lim_max != DEF_YES) {                           /* ... & max timeout lim NOT req'd,                 ... */
               *perr         = NET_OS_ERR_INVALID_TIME;         /* ... return error.                                    */
                return (0);
            } else {                                            /* Else limit to maximum millisecond timeout.           */
                timeout_ms   = NET_OS_TIMEOUT_MAX_mS;
            }
        }

        timeout_tick = (timeout_ms * OS_TICKS_PER_SEC) / DEF_TIME_NBR_mS_PER_SEC;

        if (timeout_tick > NET_OS_TIMEOUT_MAX_TICK) {           /* If timeout in ticks > maximum (see Note #1b1B) ...   */
            if (lim_max != DEF_YES) {                           /* ... & max timeout lim NOT req'd,               ...   */
               *perr         = NET_OS_ERR_INVALID_TIME;         /* ... return error.                                    */
                return (0);
            } else {
                timeout_tick = NET_OS_TIMEOUT_MAX_TICK;         /* Else limit to maximum tick        timeout.           */
            }
        }

        if (timeout_tick > 0) {                                 /* If timeout value > 0, ...                            */
            timeout_os_tick = (INT16U)timeout_tick;             /* ... cast to OS tick size.                            */
        } else {
            timeout_os_tick = (INT16U)NET_OS_TIMEOUT_MIN_TICK;  /* Else set at least one OS tick (see Note #1a2).       */
        }

    } else {                                                    /* Else for infinite/NO timeout, ...                    */
        timeout_os_tick = (INT16U)0;                            /* ...  set 0 OS ticks (see Note #1a1).                 */
    }

   *perr =  NET_OS_ERR_NONE;

    return (timeout_os_tick);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_TimeoutCalc_ms()
*
* Description : Get timeout value.
*
* Argument(s) : timeout_os_tick     Timeout value (in OS ticks).
*
* Return(s)   : Timeout value :
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) (a) uC/OS-II timeout value of '0' implements NO timeout -- i.e. timeouts wait 
*                       forever (see 'uC/OS-II  os_sem.c  OSSemPend()  timeout').
*
*                   (b) For infinite uC/OS-II timeout value, return 'NET_TMR_TIME_INFINITE' value.
*********************************************************************************************************
*/

#ifdef  NET_OS_TIMEOUT_PRESENT
static  CPU_INT32U  NetOS_TimeoutCalc_ms (INT16U  timeout_os_tick)
{
    CPU_INT32U  timeout_ms;


    if (timeout_os_tick == 0) {                                 /* If NO OS ticks set, ...                              */
        timeout_ms =  (CPU_INT32U)NET_TMR_TIME_INFINITE;        /* ... return infinite/NO timeout value (see Note #1b). */
    } else {                                                    /* Else calculate timeout value (in milliseconds).      */
        timeout_ms = ((CPU_INT32U)timeout_os_tick * DEF_TIME_NBR_mS_PER_SEC) / OS_TICKS_PER_SEC;
    }

    return (timeout_ms);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetOS_TimeDly()
*
* Description : Delay for specified time.
*
* Argument(s) : time_dly_sec      Time delay value, in seconds.
*
*               time_dly_us       Time delay value, in microseconds.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Time delay successful.
*                               NET_OS_ERR_TIME_DLY_MAX         Time delay successful but limited to 
*                                                                   maximum OS time delay.
*                               NET_OS_ERR_TIME_DLY_FAULT       Time delay fault.
*                               NET_OS_ERR_INVALID_TIME         Invalid time delay value.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_OS_TIME_DLY_PRESENT
void  NetOS_TimeDly (CPU_INT32U   time_dly_sec,
                     CPU_INT32U   time_dly_us,
                     NET_ERR     *perr)
{
    CPU_INT32U  time_dly_sec_tot;
    CPU_INT32U  time_dly_ms;
    INT16U      time_ms;
    INT8U       time_sec;
    INT8U       time_min;
    INT8U       time_hr;
    INT8U       os_err;
    NET_ERR     net_err;

                                                                    /*    Calculate time delay's  total second value.   */
    time_dly_sec_tot = time_dly_sec + (time_dly_us / DEF_TIME_NBR_uS_PER_SEC);
    if ((time_dly_sec_tot >= time_dly_sec) &&                       /* If NO        time delay integer overflow       ..*/
        (time_dly_sec_tot <  NET_OS_TIME_DLY_MAX_SEC)) {            /* .. & NOT > maximum OS time delay second value; ..*/
                                                                    /* .. calculate time delay's   millisecond value, ..*/
                                                                    /* .. rounded up to next millisecond.               */
         time_dly_ms       = ((time_dly_us % DEF_TIME_NBR_uS_PER_SEC) + ((DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC) - 1))
                                                                      /  (DEF_TIME_NBR_uS_PER_SEC / DEF_TIME_NBR_mS_PER_SEC);
         net_err           = NET_OS_ERR_NONE;

    } else {                                                        /* Else limit to maximum OS time delay values.      */
         time_dly_sec_tot  = NET_OS_TIME_DLY_MAX_SEC;
         time_dly_ms       = NET_OS_TIME_DLY_MAX_mS;
         net_err           = NET_OS_ERR_TIME_DLY_MAX;
    }

                                                                    /* Calc OS time delay values.                       */
    time_hr  = (INT8U ) (time_dly_sec_tot                             / DEF_TIME_NBR_SEC_PER_HR );
    time_min = (INT8U )((time_dly_sec_tot % DEF_TIME_NBR_SEC_PER_HR ) / DEF_TIME_NBR_SEC_PER_MIN);
    time_sec = (INT8U ) (time_dly_sec_tot % DEF_TIME_NBR_SEC_PER_MIN);
    time_ms  = (INT16U) (time_dly_ms      % DEF_TIME_NBR_mS_PER_SEC );

    os_err   = (INT8U )OSTimeDlyHMSM((INT8U )time_hr,
                                     (INT8U )time_min,
                                     (INT8U )time_sec,
                                     (INT16U)time_ms);

    switch (os_err) {
        case OS_ERR_NONE:
        case OS_ERR_TIME_ZERO_DLY:
            *perr = net_err;
             break;


        case OS_ERR_TIME_INVALID_MINUTES:
        case OS_ERR_TIME_INVALID_SECONDS:
        case OS_ERR_TIME_INVALID_MS:
            *perr = NET_OS_ERR_INVALID_TIME;
             break;


        case OS_ERR_TIME_DLY_ISR:
        default:
            *perr = NET_OS_ERR_TIME_DLY_FAULT;
             break;
    }
}
#endif

