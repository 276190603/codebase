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
*                                          NETWORK ARP LAYER
*                                    (ADDRESS RESOLUTION PROTOCOL)
*
* Filename      : net_arp.c
* Version       : V1.92
* Programmer(s) : ITJ
*                 SR
*********************************************************************************************************
* Note(s)       : (1) Address Resolution Protocol ONLY required for network interfaces that require
*                     network-address-to-hardware-address bindings (see RFC #826 'Abstract').
*
*                 (2) Supports Address Resolution Protocol as described in RFC #826 with the following
*                     restrictions/constraints :
*
*                     (a) ONLY supports the following hardware types :
*                         (1) 48-bit Ethernet
*
*                     (b) ONLY supports the following protocol types :
*                         (1) 32-bit IP
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_ARP_MODULE
#include  <net.h>


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) See 'net_arp.h  MODULE'.
*********************************************************************************************************
*/

#ifdef  NET_ARP_MODULE_PRESENT


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

                                                                            /* --------------- RX FNCTS --------------- */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void            NetARP_RxPktValidateBuf     (NET_BUF_HDR    *pbuf_hdr,
                                                     NET_ERR        *perr);
#endif

static  void            NetARP_RxPktValidate        (NET_BUF_HDR    *pbuf_hdr,
                                                     NET_ARP_HDR    *parp_hdr,
                                                     NET_ERR        *perr);


static  void            NetARP_RxPktCacheUpdate     (NET_ARP_HDR    *parp_hdr,
                                                     NET_ERR        *perr);


static  void            NetARP_RxPktReply           (NET_ARP_HDR    *parp_hdr,
                                                     NET_ERR        *perr);


static  void            NetARP_RxPktIsTargetThisHost(NET_ARP_HDR    *parp_hdr,
                                                     NET_ERR        *perr);


static  void            NetARP_RxPktFree            (NET_BUF        *pbuf);

static  void            NetARP_RxPktDiscard         (NET_BUF        *pbuf,
                                                     NET_ERR        *perr);
                                                   

                                                                            /* --------------- TX FNCTS --------------- */

static  void            NetARP_Tx                   (CPU_INT08U     *paddr_hw_sender,
                                                     CPU_INT08U     *paddr_hw_target,
                                                     CPU_INT08U     *paddr_protocol_sender,
                                                     CPU_INT08U     *paddr_protocol_target,
                                                     CPU_INT16U      op_code,
                                                     NET_ERR        *perr);


static  void            NetARP_TxReq                (NET_ARP_CACHE  *pcache);

static  void            NetARP_TxReply              (NET_ARP_HDR    *parp_hdr);


static  void            NetARP_TxPktPrepareHdr      (NET_BUF        *pbuf,
                                                     CPU_INT16U      msg_ix,
                                                     CPU_INT08U     *paddr_hw_sender,
                                                     CPU_INT08U     *paddr_hw_target,
                                                     CPU_INT08U     *paddr_protocol_sender,
                                                     CPU_INT08U     *paddr_protocol_target,
                                                     CPU_INT16U      op_code,
                                                     NET_ERR        *perr);


static  void            NetARP_TxPktFree            (NET_BUF        *pbuf);

static  void            NetARP_TxPktDiscard         (NET_BUF        *pbuf,
                                                     NET_ERR        *perr);


/*$PAGE*/
                                                                            /* ----------- ARP CACHE FNCTS ------------ */

static  void            NetARP_CacheTxPktHandler    (NET_BUF        *pbuf_q,
                                                     CPU_INT08U     *paddr_hw);


static  NET_ARP_CACHE  *NetARP_CacheSrch            (CPU_INT08U     *paddr_protocol);


static  void            NetARP_CacheAddPend         (NET_BUF        *pbuf,
                                                     NET_BUF_HDR    *pbuf_hdr,
                                                     CPU_INT08U     *paddr_protocol,
                                                     NET_ERR        *perr);

static  void            NetARP_CacheAddResolved     (CPU_INT08U     *paddr_hw,
                                                     CPU_INT08U     *paddr_protocol,
                                                     NET_ERR        *perr);


static  void            NetARP_CacheRemove          (NET_ARP_CACHE  *pcache,
                                                     CPU_BOOLEAN     tmr_free);

static  void            NetARP_CacheInsert          (NET_ARP_CACHE  *pcache);

static  void            NetARP_CacheUnlink          (NET_ARP_CACHE  *pcache);


static  void            NetARP_CacheUnlinkBuf       (NET_BUF        *pbuf);


static  void            NetARP_CacheReqTimeout      (void           *pcache_timeout);

static  void            NetARP_CacheTimeout         (void           *pcache_timeout);


static  NET_ARP_CACHE  *NetARP_CacheCfg             (CPU_INT08U     *paddr_hw,
                                                     CPU_INT08U     *paddr_protocol,
                                                     CPU_FNCT_PTR    timeout_fnct,
                                                     NET_TMR_TICK    timeout_tick,
                                                     NET_ERR        *perr);

static  NET_ARP_CACHE  *NetARP_CacheGet             (NET_ERR        *perr);

static  void            NetARP_CacheFree            (NET_ARP_CACHE  *pcache,
                                                     CPU_BOOLEAN     tmr_free);

static  void            NetARP_CacheClr             (NET_ARP_CACHE  *pcache);

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  CPU_BOOLEAN     NetARP_CacheIsUsed          (NET_ARP_CACHE  *pcache);

static  void            NetARP_CacheDiscard         (NET_ARP_CACHE  *pcache);
#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetARP_Init()
*
* Description : (1) Initialize Address Resolution Protocol Layer :
*
*                   (a) Initialize ARP cache pool
*                   (b) Initialize ARP cache table
*                   (c) Initialize ARP cache list pointers
*                   (d) Initialize ARP host addresses
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Init().
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface initialization function(s).
*
* Note(s)     : (2) ARP cache pool MUST be initialized PRIOR to initializing the pool with pointers to
*                   ARP caches.
*********************************************************************************************************
*/

void  NetARP_Init (void)
{
    NET_ARP_CACHE      *pcache;
    NET_ARP_CACHE_QTY   i;
    NET_ERR             stat_err;


                                                                /* ------------ INIT ARP CACHE POOL/STATS ------------- */
    NetARP_CachePoolPtr = (NET_ARP_CACHE *)0;                   /* Init-clr ARP cache pool (see Note #2).               */

    NetStat_PoolInit((NET_STAT_POOL   *)&NetARP_CachePoolStat,
                     (NET_STAT_POOL_QTY) NET_ARP_CFG_NBR_CACHE,
                     (NET_ERR         *)&stat_err);

#if (NET_CTR_CFG_STAT_EN    == DEF_ENABLED)
    NetARP_CacheSrchCtr_lo  =  0;
    NetARP_CacheSrchCtr_hi  =  0;
    NetARP_CacheFoundCtr_lo =  0;
    NetARP_CacheFoundCtr_hi =  0;
    NetARP_CacheFoundPct    =  0;
#endif

                                                                /* ---------------- INIT ARP CACHE TBL ---------------- */
    pcache = &NetARP_CacheTbl[0];
    for (i = 0; i < NET_ARP_CFG_NBR_CACHE; i++) {
                                                                /* Init each ARP cache type/addr len--NEVER modify.     */
        pcache->Type            = NET_ARP_TYPE_CACHE;
        pcache->HW_Type         = NET_ARP_CFG_HW_TYPE;
        pcache->HW_AddrLen      = NET_ARP_CFG_HW_ADDR_LEN;
        pcache->ProtocolType    = NET_ARP_CFG_PROTOCOL_TYPE;
        pcache->ProtocolAddrLen = NET_ARP_CFG_PROTOCOL_ADDR_LEN;

        pcache->State           = NET_ARP_CACHE_STATE_FREE;     /* Init each ARP cache as free/NOT used.                */
        pcache->Flags           = NET_ARP_FLAG_NONE;

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
        NetARP_CacheClr(pcache);
#endif
                                                                /* Free each ARP cache to cache pool (see Note #2).     */
        pcache->NextPtr     = NetARP_CachePoolPtr;
        NetARP_CachePoolPtr = pcache;

        pcache++;
    }

                                                                /* ------------- INIT ARP CACHE LIST PTRS ------------- */
    NetARP_CacheListHead = (NET_ARP_CACHE *)0;
    NetARP_CacheListTail = (NET_ARP_CACHE *)0;


                                                                /* --------------- INIT ARP HOST ADDRS ---------------- */
    NetARP_HostAddrPtrHW            = NetARP_GetHostAddrPtrHW();
    NetARP_HostAddrPtrProtocol      = NetARP_GetHostAddrPtrProtocol();

    NetARP_HostAddrProtocolConflict = DEF_NO;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetARP_CfgCacheTimeout()
*
* Description : Configure ARP cache timeout from ARP Cache List.
*
* Argument(s) : timeout_sec     Desired value for ARP cache timeout (in seconds).
*
* Return(s)   : DEF_OK,   ARP cache timeout configured.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) RFC #1122, Section 2.3.2.1 states that "an implementation of the Address Resolution
*                   Protocol (ARP) ... MUST provide a mechanism to flush out-of-date cache entries.  If
*                   this mechanism involves a timeout, it SHOULD be possible to configure the timeout
*                   value".
*
*               (2) Timeout in seconds converted to 'NET_TMR_TICK' ticks in order to pre-compute initial 
*                   timeout value in 'NET_TMR_TICK' ticks.
*
*               (3) 'NetARP_CacheTimeout' variables MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetARP_CfgCacheTimeout (CPU_INT16U  timeout_sec)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR        cpu_sr;
#endif
    NET_TMR_TICK  timeout_tick;


#if (NET_ARP_CACHE_TIMEOUT_MIN_SEC > DEF_INT_16U_MIN_VAL)
    if (timeout_sec < NET_ARP_CACHE_TIMEOUT_MIN_SEC) {
        return (DEF_FAIL);
    }
#endif
#if (NET_ARP_CACHE_TIMEOUT_MAX_SEC < DEF_INT_16U_MAX_VAL)
    if (timeout_sec > NET_ARP_CACHE_TIMEOUT_MAX_SEC) {
        return (DEF_FAIL);
    }
#endif

    timeout_tick             = (NET_TMR_TICK)timeout_sec * NET_TMR_TIME_TICK_PER_SEC;
    CPU_CRITICAL_ENTER();
    NetARP_CacheTimeout_sec  =  timeout_sec;
    NetARP_CacheTimeout_tick =  timeout_tick;
    CPU_CRITICAL_EXIT();

    return (DEF_OK);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetARP_CfgCacheAccessedTh()
*
* Description : Configure ARP cache access promotion threshold.
*
* Argument(s) : nbr_access      Desired number of ARP cache accesses before ARP cache is promoted.
*
* Return(s)   : DEF_OK,   ARP cache access promotion threshold configured.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) 'NetARP_CacheAccessedTh_nbr' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetARP_CfgCacheAccessedTh (CPU_INT16U  nbr_access)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_ARP_CACHE_ACCESSED_TH_MIN > DEF_INT_16U_MIN_VAL)
    if (nbr_access < NET_ARP_CACHE_ACCESSED_TH_MIN) {
        return (DEF_FAIL);
    }
#endif
#if (NET_ARP_CACHE_ACCESSED_TH_MAX < DEF_INT_16U_MAX_VAL)
    if (nbr_access > NET_ARP_CACHE_ACCESSED_TH_MAX) {
        return (DEF_FAIL);
    }
#endif

    CPU_CRITICAL_ENTER();
    NetARP_CacheAccessedTh_nbr = nbr_access;
    CPU_CRITICAL_EXIT();

    return (DEF_OK);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetARP_CfgReqTimeout()
*
* Description : Configure timeout between ARP Request retries.
*
* Argument(s) : timeout_sec     Desired value for ARP Request pending ARP Reply timeout (in seconds).
*
* Return(s)   : DEF_OK,   ARP Request timeout configured.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) Timeout in seconds converted to 'NET_TMR_TICK' ticks in order to pre-compute initial 
*                   timeout value in 'NET_TMR_TICK' ticks.
*
*               (2) 'NetARP_ReqTimeout' variables MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetARP_CfgReqTimeout (CPU_INT08U  timeout_sec)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR        cpu_sr;
#endif
    NET_TMR_TICK  timeout_tick;


    if (timeout_sec < NET_ARP_REQ_RETRY_TIMEOUT_MIN_SEC) {
        return (DEF_FAIL);
    }
    if (timeout_sec > NET_ARP_REQ_RETRY_TIMEOUT_MAX_SEC) {
        return (DEF_FAIL);
    }

    timeout_tick           = (NET_TMR_TICK)timeout_sec * NET_TMR_TIME_TICK_PER_SEC;
    CPU_CRITICAL_ENTER();
    NetARP_ReqTimeout_sec  =  timeout_sec;
    NetARP_ReqTimeout_tick =  timeout_tick;
    CPU_CRITICAL_EXIT();

    return (DEF_OK);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetARP_CfgReqMaxRetries()
*
* Description : Configure ARP Request maximum number of requests.
*
* Argument(s) : max_nbr_retries     Desired maximum number of ARP Request attempts.
*
* Return(s)   : DEF_OK,   ARP Request maximum number of request attempts configured.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) An ARP cache monitors the number of ARP Requests transmitted before receiving an ARP
*                   Reply.  In other words, an ARP cache monitors the number of ARP Request attempts.
*
*                   However, the maximum number of ARP Requests that each ARP cache is allowed to transmit
*                   is configured in terms of retries.  Thus, the total number of attempts is equal to the
*                   configured number of retries plus one (1).
*
*               (2) 'NetARP_ReqMaxAttempts_nbr' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetARP_CfgReqMaxRetries (CPU_INT08U  max_nbr_retries)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_ARP_REQ_RETRY_MIN > DEF_INT_08U_MIN_VAL)
    if (max_nbr_retries < NET_ARP_REQ_RETRY_MIN) {
        return (DEF_FAIL);
    }
#endif
#if (NET_ARP_REQ_RETRY_MAX < DEF_INT_08U_MAX_VAL)
    if (max_nbr_retries > NET_ARP_REQ_RETRY_MAX) {
        return (DEF_FAIL);
    }
#endif

    CPU_CRITICAL_ENTER();
    NetARP_ReqMaxAttempts_nbr = max_nbr_retries + 1;        /* Set max attempts as max retries + 1 (see Note #1).       */
    CPU_CRITICAL_EXIT();

    return (DEF_OK);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetARP_ProbeAddrOnNet()
*
* Description : (1) Transmit an ARP Request to probe the local network for a specific protocol address :
*
*                   (a) Acquire network lock
*                   (b) Remove ARP cache with desired protocol address from ARP Cache List, if available
*                   (c) Configure ARP cache :
*                       (1) Get default-configured ARP cache
*                       (2) ARP cache state
*                   (d) Transmit ARP Request to probe local network for desired protocol address
*                   (e) Release network lock
*
*
*               (2) NetARP_ProbeAddrOnNet() SHOULD be used in conjunction with NetARP_CacheGetAddrHW()
*                   to determine if a specific protocol address is available on the local network :
*
*                   (a) After successfully transmitting an ARP Request to probe the local network &  ...
*                   (b) After some time delay(s) [on the order of ARP Request timeouts & retries];   ...
*                   (c) Check ARP Cache for the hardware address of a host on the local network that
*                           corresponds to the desired protocol address.
*
*                   See also 'NetARP_CacheGetAddrHW()   Note #1'
*                          & 'NetARP_TxReqGratuitous()  Note #2'.
*
*
* Argument(s) : paddr_protocol      Pointer to protocol address to probe local network (see Note #4a).
*
*               addr_protocol_len   Length  of protocol address (in octets)            [see Note #4b].
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_NONE                    ARP Request successfully transmitted to
*                                                                       probe local network for the desired 
*                                                                       protocol address (see Note #2).
*                               NET_ARP_ERR_NULL_PTR                Argument 'paddr_protocol' passed a NULL
*                                                                      pointer.
*                               NET_ARP_ERR_INVALID_PROTOCOL_LEN    Invalid ARP protocol address length.
*
*                                                                   --- RETURNED BY NetARP_CacheCfg() : ----
*                               NET_ARP_ERR_CACHE_NONE_AVAIL        NO available ARP caches to allocate.
*                               NET_ARP_ERR_CACHE_INVALID_TYPE      ARP cache is NOT a valid cache type.
*                               NET_TMR_ERR_NULL_OBJ                Argument 'obj'  passed a NULL pointer.
*                               NET_TMR_ERR_NULL_FNCT               Argument 'fnct' passed a NULL pointer.
*                               NET_TMR_ERR_NONE_AVAIL              NO available timers to allocate.
*                               NET_TMR_ERR_INVALID_TYPE            Network timer is NOT a valid timer type.
*
*                                                                   ------ RETURNED BY NetOS_Lock() : ------
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s) [see also Note #3].
*
* Note(s)     : (3) NetARP_ProbeAddrOnNet() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock.
*
*                   This is required since an application's network protocol suite API function access is
*                   asynchronous to other network protocol tasks.
*
*               (4) (a) 'paddr_protocol' MUST point to a valid protocol address that is in network-order.
*
*                       See also 'NetARP_CacheHandler()  Note #2e3'.
*
*                   (b) The length of the protocol address MUST be equal to NET_ARP_CFG_PROTOCOL_ADDR_LEN
*                       & is included for correctness & completeness.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetARP_ProbeAddrOnNet (CPU_INT08U        *paddr_protocol,
                             NET_ARP_ADDR_LEN   addr_protocol_len,
                             NET_ERR           *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR          cpu_sr;
#endif
    NET_ARP_CACHE  *pcache;
    NET_TMR_TICK    timeout_tick;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* -------------- VALIDATE PROTOCOL ADDR -------------- */
    if (paddr_protocol == (CPU_INT08U *)0) {
       *perr = NET_ARP_ERR_NULL_PTR;
        return;
    }
    if (addr_protocol_len != NET_ARP_CFG_PROTOCOL_ADDR_LEN) {   /* Chk protocol addr len (see Note #4b).                */
       *perr = NET_ARP_ERR_INVALID_PROTOCOL_LEN;
        return;
    }
#else
   (void)&addr_protocol_len;                                    /* Prevent compiler warning.                            */
#endif


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }


                                                                /* --------- REMOVE PROTOCOL ADDR'S ARP CACHE --------- */
    pcache = NetARP_CacheSrch(paddr_protocol);
    if (pcache != (NET_ARP_CACHE *)0) {                         /* If protocol addr's ARP cache avail, ...              */
        NetARP_CacheRemove(pcache, DEF_YES);                    /* ... remove from ARP Cache List (see Note #1b).       */
    }


                                                                /* ------------------ CFG ARP CACHE ------------------- */
    CPU_CRITICAL_ENTER();
    timeout_tick = NetARP_ReqTimeout_tick;
    CPU_CRITICAL_EXIT();
    pcache       = NetARP_CacheCfg((CPU_INT08U *)0,
                                   (CPU_INT08U *)paddr_protocol,
                                   (CPU_FNCT_PTR)NetARP_CacheReqTimeout,
                                   (NET_TMR_TICK)timeout_tick,
                                   (NET_ERR    *)perr);
    if (*perr != NET_ARP_ERR_NONE) {
         NetOS_Unlock();
         return;
    }

    pcache->State = NET_ARP_CACHE_STATE_PEND;

                                                                /* ------- INSERT ARP CACHE INTO ARP CACHE LIST ------- */
    NetARP_CacheInsert(pcache);

                                                                /* -------------------- TX ARP REQ -------------------- */
    NetARP_TxReq(pcache);                                       /* Tx ARP req to probe local net for protocol addr.     */

                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


   *perr = NET_ARP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetARP_IsAddrProtocolConflict()
*
* Description : Check for any address conflict between this host's ARP protocol address & other host(s)
*                   on the local network.
*
* Argument(s) : none.
*
* Return(s)   : DEF_YES, if address conflict detected.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetARP_IsAddrProtocolConflict() is called by application function(s) & SHOULD NOT be 
*                   called with the global network lock already acquired.
*
*               (2) (a) RFC #3927, Section 2.5 states that :
*
*                       (1) "If a host receives an ARP packet (request *or* reply) on an interface where" ...
*                           (A) "the 'sender IP       address' is the IP address the host has configured for
*                                     that interface, but"                                                ...
*                           (B) "the 'sender hardware address' does not match the hardware address of 
*                                     that interface,"                                                    ...
*                       (2) "then this is a conflicting ARP packet, indicating an address conflict."
*
*                   (b) Any address conflict between this host's ARP protocol address & other host(s) on the
*                       local network is latched until checked & reset.
*
*                   See also 'NetARP_RxPktIsTargetThisHost()  Note #4'.
*
*               (3) 'NetARP_HostAddrProtocolConflict' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetARP_IsAddrProtocolConflict (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR       cpu_sr;
#endif
    CPU_BOOLEAN  addr_conflict;


    CPU_CRITICAL_ENTER();
    addr_conflict                   = NetARP_HostAddrProtocolConflict;
    NetARP_HostAddrProtocolConflict = DEF_NO;                       /* Reset addr conflict flag (see Note #2b).         */
    CPU_CRITICAL_EXIT();

    return (addr_conflict);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetARP_CacheHandler()
*
* Description : (1) Resolve destination hardware address for transmit data packet :
*
*                   (a) Search ARP Cache List for ARP cache with corresponding protocol address
*                   (b) If ARP cache     found, handle packet based on ARP cache state :
*
*                       (1) PENDING  -> Enqueue transmit packet buffer to ARP cache
*                       (2) RESOLVED -> Copy ARP cache's hardware address to data packet;
*                                       Return to Network Interface to transmit data packet
*
*                   (c) If ARP cache NOT found, allocate new ARP cache in 'PENDING' state (see Note #1b1)
*
*                   See 'net_arp.h  ARP CACHE STATES' for ARP cache state diagram.
*
*               (2) This ARP cache handler function assumes the following :
*
*                   (a) ALL ARP caches in the ARP Cache List are valid [validated by NetARP_CacheGet()]
*                   (b) ANY ARP cache in the 'PENDING' state MAY have already enqueued at LEAST one 
*                           transmit packet buffer when ARP cache allocated [see NetARP_CacheGet()]
*                   (c) ALL ARP caches in the 'RESOLVED' state have valid hardware addresses
*                   (d) ALL transmit buffers enqueued on any ARP cache are valid
*                   (e) Buffer's ARP address pointers pre-configured by Network Interface to point to :
*
*                       (1) 'ARP_AddrProtocolPtr'               Pointer to the protocol address used to 
*                                                                   resolve the hardware address
*                       (2) 'ARP_AddrHW_Ptr'                    Pointer to memory buffer to return the 
*                                                                   resolved hardware address
*                       (3) ARP addresses which MUST be in network-order
*
*
* Argument(s) : pbuf        Pointer to network buffer to transmit.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_CACHE_RESOLVED          ARP cache resolved & hardware address
*                                                                       successfully copied.
*                               NET_ARP_ERR_CACHE_PEND              ARP cache in 'PENDING' state; transmit
*                                                                       buffer enqueued to ARP cache.
*                               NET_ARP_ERR_NULL_PTR                Argument 'pbuf' passed a NULL pointer;
*                                                                       OR
*                                                                   'pbuf's 'ARP_AddrProtocolPtr'/'ARP_AddrHWPtr'
*                                                                       are set as NULL pointers.
*
*                                                                   ---- RETURNED BY NetARP_CacheAddPend() : ----
*                               NET_ARP_ERR_CACHE_NONE_AVAIL        NO available ARP caches to allocate.
*                               NET_ARP_ERR_CACHE_INVALID_TYPE      ARP cache is NOT a valid cache type.
*                               NET_TMR_ERR_NULL_OBJ                Argument 'obj'  passed a NULL pointer.
*                               NET_TMR_ERR_NULL_FNCT               Argument 'fnct' passed a NULL pointer.
*                               NET_TMR_ERR_NONE_AVAIL              NO available timers to allocate.
*                               NET_TMR_ERR_INVALID_TYPE            Network timer is NOT a valid timer type.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Tx().
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface function(s).
*
* Note(s)     : (3) (a) ARP Cache List is accessed by
*
*                       (1) NetARP_CacheHandler()
*                       (2) NetARP_CacheAddPend()                   via NetARP_CacheInsert()
*                       (3) NetARP_CacheAddResolved()               via NetARP_CacheInsert()
*                       (4) NetARP_CacheRemove()                    via NetARP_CacheUnlink()
*                       (5) NetARP_RxPktCacheUpdate()
*                       (6) ARP cache's 'TMR->Obj' pointer          via NetARP_CacheReqTimeout() &
*                                                                       NetARP_CacheTimeout()
*
*                   (b) Since the primary tasks of the network protocol suite are prevented from running 
*                       concurrently (see 'net.h  Note #2'), it is NOT necessary to protect the shared 
*                       resources of the ARP Cache List since no asynchronous access from other network
*                       tasks is possible.
*
*               (4) (a) RFC #1122, Section 2.3.2.2 states that "the link layer SHOULD" :
*
*                       (1) "Save (rather than discard) ... packets destined to the same unresolved
*                            IP address and" ...
*                       (2) "Transmit the saved packet[s] when the address has been resolved."
*
*                   (b) Since ARP Layer is the last layer to handle & queue the transmit network 
*                       buffer, it is NOT necessary to increment the network buffer's reference 
*                       counter to include the pending ARP cache buffer queue as a new reference
*                       to the network buffer.
*
*               (5) Some buffer controls were previously initialized in NetBuf_Get() when the packet
*                   was received at the network interface layer.  These buffer controls do NOT need
*                   to be re-initialized but are shown for completeness.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetARP_CacheHandler (NET_BUF  *pbuf,
                           NET_ERR  *perr)
{
    CPU_INT08U     *paddr_hw;
    CPU_INT08U     *paddr_protocol;
    NET_BUF_HDR    *pbuf_hdr;
    NET_BUF_HDR    *ptail_buf_hdr;
    NET_BUF        *ptail_buf;
    NET_ARP_CACHE  *pcache;

                                                                /* ------------------- VALIDATE PTRS ------------------ */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (pbuf == (NET_BUF *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
       *perr = NET_ARP_ERR_NULL_PTR;
        return;
    }
#endif

    pbuf_hdr       = &pbuf->Hdr;
    paddr_hw       =  pbuf_hdr->ARP_AddrHW_Ptr;
    paddr_protocol =  pbuf_hdr->ARP_AddrProtocolPtr;

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (paddr_hw == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
       *perr = NET_ARP_ERR_NULL_PTR;
        return;
    }
    if (paddr_protocol == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
       *perr = NET_ARP_ERR_NULL_PTR;
        return;
    }
#endif

                                                                /* ------------------ SRCH ARP CACHE ------------------ */
    pcache = NetARP_CacheSrch(paddr_protocol);
    if (pcache != (NET_ARP_CACHE *)0) {                         /* If ARP cache found, chk state.                       */
        switch (pcache->State) {
            case NET_ARP_CACHE_STATE_PEND:                      /* If ARP cache pend, append buf onto Q (see Note #4a1).*/
                 ptail_buf = pcache->BufQ_Tail;
                 if (ptail_buf != (NET_BUF *)0) {               /* If Q NOT empty,    append buf @ Q tail.              */
                     ptail_buf_hdr                 = &ptail_buf->Hdr;
                     ptail_buf_hdr->NextSecListPtr = (NET_BUF *)pbuf;
                     pbuf_hdr->PrevSecListPtr      = (NET_BUF *)ptail_buf;
                     pcache->BufQ_Tail             = (NET_BUF *)pbuf;

                 } else {                                       /* Else add buf as first q'd buf.                       */
                     pcache->BufQ_Head             = (NET_BUF *)pbuf;
                     pcache->BufQ_Tail             = (NET_BUF *)pbuf;
#if 0                                                           /* Init'd in NetBuf_Get() [see Note #5].                */
                     pbuf_hdr->PrevSecListPtr      = (NET_BUF *)0;
                     pbuf_hdr->NextSecListPtr      = (NET_BUF *)0;
#endif
                 }
                                                                /* Set ARP cache as unlink fnct/obj.                    */
                 pbuf_hdr->UnlinkFnctPtr = (CPU_FNCT_PTR)NetARP_CacheUnlinkBuf;
                 pbuf_hdr->UnlinkObjPtr  = (void       *)pcache;

                *perr                    =  NET_ARP_ERR_CACHE_PEND;
                 break;


            case NET_ARP_CACHE_STATE_RESOLVED:                  /* If ARP cache resolved, copy hw addr.                 */
                 Mem_Copy((void     *) paddr_hw,
                          (void     *)&pcache->HW_Addr[0],
                          (CPU_SIZE_T) NET_ARP_CFG_HW_ADDR_LEN);
                *perr = NET_ARP_ERR_CACHE_RESOLVED;
                 break;


            case NET_ARP_CACHE_STATE_NONE:
            case NET_ARP_CACHE_STATE_FREE:
            default:
                 NetARP_CacheRemove(pcache, DEF_YES);
                 NetARP_CacheAddPend(pbuf, pbuf_hdr, paddr_protocol, perr);
                 break;
        }

    } else {                                                    /* Else add new ARP cache into ARP Cache List.          */
        NetARP_CacheAddPend(pbuf, pbuf_hdr, paddr_protocol, perr);
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetARP_CacheGetAddrHW()
*
* Description : (1) Get hardware address that corresponds to a specific ARP cache's protocol address :
*
*                   (a) Search ARP Cache List for ARP cache with desired protocol address
*                   (b) If corresponding ARP cache found, get/return     hardware address
*
*
* Argument(s) : paddr_hw            Pointer to a memory buffer that will receive the hardware address :
*                                       (see Notes #2a & 3a) :
*
*                                       Hardware address that corresponds to the desired protocol address,
*                                           if NO errors.
*
*                                       Hardware address cleared to all zeros (see Note #2a3), otherwise.
*
*               addr_hw_len_buf     Length of hardware address memory buffer (in octets) [see Note #2a1].
*
*               paddr_protocol      Pointer to the specific protocol address to search for corresponding 
*                                       hardware address (see Note #3b).
*
*               addr_protocol_len   Length of protocol address (in octets) [see Note #2b].
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_NONE                    Hardware address successfully returned.
*                               NET_ARP_ERR_CACHE_NOT_FOUND         ARP cache with corresponding  hardware/protocol
*                                                                       address NOT found.
*                               NET_ARP_ERR_CACHE_PEND              ARP cache in 'PENDING' state; hardware address
*                                                                       NOT yet resolved (see Note #4).
*
*                               NET_ARP_ERR_NULL_PTR                Argument 'paddr_hw'/'paddr_protocol' passed
*                                                                       a NULL pointer.
*                               NET_ARP_ERR_INVALID_HW_LEN          Invalid ARP hardware address length.
*                               NET_ARP_ERR_INVALID_PROTOCOL_LEN    Invalid ARP protocol address length.
*
* Return(s)   : Length of returned hardware address (see Note #2a2), if available.
*
*               0,                                                   otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) (a) (1) The size of the memory buffer that will receive the returned hardware address 
*                           MUST be greater than or equal to NET_ARP_CFG_HW_ADDR_LEN.
*
*                       (2) The length of any returned hardware address is equal to NET_ARP_CFG_HW_ADDR_LEN.
*
*                       (3) Address memory buffer array cleared in case of any error(s).
*
*                           (A) Address memory buffer array SHOULD be initialized to return a NULL address 
*                               PRIOR to all other validation or function handling in case of any error(s).
*
*                   (b) The length of the protocol address MUST be equal to NET_ARP_CFG_PROTOCOL_ADDR_LEN
*                       & is included for correctness & completeness.
*
*               (3) ARP addresses MUST be in network-order :
*
*                   (a) 'paddr_hw'                       hardware address returned in network-order
*                   (b) 'paddr_protocol' MUST point to a protocol address that is  in network-order
*
*                   See also 'NetARP_CacheHandler()  Note #2e3'.
*
*               (4) While an ARP cache is in 'PENDING' state the hardware address is NOT yet resolved,
*                   but MAY be resolved in the near future by an awaited ARP Reply.
*********************************************************************************************************
*/
/*$PAGE*/
NET_ARP_ADDR_LEN  NetARP_CacheGetAddrHW (CPU_INT08U        *paddr_hw,
                                         NET_ARP_ADDR_LEN   addr_hw_len_buf,
                                         CPU_INT08U        *paddr_protocol,
                                         NET_ARP_ADDR_LEN   addr_protocol_len,
                                         NET_ERR           *perr)
{
    NET_ARP_ADDR_LEN   addr_hw_len;
    NET_ARP_CACHE     *pcache;


                                                                /* --------------- VALIDATE HW ADDR BUF --------------- */
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (paddr_hw == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
       *perr =  NET_ARP_ERR_NULL_PTR;
        return (0);
    }
#endif
                                                                /* Clr hw addr         (see Note #2a3).                 */
    Mem_Clr((void     *)paddr_hw,
            (CPU_SIZE_T) addr_hw_len_buf);

    addr_hw_len = NET_ARP_CFG_HW_ADDR_LEN;                      /* Cfg hw addr     len (see Note #2a2).                 */

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (addr_hw_len_buf < addr_hw_len) {                        /* Chk hw addr buf len (see Note #2a1).                 */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrInvalidAddrLenHW_Ctr);
       *perr =  NET_ARP_ERR_INVALID_HW_LEN;
        return (0);
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ------------ VALIDATE PROTOCOL ADDR BUF ------------ */
    if (paddr_protocol == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
       *perr =  NET_ARP_ERR_NULL_PTR;
        return (0);
    }
    if (addr_protocol_len != NET_ARP_CFG_PROTOCOL_ADDR_LEN) {   /* Chk protocol addr len (see Note #2b).                */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrInvalidAddrLenProtocolCtr);
       *perr =  NET_ARP_ERR_INVALID_PROTOCOL_LEN;
        return (0);
    }
#else
   (void)&addr_protocol_len;                                    /* Prevent compiler warning.                            */
#endif


                                                                /* ------------------ SRCH ARP CACHE ------------------ */
    pcache = NetARP_CacheSrch(paddr_protocol);
    if (pcache == (NET_ARP_CACHE *)0) {
       *perr =  NET_ARP_ERR_CACHE_NOT_FOUND;
        return (0);
    }

    switch (pcache->State) {
        case NET_ARP_CACHE_STATE_PEND:                          /* If ARP cache pend,     hw addr NOT yet avail ...     */
            *perr =  NET_ARP_ERR_CACHE_PEND;                    /* ... (see Note #4).                                   */
             return (0);                                        /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_ARP_CACHE_STATE_RESOLVED:                      /* If ARP cache resolved, hw addr         avail.        */
             break;


        case NET_ARP_CACHE_STATE_NONE:
        case NET_ARP_CACHE_STATE_FREE:
        default:
             NetARP_CacheRemove(pcache, DEF_YES);
            *perr =  NET_ARP_ERR_CACHE_NOT_FOUND;
             return (0);                                        /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ------------------- GET HW ADDR -------------------- */
    Mem_Copy((void     *) paddr_hw,
             (void     *)&pcache->HW_Addr[0],
             (CPU_SIZE_T) addr_hw_len);


   *perr =  NET_ARP_ERR_NONE;

    return (addr_hw_len);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetARP_CacheCalcStat()
*
* Description : (1) Calculate ARP statistics :
*
*                   (a) ARP cache found percentage
*
*
* Argument(s) : none.
*
* Return(s)   : ARP  cache found percentage, if NO errors,
*
*               NULL cache found percentage, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetARP_CacheCalcStat() blocked until network initialization completes; return NULL
*                   ARP cache found percentage.
*
*               (3) These ARP statistics calculation(s) are potentially expensive operations.  Recommended
*                   only from low-priority, background tasks.
*
*               (4) 'NetARP_Cache' counter variables MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

CPU_INT08U  NetARP_CacheCalcStat (void)
{
#if (NET_CTR_CFG_STAT_EN     == DEF_ENABLED)
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    NET_CTR     pct_numer_hi;
    NET_CTR     pct_numer_lo;
    NET_CTR     pct_denom_hi;
    NET_CTR     pct_denom_lo;
#endif
    CPU_INT08U  pct;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, ...                            */
        return (0);                                             /* ... rtn NULL pct (see Note #2).                      */
    }
#endif

                                                                /* ------------- CALC ARP CACHE FOUND PCT ------------- */
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
    CPU_CRITICAL_ENTER();
    pct_numer_hi = NetARP_CacheFoundCtr_hi;
    pct_numer_lo = NetARP_CacheFoundCtr_lo;
    pct_denom_hi = NetARP_CacheSrchCtr_hi;
    pct_denom_lo = NetARP_CacheSrchCtr_lo;
    CPU_CRITICAL_EXIT();

    pct = NetCtr_CalcPctLarge(pct_numer_hi,
                              pct_numer_lo,
                              pct_denom_hi,
                              pct_denom_lo);

    CPU_CRITICAL_ENTER();
    NetARP_CacheFoundPct = pct;
    CPU_CRITICAL_EXIT();

#else
    pct = 0;
#endif

    return (pct);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetARP_CachePoolStatGet()
*
* Description : Get ARP statistics pool.
*
* Argument(s) : none.
*
* Return(s)   : ARP  statistics pool, if NO errors.
*
*               NULL statistics pool, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) NetARP_CachePoolStatGet() blocked until network initialization completes; return NULL
*                   statistics pool.
*
*               (2) 'NetARP_CachePoolStat' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

NET_STAT_POOL  NetARP_CachePoolStatGet (void)
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
    stat_pool = NetARP_CachePoolStat;
    CPU_CRITICAL_EXIT();

    return (stat_pool);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetARP_CachePoolStatResetMaxUsed()
*
* Description : Reset ARP statistics pool's maximum number of entries used.
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

void  NetARP_CachePoolStatResetMaxUsed (void)
{
    NET_ERR  err;


    NetStat_PoolResetUsedMax(&NetARP_CachePoolStat, &err);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             NetARP_Rx()
*
* Description : (1) Process received ARP packets & update ARP Cache List :
*
*                   (a) Validate ARP packet
*                   (b) Update   ARP cache
*                   (c) Prepare & transmit an ARP Reply for a received ARP Request
*                   (d) Free ARP packet
*                   (e) Update receive statistics
*
*
* Argument(s) : pbuf        Pointer to network buffer that received ARP packet.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_NONE                ARP packet successfully received & processed.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               ---- RETURNED BY NetARP_RxPktDiscard() : ----
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxPktFrameDemux().
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface function(s).
*
* Note(s)     : (2) NetARP_Rx() blocked until network initialization completes.
*********************************************************************************************************
*/

void  NetARP_Rx (NET_BUF  *pbuf,
                 NET_ERR  *perr)
{
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
    NET_CTR      *pctr;
#endif
    NET_BUF_HDR  *pbuf_hdr;
    NET_ARP_HDR  *parp_hdr;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit rx (see Note #2).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NetARP_RxPktDiscard(pbuf, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
        return;
    }
#endif


    NET_CTR_STAT_INC(Net_StatCtrs.NetARP_StatRxPktCtr);


                                                                /* ----------------- VALIDATE ARP PKT ----------------- */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NetARP_RxPktValidateBuf(pbuf_hdr, perr);                    /* Validate rx'd buf.                                   */
    switch (*perr) {
        case NET_ARP_ERR_NONE:                                  
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        default:
             NetARP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif
    parp_hdr = (NET_ARP_HDR *)&pbuf->Data[pbuf_hdr->ARP_MsgIx];
    NetARP_RxPktValidate(pbuf_hdr, parp_hdr, perr);             /* Validate rx'd pkt.                                   */


/*$PAGE*/
                                                                /* ----------------- UPDATE ARP CACHE ----------------- */
    switch (*perr) {                                            /* Chk err from NetARP_RxPktValidate().                 */
        case NET_ARP_ERR_NONE:
             NetARP_RxPktCacheUpdate(parp_hdr, perr);
             break;


        case NET_ARP_ERR_INVALID_HW_TYPE:
        case NET_ARP_ERR_INVALID_HW_LEN:
        case NET_ARP_ERR_INVALID_HW_ADDR:
        case NET_ARP_ERR_INVALID_PROTOCOL_TYPE:
        case NET_ARP_ERR_INVALID_PROTOCOL_LEN:
        case NET_ARP_ERR_INVALID_PROTOCOL_ADDR:
        case NET_ARP_ERR_INVALID_OP_CODE:
        case NET_ARP_ERR_INVALID_OP_ADDR:
        case NET_ARP_ERR_INVALID_LEN_MSG:
        default:
             NetARP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ------------------- TX ARP REPLY ------------------- */
    switch (*perr) {                                            /* Chk err from NetARP_RxPktCacheUpdate().              */
        case NET_ARP_ERR_CACHE_RESOLVED:
             NetARP_RxPktReply(parp_hdr, perr);
             break;


        case NET_ARP_ERR_CACHE_NONE_AVAIL:
        case NET_ARP_ERR_CACHE_INVALID_TYPE:
        case NET_ARP_ERR_INVALID_OP_CODE:
        case NET_ARP_ERR_RX_TARGET_REPLY:
        case NET_ARP_ERR_RX_TARGET_NOT_THIS_HOST:
        case NET_TMR_ERR_NULL_OBJ:
        case NET_TMR_ERR_NULL_FNCT:
        case NET_TMR_ERR_NONE_AVAIL:
        case NET_TMR_ERR_INVALID_TYPE:
        default:
             NetARP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ---------- FREE ARP PKT / UPDATE RX STATS ---------- */
    switch (*perr) {                                            /* Chk err from NetARP_RxPktReply().                    */
        case NET_ARP_ERR_RX_REQ_TX_REPLY:
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
             pctr = &Net_StatCtrs.NetARP_StatRxMsgReqProcessedCtr;
#endif
             break;


        case NET_ARP_ERR_RX_REPLY_TX_PKTS:
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
             pctr = &Net_StatCtrs.NetARP_StatRxMsgReplyProcessedCtr;
#endif
             break;


        case NET_ARP_ERR_INVALID_OP_CODE:
        case NET_ARP_ERR_RX_TARGET_NOT_THIS_HOST:
        default:
             NetARP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

    NetARP_RxPktFree(pbuf);

    NET_CTR_STAT_INC(Net_StatCtrs.NetARP_StatRxMsgProcessedCtr);
    NET_CTR_STAT_INC(*pctr);


   *perr = NET_ARP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetARP_TxReqGratuitous()
*
* Description : (1) Prepare & transmit a gratuitous ARP Request onto the local network :
*
*                   (a) Acquire  network lock
*                   (b) Prepare  ARP Request packet :
*                       (1) Configure sender's hardware address as this host's hardware address
*                       (2) Configure target's hardware address as NULL
*                       (3) Configure sender's protocol address as this host's protocol address
*                       (4) Configure target's protocol address as this host's protocol address     See Note #5a
*                       (5) Configure ARP operation as ARP Request
*                   (c) Transmit ARP Request
*                   (d) Release  network lock
*
*
*               (2) NetARP_TxReqGratuitous() COULD be used in conjunction with NetARP_IsAddrProtocolConflict()
*                   to determine if the host's protocol address is already present on the local network :
*
*                   (a) After successfully transmitting a gratuitous ARP Request onto the local network & ...
*                   (b) After some time delay(s) [on the order of ARP Request timeouts & retries];        ...
*                   (c) Check this host's ARP protocol address conflict flag to see if any other host(s)
*                           are configured with this host's ARP protocol address.
*
*                   See also 'NetARP_IsAddrProtocolConflict()  Note #2'
*                          & 'NetARP_ProbeAddrOnNet()          Note #2'.
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               ------- RETURNED BY NetARP_Tx() : --------
*                               NET_ARP_ERR_NONE                Gratuitous ARP request packet successfully 
*                                                                   transmitted.
*                               NET_ARP_ERR_NULL_PTR            Argument(s) passed a NULL pointer.
*                               NET_ARP_ERR_INVALID_OP_CODE     Invalid ARP operation code.
*                               NET_BUF_ERR_NONE_AVAIL          NO available buffers to allocate.
*                               NET_BUF_ERR_INVALID_TYPE        Network buffer is NOT a valid buffer type.
*                               NET_BUF_ERR_INVALID_SIZE        Requested size is greater then the maximum
*                                                                   buffer size available.
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
*                                                               ------- RETURNED BY NetOS_Lock() : -------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s) [see also Note #3].
*
* Note(s)     : (3) NetARP_TxReqGratuitous() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock.
*
*                   This is required since an application's network protocol suite API function access is
*                   asynchronous to other network protocol tasks.
*
*               (4) NetARP_TxReqGratuitous() blocked until network initialization completes.
*
*               (5) RFC #3927, Section 2.4 states that one purpose for transmitting a gratuitous ARP Request
*                   is for a host to "announce its [claim] ... [on] a unique address ... by broadcasting ... 
*                   an ARP Request ... to make sure that other hosts on the link do not have stale ARP cache 
*                   entries left over from some other host that may previously have been using the same 
*                   address".
*                   
*                   (a) "The ... ARP Request ... announcement ... sender and target IP addresses are both
*                        set to the host's newly selected ... address."
*********************************************************************************************************
*/
/*$PAGE*/
void  NetARP_TxReqGratuitous (NET_ERR  *perr)
{
    CPU_INT08U  *paddr_hw_sender;
    CPU_INT08U  *paddr_hw_target;
    CPU_INT08U  *paddr_protocol_sender;
    CPU_INT08U  *paddr_protocol_target;
    CPU_INT16U   op_code;
    NET_ERR      err;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(&err);
    if (err != NET_OS_ERR_NONE) {
        return;
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit tx (see Note #4).         */
        NetOS_Unlock();
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif

                                                                /* ---------------- PREPARE ARP REQ PKT --------------- */
                                                                /* See Note #1b.                                        */
    paddr_hw_sender       = (CPU_INT08U *)NetARP_HostAddrPtrHW;
    paddr_hw_target       = (CPU_INT08U *)0;
    paddr_protocol_sender = (CPU_INT08U *)NetARP_HostAddrPtrProtocol;
    paddr_protocol_target = (CPU_INT08U *)NetARP_HostAddrPtrProtocol;

    op_code               =  NET_ARP_HDR_OP_REQ;

                                                                /* -------------------- TX ARP REQ -------------------- */
    NetARP_Tx(paddr_hw_sender,
              paddr_hw_target,
              paddr_protocol_sender,
              paddr_protocol_target,
              op_code,
              perr);

    if (*perr == NET_ARP_ERR_NONE) {
         NET_CTR_STAT_INC(Net_StatCtrs.NetARP_StatTxMsgReqCtr);
    }


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
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
*                                      NetARP_RxPktValidateBuf()
*
* Description : Validate received buffer header as ARP protocol.
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header that received ARP packet.
*               --------    Argument validated in NetARP_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_NONE                Received buffer's ARP header validated.
*                               NET_ERR_INVALID_PROTOCOL        Buffer's protocol type is NOT ARP.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetARP_RxPktValidateBuf (NET_BUF_HDR  *pbuf_hdr,
                                       NET_ERR      *perr)
{
                                                                /* --------------- VALIDATE ARP BUF HDR --------------- */
    if (pbuf_hdr->ProtocolHdrType != NET_PROTOCOL_TYPE_ARP) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxInvalidProtocolCtr);
       *perr = NET_ERR_INVALID_PROTOCOL;
        return;
    }

    if (pbuf_hdr->ARP_MsgIx == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

   *perr = NET_ARP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetARP_RxPktValidate()
*
* Description : (1) Validate received ARP packet :
*
*                   (a) Validate the received packet's following ARP header fields :
*
*                       (1) Hardware  Type
*                       (2) Hardware  Address Length
*                       (3) Hardware  Address : Sender's
*                       (4) Protocol  Type
*                       (5) Protocol  Address Length
*                       (6) Protocol  Address : Sender's
*                       (7) Operation Code
*
*                   (b) Convert the following ARP header fields from network-order to host-order :
*
*                       (1) Hardware  Type
*                       (2) Protocol  Type
*                       (3) Operation Code                                          See Note #1bB
*
*                           (A) These fields are NOT converted directly in the received packet buffer's 
*                               data area but are converted in local or network buffer variables ONLY.
*
*                           (B) To avoid storing the ARP operation code in a network buffer variable & 
*                               passing an additional pointer to the network buffer header that received 
*                               ARP packet, ARP operation code is converted in the following functions :
*
*                               (1) NetARP_RxPktValidate()
*                               (2) NetARP_RxPktCacheUpdate()
*                               (3) NetARP_RxPktReply()
*                               (4) NetARP_RxPktIsTargetThisHost()
*
*                           (C) Hardware & Protocol Addresses are NOT converted from network-order to 
*                               host-order & MUST be accessed as multi-octet arrays.
*
*                   (c) Validate the received packet's following ARP packet controls :
*
*                       (1) ARP message length                                      See Note #4
*
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header that received ARP packet.
*               --------    Argument validated in NetARP_Rx().
*
*               parp_hdr    Pointer to received packet's ARP header.
*               --------    Argument validated in NetARP_Rx()/NetARP_RxPktValidateBuf().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_NONE                        Received packet validated.
*                               NET_ARP_ERR_INVALID_HW_TYPE             Invalid ARP hardware type.
*                               NET_ARP_ERR_INVALID_HW_LEN              Invalid ARP hardware address length.
*                               NET_ARP_ERR_INVALID_HW_ADDR             Invalid ARP hardware address.
*                               NET_ARP_ERR_INVALID_PROTOCOL_TYPE       Invalid ARP protocol type.
*                               NET_ARP_ERR_INVALID_PROTOCOL_LEN        Invalid ARP protocol address length.
*                               NET_ARP_ERR_INVALID_PROTOCOL_ADDR       Invalid ARP protocol address.
*                               NET_ARP_ERR_INVALID_OP_CODE             Invalid ARP operation code.
*                               NET_ARP_ERR_INVALID_OP_ADDR             Invalid address for ARP operation
*                                                                           (see Note #3).
*                               NET_ARP_ERR_INVALID_LEN_MSG             Invalid ARP message length
*                                                                           (see Note #1c1).
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_Rx().
*$PAGE*
* Note(s)     : (2) See RFC #826, Section 'Packet Format' for ARP packet header format.
*
*               (3) (a) (1) RFC #826, Section 'Packet Generation' states that "the Address Resolution module
*                           ... does not set [the ARP Request packet's target hardware address] to anything 
*                           in particular, because it is this value that it is trying to determine" :
*
*                           (A) "Could    set [the ARP Request packet's target hardware address] to the 
*                                broadcast address for the hardware."
*
*                           (B) "Could    set [the ARP Request packet's target hardware address] to the ... 
*                                hardware  address of target of this packet (if known)."
*
*                       (2) (A) Therefore, ARP Requests SHOULD typically be transmitted onto a network via 
*                               the network's broadcast address.
*
*                           (B) However, an ARP Request COULD be transmitted directly to a specific host/
*                               hardware address.
*
*                           (C) Thus, any ARP Request NOT received as a broadcast OR directly-addressed 
*                               packet MUST be discarded.
*
*                   (b) (1) RFC #826, Section 'Packet Reception' states to "send the [ARP Reply] packet to 
*                           the ... address ... [from] which the request was received".
*
*                       (2) (A) Therefore, an ARP Reply SHOULD be transmitted directly to the ARP-Requesting 
*                               host & SHOULD NOT be broadcast onto the network.
*
*                           (B) Thus, any ARP Reply received as a broadcast packet SHOULD be discarded.
*
*               (4) (a) The ARP message length SHOULD be compared to the remaining packet data length which 
*                       should be identical.
*
*                   (b) (1) However, some network interfaces MAY append octets to their frames :
*
*                           (A) 'pad' octets, if the frame length does NOT meet the frame's required minimum size :
*
*                               (1) RFC #894, Section 'Frame Format' states that "the minimum length of  the data 
*                                   field of a packet sent over an Ethernet is 46 octets.  If necessary, the data 
*                                   field should be padded (with octets of zero) to meet the Ethernet minimum frame 
*                                   size".
*
*                               (2) RFC #1042, Section 'Frame Format and MAC Level Issues : For all hardware types' 
*                                   states that "IEEE 802 packets may have a minimum size restriction.  When  
*                                   necessary, the data field should be padded (with octets of zero) to meet the 
*                                   IEEE 802 minimum frame size requirements".
*
*                           (B) Trailer octets, to improve overall throughput :
*
*                               (1) RFC #893, Section 'Introduction' specifies "a link-level ... trailer 
*                                   encapsulation, or 'trailer' ... to minimize the number and size of memory-
*                                   to-memory copy operations performed by a receiving host when processing a 
*                                   data packet".
*
*                               (2) RFC #1122, Section 2.3.1 states that "trailer encapsulations[s] ... rearrange 
*                                   the data contents of packets ... [to] improve the throughput of higher layer 
*                                   protocols".
*
*                           (C) CRC or checksum values, optionally copied from a NIC.
*
*                       (2) Therefore, if ANY octets are appended to the total frame length, then the packet's 
*                           remaining data length MAY be greater than the ARP message length :
*
*                           (A) Thus,    the ARP message length & the packet's remaining data length CANNOT be 
*                               compared for equality.
*
*                               (1) Unfortunately, this eliminates the possibility to validate the ARP message 
*                                   length to the packet's remaining data length.
*
*                           (B) And      the ARP message length MAY    be less    than the packet's remaining 
*                               data length.
*
*                               (1) However, the packet's remaining data length MUST be reset to the ARP message
*                                   length to correctly calculate higher-layer application data length.
*
*                           (C) However, the ARP message length CANNOT be greater than the packet's remaining 
*                               data length.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetARP_RxPktValidate (NET_BUF_HDR  *pbuf_hdr,
                                    NET_ARP_HDR  *parp_hdr,
                                    NET_ERR      *perr)
{
#if (NET_ARP_CFG_ADDR_FLTR_EN == DEF_ENABLED)
    CPU_INT08U   *paddr_target_hw;
    CPU_BOOLEAN   target_hw_verifd;
#endif
    CPU_INT16U    hw_type;
    CPU_INT16U    protocol_type;
    CPU_INT16U    op_code;
    CPU_BOOLEAN   rx_broadcast;
    CPU_BOOLEAN   valid;


                                                                /* ------------ VALIDATE ARP HW TYPE/ADDR ------------- */
    NET_UTIL_VAL_COPY_GET_NET_16(&hw_type, &parp_hdr->HW_Type);
    if (hw_type != NET_ARP_CFG_HW_TYPE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrHW_TypeCtr);
       *perr = NET_ARP_ERR_INVALID_HW_TYPE;
        return;
    }

    if (parp_hdr->HW_AddrLen != NET_ARP_CFG_HW_ADDR_LEN) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrHW_AddrLenCtr);
       *perr = NET_ARP_ERR_INVALID_HW_LEN;
        return;
    }

    valid = NetARP_IsValidAddrHW(&parp_hdr->HW_AddrSender[0]);
    if (valid != DEF_YES) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrHW_AddrCtr);
       *perr = NET_ARP_ERR_INVALID_HW_ADDR;
        return;
    }


                                                                /* --------- VALIDATE ARP PROTOCOL TYPE/ADDR ---------- */
    NET_UTIL_VAL_COPY_GET_NET_16(&protocol_type, &parp_hdr->ProtocolType);
    if (protocol_type != NET_ARP_CFG_PROTOCOL_TYPE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrProtocolTypeCtr);
       *perr = NET_ARP_ERR_INVALID_PROTOCOL_TYPE;
        return;
    }

    if (parp_hdr->ProtocolAddrLen != NET_ARP_CFG_PROTOCOL_ADDR_LEN) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrProtocolAddrLenCtr);
       *perr = NET_ARP_ERR_INVALID_PROTOCOL_LEN;
        return;
    }

    valid = NetARP_IsValidAddrProtocol(&parp_hdr->ProtocolAddrSender[0]);
    if (valid != DEF_YES) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrProtocolAddrCtr);
       *perr = NET_ARP_ERR_INVALID_PROTOCOL_ADDR;
        return;
    }


/*$PAGE*/
                                                                /* --------------- VALIDATE ARP OP CODE --------------- */
    NET_UTIL_VAL_COPY_GET_NET_16(&op_code, &parp_hdr->OpCode);
    rx_broadcast = DEF_BIT_IS_SET(pbuf_hdr->Flags, NET_BUF_FLAG_RX_BROADCAST);
    switch (op_code) {
        case NET_ARP_HDR_OP_REQ:
#if (NET_ARP_CFG_ADDR_FLTR_EN == DEF_ENABLED)
             if (rx_broadcast != DEF_YES) {                     /* If rx'd ARP Req NOT broadcast (see Note #3a2A)  ...  */
                 paddr_target_hw  = parp_hdr->HW_AddrTarget;
                 target_hw_verifd = Mem_Cmp((void     *)paddr_target_hw,
                                            (void     *)NetARP_HostAddrPtrHW,
                                            (CPU_SIZE_T)NET_ARP_CFG_HW_ADDR_LEN);
                 if (target_hw_verifd != DEF_YES) {             /* ... & NOT addr'd to this host (see Note #3a2B), ...  */
                     NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrOpAddrCtr);
                    *perr = NET_ARP_ERR_INVALID_OP_ADDR;        /* ... rtn err / discard pkt     (see Note #3a2C).      */
                     return;
                 }
             }
#endif
             break;


        case NET_ARP_HDR_OP_REPLY:
             if (rx_broadcast != DEF_NO) {                      /* If rx'd ARP Reply broadcast, ...                     */
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrOpAddrCtr);
                *perr = NET_ARP_ERR_INVALID_OP_ADDR;            /* ... rtn err / discard pkt (see Note #3b2B).          */
                 return;
             }
             break;


        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrOpCodeCtr);
            *perr = NET_ARP_ERR_INVALID_OP_CODE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* --------------- VALIDATE ARP MSG LEN --------------- */
    pbuf_hdr->ARP_MsgLen = NET_ARP_HDR_SIZE;
    if (pbuf_hdr->ARP_MsgLen > pbuf_hdr->DataLen) {             /* If ARP msg len > rem pkt data len, ...               */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrMsgLenCtr);
       *perr = NET_ARP_ERR_INVALID_LEN_MSG;                     /* ... rtn err (see Note #4b2C).                        */
        return;
    }

    pbuf_hdr->DataLen = (NET_BUF_SIZE)pbuf_hdr->ARP_MsgLen;     /* Trunc pkt data len to ARP msg len (see Note #4b2B1). */



   *perr = NET_ARP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetARP_RxPktCacheUpdate()
*
* Description : (1) Update an ARP cache based on received ARP packet's sender addresses :
*
*                   (a) Verify ARP message's intended target address is this host
*                   (b) Search ARP Cache List
*                   (c) Update ARP cache
*
*
* Argument(s) : parp_hdr    Pointer to received packet's ARP header.
*               --------    Argument validated in NetARP_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_CACHE_RESOLVED              ARP cache resolved & hardware address
*                                                                           successfully copied.
*                               NET_ARP_ERR_RX_TARGET_REPLY             ARP Reply received but NO corresponding ARP
*                                                                           cache currently pending for ARP Reply.
*
*                                                                       --- RETURNED BY NetARP_CacheAddResolved() : ----
*                               NET_ARP_ERR_CACHE_NONE_AVAIL            NO available ARP caches to allocate.
*                               NET_ARP_ERR_CACHE_INVALID_TYPE          ARP cache is NOT a valid cache type.
*                               NET_TMR_ERR_NULL_OBJ                    Argument 'obj'  passed a NULL pointer.
*                               NET_TMR_ERR_NULL_FNCT                   Argument 'fnct' passed a NULL pointer.
*                               NET_TMR_ERR_NONE_AVAIL                  NO available timers to allocate.
*                               NET_TMR_ERR_INVALID_TYPE                Network timer is NOT a valid timer type.
*
*                                                                       - RETURNED BY NetARP_RxPktIsTargetThisHost() : -
*                               NET_ARP_ERR_RX_TARGET_NOT_THIS_HOST     Received ARP message NOT intended for this host.
*                               NET_ARP_ERR_INVALID_OP_CODE             Invalid ARP operation code.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_Rx().
*
* Note(s)     : (2) (a) RFC #826, Section 'Packet Reception' states that :
*
*                       (1) "If the ... sender protocol address ... is already in ... [the] translation table, 
*                            update the sender hardware address field of the entry with the new information in
*                            the packet."
*
*                       (2) Otherwise, if the packet's target protocol address matches this host's ARP protocol
*                           address; then "add the ... sender protocol address, sender hardware address to the
*	                        translation table".
*
*                   (b) (1) Thus, the ARP cache algorithm implies that ALL messages received at the ARP layer 
*                           automatically update the ARP Cache List EVEN if this host is NOT the intended target 
*                           host of a received ARP message -- but ONLY if an ARP cache for the sender's addresses
*                           is already cached.
*
*                       (2) However, if NO ARP cache exists for the sender's addresses, then even the ARP cache
*                           algorithm implies that a misdirected or incorrectly received ARP message is discarded.
*
*               (3) A configurable ARP address filtering feature is provided to selectively filter & discard
*                   ALL misdirected or incorrectly received ARP messages (see 'net_cfg.h  ADDRESS RESOLUTION
*                   PROTOCOL LAYER CONFIGURATION  Note #3') :
*
*                   (a) When ENABLED,  the ARP address filter discards :
*
*                       (1) ALL misdirected, broadcasted, or incorrectly received ARP messages.
*
*                       (2) Any ARP Reply received for this host for which NO corresponding ARP cache currently
*                           exists.  Note that such an ARP Reply may be a legitimate, yet late, ARP Reply to a
*                           pending ARP Request that has timed-out & been removed from the ARP Cache List.
*
*                   (b) When DISABLED, the ARP address filter discards :
*
*                       (1) Any misdirected or incorrectly received ARP messages if the sender's address(s) 
*                           are NOT already cached.
*
*               (4) (a) RFC # 826, Section 'Related issue' states that "perhaps receiving of a packet from a 
*                       host should reset a timeout in the address resolution entry ... [but] this may cause 
*                       extra overhead to scan the table for each incoming packet".
*
*                   (b) RFC #1122, Section 2.3.2.1 restates "that this timeout should be restarted when the 
*                       cache entry is 'refreshed' ... by ... an ARP broadcast from the system in question".
*
*                   (c) However, Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 4.5 'ARP Examples : 
*                       ARP Cache Timeout' adds that "the Host Requirements RFC [#1122] says that this timeout 
*                       should occur even if the entry is in use, but most Berkeley-derived implementations do 
*                       not do this -- they restart the timeout each time the entry is referenced".
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetARP_RxPktCacheUpdate (NET_ARP_HDR  *parp_hdr,
                                       NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR          cpu_sr;
#endif
#if (NET_ARP_CFG_ADDR_FLTR_EN == DEF_ENABLED)
    CPU_INT16U      op_code;
#else
    CPU_BOOLEAN     cache_add;
#endif
    CPU_INT08U     *paddr_sender_hw;
    CPU_INT08U     *paddr_sender_protocol;
    NET_ARP_CACHE  *pcache;
    NET_BUF        *pbuf_head;
    NET_TMR_TICK    timeout_tick;


                                                                /* ----------------- CHK TARGET ADDR ------------------ */
    NetARP_RxPktIsTargetThisHost(parp_hdr, perr);
#if (NET_ARP_CFG_ADDR_FLTR_EN == DEF_ENABLED)
    if (*perr != NET_ARP_ERR_RX_TARGET_THIS_HOST) {             /* Fltr misdirected rx'd ARP msgs (see Note #3b1).      */
         NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxPktTargetNotThisHostCtr);
         return;
    }
#else
    cache_add = (*perr == NET_ARP_ERR_RX_TARGET_THIS_HOST) ? DEF_YES : DEF_NO;
#endif


                                                                /* ------------------ SRCH ARP CACHE ------------------ */
    paddr_sender_hw       = parp_hdr->HW_AddrSender;
    paddr_sender_protocol = parp_hdr->ProtocolAddrSender;
    pcache                = NetARP_CacheSrch(paddr_sender_protocol);


/*$PAGE*/
                                                                /* ----------------- UPDATE ARP CACHE ----------------- */
    if (pcache != (NET_ARP_CACHE *)0) {                         /* If ARP cache found, chk state.                       */
        switch (pcache->State) {
            case NET_ARP_CACHE_STATE_PEND:                      /* If ARP cache pend, add sender's hw addr, ...         */
                 Mem_Copy((void     *)&pcache->HW_Addr[0],
                          (void     *) paddr_sender_hw,
                          (CPU_SIZE_T) NET_ARP_CFG_HW_ADDR_LEN);
                                                                /* Reset ARP cache tmr (see Note #4).                   */
                 CPU_CRITICAL_ENTER();
                 timeout_tick = NetARP_CacheTimeout_tick;
                 CPU_CRITICAL_EXIT();
                 NetTmr_Set((NET_TMR    *)pcache->TmrPtr,       
                            (CPU_FNCT_PTR)NetARP_CacheTimeout,
                            (NET_TMR_TICK)timeout_tick,
                            (NET_ERR    *)perr);

                 pbuf_head         =  pcache->BufQ_Head;
                 pcache->BufQ_Head = (NET_BUF *)0;
                 pcache->BufQ_Tail = (NET_BUF *)0;
                 NetARP_CacheTxPktHandler(pbuf_head,            /* ... & handle/tx cache's buf Q.                       */
                                          paddr_sender_hw);           
                 pcache->State     =  NET_ARP_CACHE_STATE_RESOLVED;
                *perr              =  NET_ARP_ERR_CACHE_RESOLVED;
                 break;


            case NET_ARP_CACHE_STATE_RESOLVED:                  /* If ARP cache resolved, update sender's hw addr.      */
                 Mem_Copy((void     *)&pcache->HW_Addr[0],
                          (void     *) paddr_sender_hw,
                          (CPU_SIZE_T) NET_ARP_CFG_HW_ADDR_LEN);
                                                                /* Reset ARP cache tmr (see Note #4).                   */
                 CPU_CRITICAL_ENTER();
                 timeout_tick = NetARP_CacheTimeout_tick;
                 CPU_CRITICAL_EXIT();
                 NetTmr_Set((NET_TMR    *)pcache->TmrPtr,       
                            (CPU_FNCT_PTR)NetARP_CacheTimeout,
                            (NET_TMR_TICK)timeout_tick,
                            (NET_ERR    *)perr);

                *perr = NET_ARP_ERR_CACHE_RESOLVED;
                 break;


            case NET_ARP_CACHE_STATE_NONE:
            case NET_ARP_CACHE_STATE_FREE:
            default:
                 NetARP_CacheRemove(pcache, DEF_YES);
                 NetARP_CacheAddResolved(paddr_sender_hw, paddr_sender_protocol, perr);
                 break;
        }

    } else {                                                    /* Else add new ARP cache into ARP Cache List.          */
#if (NET_ARP_CFG_ADDR_FLTR_EN == DEF_ENABLED)                   /* If ARP addr fltr en'd          ..                    */
        NET_UTIL_VAL_COPY_GET_NET_16(&op_code, &parp_hdr->OpCode);
        if (op_code != NET_ARP_HDR_OP_REQ) {                    /* .. but ARP pkt NOT an ARP Req, ..                    */
                                                                /* .. do  NOT add new ARP cache (see Note #3a2).        */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxPktTargetReplyCtr);
           *perr = NET_ARP_ERR_RX_TARGET_REPLY;
            return;
        }
#else                                                           /* If ARP addr fltr dis'd            ..                 */
        if (cache_add != DEF_YES) {                             /* .. &   ARP pkt NOT for this host, ..                 */
                                                                /* .. do  NOT add new ARP cache (see Note #3b1).        */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxPktTargetNotThisHostCtr);
            return;                                             /* Err rtn'd by NetARP_RxPktIsTargetThisHost().         */
        }
#endif
        NetARP_CacheAddResolved(paddr_sender_hw, paddr_sender_protocol, perr);
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetARP_RxPktReply()
*
* Description : Reply to received ARP message, if necessary.
*
* Argument(s) : parp_hdr    Pointer to received packet's ARP header.
*               --------    Argument validated in NetARP_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_RX_REQ_TX_REPLY             ARP Reply transmitted.
*                               NET_ARP_ERR_RX_REPLY_TX_PKTS            ARP Reply received; ARP cache transmit
*                                                                           queue packets already transmitted
*                                                                           (see Note #1).
*                               NET_ARP_ERR_INVALID_OP_CODE             Invalid ARP operation code.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_Rx().
*
* Note(s)     : (1) ARP Reply already transmitted the ARP cache's transmit buffer queue, if any, in
*                   NetARP_RxPktCacheUpdate(); no further action required.
*
*               (2) Default case already invalidated in NetARP_RxPktValidate().  However, the default
*                   case is included as an extra precaution in case 'OpCode' is incorrectly modified.
*********************************************************************************************************
*/

static  void  NetARP_RxPktReply (NET_ARP_HDR  *parp_hdr,
                                 NET_ERR      *perr)
{
    CPU_INT16U  op_code;


    NET_UTIL_VAL_COPY_GET_NET_16(&op_code, &parp_hdr->OpCode);

    switch (op_code) {
        case NET_ARP_HDR_OP_REQ:                                /* Use rx'd ARP Req to tx ARP Reply.                    */
             NetARP_TxReply(parp_hdr);
            *perr = NET_ARP_ERR_RX_REQ_TX_REPLY;
             break;


        case NET_ARP_HDR_OP_REPLY:                              /* See Note #1.                                         */ 
            *perr = NET_ARP_ERR_RX_REPLY_TX_PKTS;
             break;


        default:                                                /* See Note #2.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrOpCodeCtr);
            *perr = NET_ARP_ERR_INVALID_OP_CODE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetARP_RxPktIsTargetThisHost()
*
* Description : (1) Determine if this host is the intended target of the received ARP message :
*
*                   (a) Verify target hardware address                                  See Note #2
*                   (b) Verify target protocol address
*                   (c) Check         protocol address conflict                         See Note #4
*                   (d) Return target validation
*
*
* Argument(s) : parp_hdr    Pointer to received packet's ARP header.
*               --------    Argument validated in NetARP_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_RX_TARGET_THIS_HOST         Received ARP message     intended 
*                                                                           for this host.
*                               NET_ARP_ERR_RX_TARGET_NOT_THIS_HOST     Received ARP message NOT intended
*                                                                           for this host.
*                               NET_ARP_ERR_INVALID_OP_CODE             Invalid ARP operation code.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_RxPktCacheUpdate().
*
* Note(s)     : (2) (a) ARP Request target hardware address previously verified in NetARP_RxPktValidate()
*                       as     a network interface broadcast address or directly-addressed to this host's
*                       ARP hardware address.
*
*                       See 'NetARP_RxPktValidate()  Note #3a'.
*
*                   (b) ARP Reply   target hardware address previously verified in NetARP_RxPktValidate()
*                       as NOT a network interface broadcast address but NOT yet verified as directly-
*                       addressed to this host's ARP hardware address.
*
*                       See 'NetARP_RxPktValidate()  Note #3b'.
*
*               (3) Default case already invalidated in NetARP_RxPktValidate().  However, the default case 
*                   is included as an extra precaution in case 'OpCode' is incorrectly modified.
*
*               (4) (a) RFC #3927, Section 2.5 states that :
*
*                       (1) "If a host receives an ARP packet (request *or* reply) on an interface where" ...
*                           (A) "the 'sender IP       address' is the IP address the host has configured for
*                                     that interface, but"                                                ...
*                           (B) "the 'sender hardware address' does not match the hardware address of 
*                                     that interface,"                                                    ...
*                       (2) "then this is a conflicting ARP packet, indicating an address conflict."
*
*                   (b) Any address conflict between this host's ARP protocol address & other host(s) on the
*                       local network is latched until checked & reset.
*
*                   See also 'NetARP_IsAddrProtocolConflict()  Note #2'.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetARP_RxPktIsTargetThisHost (NET_ARP_HDR  *parp_hdr,
                                            NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR        cpu_sr;
#endif
#if (NET_ARP_CFG_ADDR_FLTR_EN == DEF_ENABLED)
    CPU_INT16U    op_code;
    CPU_INT08U   *paddr_target_hw;
#endif
    CPU_INT08U   *paddr_sender_hw;
    CPU_INT08U   *paddr_target_protocol;
    CPU_INT08U   *paddr_sender_protocol;
    CPU_BOOLEAN   target_hw_verifd;
    CPU_BOOLEAN   sender_hw_verifd;
    CPU_BOOLEAN   target_protocol_verifd;
    CPU_BOOLEAN   sender_protocol_verifd;
    CPU_BOOLEAN   addr_conflict;

                                                                /* -------------- VERIFY TARGET HW ADDR --------------- */
#if (NET_ARP_CFG_ADDR_FLTR_EN == DEF_ENABLED)
    NET_UTIL_VAL_COPY_GET_NET_16(&op_code, &parp_hdr->OpCode);
    switch (op_code) {
        case NET_ARP_HDR_OP_REQ:                                /* See Note #2a.                                        */
             target_hw_verifd = DEF_YES;
             break;


        case NET_ARP_HDR_OP_REPLY:                              /* See Note #2b.                                        */
             paddr_target_hw  = parp_hdr->HW_AddrTarget;
             target_hw_verifd = Mem_Cmp((void     *)paddr_target_hw,
                                        (void     *)NetARP_HostAddrPtrHW,
                                        (CPU_SIZE_T)NET_ARP_CFG_HW_ADDR_LEN);
             break;


        default:                                                /* See Note #3.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrRxHdrOpCodeCtr);
            *perr = NET_ARP_ERR_INVALID_OP_CODE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

#else
    target_hw_verifd = DEF_YES;
#endif

                                                                /* ----------- VERIFY TARGET PROTOCOL ADDR ------------ */
    paddr_target_protocol  = parp_hdr->ProtocolAddrTarget;
    target_protocol_verifd = Mem_Cmp((void     *)paddr_target_protocol,
                                     (void     *)NetARP_HostAddrPtrProtocol,
                                     (CPU_SIZE_T)NET_ARP_CFG_PROTOCOL_ADDR_LEN);

                                                                /* ------------ CHK PROTOCOL ADDR CONFLICT ------------ */
    paddr_sender_protocol  = parp_hdr->ProtocolAddrSender;      /* Cmp sender's protocol addr (see Note #4a1A).         */
    sender_protocol_verifd = Mem_Cmp((void     *)paddr_sender_protocol,
                                     (void     *)NetARP_HostAddrPtrProtocol,
                                     (CPU_SIZE_T)NET_ARP_CFG_PROTOCOL_ADDR_LEN);
    paddr_sender_hw        = parp_hdr->HW_AddrSender;           /* Cmp sender's hw       addr (see Note #4a1B).         */
    sender_hw_verifd       = Mem_Cmp((void     *)paddr_sender_hw,
                                     (void     *)NetARP_HostAddrPtrHW,
                                     (CPU_SIZE_T)NET_ARP_CFG_HW_ADDR_LEN);

    addr_conflict = ((sender_protocol_verifd == DEF_YES) &&
                     (sender_hw_verifd       != DEF_YES)) ? DEF_YES : DEF_NO;
    if (addr_conflict == DEF_YES) {                             /* If another hw host's protocol addr matches ...       */
        CPU_CRITICAL_ENTER();                                   /* ...      this host's protocol addr,        ...       */
        NetARP_HostAddrProtocolConflict = DEF_YES;              /* ...  set addr conflict flag (see Note #4a2).         */
        CPU_CRITICAL_EXIT();
    }

                                                                /* -------------- RTN TARGET VALIDATION --------------- */
   *perr = ((target_hw_verifd       == DEF_YES) &&
            (target_protocol_verifd == DEF_YES)) ? NET_ARP_ERR_RX_TARGET_THIS_HOST :
                                                   NET_ARP_ERR_RX_TARGET_NOT_THIS_HOST;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetARP_RxPktFree()
*
* Description : Free network buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetARP_RxPktFree (NET_BUF  *pbuf)
{
    NetBuf_FreeBuf((NET_BUF *)pbuf,
                   (NET_CTR *)0);
}


/*
*********************************************************************************************************
*                                        NetARP_RxPktDiscard()
*
* Description : On any ARP receive errors, discard ARP packet & buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_Rx(). 
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetARP_RxPktDiscard (NET_BUF  *pbuf,
                                   NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetARP_ErrRxPktDiscardedCtr;
#else
    pctr = (NET_CTR *) 0;
#endif
    NetBuf_FreeBuf((NET_BUF *)pbuf,
                   (NET_CTR *)pctr);

   *perr = NET_ERR_RX;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             NetARP_Tx()
*
* Description : (1) Prepare & transmit an ARP Request or ARP Reply :
*
*                   (a) Get network buffer for ARP transmit packet
*                   (b) Prepare & transmit packet
*                   (c) Free      transmit packet buffer(s)
*                   (d) Update    transmit statistics
*
*
* Argument(s) : paddr_hw_sender         Pointer to sender's hardware address (see Note #2).
*
*               paddr_hw_target         Pointer to target's hardware address (see Note #2).
*
*               paddr_protocol_sender   Pointer to sender's protocol address (see Note #2).
*
*               paddr_protocol_target   Pointer to target's protocol address (see Note #2).
*
*               op_code                 ARP operation : Request or Reply.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_NONE                ARP packet successfully transmitted.
*                               NET_ARP_ERR_NULL_PTR            Argument(s) passed a NULL pointer.
*                               NET_ARP_ERR_INVALID_OP_CODE     Invalid ARP operation code.
*
*                                                               ------- RETURNED BY NetBuf_Get() : -------
*                               NET_BUF_ERR_NONE_AVAIL          NO available buffers to allocate.
*                               NET_BUF_ERR_INVALID_TYPE        Network buffer is NOT a valid buffer type.
*                               NET_BUF_ERR_INVALID_SIZE        Requested size is greater then the maximum
*                                                                   buffer size available.
*
*                                                               -- RETURNED BY NetARP_TxPktDiscard() : ---
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
*                                                               -------- RETURNED BY NetIF_Tx() : --------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_TxReq(),
*               NetARP_TxReply(),
*               NetARP_TxReqGratuitous().
*
* Note(s)     : (2) ARP addresses MUST be in network-order.
*
*               (3) Assumes network buffer's protocol header size is large enough to accommodate ARP header
*                   size (see 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1').
*
*               (4) Network buffer already freed by lower layer; only increment error counter.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetARP_Tx (CPU_INT08U  *paddr_hw_sender,
                         CPU_INT08U  *paddr_hw_target,
                         CPU_INT08U  *paddr_protocol_sender,
                         CPU_INT08U  *paddr_protocol_target,
                         CPU_INT16U   op_code,
                         NET_ERR     *perr)
{
    CPU_INT16U   msg_ix;
    NET_BUF     *pbuf;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------- VALIDATE PTRS ------------------ */
    if (paddr_hw_sender == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrTxPktDiscardedCtr);
       *perr = NET_ARP_ERR_NULL_PTR;
        return;
    }

    if (paddr_protocol_sender == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrTxPktDiscardedCtr);
       *perr = NET_ARP_ERR_NULL_PTR;
        return;
    }

    if (paddr_protocol_target == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrTxPktDiscardedCtr);
       *perr = NET_ARP_ERR_NULL_PTR;
        return;
    }

                                                                /* -------------------- VALIDATE OP ------------------- */
    switch (op_code) {
        case NET_ARP_HDR_OP_REQ:                                /* For ARP Req, NULL hw addr ptr expected.              */
             break;


        case NET_ARP_HDR_OP_REPLY:
             if (paddr_hw_target == (CPU_INT08U *)0) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrTxPktDiscardedCtr);
                *perr = NET_ARP_ERR_NULL_PTR;
                 return;
             }
             break;


        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrTxHdrOpCodeCtr);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrTxPktDiscardedCtr);
            *perr = NET_ARP_ERR_INVALID_OP_CODE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif


/*$PAGE*/
                                                                /* --------------------- GET BUF ---------------------- */
#if (NET_BUF_DATA_TX_IX < NET_ARP_HDR_SIZE)                     /* See Note #3.                                         */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrTxInvalidBufIxCtr);
    return;
#endif
    msg_ix = NET_BUF_DATA_TX_IX - NET_ARP_HDR_SIZE;
    pbuf   = NetBuf_Get((NET_BUF_SIZE)NET_ARP_MSG_LEN_DATA,
                        (NET_BUF_SIZE)msg_ix,
                        (CPU_INT16U  )NET_BUF_FLAG_NONE,
                        (NET_ERR    *)perr);
    if (*perr != NET_BUF_ERR_NONE) {
         return;
    }


                                                                /* ---------------- PREPARE/TX ARP PKT ---------------- */
    NetARP_TxPktPrepareHdr(pbuf,
                           msg_ix,
                           paddr_hw_sender,
                           paddr_hw_target,
                           paddr_protocol_sender,
                           paddr_protocol_target,
                           op_code,
                           perr);
    switch (*perr) {
        case NET_ARP_ERR_NONE:
             NetIF_Tx(pbuf, perr);
             break;


        case NET_ARP_ERR_NULL_PTR:
        default:
             NetARP_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ---------- FREE TX PKT / UPDATE TX STATS ----------- */
    switch (*perr) {                                            /* Chk err from NetIF_Tx().                             */
        case NET_IF_ERR_NONE:
             NetARP_TxPktFree(pbuf);
             NET_CTR_STAT_INC(Net_StatCtrs.NetARP_StatTxMsgCtr);
            *perr = NET_ARP_ERR_NONE;
             break;


        case NET_ERR_TX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrTxPktDiscardedCtr);  /* See Note #4.                                 */
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
        
                       
        case NET_ERR_INIT_INCOMPLETE:
        default:
             NetARP_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetARP_TxReq()
*
* Description : (1) Prepare & transmit an ARP Request to resolve a pending ARP cache :
*
*                   (a) Configure sender's hardware address as this host's hardware address
*                   (b) Configure target's hardware address as NULL since unknown
*                   (c) Configure sender's protocol address as this host's protocol address
*                   (d) Configure target's protocol address as the protocol address listed in the ARP cache
*                   (e) Configure ARP operation as ARP Request
*
*
* Argument(s) : pcache      Pointer to an ARP cache.
*               ------      Argument checked in NetARP_CacheAddPend(),
*                                               NetARP_CacheReqTimeout(),
*                                               NetARP_ProbeAddrOnNet().
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_CacheAddPend(),
*               NetARP_CacheReqTimeout(),
*               NetARP_ProbeAddrOnNet().
*
* Note(s)     : (2) Do NOT need to verify success of ARP Request since failure will cause timeouts & retries.
*********************************************************************************************************
*/

static  void  NetARP_TxReq (NET_ARP_CACHE  *pcache)
{
    CPU_INT08U  *paddr_hw_sender;
    CPU_INT08U  *paddr_hw_target;
    CPU_INT08U  *paddr_protocol_sender;
    CPU_INT08U  *paddr_protocol_target;
    CPU_INT16U   op_code;
    NET_ERR      err_tx;

                                                                /* Cfg ARP Req from ARP cache data (see Note #1).       */
    paddr_hw_sender       = (CPU_INT08U *) NetARP_HostAddrPtrHW;
    paddr_hw_target       = (CPU_INT08U *) 0;
    paddr_protocol_sender = (CPU_INT08U *) NetARP_HostAddrPtrProtocol;
    paddr_protocol_target = (CPU_INT08U *)&pcache->ProtocolAddr[0];

    op_code               =  NET_ARP_HDR_OP_REQ;

    NetARP_Tx(paddr_hw_sender,
              paddr_hw_target,
              paddr_protocol_sender,
              paddr_protocol_target,
              op_code,
             &err_tx);

    if (err_tx == NET_ARP_ERR_NONE) {
        NET_CTR_STAT_INC(Net_StatCtrs.NetARP_StatTxMsgReqCtr);
    }


    pcache->ReqAttemptsCtr++;                                   /* Inc req attempts ctr.                                */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetARP_TxReply()
*
* Description : (1) Prepare & transmit an ARP Reply in response to an ARP Request :
*
*                   (a) Configure sender's hardware address as this host's                hardware address
*                   (b) Configure target's hardware address from the ARP Request's sender hardware address
*                   (c) Configure sender's protocol address from the ARP Request's target protocol address
*                   (d) Configure target's protocol address from the ARP Request's sender protocol address
*                   (e) Configure ARP operation as ARP Reply
*
*
* Argument(s) : parp_hdr    Pointer to a packet's ARP header.
*               --------    Argument checked in NetARP_RxPktValidate().
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_RxPktReply().
*
* Note(s)     : (2) Do NOT need to verify success of ARP Reply since failure will cause timeouts & retries.
*********************************************************************************************************
*/

static  void  NetARP_TxReply (NET_ARP_HDR  *parp_hdr)
{
    CPU_INT08U  *paddr_hw_sender;
    CPU_INT08U  *paddr_hw_target;
    CPU_INT08U  *paddr_protocol_sender;
    CPU_INT08U  *paddr_protocol_target;
    CPU_INT16U   op_code;
    NET_ERR      err_tx;

                                                                /* Cfg ARP Reply from ARP Req (see Note #1).            */
    paddr_hw_sender       = (CPU_INT08U *) NetARP_HostAddrPtrHW;
    paddr_hw_target       = (CPU_INT08U *)&parp_hdr->HW_AddrSender[0];
    paddr_protocol_sender = (CPU_INT08U *)&parp_hdr->ProtocolAddrTarget[0];
    paddr_protocol_target = (CPU_INT08U *)&parp_hdr->ProtocolAddrSender[0];

    op_code               =  NET_ARP_HDR_OP_REPLY;

    NetARP_Tx(paddr_hw_sender,
              paddr_hw_target,
              paddr_protocol_sender,
              paddr_protocol_target,
              op_code,
             &err_tx);

    if (err_tx == NET_ARP_ERR_NONE) {
        NET_CTR_STAT_INC(Net_StatCtrs.NetARP_StatTxMsgReplyCtr);
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetARP_TxPktPrepareHdr()
*
* Description : (1) Prepare ARP packet header :
*
*                   (a) Update network buffer's index & length controls
*
*                   (b) Prepare the transmit packet's following ARP header fields :
*
*                       (1) Hardware  Type
*                       (2) Protocol  Type
*                       (3) Hardware  Address Length
*                       (4) Protocol  Address Length
*                       (5) Operation Code
*                       (6) Sender's  Hardware Address
*                       (7) Sender's  Protocol Address
*                       (8) Target's  Hardware Address
*                       (9) Target's  Protocol Address
*
*                   (c) Convert the following ARP header fields from host-order to network-order :
*
*                       (1) Hardware  Type
*                       (2) Protocol  Type
*                       (3) Operation Code
*
*                   (d) Configure ARP protocol address pointer                      See Note #2
*
*
* Argument(s) : pbuf                    Pointer to network buffer to prepare ARP packet.
*
*               msg_ix                  Buffer index to prepare ARP packet.
*               ------                  Argument checked in NetARP_Tx().
*
*               paddr_hw_sender         Pointer to sender's hardware address (see Note #2).
*               ---------------         Argument checked in NetARP_Tx().
*
*               paddr_hw_target         Pointer to target's hardware address (see Note #2).
*               ---------------         Argument checked in NetARP_Tx().
*
*               paddr_protocol_sender   Pointer to sender's protocol address (see Note #2).
*               ---------------------   Argument checked in NetARP_Tx().
*
*               paddr_protocol_target   Pointer to target's protocol address (see Note #2).
*               ---------------------   Argument checked in NetARP_Tx().
*
*               op_code                 ARP operation : Request or Reply.
*               -------                 Argument checked in NetARP_Tx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_NONE                ARP packet successfully prepared.
*                               NET_ARP_ERR_NULL_PTR            Argument 'pbuf' passed a NULL pointer.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_Tx().
*
* Note(s)     : (2) ARP addresses MUST be in network-order.
*
*               (3) Some buffer controls were previously initialized in NetBuf_Get() when the buffer was
*                   allocated.  These buffer controls do NOT need to be re-initialized but are shown for
*                   completeness.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetARP_TxPktPrepareHdr (NET_BUF     *pbuf,
                                      CPU_INT16U   msg_ix,
                                      CPU_INT08U  *paddr_hw_sender,
                                      CPU_INT08U  *paddr_hw_target,
                                      CPU_INT08U  *paddr_protocol_sender,
                                      CPU_INT08U  *paddr_protocol_target,
                                      CPU_INT16U   op_code,
                                      NET_ERR     *perr)
{
    NET_BUF_HDR  *pbuf_hdr;
    NET_ARP_HDR  *parp_hdr;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                         /* --------------- VALIDATE PTR --------------- */
    if (pbuf == (NET_BUF *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
       *perr = NET_ARP_ERR_NULL_PTR;
        return;
    }
#endif


                                                                        /* ------------- UPDATE BUF CTRLS ------------- */
    pbuf_hdr                  = &pbuf->Hdr;
    pbuf_hdr->ARP_MsgIx       = (CPU_INT16U  )msg_ix;
    pbuf_hdr->ARP_MsgLen      = (CPU_INT16U  )NET_ARP_HDR_SIZE;
    pbuf_hdr->TotLen          = (NET_BUF_SIZE)pbuf_hdr->ARP_MsgLen;
    pbuf_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_ARP;
#if 0                                                                   /* Init'd in NetBuf_Get() [see Note #3].        */
    pbuf_hdr->DataIx          =  NET_BUF_IX_NONE;
    pbuf_hdr->DataLen         =  0;
#endif


                                                                        /* ------------- PREPARE ARP HDR -------------- */
    parp_hdr = (NET_ARP_HDR *)&pbuf->Data[pbuf_hdr->ARP_MsgIx];


                                                                        /* ------ PREPARE ARP HW/PROTOCOL TYPES ------- */
    NET_UTIL_VAL_SET_NET_16(&parp_hdr->HW_Type,      NET_ARP_CFG_HW_TYPE);
    NET_UTIL_VAL_SET_NET_16(&parp_hdr->ProtocolType, NET_ARP_CFG_PROTOCOL_TYPE);

                                                                        /* ---- PREPARE ARP HW/PROTOCOL ADDR LENS ----- */
    parp_hdr->HW_AddrLen      =  NET_ARP_CFG_HW_ADDR_LEN;
    parp_hdr->ProtocolAddrLen =  NET_ARP_CFG_PROTOCOL_ADDR_LEN;

                                                                        /* ----------- PREPARE ARP OP CODE ------------ */
    NET_UTIL_VAL_COPY_SET_NET_16(&parp_hdr->OpCode, &op_code);

                                                                        /* --- PREPARE ARP HW/PROTOCOL SENDER ADDRS --- */
    Mem_Copy((void     *)&parp_hdr->HW_AddrSender[0],
             (void     *) paddr_hw_sender,
             (CPU_SIZE_T) NET_ARP_CFG_HW_ADDR_LEN);

    Mem_Copy((void     *)&parp_hdr->ProtocolAddrSender[0],
             (void     *) paddr_protocol_sender,
             (CPU_SIZE_T) NET_ARP_CFG_PROTOCOL_ADDR_LEN);

                                                                        /* --- PREPARE ARP HW/PROTOCOL TARGET ADDRS --- */
    if (paddr_hw_target == (CPU_INT08U *)0) {                           /* If ARP target hw addr NULL for ARP Req, ...  */
        Mem_Clr( (void     *)&parp_hdr->HW_AddrTarget[0],               /* .. clr target hw addr octets.                */
                 (CPU_SIZE_T) NET_ARP_CFG_HW_ADDR_LEN);

        DEF_BIT_SET(pbuf_hdr->Flags, NET_BUF_FLAG_TX_BROADCAST);        /* ARP Req broadcast to ALL hosts on local net. */

    } else {                                                            /* Else copy target hw addr for ARP Reply.      */
        Mem_Copy((void     *)&parp_hdr->HW_AddrTarget[0],
                 (void     *) paddr_hw_target,
                 (CPU_SIZE_T) NET_ARP_CFG_HW_ADDR_LEN);
                                                                        /* ARP Reply tx'd directly to target host.      */
    }

    Mem_Copy((void     *)&parp_hdr->ProtocolAddrTarget[0],
             (void     *) paddr_protocol_target,
             (CPU_SIZE_T) NET_ARP_CFG_PROTOCOL_ADDR_LEN);

                                                                        /* -------- CFG ARP PROTOCOL ADDR PTR --------- */
    pbuf_hdr->ARP_AddrProtocolPtr = &parp_hdr->ProtocolAddrTarget[0];



   *perr = NET_ARP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetARP_TxPktFree()
*
* Description : Free network buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_Tx().
*
* Note(s)     : (1) (a) Although ARP Transmit initially requests the network buffer for transmit, 
*                       the ARP layer does NOT maintain a reference to the buffer.
*
*                   (b) Also, since the network interface layer frees ALL unreferenced buffers 
*                       after successful transmission [see 'net_if_pkt.c  NetIF_Pkt_TxPktFree()'
*                       or 'net_if_char.c  NetIF_Char_TxPktFree()'], the ARP layer MUST not
*                       free the transmit buffer.
*********************************************************************************************************
*/

static  void  NetARP_TxPktFree (NET_BUF  *pbuf)
{
   (void)&pbuf;                                                 /* Prevent compiler warning (see Note #1).              */
}


/*
*********************************************************************************************************
*                                        NetARP_TxPktDiscard()
*
* Description : On any ARP transmit handler errors, discard packet & buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_Tx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetARP_TxPktDiscard (NET_BUF  *pbuf,
                                   NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetARP_ErrTxPktDiscardedCtr;
#else
    pctr = (NET_CTR *) 0;
#endif
    NetBuf_FreeBuf((NET_BUF *)pbuf,
                   (NET_CTR *)pctr);

   *perr = NET_ERR_TX;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetARP_CacheTxPktHandler()
*
* Description : (1) Transmit packet buffers from ARP cache transmit buffer queue :
*
*                   (a) Resolve  packet  buffer(s)' hardware address(s)
*                   (b) Update   packet  buffer(s)' unlink/reference values
*                   (c) Transmit packet  buffer(s)
*
*
* Argument(s) : pbuf_q      Pointer to network buffer(s) to transmit.
*
*               paddr_hw    Pointer to sender's hardware address (see Note #2).
*               --------    Argument checked in NetARP_RxPktCacheUpdate().
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_RxPktCacheUpdate().
*
* Note(s)     : (2) ARP addresses MUST be in network-order.
*
*               (3) RFC #1122, Section 2.3.2.2 states that "the link layer SHOULD" :
*
*                   (a) "Save (rather than discard) ... packets destined to the same unresolved
*                        IP address and" ...
*                   (b) "Transmit the saved packet[s] when the address has been resolved."
*********************************************************************************************************
*/

static  void  NetARP_CacheTxPktHandler (NET_BUF     *pbuf_q,
                                        CPU_INT08U  *paddr_hw)
{
    NET_BUF      *pbuf_list;
    NET_BUF      *pbuf_list_next;
    NET_BUF      *pbuf;
    NET_BUF      *pbuf_next;
    NET_BUF_HDR  *pbuf_hdr;
    CPU_INT08U   *pbuf_addr_hw;
    NET_ERR       err;


    pbuf_list = pbuf_q;
    while (pbuf_list  != (NET_BUF *)0) {                        /* Handle ALL buf lists in Q.                           */
        pbuf_hdr       = &pbuf_list->Hdr;
        pbuf_list_next = (NET_BUF *)pbuf_hdr->NextSecListPtr;

        pbuf           = (NET_BUF *)pbuf_list;
        while (pbuf   != (NET_BUF *)0) {                        /* Handle ALL bufs in buf list.                         */
            pbuf_hdr   = &pbuf->Hdr;
            pbuf_next  = (NET_BUF *)pbuf_hdr->NextBufPtr;

            pbuf_addr_hw = pbuf_hdr->ARP_AddrHW_Ptr;
            Mem_Copy((void     *)pbuf_addr_hw,                  /* Copy hw addr into pkt buf.                           */
                     (void     *)paddr_hw,
                     (CPU_SIZE_T)NET_ARP_CFG_HW_ADDR_LEN);
                                                                /* Clr buf sec list & unlink ptrs.                      */
            pbuf_hdr->PrevSecListPtr = (NET_BUF    *)0;
            pbuf_hdr->NextSecListPtr = (NET_BUF    *)0;
            pbuf_hdr->UnlinkFnctPtr  = (CPU_FNCT_PTR)0;
            pbuf_hdr->UnlinkObjPtr   = (void       *)0;

            NetIF_TxPkt(pbuf, &err);                            /* Tx pkt to IF (see Note #3b).                         */

            pbuf  = pbuf_next;
        }

        pbuf_list = pbuf_list_next;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetARP_CacheSrch()
*
* Description : Search ARP Cache List for ARP cache with specific protocol address.
*
*               (1) (a) ARP Cache List resolves protocol-address-to-hardware-address bindings based on the
*                       following ARP cache fields :
*
*                           - Hardware Type                     Configured at compile time (see Note #1a1)
*                           - Hardware Address Length           Configured at compile time (see Note #1a1)
*                           - Protocol Type                     Configured at compile time (see Note #1a1)
*                           - Protocol Address Length           Configured at compile time (see Note #1a1)
*                           - Protocol Address                  Generated  at run time 
*
*                       (1) These fields are configured at compile time (see 'net_arp.h  ARP CACHE  Note #3').
*
*                   (b) ARP caches are linked to form an ARP Cache List.  In the diagram below, the 
*                       top horizontal row represents the list of ARP caches.
*
*                       (1) 'NetARP_CacheListHead' points to the head of the ARP Cache List; 
*                           'NetARP_CacheListTail' points to the tail of the ARP Cache List.  
*
*                       (2) ARP caches' 'PrevPtr' & 'NextPtr' doubly-link each ARP cache to form the
*                           ARP Cache List.
*
*                   (c) ARP caches in the 'PENDING' state are pending hardware address resolution by an 
*                       ARP Reply.  While in the 'PENDING' state, ALL transmit packet buffers are enqueued 
*                       for later transmission when the corresponding ARP Reply is received.
*
*                       (1) ARP caches' 'BufQ_Head' points to the head of the pending transmit packet queue;
*                           ARP caches' 'BufQ_Tail' points to the tail of the pending transmit packet queue.
*
*                       (2) Buffer's 'PrevSecListPtr' & 'NextSecListPtr' link each buffer in a pending transmit
*                           packet queue.
*
*                   (d) For any ARP cache lookup, all ARP caches are searched in order to find the ARP cache
*                       with the appropriate hardware address--i.e. the ARP cache with the corresponding 
*                       protocol address (see Note #1a).
*
*                   (e) New ARP caches are added at the head of the ARP Cache List.  This allows faster ARP
*                       cache lookup for recently added ARP caches.
*
*                   (f) As ARP caches are added into the list, older ARP caches migrate to the tail of the 
*                       ARP Cache List.  Once an ARP cache expires or is discarded, it is removed from the 
*                       ARP Cache List.  Also if the ARP Cache List is full & a new ARP cache is needed,
*                       then the oldest ARP cache at the tail of the list is removed for allocation.
*
*$PAGE*
*
*                                          |                                               |
*                                          |<------------ List of ARP Caches ------------->|
*                                          |                (see Note #1b)                 |
*
*                                      New ARP caches                           Oldest ARP cache
*                                     inserted at head                          in ARP Cache List
*                                      (see Note #1e)                            (see Note #1f)
*
*                                             |             NextPtr                     |
*                                             |         (see Note #1b2)                 |
*                                             v                   |                     v
*                                                                 |
*                           Head of        -------       -------  v    -------       -------    (see Note #1b1)
*                          ARP Cache  ---->|     |------>|     |------>|     |------>|     |
*                            List          |     |       |     |       |     |       |     |        Tail of
*                                          |     |<------|     |<------|     |<------|     |<----  ARP Cache
*                      (see Note #1b1)     -------       -------  ^    -------       -------         List
*                                            | |                  |      | |     
*                                            | |                  |      | |     
*                                            | ------       PrevPtr      | ------
*                            BufQ_Head  ---> |      |   (see Note #1b2)  |      | <---  BufQ_Tail
*                         (see Note #1c1)    v      |                    v      |    (see Note #1c1)
*               ---                        -------  |                  -------  |
*                ^                         |     |  |                  |     |  |
*                |                         |     |  |                  |     |  |
*                |                         |     |  |                  |     |  |
*                |                         |     |  |                  |     |  |
*                |                         -------  |                  -------  |
*                |                           | ^    |                    | ^    |   
*                |       NextSecListPtr ---> | |    |                    | | <----- PrevSecListPtr 
*                |       (see Note #1c2)     v |    |                    v |    |   (see Note #1c2)
*                |                         -------  |                  -------  |
*                                          |     |  |                  |     |<--
*        Buffers pending on                |     |  |                  |     |
*       ARP cache resolution               |     |  |                  -------
*          (see Note #1c)                  |     |  |
*                                          -------  |
*                |                           | ^    |
*                |                           | |    |
*                |                           v |    |
*                |                         -------  |
*                |                         |     |<--
*                |                         |     |
*                |                         |     |
*                v                         |     |
*               ---                        -------
*
*
* Argument(s) : paddr_protocol      Pointer to protocol address (see Note #2).
*               --------------      Argument checked in NetARP_CacheHandler(),
*                                                       NetARP_ProbeAddrOnNet().
*
* Return(s)   : Pointer to ARP cache with specific protocol address, if found.
*
*               Pointer to NULL,                                     otherwise.
*
* Caller(s)   : NetARP_CacheHandler(),
*               NetARP_ProbeAddrOnNet().
*
* Note(s)     : (2) 'paddr_protocol' MUST point to a protocol address that is in network-order.
*
*                   See also 'NetARP_CacheHandler()  Note #2e3'.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_ARP_CACHE  *NetARP_CacheSrch (CPU_INT08U  *paddr_protocol)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR          cpu_sr;
#endif
    NET_ARP_CACHE  *pcache;
    NET_ARP_CACHE  *pcache_next;
    CPU_INT08U     *pcache_addr;
    CPU_INT16U      th;
    CPU_BOOLEAN     found;


    CPU_CRITICAL_ENTER();
    NET_CTR_STAT_INC_LARGE(NetARP_CacheSrchCtr_hi, NetARP_CacheSrchCtr_lo);
    CPU_CRITICAL_EXIT();

    pcache = NetARP_CacheListHead;                                  /* Start @ ARP Cache List head.                     */
    found  = DEF_NO;

    while ((pcache != (NET_ARP_CACHE *)0) &&                        /* Srch ARP Cache List ...                          */
           (found  ==  DEF_NO)) {                                   /* ... until cache found.                           */

        pcache_next = (NET_ARP_CACHE *) pcache->NextPtr;
        pcache_addr = (CPU_INT08U    *)&pcache->ProtocolAddr[0];
                                                                    /* Cmp ARP cache protocol addr.                     */
        found       =  Mem_Cmp((void     *)paddr_protocol,
                               (void     *)pcache_addr,
                               (CPU_SIZE_T)NET_ARP_CFG_PROTOCOL_ADDR_LEN);

        if (found != DEF_YES) {                                     /* If NOT found, adv to next ARP cache.             */
            pcache = pcache_next;
        } else {                                                    /* Else rtn found ARP cache.                        */
            pcache->AccessedCtr++;
            CPU_CRITICAL_ENTER();
            th = NetARP_CacheAccessedTh_nbr;
            CPU_CRITICAL_EXIT();
            if (pcache->AccessedCtr > th) {                         /* If ARP cache accessed > th, & ...                */
                pcache->AccessedCtr = 0;
                if (pcache != NetARP_CacheListHead) {               /* .. ARP cache NOT @ list head, ...                */
                    NetARP_CacheUnlink(pcache);
                    NetARP_CacheInsert(pcache);                     /* .. promote ARP cache to list head.               */
                }
            }

            CPU_CRITICAL_ENTER();
            NET_CTR_STAT_INC_LARGE(NetARP_CacheFoundCtr_hi, NetARP_CacheFoundCtr_lo);
            CPU_CRITICAL_EXIT();
        }
    }

    return (pcache);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetARP_CacheAddPend()
*
* Description : (1) Add a 'PENDING' ARP cache into the ARP Cache List & transmit an ARP Request :
*
*                   (a) Configure ARP cache :
*                       (1) Get default-configured ARP cache
*                       (2) ARP cache state
*                       (3) Enqueue transmit buffer to ARP cache queue
*                   (b) Insert   ARP cache into ARP Cache List
*                   (c) Transmit ARP Request to resolve ARP cache
*
*
* Argument(s) : pbuf            Pointer to network buffer to transmit.
*               ----            Argument checked   in NetARP_CacheHandler().
*
*               pbuf_hdr        Pointer to network buffer header.
*               --------        Argument validated in NetARP_CacheHandler().
*
*               paddr_protocol  Pointer to protocol address (see Note #2).
*               --------------  Argument checked   in NetARP_CacheHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_CACHE_PEND              ARP cache added in 'PENDING' state.
*
*                                                                   --- RETURNED BY NetARP_CacheCfg() : ----
*                               NET_ARP_ERR_CACHE_NONE_AVAIL        NO available ARP caches to allocate.
*                               NET_ARP_ERR_CACHE_INVALID_TYPE      ARP cache is NOT a valid cache type.
*                               NET_TMR_ERR_NULL_OBJ                Argument 'obj'  passed a NULL pointer.
*                               NET_TMR_ERR_NULL_FNCT               Argument 'fnct' passed a NULL pointer.
*                               NET_TMR_ERR_NONE_AVAIL              NO available timers to allocate.
*                               NET_TMR_ERR_INVALID_TYPE            Network timer is NOT a valid timer type.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_CacheHandler().
*
* Note(s)     : (2) Assumes 'paddr_protocol' points to a valid protocol address that is in network-order.
*
*                   See also 'NetARP_CacheHandler()  Note #2e3'.
*
*               (3) (a) RFC #1122, Section 2.3.2.2 states that "the link layer SHOULD" :
*
*                       (1) "Save (rather than discard) ... packets destined to the same unresolved
*                            IP address and" ...
*                       (2) "Transmit the saved packet[s] when the address has been resolved."
*
*                   (b) Since ARP Layer is the last layer to handle & queue the transmit network 
*                       buffer, it is NOT necessary to increment the network buffer's reference 
*                       counter to include the pending ARP cache buffer queue as a new reference
*                       to the network buffer.
*
*               (4) Some buffer controls were previously initialized in NetBuf_Get() when the packet
*                   was received at the network interface layer.  These buffer controls do NOT need
*                   to be re-initialized but are shown for completeness.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetARP_CacheAddPend (NET_BUF      *pbuf,
                                   NET_BUF_HDR  *pbuf_hdr,
                                   CPU_INT08U   *paddr_protocol,
                                   NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR          cpu_sr;
#endif
    NET_ARP_CACHE  *pcache;
    NET_TMR_TICK    timeout_tick;

                                                                /* ------------------ CFG ARP CACHE ------------------- */
    CPU_CRITICAL_ENTER();
    timeout_tick = NetARP_ReqTimeout_tick;
    CPU_CRITICAL_EXIT();
    pcache       = NetARP_CacheCfg((CPU_INT08U *)0,
                                   (CPU_INT08U *)paddr_protocol,
                                   (CPU_FNCT_PTR)NetARP_CacheReqTimeout,
                                   (NET_TMR_TICK)timeout_tick,
                                   (NET_ERR    *)perr);
    if (*perr != NET_ARP_ERR_NONE) {
         return;
    }

                                                                /* Cfg buf's unlink fnct/obj to ARP cache.              */
    pbuf_hdr->UnlinkFnctPtr  = (CPU_FNCT_PTR)NetARP_CacheUnlinkBuf;
    pbuf_hdr->UnlinkObjPtr   = (void       *)pcache;

#if 0                                                           /* Init'd in NetBuf_Get() [see Note #4].                */
    pbuf_hdr->PrevSecListPtr = (NET_BUF    *)0;
    pbuf_hdr->NextSecListPtr = (NET_BUF    *)0;
#endif
                                                                /* Q buf to ARP cache (see Note #3a1).                  */
    pcache->BufQ_Head        = (NET_BUF    *)pbuf;
    pcache->BufQ_Tail        = (NET_BUF    *)pbuf;

    pcache->State            =  NET_ARP_CACHE_STATE_PEND;

                                                                /* ------- INSERT ARP CACHE INTO ARP CACHE LIST ------- */
    NetARP_CacheInsert(pcache);

                                                                /* -------------------- TX ARP REQ -------------------- */
    NetARP_TxReq(pcache);                                       /* Tx ARP req to resolve ARP cache.                     */


   *perr = NET_ARP_ERR_CACHE_PEND;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetARP_CacheAddResolved()
*
* Description : (1) Add a 'RESOLVED' ARP cache into the ARP Cache List :
*
*                   (a) Configure ARP cache :
*                       (1) Get default-configured ARP cache
*                       (2) ARP cache state
*                   (b) Insert    ARP cache into ARP Cache List
*
*
* Argument(s) : paddr_hw        Pointer to hardware address (see Note #2).
*               --------        Argument checked in NetARP_RxPktCacheUpdate().
*
*               paddr_protocol  Pointer to protocol address (see Note #2).
*               --------------  Argument checked in NetARP_RxPktCacheUpdate().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_CACHE_RESOLVED          ARP cache added in 'RESOLVED' state.
*
*                                                                   --- RETURNED BY NetARP_CacheCfg() : ----
*                               NET_ARP_ERR_CACHE_NONE_AVAIL        NO available ARP caches to allocate.
*                               NET_ARP_ERR_CACHE_INVALID_TYPE      ARP cache is NOT a valid cache type.
*                               NET_TMR_ERR_NULL_OBJ                Argument 'obj'  passed a NULL pointer.
*                               NET_TMR_ERR_NULL_FNCT               Argument 'fnct' passed a NULL pointer.
*                               NET_TMR_ERR_NONE_AVAIL              NO available timers to allocate.
*                               NET_TMR_ERR_INVALID_TYPE            Network timer is NOT a valid timer type.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_RxPktCacheUpdate().
*
* Note(s)     : (2) Assumes 'paddr_hw' & 'paddr_protocol' point to valid hardware & protocol addresses 
*                   that are in network-order.
*
*                   See also 'NetARP_CacheHandler()  Note #2e3'.
*********************************************************************************************************
*/

static  void  NetARP_CacheAddResolved (CPU_INT08U  *paddr_hw,
                                       CPU_INT08U  *paddr_protocol,
                                       NET_ERR     *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR          cpu_sr;
#endif
    NET_TMR_TICK    timeout_tick;
    NET_ARP_CACHE  *pcache;

                                                                /* ------------------ CFG ARP CACHE ------------------- */
    CPU_CRITICAL_ENTER();
    timeout_tick = NetARP_CacheTimeout_tick;
    CPU_CRITICAL_EXIT();
    pcache       = NetARP_CacheCfg((CPU_INT08U *)paddr_hw,
                                   (CPU_INT08U *)paddr_protocol,
                                   (CPU_FNCT_PTR)NetARP_CacheTimeout,
                                   (NET_TMR_TICK)timeout_tick,
                                   (NET_ERR    *)perr);
    if (*perr != NET_ARP_ERR_NONE) {
         return;
    }

    pcache->State = NET_ARP_CACHE_STATE_RESOLVED;

                                                                /* ------- INSERT ARP CACHE INTO ARP CACHE LIST ------- */
    NetARP_CacheInsert(pcache);


   *perr = NET_ARP_ERR_CACHE_RESOLVED;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetARP_CacheRemove()
*
* Description : (1) Remove an ARP cache from the ARP Cache List :
*
*                   (a) Remove ARP cache from    ARP Cache List
*                   (b) Free   ARP cache back to ARP cache pool
*
*
* Argument(s) : pcache      Pointer to an ARP cache.
*               ------      Argument checked in NetARP_CacheHandler(),
*                                               NetARP_RxPktCacheUpdate(),
*                                               NetARP_CacheGet(),
*                                               NetARP_CacheTimeout(),
*                                               NetARP_CacheReqTimeout(),
*                                               NetARP_ProbeAddrOnNet().
*
*               tmr_free    Indicate whether to free network timer :
*
*                               DEF_YES                Free network timer for ARP cache.
*                               DEF_NO          Do NOT free network timer for ARP cache
*                                                     [Freed by NetTmr_TaskHandler()].
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_CacheHandler(),
*               NetARP_RxPktCacheUpdate(),
*               NetARP_CacheGet(),
*               NetARP_CacheTimeout(),
*               NetARP_CacheReqTimeout(),
*               NetARP_ProbeAddrOnNet().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetARP_CacheRemove (NET_ARP_CACHE  *pcache,
                                  CPU_BOOLEAN     tmr_free)
{
                                                                /* ------- REMOVE ARP CACHE FROM ARP CACHE LIST ------- */
    NetARP_CacheUnlink(pcache);
                                                                /* ------------------ FREE ARP CACHE ------------------ */
    NetARP_CacheFree(pcache, tmr_free);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetARP_CacheInsert()
*
* Description : Insert an ARP cache into the ARP Cache List at the head of the list.
*
* Argument(s) : pcache      Pointer to an ARP cache.
*               ------      Argument checked in NetARP_CacheSrch(),
*                                               NetARP_CacheAddPend(),
*                                               NetARP_CacheAddResolved().
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_CacheSrch(),
*               NetARP_CacheAddPend(),
*               NetARP_CacheAddResolved(),
*               NetARP_ProbeAddrOnNet().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetARP_CacheInsert (NET_ARP_CACHE  *pcache)
{
    pcache->PrevPtr = (NET_ARP_CACHE *)0;
    pcache->NextPtr = (NET_ARP_CACHE *)NetARP_CacheListHead;

    if (NetARP_CacheListHead != (NET_ARP_CACHE *)0) {           /* If list NOT empty, insert before head.               */
        NetARP_CacheListHead->PrevPtr = pcache;
    } else {                                                    /* Else add first ARP cache to list.                    */
        NetARP_CacheListTail          = pcache;
    }
    NetARP_CacheListHead = pcache;                              /* Insert ARP cache @ list head.                        */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetARP_CacheUnlink()
*
* Description : Unlink an ARP cache from the ARP Cache List.
*
* Argument(s) : pcache      Pointer to an ARP cache.
*               ------      Argument checked in NetARP_CacheSrch();
*                                               NetARP_CacheRemove()
*                                                 by NetARP_CacheHandler(),
*                                                    NetARP_CacheGet(),
*                                                    NetARP_CacheReqTimeout(),
*                                                    NetARP_CacheTimeout().
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_CacheSrch(),
*               NetARP_CacheRemove().
*
* Note(s)     : (1) Since NetARP_CacheUnlink() called ONLY to remove & then re-link or free ARP caches, 
*                   it is NOT necessary to clear the entry's previous & next pointers.  However, pointers
*                   cleared to NULL shown for correctness & completeness.
*********************************************************************************************************
*/

static  void  NetARP_CacheUnlink (NET_ARP_CACHE  *pcache)
{
    NET_ARP_CACHE  *pcache_prev;
    NET_ARP_CACHE  *pcache_next;

                                                                /* ------- UNLINK ARP CACHE FROM ARP CACHE LIST ------- */
    pcache_prev = pcache->PrevPtr;
    pcache_next = pcache->NextPtr;
                                                                /* Point prev ARP cache to next ARP cache.              */
    if (pcache_prev != (NET_ARP_CACHE *)0) {
        pcache_prev->NextPtr = pcache_next;
    } else {
        NetARP_CacheListHead = pcache_next;
    }
                                                                /* Point next ARP cache to prev ARP cache.              */
    if (pcache_next != (NET_ARP_CACHE *)0) {
        pcache_next->PrevPtr = pcache_prev;
    } else {
        NetARP_CacheListTail = pcache_prev;
    }

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)                     /* Clr ARP cache's ptrs (see Note #1).                  */
    pcache->PrevPtr = (NET_ARP_CACHE *)0;
    pcache->NextPtr = (NET_ARP_CACHE *)0;
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetARP_CacheUnlinkBuf()
*
* Description : Unlink a network buffer from a cache's transmit queue.
*
* Argument(s) : pbuf        Pointer to network buffer enqueued in an ARP cache transmit buffer queue.
*
* Return(s)   : none.
*
* Caller(s)   : Referenced in NetARP_CacheHandler(),
*                             NetARP_CacheAddPend().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetARP_CacheUnlinkBuf (NET_BUF  *pbuf)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN     used;
#endif
    NET_BUF        *pbuf_prev;
    NET_BUF        *pbuf_next;
    NET_BUF_HDR    *pbuf_hdr;
    NET_BUF_HDR    *pbuf_hdr_prev;
    NET_BUF_HDR    *pbuf_hdr_next;
    NET_ARP_CACHE  *pcache;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
        return;
    }
                                                                /* ------------------- VALIDATE BUF ------------------- */
    used = NetBuf_IsUsed(pbuf);
    if (used != DEF_YES) {
        return;
    }
#endif

    pbuf_hdr = (NET_BUF_HDR   *)&pbuf->Hdr;
    pcache   = (NET_ARP_CACHE *) pbuf_hdr->UnlinkObjPtr;

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE ARP CACHE ---------------- */
    used = NetARP_CacheIsUsed(pcache);
    if (used != DEF_YES) {
        return;
    }
#endif


                                                                /* ---------- UNLINK BUF FROM ARP CACHE TX Q ---------- */
    pbuf_prev = pbuf_hdr->PrevSecListPtr;
    pbuf_next = pbuf_hdr->NextSecListPtr;
                                                                /* Point prev ARP cache pending tx Q buf to next buf.   */
    if (pbuf_prev != (NET_BUF *)0) {
        pbuf_hdr_prev                 = &pbuf_prev->Hdr;
        pbuf_hdr_prev->NextSecListPtr =  pbuf_next;
    } else {
        pcache->BufQ_Head             =  pbuf_next;
    }
                                                                /* Point next ARP cache pending tx Q buf to prev buf.   */
    if (pbuf_next != (NET_BUF *)0) {
        pbuf_hdr_next                 = &pbuf_next->Hdr;
        pbuf_hdr_next->PrevSecListPtr =  pbuf_prev;
    } else {
        pcache->BufQ_Tail             =  pbuf_prev;
    }


                                                                /* -------------- CLR BUF'S UNLINK CTRLS -------------- */
    pbuf_hdr->PrevSecListPtr = (NET_BUF    *)0;                 /* Clr pending tx Q ptrs.                               */
    pbuf_hdr->NextSecListPtr = (NET_BUF    *)0;

    pbuf_hdr->UnlinkFnctPtr  = (CPU_FNCT_PTR)0;                 /* Clr unlink ptrs.                                     */
    pbuf_hdr->UnlinkObjPtr   = (void       *)0;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetARP_CacheTimeout()
*
* Description : Discard an ARP cache in the 'RESOLVED' state on timeout.
*
* Argument(s) : pcache_timeout      Pointer to an ARP cache (see Note #2b).
*
* Return(s)   : none.
*
* Caller(s)   : Referenced in NetARP_CacheAddResolved().
*
* Note(s)     : (1) RFC #1122, Section 2.3.2.1 states that "an implementation of the Address Resolution
*                   Protocol (ARP) ... MUST provide a mechanism to flush out-of-date cache entries".
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
*                       (1) Be defined as 'CPU_FNCT_PTR' type (i.e. '[(void) (void *)]'); &       ...
*                       (2) Explicitly cast 'void' pointer arguments to specific object pointers; ...
*                           (A) in this case, a 'NET_ARP_CACHE' pointer.
*
*                   See also 'net_tmr.c  NetTmr_Get()  Note #3'.
*
*               (3) This function is a network timer callback function :
*
*                   (a) Clear the timer pointer; ...
*                       (1) Cleared in NetARP_CacheFree() via NetARP_CacheRemove().
*
*                   (b) but do NOT re-free the timer.
*********************************************************************************************************
*/

static  void  NetARP_CacheTimeout (void  *pcache_timeout)
{
    NET_ARP_CACHE  *pcache;


    pcache = (NET_ARP_CACHE *)pcache_timeout;                   /* See Note #2b2A.                                      */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (pcache == (NET_ARP_CACHE *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
        return;
    }
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    if (pcache->Type != NET_ARP_TYPE_CACHE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrInvalidTypeCtr);
        return;
    }
#endif

                                                                /* ---------------- DISCARD ARP CACHE ----------------- */
    NetARP_CacheRemove((NET_ARP_CACHE *)pcache,
                       (CPU_BOOLEAN    )DEF_NO);                /* Clr but do NOT free tmr (see Note #3).               */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetARP_CacheReqTimeout()
*
* Description : Retry ARP Request to resolve an ARP cache in the 'PENDING' state on ARP Request timeout.
*
* Argument(s) : pcache_timeout      Pointer to an ARP cache (see Note #1b).
*
* Return(s)   : none.
*
* Caller(s)   : Referenced in NetARP_CacheAddPend().
*
* Note(s)     : (1) Ideally, network timer callback functions could be defined as '[(void) (OBJECT *)]' 
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
*                       (1) Be defined as 'CPU_FNCT_PTR' type (i.e. '[(void) (void *)]'); &       ...
*                       (2) Explicitly cast 'void' pointer arguments to specific object pointers; ...
*                           (A) in this case, a 'NET_ARP_CACHE' pointer.
*
*                   See also 'net_tmr.c  NetTmr_Get()  Note #3'.
*
*               (2) This function is a network timer callback function :
*
*                   (a) Clear the timer pointer; ...
*                       (1) Cleared in NetARP_CacheFree() via NetARP_CacheRemove(); or
*                       (2) Reset   by NetTmr_Get().
*
*                   (b) but do NOT re-free the timer.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetARP_CacheReqTimeout (void  *pcache_timeout)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR          cpu_sr;
#endif
    NET_ARP_CACHE  *pcache;
    NET_TMR_TICK    timeout_tick;
    CPU_INT08U      th_max;
    NET_ERR         err;


    pcache = (NET_ARP_CACHE *)pcache_timeout;                   /* See Note #1b2A.                                      */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (pcache == (NET_ARP_CACHE *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
        return;
    }
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    if (pcache->Type != NET_ARP_TYPE_CACHE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrInvalidTypeCtr);
        return;
    }
#endif


    CPU_CRITICAL_ENTER();
    th_max = NetARP_ReqMaxAttempts_nbr;
    CPU_CRITICAL_EXIT();
    if (pcache->ReqAttemptsCtr >= th_max) {                     /* If nbr attempts >= max, free ARP cache.              */
        NetARP_CacheRemove((NET_ARP_CACHE *)pcache,
                           (CPU_BOOLEAN    )DEF_NO);            /* Clr but do NOT free tmr (see Note #2).               */
        return;
    }

                                                                /* ------------------ RETRY ARP REQ ------------------- */
    CPU_CRITICAL_ENTER();
    timeout_tick   = NetARP_ReqTimeout_tick;
    CPU_CRITICAL_EXIT();
    pcache->TmrPtr = NetTmr_Get((void       *) pcache,
                                (CPU_FNCT_PTR) NetARP_CacheReqTimeout,
                                (NET_TMR_TICK) timeout_tick,
                                (CPU_INT16U  ) NET_TMR_FLAG_NONE,
                                (NET_ERR    *)&err);
    if (err != NET_TMR_ERR_NONE) {                              /* If tmr unavail, free ARP cache.                      */
        NetARP_CacheRemove((NET_ARP_CACHE *)pcache,
                           (CPU_BOOLEAN    )DEF_NO);            /* Clr but do NOT free tmr (see Note #2).               */
        return;
    }

    NetARP_TxReq(pcache);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetARP_CacheCfg()
*
* Description : (1) Configure an ARP cache :
*
*                   (a) Get ARP cache from ARP cache pool
*                   (b) Get ARP cache timer
*                   (c) Configure ARP cache :
*                       (1) Configure ARP cache addresses :
*                           (A) Hardware address
*                           (B) Protocol address
*                       (2) Configure ARP cache controls
*
*
* Argument(s) : paddr_hw        Pointer to hardware address (see Note #2a).
*
*               paddr_protocol  Pointer to protocol address (see Note #2b).
*               --------------  Argument checked in NetARP_ProbeAddrOnNet(),
*                                                   NetARP_CacheAddPend(),
*                                                   NetARP_CacheAddResolved().
*
*               timeout_fnct    Pointer to timeout function.
*
*               timeout_tick    Timeout value (in 'NET_TMR_TICK' ticks).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_NONE                    ARP cache successfully configured.
*
*                                                                   ---- RETURNED BY NetARP_CacheGet() : ---
*                               NET_ARP_ERR_CACHE_NONE_AVAIL        NO available ARP caches to allocate.
*                               NET_ARP_ERR_CACHE_INVALID_TYPE      ARP cache is NOT a valid cache type.
*
*                                                                   ------ RETURNED BY NetTmr_Get() : ------
*                               NET_TMR_ERR_NULL_OBJ                Argument 'obj'  passed a NULL pointer.
*                               NET_TMR_ERR_NULL_FNCT               Argument 'fnct' passed a NULL pointer.
*                               NET_TMR_ERR_NONE_AVAIL              NO available timers to allocate.
*                               NET_TMR_ERR_INVALID_TYPE            Network timer is NOT a valid timer type.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_ProbeAddrOnNet(),
*               NetARP_CacheAddPend(),
*               NetARP_CacheAddResolved().
*
* Note(s)     : (2) (a) If 'paddr_hw' available, points to a valid hardware address that is in network-order.
*
*                   (b) Assumes 'paddr_protocol' points to a valid protocol address that is in network-order.
*
*                   See also 'NetARP_CacheHandler()  Note #2e3'.
*
*               (3) On ANY errors, network resources MUST be appropriately freed.
*
*               (4) During ARP cache initialization, some ARP cache controls were previously initialized
*                   in NetARP_CacheGet() when the ARP cache was allocated from the ARP cache pool.  These
*                   ARP cache controls do NOT need to be re-initialized but are shown for completeness.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_ARP_CACHE  *NetARP_CacheCfg (CPU_INT08U    *paddr_hw,
                                         CPU_INT08U    *paddr_protocol,
                                         CPU_FNCT_PTR   timeout_fnct,
                                         NET_TMR_TICK   timeout_tick,
                                         NET_ERR       *perr)
{
    NET_ARP_CACHE  *pcache;

                                                                /* ------------------ GET ARP CACHE ------------------- */
    pcache = NetARP_CacheGet(perr);
    if (pcache == (NET_ARP_CACHE *)0) {
        return   ((NET_ARP_CACHE *)0);                          /* Rtn err from NetARP_CacheGet().                      */
    }

                                                                /* ---------------- GET ARP CACHE TMR ----------------- */
    pcache->TmrPtr = NetTmr_Get((void       *)pcache,
                                (CPU_FNCT_PTR)timeout_fnct,
                                (NET_TMR_TICK)timeout_tick,
                                (CPU_INT16U  )NET_TMR_FLAG_NONE,
                                (NET_ERR    *)perr);
    if (*perr != NET_TMR_ERR_NONE) {                            /* If tmr unavail, ...                                  */
         NetARP_CacheFree(pcache, DEF_NO);                      /* ... free ARP cache (see Note #3).                    */
         return ((NET_ARP_CACHE *)0);
    }

                                                                /* ------------------ CFG ARP CACHE ------------------- */
                                                                /* Cfg ARP cache addr(s).                               */
    if (paddr_hw != (CPU_INT08U *)0) {                          /* If hw addr avail (see Note #2a), ...                 */
        Mem_Copy((void     *)&pcache->HW_Addr[0],               /* ... copy into ARP cache.                             */
                 (void     *) paddr_hw,
                 (CPU_SIZE_T) NET_ARP_CFG_HW_ADDR_LEN);
    }

    Mem_Copy((void     *)&pcache->ProtocolAddr[0],              /* Copy protocol addr into ARP cache (see Note #2b).    */
             (void     *) paddr_protocol,
             (CPU_SIZE_T) NET_ARP_CFG_PROTOCOL_ADDR_LEN);

                                                                /* Cfg ARP cache ctrl(s).                               */
#if 0                                                           /* Init'd in NetARP_CacheGet() [see Note #4].           */
    pcache->AccessedCtr    =  0;
    pcache->ReqAttemptsCtr =  0;

    pcache->BufQ_Head      = (NET_BUF       *)0;
    pcache->BufQ_Tail      = (NET_BUF       *)0;
                                                                /* Cfg'd  in NetARP_CacheInsert().                      */
    pcache->PrevPtr        = (NET_ARP_CACHE *)0;
    pcache->NextPtr        = (NET_ARP_CACHE *)0;
#endif


   *perr =  NET_ARP_ERR_NONE;

    return (pcache);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetARP_CacheGet()
*
* Description : (1) Allocate & initialize an ARP cache :
*
*                   (a) Get an ARP cache
*                   (b) Validate   ARP cache
*                   (c) Initialize ARP cache
*                   (d) Update ARP cache pool statistics
*                   (e) Return pointer to ARP cache
*                         OR
*                       Null pointer & error code, on failure
*
*               (2) The ARP cache pool is implemented as a stack :
*
*                   (a) 'NetARP_CachePoolPtr' points to the head of the ARP cache pool.
*
*                   (b) ARP caches' 'NextPtr's link each ARP cache to form the ARP cache pool stack.
*
*                   (c) ARP caches are inserted & removed at the head of   the ARP cache pool stack.
*
*
*                                       ARP caches are
*                                     inserted & removed
*                                        at the head
*                                       (see Note #2c)
*
*                                             |                 NextPtr
*                                             |             (see Note #2b)
*                                             v                    |
*                                                                  |
*                                          -------       -------   v   -------       -------
*                            ARP Pool ---->|     |------>|     |------>|     |------>|     |
*                            Pointer       |     |       |     |       |     |       |     |
*                                          |     |       |     |       |     |       |     |
*                         (see Note #2a)   -------       -------       -------       -------
*
*                                          |                                               |
*                                          |<---------- Pool of Free ARP Caches ---------->|
*                                          |                (see Note #2)                  |
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ARP_ERR_NONE                    ARP cache successfully allocated &
*                                                                       initialized.
*                               NET_ARP_ERR_CACHE_NONE_AVAIL        NO available ARP caches to allocate.
*                               NET_ARP_ERR_CACHE_INVALID_TYPE      ARP cache is NOT a valid cache type.
*
* Return(s)   : Pointer to ARP cache, if NO errors.
*
*               Pointer to NULL,      otherwise.
*
* Caller(s)   : NetARP_CacheCfg().
*
* Note(s)     : (3) (a) ARP cache pool is accessed by 'NetARP_CachePoolPtr' during execution of
*
*                       (1) NetARP_Init()
*                       (2) NetARP_CacheGet()
*                       (3) NetARP_CacheFree()
*
*                   (b) Since the primary tasks of the network protocol suite are prevented from running 
*                       concurrently (see 'net.h  Note #2'), it is NOT necessary to protect the shared 
*                       resources of the ARP cache pool since no asynchronous access from other network
*                       tasks is possible.
*
*               (4) 'No cache available' case NOT possible during correct operation of  the ARP Cache.
*                   However, the 'else' case is included as an extra precaution in case the ARP Cache
*                   List is incorrectly modified &/or corrupted.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_ARP_CACHE  *NetARP_CacheGet (NET_ERR  *perr)
{
    NET_ARP_CACHE  *pcache;
    NET_ERR         stat_err;


                                                                /* ------------------ GET ARP CACHE ------------------- */
    if (NetARP_CachePoolPtr != (NET_ARP_CACHE *)0) {            /* If ARP cache pool NOT empty, get cache from pool.    */
        pcache               = (NET_ARP_CACHE *)NetARP_CachePoolPtr;
        NetARP_CachePoolPtr  = (NET_ARP_CACHE *)pcache->NextPtr;

    } else if (NetARP_CacheListTail != (NET_ARP_CACHE *)0) {    /* If ARP Cache List NOT empty, ...                     */
                                                                /* ... get ARP cache from list tail.                    */
        pcache               = (NET_ARP_CACHE *)NetARP_CacheListTail;
        NetARP_CacheRemove(pcache, DEF_YES);                    
        pcache               = (NET_ARP_CACHE *)NetARP_CachePoolPtr;
        NetARP_CachePoolPtr  = (NET_ARP_CACHE *)pcache->NextPtr;

    } else {                                                    /* Else none avail, rtn err (see Note #4).              */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNoneAvailCtr);
       *perr =   NET_ARP_ERR_CACHE_NONE_AVAIL;
        return ((NET_ARP_CACHE *)0);

    }


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE ARP CACHE ---------------- */
    if (pcache->Type != NET_ARP_TYPE_CACHE) {
        NetARP_CacheDiscard(pcache);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrInvalidTypeCtr);
       *perr =   NET_ARP_ERR_CACHE_INVALID_TYPE;
        return ((NET_ARP_CACHE *)0);
    }
#endif

                                                                /* ------------------ INIT ARP CACHE ------------------ */
    NetARP_CacheClr(pcache);
    DEF_BIT_SET(pcache->Flags, NET_ARP_FLAG_USED);              /* Set ARP cache as used.                               */

                                                                /* ----------- UPDATE ARP CACHE POOL STATS ------------ */
    NetStat_PoolEntryUsedInc(&NetARP_CachePoolStat, &stat_err);

   *perr =  NET_ARP_ERR_NONE;

    return (pcache);                                            /* ------------------ RTN ARP CACHE ------------------- */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetARP_CacheFree()
*
* Description : (1) Free an ARP cache :
*
*                   (a) Free   ARP cache timer
*                   (b) Free   ARP cache buffer queue
*                   (c) Clear  ARP cache controls
*                   (d) Free   ARP cache back to ARP cache pool
*                   (e) Update ARP cache pool statistics
*
*
* Argument(s) : pcache      Pointer to an ARP cache.
*               ------      Argument checked in NetARP_CacheCfg(),
*                                               NetARP_CacheRemove()
*                                                 by NetARP_CacheHandler(),
*                                                    NetARP_CacheGet(),
*                                                    NetARP_CacheReqTimeout(),
*                                                    NetARP_CacheTimeout().
*
*               tmr_free    Indicate whether to free network timer :
*
*                               DEF_YES                Free network timer for ARP cache.
*                               DEF_NO          Do NOT free network timer for ARP cache
*                                                     [Freed by  NetTmr_TaskHandler()
*                                                            via NetARP_CacheRemove()].
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_CacheCfg(),
*               NetARP_CacheRemove().
*
* Note(s)     : (2) #### To prevent freeing an ARP cache already freed via previous ARP cache free,
*                   NetARP_CacheFree() checks the ARP cache's 'USED' flag BEFORE freeing the cache.
*
*                   This prevention is only best-effort since any invalid duplicate ARP cache frees 
*                   MAY be asynchronous to potentially valid ARP cache gets.  Thus the invalid ARP 
*                   cache free(s) MAY corrupt the ARP cache's valid operation(s).
*
*                   However, since the primary tasks of the network protocol suite are prevented from
*                   running concurrently (see 'net.h  Note #2'), it is NOT necessary to protect ARP
*                   cache resources from possible corruption since no asynchronous access from other
*                   network tasks is possible.
*
*               (3) When an ARP cache in the 'PENDING' state is freed, it discards its transmit packet
*                   buffer queue.  The discard is performed by the network interface layer since it is
*                   the last layer to initiate transmission for these packet buffers.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetARP_CacheFree (NET_ARP_CACHE  *pcache,
                                CPU_BOOLEAN     tmr_free)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN   used;
#endif
    NET_CTR      *pctr;
    NET_ERR       err;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                    /* ---------------- VALIDATE TYPE ----------------- */
    if (pcache->Type != NET_ARP_TYPE_CACHE) {
        NetARP_CacheDiscard(pcache);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrInvalidTypeCtr);
        return;
    }
                                                                    /* ------------ VALIDATE ARP CACHE USED ----------- */
    used = DEF_BIT_IS_SET(pcache->Flags, NET_ARP_FLAG_USED);
    if (used != DEF_YES) {                                          /* If ARP cache NOT used, ...                       */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNotUsedCtr);
        return;                                                     /* ... rtn but do NOT free (see Note #2).           */
    }
#endif

                                                                    /* -------------- FREE ARP CACHE TMR -------------- */
    if (tmr_free == DEF_YES) {
        if (pcache->TmrPtr != (NET_TMR *)0) {
            NetTmr_Free(pcache->TmrPtr);
        }
    }

                                                                    /* ------------- FREE ARP CACHE BUF Q ------------- */
#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetIF_ErrTxPktDiscardedCtr;      /* See Note #3.                                     */
#else
    pctr = (NET_CTR *) 0;
#endif
    NetBuf_FreeBufQ_SecList((NET_BUF    *)pcache->BufQ_Head,
                            (NET_CTR    *)pctr,
                            (CPU_FNCT_PTR)NetARP_CacheUnlinkBuf);

                                                                    /* ----------------- CLR ARP CACHE ---------------- */
    pcache->State = NET_ARP_CACHE_STATE_FREE;                       /* Set ARP cache as freed/NOT used.                 */
    DEF_BIT_CLR(pcache->Flags, NET_ARP_FLAG_USED);
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    NetARP_CacheClr(pcache);
#endif    

                                                                    /* ---------------- FREE ARP CACHE ---------------- */
    pcache->NextPtr     = NetARP_CachePoolPtr;
    NetARP_CachePoolPtr = pcache;

                                                                    /* --------- UPDATE ARP CACHE POOL STATS ---------- */
    NetStat_PoolEntryUsedDec(&NetARP_CachePoolStat, &err);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetARP_CacheClr()
*
* Description : Clear ARP cache controls.
*
* Argument(s) : pcache      Pointer to an ARP cache.
*               ------      Argument validated in NetARP_Init(),
*                                                 NetARP_CacheGet(),
*                                                 NetARP_CacheFree().
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_Init(),
*               NetARP_CacheGet(),
*               NetARP_CacheFree().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetARP_CacheClr (NET_ARP_CACHE  *pcache)
{
    pcache->PrevPtr        = (NET_ARP_CACHE *)0;
    pcache->NextPtr        = (NET_ARP_CACHE *)0;
    pcache->BufQ_Head      = (NET_BUF       *)0;
    pcache->BufQ_Tail      = (NET_BUF       *)0;
    pcache->TmrPtr         = (NET_TMR       *)0;
    pcache->AccessedCtr    =  0;
    pcache->ReqAttemptsCtr =  0;
    pcache->State          =  NET_ARP_CACHE_STATE_FREE;
    pcache->Flags          =  NET_ARP_FLAG_NONE;

    Mem_Clr((void     *)&pcache->HW_Addr[0],
            (CPU_SIZE_T) NET_ARP_CFG_HW_ADDR_LEN);
    Mem_Clr((void     *)&pcache->ProtocolAddr[0],
            (CPU_SIZE_T) NET_ARP_CFG_PROTOCOL_ADDR_LEN);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetARP_CacheIsUsed()
*
* Description : Validate ARP cache in use.
*
* Argument(s) : pcache      Pointer to object to validate as an ARP cache in use.
*
* Return(s)   : DEF_YES, ARP cache   valid &      in use.
*
*               DEF_NO,  ARP cache invalid or NOT in use.
*
* Caller(s)   : various.
*
* Note(s)     : (1) NetARP_CacheIsUsed() MUST be called with the global network lock already acquired.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetARP_CacheIsUsed (NET_ARP_CACHE  *pcache)
{
    CPU_BOOLEAN  used;

                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (pcache == (NET_ARP_CACHE *)0) {
        return (DEF_NO);
    }
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    if (pcache->Type != NET_ARP_TYPE_CACHE) {
        return (DEF_NO);
    }

                                                                /* ------------- VALIDATE ARP CACHE USED -------------- */
    used = DEF_BIT_IS_SET(pcache->Flags, NET_ARP_FLAG_USED);

    return (used);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetARP_CacheDiscard()
*
* Description : (1) Discard an invalid/corrupted ARP cache :
*
*                   (a) Discard ARP cache from available cache pool                 See Note #2
*                   (b) Update  ARP cache pool statistics
*
*               (2) Assumes ARP cache is invalid/corrupt & MUST be removed.  ARP cache removed
*                   simply by NOT returning the ARP cache back to the ARP cache pool.
*
*
* Argument(s) : pcache      Pointer to an invalid/corrupted ARP cache.
*
* Return(s)   : none.
*
* Caller(s)   : NetARP_CacheGet(),
*               NetARP_CacheFree().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetARP_CacheDiscard (NET_ARP_CACHE  *pcache)
{
    NET_ERR  stat_err;

                                                                /* ---------------- DISCARD ARP CACHE ----------------- */
    if (pcache == (NET_ARP_CACHE *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetARP_ErrNullPtrCtr);
        return;
    }

                                                                /* --------------- UPDATE DISCARD STATS --------------- */
    NetStat_PoolEntryLostInc(&NetARP_CachePoolStat, &stat_err);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_arp.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of ARP module include (see Note #1).             */

