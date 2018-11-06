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
*                                     NETWORK STATISTICS MANAGEMENT
*
* Filename      : net_stat.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_STAT_MODULE
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


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetStat_Init()
*
* Description : (1) Initialize Network Statistic Management Module :
*
*                   #### Module initialization NOT yet required/implemented
*
*
* Argument(s) : none.
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

void  NetStat_Init (void)
{
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetStat_PoolInit()
*
* Description : Initialize a statistics pool.
*
* Argument(s) : pstat_pool  Pointer to a statistics pool.
*
*               nbr_avail   Total number of available statistics pool entries.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_STAT_ERR_NONE               Statistics pool successfully initialized.
*                               NET_STAT_ERR_NULL_PTR           Argument 'pstat_pool' passed a NULL pointer.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Assumes pointer to statistics pool is valid (if non-NULL).
*********************************************************************************************************
*/

void  NetStat_PoolInit (NET_STAT_POOL      *pstat_pool,
                        NET_STAT_POOL_QTY   nbr_avail,
                        NET_ERR            *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pstat_pool == (NET_STAT_POOL *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrNullPtrCtr);
       *perr = NET_STAT_ERR_NULL_PTR;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();
    pstat_pool->Type                  = NET_STAT_TYPE_POOL;     /* Init stat pool type--NEVER modify.                   */
    pstat_pool->EntriesInit           = nbr_avail;              /* Init nbr of pool  entries is also ...                */
    pstat_pool->EntriesTot            = nbr_avail;              /* Tot  nbr of pool  entries is also ...                */
    pstat_pool->EntriesAvail          = nbr_avail;              /* Init nbr of avail entries.                           */
    pstat_pool->EntriesUsed           = 0;
    pstat_pool->EntriesUsedMax        = 0;
    pstat_pool->EntriesLostCur        = 0;
    pstat_pool->EntriesLostTot        = 0;
    pstat_pool->EntriesAllocatedCtr   = 0;
    pstat_pool->EntriesDeallocatedCtr = 0;
    CPU_CRITICAL_EXIT();


   *perr = NET_STAT_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetStat_PoolClr()
*
* Description : Clear a statistics pool.
*
* Argument(s) : pstat_pool  Pointer to a statistics pool.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_STAT_ERR_NONE               Statistics pool successfully cleared.
*                               NET_STAT_ERR_NULL_PTR           Argument 'pstat_pool' passed a NULL pointer.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Assumes pointer to statistics pool is valid (if non-NULL).
*********************************************************************************************************
*/

void  NetStat_PoolClr (NET_STAT_POOL  *pstat_pool,
                       NET_ERR        *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pstat_pool == (NET_STAT_POOL *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrNullPtrCtr);
       *perr = NET_STAT_ERR_NULL_PTR;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();
    pstat_pool->Type                  = NET_STAT_TYPE_NONE;
    pstat_pool->EntriesInit           = 0;
    pstat_pool->EntriesTot            = 0;
    pstat_pool->EntriesAvail          = 0;
    pstat_pool->EntriesUsed           = 0;
    pstat_pool->EntriesUsedMax        = 0;
    pstat_pool->EntriesLostCur        = 0;
    pstat_pool->EntriesLostTot        = 0;
    pstat_pool->EntriesAllocatedCtr   = 0;
    pstat_pool->EntriesDeallocatedCtr = 0;
    CPU_CRITICAL_EXIT();


   *perr = NET_STAT_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetStat_PoolReset()
*
* Description : Reset a statistics pool.
*
*               (1) Assumes object pool is also reset; otherwise, statistics pool will NOT accurately
*                   reflect the state of the object pool.
*
*
* Argument(s) : pstat_pool  Pointer to a statistics pool.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_STAT_ERR_NONE                   Statistics pool successfully reset.
*                               NET_STAT_ERR_NULL_PTR               Argument 'pstat_pool' passed a NULL pointer.
*                               NET_STAT_ERR_POOL_INVALID_TYPE      Invalid statistics pool type.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetStat_PoolReset (NET_STAT_POOL  *pstat_pool,
                         NET_ERR        *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (pstat_pool == (NET_STAT_POOL *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrNullPtrCtr);
       *perr = NET_STAT_ERR_NULL_PTR;
        return;
    }
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    if (pstat_pool->Type != NET_STAT_TYPE_POOL) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrInvalidTypeCtr);
       *perr = NET_STAT_ERR_POOL_INVALID_TYPE;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();
    pstat_pool->EntriesAvail          = pstat_pool->EntriesTot;
    pstat_pool->EntriesUsed           = 0;
    pstat_pool->EntriesUsedMax        = 0;
    pstat_pool->EntriesLostCur        = 0;
    pstat_pool->EntriesAllocatedCtr   = 0;
    pstat_pool->EntriesDeallocatedCtr = 0;
    CPU_CRITICAL_EXIT();


   *perr = NET_STAT_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetStat_PoolResetUsedMax()
*
* Description : Reset a statistics pool's maximum number of entries used.
*
*               (1) Resets maximum number of entries used to the current number of entries used.
*
*
* Argument(s) : pstat_pool  Pointer to a statistics pool.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_STAT_ERR_NONE                   Statistics pool's maximum number of entries
*                                                                       used successfully reset.
*                               NET_STAT_ERR_NULL_PTR               Argument 'pstat_pool' passed a NULL pointer.
*                               NET_STAT_ERR_POOL_INVALID_TYPE      Invalid statistics pool type.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetStat_PoolResetUsedMax (NET_STAT_POOL  *pstat_pool,
                                NET_ERR        *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (pstat_pool == (NET_STAT_POOL *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrNullPtrCtr);
       *perr = NET_STAT_ERR_NULL_PTR;
        return;
    }
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    if (pstat_pool->Type != NET_STAT_TYPE_POOL) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrInvalidTypeCtr);
       *perr = NET_STAT_ERR_POOL_INVALID_TYPE;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();
    pstat_pool->EntriesUsedMax = pstat_pool->EntriesUsed;       /* Reset nbr max used (see Note #1).                    */
    CPU_CRITICAL_EXIT();


   *perr = NET_STAT_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetStat_PoolEntryUsedInc()
*
* Description : Increment a statistics pool's number of 'Used' entries.
*
* Argument(s) : pstat_pool  Pointer to a statistics pool.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_STAT_ERR_NONE                   Statistics pool's number used 
*                                                                       successfully incremented.
*                               NET_STAT_ERR_NULL_PTR               Argument 'pstat_pool' passed a NULL pointer.
*                               NET_STAT_ERR_POOL_INVALID_TYPE      Invalid statistics pool type.
*                               NET_STAT_ERR_POOL_NONE_AVAIL        NO available statistics pool entries; i.e. 
*                                                                       number of available entries already zero.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetStat_PoolEntryUsedInc (NET_STAT_POOL  *pstat_pool,
                                NET_ERR        *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (pstat_pool == (NET_STAT_POOL *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrNullPtrCtr);
       *perr = NET_STAT_ERR_NULL_PTR;
        return;
    }
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    if (pstat_pool->Type != NET_STAT_TYPE_POOL) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrInvalidTypeCtr);
       *perr = NET_STAT_ERR_POOL_INVALID_TYPE;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();
    if (pstat_pool->EntriesAvail > 0) {                                 /* If any stat pool entry avail,            ..  */
        pstat_pool->EntriesAvail--;                                     /* .. adj nbr of avail/used entries in pool ..  */
        pstat_pool->EntriesUsed++;
        pstat_pool->EntriesAllocatedCtr++;                              /* .. & inc total nbr of allocated entries.     */
        if (pstat_pool->EntriesUsed    > pstat_pool->EntriesUsedMax) {  /* If nbr used > max used, set new max used.    */
            pstat_pool->EntriesUsedMax = pstat_pool->EntriesUsed;
        }

       *perr = NET_STAT_ERR_NONE;

    } else {
       *perr = NET_STAT_ERR_POOL_NONE_AVAIL;
    }
    CPU_CRITICAL_EXIT();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetStat_PoolEntryUsedDec()
*
* Description : Decrement a statistics pool's number of 'Used' entries.
*
* Argument(s) : pstat_pool  Pointer to a statistics pool.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_STAT_ERR_NONE                   Statistics pool's number used 
*                                                                       successfully decremented.
*                               NET_STAT_ERR_NULL_PTR               Argument 'pstat_pool' passed a NULL pointer.
*                               NET_STAT_ERR_POOL_INVALID_TYPE      Invalid statistics pool type.
*                               NET_STAT_ERR_POOL_NONE_USED         NO used statistics pool entries; i.e. 
*                                                                       number of used entries already zero.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetStat_PoolEntryUsedDec (NET_STAT_POOL  *pstat_pool,
                                NET_ERR        *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (pstat_pool == (NET_STAT_POOL *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrNullPtrCtr);
       *perr = NET_STAT_ERR_NULL_PTR;
        return;
    }
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    if (pstat_pool->Type != NET_STAT_TYPE_POOL) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrInvalidTypeCtr);
       *perr = NET_STAT_ERR_POOL_INVALID_TYPE;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();
    if (pstat_pool->EntriesUsed > 0) {                          /* If any stat pool entry used,              ...        */
        pstat_pool->EntriesAvail++;                             /* ... adj nbr of avail/used entries in pool ...        */
        pstat_pool->EntriesUsed--;
        pstat_pool->EntriesDeallocatedCtr++;                    /* ... & inc total nbr of deallocated entries.          */

       *perr = NET_STAT_ERR_NONE;

    } else {
       *perr = NET_STAT_ERR_POOL_NONE_USED;
    }
    CPU_CRITICAL_EXIT();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetStat_PoolEntryLostInc()
*
* Description : Increment a statistics pool's number of 'Lost' entries.
*
* Argument(s) : pstat_pool  Pointer to a statistics pool.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_STAT_ERR_NONE                   Statistics pool's number lost 
*                                                                       successfully incremented.
*                               NET_STAT_ERR_NULL_PTR               Argument 'pstat_pool' passed a NULL pointer.
*                               NET_STAT_ERR_POOL_INVALID_TYPE      Invalid statistics pool type.
*                               NET_STAT_ERR_POOL_NONE_REM          NO statistics pool entries remain; i.e. 
*                                                                       total number of entries already zero.
*                               NET_STAT_ERR_POOL_NONE_USED         NO used statistics pool entries; i.e. 
*                                                                       number of used entries is zero.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetStat_PoolEntryLostInc (NET_STAT_POOL  *pstat_pool,
                                NET_ERR        *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (pstat_pool == (NET_STAT_POOL *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrNullPtrCtr);
       *perr = NET_STAT_ERR_NULL_PTR;
        return;
    }
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    if (pstat_pool->Type != NET_STAT_TYPE_POOL) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetStat_ErrInvalidTypeCtr);
       *perr = NET_STAT_ERR_POOL_INVALID_TYPE;
        return;
    }
#endif


    CPU_CRITICAL_ENTER();
    if (pstat_pool->EntriesTot > 0) {                           /* If   tot stat pool entries > 0  ...                  */
        if (pstat_pool->EntriesUsed > 0) {                      /* ... & any stat pool entry used, ...                  */
            pstat_pool->EntriesUsed--;                          /* ... adj nbr used/total/lost entries in pool.         */
            pstat_pool->EntriesTot--;                         
            pstat_pool->EntriesLostCur++;
            pstat_pool->EntriesLostTot++;

           *perr = NET_STAT_ERR_NONE;
        } else {
           *perr = NET_STAT_ERR_POOL_NONE_USED;
        }

    } else {
       *perr = NET_STAT_ERR_POOL_NONE_REM;
    }
    CPU_CRITICAL_EXIT();
}

