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
*                                         NETWORK SOURCE FILE
*
* Filename      : net.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_MODULE
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
*                                             Net_Init()
*
* Description : (1) Initialize & startup network protocol suite :
*
*                   (a) Initialize network protocol suite default values
*                   (b) Initialize network counters
*                   (c) Initialize network protocol suite global OS objects
*                   (d) Initialize network protocol suite modules
*                   (e) Signal ALL network protocol suite modules & tasks
*                           that network initialization is complete
*                   (f) Enable NIC interrupts
*
*
* Argument(s) : none.
*
* Return(s)   : NET_ERR_NONE,                                     if NO errors.
*
*               Specific initialization error code (see Note #4), otherwise.
*
* Caller(s)   : Your Product's Application.
*
*               This function is a network protocol suite initialization function & MAY be called by
*               application/initialization function(s).
*
* Note(s)     : (2) NetInit() MUST be called ...
*
*                   (a) ONLY ONCE from a product's application; ...
*                   (b) (1) AFTER  product's OS has been initialized
*                       (2) BEFORE product's application calls any network protocol suite function(s)
*
*               (3) The following initialization functions MUST be sequenced as follows :
*
*                   (a) Net_InitDflt()  MUST precede ALL other network initialization functions
*                   (b) NetOS_Init()    MUST precede remaining network initialization functions
*                   (c) NetDbg_Init()   MUST follow  NetTmr_Init()
*                   (d) NetNIC_Init()   MUST precede NetIF_Init()
*                   (e) NetNIC_IntEn()  MUST follow  ALL other network initialization
*
*               (4) (a) If any network initialization error occurs, any remaining network initialization
*                       is immediately aborted & the specific initialization error code is returned.
*
*                   (b) Network error codes are listed in 'net_err.h', organized by network modules &/or
*                       layers.  A search of the specific error code number(s) provides the corresponding
*                       error code label(s).  A search of the error code label(s) provides the source code
*                       location of the network initialization error(s).
*********************************************************************************************************
*/

NET_ERR  Net_Init (void)
{
    NET_ERR     err;
    CPU_INT08U  i;


    Net_InitDone = DEF_NO;                                      /* Block net fncts/tasks until init complete.           */


                                                                /* ---------------- INIT NET DFLT VALS ---------------- */
#if (NET_CFG_INIT_CFG_VALS == NET_INIT_CFG_VALS_DFLT)
    Net_InitDflt();                                             /* Init cfg vals to dflt vals (see Note #3a).           */
#endif

                                                                /* ------------------ INIT NET CTRS ------------------- */
#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
    Net_RxPktCtr     = 0;
    Net_TxSuspendCtr = 0;
#endif


/*$PAGE*/
                                                                /* --------------- PERFORM NET/OS INIT ---------------- */
    NetOS_Init(&err);                                           /* Create net obj(s) [see Note #3b].                    */
    if (err != NET_OS_ERR_NONE) {
        return (err);
    }

                                                                /* ----------------- INIT NET MODULES ----------------- */
                                                                /* Init net rsrc mgmt module(s).                        */
    NetCtr_Init();
    NetStat_Init();

    NetTmr_Init(&err);
    if (err != NET_TMR_ERR_NONE) {
        return (err);
    }

    NetBuf_Init();
#ifdef  NET_CONN_MODULE_PRESENT
    NetConn_Init();
#endif

    NetDbg_Init();                                              /* See Note #3c.                                        */

                                                                /* Init net NIC/IF layers.                              */
    NetNIC_Init(&err);                                          /* See Note #3d.                                        */
    if (err != NET_NIC_ERR_NONE) {
        return (err);
    }

    NetIF_Init(&err);
    if (err != NET_IF_ERR_NONE) {
        return (err);
    }

                                                                /* Init net layers.                                     */
    NetIP_Init();
    NetICMP_Init();

                                                                /* Init net transport layers.                           */
    NetUDP_Init();

#ifdef  NET_TCP_MODULE_PRESENT
    NetTCP_Init(&err);
    if (err != NET_TCP_ERR_NONE) {
        return (err);
    }
#endif

                                                                /* Init net app layers.                                 */
#ifdef  NET_SOCK_MODULE_PRESENT
    NetSock_Init(&err);
    if (err != NET_SOCK_ERR_NONE) {
        return (err);
    }
#endif


                                                                /* ------------- SIGNAL NET INIT COMPLETE ------------- */
    Net_InitDone = DEF_YES;                                     /* Signal net fncts/tasks that init complete.           */

    for (i = 0; i < NET_TASK_NBR; i++) {                        /* Signal ALL net   tasks that init complete.           */
        NetOS_InitSignal(&err);
        if (err != NET_OS_ERR_NONE) {
            Net_InitDone = DEF_NO;
            return (err);
        }
    }


                                                                /* ------------------- EN NIC INTS -------------------- */
    NetNIC_IntEn(&err);                                         /* See Note #3e.                                        */
    if (err != NET_NIC_ERR_NONE) {
        Net_InitDone = DEF_NO;
        return (err);
    }


    return (NET_ERR_NONE);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           Net_InitDflt()
*
* Description : Initialize default values for network protocol suite configurable parameters.
*
*               (1) Network protocol suite configurable parameters MUST be initialized PRIOR to all other
*                   network initialization.
*
*               (2) (a) When NET_CFG_INIT_CFG_VALS is configured as NET_INIT_CFG_VALS_DFLT, the network
*                       protocol suite's configurable parameters are configured with default values.
*
*                       The application need only call Net_Init() to initialize both the network protocol 
*                       suite & its configurable parameters.
*
*                   (b) When NET_CFG_INIT_CFG_VALS is configured as NET_INIT_CFG_VALS_APP_INIT, the 
*                       application MUST ...
*
*                       (1) Initialize ALL network protocol suite configurable parameters from values
*                           recalled from non-volatile memory &/or hard-coded application values :
*
*                           (A) Call each of the configuration functions in Net_InitDflt() passing the
*                               application-recalled &/or hard-coded values
*                                 OR
*                           (B) Call Net_InitDflt() to initialize ALL network protocol suite configurable
*                               parameters to their default values & then call any of the configuration
*                               functions in Net_InitDflt() passing the recalled &/or hard-coded values
*                               to initialize some of the network protocol suite configurable parameters
*
*                       (2) Call Net_Init()
*
*               See also 'net_cfg.h  NETWORK   CONFIGURATION  Note #1'
*                      & 'net_def.h  PARAMETER CONFIGURATION INITIALIZATION  Note #1'.
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*
*               This function is an INTERNAL network protocol suite function but MAY be called by 
*               application/initialization functions (see Note #2b1B).
*
* Note(s)     : (3) Ignores configuration functions' return value indicating configuration success/failure.
*********************************************************************************************************
*/
/*$PAGE*/
void  Net_InitDflt (void)
{

                                                                /* -------------- CFG DBG INIT DFLT VALS -------------- */
#if (NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED)

#if (NET_BUF_CFG_NBR_SMALL > 0)
   (void)NetDbg_CfgRsrcBufSmallThLo (NET_DBG_RSRC_TH_LO_DFLT,
                                     NET_DBG_RSRC_TH_LO_HYST_DFLT);
#endif
#if (NET_BUF_CFG_NBR_LARGE > 0)
   (void)NetDbg_CfgRsrcBufLargeThLo (NET_DBG_RSRC_TH_LO_DFLT,
                                     NET_DBG_RSRC_TH_LO_HYST_DFLT);
#endif
   (void)NetDbg_CfgRsrcTmrThLo      (NET_DBG_RSRC_TH_LO_DFLT,
                                     NET_DBG_RSRC_TH_LO_HYST_DFLT);
#ifdef  NET_CONN_MODULE_PRESENT
   (void)NetDbg_CfgRsrcConnThLo     (NET_DBG_RSRC_TH_LO_DFLT,
                                     NET_DBG_RSRC_TH_LO_HYST_DFLT);
#endif
#ifdef  NET_ARP_MODULE_PRESENT
   (void)NetDbg_CfgRsrcARP_CacheThLo(NET_DBG_RSRC_TH_LO_DFLT,
                                     NET_DBG_RSRC_TH_LO_HYST_DFLT);
#endif
#ifdef  NET_TCP_MODULE_PRESENT
   (void)NetDbg_CfgRsrcTCP_ConnThLo (NET_DBG_RSRC_TH_LO_DFLT,
                                     NET_DBG_RSRC_TH_LO_HYST_DFLT);
#endif
#ifdef  NET_SOCK_MODULE_PRESENT
   (void)NetDbg_CfgRsrcSockThLo     (NET_DBG_RSRC_TH_LO_DFLT,
                                     NET_DBG_RSRC_TH_LO_HYST_DFLT);
#endif
#endif

#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)
   (void)NetDbg_CfgMonTaskTime(NET_DBG_MON_TASK_TIME_DFLT_SEC);
#endif




#ifdef  NET_CONN_MODULE_PRESENT
                                                                /* ----------- CFG NET CONN INIT DFLT VALS ------------ */
   (void)NetConn_CfgAccessedTh(NET_CONN_ACCESSED_TH_DFLT);
#endif




#ifdef  NET_ARP_MODULE_PRESENT
                                                                /* -------------- CFG ARP INIT DFLT VALS -------------- */
   (void)NetARP_CfgCacheTimeout(NET_ARP_CACHE_TIMEOUT_DFLT_SEC);
   (void)NetARP_CfgCacheAccessedTh(NET_ARP_CACHE_ACCESSED_TH_DFLT);
   (void)NetARP_CfgReqMaxRetries(NET_ARP_REQ_RETRY_DFLT);
   (void)NetARP_CfgReqTimeout(NET_ARP_REQ_RETRY_TIMEOUT_DFLT_SEC);
#endif




                                                                /* -------------- CFG IP INIT DFLT VALS --------------- */
   (void)NetIP_CfgFragReasmTimeout(NET_IP_FRAG_REASM_TIMEOUT_DFLT_SEC);




                                                                /* ------------- CFG ICMP INIT DFLT VALS -------------- */
#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
   (void)NetICMP_CfgTxSrcQuenchTh(NET_ICMP_TX_SRC_QUENCH_TH_DFLT);
#endif




#ifdef  NET_TCP_MODULE_PRESENT
                                                                /* -------------- CFG TCP INIT DFLT VALS -------------- */
                                                                /* #### NOT yet implemented (remove if unnecessary).    */
#endif




#ifdef  NET_SOCK_MODULE_PRESENT
                                                                /* ------------- CFG SOCK INIT DFLT VALS -------------- */
                                                                /* #### NOT yet implemented (remove if unnecessary).    */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          Net_VersionGet()
*
* Description : Get network protocol suite software version.
*
* Argument(s) : none.
*
* Return(s)   : Network protocol suite software version.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16U  Net_VersionGet (void)
{
    CPU_INT16U  ver;


    ver = NET_VERSION;

    return (ver);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         Net_RxPktIsAvail()
*
* Description : Determine if any network receive packet(s) are available.
*
* Argument(s) : rx_chk_nbr      Number of consecutive times that network receive packet availability has
*                                   been checked (see Note #2b1).
*
* Return(s)   : DEF_YES, network receive packet(s)     available (see Note #2a1).
*
*               DEF_NO,  network receive packet(s) NOT available (see Note #2a2).
*
* Caller(s)   : NetTCP_TxConnTxQ().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) To balance network receive versus transmit packet loads for certain network connection
*                   types (e.g. stream-type connections), network receive & transmit packets SHOULD be 
*                   handled in an APPROXIMATELY balanced ratio.
*
*                   (a) Network task priorities & lock mechanisms partially maintain a balanced ratio 
*                       between network receive versus transmit packet handling.
*
*                       However, the handling of network receive & transmit packets :
*
*                       (1) SHOULD be interleaved so that for every few packet(s) received & handled, 
*                           several packet(s) should be transmitted; & vice versa.
*
*                       (2) SHOULD NOT exclusively handle receive nor transmit packets, even for a
*                           short period of time.
*
*                   (b) To implement network receive versus transmit load balancing :
*
*                       (1) The availability of network receive packets MUST be managed at the network 
*                           interface layer :
*
*                           (A) Increment the number of available network receive packets queued for
*                               each network  packet received.
*
*                           (B) Decrement the number of available network receive packets queued for
*                               each received packet handled.
*
*                   See also 'net.h  NETWORK RECEIVE PACKET MACRO'S  Note #1'.
*
*               (2) (a) To approximate a balanced ratio of network receive versus transmit packets
*                       handled; the availability of network receive packets returned is conditionally
*                       based on the consecutive number of times the availability is checked :
*
*                       (1) If the number of available network receive packets queued ('Net_RxPktCtr')
*                           is greater than the consecutive number of times the availability is checked
*                           ('rx_chk_nbr'), then the actual availability of network receive packet is
*                           returned.
*
*                       (2) Otherwise, no available network receive packets is returned -- even if
*                           network receive packets ARE available.
*
*                   (b) (1) The number of consecutive times that the network receive availability
*                           is checked ('rx_chk_nbr') SHOULD correspond to the consecutive number
*                           of times that a network transmit suspends itself to check for & handle
*                           any network receive packet(s).
*
*                       (2) (A) To check actual network receive packet availability,               
*                               call Net_RxPktIsAvail() with 'rx_chk_nbr' always    set to 0.
*
*                           (B) To check        network receive packet availability consecutively, 
*                               call Net_RxPktIsAvail() with 'rx_chk_nbr' initially set to 0 & 
*                               incremented by 1 for each consecutive call thereafter.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
CPU_BOOLEAN  Net_RxPktIsAvail (NET_CTR  rx_chk_nbr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR       cpu_sr;
#endif
    CPU_BOOLEAN  rx_pkt_avail;


    CPU_CRITICAL_ENTER();
    rx_pkt_avail = (Net_RxPktCtr > rx_chk_nbr) ? DEF_YES : DEF_NO;
    CPU_CRITICAL_EXIT();

    return (rx_pkt_avail);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                           Net_TxSuspend()
*
* Description : Suspend network transmit.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_TxConnTxQ().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) To balance network receive versus transmit packet loads for certain network connection
*                   types (e.g. stream-type connections), network receive & transmit packets SHOULD be 
*                   handled in an APPROXIMATELY balanced ratio.
*
*                   (a) Network task priorities & lock mechanisms partially maintain a balanced ratio 
*                       between network receive versus transmit packet handling.
*
*                       However, the handling of network receive & transmit packets :
*
*                       (1) SHOULD be interleaved so that for every few packet(s) received & handled, 
*                           several packet(s) should be transmitted; & vice versa.
*
*                       (2) SHOULD NOT exclusively handle receive nor transmit packets, even for a
*                           short period of time.
*
*                   (b) To implement network receive versus transmit load balancing :
*
*                       (2) Certain network connections MUST periodically suspend network transmit(s) 
*                           to handle network receive packet(s) :
*
*                           (A) Suspend network transmit(s) if network receive packets are available.
*
*                               (1) To approximate a balanced ratio of network receive versus transmit 
*                                   packets handled; the number of consecutive times that a network 
*                                   transmit suspends itself to check for & handle any network receive 
*                                   packet(s) SHOULD APPROXIMATELY correspond to the number of queued
*                                   network receive packet(s) availabile.
*
*                                   See also 'Net_RxPktIsAvail()  Note #2'.
*
*                               (2) To protect connections from transmit corruption while suspended,
*                                   ALL transmit operations for suspended connections MUST be blocked 
*                                   until the connection is no longer suspended.
*
*                   See also 'net.h  NETWORK RECEIVE PACKET MACRO'S  Note #1'.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
void  Net_TxSuspend (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


    CPU_CRITICAL_ENTER();
    if (Net_TxSuspendCtr < NET_CTR_MAX) {
        Net_TxSuspendCtr++;                                     /* Inc     net tx suspend ctr.                          */
    }
    CPU_CRITICAL_EXIT();

    NetOS_TxSuspendWait();                                      /* Wait on net tx suspend signal (see Note #1b2A).      */

    CPU_CRITICAL_ENTER();
    if (Net_TxSuspendCtr > 0) {
        Net_TxSuspendCtr--;                                     /* Dec     net tx suspend ctr.                          */
    }
    CPU_CRITICAL_EXIT();
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        Net_TxSuspendSignal()
*
* Description : Signal suspended network transmit(s).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_RxHandlerLoadBal().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) To balance network receive versus transmit packet loads for certain network connection
*                   types (e.g. stream-type connections), network receive & transmit packets SHOULD be 
*                   handled in an APPROXIMATELY balanced ratio.
*
*                   (a) Network task priorities & lock mechanisms partially maintain a balanced ratio 
*                       between network receive versus transmit packet handling.
*
*                       However, the handling of network receive & transmit packets :
*
*                       (1) SHOULD be interleaved so that for every few packet(s) received & handled, 
*                           several packet(s) should be transmitted; & vice versa.
*
*                       (2) SHOULD NOT exclusively handle receive nor transmit packets, even for a
*                           short period of time.
*
*                   (b) To implement network receive versus transmit load balancing :
*
*                       (2) Certain network connections MUST periodically suspend network transmit(s) 
*                           to handle network receive packet(s) :
*
*                           (B) Signal or timeout network transmit suspend(s) to restart transmit(s).
*
*                   See also 'net.h  NETWORK RECEIVE PACKET MACRO'S  Note #1'.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
void  Net_TxSuspendSignal (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR   cpu_sr;
#endif
    NET_CTR  nbr_net_tx_suspend;
    NET_CTR  i;


    CPU_CRITICAL_ENTER();
    nbr_net_tx_suspend = Net_TxSuspendCtr;
    CPU_CRITICAL_EXIT();

    for (i = 0; i < nbr_net_tx_suspend; i++) {
        NetOS_TxSuspendSignal();                                /* Signal ALL suspended net tx's (see Note #1b2B).      */
    }
}
#endif

