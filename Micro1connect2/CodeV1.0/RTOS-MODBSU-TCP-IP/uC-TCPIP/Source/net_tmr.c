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
* Filename      : net_tmr.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_TMR_MODULE
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


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
static  void  NetTmr_Clr           (NET_TMR  *ptmr);
#endif

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetTmr_Discard       (NET_TMR  *ptmr);

static  void  NetTmr_DiscardTaskTmr(void);
#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetTmr_Init()
*
* Description : (1) Initialize Network Timer Management Module :
*
*                   (a) Perform Timer Module/OS initialization
*                   (b) Initialize timer pool
*                   (c) Initialize timer table
*                   (d) Initialize timer task list pointer
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TMR_ERR_NONE                Network timer module successfully initialized.
*
*                                                               ------- RETURNED BY NetOS_Tmr_Init() : -------
*                               NET_OS_ERR_INIT_TMR_TASK        Network timer task      NOT successfully
*                                                                   initialized.
*                               NET_OS_ERR_INIT_TMR_TASK_NAME   Network timer task name NOT successfully
*                                                                   configured.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) The following network timer initialization MUST be sequenced as follows :
*
*                   (a) NetOS_Tmr_Init()   MUST precede ALL other network timer initialization functions
*                   (b) Network timer pool MUST be initialized PRIOR to initializing the pool with pointers 
*                            to timers
*********************************************************************************************************
*/

void  NetTmr_Init (NET_ERR  *perr)
{
    NET_TMR      *ptmr;
    NET_TMR_QTY   i;
    NET_ERR       err;


                                                                /* --------------- PERFORM TMR/OS INIT ---------------- */
    NetOS_Tmr_Init(perr);                                       /* Create Tmr Task       (see Note #2a).                */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }


                                                                /* ------------------ INIT TMR POOL ------------------- */
    NetTmr_PoolPtr = (NET_TMR *)0;                              /* Init-clr net tmr pool (see Note #2b).                */

    NetStat_PoolInit((NET_STAT_POOL   *)&NetTmr_PoolStat,
                     (NET_STAT_POOL_QTY) NET_TMR_CFG_NBR_TMR,
                     (NET_ERR         *)&err);


                                                                /* ------------------ INIT TMR TBL -------------------- */
    ptmr = &NetTmr_Tbl[0];
    for (i = 0; i < NET_TMR_CFG_NBR_TMR; i++) {
        ptmr->Type  =  NET_TMR_TYPE_TMR;                        /* Init each tmr type/id--NEVER modify.                 */
        ptmr->ID    = (NET_TMR_QTY)i;

        ptmr->Flags =  NET_TMR_FLAG_NONE;                       /* Init each tmr as NOT used.                           */

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
        NetTmr_Clr(ptmr);
#endif

        ptmr->NextPtr  = NetTmr_PoolPtr;                        /* Free each tmr to tmr pool (see Note #2).             */
        NetTmr_PoolPtr = ptmr;

        ptmr++;
    }


                                                                /* -------------- INIT TMR TASK LIST PTR -------------- */
    NetTmr_TaskListHead = (NET_TMR *)0;
    NetTmr_TaskListPtr  = (NET_TMR *)0;


   *perr = NET_TMR_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetTmr_TaskHandler()
*
* Description : (1) Handle network timers in the Timer Task List :
*
*                   (a) Acquire network lock                                            See Note #4
*
*                   (b) Handle every  network timer in Timer Task List :
*                       (1) Decrement network timer(s)
*                       (2) For any timer that expires :                                See Note #8
*                           (A) Execute timer's callback function
*                           (B) Free from Timer Task List
*
*                   (c) Release network lock
*
*
*               (2) (a) Timers are managed in a doubly-linked Timer List.
*
*                       (1) 'NetTmr_TaskListHead' points to the head of the Timer List.
*
*                       (2) Timers' 'PrevPtr' & 'NextPtr' doubly-link each timer to form the Timer List.
*
*                   (b) New timers are added at the head of the Timer List.
*
*                   (c) As timers are added into the list, older timers migrate to the tail of the Timer
*                       List.  Once a timer expires or is discarded, it is removed from the Timer List.
*
*
*                                        |                                               |
*                                        |<-------------- List of Timers --------------->|
*                                        |                (see Note #2a)                 |
*
*                                      New Timers
*                                   inserted at head                         Oldest Timer in List
*                                    (see Note #2b)                            (see Note #2c)
*
*                                           |                 NextPtr                 |
*                                           |             (see Note #2a2)             |
*                                           v                    |                    v
*                                                                |
*                        Head of         -------       -------   v   -------       -------
*                       Timer List  ---->|     |------>|     |------>|     |------>|     |
*                                        |     |<------|     |<------|     |<------|     |
*                    (see Note #2a1)     -------       -------   ^   -------       -------
*                                                                |
*                                                                |
*                                                             PrevPtr
*                                                         (see Note #2a2)
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_Tmr_Task() [see 'net_os.c'].
*
*               This function is a network protocol suite to operating system (OS) function & SHOULD be
*               called only by appropriate network-operating system port function(s).
*$PAGE*
* Note(s)     : (3) NetTmr_TaskHandler() blocked until network initialization completes.
*
*               (4) NetTmr_TaskHandler() blocks ALL other network protocol tasks by pending on & acquiring
*                   the global network lock (see 'net.h  Note #2').
*
*               (5) (a) NetTmr_TaskHandler() handles all valid timers in Timer Task List, up to the first
*                       corrupted timer in the Timer Task List, if any.
*
*                   (b) #### If ANY timer(s) in Timer Task List are corrupted :
*
*                       (1) Discard/unlink current Timer Task timer.
*                           (A) Consequently, any remaining valid timers in Timer Task List are :
*                               (1) Unlinked from Timer Task List, ...
*                               (2) NOT handled.
*
*                       (2) Timer Task is aborted.
*
*               (6) Since NetTmr_TaskHandler() is asynchronous to NetTmr_Free() [via execution of certain
*                   timer callback functions], the Timer Task List timer ('NetTmr_TaskListPtr') MUST be
*                   coordinated with NetTmr_Free() to avoid Timer Task List corruption :
*
*                   (a) (1) Timer Task List timer is typically advanced by NetTmr_TaskHandler() to the next 
*                           timer in the Timer Task List.
*
*                       (2) However, whenever the Timer Task List timer is freed by an asynchronous timer
*                           callback function, the Timer Task List timer MUST be advanced to the next
*                           valid & available timer in the Timer Task List.
*
*                           See also 'NetTmr_Free()  Note #3a'.
*
*                   (b) Timer Task List timer MUST be cleared after handling the Timer Task List.
*
*                       (1) However, Timer Task List timer is implicitly cleared after handling the
*                           Timer Task List.
*
*                       See also 'NetTmr_DiscardTaskTmr()  Note #1c'.
*
*               (7) Since NetTmr_TaskHandler() is asynchronous to ANY timer Get/Set, one additional tick
*                   is added to each timer's count-down so that the requested timeout is ALWAYS satisfied.
*                   This additional tick is added by NOT checking for zero ticks after decrementing; any
*                   timer that expires is recognized at the next tick.
*
*               (8) When a network timer expires, the timer SHOULD be freed PRIOR to executing the timer
*                   callback function.  This ensures that at least one timer is available if the timer
*                   callback function requires a timer.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetTmr_TaskHandler (void)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN    used;
#endif
    NET_TMR       *ptmr;
    void          *obj;
    CPU_FNCT_PTR   fnct;
    NET_ERR        err;


    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, ...                    */
        NetOS_InitWait(&err);                                           /* ... wait on net init (see Note #3).          */
        if (err != NET_OS_ERR_NONE) {
            return;
        }
    }

                                                                        /* ------------- ACQUIRE NET LOCK ------------- */
    NetOS_Lock(&err);                                                   /* See Note #4.                                 */
    if (err != NET_OS_ERR_NONE) {
        return;
    }

                                                                        /* ----------- HANDLE TMR TASK LIST ----------- */
    NetTmr_TaskListPtr = NetTmr_TaskListHead;                           /* Start @ Tmr Task List head.                  */
    while (NetTmr_TaskListPtr != (NET_TMR *)0) {                        /* Handle  Tmr Task List tmrs (see Note #5a).   */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                        /* -------------- VALIDATE TYPE --------------- */
        if (NetTmr_TaskListPtr->Type != NET_TMR_TYPE_TMR) {             /* If tmr type invalid;                     ... */
            NetTmr_DiscardTaskTmr();                                    /* ... discard tmr task tmr (see Note #5b1) ... */ 
            NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrInvalidTypeCtr);
            NetOS_Unlock();
            return;                                                     /* ... & abort tmr task     (see Note #5b2).    */
        }
                                                                        /* ------------ VALIDATE TMR USED ------------- */
        used = DEF_BIT_IS_SET(NetTmr_TaskListPtr->Flags, NET_TMR_FLAG_USED);
        if (used != DEF_YES) {                                          /* If tmr NOT used,                         ... */
            NetTmr_DiscardTaskTmr();                                    /* ... discard tmr task tmr (see Note #5b1) ... */ 
            NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrNotUsedCtr);
            NetOS_Unlock();
            return;                                                     /* ... & abort tmr task     (see Note #5b2).    */
        }
#endif

        ptmr               = NetTmr_TaskListPtr;
        NetTmr_TaskListPtr = NetTmr_TaskListPtr->NextPtr;               /* Set next tmr to update (see Note #6a1).      */

        if (ptmr->TmrVal > 0) {                                         /* If tmr val > 0, dec tmr val (see Note #7).   */
            ptmr->TmrVal--;

        } else {                                                        /* Else tmr expired;           ...              */

            obj  = ptmr->Obj;                                           /* Get obj for ...                              */
            fnct = ptmr->Fnct;                                          /* ... tmr callback fnct.                       */

            NetTmr_Free(ptmr);                                          /* ... free tmr (see Note #8); ...              */

            if (fnct != (CPU_FNCT_PTR)0) {                              /* ... & if avail,             ...              */
                fnct(obj);                                              /* ... exec tmr callback fnct.                  */
            }
        }
    }

#if 0                                                                   /* Clr tmr task ptr (see Note #6b1).            */
    NetTmr_TaskListPtr = (NET_TMR *)0;
#endif

                                                                        /* ------------- RELEASE NET LOCK ------------- */
    NetOS_Unlock();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetTmr_Get()
*
* Description : (1) Allocate & initialize a network timer :
*
*                   (a) Get        timer
*                   (b) Validate   timer
*                   (c) Initialize timer
*                   (d) Insert     timer at head of Timer Task List
*                   (e) Update timer pool statistics
*                   (f) Return pointer to timer
*                         OR
*                       Null pointer & error code, on failure
*
*               (2) The timer pool is implemented as a stack :
*
*                   (a) 'NetTmr_PoolPtr' points to the head of the timer pool.
*
*                   (b) Timers' 'NextPtr's link each timer to form   the timer pool stack.
*
*                   (c) Timers are inserted & removed at the head of the timer pool stack.
*
*
*                                        Timers are
*                                    inserted & removed
*                                        at the head
*                                      (see Note #2c)
*
*                                             |                NextPtr
*                                             |            (see Note #2b)
*                                             v                   |
*                                                                 |
*                                          -------       -------  v    -------       -------
*                         Timer Pool  ---->|     |------>|     |------>|     |------>|     |
*                          Pointer         |     |       |     |       |     |       |     |
*                                          |     |       |     |       |     |       |     |
*                       (see Note #2a)     -------       -------       -------       -------
*
*                                          |                                               |
*                                          |<------------ Pool of Free Timers ------------>|
*                                          |                 (see Note #2)                 |
*
*
* Argument(s) : obj         Pointer to object that requests a timer.
*
*               fnct        Pointer to callback function to execute when timer expires (see Note #3).
*
*               time        Initial timer value (in 'NET_TMR_TICK' ticks) [see Note #4].
*
*               flags       Flags to select timer options; bit-field flags logically OR'd :
*
*                               NET_TMR_FLAG_NONE               NO timer flags selected.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TMR_ERR_NONE                Network timer successfully allocated & initialized.
*                               NET_TMR_ERR_NULL_OBJ            Argument 'obj'  passed a NULL pointer.
*                               NET_TMR_ERR_NULL_FNCT           Argument 'fnct' passed a NULL pointer.
*                               NET_TMR_ERR_NONE_AVAIL          NO available timers to allocate.
*                               NET_TMR_ERR_INVALID_TYPE        Network timer is NOT a valid timer type.
*
* Return(s)   : Pointer to network timer, if NO errors.
*
*               Pointer to NULL,          otherwise.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*$PAGE*
* Note(s)     : (3) Ideally, network timer callback functions could be defined as '[(void) (OBJECT *)]' 
*                   type functions -- even though network timer API functions cast callback functions 
*                   to generic 'CPU_FNCT_PTR' type (i.e. '[(void) (void *)]').
*
*                   (a) (1) Unfortunately, ISO/IEC 9899:TC2, Section 6.3.2.3.(7) states that "a pointer 
*                           to an object ... may be converted to a pointer to a different object ... 
*                           [but] if the resulting pointer is not correctly aligned ... the behavior 
*                           is undefined".
*
*                           And since compilers may NOT correctly convert 'void' pointers to non-'void' 
*                           pointer arguments, network timer callback functions MUST avoid incorrect
*                           pointer conversion behavior between 'void' pointer parameters & non-'void' 
*                           pointer arguments & therefore CANNOT be defined as '[(void) (OBJECT *)]'.
*
*                       (2) However, Section 6.3.2.3.(1) states that "a pointer to void may be converted 
*                           to or from a pointer to any ... object ... A pointer to any ... object ... 
*                           may be converted to a pointer to void and back again; the result shall 
*                           compare equal to the original pointer".
*                   
*                   (b) Therefore, to correctly convert 'void' pointer objects back to appropriate
*                       network object pointer objects, network timer callback functions MUST :
*
*                       (1) Be defined as 'CPU_FNCT_PTR' type (i.e. '[(void) (void *)]'); & ...
*                       (2) Explicitly cast 'void' pointer arguments to specific object pointers.
*
*               (4) Timer value of 0 ticks/seconds allowed; next tick will expire timer.
*
*                   See also 'NetTmr_TaskHandler()  Note #7'.
*
*               (5) #### 'flags' currently unused (remove if unnecessary).
*********************************************************************************************************
*/
/*$PAGE*/
NET_TMR  *NetTmr_Get (void          *obj,
                      CPU_FNCT_PTR   fnct,
                      NET_TMR_TICK   time,
                      CPU_INT16U     flags,
                      NET_ERR       *perr)
{
    NET_TMR  *ptmr;
    NET_ERR   stat_err;


   (void)&flags;                                                /* Prevent compiler warning (see Note #5).              */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTRS ------------------ */
    if (obj == (void *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrNullObjCtr);
       *perr =   NET_TMR_ERR_NULL_OBJ;
        return ((NET_TMR *)0);
    }

    if (fnct == (CPU_FNCT_PTR)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrNullFnctCtr);
       *perr =   NET_TMR_ERR_NULL_FNCT;
        return ((NET_TMR *)0);
    }
#endif

                                                                /* --------------------- GET TMR ---------------------- */
    if (NetTmr_PoolPtr != (NET_TMR *)0) {                       /* If tmr pool NOT empty, get tmr from pool.            */
        ptmr            = (NET_TMR *)NetTmr_PoolPtr;
        NetTmr_PoolPtr  = (NET_TMR *)ptmr->NextPtr;

    } else {                                                    /* If none avail, rtn err.                              */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrNoneAvailCtr);
       *perr =   NET_TMR_ERR_NONE_AVAIL;
        return ((NET_TMR *)0);
    }


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE TMR ------------------- */
    if (ptmr->Type != NET_TMR_TYPE_TMR) {
        NetTmr_Discard(ptmr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrInvalidTypeCtr);
       *perr =   NET_TMR_ERR_INVALID_TYPE;
        return ((NET_TMR *)0);
    }
#endif

                                                                /* --------------------- INIT TMR --------------------- */
    ptmr->PrevPtr = (NET_TMR *)0;
    ptmr->NextPtr = (NET_TMR *)NetTmr_TaskListHead;
    ptmr->Obj     =  obj;
    ptmr->Fnct    =  fnct;
    ptmr->TmrVal  =  time;                                      /* Set tmr val (in ticks).                              */
    ptmr->Flags   =  NET_TMR_FLAG_NONE;
    DEF_BIT_SET(ptmr->Flags, NET_TMR_FLAG_USED);                /* Set tmr as used.                                     */

                                                                /* ---------- INSERT TMR INTO TMR TASK LIST ----------- */
    if (NetTmr_TaskListHead != (NET_TMR *)0) {                  /* If list NOT empty, insert before head.               */
        NetTmr_TaskListHead->PrevPtr = ptmr;
    }
    NetTmr_TaskListHead = ptmr;                                 /* Insert tmr @ list head.                              */

                                                                /* --------------- UPDATE TMR POOL STATS -------------- */
    NetStat_PoolEntryUsedInc(&NetTmr_PoolStat, &stat_err);

   *perr =  NET_TMR_ERR_NONE;

    return (ptmr);                                              /* --------------------- RTN TMR ---------------------- */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetTmr_Free()
*
* Description : (1) Free a network timer :
*
*                   (a) Remove timer from Timer Task List
*                   (b) Clear  timer controls
*                   (c) Free   timer back to timer pool
*                   (d) Update timer pool statistics
*
*
* Argument(s) : ptmr        Pointer to a network timer.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) #### To prevent freeing a timer already freed via previous timer free, NetTmr_Free()
*                   checks the timer's 'USED' flag BEFORE freeing the timer.
*
*                   This prevention is only best-effort since any invalid duplicate timer frees MAY be
*                   asynchronous to potentially valid timer gets.  Thus the invalid timer free(s) MAY
*                   corrupt the timer's valid operation(s).
*
*                   However, since the primary tasks of the network protocol suite are prevented from
*                   running concurrently (see 'net.h  Note #2'), it is NOT necessary to protect network
*                   timer resources from possible corruption since no asynchronous access from other
*                   network tasks is possible.
*
*               (3) Since NetTmr_TaskHandler() is asynchronous to NetTmr_Free() [via execution of certain
*                   timer callback functions], the Timer Task List timer ('NetTmr_TaskListPtr') MUST be
*                   coordinated with NetTmr_Free() to avoid Timer Task List corruption :
*
*                   (a) Whenever the Timer Task List timer is freed, the Timer Task List timer MUST be
*                       advanced to the next valid & available timer in the Timer Task List.
*
*                       See also 'NetTmr_TaskHandler()  Note #6a2'.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetTmr_Free (NET_TMR  *ptmr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN   used;
#endif
    NET_TMR      *pprev;
    NET_TMR      *pnext;
    NET_ERR       stat_err;


                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (ptmr == (NET_TMR *)0) {
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    if (ptmr->Type != NET_TMR_TYPE_TMR) {
        NetTmr_Discard(ptmr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrInvalidTypeCtr);
        return;
    }
                                                                /* ---------------- VALIDATE TMR USED ----------------- */
    used = DEF_BIT_IS_SET(ptmr->Flags, NET_TMR_FLAG_USED);
    if (used != DEF_YES) {                                      /* If tmr NOT used, ...                                 */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrNotUsedCtr);
        return;                                                 /* ... rtn but do NOT free (see Note #2).               */
    }
#endif

                                                                /* ----------- REMOVE TMR FROM TMR TASK LIST ---------- */
    if (ptmr == NetTmr_TaskListPtr) {                           /* If tmr is next Tmr Task tmr to update, ...           */
        pnext              = NetTmr_TaskListPtr->NextPtr;
        NetTmr_TaskListPtr = pnext;                             /* ... adv Tmr Task ptr to skip this tmr (see Note #3a).*/
    }

    pprev = ptmr->PrevPtr;
    pnext = ptmr->NextPtr;
    if (pprev != (NET_TMR *)0) {                                /* If ptmr is NOT   the head of Tmr Task List, ...      */
        pprev->NextPtr      = pnext;                            /* ... set pprev's NextPtr to skip ptmr.                */
    } else {                                                    /* Else set pnext as head of Tmr Task List.             */
        NetTmr_TaskListHead = pnext;
    }
    if (pnext != (NET_TMR *)0) {                                /* If ptmr is NOT @ the tail of Tmr Task List, ...      */
        pnext->PrevPtr      = pprev;                            /* ... set pnext's PrevPtr to skip ptmr.                */
    }

                                                                /* ---------------------- CLR TMR --------------------- */
    DEF_BIT_CLR(ptmr->Flags, NET_TMR_FLAG_USED);                /* Set tmr as NOT used.                                 */
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    NetTmr_Clr(ptmr);
#endif

                                                                /* --------------------- FREE TMR --------------------- */
    ptmr->NextPtr  = NetTmr_PoolPtr;
    NetTmr_PoolPtr = ptmr;

                                                                /* -------------- UPDATE TMR POOL STATS --------------- */
    NetStat_PoolEntryUsedDec(&NetTmr_PoolStat, &stat_err);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetTmr_Set()
*
* Description : Update a network timer with a new callback function & timer value.
*
* Argument(s) : ptmr        Pointer to a network timer.
*
*               fnct        Pointer to callback function to execute when timer expires (see Note #2).
*
*               time        Update timer value (in seconds expressed in 'NET_TMR_TICK' ticks)
*                               [see also Note #3].
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TMR_ERR_NONE                Network timer time successfully updated.
*                               NET_TMR_ERR_NULL_PTR            Argument 'ptmr' passed a NULL pointer.
*                               NET_TMR_ERR_NULL_FNCT           Argument 'fnct' passed a NULL pointer.
*                               NET_TMR_ERR_INVALID_TYPE        Invalid timer type.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Assumes network timer is ALREADY owned by a valid network object.
*
*               (2) Ideally, network timer callback functions could be defined as '[(void) (OBJECT *)]' 
*                   type functions -- even though network timer API functions cast callback functions 
*                   to generic 'CPU_FNCT_PTR' type (i.e. '[(void) (void *)]').
*
*                   (a) (1) Unfortunately, ISO/IEC 9899:TC2, Section 6.3.2.3.(7) states that "a pointer 
*                           to an object ... may be converted to a pointer to a different object ... 
*                           [but] if the resulting pointer is not correctly aligned ... the behavior 
*                           is undefined".
*
*                           And since compilers may NOT correctly convert 'void' pointers to non-'void' 
*                           pointer arguments, network timer callback functions MUST avoid incorrect
*                           pointer conversion behavior between 'void' pointer parameters & non-'void' 
*                           pointer arguments & therefore CANNOT be defined as '[(void) (OBJECT *)]'.
*
*                       (2) However, Section 6.3.2.3.(1) states that "a pointer to void may be converted 
*                           to or from a pointer to any ... object ... A pointer to any ... object ... 
*                           may be converted to a pointer to void and back again; the result shall 
*                           compare equal to the original pointer".
*                   
*                   (b) Therefore, to correctly convert 'void' pointer objects back to appropriate
*                       network object pointer objects, network timer callback functions MUST :
*
*                       (1) Be defined as 'CPU_FNCT_PTR' type (i.e. '[(void) (void *)]'); & ...
*                       (2) Explicitly cast 'void' pointer arguments to specific object pointers.
*
*                   See also 'NetTmr_Get()  Note #3'.
*
*               (3) Timer value of 0 ticks/seconds allowed; next tick will expire timer.
*
*                   See also 'NetTmr_TaskHandler()  Note #7'.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetTmr_Set (NET_TMR       *ptmr,
                  CPU_FNCT_PTR   fnct,
                  NET_TMR_TICK   time,
                  NET_ERR       *perr)
{

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTRS ------------------- */
    if (ptmr == (NET_TMR *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrNullPtrCtr);
       *perr = NET_TMR_ERR_NULL_PTR;
        return;
    }

    if (fnct == (CPU_FNCT_PTR)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrNullFnctCtr);
       *perr = NET_TMR_ERR_NULL_FNCT;
        return;
    }
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    if (ptmr->Type != NET_TMR_TYPE_TMR) {
        NetTmr_Discard(ptmr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetTmr_ErrInvalidTypeCtr);
       *perr = NET_TMR_ERR_INVALID_TYPE;
        return;
    }
#endif

    ptmr->Fnct   = fnct;
    ptmr->TmrVal = time;
   *perr         = NET_TMR_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetTmr_PoolStatGet()
*
* Description : Get network timer statistics pool.
*
* Argument(s) : none.
*
* Return(s)   : Network timer statistics pool, if NO errors.
*
*               NULL          statistics pool, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) NetTmr_PoolStatGet() blocked until network initialization completes; return NULL
*                   statistics pool.
*
*               (2) 'NetTmr_PoolStat' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

NET_STAT_POOL  NetTmr_PoolStatGet (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR         cpu_sr;
#endif
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    NET_ERR        err;
#endif
    NET_STAT_POOL  stat_pool;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, ...                            */
        NetStat_PoolClr(&stat_pool, &err);
        return (stat_pool);                                     /* ... rtn NULL stat pool (see Note #1).                */
    }
#endif


    CPU_CRITICAL_ENTER();
    stat_pool = NetTmr_PoolStat;
    CPU_CRITICAL_EXIT();

    return (stat_pool);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetTmr_PoolStatResetMaxUsed()
*
* Description : Reset network timer statistics pool's maximum number of entries used.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetTmr_PoolStatResetMaxUsed (void)
{
    NET_ERR  err;


    NetStat_PoolResetUsedMax(&NetTmr_PoolStat, &err);
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            NetTmr_Clr()
*
* Description : Clear network timer controls.
*
* Argument(s) : ptmr        Pointer to a network timer.
*               ----        Argument validated in NetTmr_Init(),
*                                                 NetTmr_Free().
*
* Return(s)   : none.
*
* Caller(s)   : NetTmr_Init(),
*               NetTmr_Free().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
static  void  NetTmr_Clr (NET_TMR  *ptmr)
{
    ptmr->PrevPtr = (NET_TMR *)0;
    ptmr->NextPtr = (NET_TMR *)0;
    ptmr->Obj     = (void    *)0;
    ptmr->Fnct    = (CPU_FNCT_PTR)0;
    ptmr->TmrVal  =  NET_TMR_TIME_0S;
    ptmr->Flags   =  NET_TMR_FLAG_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetTmr_Discard()
*
* Description : (1) Discard an invalid/corrupted network timer  :
*
*                   (a) Discard timer from available timer pool                     See Note #2
*                   (b) Update  timer pool statistics
*
*               (2) Assumes timer is invalid/corrupt & MUST be removed.  Timer removed simply by NOT
*                   returning the timer back to the timer pool.
*
*
* Argument(s) : ptmr        Pointer to an invalid/corrupted network timer.
*               ----        Argument checked in NetTmr_Get(),
*                                               NetTmr_Free(),
*                                               NetTmr_Set().
*
* Return(s)   : none.
*
* Caller(s)   : NetTmr_Get(),
*               NetTmr_Free(),
*               NetTmr_Set().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetTmr_Discard (NET_TMR  *ptmr)
{
    NET_ERR  stat_err;

                                                                /* ------------------- DISCARD TMR -------------------- */
   (void)&ptmr;                                                 /* Prevent compiler warning (see Note #2).              */

                                                                /* --------------- UPDATE DISCARD STATS --------------- */
    NetStat_PoolEntryLostInc(&NetTmr_PoolStat, &stat_err);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetTmr_DiscardTaskTmr()
*
* Description : (1) Discard a corrupted Timer Task List timer :
*
*                   (a) Unlink Timer Task timer from Timer Task List
*
*                       (1) Consequently, any valid or corrupted network timers that may have been linked 
*                           following the corrupted Timer Task timer in the Timer Task List are :
*
*                           (A) Unlinked from the Timer Task List; ...
*                           (B) Discarded/lost without :
*                               (1) possibility of recovery; ...
*                               (2) measure of the number of network timers lost.
*
*                           See also 'NetTmr_TaskHandler()  Note #5b1A'.
*
*                   (b) Discard Timer Task timer
*
*                       (1)        Discard the following invalid/corrupted timers :
*
*                           (A) Invalid Timer Type
*
*                       (2) Do NOT discard the following invalid           timers :
*
*                           (A) Unused timers
*
*                   (c) Clear   Timer Task timer pointer
*
*
*               (2) (a) Assumes Timer Task List timer ('NetTmr_TaskListPtr') is already corrupted.
*
*                   (b) Assumes ALL timers in Timer Task List prior to Timer Task List timer are valid.
*
*                   See also 'NetTmr_TaskHandler()  Note #5b'.
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetTmr_TaskHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetTmr_DiscardTaskTmr (void)
{
    NET_TMR      *ptmr;
    NET_TMR      *pprev;
    CPU_BOOLEAN   discard;


    ptmr  = (NET_TMR *)NetTmr_TaskListHead;
    pprev = (NET_TMR *)0;
    while ((ptmr != (NET_TMR *)0) &&                            /* Srch entire Tmr Task List ...                        */
           (ptmr != (NET_TMR *)NetTmr_TaskListPtr)) {           /* ... for corrupted tmr task ptr (see Note #2a).       */
        pprev = ptmr;
        ptmr  = ptmr->NextPtr;
    }
                                                                /* Unlink corrupted tmr task ptr from Tmr Task List :   */
    if (pprev != (NET_TMR *)0) {                                /* If NOT @ head of Tmr Task List, ...                  */
        pprev->NextPtr      = (NET_TMR *)0;                     /* ...  clr prev tmr's next tmr.                        */
    } else {                                                    /* Else clr Tmr Task List.                              */
        NetTmr_TaskListHead = (NET_TMR *)0;
    }

                                                                /* Discard invalid Tmr Task tmr (see Note #1b1).        */
    discard = (NetTmr_TaskListPtr->Type != NET_TMR_TYPE_TMR) ? DEF_YES : DEF_NO;
    if (discard == DEF_YES) {
        NetTmr_Discard(NetTmr_TaskListPtr);
    }
    NetTmr_TaskListPtr = (NET_TMR *)0;                          /* Clr Tmr Task ptr.                                    */
}
#endif

