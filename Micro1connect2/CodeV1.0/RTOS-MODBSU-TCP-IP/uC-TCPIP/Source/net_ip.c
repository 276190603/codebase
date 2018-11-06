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
*                                          NETWORK IP LAYER
*                                         (INTERNET PROTOCOL)
*
* Filename      : net_ip.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports Internet Protocol as described in RFC #791, also known as IPv4, with the
*                     following restrictions/constraints :
*
*                     (a) ONLY supports a single network interface/             RFC # 791, Section 2.3
*                            host IP address                                                'Addressing'
*
*                     (b) ONLY supports a single default gateway                RFC #1122, Section 3.3.1
*
*                     (c) IP forwarding/routing  NOT           supported        RFC #1122, Sections 3.3.1,
*                                                                                           3.3.4 & 3.3.5
*
*                     (d) IP multicasting        NOT currently supported        RFC #1122, Section 3.3.7
*
*                     (e) Transmit fragmentation NOT currently supported        RFC # 791, Section 2.3
*                                                                                           'Fragmentation &
*                                                                                              Reassembly'
*                     (f) IP Security options    NOT           supported        RFC #1108
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_IP_MODULE
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

                                                                            /* -------- VALIDATE RX DATAGRAMS --------- */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void          NetIP_RxPktValidateBuf        (NET_BUF_HDR   *pbuf_hdr,
                                                     NET_ERR       *perr);
#endif

static  void          NetIP_RxPktValidate           (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     NET_IP_HDR    *pip_hdr,
                                                     NET_ERR       *perr);

static  void          NetIP_RxPktValidateOpt        (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     NET_IP_HDR    *pip_hdr,
                                                     CPU_INT08U     ip_hdr_len_size,
                                                     NET_ERR       *perr);

static  CPU_BOOLEAN   NetIP_RxPktValidateOptRoute   (CPU_INT08U    *popts,
                                                     CPU_INT08U     opt_list_len_rem,
                                                     CPU_INT08U    *popt_len,
                                                     NET_ERR       *perr);

static  CPU_BOOLEAN   NetIP_RxPktValidateOptTS      (CPU_INT08U    *popts,
                                                     CPU_INT08U     opt_list_len_rem,
                                                     CPU_INT08U    *popt_len,
                                                     NET_ERR       *perr);


                                                                            /* ------------ REASM RX FRAGS ------------ */

static  NET_BUF      *NetIP_RxPktFragReasm          (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     NET_IP_HDR    *pip_hdr,
                                                     NET_ERR       *perr);

static  NET_BUF      *NetIP_RxPktFragListAdd        (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     CPU_INT16U     frag_ip_flags,
                                                     CPU_INT16U     frag_offset,
                                                     CPU_INT16U     frag_size,
                                                     NET_ERR       *perr);

static  NET_BUF      *NetIP_RxPktFragListInsert     (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     CPU_INT16U     frag_ip_flags,
                                                     CPU_INT16U     frag_offset,
                                                     CPU_INT16U     frag_size,
                                                     NET_BUF       *pfrag_list,
                                                     NET_ERR       *perr);

static  void          NetIP_RxPktFragListRemove     (NET_BUF       *pfrag_list,
                                                     CPU_BOOLEAN    tmr_free);

static  void          NetIP_RxPktFragListDiscard    (NET_BUF       *pfrag_list,
                                                     CPU_BOOLEAN    tmr_free,
                                                     NET_ERR       *perr);

static  void          NetIP_RxPktFragListUpdate     (NET_BUF       *pfrag_list,
                                                     NET_BUF_HDR   *pfrag_list_buf_hdr,
                                                     CPU_INT16U     frag_ip_flags,
                                                     CPU_INT16U     frag_offset,
                                                     CPU_INT16U     frag_size,
                                                     NET_ERR       *perr);

static  NET_BUF      *NetIP_RxPktFragListChkComplete(NET_BUF       *pfrag_list,
                                                     NET_BUF_HDR   *pfrag_list_buf_hdr,
                                                     NET_ERR       *perr);

static  void          NetIP_RxPktFragTimeout        (void          *pfrag_list_timeout);


                                                                            /* ---------- DEMUX RX DATAGRAMS ---------- */

static  void          NetIP_RxPktDemuxDatagram      (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     NET_ERR       *perr);



static  void          NetIP_RxPktDiscard            (NET_BUF       *pbuf,
                                                     NET_ERR       *perr);



/*$PAGE*/
                                                                            /* ----------- VALIDATE TX PKTS ----------- */
#if ((NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED))
static  void          NetIP_TxPktValidate           (NET_BUF_HDR   *pbuf_hdr,
                                                     NET_IP_ADDR    addr_src,
                                                     NET_IP_ADDR    addr_dest,
                                                     NET_IP_TOS     TOS,
                                                     NET_IP_TTL     TTL,
                                                     CPU_INT16U     flags,
                                                     void          *popts,
                                                     NET_ERR       *perr);

static  void          NetIP_TxPktValidateOpt        (void          *popts,
                                                     NET_ERR       *perr);

static  void          NetIP_TxPktValidateOptRouteTS (void          *popt_route_ts,
                                                     CPU_INT08U    *popt_len,
                                                     void         **popt_next,
                                                     NET_ERR       *perr);
#endif

                                                                            /* -------------- TX IP PKTS -------------- */

static  void          NetIP_TxPkt                   (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     NET_IP_ADDR    addr_src,
                                                     NET_IP_ADDR    addr_dest,
                                                     NET_IP_TOS     TOS,
                                                     NET_IP_TTL     TTL,
                                                     CPU_INT16U     flags,
                                                     void          *popts,
                                                     NET_ERR       *perr);

static  CPU_INT08U    NetIP_TxPktPrepareOpt         (void          *popts,
                                                     CPU_INT08U    *popt_hdr,
                                                     NET_ERR       *perr);

static  void          NetIP_TxPktPrepareOptRoute    (void          *popts,
                                                     CPU_INT08U    *popt_hdr,
                                                     CPU_INT08U    *popt_len,
                                                     void         **popt_next,
                                                     NET_ERR       *perr);

static  void          NetIP_TxPktPrepareOptTS       (void          *popts,
                                                     CPU_INT08U    *popt_hdr,
                                                     CPU_INT08U    *popt_len,
                                                     void         **popt_next,
                                                     NET_ERR       *perr);

static  void          NetIP_TxPktPrepareOptTSRoute  (void          *popts,
                                                     CPU_INT08U    *popt_hdr,
                                                     CPU_INT08U    *popt_len,
                                                     void         **popt_next,
                                                     NET_ERR       *perr);

static  void          NetIP_TxPktPrepareHdr         (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     CPU_INT16U     ip_hdr_len_tot,
                                                     CPU_INT08U     ip_opt_len_tot,
                                                     CPU_INT16U     protocol_ix,
                                                     NET_IP_ADDR    addr_src,
                                                     NET_IP_ADDR    addr_dest,
                                                     NET_IP_TOS     TOS,
                                                     NET_IP_TTL     TTL,
                                                     CPU_INT16U     flags,
                                                     CPU_INT32U    *pip_hdr_opts,
                                                     NET_ERR       *perr);

/*$PAGE*/
                                                                            /* ------------ TX IP DATAGRAMS ----------- */

static  void          NetIP_TxPktDatagram           (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     NET_ERR       *perr);

static  void          NetIP_TxPktDatagramRouteSel   (NET_BUF_HDR   *pbuf_hdr,
                                                     NET_ERR       *perr);



static  void          NetIP_TxPktDiscard            (NET_BUF       *pbuf,
                                                     NET_ERR       *perr);


                                                                            /* ------------ RE-TX IP PKTS ------------- */

static  void          NetIP_ReTxPkt                 (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     NET_ERR       *perr);

static  void          NetIP_ReTxPktPrepareHdr       (NET_BUF       *pbuf,
                                                     NET_BUF_HDR   *pbuf_hdr,
                                                     NET_ERR       *perr);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetIP_Init()
*
* Description : (1) Initialize Internet Protocol Layer :
*
*                   (a) Initialize IP addresses
*                   (b) Initialize IP fragmentation list pointers
*                   (c) Initialize IP identification (ID) counter
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
* Note(s)     : (2) Default IP address initialization is invalid & forces the developer or higher-layer
*                   protocol application to configure valid IP address(s).
*
*                   See also 'NetIP_CfgAddrThisHost()     Note #1'
*                          & 'NetIP_CfgAddrDfltGateway()  Note #1'.
*********************************************************************************************************
*/

void  NetIP_Init (void)
{
                                                                /* ------------------ INIT IP ADDRS ------------------- */
    NetIP_AddrThisHost               = (NET_IP_ADDR)NET_IP_ADDR_NONE;
    NetIP_AddrThisHostNetOrder       = (NET_IP_ADDR)NET_UTIL_HOST_TO_NET_32(NET_IP_ADDR_NONE);
    NetIP_AddrThisHostSubnetMask     = (NET_IP_ADDR)NET_IP_ADDR_NONE;
    NetIP_AddrThisHostSubnetMaskHost = (NET_IP_ADDR)NET_IP_ADDR_NONE;
    NetIP_AddrThisHostSubnetNet      = (NET_IP_ADDR)NET_IP_ADDR_NONE;
    NetIP_AddrDfltGateway            = (NET_IP_ADDR)NET_IP_ADDR_NONE;

                                                                /* ---------------- INIT IP FRAG LISTS ---------------- */
    NetIP_FragReasmListsHead         = (NET_BUF *)0;
    NetIP_FragReasmListsTail         = (NET_BUF *)0;

                                                                /* ------------------ INIT IP ID CTR ------------------ */
    NetIP_TxID_Ctr                   =  NET_IP_ID_INIT;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_CfgAddrThisHost()
*
* Description : (1) Configure this host's IP address :
*
*                   (a) Acquire   network lock
*                   (b) Validate  this host's IP address & subnet mask
*                   (c) Configure this host's IP address,  subnet mask, & subnet
*                   (d) Release   network lock
*
*
* Argument(s) : addr_host           Desired IP address             for this host (see Note #3).
*
*               addr_subnet_mask    Desired IP address subnet mask for this host (see Note #3).
*
* Return(s)   : DEF_OK,   if valid host IP address & subnet mask configured.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #2].
*
* Note(s)     : (2) NetIP_CfgAddrThisHost() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global 
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*               (3) Host IP addresses MUST be in host-order.
*
*               (4) 'NetIP_AddrThisHost' variables MUST ALWAYS be accessed with the global network lock 
*                    already acquired (see Note #2b).
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_CfgAddrThisHost (NET_IP_ADDR  addr_host,
                                    NET_IP_ADDR  addr_subnet_mask)
{
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    CPU_BOOLEAN  addr_valid;
#endif
    NET_ERR      err;

                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(&err);                                           /* See Note #2b.                                        */
    if (err !=  NET_OS_ERR_NONE) {
        return (DEF_FAIL);
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ----------------- VALIDATE IP ADDR ----------------- */
    addr_valid = NetIP_IsValidAddrThisHost(addr_host, addr_subnet_mask);
    if (addr_valid != DEF_YES) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrCfgAddrHostCfgdCtr);
        NetOS_Unlock();
        return (DEF_FAIL);
    }
#endif

                                                                /* ------------------- CFG IP ADDRS ------------------- */
    NetIP_AddrThisHost               =  addr_host;
    NetIP_AddrThisHostNetOrder       = (NET_IP_ADDR) NET_UTIL_HOST_TO_NET_32(addr_host);
    NetIP_AddrThisHostSubnetMask     = (NET_IP_ADDR) addr_subnet_mask;
    NetIP_AddrThisHostSubnetMaskHost = (NET_IP_ADDR)~addr_subnet_mask;
    NetIP_AddrThisHostSubnetNet      =  addr_host &  addr_subnet_mask;

                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


    return (DEF_OK);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetIP_CfgAddrDfltGateway()
*
* Description : (1) Configure this host's default gateway IP address :
*
*                   (a) Acquire   network lock
*                   (b) Validate  this host's default gateway IP address
*                   (c) Configure this host's default gateway IP address
*                   (d) Release   network lock
*
*
* Argument(s) : addr_dflt_gateway   Desired IP address for this host's default gateway (see Note #3).
*
* Return(s)   : DEF_OK,   if valid default gateway IP address configured.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #2].
*
* Note(s)     : (2) NetIP_CfgAddrDfltGateway() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global 
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*               (3) Default gateway address MUST be in host-order.
*
*               (4) (a) RFC #1122, Section 3.3.1.1 states that "the host IP layer MUST operate correctly
*                       in a minimal network environment, and in particular, when there are no gateways".
*
*                       In other words, a host on an isolated network should be able to correctly operate
*                       & communicate with all other hosts on its local network without need of a gateway
*                       or configuration of a gateway.
*
*                   (b) However, if this host's network interface is a network type (versus a serial 
*                       connection) AND the default gateway is configured with an invalid IP address 
*                       NOT on the local network, then ALL non-local-network destination IP datagrams 
*                       will be discarded at the physical link-layer/network interface.
*
*               (5) 'NetIP_AddrDfltGateway' variables MUST ALWAYS be accessed with the global network 
*                    lock already acquired (see Note #2b).
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_CfgAddrDfltGateway (NET_IP_ADDR  addr_dflt_gateway)
{
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    CPU_BOOLEAN  addr_valid;
#endif
    NET_ERR      err;

                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(&err);                                           /* See Note #2b.                                        */
    if (err !=  NET_OS_ERR_NONE) {
        return (DEF_FAIL);
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------- VALIDATE DFLT GATEWAY IP ADDR ----------- */
    addr_valid = NetIP_IsValidAddrHost(addr_dflt_gateway);
    if (addr_valid != DEF_YES) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrCfgAddrDfltGatewayCtr);
        NetOS_Unlock();
        return (DEF_FAIL);
    }
#endif

                                                                /* ------------- CFG DFLT GATEWAY IP ADDR ------------- */
    NetIP_AddrDfltGateway = addr_dflt_gateway;

                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


    return (DEF_OK);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetIP_CfgFragReasmTimeout()
*
* Description : Configure IP fragment reassembly timeout.
*
*               (1) IP fragment reassembly timeout is the maximum time allowed between received IP fragments
*                   from the same IP datagram.
*
*
* Argument(s) : timeout_sec     Desired value for IP fragment reassembly timeout (in seconds).
*
* Return(s)   : DEF_OK,   IP fragment reassembly timeout configured.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) Timeout in seconds converted to 'NET_TMR_TICK' ticks in order to pre-compute initial 
*                   timeout value in 'NET_TMR_TICK' ticks.
*
*               (3) 'NetIP_FragReasmTimeout' variables MUST ALWAYS be accessed exclusively in critical 
*                    sections.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_CfgFragReasmTimeout (CPU_INT08U  timeout_sec)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR        cpu_sr;
#endif
    NET_TMR_TICK  timeout_tick;


    if (timeout_sec < NET_IP_FRAG_REASM_TIMEOUT_MIN_SEC) {
        return (DEF_FAIL);
    }
    if (timeout_sec > NET_IP_FRAG_REASM_TIMEOUT_MAX_SEC) {
        return (DEF_FAIL);
    }

    timeout_tick                = (NET_TMR_TICK)timeout_sec * NET_TMR_TIME_TICK_PER_SEC;
    CPU_CRITICAL_ENTER();
    NetIP_FragReasmTimeout_sec  =  timeout_sec;
    NetIP_FragReasmTimeout_tick =  timeout_tick;
    CPU_CRITICAL_EXIT();

    return (DEF_OK);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_GetAddrThisHost()
*
* Description : Get this host's configured IP address.
*
* Argument(s) : none.
*
* Return(s)   : This host's IP address (see Note #2), if NO errors.
*
*               NET_IP_ADDR_NONE,                     otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetIP_GetAddrThisHost() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global 
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*                   See also 'NetIP_GetAddrThisHostHandler()  Note #2'.
*
*               (2) IP address returned in host-order.
*********************************************************************************************************
*/

NET_IP_ADDR  NetIP_GetAddrThisHost (void)
{
    NET_IP_ADDR  addr;
    NET_ERR      err;


    NetOS_Lock(&err);                                           /* Acquire net lock (see Note #1b).                     */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_IP_ADDR_NONE);
    }

    addr = NetIP_GetAddrThisHostHandler(&err);                  /* Get this host's IP addr.                             */
   (void)&err;                                                  /* Ignore invalid  IP addr err; rtn addr NONE.          */

    NetOS_Unlock();                                             /* Release net lock.                                    */

    return (addr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetIP_GetAddrThisHostHandler()
*
* Description : Get this host's IP address.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                     IP host address successfully returned.
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host address.
*
* Return(s)   : This host's IP address (see Note #1), if NO errors.
*
*               NET_IP_ADDR_NONE,                     otherwise.
*
* Caller(s)   : NetIP_GetAddrThisHost(),
*               NetSock_BindHandler(),
*               NetSock_TxDataHandlerDatagram().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (1) IP address returned in host-order.
*
*               (2) NetIP_GetAddrThisHostHandler() is called by network protocol suite function(s) & MUST 
*                   be called with the global network lock already acquired.
*
*                   See also 'NetIP_GetAddrThisHost()  Note #1'.
*
*               (3) 'NetIP_AddrThisHost' MUST ALWAYS be accessed with the global network lock already 
*                    acquired (see Note #2).
*********************************************************************************************************
*/

NET_IP_ADDR  NetIP_GetAddrThisHostHandler (NET_ERR  *perr)
{
    NET_IP_ADDR  addr;


    addr =  NetIP_AddrThisHost;                                 /* Get this host's IP addr.                             */
   *perr = (addr == NET_IP_ADDR_NONE) ? NET_IP_ERR_INVALID_ADDR_HOST
                                      : NET_IP_ERR_NONE;

    return (addr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIP_GetAddrSubnetMask()
*
* Description : Get this host's IP address subnet mask.
*
* Argument(s) : none.
*
* Return(s)   : This host's IP address subnet mask (see Note #2), if NO errors.
*
*               NET_IP_ADDR_NONE,                                 otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetIP_GetAddrSubnetMask() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global 
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*               (2) IP address subnet mask returned in host-order.
*
*               (3) 'NetIP_AddrThisHostSubnetMask' MUST ALWAYS be accessed with the global network lock 
*                    already acquired (see Note #1b).
*********************************************************************************************************
*/

NET_IP_ADDR  NetIP_GetAddrSubnetMask (void)
{
    NET_IP_ADDR  addr;
    NET_ERR      err;


    NetOS_Lock(&err);                                           /* Acquire net lock (see Note #1b).                     */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_IP_ADDR_NONE);
    }

    addr = NetIP_AddrThisHostSubnetMask;                        /* Get this host's IP addr subnet mask.                 */

    NetOS_Unlock();                                             /* Release net lock.                                    */

    return (addr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetIP_GetAddrDfltGateway()
*
* Description : Get this host's default gateway IP address.
*
* Argument(s) : none.
*
* Return(s)   : This host's default gateway IP address (see Note #2), if NO errors.
*
*               NET_IP_ADDR_NONE,                                     otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetIP_GetAddrDfltGateway() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global 
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*               (2) IP address returned in host-order.
*
*               (3) 'NetIP_AddrDfltGateway' MUST ALWAYS be accessed with the global network lock already 
*                    acquired (see Note #1b).
*********************************************************************************************************
*/

NET_IP_ADDR  NetIP_GetAddrDfltGateway (void)
{
    NET_IP_ADDR  addr;
    NET_ERR      err;


    NetOS_Lock(&err);                                           /* Acquire net lock (see Note #1b).                     */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_IP_ADDR_NONE);
    }

    addr = NetIP_AddrDfltGateway;                               /* Get this host's dflt gateway IP addr.                */

    NetOS_Unlock();                                             /* Release net lock.                                    */

    return (addr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetIP_GetAddrStrThisHost()
*
* Description : Get this host's IP address as an ASCII string.
*
* Argument(s) : paddr       Pointer to an ASCII character array that will receive the return IP address 
*                               ASCII string from this function (see Note #2).
*
* Return(s)   : Pointer to this host's IP address string, if NO errors.
*
*               Pointer to NULL,                          otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetIP_GetAddrStrThisHost() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired.
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*                   See also 'NetIP_GetAddrThisHost()  Note #1'.
*
*               (2) The size of the ASCII character array that will receive the return IP address ASCII 
*                   string MUST be greater than or equal to NET_ASCII_LEN_MAX_ADDR_IP.
*
*                   See also 'net_ascii.c  NetASCII_IP_to_Str()  Note #2'.
*********************************************************************************************************
*/

CPU_CHAR  *NetIP_GetAddrStrThisHost (CPU_CHAR  *paddr)
{
    NET_IP_ADDR  addr;
    NET_ERR      err;


    addr = NetIP_GetAddrThisHost();                             /* Get this host's IP addr.                             */

    NetASCII_IP_to_Str(addr, paddr, DEF_NO, &err);              /* Conv IP addr to IP addr str.                         */
    if (err  !=  NET_ASCII_ERR_NONE) {                          /* On any str err,     ...                              */
       *paddr = (CPU_CHAR  )0;                                  /* ...   rtn NULL addr ...                              */
        return ((CPU_CHAR *)0);                                 /* ... & rtn NULL.                                      */
    }

    return (paddr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetIP_GetAddrStrSubnetMask()
*
* Description : Get this host's IP address subnet mask as an ASCII string.
*
* Argument(s) : paddr       Pointer to an ASCII character array that will receive the return IP address 
*                               ASCII string from this function (see Note #2).
*
* Return(s)   : Pointer to this host's IP address subnet mask string, if NO errors.
*
*               Pointer to NULL,                                      otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetIP_GetAddrStrSubnetMask() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired.
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*                   See also 'NetIP_GetAddrSubnetMask()  Note #1'.
*
*               (2) The size of the ASCII character array that will receive the return IP address ASCII 
*                   string MUST be greater than or equal to NET_ASCII_LEN_MAX_ADDR_IP.
*
*                   See also 'net_ascii.c  NetASCII_IP_to_Str()  Note #2'.
*********************************************************************************************************
*/

CPU_CHAR  *NetIP_GetAddrStrSubnetMask (CPU_CHAR  *paddr)
{
    NET_IP_ADDR  addr;
    NET_ERR      err;


    addr = NetIP_GetAddrSubnetMask();                           /* Get this host's IP addr subnet mask.                 */

    NetASCII_IP_to_Str(addr, paddr, DEF_NO, &err);              /* Conv IP addr to IP addr str.                         */
    if (err  !=  NET_ASCII_ERR_NONE) {                          /* On any str err,     ...                              */
       *paddr = (CPU_CHAR  )0;                                  /* ...   rtn NULL addr ...                              */
        return ((CPU_CHAR *)0);                                 /* ... & rtn NULL.                                      */
    }

    return (paddr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetIP_GetAddrStrDfltGateway()
*
* Description : Get this host's default gateway IP address as an ASCII string.
*
* Argument(s) : paddr       Pointer to an ASCII character array that will receive the return IP address 
*                               ASCII string from this function (see Note #2).
*
* Return(s)   : Pointer to this host's default gateway IP address string, if NO errors.
*
*               Pointer to NULL,                                          otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetIP_GetAddrStrDfltGateway() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired.
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*                   See also 'NetIP_GetAddrDfltGateway()  Note #1'.
*
*               (2) The size of the ASCII character array that will receive the return IP address ASCII 
*                   string MUST be greater than or equal to NET_ASCII_LEN_MAX_ADDR_IP.
*
*                   See also 'net_ascii.c  NetASCII_IP_to_Str()  Note #2'.
*********************************************************************************************************
*/

CPU_CHAR  *NetIP_GetAddrStrDfltGateway (CPU_CHAR  *paddr)
{
    NET_IP_ADDR  addr;
    NET_ERR      err;


    addr = NetIP_GetAddrDfltGateway();                          /* Get this host's dflt gateway IP addr.                */

    NetASCII_IP_to_Str(addr, paddr, DEF_NO, &err);              /* Conv IP addr to IP addr str.                         */
    if (err  !=  NET_ASCII_ERR_NONE) {                          /* On any str err,     ...                              */
       *paddr = (CPU_CHAR  )0;                                  /* ...   rtn NULL addr ...                              */
        return ((CPU_CHAR *)0);                                 /* ... & rtn NULL.                                      */
    }

    return (paddr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIP_IsAddrClassA()
*
* Description : Validate an IP address as a Class-A IP address.
*
*               (1) RFC #791, Section 3.2 'Addressing : Address Format' specifies IP Class-A addresses as :
*              
*                       Class  High Order Bits   
*                       -----  ---------------   
*                        (A)         0
*
*
* Argument(s) : addr        IP address to validate (see Note #2).
*
* Return(s)   : DEF_YES, if IP address is a Class-A IP address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrClassA (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_class_a;


    addr_class_a = ((addr & NET_IP_ADDR_CLASS_A_MASK) == NET_IP_ADDR_CLASS_A) ? DEF_YES : DEF_NO;

    return (addr_class_a);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIP_IsAddrClassB()
*
* Description : Validate an IP address as a Class-B IP address.
*
*               (1) RFC #791, Section 3.2 'Addressing : Address Format' specifies IP Class-B addresses as :
*              
*                       Class  High Order Bits   
*                       -----  ---------------   
*                        (B)         10
*
*
* Argument(s) : addr        IP address to validate (see Note #2).
*
* Return(s)   : DEF_YES, if IP address is a Class-B IP address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrClassB (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_class_b;


    addr_class_b = ((addr & NET_IP_ADDR_CLASS_B_MASK) == NET_IP_ADDR_CLASS_B) ? DEF_YES : DEF_NO;

    return (addr_class_b);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIP_IsAddrClassC()
*
* Description : Validate an IP address as a Class-C IP address.
*
*               (1) RFC #791, Section 3.2 'Addressing : Address Format' specifies IP Class-C addresses as :
*              
*                       Class  High Order Bits   
*                       -----  ---------------   
*                        (C)         110
*
*
* Argument(s) : addr        IP address to validate (see Note #2).
*
* Return(s)   : DEF_YES, if IP address is a Class-C IP address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrClassC (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_class_c;


    addr_class_c = ((addr & NET_IP_ADDR_CLASS_C_MASK) == NET_IP_ADDR_CLASS_C) ? DEF_YES : DEF_NO;

    return (addr_class_c);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_IsAddrThisHost()
*
* Description : Validate an IP address as a 'This Host' initialization IP address.
*
*               (1) RFC #1122, Section 3.2.1.3.(a) specifies the IP 'This Host' initialization address as :
*              
*                       0.0.0.0
*
*
* Argument(s) : addr        IP address to validate (see Note #2).
*
* Return(s)   : DEF_YES, if IP address is a 'This Host' initialization IP address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrThisHost (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_this_host;


    addr_this_host = (addr == NET_IP_ADDR_THIS_HOST) ? DEF_YES : DEF_NO;

    return (addr_this_host);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_IsAddrLocalHost()
*
* Description : Validate an IP address as a 'Localhost' IP address.
*
*               (1) RFC #1122, Section 3.2.1.3.(g) specifies the IP 'Localhost' address as :
*              
*                       127.<host>
*
*
* Argument(s) : addr        IP address to validate (see Note #2).
*
* Return(s)   : DEF_YES, if IP address is a 'Localhost' IP address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrLocalHost (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_local_host;


    addr_local_host = ((addr >= NET_IP_ADDR_LOCAL_HOST_MIN) &&
                       (addr <= NET_IP_ADDR_LOCAL_HOST_MAX)) ? DEF_YES : DEF_NO;

    return (addr_local_host);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_IsAddrLocalLink()
*
* Description : Validate an IP address as a link-local IP address.
*
*               (1) RFC #3927, Section 2.1 specifies the "IPv4 Link-Local address ... range ... [as]
*                   inclusive" ... :
*
*                   (a) "from 169.254.1.0" ...
*                   (b) "to   169.254.254.255".
*
*
* Argument(s) : addr        IP address to validate (see Note #2).
*
* Return(s)   : DEF_YES, if IP address is a link-local IP address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrLocalLink (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_local_link;


    addr_local_link = ((addr >= NET_IP_ADDR_LOCAL_LINK_HOST_MIN) &&
                       (addr <= NET_IP_ADDR_LOCAL_LINK_HOST_MAX)) ? DEF_YES : DEF_NO;

    return (addr_local_link);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_IsAddrBroadcast()
*
* Description : Validate an IP address as a limited broadcast IP address.
*
*               (1) RFC #1122, Section 3.2.1.3.(c) specifies the IP limited broadcast address as :
*              
*                       255.255.255.255
*
*
* Argument(s) : addr        IP address to validate (see Note #2).
*
* Return(s)   : DEF_YES, if IP address is a limited broadcast IP address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrBroadcast (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_broadcast;


    addr_broadcast = (addr == NET_IP_ADDR_BROADCAST) ? DEF_YES : DEF_NO;

    return (addr_broadcast);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetIP_IsAddrHost()
*
* Description : (1) Validate an IP address as an IP host address :
*
*                   (a) This host's   IP address
*                   (b) A 'Localhost' IP address
*
*
* Argument(s) : addr        IP address to validate (see Note #3).
*
* Return(s)   : DEF_YES, if IP address is one of the host's IP addresses.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #2].
*
* Note(s)     : (2) NetIP_IsAddrHost() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global 
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*                   See also 'NetIP_IsAddrHostHandler()  Note #2'.
*
*               (3) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrHost (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_host;
    NET_ERR      err;


    NetOS_Lock(&err);                                           /* Acquire net lock (see Note #2b).                     */
    if (err !=  NET_OS_ERR_NONE) {
        return (DEF_NO);
    }

    addr_host = NetIP_IsAddrHostHandler(addr);

    NetOS_Unlock();                                             /* Release net lock.                                    */

    return (addr_host);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIP_IsAddrHostHandler()
*
* Description : (1) Validate an IP address as an IP host address :
*
*                   (a) This host's   IP address
*                   (b) A 'Localhost' IP address
*
*
* Argument(s) : addr        IP address to validate (see Note #3).
*
* Return(s)   : DEF_YES, if IP address is one of the host's IP addresses.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetIP_IsAddrHost(),
*               NetICMP_TxMsgReq(),
*               NetSock_IsValidAddrLocal().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (2) NetIP_IsAddrHostHandler() is called by network protocol suite function(s) & MUST 
*                   be called with the global network lock already acquired.
*
*                   See also 'NetIP_IsAddrHost()  Note #2'.
*
*               (3) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrHostHandler (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_host;
    CPU_BOOLEAN  addr_host_cfgd;
    CPU_BOOLEAN  addr_local_host;


    addr_host_cfgd  = NetIP_IsAddrHostCfgdHandler(addr);
    addr_local_host = NetIP_IsAddrLocalHost(addr);

    addr_host       = ((addr_host_cfgd  == DEF_YES) ||
                       (addr_local_host == DEF_YES)) ? DEF_YES : DEF_NO;

    return (addr_host);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_IsAddrHostCfgd()
*
* Description : Validate an IP address as this host's configured IP address.
*
* Argument(s) : addr        IP address to validate (see Note #2).
*
* Return(s)   : DEF_YES, if IP address is this host's configured IP address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetIP_IsAddrHostCfgd() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global 
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*                   See also 'NetIP_IsAddrHostCfgdHandler()  Note #1'.
*
*               (2) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrHostCfgd (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_host;
    NET_ERR      err;


    NetOS_Lock(&err);                                           /* Acquire net lock (see Note #1b).                     */
    if (err !=  NET_OS_ERR_NONE) {
        return (DEF_NO);
    }

    addr_host = NetIP_IsAddrHostCfgdHandler(addr);

    NetOS_Unlock();                                             /* Release net lock.                                    */

    return (addr_host);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetIP_IsAddrHostCfgdHandler()
*
* Description : Validate an IP address as this host's configured IP address.
*
* Argument(s) : addr        IP address to validate (see Note #2).
*
* Return(s)   : DEF_YES, if IP address is this host's configured IP address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetIP_IsAddrHostCfgd(),
*               NetIP_IsAddrHostHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s) [see also Note #1].
*
* Note(s)     : (1) NetIP_IsAddrHostCfgdHandler() is called by network protocol suite function(s) & 
*                   MUST be called with the global network lock already acquired.
*
*                   See also 'NetIP_IsAddrHostCfgd()  Note #1'.
*
*               (2) IP address MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsAddrHostCfgdHandler (NET_IP_ADDR  addr)
{
    CPU_BOOLEAN  addr_host;


    addr_host = ((NetIP_AddrThisHost != NET_IP_ADDR_NONE) &&
                 (addr == NetIP_AddrThisHost)) ? DEF_YES : DEF_NO;

    return (addr_host);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_IsValidAddrHost()
*
* Description : (1) Validate an IP host address :
*
*                   (a) MUST NOT be one of the following :
*
*                       (1) This      Host                              RFC #1122, Section 3.2.1.3.(a)
*                       (2) Specified Host                              RFC #1122, Section 3.2.1.3.(b)
*                       (3) Localhost                                   RFC #1122, Section 3.2.1.3.(g)
*                       (4) Limited   Broadcast                         RFC #1122, Section 3.2.1.3.(c)
*                       (5) Directed  Broadcast                         RFC #1122, Section 3.2.1.3.(d)
*
*                   (b) (1) RFC #3927, Section 2.1 specifies the "IPv4 Link-Local address" :
*                           
*                           (A) "Range ... inclusive"  ...
*                               (1) "from 169.254.1.0" ...
*                               (2) "to   169.254.254.255".
*
*               (2) ONLY validates typical IP host addresses, since 'This Host' & 'Specified Host' IP host
*                   addresses are ONLY valid during a host's initialization (see Notes #1a1 & #1a4).  This
*                   function CANNOT be used to validate any 'This Host' or 'Specified Host' host addresses.
*
*
* Argument(s) : addr_host           IP host address to validate (see Note #4).
*
* Return(s)   : DEF_YES, if IP host address valid.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : various.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (3) See 'net_ip.h  IP ADDRESS DEFINES  Notes #2 & #3' for supported IP addresses.
*
*               (4) IP address MUST be in host-order.
*********************************************************************************************************
*/
/*$PAGE*/
CPU_BOOLEAN  NetIP_IsValidAddrHost (NET_IP_ADDR  addr_host)
{
    CPU_BOOLEAN  valid;


    valid = DEF_YES;
                                                                /* ---------------- VALIDATE HOST ADDR ---------------- */
                                                                /* Chk invalid 'This Host'             (see Note #1a1). */
    if (addr_host == NET_IP_ADDR_THIS_HOST) {
        valid = DEF_NO;

                                                                /* Chk invalid lim'd broadcast         (see Note #1a4). */
    } else if (addr_host == NET_IP_ADDR_BROADCAST) {
        valid = DEF_NO;

                                                                /* Chk invalid localhost               (see Note #1a3). */
    } else if ((addr_host & NET_IP_ADDR_LOCAL_HOST_MASK_NET) == NET_IP_ADDR_LOCAL_HOST_NET) {
        valid = DEF_NO;

                                                                /* Chk         link-local addrs        (see Note #1b1). */
    } else if ((addr_host & NET_IP_ADDR_LOCAL_LINK_MASK_NET) == NET_IP_ADDR_LOCAL_LINK_NET) {
                                                                /* Chk invalid link-local 'This Host'  (see Note #1a2). */
        if ((addr_host             & NET_IP_ADDR_LOCAL_LINK_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_LOCAL_LINK_MASK_HOST)) {
             valid = DEF_NO;
        }
                                                                /* Chk invalid link-local broadcast    (see Note #1a5). */
        if ((addr_host             & NET_IP_ADDR_LOCAL_LINK_MASK_HOST) ==
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_LOCAL_LINK_MASK_HOST)) {
             valid = DEF_NO;
        }
                                                                /* Chk invalid link-local addr         (see Note #1b1A).*/
        if ((addr_host < NET_IP_ADDR_LOCAL_LINK_HOST_MIN) ||
            (addr_host > NET_IP_ADDR_LOCAL_LINK_HOST_MAX)) {
             valid = DEF_NO;
        }


    } else if ((addr_host & NET_IP_ADDR_CLASS_A_MASK) == NET_IP_ADDR_CLASS_A) { 
                                                                    /* Chk invalid Class-A 'This Host' (see Note #1a2). */
        if ((addr_host             & NET_IP_ADDR_CLASS_A_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_A_MASK_HOST)) {
             valid = DEF_NO;
        }
                                                                    /* Chk invalid Class-A broadcast   (see Note #1a5). */
        if ((addr_host             & NET_IP_ADDR_CLASS_A_MASK_HOST) ==
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_A_MASK_HOST)) {
             valid = DEF_NO;
        }


    } else if ((addr_host & NET_IP_ADDR_CLASS_B_MASK) == NET_IP_ADDR_CLASS_B) {
                                                                    /* Chk invalid Class-B 'This Host' (see Note #1a2). */
        if ((addr_host             & NET_IP_ADDR_CLASS_B_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_B_MASK_HOST)) {
             valid = DEF_NO;
        }
                                                                    /* Chk invalid Class-B broadcast   (see Note #1a5). */
        if ((addr_host             & NET_IP_ADDR_CLASS_B_MASK_HOST) ==
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_B_MASK_HOST)) {
             valid = DEF_NO;
        }


    } else if ((addr_host & NET_IP_ADDR_CLASS_C_MASK) == NET_IP_ADDR_CLASS_C) {
                                                                    /* Chk invalid Class-C 'This Host' (see Note #1a2). */
        if ((addr_host             & NET_IP_ADDR_CLASS_C_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_C_MASK_HOST)) {
             valid = DEF_NO;
        }
                                                                    /* Chk invalid Class-C broadcast   (see Note #1a5). */
        if ((addr_host             & NET_IP_ADDR_CLASS_C_MASK_HOST) ==
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_C_MASK_HOST)) {
             valid = DEF_NO;
        }


    } else {                                                        /* Invalid addr class (see Note #3).                */
        valid = DEF_NO;
    }


    return (valid);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetIP_IsValidAddrThisHost()
*
* Description : (1) Validate an IP address for this host's IP address :
*
*                   (a) MUST NOT be one of the following :
*
*                       (1) This      Host                              RFC #1122, Section 3.2.1.3.(a)
*                       (2) Specified Host                              RFC #1122, Section 3.2.1.3.(b)
*                       (3) Localhost                                   RFC #1122, Section 3.2.1.3.(g)
*                       (4) Limited   Broadcast                         RFC #1122, Section 3.2.1.3.(c)
*                       (5) Directed  Broadcast                         RFC #1122, Section 3.2.1.3.(d)
*                       (6) Subnet    Broadcast                         RFC #1122, Section 3.2.1.3.(e)
*
*                   See also 'NetIP_IsValidAddrHost()  Note #1'.
*
*               (2) ONLY validates this host's IP address, since 'This Host' & 'Specified Host' IP host
*                   addresses are ONLY valid during a host's initialization (see Notes #1a1 & #1a4).  This
*                   function CANNOT be used to validate any 'This Host' or 'Specified Host' host addresses.
*
*
* Argument(s) : addr_host           IP host address to validate (see Note #4).
*
*               addr_subnet_mask    IP      address subnet mask (see Note #4).
*
* Return(s)   : DEF_YES, if this host's IP address valid.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetIP_CfgAddrThisHost().
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : (3) See 'net_ip.h  IP ADDRESS DEFINES  Notes #2 & #3' for supported IP addresses.
*
*               (4) IP addresses MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsValidAddrThisHost (NET_IP_ADDR  addr_host,
                                        NET_IP_ADDR  addr_subnet_mask)
{
    CPU_BOOLEAN  valid_host;
    CPU_BOOLEAN  valid_mask;
    CPU_BOOLEAN  valid;


    if ((addr_host        == NET_IP_ADDR_NONE) ||                   /* Chk invalid NULL addr(s).                        */
        (addr_subnet_mask == NET_IP_ADDR_NONE)) {
         return (DEF_NO);
    }
                                                                    /* Chk invalid subnet 'This Host' (see Note #1a2).  */
    if ((addr_host & addr_subnet_mask) == NET_IP_ADDR_THIS_HOST) {
         return (DEF_NO);
    }
                                                                    /* Chk invalid subnet broadcast   (see Note #1a6).  */
    if ((addr_host & addr_subnet_mask) == NET_IP_ADDR_BROADCAST) {
         return (DEF_NO);
    }


    valid_host = NetIP_IsValidAddrHost(addr_host);
    valid_mask = NetIP_IsValidAddrSubnetMask(addr_subnet_mask);

    valid      = ((valid_host == DEF_YES) &&
                  (valid_mask == DEF_YES)) ? DEF_YES : DEF_NO;

    return (valid);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetIP_IsValidAddrSubnetMask()
*
* Description : (1) Validate an IP address subnet mask :
*
*                   (a) RFC #1122, Section 3.2.1.3 states that :
*
*                       (1) "IP addresses are not permitted to have the value 0 or -1 for any of the ...
*                            <Subnet-number> fields" ...
*                       (2) "This implies that each of these fields will be at least two bits long."
*
*                   (b) RFC #950, Section 2.1 'Special Addresses' reiterates that "the values of all
*                       zeros and all ones in the subnet field should not be assigned to actual 
*                       (physical) subnets".
*
*                   (c) RFC #950, Section 2.1 also states that "the bits that identify the subnet ...
*                       need not be adjacent in the address.  However, we recommend that the subnet
*                       bits be contiguous and located as the most significant bits of the local
*                       address".
*
*                       #### Therefore, it is assumed that at least the most significant bit of the 
*                       network portion of the subnet address SHOULD be set.
*
*                       See also 'net_ip.h  IP ADDRESS DEFINES  Note #2b2'.
*
* Argument(s) : addr_subnet_mask    IP address subnet mask to validate (see Note #2).
*
* Return(s)   : DEF_YES, if IP address subnet mask valid.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetIP_IsValidAddrThisHost().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) IP addresses MUST be in host-order.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIP_IsValidAddrSubnetMask (NET_IP_ADDR  addr_subnet_mask)
{
    CPU_BOOLEAN  valid;
    NET_IP_ADDR  mask;
    CPU_INT08U   mask_size;
    CPU_INT08U   mask_nbr_one_bits;
    CPU_INT08U   mask_nbr_one_bits_min;
    CPU_INT08U   mask_nbr_one_bits_max;
    CPU_INT08U   i;

                                                                    /* ------------- VALIDATE SUBNET MASK ------------- */
                                                                    /* Chk invalid subnet class (see Note #1c).         */
    if ((addr_subnet_mask & NET_IP_ADDR_CLASS_SUBNET_MASK_MIN) == NET_IP_ADDR_NONE) {
        valid = DEF_NO;

    } else {                                                        /* Chk invalid subnet mask (see Notes #1a & #1b).   */
        mask_size         = sizeof(addr_subnet_mask) * DEF_OCTET_NBR_BITS;
        mask              = DEF_BIT_00;
        mask_nbr_one_bits = 0;
        for (i = 0; i < mask_size; i++) {                           /* Calc nbr subnet bits.                            */
            if (addr_subnet_mask & mask) {
                mask_nbr_one_bits++;
            }
            mask <<= 1;
        }

        mask_nbr_one_bits_min = 2;                                  /* See Note #1a2.                                   */
        mask_nbr_one_bits_max = mask_size - mask_nbr_one_bits_min;
                                                                    /* Chk invalid nbr subnet bits (see Note #1a2).     */
        if (mask_nbr_one_bits < mask_nbr_one_bits_min) {
            valid = DEF_NO;

        } else if (mask_nbr_one_bits > mask_nbr_one_bits_max) {
            valid = DEF_NO;

        } else {
            valid = DEF_YES;
        }
    }


    return (valid);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             NetIP_Rx()
*
* Description : (1) Process received datagrams & forward to network protocol layers :
*
*                   (a) Validate IP packet & options
*                   (b) Reassemble fragmented datagrams
*                   (c) Demultiplex datagram to higher-layer protocols
*                   (d) Update receive statistics
*
*               (2) Although IP data units are typically referred to as 'datagrams' (see RFC #791, Section 1.1), 
*                   the term 'IP packet' (see RFC #1983, 'packet') is used for IP Receive until the packet is 
*                   validated, & possibly reassembled, as an IP datagram.
*
*
* Argument(s) : pbuf        Pointer to network buffer that received IP packet.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 IP datagram successfully received & processed.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               ----- RETURNED BY NetIP_RxPktDiscard() : -----
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxPktFrameDemux(),
*               NetIP_TxPktDatagram().
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface function(s).
*
* Note(s)     : (3) NetIP_Rx() blocked until network initialization completes.
*
*               (4) Since NetIP_RxPktFragReasm() may return a pointer to a different packet buffer (see
*                   'NetIP_RxPktFragReasm()  Return(s)', 'pbuf_hdr' MUST be reloaded.
*
*               (5) (a) For single packet buffer IP datagrams, the datagram length is equal to the IP
*                       Total Length minus the IP Header Length.
*
*                   (b) For multiple packet buffer, fragmented IP datagrams, the datagram length is
*                       equal to the previously calculated total fragment size.
*
*                       (1) IP datagram length is stored ONLY in the first packet buffer of any
*                           fragmented packet buffers.
*
*               (6) Network buffer already freed by higher layer; only increment error counter.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetIP_Rx (NET_BUF  *pbuf,
                NET_ERR  *perr)
{
    NET_BUF_HDR  *pbuf_hdr;
    NET_IP_HDR   *pip_hdr;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit rx (see Note #3).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NetIP_RxPktDiscard(pbuf, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrNullPtrCtr);
        return;
    }
#endif


    NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatRxPktCtr);


                                                                /* ------------------ VALIDATE IP PKT ----------------- */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NetIP_RxPktValidateBuf(pbuf_hdr, perr);                     /* Validate rx'd buf.                                   */
    switch (*perr) {
        case NET_IP_ERR_NONE:
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        default:
             NetIP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif
    pip_hdr = (NET_IP_HDR *)&pbuf->Data[pbuf_hdr->IP_HdrIx];
    NetIP_RxPktValidate(pbuf, pbuf_hdr, pip_hdr, perr);         /* Validate rx'd pkt.                                   */


                                                                /* ------------------- REASM FRAGS -------------------- */
    switch (*perr) {
        case NET_IP_ERR_NONE:
             pbuf = NetIP_RxPktFragReasm(pbuf, pbuf_hdr, pip_hdr, perr);
             break;


        case NET_IP_ERR_INVALID_VER:
        case NET_IP_ERR_INVALID_LEN_HDR:
        case NET_IP_ERR_INVALID_LEN_TOT:
        case NET_IP_ERR_INVALID_LEN_DATA:
        case NET_IP_ERR_INVALID_FLAG:
        case NET_IP_ERR_INVALID_FRAG:
        case NET_IP_ERR_INVALID_PROTOCOL:
        case NET_IP_ERR_INVALID_CHK_SUM:
        case NET_IP_ERR_INVALID_ADDR_SRC:
        case NET_IP_ERR_INVALID_ADDR_DEST:
        case NET_IP_ERR_INVALID_OPT:
        case NET_IP_ERR_INVALID_OPT_LEN:
        case NET_IP_ERR_INVALID_OPT_NBR:
        case NET_IP_ERR_INVALID_OPT_END:
        case NET_IP_ERR_INVALID_OPT_FLAG:
        case NET_IP_ERR_RX_OPT_BUF_NONE_AVAIL:
        case NET_IP_ERR_RX_OPT_BUF_WR:
        default:
             NetIP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


/*$PAGE*/
                                                                        /* -------------- DEMUX DATAGRAM -------------- */
    switch (*perr) {                                                    /* Chk err from NetIP_RxPktFragReasm().         */
        case NET_IP_ERR_RX_FRAG_NONE:
        case NET_IP_ERR_RX_FRAG_COMPLETE:
             pbuf_hdr = &pbuf->Hdr;                                     /* Reload buf hdr ptr (see Note #4).            */
             if (*perr == NET_IP_ERR_RX_FRAG_NONE) {                    /* If pkt NOT frag'd, ...                       */
                  pbuf_hdr->IP_DatagramLen = pbuf_hdr->IP_TotLen        /* ... calc buf datagram len (see Note #5a).    */
                                           - pbuf_hdr->IP_HdrLen;
             } else {                                                   /* Else set tot frag size ...                   */
                  pbuf_hdr->IP_DatagramLen = pbuf_hdr->IP_FragSizeTot;  /* ...       as datagram len (see Note #5b).    */
             }
             NetIP_RxPktDemuxDatagram(pbuf, pbuf_hdr, perr);
             break;


        case NET_IP_ERR_RX_FRAG_REASM:                          /* Frag'd datagram in reasm.                            */
            *perr = NET_IP_ERR_NONE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_IP_ERR_RX_FRAG_DISCARD:
        case NET_IP_ERR_RX_FRAG_OFFSET:
        case NET_IP_ERR_RX_FRAG_SIZE:
        case NET_IP_ERR_RX_FRAG_SIZE_TOT:
        case NET_IP_ERR_RX_FRAG_LEN_TOT:
        default:
             NetIP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ----------------- UPDATE RX STATS ------------------ */
    switch (*perr) {                                            /* Chk err from NetIP_RxPktDemuxDatagram().             */
        case NET_ICMP_ERR_NONE:
        case NET_TCP_ERR_NONE:
        case NET_UDP_ERR_NONE:
             NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatRxDatagramProcessedCtr);
            *perr = NET_IP_ERR_NONE;
             break;


        case NET_ERR_RX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxPktDiscardedCtr);   /* See Note #6.                                 */
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
            

        case NET_ERR_INIT_INCOMPLETE:
        case NET_ERR_INVALID_PROTOCOL:
        default:
             NetIP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             NetIP_Tx()
*
* Description : (1) Prepare & transmit packets from transport protocol layers to network interface layer :
*
*                   (a) Validate  transmit packet
*                   (b) Prepare & transmit packet datagram
*                   (c) Update    transmit statistics
*
*
* Argument(s) : pbuf        Pointer to network buffer to transmit IP packet.
*
*               addr_src    Source      IP address.
*
*               addr_dest   Destination IP address.
*
*               TOS         Specific TOS to transmit IP packet (see Note #3a).
*
*               TTL         Specific TTL to transmit IP packet (see Note #3b) :
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
*               popts       Pointer to one or more IP options configuration data structures (see Note #3c) :
*
*                               NULL                            NO IP transmit options configuration.
*                               NET_IP_OPT_CFG_ROUTE_TS         Route &/or Internet Timestamp options configuration.
*                               NET_IP_OPT_CFG_SECURITY         Security options configuration
*                                                                   (see 'net_ip.c  Note #1f').
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 IP datagram(s) successfully prepared & transmitted
*                                                                   to network interface layer.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               ------- RETURNED BY NetIP_TxPktDiscard() : -------
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_TxMsgErr(),
*               NetICMP_TxMsgReq(),
*               NetICMP_TxMsgReply(),
*               NetUDP_Tx(),
*               NetTCP_TxPkt().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) NetIP_Tx() blocked until network initialization completes.
*
*               (3) (a) RFC #1122, Section 3.2.1.6 states that :
*
*                       (1) "The IP layer MUST provide a means ... to set the TOS field of every datagram 
*                            that is sent;" ...
*                       (2) "the default is all zero bits."
*
*                       See also 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES'.
*
*                   (b) RFC #1122, Section 3.2.1.7 states that :
*
*                       (1) "The IP layer MUST provide a means ... to set the TTL field of every datagram 
*                            that is sent."
*
*                       (2) "A host MUST NOT send a datagram with a Time-to-Live (TTL) value of zero."
*
*                       (3) "When a fixed TTL value is used, it MUST be configurable."
*
*                       See also 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES'.
*
*                   (c) RFC #1122, Section 3.2.1.8 states that "there MUST be a means ... to specify IP 
*                       options to be included in transmitted IP datagrams".
*
*                       See also 'net_ip.h  IP HEADER OPTION CONFIGURATION DATA TYPES'.
*
*               (4) Network buffer already freed by lower layer; only increment error counter.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetIP_Tx (NET_BUF      *pbuf,
                NET_IP_ADDR   addr_src,
                NET_IP_ADDR   addr_dest,
                NET_IP_TOS    TOS,
                NET_IP_TTL    TTL,
                CPU_INT16U    flags,
                void         *popts,
                NET_ERR      *perr)
{
    NET_BUF_HDR  *pbuf_hdr;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit tx (see Note #2).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NetIP_TxPktDiscard(pbuf, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrNullPtrCtr);
        return;
    }
#endif


                                                                /* ---------------- VALIDATE TX IP PKT ---------------- */
    pbuf_hdr = &pbuf->Hdr;
#if ((NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED))
    NetIP_TxPktValidate(pbuf_hdr,
                        addr_src,
                        addr_dest,
                        TOS,
                        TTL,
                        flags,
                        popts,
                        perr);
    switch (*perr) {
        case NET_IP_ERR_NONE:
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        case NET_IP_ERR_INVALID_LEN_DATA:
        case NET_IP_ERR_INVALID_TOS:
        case NET_IP_ERR_INVALID_ADDR_SRC:
        case NET_IP_ERR_INVALID_ADDR_DEST:
        case NET_IP_ERR_INVALID_FLAG:
        case NET_IP_ERR_INVALID_OPT_TYPE:
        case NET_IP_ERR_INVALID_OPT_LEN:
        case NET_IP_ERR_INVALID_OPT_CFG:
        case NET_IP_ERR_INVALID_OPT_ROUTE:
        default:
             NetIP_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif


/*$PAGE*/
                                                                /* -------------------- TX IP PKT --------------------- */
    NetIP_TxPkt(pbuf,
                pbuf_hdr,
                addr_src,
                addr_dest,
                TOS,
                TTL,
                flags,
                popts,
                perr);


                                                                /* ----------------- UPDATE TX STATS ------------------ */
    switch (*perr) {
        case NET_IP_ERR_NONE:
        case NET_IF_ERR_NONE:
             NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDatagramCtr);
            *perr = NET_IP_ERR_NONE;
             break;


        case NET_ERR_RX:
        case NET_ERR_TX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxPktDiscardedCtr);   /* See Note #4.                                 */
            *perr = NET_ERR_TX;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_ERR_INIT_INCOMPLETE:
        case NET_ERR_INVALID_PROTOCOL:
        case NET_IP_ERR_INVALID_LEN_HDR:
        case NET_IP_ERR_INVALID_FRAG:
        case NET_IP_ERR_INVALID_OPT_TYPE:
        case NET_IP_ERR_INVALID_OPT_LEN:
        case NET_IP_ERR_INVALID_ADDR_HOST:
        case NET_IP_ERR_INVALID_ADDR_NET:
        case NET_IP_ERR_INVALID_ADDR_GATEWAY:
        case NET_IP_ERR_TX_DEST_INVALID:
        case NET_BUF_ERR_INVALID_IX:
        case NET_BUF_ERR_INVALID_LEN:
        case NET_UTIL_ERR_NULL_PTR:
        case NET_UTIL_ERR_NULL_SIZE:
        default:
             NetIP_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetIP_ReTx()
*
* Description : (1) Prepare & re-transmit packets from transport protocol layers to network interface layer :
*
*                   (a) Validate  re-transmit packet
*                   (b) Prepare & re-transmit packet datagram
*                   (c) Update    re-transmit statistics
*
*
* Argument(s) : pbuf        Pointer to network buffer to re-transmit IP packet.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 IP datagram(s) successfully prepared & re-transmitted
*                                                                   to network interface layer.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               -------- RETURNED BY NetIP_TxPktDiscard() : ---------
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_TxConnReTxQ_Timeout().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) NetIP_ReTx() blocked until network initialization completes.
*
*               (3) Network buffer already freed by lower layer; only increment error counter.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetIP_ReTx (NET_BUF  *pbuf,
                  NET_ERR  *perr)
{
    NET_BUF_HDR  *pbuf_hdr;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit tx (see Note #2).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NetIP_TxPktDiscard(pbuf, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrNullPtrCtr);
        return;
    }
#endif


                                                                /* -------------- VALIDATE RE-TX IP PKT --------------- */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (pbuf_hdr->IP_HdrIx == NET_BUF_IX_NONE) {
        NetIP_TxPktDiscard(pbuf, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxInvalidBufIxCtr);
        return;
    }
#endif


                                                                /* ------------------- RE-TX IP PKT ------------------- */
    NetIP_ReTxPkt(pbuf,
                  pbuf_hdr,
                  perr);


                                                                /* ---------------- UPDATE RE-TX STATS ---------------- */
    switch (*perr) {
        case NET_IP_ERR_NONE:
        case NET_IF_ERR_NONE:
             NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDatagramCtr);
            *perr = NET_IP_ERR_NONE;
             break;


        case NET_ERR_RX:
        case NET_ERR_TX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxPktDiscardedCtr);   /* See Note #3.                                 */
            *perr = NET_ERR_TX;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_ERR_INIT_INCOMPLETE:
        case NET_IP_ERR_INVALID_ADDR_HOST:
        case NET_IP_ERR_INVALID_ADDR_NET:
        case NET_IP_ERR_INVALID_ADDR_GATEWAY:
        case NET_IP_ERR_TX_DEST_INVALID:
        case NET_UTIL_ERR_NULL_PTR:
        case NET_UTIL_ERR_NULL_SIZE:
        default:
             NetIP_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
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
*                                      NetIP_RxPktValidateBuf()
*
* Description : Validate received buffer header as IP protocol.
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header that received IP packet.
*               --------    Argument validated in NetIP_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 Received buffer's IP header validated.
*                               NET_ERR_INVALID_PROTOCOL        Buffer's protocol type is NOT IP.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetIP_RxPktValidateBuf (NET_BUF_HDR  *pbuf_hdr,
                                      NET_ERR      *perr)
{
                                                                /* ---------------- VALIDATE IP BUF HDR --------------- */
    if (pbuf_hdr->ProtocolHdrType != NET_PROTOCOL_TYPE_IP_V4) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxInvalidProtocolCtr);
       *perr = NET_ERR_INVALID_PROTOCOL;
        return;
    }

    if (pbuf_hdr->IP_HdrIx == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

   *perr = NET_IP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIP_RxPktValidate()
*
* Description : (1) Validate received IP packet :
*
*                   (a) (1) Validate the received packet's following IP header fields :
*
*                           (A) Version
*                           (B) Header Length
*                           (C) Total  Length                               See Note #4
*                           (D) Flags
*                           (E) Fragment Offset
*                           (F) Protocol
*                           (G) Check-Sum                                   See Note #5
*                           (H) Source      Address                         See Note #9c
*                           (I) Destination Address                         See Note #9d
*                           (J) Options
*
*                       (2) Validation ignores the following IP header fields :
*
*                           (A) Type of Service (TOS)
*                           (B) Identification  (ID)
*                           (C) Time-to-Live    (TTL)
*
*                   (b) Convert the following IP header fields from network-order to host-order :
*
*                       (1) Total Length                                    See Notes #1bB1 & #3b
*                       (2) Identification (ID)                             See Note  #1bB2
*                       (3) Flags/Fragment Offset                           See Note  #1bB3
*                       (4) Check-Sum                                       See Note  #5d
*                       (5) Source      Address                             See Notes #1bB4 & #3c
*                       (6) Destination Address                             See Notes #1bB5 & #3d
*                       (7) All Options' multi-octet words                  See Notes #1bB6 & #1bC
*
*                           (A) These fields are NOT converted directly in the received packet buffer's 
*                               data area but are converted in local or network buffer variables ONLY.
*
*                           (B) The following IP header fields are converted & stored in network buffer
*                               variables :
*
*                               (1) Total Length
*                               (2) Identification (ID)
*                               (3) Flags/Fragment Offset
*                               (4) Source      Address
*                               (5) Destination Address
*                               (6) IP Options' multi-octet words
*
*                           (C) Since any IP packet may receive a number of various IP options that may 
*                               require conversion from network-order to host-order, IP options are copied 
*                               into a separate network buffer for validation, conversion, & demultiplexing.
*
*                   (c) Update network buffer's protocol controls
*
*                   (d) Process IP packet in ICMP Receive Handler
*
*
* Argument(s) : pbuf        Pointer to network buffer that received IP packet.
*               ----        Argument checked   in NetIP_Rx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetIP_Rx().
*
*               pip_hdr     Pointer to received packet's IP header.
*               -------     Argument validated in NetIP_Rx()/NetIP_RxPktValidateBuf().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                     Received packet validated.
*                               NET_IP_ERR_INVALID_VER              Invalid IP version.
*                               NET_IP_ERR_INVALID_LEN_HDR          Invalid IP header length.
*                               NET_IP_ERR_INVALID_LEN_TOT          Invalid IP total  length.
*                               NET_IP_ERR_INVALID_LEN_DATA         Invalid IP data   length.
*                               NET_IP_ERR_INVALID_FLAG             Invalid IP flags.
*                               NET_IP_ERR_INVALID_FRAG             Invalid IP fragmentation.
*                               NET_IP_ERR_INVALID_PROTOCOL         Invalid/unknown protocol type.
*                               NET_IP_ERR_INVALID_CHK_SUM          Invalid IP check-sum.
*                               NET_IP_ERR_INVALID_ADDR_SRC         Invalid IP source      address.
*                               NET_IP_ERR_INVALID_ADDR_DEST        Invalid IP destination address.
*                               NET_IP_ERR_INVALID_ADDR_BROADCAST   Invalid IP broadcast.
*
*                                                                   - RETURNED BY NetIP_RxPktValidateOpt() : --
*                               NET_IP_ERR_RX_OPT_BUF_NONE_AVAIL    No available buffers to process IP options.
*                               NET_IP_ERR_RX_OPT_BUF_WR            IP options failed to write to buffer.
*                               NET_IP_ERR_INVALID_OPT              Invalid IP option.
*                               NET_IP_ERR_INVALID_OPT_LEN          Invalid IP option length.
*                               NET_IP_ERR_INVALID_OPT_NBR          Invalid IP option number of same option.
*                               NET_IP_ERR_INVALID_OPT_END          Invalid IP option list ending.
*                               NET_IP_ERR_INVALID_OPT_FLAG         Invalid IP option flag.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_Rx().
*$PAGE*
* Note(s)     : (2) See 'net_ip.h  IP HEADER' for IP header format.
*
*               (3) The following IP header fields MUST be decoded &/or converted from network-order to host-order 
*                   BEFORE any ICMP Error Messages are transmitted (see 'net_icmp.c  NetICMP_TxMsgErr()  Note #3') :
*
*                   (a) Header Length
*                   (b) Total  Length
*                   (c) Source      Address
*                   (d) Destination Address
*
*               (4) (a) In addition to validating that the IP header Total Length is greater than or equal to the
*                       IP header Header Length, the IP total length SHOULD be compared to the remaining packet 
*                       data length which should be identical.
*
*                   (b) (1) However, some network interfaces MAY append octets to their frames :
*
*                           (A) 'pad' octets, if the frame length does NOT meet the frame's required minimum size :
*
*                               (1) RFC #894, Section 'Frame Format' states that "the minimum length of  the data 
*                                   field of a packet sent over an Ethernet is 46 octets.  If necessary, the data 
*                                   field should be padded (with octets of zero) to meet the Ethernet minimum frame 
*                                   size.  This padding is not part of the IP packet and is not included in the  
*                                   total length field of the IP header".
*
*                               (2) RFC #1042, Section 'Frame Format and MAC Level Issues : For all hardware types' 
*                                   states that "IEEE 802 packets may have a minimum size restriction.  When  
*                                   necessary, the data field should be padded (with octets of zero) to meet the 
*                                   IEEE 802 minimum frame size requirements.  This padding is not part of the IP 
*                                   datagram and is not included in the total length field of the IP header".
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
*                           remaining data length MAY be greater than the IP total length :
*
*                           (A) Thus,    the IP total length & the packet's remaining data length CANNOT be 
*                               compared for equality.
*
*                               (1) Unfortunately, this eliminates the possibility to validate the IP total 
*                                   length to the packet's remaining data length.
*
*                           (B) And      the IP total length MAY    be less    than the packet's remaining 
*                               data length.
*
*                               (1) However, the packet's remaining data length MUST be reset to the IP 
*                                   total length to correctly calculate higher-layer application data
*                                   length.
*
*                           (C) However, the IP total length CANNOT be greater than the packet's remaining 
*                               data length.
*$PAGE*
*               (5) (a) IP header Check-Sum field MUST be validated BEFORE (or AFTER) any multi-octet words
*                       are converted from network-order to host-order since "the sum of 16-bit integers can
*                       be computed in either byte order" [RFC #1071, Section 2.(B)].
*
*                       In other words, the IP header Check-Sum CANNOT be validated AFTER SOME but NOT ALL 
*                       multi-octet words have been converted from network-order to host-order.
*
*                   (b) However, ALL received packets' multi-octet words are converted in local or network 
*                       buffer variables ONLY (see Note #1bA).  Therefore, IP header Check-Sum may be validated
*                       at any point.
*
*                   (c) For convenience, the IP header Check-Sum is validated AFTER IP Version, Header Length,
*                       & Total Length fields have been validated.  Thus, invalid IP version or length packets
*                       are quickly discarded (see Notes #9a, #8a, & #8b) & the total IP header length
*                       (in octets) will already be calculated for the IP Check-Sum calculation.
*
*                   (d) After the IP header Check-Sum is validated, it is NOT necessary to convert the Check-
*                       Sum from network-order to host-order since  it is NOT required for further processing.
*
*               (6) (a) RFC #791, Section 3.2 'Fragmentation and Reassembly' states that "if an internet datagram
*                       is fragmented" :
*
*                       (1) "Fragments are counted in units of 8 octets."
*                       (2) "The minimum fragment is 8 octets."
*
*                   (b) (1) However, this CANNOT apply "if this is the last fragment" ...
*                           (A) "(that is the more fragments field is zero)";         ...
*                       (2) Which may be of ANY size.
*
*                   See also 'net_ip.h  IP FRAGMENTATION DEFINES  Note #1a'.
*
*               (7) (a) RFC #792, Section 'Destination Unreachable Message : Description' states that "if, in 
*                       the destination host, the IP module cannot deliver the datagram because the indicated 
*                       protocol module ... is not active, the destination host may send a destination unreachable 
*                       message to the source host".
*
*                   (b) Default case already invalidated earlier in this function.  However, the default case
*                       is included as an extra precaution in case 'Protocol' is incorrectly modified.
*
*               (8) ICMP Error Messages are sent if any of the following IP header fields are invalid :
*
*                   (a) Header Length                               ICMP 'Parameter   Problem'  Error Message
*                   (b) Total  Length                               ICMP 'Parameter   Problem'  Error Message
*                   (c) Flags                                       ICMP 'Parameter   Problem'  Error Message
*                   (d) Fragment Offset                             ICMP 'Parameter   Problem'  Error Message
*                   (e) Protocol                                    ICMP 'Unreachable Protocol' Error Message
*                   (f) Options                                     ICMP 'Parameter   Problem'  Error Messages
*                                                                        [see NetIP_RxPktValidateOpt()]
*$PAGE*
*               (9) RFC #1122, Section 3.2.1 requires that IP packets with the following invalid IP header
*                   fields be "silently discarded" :
*
*                   (a) Version                                             RFC #1122, Section 3.2.1.1
*                   (b) Check-Sum                                           RFC #1122, Section 3.2.1.2
*
*                   (c) Source Address
*
*                       (1) (A) RFC #1122, Section 3.2.1.3 states that "a host MUST silently discard  
*                               an incoming datagram containing an IP source address that is invalid 
*                               by the rules of this section".
*
*                           (B) (1) MAY      be one of the following :
*                                   (a) Configured host address             RFC #1122, Section 3.2.1.3.(1)
*                                   (b) Localhost       address             RFC #1122, Section 3.2.1.3.(g)
*                                   (c) This       Host                     RFC #1122, Section 3.2.1.3.(a)
*                                   (d) Specified  Host                     RFC #1122, Section 3.2.1.3.(b)
*
*                               (2) MUST NOT be one of the following :
*                                   (a) Limited   Broadcast                 RFC #1122, Section 3.2.1.3.(c)
*                                   (b) Directed  Broadcast                 RFC #1122, Section 3.2.1.3.(d)
*                                   (c) Subnet    Broadcast                 RFC #1122, Section 3.2.1.3.(e)
*                                   (d) Multicast                           RFC #1122, Section 3.2.1.3.(3)
*
*                               (3) RFC #1122, Section 3.2.1.3.(g) states that the "internal host loopback
*                                   address ... MUST NOT appear outside a host".
*
*                                   (a) However, this does NOT prevent the host loopback address from being 
*                                       used as an IP packet's source address as long as BOTH the packet's 
*                                       source AND destination addresses are internal host addresses, either 
*                                       the host's configured IP address or any host loopback address.
*
*                       (2) (A) RFC #3927, Section 2.1 specifies the "IPv4 Link-Local address" :
*                           
*                               (1) "Range ... inclusive"  ...
*                                   (a) "from 169.254.1.0" ...
*                                   (b) "to   169.254.254.255".
*
*                   (d) Destination Address
*
*                       (1) (A) RFC #1122, Section 3.2.1.3 states that "a host MUST silently discard 
*                               an incoming datagram that is not destined for" :
*
*                               (1) "(one of) the host's IP address(es); or" ...
*                               (2) "an IP broadcast address valid for the connected network."
*
*                           (B) (1) MUST     be one of the following :
*                                   (a) Configured host address             RFC #1122, Section 3.2.1.3.(1)
*                                   (b) Localhost                           RFC #1122, Section 3.2.1.3.(g)
*                                   (c) Limited    Broadcast                RFC #1122, Section 3.2.1.3.(c)
*                                   (d) Directed   Broadcast                RFC #1122, Section 3.2.1.3.(d)
*                                   (e) Subnet     Broadcast                RFC #1122, Section 3.2.1.3.(e)
*
*                               (2) MUST NOT be one of the following :
*                                   (a) This      Host                      RFC #1122, Section 3.2.1.3.(a)
*                                   (b) Specified Host                      RFC #1122, Section 3.2.1.3.(b)
*
*                               (3) RFC #1122, Section 3.2.1.3.(g) states that the "internal host loopback
*                                   address ... MUST NOT appear outside a host".
*
*                       (2) (A) RFC #1122, Section 3.3.6 states that :
*
*                               (1) "When a host sends a datagram to a link-layer broadcast address, the IP
*                                    destination address MUST be a legal IP broadcast or IP multicast address."
*
*                               (2) "A host SHOULD silently discard a datagram that is received via a link-
*                                    layer broadcast ... but does not specify an IP multicast or broadcast
*                                    destination address."
*
*                           (B) (1) Thus, any packet received as ... :
*
*                                   (a) ... an IP broadcast destination address MUST also have been received
*                                        as a link-layer broadcast.
*
*                                   (b) ... a link-layer broadcast MUST also be received as an IP broadcast.
*
*                               (2) Therefore, any packet received as EITHER a link-layer broadcast or an IP
*                                   broadcast, but not BOTH, MUST be silently discarded.
*
*              (10) See 'net_ip.h  IP ADDRESS DEFINES  Notes #2 & #3' for supported IP addresses.
*$PAGE*
*              (11) (a) RFC #1122, Section 3.2.1.6 states that "the IP layer SHOULD pass received TOS values 
*                       up to the transport layer".
*
*                       #### NOT currently implemented.
*
*                   (b) RFC #1122, Section 3.2.1.8 states that "all IP options ... received in datagrams MUST 
*                       be passed to the transport layer (or to ICMP processing when the datagram is an ICMP 
*                       message).  The IP and transport layer MUST each interpret those IP options that they 
*                       understand and silently ignore the others".
*
*                       #### NOT currently implemented.
*********************************************************************************************************
*/

static  void  NetIP_RxPktValidate (NET_BUF      *pbuf,
                                   NET_BUF_HDR  *pbuf_hdr,
                                   NET_IP_HDR   *pip_hdr,
                                   NET_ERR      *perr)
{
    NET_ERR      msg_err;
    CPU_INT08U   ip_ver;
    CPU_INT08U   ip_hdr_len;
    CPU_INT16U   ip_hdr_len_tot;
    CPU_INT16U   ip_flags;
    CPU_INT16U   ip_frag_offset;
    CPU_INT16U   protocol_ix;
    CPU_BOOLEAN  ip_flag_reserved;
    CPU_BOOLEAN  ip_flag_dont_frag;
    CPU_BOOLEAN  ip_flag_frags_more;
    CPU_BOOLEAN  ip_chk_sum_valid;
    CPU_BOOLEAN  ip_broadcast;
    CPU_BOOLEAN  rx_broadcast;
    CPU_BOOLEAN  rx_remote;


                                                                /* ---------------- CONVERT IP FIELDS ----------------- */
                                                                /* See Note #3.                                         */
    NET_UTIL_VAL_COPY_GET_NET_16(&pbuf_hdr->IP_TotLen,   &pip_hdr->TotLen);
    NET_UTIL_VAL_COPY_GET_NET_32(&pbuf_hdr->IP_AddrSrc,  &pip_hdr->AddrSrc);
    NET_UTIL_VAL_COPY_GET_NET_32(&pbuf_hdr->IP_AddrDest, &pip_hdr->AddrDest);



                                                                /* ----------------- VALIDATE IP VER ------------------ */
    ip_ver   = pip_hdr->Ver_HdrLen & NET_IP_HDR_VER_MASK;       /* See 'net_ip.h  IP HEADER  Note #2'.                  */
    ip_ver >>= NET_IP_HDR_VER_SHIFT;
    if (ip_ver != NET_IP_HDR_VER) {                             /* Validate IP ver.                                     */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrVerCtr);
       *perr = NET_IP_ERR_INVALID_VER;
        return;
    }


                                                                /* --------------- VALIDATE IP HDR LEN ---------------- */
                                                                /* See 'net_ip.h  IP HEADER  Note #2'.                  */
    ip_hdr_len          =  pip_hdr->Ver_HdrLen   & NET_IP_HDR_LEN_MASK;
    ip_hdr_len_tot      = (CPU_INT16U)ip_hdr_len * NET_IP_HDR_LEN_WORD_SIZE;
    pbuf_hdr->IP_HdrLen = (CPU_INT16U)ip_hdr_len_tot;           /* See Note #3a.                                        */

    if (ip_hdr_len < NET_IP_HDR_LEN_MIN) {                      /* If hdr len < min hdr len, rtn err.                   */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrLenCtr);
        NetICMP_TxMsgErr(pbuf,
                         NET_ICMP_MSG_TYPE_PARAM_PROB,
                         NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                         NET_ICMP_PTR_IX_IP_HDR_HDR_LEN,
                        &msg_err);
       *perr = NET_IP_ERR_INVALID_LEN_HDR;
        return;
    }
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (ip_hdr_len > NET_IP_HDR_LEN_MAX) {                      /* If hdr len > max hdr len, rtn err.                   */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrLenCtr);
        NetICMP_TxMsgErr(pbuf,
                         NET_ICMP_MSG_TYPE_PARAM_PROB,
                         NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                         NET_ICMP_PTR_IX_IP_HDR_HDR_LEN,
                        &msg_err);
       *perr = NET_IP_ERR_INVALID_LEN_HDR;
        return;
    }
#endif
                                  


/*$PAGE*/
                                                                /* --------------- VALIDATE IP TOT LEN ---------------- */
#if 0                                                           /* See Note #3b.                                        */
    NET_UTIL_VAL_COPY_GET_NET_16(&pbuf_hdr->IP_TotLen, &pip_hdr->TotLen);
#endif
    if (pbuf_hdr->IP_TotLen < ip_hdr_len_tot) {                 /* If IP tot len < hdr len, rtn err.                    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrTotLenCtr);
        NetICMP_TxMsgErr(pbuf,
                         NET_ICMP_MSG_TYPE_PARAM_PROB,
                         NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                         NET_ICMP_PTR_IX_IP_HDR_TOT_LEN,
                        &msg_err);
       *perr = NET_IP_ERR_INVALID_LEN_TOT;
        return;
    }

    if (pbuf_hdr->IP_TotLen > pbuf_hdr->DataLen) {              /* If IP tot len > rem pkt data len, ...                */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrTotLenCtr);
        NetICMP_TxMsgErr(pbuf,
                         NET_ICMP_MSG_TYPE_PARAM_PROB,
                         NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                         NET_ICMP_PTR_IX_IP_HDR_TOT_LEN,
                        &msg_err);
       *perr = NET_IP_ERR_INVALID_LEN_TOT;                      /* ... rtn err (see Note #4b2C).                        */
        return;
    }

    pbuf_hdr->DataLen    = (NET_BUF_SIZE) pbuf_hdr->IP_TotLen;  /* Trunc pkt data len to IP tot len (see Note #4b2B1).  */
    pbuf_hdr->IP_DataLen = (CPU_INT16U  )(pbuf_hdr->IP_TotLen - pbuf_hdr->IP_HdrLen);



                                                                /* ---------------- VALIDATE IP CHK SUM --------------- */
                                                                /* See Note #5.                                         */
    ip_chk_sum_valid = NetUtil_16BitOnesCplChkSumHdrVerify((void     *)pip_hdr,
                                                           (CPU_INT16U)ip_hdr_len_tot,
                                                           (NET_ERR  *)perr);
    if (ip_chk_sum_valid != DEF_OK) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrChkSumCtr);
       *perr = NET_IP_ERR_INVALID_CHK_SUM;
        return;
    }
#if 0                                                           /* Conv to host-order NOT necessary (see Note #5d).     */
   (void)NET_UTIL_VAL_GET_NET_16(&pip_hdr->ChkSum);
#endif



                                                                /* ------------------ CONVERT IP ID ------------------- */
    NET_UTIL_VAL_COPY_GET_NET_16(&pbuf_hdr->IP_ID, &pip_hdr->ID);


                                                                /* ---------------- VALIDATE IP FLAGS ----------------- */
                                                                /* See 'net_ip.h  IP HEADER  Note #4'.                  */
    NET_UTIL_VAL_COPY_GET_NET_16(&pbuf_hdr->IP_Flags_FragOffset, &pip_hdr->Flags_FragOffset);
    ip_flags         = pbuf_hdr->IP_Flags_FragOffset & NET_IP_HDR_FLAG_MASK;
#if 1                                                           /* ???? Allow invalid reserved flag for rx'd datagrams? */
    ip_flag_reserved = DEF_BIT_IS_SET_ANY(ip_flags, NET_IP_HDR_FLAG_RESERVED);
    if (ip_flag_reserved != DEF_NO) {                           /* If reserved flag bit set, rtn err.                   */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrFlagsCtr);
        NetICMP_TxMsgErr(pbuf,
                         NET_ICMP_MSG_TYPE_PARAM_PROB,
                         NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                         NET_ICMP_PTR_IX_IP_HDR_FLAGS,
                        &msg_err);
       *perr = NET_IP_ERR_INVALID_FLAG;
        return;
    }
#endif


/*$PAGE*/
                                                                /* ----------------- VALIDATE IP FRAG ----------------- */
                                                                /* See 'net_ip.h  IP HEADER  Note #4'.                  */
    ip_flag_dont_frag  = DEF_BIT_IS_SET(ip_flags, NET_IP_HDR_FLAG_FRAG_DONT);
    ip_flag_frags_more = DEF_BIT_IS_SET(ip_flags, NET_IP_HDR_FLAG_FRAG_MORE);
    if (ip_flag_dont_frag != DEF_NO) {                          /* If  'Don't Frag' flag set & ...                      */
        if (ip_flag_frags_more != DEF_NO) {                     /* ... 'More Frags' flag set, rtn err.                  */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrFragCtr);
            NetICMP_TxMsgErr(pbuf,
                             NET_ICMP_MSG_TYPE_PARAM_PROB,
                             NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                             NET_ICMP_PTR_IX_IP_HDR_FRAG_OFFSET,
                            &msg_err);
           *perr = NET_IP_ERR_INVALID_FRAG;
            return;
        }

        ip_frag_offset = pbuf_hdr->IP_Flags_FragOffset & NET_IP_HDR_FRAG_OFFSET_MASK;
        if (ip_frag_offset != NET_IP_HDR_FRAG_OFFSET_NONE) {    /* ... frag offset != 0,      rtn err.                  */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrFragCtr);
            NetICMP_TxMsgErr(pbuf,
                             NET_ICMP_MSG_TYPE_PARAM_PROB,
                             NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                             NET_ICMP_PTR_IX_IP_HDR_FRAG_OFFSET,
                            &msg_err);
           *perr = NET_IP_ERR_INVALID_FRAG;
            return;
        }
    }

    if (ip_flag_frags_more != DEF_NO) {                             /* If 'More Frags' set (see Note #6b1A) ...         */
                                                                    /* ... & IP data len NOT multiple of    ...         */
        if ((pbuf_hdr->IP_DataLen % NET_IP_FRAG_SIZE_UNIT) != 0) {  /* ... frag size units (see Note #6a), rtn err.     */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrFragCtr);
            NetICMP_TxMsgErr(pbuf,
                             NET_ICMP_MSG_TYPE_PARAM_PROB,
                             NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                             NET_ICMP_PTR_IX_IP_HDR_TOT_LEN,
                            &msg_err);
           *perr = NET_IP_ERR_INVALID_FRAG;
            return;
        }
    }



                                                                /* --------------- VALIDATE IP PROTOCOL --------------- */
    switch (pip_hdr->Protocol) {                                /* See 'net_ip.h  IP HEADER  Note #5'.                  */
        case NET_IP_HDR_PROTOCOL_ICMP:
        case NET_IP_HDR_PROTOCOL_TCP:
        case NET_IP_HDR_PROTOCOL_UDP:
             break;


        default:                                                /* See Note #7a.                                        */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrProtocolCtr);
             NetICMP_TxMsgErr(pbuf,
                              NET_ICMP_MSG_TYPE_DEST_UNREACH,
                              NET_ICMP_MSG_CODE_DEST_PROTOCOL,
                              NET_ICMP_MSG_PTR_NONE,
                             &msg_err);
            *perr = NET_IP_ERR_INVALID_PROTOCOL;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                    /* -------------- VALIDATE IP ADDRS --------------- */
#if 0                                                               /* See Notes #3c & #3d.                             */
    NET_UTIL_VAL_COPY_GET_NET_32(&pbuf_hdr->IP_AddrSrc,  &pip_hdr->AddrSrc);
    NET_UTIL_VAL_COPY_GET_NET_32(&pbuf_hdr->IP_AddrDest, &pip_hdr->AddrDest);
#endif
                                                                    /* Chk pkt rx'd via local or remote host.           */
    rx_remote = DEF_BIT_IS_SET(pbuf_hdr->Flags, NET_BUF_FLAG_RX_REMOTE);


                                                                    /* ------------- VALIDATE IP SRC ADDR ------------- */
                                                                    /* See Note #9c.                                    */
    if (pbuf_hdr->IP_AddrSrc == NET_IP_ADDR_THIS_HOST) {            /* Chk 'This Host' addr        (see Note #9c1B1c).  */

                                                                    /* Chk         localhost addrs.                     */
    } else if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_LOCAL_HOST_MASK_NET) == NET_IP_ADDR_LOCAL_HOST_NET) {
                                                                    /* Chk invalid localhost addrs.                     */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_LOCAL_HOST_MASK_HOST) == 
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_LOCAL_HOST_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_SRC;
             return;
        }
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_LOCAL_HOST_MASK_HOST) == 
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_LOCAL_HOST_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_SRC;
             return;
        }

        if (rx_remote != DEF_NO) {                                  /* If localhost addr rx'd via remote host, ...      */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
           *perr = NET_IP_ERR_INVALID_ADDR_SRC;                     /* ... rtn err / discard pkt   (see Note #9c1B3a).  */
            return;
        }

                                                                    /* Chk link-local addrs        (see Note #9c2).     */
    } else if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_LOCAL_LINK_MASK_NET) == NET_IP_ADDR_LOCAL_LINK_NET) {
                                                                    /* Chk link-local broadcast    (see Note #9c1B2b).  */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_LOCAL_LINK_MASK_HOST) ==
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_LOCAL_LINK_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_SRC;
             return;
        }
                                                                    /* Chk invalid link-local addr (see Note #9c2A1).   */
        if ((pbuf_hdr->IP_AddrSrc < NET_IP_ADDR_LOCAL_LINK_HOST_MIN) ||
            (pbuf_hdr->IP_AddrSrc > NET_IP_ADDR_LOCAL_LINK_HOST_MAX)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_SRC;
             return;
        }


/*$PAGE*/
                                                                    /* Chk invalid lim'd broadcast (see Note #9c1B2a).  */
    } else if (pbuf_hdr->IP_AddrSrc == NET_IP_ADDR_BROADCAST) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
       *perr = NET_IP_ERR_INVALID_ADDR_SRC;
        return;


    } else if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_CLASS_A_MASK) == NET_IP_ADDR_CLASS_A) { 
                                                                    /* Chk Class-A broadcast       (see Note #9c1B2b).  */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_CLASS_A_MASK_HOST) ==
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_A_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_SRC;
             return;
        }

    } else if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_CLASS_B_MASK) == NET_IP_ADDR_CLASS_B) {
                                                                    /* Chk Class-B broadcast       (see Note #9c1B2b).  */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_CLASS_B_MASK_HOST) ==
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_B_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_SRC;
             return;
        }

    } else if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_CLASS_C_MASK) == NET_IP_ADDR_CLASS_C) {
                                                                    /* Chk Class-C broadcast       (see Note #9c1B2b).  */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_CLASS_C_MASK_HOST) ==
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_C_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_SRC;
             return;
        }

    } else {                                                        /* Discard invalid addr class  (see Note #10).      */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
       *perr = NET_IP_ERR_INVALID_ADDR_SRC;
        return;
    }

                                                                    /* Chk subnet  broadcast       (see Note #9c1B2c).  */
    if ((NetIP_AddrThisHostSubnetMask     != NET_IP_ADDR_NONE) &&
        (NetIP_AddrThisHostSubnetMaskHost != NET_IP_ADDR_NONE) &&
        (NetIP_AddrThisHostSubnetNet      != NET_IP_ADDR_NONE)) {
        if ((pbuf_hdr->IP_AddrSrc & NetIP_AddrThisHostSubnetMask) ==
                                    NetIP_AddrThisHostSubnetNet ) {
            if ((pbuf_hdr->IP_AddrSrc  & NetIP_AddrThisHostSubnetMaskHost) ==
                (NET_IP_ADDR_BROADCAST & NetIP_AddrThisHostSubnetMaskHost)) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrAddrSrcCtr);
                *perr = NET_IP_ERR_INVALID_ADDR_SRC;
                 return;
            }
        }
    }



/*$PAGE*/
                                                                    /* ------------- VALIDATE IP DEST ADDR ------------ */
                                                                    /* See Note #9d.                                    */
    ip_broadcast = DEF_NO;


    if (pbuf_hdr->IP_AddrDest  == NetIP_AddrThisHost) {             /* Chk this host's cfg'd addr (see Note #9d1B1a).   */

        if (NetIP_AddrThisHost == NET_IP_ADDR_NONE) {               /* Chk invalid 'This Host'    (see Note #9d1B2a).   */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestCtr);
           *perr = NET_IP_ERR_INVALID_ADDR_DEST;
            return;
        }

                                                                    /* Chk         localhost      (see Note #9d1B1b).   */
    } else if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_LOCAL_HOST_MASK_NET) == NET_IP_ADDR_LOCAL_HOST_NET) {
                                                                    /* Chk invalid localhost addrs.                     */
        if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_LOCAL_HOST_MASK_HOST) == 
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_LOCAL_HOST_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_DEST;
             return;
        }

        if (rx_remote != DEF_NO) {                                  /* If localhost addr rx'd via remote host, ...      */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestCtr);
           *perr = NET_IP_ERR_INVALID_ADDR_DEST;                    /* ... rtn err / discard pkt  (see Note #9d1B3).    */
            return;
        }

        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatRxDestLocalHostCtr);

                                                                    /* Chk invalid 'This Host'    (see Note #9d1B2a).   */
    } else if (pbuf_hdr->IP_AddrDest == NET_IP_ADDR_THIS_HOST) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestCtr);
       *perr = NET_IP_ERR_INVALID_ADDR_DEST;
        return;


    } else {
                                                                    /* Chk lim'd broadcast        (see Note #9d1B1c).   */
        if (pbuf_hdr->IP_AddrDest != NET_IP_ADDR_BROADCAST) {   

            if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_CLASS_A_MASK) == NET_IP_ADDR_CLASS_A) {
                                                                    /* Chk Class-A 'This Host'    (see Note #9d1B2b).   */
                if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_CLASS_A_MASK_HOST) ==
                    (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_A_MASK_HOST)) {
                     NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestCtr);
                    *perr = NET_IP_ERR_INVALID_ADDR_DEST;
                     return;
                }
                                                                    /* Chk Class-A broadcast      (see Note #9d1B1d).   */
                if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_CLASS_A_MASK_HOST) ==
                    (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_A_MASK_HOST)) {
                     ip_broadcast = DEF_YES;
                }

            } else if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_CLASS_B_MASK) == NET_IP_ADDR_CLASS_B) {
                                                                    /* Chk Class-B 'This Host'    (see Note #9d1B2b).   */
                if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_CLASS_B_MASK_HOST) ==
                    (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_B_MASK_HOST)) {
                     NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestCtr);
                    *perr = NET_IP_ERR_INVALID_ADDR_DEST;
                     return;
                }
                                                                    /* Chk Class-B broadcast      (see Note #9d1B1d).   */
                if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_CLASS_B_MASK_HOST) ==
                    (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_B_MASK_HOST)) {
                     ip_broadcast = DEF_YES;
                }

            } else if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_CLASS_C_MASK) == NET_IP_ADDR_CLASS_C) {
                                                                    /* Chk Class-C 'This Host'    (see Note #9d1B2b).   */
                if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_CLASS_C_MASK_HOST) ==
                    (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_C_MASK_HOST)) {
                     NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestCtr);
                    *perr = NET_IP_ERR_INVALID_ADDR_DEST;
                     return;
                }
                                                                    /* Chk Class-C broadcast      (see Note #9d1B1d).   */
                if ((pbuf_hdr->IP_AddrDest & NET_IP_ADDR_CLASS_C_MASK_HOST) ==
                    (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_C_MASK_HOST)) {
                     ip_broadcast = DEF_YES;
                }

            } else {                                                /* Discard invalid addr class (see Note #10).       */
                NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestCtr);
               *perr = NET_IP_ERR_INVALID_ADDR_DEST;
                return;
            }
/*$PAGE*/
                                                                    /* Chk subnet broadcast       (see Note #9d1B1e).   */
            if ((NetIP_AddrThisHostSubnetMask     != NET_IP_ADDR_NONE) &&
                (NetIP_AddrThisHostSubnetMaskHost != NET_IP_ADDR_NONE) &&
                (NetIP_AddrThisHostSubnetNet      != NET_IP_ADDR_NONE)) {
                if ((pbuf_hdr->IP_AddrDest & NetIP_AddrThisHostSubnetMask) ==
                                             NetIP_AddrThisHostSubnetNet ) {
                    if ((pbuf_hdr->IP_AddrDest & NetIP_AddrThisHostSubnetMaskHost) ==
                        (NET_IP_ADDR_BROADCAST & NetIP_AddrThisHostSubnetMaskHost)) {
                         ip_broadcast = DEF_YES;
                    }
                }
            }


        } else {
            ip_broadcast =  DEF_YES;
        }

                                                                    /* If NOT any this host's addrs (see Note #9d1A1) & */
        if (ip_broadcast != DEF_YES) {                              /* .. NOT any broadcast   addrs (see Note #9d1A2);  */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestCtr);
           *perr = NET_IP_ERR_INVALID_ADDR_DEST;                    /* .. rtn err / discard pkt     (see Note #9d1A).   */
            return;
        }
    }

                                                                    /* Chk invalid    broadcast      (see Note #9d2).   */
    rx_broadcast = DEF_BIT_IS_SET(pbuf_hdr->Flags, NET_BUF_FLAG_RX_BROADCAST);
    if (ip_broadcast == DEF_NO) {                                   /* If          IP broadcast rx'd but            ... */
        if (rx_broadcast != DEF_NO) {                               /* ... NOT net IF broadcast rx'd (see Note #9d2B1a),*/
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestBroadcastCtr);
           *perr = NET_IP_ERR_INVALID_ADDR_BROADCAST;               /* ... rtn err / discard pkt     (see Note #9d2B2). */
            return;
        }
    } else {                                                        /* If      NO  IP broadcast rx'd                ... */
        if (rx_broadcast != DEF_YES) {                              /* ... BUT net IF broadcast rx'd (see Note #9d2B1b),*/
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxDestBroadcastCtr);
           *perr = NET_IP_ERR_INVALID_ADDR_BROADCAST;               /* ... rtn err / discard pkt     (see Note #9d2B2). */
            return;
        }
        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatRxDestBroadcastCtr);
    }



/*$PAGE*/
                                                                /* ----------------- VALIDATE IP OPTS ----------------- */
    if (ip_hdr_len_tot > NET_IP_HDR_SIZE_MIN) {                 /* If hdr len > min, ...                                */
                                                                /* ... validate/process IP opts (see Note #11b).        */
        NetIP_RxPktValidateOpt((NET_BUF     *)pbuf,
                               (NET_BUF_HDR *)pbuf_hdr,
                               (NET_IP_HDR  *)pip_hdr,
                               (CPU_INT08U   )ip_hdr_len_tot,
                               (NET_ERR     *)perr);
        if (*perr != NET_IP_ERR_NONE) {
             return;
        }
    }



                                                                /* ----------------- UPDATE BUF CTRLS ----------------- */
#if 0                                                           /* See Note #3a.                                        */
    pbuf_hdr->IP_HdrLen = ip_hdr_len_tot;
#endif
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (pbuf_hdr->IP_HdrLen > pbuf_hdr->DataLen) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrDataLenCtr);
       *perr = NET_IP_ERR_INVALID_LEN_DATA;
        return;
    }
#endif
    pbuf_hdr->DataLen -= (NET_BUF_SIZE) pbuf_hdr->IP_HdrLen;
    protocol_ix        = (CPU_INT16U  )(pbuf_hdr->IP_HdrIx + pbuf_hdr->IP_HdrLen);
    switch (pip_hdr->Protocol) {
        case NET_IP_HDR_PROTOCOL_ICMP:
             pbuf_hdr->ProtocolHdrType   = NET_PROTOCOL_TYPE_ICMP;
             pbuf_hdr->ICMP_MsgIx        = protocol_ix;
             break;


        case NET_IP_HDR_PROTOCOL_UDP:
             pbuf_hdr->ProtocolHdrType   = NET_PROTOCOL_TYPE_UDP;
             pbuf_hdr->TCP_UDP_HdrDataIx = protocol_ix;
             break;


#ifdef  NET_TCP_MODULE_PRESENT
        case NET_IP_HDR_PROTOCOL_TCP:
             pbuf_hdr->ProtocolHdrType   = NET_PROTOCOL_TYPE_TCP;
             pbuf_hdr->TCP_UDP_HdrDataIx = protocol_ix;
             break;
#endif

        default:                                                /* See Note #7b.                                        */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrProtocolCtr);
            *perr = NET_IP_ERR_INVALID_PROTOCOL;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }



                                                                /* ------------------ ICMP RX HANDLER ----------------- */
    NetICMP_HandlerRx(pbuf);


   *perr = NET_IP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIP_RxPktValidateOpt()
*
* Description : (1) Validate & process received packet's IP options :
*
*                   (a) Get buffer to process IP options        See 'NetIP_RxPktValidate()  Note #1bC'
*                   (b) Decode/validate       IP options
*
*
* Argument(s) : pbuf                Pointer to network buffer that received IP packet.
*               ----                Argument checked   in NetIP_Rx().
*
*               pbuf_hdr            Pointer to network buffer header.
*               --------            Argument validated in NetIP_Rx().
*
*               pip_hdr             Pointer to received packet's IP header.
*               -------             Argument validated in NetIP_Rx()/NetIP_RxPktValidateBuf().
*
*               ip_hdr_len_size     Length  of received packet's IP header.
*               ---------------     Argument validated in NetIP_RxPktValidate().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                     IP options validated & processed.
*                               NET_IP_ERR_RX_OPT_BUF_NONE_AVAIL    No available buffers to process IP options.
*                               NET_IP_ERR_RX_OPT_BUF_WR            IP options failed to write to buffer.
*                               NET_IP_ERR_INVALID_OPT              Invalid IP option.
*                               NET_IP_ERR_INVALID_OPT_LEN          Invalid IP option length.
*                               NET_IP_ERR_INVALID_OPT_NBR          Invalid IP option number of same option.
*                               NET_IP_ERR_INVALID_OPT_END          Invalid IP option list ending.
*                               NET_IP_ERR_INVALID_OPT_FLAG         Invalid IP option flag.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_RxPktValidate().
*
* Note(s)     : (2) (a) See 'net_ip.h  IP HEADER OPTIONS DEFINES' for   supported IP options' summary.
*
*                   (b) See 'net_ip.c  Note #5'                   for unsupported IP options.
*
*               (3) RFC #1122, Section 3.2.1.8 lists the processing of the following IP options as optional :
*
*                   (a) Record Route                            RFC #1122, Section 3.2.1.8.(d) 
*                   (b) Internet Timestamp                      RFC #1122, Section 3.2.1.8.(e) 
*
*               (4) Each option's length MUST be multiples of NET_IP_HDR_OPT_SIZE_WORD octets so that "the 
*                   beginning of a subsequent option [aligns] on a 32-bit boundary" (RFC #791, Section 3.1
*                   'Options : No Operation').
*
*               (5) RFC #1122, Section 3.2.1.8.(c) prohibits "an IP header" from transmitting with "more 
*                   than one Source Route option".
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetIP_RxPktValidateOpt (NET_BUF      *pbuf,
                                      NET_BUF_HDR  *pbuf_hdr,
                                      NET_IP_HDR   *pip_hdr,
                                      CPU_INT08U    ip_hdr_len_size,
                                      NET_ERR      *perr)
{
    NET_ERR       err;
    NET_BUF      *popt_buf;
    NET_BUF_HDR  *popt_buf_hdr;
    CPU_INT08U   *popts;
    CPU_INT08U    opt_list_len_size;
    CPU_INT08U    opt_list_len_rem;
    CPU_INT08U    opt_len;
    CPU_INT08U    opt_nbr_src_routes;
    CPU_INT08U    opt_ix_err;
    CPU_INT16U    opt_ix;
    CPU_BOOLEAN   opt_err;
    CPU_BOOLEAN   opt_list_end;


    opt_list_len_size = ip_hdr_len_size - NET_IP_HDR_SIZE_MIN;  /* Calc opt list len size.                              */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ----------- VALIDATE IP HDR OPT LIST SIZE ---------- */
    if (opt_list_len_size > NET_IP_HDR_OPT_SIZE_MAX) {          /* If tot opt len > max opt size,           rtn err.    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrOptsCtr);
        NetICMP_TxMsgErr(pbuf,
                         NET_ICMP_MSG_TYPE_PARAM_PROB,
                         NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                         NET_ICMP_PTR_IX_IP_HDR_OPTS,
                        &err);
       *perr = NET_IP_ERR_INVALID_OPT_LEN;
        return;
    }

    if ((opt_list_len_size % NET_IP_HDR_OPT_SIZE_WORD) != 0) {  /* If tot opt len NOT multiple of opt size, rtn err.    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrOptsCtr);
        NetICMP_TxMsgErr(pbuf,
                         NET_ICMP_MSG_TYPE_PARAM_PROB,
                         NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                         NET_ICMP_PTR_IX_IP_HDR_OPTS,
                        &err);
       *perr = NET_IP_ERR_INVALID_OPT_LEN;
        return;
    }
#endif

                                                                /* ------------ GET BUF TO PROCESS IP OPTS ------------ */
    opt_ix   = NET_IP_OPT_IX_RX;
    popt_buf = NetBuf_Get((NET_BUF_SIZE) opt_list_len_size,
                          (NET_BUF_SIZE) opt_ix,
                          (CPU_INT16U  ) NET_BUF_FLAG_NONE,
                          (NET_ERR    *)&err);
    if ( err != NET_BUF_ERR_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrOptsBufNoneAvailCtr);
       *perr  = NET_IP_ERR_RX_OPT_BUF_NONE_AVAIL;
        return;
    }

    NetBuf_DataWr((NET_BUF    *) popt_buf,                      /* Copy IP opts from rx'd pkt to IP opt buf.            */
                  (NET_BUF_SIZE) opt_ix,
                  (NET_BUF_SIZE) opt_list_len_size,
                  (CPU_INT08U *)&pip_hdr->Opts[0],
                  (NET_ERR    *)&err);
    if ( err != NET_BUF_ERR_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrOptsBufWrCtr);
       *perr  = NET_IP_ERR_RX_OPT_BUF_WR;
        return;
    }
                                                                /* Init IP opt buf ctrls.                               */
    pbuf_hdr->IP_OptPtr           = (NET_BUF     *) popt_buf;
    popt_buf_hdr                  = (NET_BUF_HDR *)&popt_buf->Hdr;
    popt_buf_hdr->IP_HdrIx        = (CPU_INT16U   ) opt_ix;
    popt_buf_hdr->IP_HdrLen       = (CPU_INT16U   ) opt_list_len_size;
    popt_buf_hdr->TotLen          = (NET_BUF_SIZE ) popt_buf_hdr->IP_HdrLen;
    popt_buf_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_IP_V4_OPT;


/*$PAGE*/
    opt_err            =  DEF_NO;
    opt_list_end       =  DEF_NO;
    opt_nbr_src_routes =  0;

    popts              = (CPU_INT08U *)&popt_buf->Data[opt_ix];
    opt_list_len_rem   =  opt_list_len_size;

                                                                /* ------------- DECODE/VALIDATE IP OPTS -------------- */
    while (opt_list_len_rem > 0) {                              /* Process each opt in list (see Notes #2 & #4).        */
        switch (*popts) {                                                                                               
            case NET_IP_HDR_OPT_END_LIST:                       /* ------------------- END OPT LIST ------------------- */
                 opt_list_end = DEF_YES;                        /* Mark end of opt list.                                */
                 opt_len      = NET_IP_HDR_OPT_SIZE_WORD;
                 break;


            case NET_IP_HDR_OPT_NOP:                            /* --------------------- NOP OPT ---------------------- */
                 opt_len      = NET_IP_HDR_OPT_SIZE_WORD;
                 break;


            case NET_IP_HDR_OPT_ROUTE_SRC_LOOSE:                /* ---------------- SRC/REC ROUTE OPTS ---------------- */
            case NET_IP_HDR_OPT_ROUTE_SRC_STRICT:
            case NET_IP_HDR_OPT_ROUTE_REC:
                 if (opt_list_end == DEF_NO) {
                     if (opt_nbr_src_routes < 1) {
                         opt_nbr_src_routes++;
                         opt_err = NetIP_RxPktValidateOptRoute(popts, opt_list_len_rem, &opt_len, perr);

                     } else {                                   /* If > 1 src route opt, rtn err (see Note #5).         */
                        *perr    = NET_IP_ERR_INVALID_OPT_NBR;
                         opt_err = DEF_YES;
                     }
                 } else {                                       /* If opt found AFTER end of opt list, rtn err.         */
                    *perr    = NET_IP_ERR_INVALID_OPT_END;
                     opt_err = DEF_YES;
                 }
                 break;


            case NET_IP_HDR_OPT_TS:                             /* --------------------- TS OPTS ---------------------- */
                 if (opt_list_end == DEF_NO) {
                     opt_err = NetIP_RxPktValidateOptTS(popts, opt_list_len_rem, &opt_len, perr);

                 } else {                                       /* If opt found AFTER end of opt list, rtn err.         */
                    *perr    = NET_IP_ERR_INVALID_OPT_END;
                     opt_err = DEF_YES;
                 }
                 break;

#if 0                                                           /* ----------------- UNSUPPORTED OPTS ----------------- */
                                                                /* See Note #2b.                                        */
            case NET_IP_HDR_OPT_SECURITY:
            case NET_IP_HDR_OPT_SECURITY_EXTENDED:
                                                                /* 'break' intentionally omitted; do NOT move from the  */
                                                                /* ... following case : 'default'.                      */
#endif

            default:                                            /* ------------------- INVALID OPTS ------------------- */
                *perr    = NET_IP_ERR_INVALID_OPT;
                 opt_err = DEF_YES;
                 break;
        }

        if (opt_err == DEF_NO) {
            if (opt_list_len_rem >= opt_len) {
                opt_list_len_rem -= opt_len;
                popts            += opt_len;
            } else {                                            /* If rem opt list len NOT multiple of opt size, ...    */
               *perr              = NET_IP_ERR_INVALID_OPT_LEN; /* ... rtn err.                                         */
                opt_err           = DEF_YES;
            }
        }

        if (opt_err != DEF_NO) {                                /* If ANY opt errs, tx ICMP err msg.                    */
            opt_ix_err = NET_ICMP_PTR_IX_IP_HDR_OPTS + (opt_list_len_size - opt_list_len_rem);
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxHdrOptsCtr);
            NetICMP_TxMsgErr(pbuf,
                             NET_ICMP_MSG_TYPE_PARAM_PROB,
                             NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR,
                             opt_ix_err,
                            &err);
            return;
        }
    }

   *perr = NET_IP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetIP_RxPktValidateOptRoute()
*
* Description : (1) Validate & process Source Route options :
*
*                   (a) Convert ALL Source Route IP addresses from network-order to host-order
*                   (b) Add this host's IP address to Source Route
*
*               (2) See 'net_ip.h  IP SOURCE ROUTE OPTION DATA TYPE' for Source Route options summary.
*
*
* Argument(s) : popts               Pointer to Source Route option.
*               -----               Argument validated in NetIP_RxPktValidateOpt().
*
*               opt_list_len_rem    Remaining option list length (in octets).
*
*               popt_len            Pointer to variable that will receive the Source Route option length 
*               --------                (in octets).
*
*                                   Argument validated in NetIP_RxPktValidateOpt().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 Source Route IP option validated & processed.
*                               NET_IP_ERR_INVALID_OPT_LEN      Invalid IP option length.
*
* Return(s)   : DEF_NO,  NO Source Route option error.
*
*               DEF_YES, otherwise.
*
* Caller(s)   : NetIP_RxPktValidateOpt().
*
* Note(s)     : (3) If Source Route option appends this host's IP address to the source route, the IP 
*                   header check-sum is NOT re-calculated since the check-sum was previously validated 
*                   in NetIP_RxPktValidate() & is NOT required for further validation or processing.
*
*               (4) Default case already invalidated earlier in this function.  However, the default
*                   case is included as an extra precaution in case any of the IP receive options is
*                   incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
static  CPU_BOOLEAN  NetIP_RxPktValidateOptRoute (CPU_INT08U  *popts,
                                                  CPU_INT08U   opt_list_len_rem,
                                                  CPU_INT08U  *popt_len,
                                                  NET_ERR     *perr)
{
    NET_IP_OPT_SRC_ROUTE  *popt_route;
    NET_IP_ADDR            opt_addr;
    CPU_INT08U             opt_ptr;
    CPU_INT08U             opt_ix;


    popt_route = (NET_IP_OPT_SRC_ROUTE *)popts;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ----------------- VALIDATE OPT TYPE ---------------- */
    switch (popt_route->Type) {
        case NET_IP_HDR_OPT_ROUTE_SRC_LOOSE:
        case NET_IP_HDR_OPT_ROUTE_SRC_STRICT:
        case NET_IP_HDR_OPT_ROUTE_REC:
             break;


        default:
            *perr =  NET_IP_ERR_INVALID_OPT;
             return (DEF_YES);                                  /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif

                                                                /* ----------------- VALIDATE OPT LEN ----------------- */
    if (popt_route->Ptr > popt_route->Len) {                    /* If ptr exceeds opt len,              rtn err.        */
       *perr =  NET_IP_ERR_INVALID_OPT_LEN;
        return (DEF_YES);
    }
    
    if (popt_route->Len > opt_list_len_rem) {                   /* If opt len exceeds rem opt len,      rtn err.        */
       *perr =  NET_IP_ERR_INVALID_OPT_LEN;
        return (DEF_YES);
    }

    if ((popt_route->Len % NET_IP_HDR_OPT_SIZE_WORD) != 0) {    /* If opt len NOT multiple of opt size, rtn err.        */
       *perr =  NET_IP_ERR_INVALID_OPT_LEN;
        return (DEF_YES);
    }


    opt_ptr = NET_IP_OPT_ROUTE_PTR_ROUTE;
    opt_ix  = 0;

                                                                /* --------------- CONVERT TO HOST-ORDER -------------- */
    while (opt_ptr < popt_route->Ptr) {                         /* Convert ALL src route addrs to host-order.           */
        popt_route->Route[opt_ix]  = NET_UTIL_NET_TO_HOST_32(popt_route->Route[opt_ix]);
        opt_ptr                   += NET_IP_HDR_OPT_SIZE_WORD;
        opt_ix++;
    }

                                                                /* ------------------- INSERT ROUTE ------------------- */
    if (popt_route->Ptr < popt_route->Len) {                    /* If ptr < len, append this host addr to src route.    */
        switch (*popts) {
            case NET_IP_HDR_OPT_ROUTE_SRC_LOOSE:
            case NET_IP_HDR_OPT_ROUTE_REC:
                 opt_addr         = NetIP_AddrThisHost;
                 NET_UTIL_VAL_COPY_GET_NET_32(&popt_route->Route[opt_ix], &opt_addr);
                 popt_route->Ptr += NET_IP_HDR_OPT_SIZE_WORD;
                 break;


            case NET_IP_HDR_OPT_ROUTE_SRC_STRICT:
                 break;


            default:                                            /* See Note #4.                                         */
                *perr =  NET_IP_ERR_INVALID_OPT;
                 return (DEF_YES);                              /* Prevent 'break NOT reachable' compiler warning.      */
        }
    }


   *popt_len = popt_route->Len;                                 /* Rtn src route opt len.                               */
   *perr     = NET_IP_ERR_NONE;

    return (DEF_NO);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetIP_RxPktValidateOptTS()
*
* Description : (1) Validate & process Internet Timestamp options :
*
*                   (a) Convert ALL Internet Timestamps & Source Route IP addresses from network-order 
*                           to host-order
*                   (b) Add current Internet Timestamp & this host's IP address to Internet Timestamp
*
*               (2) See 'net_ip.h  IP INTERNET TIMESTAMP OPTION DATA TYPE' for Internet Timestamp options summary.
*
*
* Argument(s) : popts               Pointer to Internet Timestamp option.
*               -----               Argument validated in NetIP_RxPktValidateOpt().
*
*               opt_list_len_rem    Remaining option list length (in octets).
*
*               popt_len            Pointer to variable that will return the Internet Timestamp option length 
*               --------                (in octets).
*
*                                   Argument validated in NetIP_RxPktValidateOpt().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 Internet Timestamp IP option validated 
*                                                                   & processed.
*                               NET_IP_ERR_INVALID_OPT_LEN      Invalid IP option length.
*
* Return(s)   : DEF_NO,  NO Internet Timestamp option error.
*
*               DEF_YES, otherwise.
*
* Caller(s)   : NetIP_RxPktValidateOpt().
*
* Note(s)     : (3) If Internet Timestamp option appends the current Internet Timestamp &/or this host's 
*                   IP address to the Internet Timestamp, the IP header check-sum is NOT re-calculated 
*                   since the check-sum was previously validated in NetIP_RxPktValidate() & is NOT 
*                   required for further validation or processing.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  NetIP_RxPktValidateOptTS (CPU_INT08U  *popts,
                                               CPU_INT08U   opt_list_len_rem,
                                               CPU_INT08U  *popt_len,
                                               NET_ERR     *perr)
{
    NET_IP_OPT_TS        *popt_ts;
    NET_IP_OPT_TS_ROUTE  *popt_ts_route;
    NET_IP_ROUTE_TS      *proute_ts;
    CPU_INT08U            opt_ptr;
    CPU_INT08U            opt_ix;
    CPU_INT08U            opt_ts_flags;
    CPU_INT08U            opt_ts_ovf;
    NET_TS                opt_ts;
    NET_IP_ADDR           opt_addr;


    popt_ts = (NET_IP_OPT_TS *)popts;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ----------------- VALIDATE OPT TYPE ---------------- */
    switch (popt_ts->Type) {
        case NET_IP_HDR_OPT_TS:
             break;


        default:
            *perr =  NET_IP_ERR_INVALID_OPT;
             return (DEF_YES);                                  /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif

                                                                /* ----------------- VALIDATE OPT LEN ----------------- */
    if (popt_ts->Ptr > popt_ts->Len) {                          /* If ptr exceeds opt len,         rtn err.             */
       *perr =  NET_IP_ERR_INVALID_OPT_LEN;
        return (DEF_YES);
    }
    
    if (popt_ts->Len > opt_list_len_rem) {                      /* If opt len exceeds rem opt len, rtn err.             */
       *perr =  NET_IP_ERR_INVALID_OPT_LEN;
        return (DEF_YES);
    }


/*$PAGE*/
    opt_ptr      = NET_IP_OPT_TS_PTR_TS;
    opt_ix       = 0;
    opt_ts_flags = popt_ts->Ovf_Flags & NET_IP_OPT_TS_FLAG_MASK;

    switch (opt_ts_flags) {
        case NET_IP_OPT_TS_FLAG_TS_ONLY:
             if ((popt_ts->Len % NET_IP_HDR_OPT_SIZE_WORD) != 0) {  /* If opt len NOT multiple of opt size, rtn err.    */
                *perr =  NET_IP_ERR_INVALID_OPT_LEN;
                 return (DEF_YES);
             }

                                                                /* --------------- CONVERT TO HOST-ORDER -------------- */
             while (opt_ptr < popt_ts->Ptr) {                   /* Convert ALL TS's to host-order.                      */
                 popt_ts->TS[opt_ix]  = NET_UTIL_NET_TO_HOST_32(popt_ts->TS[opt_ix]);
                 opt_ptr             += NET_IP_HDR_OPT_SIZE_WORD;
                 opt_ix++;
             }

                                                                /* --------------------- INSERT TS -------------------- */
             if (popt_ts->Ptr < popt_ts->Len) {                 /* If ptr < len, append ts to list.                     */
                 opt_ts        = NetUtil_TS_Get();
                 NET_UTIL_VAL_COPY_GET_NET_32(&popt_ts->TS[opt_ix], &opt_ts);
                 popt_ts->Ptr += NET_IP_HDR_OPT_SIZE_WORD;

             } else {                                           /* Else inc & chk ovf ctr.                              */
                 opt_ts_ovf    = popt_ts->Ovf_Flags & NET_IP_OPT_TS_OVF_MASK;
                 opt_ts_ovf  >>= NET_IP_OPT_TS_OVF_SHIFT;
                 opt_ts_ovf++;

                 if (opt_ts_ovf > NET_IP_OPT_TS_OVF_MAX) {      /* If ovf ctr ovfs, rtn err.                            */
                    *perr =  NET_IP_ERR_INVALID_OPT_LEN;
                     return (DEF_YES);
                 }
             }
             break;


        case NET_IP_OPT_TS_FLAG_TS_ROUTE_REC:
        case NET_IP_OPT_TS_FLAG_TS_ROUTE_SPEC:
                                                                /* If opt len NOT multiple of opt size, rtn err.        */
             if ((popt_ts->Len % NET_IP_OPT_TS_ROUTE_SIZE) != NET_IP_HDR_OPT_SIZE_WORD) {   
                *perr =  NET_IP_ERR_INVALID_OPT_LEN;
                 return (DEF_YES);
             }

             popt_ts_route = (NET_IP_OPT_TS_ROUTE *)popts;
             proute_ts     = &popt_ts_route->Route_TS[0];

                                                                /* --------------- CONVERT TO HOST-ORDER -------------- */
             while (opt_ptr < popt_ts_route->Ptr) {             /* Convert ALL src route addrs & ts's to host-order.    */
                 proute_ts->Route[opt_ix]  = NET_UTIL_NET_TO_HOST_32(proute_ts->Route[opt_ix]);
                 proute_ts->TS[opt_ix]     = NET_UTIL_NET_TO_HOST_32(proute_ts->TS[opt_ix]);
                 opt_ptr                  += NET_IP_OPT_TS_ROUTE_SIZE;
                 opt_ix++;
             }

                                                                /* ---------------- INSERT SRC ROUTE/TS --------------- */
             if (popt_ts_route->Ptr < popt_ts_route->Len) {     /* If ptr < len, append src route addr & ts to list.    */
                 opt_addr            = NetIP_AddrThisHost;
                 opt_ts              = NetUtil_TS_Get();
                 NET_UTIL_VAL_COPY_GET_NET_32(&proute_ts->Route[opt_ix], &opt_addr);
                 NET_UTIL_VAL_COPY_GET_NET_32(&proute_ts->TS[opt_ix],    &opt_ts);
                 popt_ts_route->Ptr += NET_IP_OPT_TS_ROUTE_SIZE;

             } else {                                           /* Else inc & chk ovf ctr.                              */
                 opt_ts_ovf   = popt_ts->Ovf_Flags & NET_IP_OPT_TS_OVF_MASK;
                 opt_ts_ovf >>= NET_IP_OPT_TS_OVF_SHIFT;
                 opt_ts_ovf++;

                 if (opt_ts_ovf > NET_IP_OPT_TS_OVF_MAX) {      /* If ovf ctr ovfs, rtn err.                            */
                    *perr =  NET_IP_ERR_INVALID_OPT_LEN;
                     return (DEF_YES);
                 }
             }
             break;


        default:                                                /* If invalid/unknown TS flag, rtn err.                 */
            *perr =  NET_IP_ERR_INVALID_OPT_FLAG;
             return (DEF_YES);                                  /* Prevent 'break NOT reachable' compiler warning.      */
    }


   *popt_len = popt_ts->Len;                                    /* Rtn TS opt len.                                      */
   *perr     = NET_IP_ERR_NONE;

    return (DEF_NO);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_RxPktFragReasm()
*
* Description : (1) Reassemble any IP datagram fragments :
*
*                   (a) Determine if received IP packet is a fragment
*                   (b) Reassemble IP fragments, when possible
*
*               (2) (a) Received fragments are reassembled by sorting datagram fragments into fragment lists 
*                       (also known as 'Fragmented Datagrams') grouped by the following IP header fields :
*
*                       (1) Source      Address
*                       (2) Destination Address
*                       (3) Identification
*                       (4) Protocol
*
*                       See also Note #3a.
*
*                   (b) Fragment lists are linked to form a list of Fragment Lists.  In the diagram below,
*                       network buffers in vertical columns represent fragments in the same fragment list/
*                       Fragmented Datagram.  The head fragment of each fragment list/Fragmented Datagram
*                       is linked horizontally to form the list of Fragment Lists.
*
*                       (1) 'NetIP_FragReasmListsHead' points to the head of the Fragment Lists; 
*                           'NetIP_FragReasmListsTail' points to the tail of the Fragment Lists.  
*
*                       (2) Fragment buffers' 'PrevPrimListPtr' & 'NextPrimListPtr' doubly-link each fragment 
*                           list's head buffer to form the list of Fragment Lists.
*
*                       (3) Fragment buffer's 'PrevBufPtr'      & 'NextBufPtr'      doubly-link each fragment 
*                           in a fragment list.
*
*                   (c) For each received fragment, all fragment lists are searched in order to insert the 
*                       fragment into the appropriate fragment list--i.e. the fragment list with identical 
*                       fragment list IP header field values (see Note #2a).
*
*                   (d) If a received fragment is the first fragment with its specific fragment list IP header 
*                       field values, the received fragment starts a new fragment list which is added at the
*                       tail of the Fragment Lists.
*
*                       See also Note #3b2.
*
*                   (e) As fragments are received & processed into fragment lists, older fragment lists migrate 
*                       to the head of the Fragment Lists.  Once a fragment list is reassembled or discarded,
*                       it is removed from the Fragment Lists.
*
*                   (f) Fragment buffer size is irrelevant & ignored in the fragment reassembly procedure--
*                       i.e. the procedure functions correctly regardless of the buffer sizes used for any & 
*                       all received fragments.
*
*
*                                        |                   List of                     |
*                                        |<----------- Fragmented Datagrams ------------>|
*                                        |                (see Note #2b)                 |
*
*                               Oldest Fragmented Datagram                    New fragment lists
*                                   in Fragment Lists                          inserted at tail
*                                     (see Note #2e)                            (see Note #2d)
*
*                                           |        NextPrimListPtr                  |
*                                           |        (see Note #2b2)                  |
*                                           v                   |                     v
*                                                               |
*             ---         Head of        -------       -------  v    -------       -------   (see Note #2b1)
*              ^          Fragment  ---->|     |------>|     |------>|     |------>|     |
*              |           Lists         |     |       |     |       |     |       |     |       Tail of
*              |                         |     |<------|     |<------|     |<------|     |<----  Fragment
*              |      (see Note #2b1)    |     |       |     |  ^    |     |       |     |        Lists
*              |                         |     |       |     |  |    |     |       |     |
*              |                         -------       -------  |    -------       -------
*              |                           | ^                  |      | ^
*              |                           | |       PrevPrimListPtr   | |
*              |                           v |       (see Note #2b2)   v |
*              |                         -------                     -------
*                                        |     |                     |     |
*     Fragments in the same              |     |                     |     |
*      Fragmented Datagram               |     |                     |     |
*        (see Note #2b)                  |     |                     |     |
*                                        |     |                     |     |        Fragments in a fragment
*              |                         -------                     -------        list may use different
*              |                           | ^                         | ^           size network buffers
*              |           NextBufPtr ---> | | <--- PrevBufPtr         | |              (see Note #2f)
*              |        (see Note #2b3)    v |   (see Note #2b3)       v |
*              |                         -------                     -------        The last fragment in a
*              |                         |     |                     |     |  <--- fragment list may likely
*              |                         |     |                     |     |       use a smaller buffer size
*              |                         |     |                     -------
*              |                         |     |
*              v                         |     |
*             ---                        -------
*
*
*$PAGE*
* Argument(s) : pbuf        Pointer to network buffer that received IP packet.
*               ----        Argument checked   in NetIP_Rx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetIP_Rx().
*
*               pip_hdr     Pointer to received packet's IP header.
*               -------     Argument checked   in NetIP_RxPktValidate().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_RX_FRAG_NONE         Datagram complete; NOT a fragment (see Note #3b1B).
*
*                                                               -------- RETURNED BY NetIP_RxPktFragListAdd() : ---------
*                                                               ------- RETURNED BY NetIP_RxPktFragListInsert() : -------
*                               NET_IP_ERR_RX_FRAG_REASM        Fragments in reassembly progress.
*                               NET_IP_ERR_RX_FRAG_DISCARD      Fragment &/or datagram discarded.
*                               NET_IP_ERR_RX_FRAG_OFFSET       Invalid fragment offset.
*                               NET_IP_ERR_RX_FRAG_SIZE         Invalid fragment size.
*                               NET_IP_ERR_RX_FRAG_LEN_TOT      Invalid fragment   datagram total IP length.
*
*                                                               ------- RETURNED BY NetIP_RxPktFragListInsert() : -------
*                               NET_IP_ERR_RX_FRAG_COMPLETE     Datagram complete; fragments reassembled (see Note #3b3).
*                               NET_IP_ERR_RX_FRAG_SIZE_TOT     Invalid fragmented datagram total size.
*
* Return(s)   : Pointer to reassembled datagram, if fragment reassembly complete.
*
*               Pointer to NULL,                 if fragment reassembly in progress.
*
*               Pointer to fragment buffer,      for any fragment discard error.
*
* Caller(s)   : NetIP_Rx().
*
* Note(s)     : (3) (a) RFC #791, Section 3.2 'Fragmentation & Reassembly' states that the following IP header
*                       fields are "used together ... to identify datagram fragments for reassembly" :
*
*                       (1) "Internet identification field (ID)", ...
*                       (2) "source"     address,                 ...
*                       (3) "destination address", &              ...
*                       (4) "protocol field".
*
*                   (b) RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly Procedure' 
*                       states that :
*
*                       (1) (A) "If ... both [of the following IP header fields] ... are zero" ... :
*                               (1) "the fragment offset and" ...
*                               (2) "the more fragments";     ...
*
*                           (B) "Then ... this is a whole datagram ... and the datagram is forwarded to the 
*                                next step in datagram processing."
*
*                       (2) "If no other fragment with [identical] buffer identifier[s] is [available] then
*                            [new] reassembly resources are allocated."
*
*                           See also 'NetIP_RxPktFragListAdd()  Note #2'.
*
*                       (3) When a "fragment [finally] completes the datagram ... then the datagram is sent
*                           to the next step in datagram processing".
*
*               (4) (a) Fragment lists are accessed by :
*
*                       (1) NetIP_RxPktFragReasm()
*                       (2) Fragment list's 'TMR->Obj' pointer      during execution of NetIP_RxPktFragTimeout()
*
*                   (b) Since the primary tasks of the network protocol suite are prevented from running 
*                       concurrently (see 'net.h  Note #2'), it is NOT necessary to protect the shared 
*                       resources of the fragment lists since no asynchronous access from other network
*                       tasks is possible.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_BUF  *NetIP_RxPktFragReasm (NET_BUF      *pbuf,
                                        NET_BUF_HDR  *pbuf_hdr,
                                        NET_IP_HDR   *pip_hdr,
                                        NET_ERR      *perr)
{
    CPU_BOOLEAN   frag;
    CPU_BOOLEAN   frag_done;
    CPU_BOOLEAN   ip_flag_frags_more;
    CPU_INT16U    ip_flags;
    CPU_INT16U    frag_offset;
    CPU_INT16U    frag_size;
    NET_BUF      *pfrag;
    NET_BUF      *pfrag_list;
    NET_BUF_HDR  *pfrag_list_buf_hdr;
    NET_IP_HDR   *pfrag_list_ip_hdr;


                                                                /* -------------- CHK FRAG REASM REQUIRED ------------- */
    frag = DEF_NO;

    ip_flags           = pbuf_hdr->IP_Flags_FragOffset & NET_IP_HDR_FLAG_MASK;
    ip_flag_frags_more = DEF_BIT_IS_SET(ip_flags, NET_IP_HDR_FLAG_FRAG_MORE);
    if (ip_flag_frags_more != DEF_NO) {                         /* If 'More Frags' set (see Note #3b1A2), ...           */
        frag  = DEF_YES;                                        /* ... mark as frag.                                    */
    }

    frag_offset = pbuf_hdr->IP_Flags_FragOffset & NET_IP_HDR_FRAG_OFFSET_MASK;
    if (frag_offset != NET_IP_HDR_FRAG_OFFSET_NONE) {           /* If frag offset != 0 (see Note #3b1A1), ...           */
        frag  = DEF_YES;                                        /* ... mark as frag.                                    */
    }

    if (frag != DEF_YES) {                                      /* If pkt NOT a frag, ...                               */
       *perr  = NET_IP_ERR_RX_FRAG_NONE;
        return (pbuf);                                          /* ... rtn non-frag'd datagram (see Note #3b1B).        */
    }


    NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatRxFragCtr);


                                                                /* ------------------- REASM FRAGS -------------------- */
    frag_size  = pbuf_hdr->IP_TotLen - pbuf_hdr->IP_HdrLen;
    pfrag_list = NetIP_FragReasmListsHead;
    frag_done  = DEF_NO;

    while (frag_done == DEF_NO) {                               /* Insert frag into a frag list.                        */

        if (pfrag_list != (NET_BUF *)0) {                       /* Srch ALL existing frag lists first (see Note #2c).   */
            pfrag_list_buf_hdr =               &pfrag_list->Hdr;
            pfrag_list_ip_hdr  = (NET_IP_HDR *)&pfrag_list->Data[pfrag_list_buf_hdr->IP_HdrIx];

                                                                                /* If frag & this frag list's    ...    */
            if (pbuf_hdr->IP_AddrSrc    == pfrag_list_buf_hdr->IP_AddrSrc) {    /* ... src  addr (see Note #2a1) ...    */
                if (pbuf_hdr->IP_AddrDest == pfrag_list_buf_hdr->IP_AddrDest) { /* ... dest addr (see Note #2a2) ...    */
                    if (pbuf_hdr->IP_ID     == pfrag_list_buf_hdr->IP_ID) {     /* ... ID        (see Note #2a3) ...    */
                        if (pip_hdr->Protocol == pfrag_list_ip_hdr->Protocol) { /* ... protocol  (see Note #2a4) ...    */
                                                                                /* ... fields identical,         ...    */
                            pfrag     = NetIP_RxPktFragListInsert(pbuf,         /* ... insert frag onto frag list.      */
                                                                  pbuf_hdr,
                                                                  ip_flags,
                                                                  frag_offset,
                                                                  frag_size,
                                                                  pfrag_list,
                                                                  perr);
                            frag_done = DEF_YES;
                        }
                    }
                }
            }

            if (frag_done != DEF_YES) {                         /* If NOT done, adv to next frag list.                  */
                pfrag_list = pfrag_list_buf_hdr->NextPrimListPtr;
            }

        } else {                                                /* Else add new frag list (see Note #2d).               */
            pfrag     = NetIP_RxPktFragListAdd(pbuf,
                                               pbuf_hdr,
                                               ip_flags,
                                               frag_offset,
                                               frag_size,
                                               perr);
            frag_done = DEF_YES;
        }
    }

    return (pfrag);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIP_RxPktFragListAdd()
*
* Description : (1) Add fragment as new fragment list at end of Fragment Lists :
*
*                   (a) Get    fragment reassembly timer
*                   (b) Insert fragment into Fragment Lists
*                   (c) Update fragment list reassembly calculations
*
*
* Argument(s) : pbuf            Pointer to network buffer that received fragment.
*               ----            Argument checked   in NetIP_Rx().
*
*               pbuf_hdr        Pointer to network buffer header.
*               --------        Argument validated in NetIP_Rx().
*
*               frag_ip_flags   Fragment IP header flags.
*               -------------   Argument validated in NetIP_RxPktFragReasm().
*
*               frag_offset     Fragment offset.
*
*               frag_size       Fragment size (in octets).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_RX_FRAG_REASM        Fragment reassembly in progress.
*                               NET_IP_ERR_RX_FRAG_DISCARD      Fragment discarded.
*                               NET_IP_ERR_RX_FRAG_OFFSET       Invalid  fragment offset.
*                               NET_IP_ERR_RX_FRAG_SIZE         Invalid  fragment size.
*
*                                                               - RETURNED BY NetIP_RxPktFragListUpdate() : -
*                               NET_IP_ERR_RX_FRAG_LEN_TOT      Invalid fragment datagram total IP length.
*
* Return(s)   : Pointer to NULL,            if fragment added as new fragment list.
*
*               Pointer to fragment buffer, for any fragment discard error.
*
* Caller(s)   : NetIP_RxPktFragReasm().
*
* Note(s)     : (2) (a) RFC #791, Section 3.2 'Fragmentation and Reassembly' states that "if an internet 
*                       datagram is fragmented" :
*
*                       (1) (A) "Fragments are counted in units of 8 octets."
*                           (B) "The minimum fragment is 8 octets."
*
*                       (2) (A) However, this CANNOT apply "if this is the last fragment" ...
*                               (1) "(that is the more fragments field is zero)";         ...
*                           (B) Which may be of ANY size.
*
*                       See also 'net_ip.h  IP FRAGMENTATION DEFINES  Note #1a'.
*
*                   (b) RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly Procedure' 
*                       states that "if no other fragment with [identical] buffer identifier[s] is [available] 
*                       then [new] reassembly resources are allocated".
*
*               (3) During fragment list insertion, some fragment buffer controls were previously initialized
*                   in NetBuf_Get() when the packet was received at the network interface layer.  These buffer
*                   controls do NOT need to be re-initialized but are shown for completeness.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_BUF  *NetIP_RxPktFragListAdd (NET_BUF      *pbuf,
                                          NET_BUF_HDR  *pbuf_hdr,
                                          CPU_INT16U    frag_ip_flags,
                                          CPU_INT16U    frag_offset,
                                          CPU_INT16U    frag_size,
                                          NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR         cpu_sr;
#endif
    CPU_BOOLEAN    ip_flag_frags_more;
    CPU_INT16U     frag_size_min;
    NET_TMR_TICK   timeout_tick;
    NET_ERR        tmr_err;
    NET_BUF       *pfrag;
    NET_BUF_HDR   *pfrag_list_tail_buf_hdr;


                                                                /* ------------------- VALIDATE FRAG ------------------ */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (frag_offset > NET_IP_HDR_FRAG_OFFSET_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragOffsetCtr);
       *perr =  NET_IP_ERR_RX_FRAG_OFFSET;
        return (pbuf);
    }
#endif

    ip_flag_frags_more =  DEF_BIT_IS_SET(frag_ip_flags, NET_IP_HDR_FLAG_FRAG_MORE);
    frag_size_min      = (ip_flag_frags_more == DEF_YES) ? NET_IP_FRAG_SIZE_MIN_FRAG_MORE
                                                         : NET_IP_FRAG_SIZE_MIN_FRAG_LAST;
    if (frag_size < frag_size_min) {                            /* See Note #2a.                                        */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragSizeCtr);
       *perr =  NET_IP_ERR_RX_FRAG_SIZE;
        return (pbuf);
    }

    if (frag_size > NET_IP_FRAG_SIZE_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragSizeCtr);
       *perr =  NET_IP_ERR_RX_FRAG_SIZE;
        return (pbuf);
    }


                                                                /* ------------------- GET FRAG TMR ------------------- */
    CPU_CRITICAL_ENTER();
    timeout_tick     = NetIP_FragReasmTimeout_tick;
    CPU_CRITICAL_EXIT();
    pbuf_hdr->TmrPtr = NetTmr_Get((void       *) pbuf, 
                                  (CPU_FNCT_PTR) NetIP_RxPktFragTimeout,
                                  (NET_TMR_TICK) timeout_tick,
                                  (CPU_INT16U  ) NET_TMR_FLAG_NONE,
                                  (NET_ERR    *)&tmr_err);
    if (tmr_err != NET_TMR_ERR_NONE) {                          /* If tmr unavail, discard frag.                        */
       *perr =  NET_IP_ERR_RX_FRAG_DISCARD;
        return (pbuf);
    }


                                                                /* ------------ INSERT FRAG INTO FRAG LISTS ----------- */
    if (NetIP_FragReasmListsTail != (NET_BUF *)0) {             /* If frag lists NOT empty, insert @ tail.              */
        pbuf_hdr->PrevPrimListPtr                = (NET_BUF     *) NetIP_FragReasmListsTail;
        pfrag_list_tail_buf_hdr                  = (NET_BUF_HDR *)&NetIP_FragReasmListsTail->Hdr;
        pfrag_list_tail_buf_hdr->NextPrimListPtr = (NET_BUF     *) pbuf;
        NetIP_FragReasmListsTail                 = (NET_BUF     *) pbuf;

    } else {                                                    /* Else add frag as first frag list.                    */
        NetIP_FragReasmListsHead                 = (NET_BUF     *) pbuf;
        NetIP_FragReasmListsTail                 = (NET_BUF     *) pbuf;
        pbuf_hdr->PrevPrimListPtr                = (NET_BUF     *) 0;
    }

#if 0                                                           /* Init'd in NetBuf_Get() [see Note #3].                */
    pbuf_hdr->NextPrimListPtr = (NET_BUF *)0;
    pbuf_hdr->PrevBufPtr      = (NET_BUF *)0;
    pbuf_hdr->NextBufPtr      = (NET_BUF *)0;
    pbuf_hdr->IP_FragSizeTot  =  NET_IP_FRAG_SIZE_NONE;
    pbuf_hdr->IP_FragSizeCur  =  0;
#endif


                                                                /* ----------------- UPDATE FRAG CALCS ---------------- */
    NetIP_RxPktFragListUpdate(pbuf,
                              pbuf_hdr,
                              frag_ip_flags,
                              frag_offset,
                              frag_size,
                              perr);
    if (*perr !=  NET_IP_ERR_NONE) {
         return ((NET_BUF *)0);
    }


   *perr  =  NET_IP_ERR_RX_FRAG_REASM;
    pfrag = (NET_BUF *)0;

    return (pfrag);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetIP_RxPktFragListInsert()
*
* Description : Insert fragment into corresponding fragment list.
*
*               (1) (a) Fragments are sorted into fragment lists by fragment offset.
*
*                       See Notes #2a3 & #2b3.
*
*                   (b) Although RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly 
*                       Procedure' states that "in the case that two or more fragments contain the same data
*                       either identically or through a partial overlap ... [the IP fragmentation reassembly
*                       algorithm should] use the more recently arrived copy in the data buffer and datagram
*                       delivered"; in order to avoid the complexity of sequencing received fragments with
*                       duplicate data that overlap multiple previously-received fragments' data, duplicate
*                       & overlap fragments are discarded :
*
*                       (1) Duplicate fragments are discarded.  A fragment is a duplicate of a fragment already
*                           in the fragment list if both fragments have identical fragment offset & size values.
*
*                       (2) Overlap fragments are discarded & the entire Fragmented Datagram is also discarded.
*                           A fragment is an overlap fragment if any portion of its fragment data overlaps any
*                           other fragment's data :
*
*                           (A) [Fragment offset]  <  [(Any other fragment offset * FRAG_OFFSET_SIZE) + 
*                                                       Any other fragment size                       ]
*
*                           (B) [(Fragment offset * FRAG_OFFSET_SIZE) + Fragment size]  >  [Any other fragment offset]
*
*
*                       See also 'net_tcp.c  NetTCP_RxPktConnHandlerRxQ_Conn()  Note #2a3'.
*
*
* Argument(s) : pbuf            Pointer to network buffer that received fragment.
*               ----            Argument checked   in NetIP_Rx().
*
*               pbuf_hdr        Pointer to network buffer header.
*               --------        Argument validated in NetIP_Rx().
*
*               frag_ip_flags   Fragment IP header flags.
*               -------------   Argument validated in NetIP_RxPktFragReasm().
*
*               frag_offset     Fragment offset.
*
*               frag_size       Fragment size (in octets).
*
*               pfrag_list      Pointer to fragment list head buffer.
*               ----------      Argument validated in NetIP_RxPktFragReasm().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_RX_FRAG_REASM        Fragment reassembly in progress.
*                               NET_IP_ERR_RX_FRAG_DISCARD      Fragment discarded.
*                               NET_IP_ERR_RX_FRAG_OFFSET       Invalid  fragment offset.
*                               NET_IP_ERR_RX_FRAG_SIZE         Invalid  fragment size.
*
*                                                               -- RETURNED BY NetIP_RxPktFragListChkComplete() : -
*                               NET_IP_ERR_RX_FRAG_COMPLETE     Datagram complete; fragments reassembled.
*                               NET_IP_ERR_RX_FRAG_SIZE_TOT     Invalid fragmented datagram total size.
*
*                                                               ---- RETURNED BY NetIP_RxPktFragListUpdate() : ----
*                                                               -- RETURNED BY NetIP_RxPktFragListChkComplete() : -
*                               NET_IP_ERR_RX_FRAG_LEN_TOT      Invalid fragment   datagram total IP length.
*
* Return(s)   : Pointer to reassembled datagram, if fragment reassembly complete.
*
*               Pointer to NULL,                 if fragment reassembly in progress.
*
*               Pointer to fragment buffer,      for any fragment discard error.
*
* Caller(s)   : NetIP_RxPktFragReasm().
*$PAGE*
* Note(s)     : (2) (a) RFC #791, Section 3.2 'Fragmentation and Reassembly' states that "if an internet 
*                       datagram is fragmented" :
*
*                       (1) (A) (1) "Fragments are counted in units of 8 octets."
*                               (2) "The minimum fragment is 8 octets."
*
*                           (B) (1) However, this CANNOT apply "if this is the last fragment" ...
*                                   (a) "(that is the more fragments field is zero)";         ...
*                               (2) Which may be of ANY size.
*
*                           See also 'net_ip.h  IP FRAGMENTATION DEFINES  Note #1a'.
*
*                       (2) "Fragments ... data portion must be ... on 8 octet boundaries."
*
*                       (3) "The Fragment Offset field identifies the fragment location, relative to the 
*                            beginning of the original unfragmented datagram."
*
*                   (b) RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly Procedure' 
*                       states that :
*
*                       (1) (A) "If this is the first fragment"              ...
*                               (1) "(that is the fragment offset is zero)", ...
*                           (B) "this header is placed in the header buffer."
*
*                       (2) "Some [IP] options are copied [in every fragment], but others remain with 
*                            the first fragment only."
*
*                       (3) "The data from the fragment is placed in the data buffer according to its 
*                            fragment offset and length."
*
*                            See also Note #2a3.
*
*               (3) Assumes ALL fragments in fragment lists have previously been validated for buffer &
*                   IP header fields.
*
*               (4) During fragment list insertion, some fragment buffer controls were previously
*                   initialized in NetBuf_Get() when the packet was received at the network interface
*                   layer.  These buffer controls do NOT need to be re-initialized but are shown for
*                   completeness.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_BUF  *NetIP_RxPktFragListInsert (NET_BUF      *pbuf,
                                             NET_BUF_HDR  *pbuf_hdr,
                                             CPU_INT16U    frag_ip_flags,
                                             CPU_INT16U    frag_offset,
                                             CPU_INT16U    frag_size,
                                             NET_BUF      *pfrag_list,
                                             NET_ERR      *perr)
{
    CPU_BOOLEAN   ip_flag_frags_more;
    CPU_BOOLEAN   frag_insert_done;
    CPU_BOOLEAN   frag_list_discard;
    CPU_INT16U    frag_offset_actual;
    CPU_INT16U    frag_list_cur_frag_offset;
    CPU_INT16U    frag_list_cur_frag_offset_actual;
    CPU_INT16U    frag_list_prev_frag_offset;
    CPU_INT16U    frag_list_prev_frag_offset_actual;
    CPU_INT16U    frag_size_min;
    CPU_INT16U    frag_size_cur;
    NET_BUF      *pfrag;
    NET_BUF      *pfrag_list_prev_buf;
    NET_BUF      *pfrag_list_cur_buf;
    NET_BUF      *pfrag_list_prev_list;
    NET_BUF      *pfrag_list_next_list;
    NET_BUF_HDR  *pfrag_list_buf_hdr;
    NET_BUF_HDR  *pfrag_list_prev_buf_hdr;
    NET_BUF_HDR  *pfrag_list_cur_buf_hdr;
    NET_BUF_HDR  *pfrag_list_prev_list_buf_hdr;
    NET_BUF_HDR  *pfrag_list_next_list_buf_hdr;
    NET_TMR      *ptmr;
    NET_ERR       err;


                                                                /* ------------------- VALIDATE FRAG ------------------ */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (frag_offset > NET_IP_HDR_FRAG_OFFSET_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragOffsetCtr);
       *perr =  NET_IP_ERR_RX_FRAG_OFFSET;
        return (pbuf);
    }
#endif

    ip_flag_frags_more =  DEF_BIT_IS_SET(frag_ip_flags, NET_IP_HDR_FLAG_FRAG_MORE);
    frag_size_min      = (ip_flag_frags_more == DEF_YES) ? NET_IP_FRAG_SIZE_MIN_FRAG_MORE
                                                         : NET_IP_FRAG_SIZE_MIN_FRAG_LAST;
    if (frag_size < frag_size_min) {                            /* See Note #2a1.                                       */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragSizeCtr);
       *perr =  NET_IP_ERR_RX_FRAG_SIZE;
        return (pbuf);
    }

    if (frag_size > NET_IP_FRAG_SIZE_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragSizeCtr);
       *perr =  NET_IP_ERR_RX_FRAG_SIZE;
        return (pbuf);
    }


/*$PAGE*/
                                                                        /* ------- INSERT FRAG INTO FRAG LISTS -------- */
    frag_insert_done       =  DEF_NO;

    pfrag_list_cur_buf     =  pfrag_list;
    pfrag_list_cur_buf_hdr = &pfrag_list_cur_buf->Hdr;

    while (frag_insert_done == DEF_NO) {

        frag_list_cur_frag_offset = pfrag_list_cur_buf_hdr->IP_Flags_FragOffset & NET_IP_HDR_FRAG_OFFSET_MASK;
        if (frag_offset > frag_list_cur_frag_offset) {                  /* While frag offset > cur frag offset, ...     */

            if (pfrag_list_cur_buf_hdr->NextBufPtr != (NET_BUF *)0) {   /* ... adv to next frag in list.                */
                pfrag_list_cur_buf     =  pfrag_list_cur_buf_hdr->NextBufPtr;
                pfrag_list_cur_buf_hdr = &pfrag_list_cur_buf->Hdr;

            } else {                                                    /* If @ last frag in list, append frag @ end.   */
                frag_offset_actual               =   frag_offset                      * NET_IP_FRAG_SIZE_UNIT;
                frag_list_cur_frag_offset_actual = ( frag_list_cur_frag_offset        * NET_IP_FRAG_SIZE_UNIT            )
                                                 + (pfrag_list_cur_buf_hdr->IP_TotLen - pfrag_list_cur_buf_hdr->IP_HdrLen);

                if (frag_offset_actual >= frag_list_cur_frag_offset_actual) {   /* If frag does NOT overlap, ...        */
                                                                                /* ... append @ end of frag list.       */
                    pbuf_hdr->PrevBufPtr               = (NET_BUF *)pfrag_list_cur_buf;
#if 0                                                                   /* Init'd in NetBuf_Get() [see Note #4].        */
                    pbuf_hdr->NextBufPtr               = (NET_BUF *)0;
                    pbuf_hdr->PrevPrimListPtr          = (NET_BUF *)0;
                    pbuf_hdr->NextPrimListPtr          = (NET_BUF *)0;
                    pbuf_hdr->TmrPtr                   = (NET_TMR *)0;
                    pbuf_hdr->IP_FragSizeTot           =  NET_IP_FRAG_SIZE_NONE;
                    pbuf_hdr->IP_FragSizeCur           =  0;
#endif

                    pfrag_list_cur_buf_hdr->NextBufPtr = (NET_BUF *)pbuf;


                    pfrag_list_buf_hdr = &pfrag_list->Hdr;
                    NetIP_RxPktFragListUpdate(pfrag_list,               /* Update frag list reasm calcs.                */
                                              pfrag_list_buf_hdr,
                                              frag_ip_flags,
                                              frag_offset,
                                              frag_size,
                                              perr);
                    if (*perr !=  NET_IP_ERR_NONE) {
                         return ((NET_BUF *)0);
                    }
                                                                        /* Chk    frag list reasm complete.             */
                    pfrag = NetIP_RxPktFragListChkComplete(pfrag_list,
                                                           pfrag_list_buf_hdr,
                                                           perr);

                } else {                                                /* Else discard overlap frag & datagram.        */
                    NetIP_RxPktFragListDiscard(pfrag_list, DEF_YES, &err);
                    NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragDiscardedCtr);
                    pfrag = pbuf;
                   *perr  = NET_IP_ERR_RX_FRAG_DISCARD;
                }

                frag_insert_done = DEF_YES;
            }


        } else if (frag_offset < frag_list_cur_frag_offset) {           /* If frag offset < cur frag offset, ...        */
                                                                        /* ... insert frag into frag list.              */
            frag_list_discard                =  DEF_NO;

            frag_offset_actual               = (frag_offset               * NET_IP_FRAG_SIZE_UNIT) + frag_size;
            frag_list_cur_frag_offset_actual =  frag_list_cur_frag_offset * NET_IP_FRAG_SIZE_UNIT;

            if (frag_offset_actual > frag_list_cur_frag_offset_actual) {/* If frag overlaps with next frag, ...         */
                frag_list_discard  = DEF_YES;                           /* ... discard frag & datagram (see Note #1b2). */

            } else if (pfrag_list_cur_buf_hdr->PrevBufPtr != (NET_BUF *)0) {
                pfrag_list_prev_buf               =  pfrag_list_cur_buf_hdr->PrevBufPtr;
                pfrag_list_prev_buf_hdr           = &pfrag_list_prev_buf->Hdr;
                
                frag_offset_actual                =   frag_offset * NET_IP_FRAG_SIZE_UNIT;

                frag_list_prev_frag_offset        =  pfrag_list_prev_buf_hdr->IP_Flags_FragOffset & NET_IP_HDR_FRAG_OFFSET_MASK;
                frag_list_prev_frag_offset_actual = ( frag_list_prev_frag_offset        * NET_IP_FRAG_SIZE_UNIT             )
                                                  + (pfrag_list_prev_buf_hdr->IP_TotLen - pfrag_list_prev_buf_hdr->IP_HdrLen);
                                                                        /* If frag overlaps with prev frag, ...         */
                if (frag_offset_actual < frag_list_prev_frag_offset_actual) {
                    frag_list_discard  = DEF_YES;                       /* ... discard frag & datagram (see Note #1b2). */
                }
            }
/*$PAGE*/
            if (frag_list_discard == DEF_NO) {                          /* If frag does NOT overlap, ...                */
                                                                        /* ... insert into frag list.                   */
                pbuf_hdr->PrevBufPtr = pfrag_list_cur_buf_hdr->PrevBufPtr;
                pbuf_hdr->NextBufPtr = pfrag_list_cur_buf;

                if (pbuf_hdr->PrevBufPtr != (NET_BUF *)0) {             /* Insert pbuf between prev & cur bufs.         */
                    pfrag_list_prev_buf                     =  pbuf_hdr->PrevBufPtr;
                    pfrag_list_prev_buf_hdr                 = &pfrag_list_prev_buf->Hdr;

                    pfrag_list_prev_buf_hdr->NextBufPtr     =  pbuf;
                    pfrag_list_cur_buf_hdr->PrevBufPtr      =  pbuf;

#if 0                                                                   /* Init'd in NetBuf_Get() [see Note #4].        */
                    pbuf_hdr->PrevPrimListPtr               = (NET_BUF *)0;
                    pbuf_hdr->NextPrimListPtr               = (NET_BUF *)0;
                    pbuf_hdr->TmrPtr                        = (NET_TMR *)0;
                    pbuf_hdr->IP_FragSizeTot                =  NET_IP_FRAG_SIZE_NONE;
                    pbuf_hdr->IP_FragSizeCur                =  0;
#endif

                } else {                                                /* Else pbuf is new frag list head.             */
                    pfrag_list                              =  pbuf;
                                                                        /* Move frag list head info to cur buf ...      */
                                                                        /* ... (see Note #2b1).                         */
                    pbuf_hdr->PrevPrimListPtr               =  pfrag_list_cur_buf_hdr->PrevPrimListPtr;
                    pbuf_hdr->NextPrimListPtr               =  pfrag_list_cur_buf_hdr->NextPrimListPtr;
                    pbuf_hdr->TmrPtr                        =  pfrag_list_cur_buf_hdr->TmrPtr;
                    pbuf_hdr->IP_FragSizeTot                =  pfrag_list_cur_buf_hdr->IP_FragSizeTot;
                    pbuf_hdr->IP_FragSizeCur                =  pfrag_list_cur_buf_hdr->IP_FragSizeCur;

                    pfrag_list_cur_buf_hdr->PrevBufPtr      = (NET_BUF *)pbuf;
                    pfrag_list_cur_buf_hdr->PrevPrimListPtr = (NET_BUF *)0;
                    pfrag_list_cur_buf_hdr->NextPrimListPtr = (NET_BUF *)0;
                    pfrag_list_cur_buf_hdr->TmrPtr          = (NET_TMR *)0;
                    pfrag_list_cur_buf_hdr->IP_FragSizeTot  =  NET_IP_FRAG_SIZE_NONE;
                    pfrag_list_cur_buf_hdr->IP_FragSizeCur  =  0;

                                                                        /* Point tmr            to new frag list head.  */
                    ptmr                                    = (NET_TMR *)pbuf_hdr->TmrPtr;
                    ptmr->Obj                               = (void    *)pbuf;

                                                                        /* Point prev frag list to new frag list head.  */
                    pfrag_list_prev_list = pbuf_hdr->PrevPrimListPtr;
                    if (pfrag_list_prev_list != (NET_BUF *)0) {
                        pfrag_list_prev_list_buf_hdr                  = &pfrag_list_prev_list->Hdr;
                        pfrag_list_prev_list_buf_hdr->NextPrimListPtr =  pbuf;
                    } else {
                        NetIP_FragReasmListsHead                      =  pbuf;
                    }

                                                                        /* Point next frag list to new frag list head.  */
                    pfrag_list_next_list = pbuf_hdr->NextPrimListPtr;
                    if (pfrag_list_next_list != (NET_BUF *)0) {
                        pfrag_list_next_list_buf_hdr                  = &pfrag_list_next_list->Hdr;
                        pfrag_list_next_list_buf_hdr->PrevPrimListPtr =  pbuf;
                    } else {
                        NetIP_FragReasmListsTail                      =  pbuf;
                    }
                }

                pfrag_list_buf_hdr = &pfrag_list->Hdr;
                NetIP_RxPktFragListUpdate(pfrag_list,                   /* Update frag list reasm calcs.                */
                                          pfrag_list_buf_hdr,
                                          frag_ip_flags,
                                          frag_offset,
                                          frag_size,
                                          perr);
                if (*perr !=  NET_IP_ERR_NONE) {
                     return ((NET_BUF *)0);
                }
                                                                        /* Chk    frag list reasm complete.             */
                pfrag = NetIP_RxPktFragListChkComplete(pfrag_list,
                                                       pfrag_list_buf_hdr,
                                                       perr);

            } else {                                                    /* Else discard overlap frag & datagram ...     */
                NetIP_RxPktFragListDiscard(pfrag_list, DEF_YES, &err);  /* ... (see Note #1b2).                         */
                NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragDiscardedCtr);
                pfrag = pbuf;
               *perr  = NET_IP_ERR_RX_FRAG_DISCARD;
            }

            frag_insert_done = DEF_YES;

/*$PAGE*/
        } else {                                                        /* Else if frag offset = cur frag offset, ...   */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragDiscardedCtr);
            pfrag = pbuf;
           *perr  = NET_IP_ERR_RX_FRAG_DISCARD;                         /* ... discard duplicate frag (see Note #1b1).  */

            frag_size_cur  = pfrag_list_cur_buf_hdr->IP_TotLen - pfrag_list_cur_buf_hdr->IP_HdrLen;
            if (frag_size != frag_size_cur) {                           /* If frag size != cur frag size,    ...        */
                NetIP_RxPktFragListDiscard(pfrag_list, DEF_YES, &err);  /* ... discard overlap frag datagram ...        */
                                                                        /* ... (see Note #1b2).                         */
            }

            frag_insert_done = DEF_YES;
        }
    }

    return (pfrag);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetIP_RxPktFragListRemove()
*
* Description : (1) Remove fragment list from Fragment Lists :
*
*                   (a) Free   fragment reassembly timer
*                   (b) Remove fragment list from Fragment Lists
*                   (c) Clear  buffer's fragment pointers
*
*
* Argument(s) : pfrag_list      Pointer to fragment list head buffer.
*               ----------      Argument validated in NetIP_RxPktFragListDiscard(),
*                                                     NetIP_RxPktFragListChkComplete().
*
*               tmr_free        Indicate whether to free network timer :
*
*                                   DEF_YES            Free network timer for fragment list discard.
*                                   DEF_NO      Do NOT free network timer for fragment list discard
*                                                   [Freed by  NetTmr_TaskHandler()
*                                                          via NetIP_RxPktFragListDiscard()].
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_RxPktFragListDiscard(),
*               NetIP_RxPktFragListChkComplete().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIP_RxPktFragListRemove (NET_BUF      *pfrag_list,
                                         CPU_BOOLEAN   tmr_free)
{
    NET_BUF      *pfrag_list_prev_list;
    NET_BUF      *pfrag_list_next_list;
    NET_BUF_HDR  *pfrag_list_prev_list_buf_hdr;
    NET_BUF_HDR  *pfrag_list_next_list_buf_hdr;
    NET_BUF_HDR  *pfrag_list_buf_hdr;
    NET_TMR      *ptmr;


    pfrag_list_buf_hdr = &pfrag_list->Hdr;

                                                                /* ------------------ FREE FRAG TMR ------------------- */
    if (tmr_free == DEF_YES) {
        ptmr = pfrag_list_buf_hdr->TmrPtr;
        if (ptmr != (NET_TMR *)0) {
            NetTmr_Free(ptmr);
        }
    }

                                                                /* --------- REMOVE FRAG LIST FROM FRAG LISTS --------- */
    pfrag_list_prev_list = pfrag_list_buf_hdr->PrevPrimListPtr;
    pfrag_list_next_list = pfrag_list_buf_hdr->NextPrimListPtr;

                                                                /* Point prev frag list to next frag list.              */
    if (pfrag_list_prev_list != (NET_BUF *)0) {
        pfrag_list_prev_list_buf_hdr                  = &pfrag_list_prev_list->Hdr;
        pfrag_list_prev_list_buf_hdr->NextPrimListPtr =  pfrag_list_next_list;
    } else {
        NetIP_FragReasmListsHead                      =  pfrag_list_next_list;
    }

                                                                /* Point next frag list to prev frag list.              */
    if (pfrag_list_next_list != (NET_BUF *)0) {
        pfrag_list_next_list_buf_hdr                  = &pfrag_list_next_list->Hdr;
        pfrag_list_next_list_buf_hdr->PrevPrimListPtr =  pfrag_list_prev_list;
    } else {
        NetIP_FragReasmListsTail                      =  pfrag_list_prev_list;
    }

                                                                /* ---------------- CLR BUF FRAG PTRS ----------------- */
    pfrag_list_buf_hdr->PrevPrimListPtr = (NET_BUF *)0;
    pfrag_list_buf_hdr->NextPrimListPtr = (NET_BUF *)0;
    pfrag_list_buf_hdr->TmrPtr          = (NET_TMR *)0;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetIP_RxPktFragListDiscard()
*
* Description : Discard fragment list from Fragment Lists.
*
* Argument(s) : pfrag_list      Pointer to fragment list head buffer.
*               ----------      Argument validated in NetIP_RxPktFragListInsert(),
*                                                     NetIP_RxPktFragListChkComplete(),
*                                                     NetIP_RxPktFragListDiscard().
*
*               tmr_free        Indicate whether to free network timer :
*
*                                   DEF_YES            Free network timer for fragment list discard.
*                                   DEF_NO      Do NOT free network timer for fragment list discard
*                                                   [Freed by NetTmr_TaskHandler()].
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_RX_FRAG_DISCARD      Fragment list discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_RxPktFragListInsert(),
*               NetIP_RxPktFragListChkComplete(),
*               NetIP_RxPktFragListTimeout().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIP_RxPktFragListDiscard (NET_BUF      *pfrag_list,
                                          CPU_BOOLEAN   tmr_free,
                                          NET_ERR      *perr)
{
    NET_ERR  err;


    NetIP_RxPktFragListRemove(pfrag_list, tmr_free);                    /* Remove frag list from Frag Lists.            */
    NetIP_RxPktDiscard(pfrag_list, &err);                               /* Discard every frag buf in frag list.         */

    NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragDatagramDiscardedCtr);   /* Inc discarded frag'd datagram ctr.           */

   *perr = NET_IP_ERR_RX_FRAG_DISCARD;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetIP_RxPktFragListUpdate()
*
* Description : Update fragment list reassembly calculations.
*
* Argument(s) : pfrag_list_buf_hdr      Pointer to fragment list head buffer's header.
*               ------------------      Argument validated in NetIP_RxPktFragListAdd(),
*                                                             NetIP_RxPktFragListInsert().
*
*               frag_ip_flags           Fragment IP header flags.
*               -------------           Argument validated in NetIP_RxPktFragListAdd(),
*                                                             NetIP_RxPktFragListInsert().
*
*               frag_offset             Fragment offset.
*               -----------             Argument validated in NetIP_RxPktFragListAdd(),
*                                                             NetIP_RxPktFragListInsert().
*
*               frag_size               Fragment size (in octets).
*               ---------               Argument validated in NetIP_RxPktFragListAdd(),
*                                                             NetIP_RxPktFragListInsert().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 Fragment list reassembly calculations
*                                                                   successfully updated.
*                               NET_IP_ERR_RX_FRAG_LEN_TOT      Invalid fragment datagram total IP length.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_RxPktFragListAdd(),
*               NetIP_RxPktFragListInsert().
*
* Note(s)     : (1) (a) RFC #791, Section 3.2 'Fragmentation and Reassembly' states that "fragments ... data 
*                       portion ... allows" up to :
*
*                       (1) "2**13 = 8192 fragments" ...
*                       (2) "of 8 octets each"       ...
*                       (3) "for a total of 65,536 [sic] octets."
*
*                   (b) RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly Procedure' 
*                       states that :
*
*                       (1) "If this is the last fragment"                    ...
*                           (A) "(that is the more fragments field is zero)"; ...
*
*                       (2) "The total data length is computed."
*
*               (2) To avoid possible integer arithmetic overflow, the fragmentation arithmetic result MUST
*                   be declared as an integer data type with a greater resolution -- i.e. greater number of
*                   bits -- than the fragmentation arithmetic operands' data type(s).
*********************************************************************************************************
*/

static  void  NetIP_RxPktFragListUpdate (NET_BUF      *pfrag_list,
                                         NET_BUF_HDR  *pfrag_list_buf_hdr,
                                         CPU_INT16U    frag_ip_flags,
                                         CPU_INT16U    frag_offset,
                                         CPU_INT16U    frag_size,
                                         NET_ERR      *perr)
{
    CPU_INT32U   frag_size_tot;                                     /* See Note #2.                                     */
    CPU_BOOLEAN  ip_flag_frags_more;
    NET_ERR      err;


    pfrag_list_buf_hdr->IP_FragSizeCur += frag_size;
    ip_flag_frags_more                  = DEF_BIT_IS_SET(frag_ip_flags, NET_IP_HDR_FLAG_FRAG_MORE);
    if (ip_flag_frags_more != DEF_YES) {                            /* If 'More Frags' NOT set (see Note #1b1A), ...    */
                                                                    /* ... calc frag tot size  (see Note #1b2).         */
        frag_size_tot = ((CPU_INT32U)frag_offset * NET_IP_FRAG_SIZE_UNIT) + (CPU_INT32U)frag_size;
        if (frag_size_tot > NET_IP_TOT_LEN_MAX) {                   /* If frag tot size > IP tot len max, ...           */
            NetIP_RxPktFragListDiscard(pfrag_list, DEF_YES, &err);  /* ... discard ovf'd frag datagram (see Note #1a3). */
           *perr = NET_IP_ERR_RX_FRAG_LEN_TOT;
            return;
        }

        pfrag_list_buf_hdr->IP_FragSizeTot = (CPU_INT16U)frag_size_tot;
    }

   *perr = NET_IP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetIP_RxPktFragListChkComplete()
*
* Description : Check if fragment list complete; i.e. fragmented datagram reassembled.
*
* Argument(s) : pfrag_list              Pointer to fragment list head buffer.
*               ----------              Argument validated in NetIP_RxPktFragListInsert().
*
*               pfrag_list_buf_hdr      Pointer to fragment list head buffer's header.
*               ------------------      Argument validated in NetIP_RxPktFragListInsert().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_RX_FRAG_COMPLETE     Datagram complete; fragments reassembled.
*                               NET_IP_ERR_RX_FRAG_REASM        Fragments in reassembly progress.
*                               NET_IP_ERR_RX_FRAG_SIZE_TOT     Invalid fragmented datagram total size.
*                               NET_IP_ERR_RX_FRAG_LEN_TOT      Invalid fragment   datagram total IP length.
*
*                                                               - RETURNED BY NetIP_RxPktFragListDiscard() : -
*                               NET_IP_ERR_RX_FRAG_DISCARD      Fragment list discarded.
*
* Return(s)   : Pointer to reassembled datagram, if fragment reassembly complete.
*
*               Pointer to NULL,                 if fragment reassembly in progress.
*                                                   OR
*                                                for any fragment discard error.
*
* Caller(s)   : NetIP_RxPktFragListInsert().
*
* Note(s)     : (1) (a) RFC #791, Section 3.2 'Fragmentation and Reassembly' states that :
*
*                       (1) "Fragments ... data portion ... allows" up to :
*                           (A) "2**13 = 8192 fragments" ...
*                           (B) "of 8 octets each"       ...
*                           (C) "for a total of 65,536 [sic] octets."
*
*                       (2) "The header is counted in the total length and not in the fragments."
*
*                   (b) RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly 
*                       Procedure' states that :
*
*                       (1) "If this fragment completes the datagram ... then the datagram is sent to 
*                            the next step in datagram processing."
*
*                       (2) "Otherwise the timer is set to" :
*                           (A) "the maximum of the current timer value" ...
*                           (B) "and the value of the time to live field from this fragment."
*                               (1) #### However, since IP headers' Time-To-Live (TTL) field does NOT 
*                                   directly correlate to a specific timeout value in seconds, this 
*                                   requirement is NOT implemented.
*
*               (2) To avoid possible integer arithmetic overflow, the fragmentation arithmetic result 
*                   MUST be declared as an integer data type with a greater resolution -- i.e. greater 
*                   number of bits -- than the fragmentation arithmetic operands' data type(s).
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_BUF  *NetIP_RxPktFragListChkComplete (NET_BUF      *pfrag_list,
                                                  NET_BUF_HDR  *pfrag_list_buf_hdr,
                                                  NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR         cpu_sr;
#endif
    NET_BUF       *pfrag;
    CPU_INT32U     frag_tot_len;                                    /* See Note #2.                                     */
    NET_TMR_TICK   timeout_tick;
    NET_ERR        err;

                                                                    /* If tot frag size complete, ...                   */
    if (pfrag_list_buf_hdr->IP_FragSizeCur == pfrag_list_buf_hdr->IP_FragSizeTot) {
                                                                    /* Calc frag IP tot len (see Note #1a2).            */
        frag_tot_len = (CPU_INT32U)pfrag_list_buf_hdr->IP_HdrLen + (CPU_INT32U)pfrag_list_buf_hdr->IP_FragSizeTot;
        if (frag_tot_len > NET_IP_TOT_LEN_MAX) {                    /* If tot frag len > IP tot len max, ...            */
            NetIP_RxPktFragListDiscard(pfrag_list, DEF_YES, &err);  /* ... discard ovf'd frag datagram (see Note #1a1C).*/
           *perr =   NET_IP_ERR_RX_FRAG_LEN_TOT;
            return ((NET_BUF *)0);
        }

        NetIP_RxPktFragListRemove(pfrag_list, DEF_YES);
        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatRxFragDatagramReasmCtr);
        pfrag = pfrag_list;                                         /* ... rtn reasm'd datagram (see Note #1b1).        */
       *perr  = NET_IP_ERR_RX_FRAG_COMPLETE;

                                                                    /* If cur frag size > tot frag size, ...            */
    } else if (pfrag_list_buf_hdr->IP_FragSizeCur > pfrag_list_buf_hdr->IP_FragSizeTot) { 
        NetIP_RxPktFragListDiscard(pfrag_list, DEF_YES, perr);      /* ... discard ovf'd frag datagram.                 */
       *perr =   NET_IP_ERR_RX_FRAG_SIZE_TOT;
        return ((NET_BUF *)0);


    } else {                                                        /* Else reset frag tmr (see Note #1b2A).            */
        CPU_CRITICAL_ENTER();
        timeout_tick = NetIP_FragReasmTimeout_tick;
        CPU_CRITICAL_EXIT();
        NetTmr_Set((NET_TMR    *) pfrag_list_buf_hdr->TmrPtr,
                   (CPU_FNCT_PTR) NetIP_RxPktFragTimeout,
                   (NET_TMR_TICK) timeout_tick,
                   (NET_ERR    *)&err);
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        if (err != NET_TMR_ERR_NONE) {
            NetIP_RxPktFragListDiscard(pfrag_list, DEF_YES, perr);
            return ((NET_BUF *)0);
        }
#endif

       *perr  =  NET_IP_ERR_RX_FRAG_REASM;
        pfrag = (NET_BUF *)0;
    }


    return (pfrag);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIP_RxPktFragTimeout()
*
* Description : Discard fragment list on fragment reassembly timeout.
*
* Argument(s) : pfrag_list_timeout      Pointer to network buffer fragment reassembly list (see Note #1b).
*
* Return(s)   : none.
*
* Caller(s)   : Referenced in NetIP_RxPktFragListAdd().
*
* Note(s)     : (1) RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly Procedure' 
*                   states that :
*
*                   (a) "If the [IP fragments' reassembly] timer runs out," ...
*                   (b) "the [sic] all reassembly resources for this buffer identifier are released."
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
*                           (A) in this case, a 'NET_BUF' pointer.
*
*                   See also 'net_tmr.c  NetTmr_Get()  Note #3'.
*
*               (3) This function is a network timer callback function :
*
*                   (a) Clear the timer pointer; ...
*                       (1) Cleared in NetIP_RxPktFragListRemove() via NetIP_RxPktFragListDiscard().
*
*                   (b) but do NOT re-free the timer.
*
*               (4) (a) RFC #792, Section 'Time Exceeded Message' states that :
*                    
*                       (1) "if a host reassembling a fragmented datagram cannot complete the reassembly 
*                            due to missing fragments within its time limit" ...
*                       (2) (A) "it discards the datagram,"                  ...
*                           (B) "and it may send a time exceeded message."
*
*                   (b) MUST send ICMP 'Time Exceeded' error message BEFORE NetIP_RxPktFragListDiscard() 
*                       frees fragment buffers.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetIP_RxPktFragTimeout (void  *pfrag_list_timeout)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN   used;
#endif
    NET_BUF      *pfrag_list;
    NET_ERR       err;


    pfrag_list = (NET_BUF *)pfrag_list_timeout;                 /* See Note #2b2A.                                      */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (pfrag_list == (NET_BUF *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrNullPtrCtr);
        return;
    }
                                                                /* ---------------- VALIDATE BUF USED ----------------- */
    used = NetBuf_IsUsed(pfrag_list);
    if (used != DEF_YES) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrNullPtrCtr);
        return;
    }
#endif


    NetICMP_TxMsgErr((NET_BUF  *) pfrag_list,                   /* Send ICMP 'Time Exceeded' err msg (see Note #4).     */
                     (CPU_INT08U) NET_ICMP_MSG_TYPE_TIME_EXCEED, 
                     (CPU_INT08U) NET_ICMP_MSG_CODE_TIME_EXCEED_FRAG_REASM, 
                     (CPU_INT08U) NET_ICMP_MSG_PTR_NONE,
                     (NET_ERR  *)&err);
                                                                /* Discard frag list (see Note #1b).                    */
    NetIP_RxPktFragListDiscard((NET_BUF   *) pfrag_list,
                               (CPU_BOOLEAN) DEF_NO,            /* Clr but do NOT free tmr (see Note #3).               */
                               (NET_ERR   *)&err);


    NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxFragDatagramTimeoutCtr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetIP_RxPktDemuxDatagram()
*
* Description : Demultiplex IP datagram to appropriate ICMP, TCP, or UDP queue.
*
* Argument(s) : pbuf        Pointer to network buffer that received IP datagram.
*               ----        Argument checked   in NetIP_Rx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetIP_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_ERR_INVALID_PROTOCOL        Invalid/unknown protocol type.
*                               NET_ERR_RX                      Receive error; packet discarded.
*
*                                                               ------ RETURNED BY NetICMP_Rx() : ------
*                               NET_ICMP_ERR_NONE               ICMP message successfully demultiplexed.
*
*                                                               ------ RETURNED BY NetTCP_Rx() : -------
*                               NET_TCP_ERR_NONE                TCP segment  successfully demultiplexed.
*
*                                                               ------ RETURNED BY NetUDP_Rx() : -------
*                               NET_UDP_ERR_NONE                UDP datagram successfully demultiplexed.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_Rx().
*
* Note(s)     : (1) When network buffer is demultiplexed to higher-layer protocol receive, buffer's reference
*                   counter is NOT incremented since the IP layer does NOT maintain a reference to the buffer.
*
*               (2) Default case already invalidated in NetIP_RxPktValidate().  However, the default case
*                   is included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
*********************************************************************************************************
*/

static  void  NetIP_RxPktDemuxDatagram (NET_BUF      *pbuf,
                                        NET_BUF_HDR  *pbuf_hdr,
                                        NET_ERR      *perr)
{

    switch (pbuf_hdr->ProtocolHdrType) {                        /* Demux buf to appropriate protocol (see Note #1).     */
        case NET_PROTOCOL_TYPE_ICMP:
             NetICMP_Rx(pbuf, perr);
             break;


        case NET_PROTOCOL_TYPE_UDP:
             NetUDP_Rx(pbuf, perr);
             break;


#ifdef  NET_TCP_MODULE_PRESENT
        case NET_PROTOCOL_TYPE_TCP:
             NetTCP_Rx(pbuf, perr);
             break;
#endif

        case NET_PROTOCOL_TYPE_NONE:
        default:                                                /* See Note #2.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrRxInvalidProtocolCtr);
            *perr = NET_ERR_INVALID_PROTOCOL;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIP_RxPktDiscard()
*
* Description : On any IP receive errors, discard IP packet(s) & buffer(s).
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_Rx(),
*               NetIP_RxPktFragListDiscard().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIP_RxPktDiscard (NET_BUF  *pbuf,
                                  NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetIP_ErrRxPktDiscardedCtr;
#else
    pctr = (NET_CTR *) 0;
#endif
    NetBuf_FreeBufList((NET_BUF *)pbuf,
                       (NET_CTR *)pctr);

   *perr = NET_ERR_RX;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIP_TxPktValidate()
*
* Description : (1) Validate IP transmit packet parameters & options :
*
*                   (a) Validate the following transmit packet parameters :
*
*                       (1) Supported protocols :
*                           (A) ICMP
*                           (B) UDP
*                           (C) TCP
*
*                       (2) Buffer protocol index
*                       (3) Total Length
*                       (4) Type of Service (TOS)                                   See Note #2c
*                       (5) Destination Address                                     See Note #2e
*                       (6) Source      Address                                     See Note #2d
*                       (7) Flags
*
*                   (b) The following parameters are inherently assumed to be valid :
*
*                       (1) Time-to-Live (TTL)
*
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetIP_Tx().
*
*               addr_src    Source      IP address.
*
*               addr_dest   Destination IP address.
*
*               TOS         Specific TOS to transmit IP packet
*                               (see 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
*
*               TTL         Specific TTL to transmit IP packet 
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
*                               NET_IP_ERR_NONE                 Transmit packet validated.
*                               NET_ERR_INVALID_PROTOCOL        Invalid/unknown protocol type.
*                               NET_BUF_ERR_INVALID_IX          Invalid or insufficient buffer index.
*                               NET_IP_ERR_INVALID_LEN_DATA     Invalid protocol/data length.
*                               NET_IP_ERR_INVALID_TOS          Invalid TOS.
*                               NET_IP_ERR_INVALID_ADDR_SRC     Invalid IP source      address.
*                               NET_IP_ERR_INVALID_ADDR_DEST    Invalid IP destination address.
*                               NET_IP_ERR_INVALID_FLAG         Invalid IP flag(s).
*
*                                                               -- RETURNED BY NetIP_TxPktValidateOpt() : --
*                               NET_IP_ERR_NONE                 IP transmit option configurations validated.
*                               NET_IP_ERR_INVALID_OPT_TYPE     Invalid IP option  type.
*                               NET_IP_ERR_INVALID_OPT_LEN      Invalid IP options length.
*                               NET_IP_ERR_INVALID_OPT_CFG      Invalid IP options configuration.
*                               NET_IP_ERR_INVALID_OPT_ROUTE    Invalid IP route address(s).
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_Tx().
*$PAGE*
* Note(s)     : (2) RFC #1122, Section 3.2.1 requires that IP packets be transmitted with the following 
*                   valid IP header fields :
*
*                   (a) Version                                             RFC #1122, Section 3.2.1.1
*                   (b) Check-Sum                                           RFC #1122, Section 3.2.1.2
*                   (c) Type of Service (TOS)                               RFC #1122, Section 3.2.1.6
*
*                   (d) Source      Address
*
*                       (1) (A) RFC #1122, Section 3.2.1.3 states that "when a host sends any datagram, 
*                               the IP source address MUST be one of its own IP addresses (but not a 
*                               broadcast or multicast address)".
*
*                           (B) (1) MUST     be one of the following :
*                                   (a) Configured host address             RFC #1122, Section 3.2.1.3.(1)
*                                   (b) Localhost                           RFC #1122, Section 3.2.1.3.(g)
*                                   (c) This       Host                     RFC #1122, Section 3.2.1.3.(a)
*                                   (d) Specified  Host                     RFC #1122, Section 3.2.1.3.(b)
*
*                               (2) MUST NOT be one of the following :
*                                   (a) Limited   Broadcast                 RFC #1122, Section 3.2.1.3.(c)
*                                   (b) Directed  Broadcast                 RFC #1122, Section 3.2.1.3.(d)
*                                   (c) Subnet    Broadcast                 RFC #1122, Section 3.2.1.3.(e)
*                                   (d) Multicast                           RFC #1122, Section 3.2.1.3.(3)
*
*                               (3) RFC #1122, Section 3.2.1.3.(g) states that the "internal host loopback 
*                                   address ... MUST NOT appear outside a host".
*
*                                   (a) However, this does NOT prevent the host loopback address from being 
*                                       used as an IP packet's source address as long as BOTH the packet's 
*                                       source AND destination addresses are internal host addresses, either 
*                                       the host's configured IP address or any host loopback address.
*
*                   (e) Destination Address
*
*                       (1) (A) (1) MAY      be one of the following :
*                                   (a) Localhost                           RFC #1122, Section 3.2.1.3.(g)
*                                   (b) Limited   Broadcast                 RFC #1122, Section 3.2.1.3.(c)
*                                   (c) Directed  Broadcast                 RFC #1122, Section 3.2.1.3.(d)
*                                   (d) Subnet    Broadcast                 RFC #1122, Section 3.2.1.3.(e)
*
*                               (2) MUST NOT be one of the following :
*                                   (a) This      Host                      RFC #1122, Section 3.2.1.3.(a)
*                                   (b) Specified Host                      RFC #1122, Section 3.2.1.3.(b)
*
*                       (2) (A) RFC #3927, Section 2.1 specifies the "IPv4 Link-Local address" :
*                           
*                               (1) "Range ... inclusive"  ...
*                                   (a) "from 169.254.1.0" ...
*                                   (b) "to   169.254.254.255".
*
*               (3) See 'net_ip.h  IP ADDRESS DEFINES  Notes #2 & #3' for supported IP addresses.
*********************************************************************************************************
*/
/*$PAGE*/
#if ((NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED))
static  void  NetIP_TxPktValidate (NET_BUF_HDR  *pbuf_hdr,
                                   NET_IP_ADDR   addr_src,
                                   NET_IP_ADDR   addr_dest,
                                   NET_IP_TOS    TOS,
                                   NET_IP_TTL    TTL,
                                   CPU_INT16U    flags,
                                   void         *popts,
                                   NET_ERR      *perr)
{
    CPU_INT16U   ix;
    CPU_INT16U   len;
    CPU_INT16U   flag_mask;
    CPU_BOOLEAN  tos_mbz;
    CPU_BOOLEAN  addr_host;
    CPU_BOOLEAN  tx_remote;


                                                                    /* --------------- VALIDATE PROTOCOL -------------- */
    switch (pbuf_hdr->ProtocolHdrType) {
        case NET_PROTOCOL_TYPE_ICMP:
             ix  = pbuf_hdr->ICMP_MsgIx;
             len = pbuf_hdr->ICMP_MsgLen;
             break;


        case NET_PROTOCOL_TYPE_UDP:
#ifdef  NET_TCP_MODULE_PRESENT
        case NET_PROTOCOL_TYPE_TCP:
#endif
             ix  = pbuf_hdr->TCP_UDP_HdrDataIx;
             len = pbuf_hdr->TCP_UDP_HdrLen + (CPU_INT16U)pbuf_hdr->DataLen;
             break;


        case NET_PROTOCOL_TYPE_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxInvalidProtocolCtr);
            *perr = NET_ERR_INVALID_PROTOCOL;
             return;                                                /* Prevent 'break NOT reachable' compiler warning.  */
    }

    if (ix == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

    if (ix < NET_IP_HDR_SIZE_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }



                                                                    /* ------------ VALIDATE TOT DATA LEN ------------- */
    if (len != pbuf_hdr->TotLen) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrDataLenCtr);
       *perr = NET_IP_ERR_INVALID_LEN_DATA;
        return;
    }



                                                                    /* --------------- VALIDATE IP TOS ---------------- */
    tos_mbz = DEF_BIT_IS_SET(TOS, NET_IP_HDR_TOS_MBZ_MASK);
    if (tos_mbz != DEF_NO) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrTOS_Ctr);
       *perr = NET_IP_ERR_INVALID_TOS;
        return;
    }



/*$PAGE*/
                                                                    /* ------------- VALIDATE IP DEST ADDR ------------ */
                                                                    /* See Note #2e.                                    */
    addr_host = NetIP_IsAddrHostCfgdHandler(addr_dest);             /* Chk this host's cfg'd addr.                      */
    if (addr_host == DEF_YES) {

        tx_remote = DEF_NO;

                                                                    /* Chk invalid 'This Host'      (see Note #2e1A2a). */
    } else if (addr_dest == NET_IP_ADDR_THIS_HOST) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrDestCtr);
       *perr = NET_IP_ERR_INVALID_ADDR_DEST;
        return;

                                                                    /* Chk localhost addrs          (see Note #2e1A1a). */
    } else if ((addr_dest & NET_IP_ADDR_LOCAL_HOST_MASK_NET) == NET_IP_ADDR_LOCAL_HOST_NET) {
                                                                    /* Chk localhost  'This Host'   (see Note #2e1A2a). */
        if ((addr_dest             & NET_IP_ADDR_LOCAL_HOST_MASK_HOST) == 
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_LOCAL_HOST_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrDestCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_DEST;
             return;
        }

        tx_remote = DEF_NO;

                                                                    /* Chk link-local addrs         (see Note #2e2).    */
    } else if ((addr_dest & NET_IP_ADDR_LOCAL_LINK_MASK_NET) == NET_IP_ADDR_LOCAL_LINK_NET) {
                                                                    /* Chk link-local 'This Host'   (see Note #2e1A2a). */
        if ((addr_dest             & NET_IP_ADDR_LOCAL_LINK_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_LOCAL_LINK_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrDestCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_DEST;
             return;
        }
                                                                    /* Chk invalid link-local addrs (see Note #2e2A1).  */
        if ((addr_dest < NET_IP_ADDR_LOCAL_LINK_HOST_MIN) ||
            (addr_dest > NET_IP_ADDR_LOCAL_LINK_HOST_MAX)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrDestCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_DEST;
             return;
        }

        tx_remote = DEF_YES;

/*$PAGE*/
                                                                    /* Chk lim'd broadcast          (see Note #2e1A1b). */
    } else if (addr_dest == NET_IP_ADDR_BROADCAST) {

        tx_remote = DEF_YES;


    } else {
        if ((addr_dest & NET_IP_ADDR_CLASS_A_MASK) == NET_IP_ADDR_CLASS_A) { 
                                                                    /* Chk Class-A 'This Host'      (see Note #2e1A2b). */
            if ((addr_dest             & NET_IP_ADDR_CLASS_A_MASK_HOST) ==
                (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_A_MASK_HOST)) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrDestCtr);
                *perr = NET_IP_ERR_INVALID_ADDR_DEST;
                 return;
            }

        } else if ((addr_dest & NET_IP_ADDR_CLASS_B_MASK) == NET_IP_ADDR_CLASS_B) {
                                                                    /* Chk Class-B 'This Host'      (see Note #2e1A2b). */
            if ((addr_dest             & NET_IP_ADDR_CLASS_B_MASK_HOST) ==
                (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_B_MASK_HOST)) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrDestCtr);
                *perr = NET_IP_ERR_INVALID_ADDR_DEST;
                 return;
            }

        } else if ((addr_dest & NET_IP_ADDR_CLASS_C_MASK) == NET_IP_ADDR_CLASS_C) {
                                                                    /* Chk Class-C 'This Host'      (see Note #2e1A2b). */
            if ((addr_dest             & NET_IP_ADDR_CLASS_C_MASK_HOST) ==
                (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_C_MASK_HOST)) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrDestCtr);
                *perr = NET_IP_ERR_INVALID_ADDR_DEST;
                 return;
            }

        } else {                                                    /* Discard invalid addr class   (see Note #3).      */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrDestCtr);
           *perr = NET_IP_ERR_INVALID_ADDR_DEST;
            return;
        }
                                                                    /* Chk subnet  'This Host'      (see Note #2e1A2b). */
        if ((NetIP_AddrThisHostSubnetMask     != NET_IP_ADDR_NONE) &&
            (NetIP_AddrThisHostSubnetMaskHost != NET_IP_ADDR_NONE) &&
            (NetIP_AddrThisHostSubnetNet      != NET_IP_ADDR_NONE)) {
            if ((addr_dest & NetIP_AddrThisHostSubnetMask) ==
                             NetIP_AddrThisHostSubnetNet ) {
                if ((addr_dest             & NetIP_AddrThisHostSubnetMaskHost) ==
                    (NET_IP_ADDR_THIS_HOST & NetIP_AddrThisHostSubnetMaskHost)) {
                     NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrDestCtr);
                    *perr = NET_IP_ERR_INVALID_ADDR_DEST;
                     return;
                }
            }
        }

        tx_remote = DEF_YES;
    }



/*$PAGE*/
                                                                    /* ------------- VALIDATE IP SRC ADDR ------------- */
                                                                    /* See Note #2d.                                    */
                                                                    /* Chk this host's cfg'd addr  (see Note #2d1B1a).  */
    if (addr_src == NetIP_AddrThisHost) {

                                                                    /* Chk 'This Host'             (see Note #2d1B1c).  */
    } else if (addr_src == NET_IP_ADDR_THIS_HOST) {

                                                                    /* Chk         localhost addrs (see Note #2d1B1b).  */
    } else if ((addr_src & NET_IP_ADDR_LOCAL_HOST_MASK_NET) == NET_IP_ADDR_LOCAL_HOST_NET) {
                                                                    /* Chk invalid localhost addrs.                     */
        if ((addr_src              & NET_IP_ADDR_LOCAL_HOST_MASK_HOST) == 
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_LOCAL_HOST_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrSrcCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_SRC;
             return;
        }
        if ((addr_src              & NET_IP_ADDR_LOCAL_HOST_MASK_HOST) == 
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_LOCAL_HOST_MASK_HOST)) {
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrSrcCtr);
            *perr = NET_IP_ERR_INVALID_ADDR_SRC;
             return;
        }

        if (tx_remote != DEF_NO) {                                  /* If localhost addr tx'd to remote host, ...       */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrSrcCtr);
           *perr = NET_IP_ERR_INVALID_ADDR_SRC;                     /* ... rtn err / discard pkt   (see Note #2d1B3a).  */
            return;
        }


    } else {
        if ((addr_src & NET_IP_ADDR_CLASS_A_MASK) == NET_IP_ADDR_CLASS_A) {
                                                                    /* Chk Class-A 'This Host'     (see Note #2d1B1d).  */
            if ((addr_src              & NET_IP_ADDR_CLASS_A_MASK_HOST) !=
                (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_A_MASK_HOST)) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrSrcCtr);
                *perr = NET_IP_ERR_INVALID_ADDR_SRC;
                 return;
            }

        } else if ((addr_src & NET_IP_ADDR_CLASS_B_MASK) == NET_IP_ADDR_CLASS_B) {
                                                                    /* Chk Class-B 'This Host'     (see Note #2d1B1d).  */
            if ((addr_src              & NET_IP_ADDR_CLASS_B_MASK_HOST) !=
                (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_B_MASK_HOST)) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrSrcCtr);
                *perr = NET_IP_ERR_INVALID_ADDR_SRC;
                 return;
            }

        } else if ((addr_src & NET_IP_ADDR_CLASS_C_MASK) == NET_IP_ADDR_CLASS_C) {
                                                                    /* Chk Class-C 'This Host'     (see Note #2d1B1d).  */
            if ((addr_src              & NET_IP_ADDR_CLASS_C_MASK_HOST) !=
                (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_C_MASK_HOST)) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrSrcCtr);
                *perr = NET_IP_ERR_INVALID_ADDR_SRC;
                 return;
            }

        } else {                                                    /* Discard invalid addr class  (see Note #3).       */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrAddrSrcCtr);
           *perr = NET_IP_ERR_INVALID_ADDR_SRC;
            return;
        }
    }



/*$PAGE*/
                                                                    /* -------------- VALIDATE IP FLAGS --------------- */
    flag_mask = NET_IP_FLAG_NONE        |
                NET_IP_FLAG_TX_DONT_FRAG;

    if ((flags & ~flag_mask) != NET_IP_FLAG_NONE) {                 /* If any invalid flags req'd, rtn err.             */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrFlagsCtr);
       *perr = NET_IP_ERR_INVALID_FLAG;
        return;
    }



                                                                    /* --------------- VALIDATE IP OPTS --------------- */
    if (popts != (void *)0) {
        NetIP_TxPktValidateOpt(popts, perr);
        if (*perr != NET_IP_ERR_NONE) {
            return;
        }
    }



                                                                    /* ------------ IGNORE VALID IP FIELDS ------------ */
   (void)&TTL;                                                      /* Prevent compiler warning.                        */


   *perr = NET_IP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIP_TxPktValidateOpt()
*
* Description : (1) Validate IP transmit option configurations :
*
*                   (a) IP transmit options MUST be configured by appropriate transmit options configuration 
*                       data structure(s) passed via 'popts'; see 'net_ip.h  IP HEADER OPTION CONFIGURATION 
*                       DATA TYPES' for IP options configuration.
*
*                   (b) IP header allows for a maximum option size of 40 octets (see 'net_ip.h  IP HEADER 
*                       OPTIONS DEFINES  Note #3').
*
*
* Argument(s) : popts       Pointer to one or more IP options configuration data structures (see Note #1a) :
*
*                               NET_IP_OPT_CFG_ROUTE_TS         IP Route &/or Internet Timestamp options configuration.
*                               NET_IP_OPT_CFG_SECURITY         Security options configuration
*                                                                   (see 'net_ip.c  Note #1f').
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 IP transmit option configurations validated.
*                               NET_IP_ERR_INVALID_OPT_TYPE     Invalid IP option type.
*                               NET_IP_ERR_INVALID_OPT_LEN      Total options length exceeds maximum IP
*                                                                     options length (see Note #1b).
*                               NET_IP_ERR_INVALID_OPT_CFG      Invalid number of exclusive IP options
*                                                                   (see Note #3).
*
*                                                               - RETURNED BY NetIP_TxPktValidateOptRouteTS() : -
*                               NET_IP_ERR_INVALID_OPT_TYPE     Invalid IP option type.
*                               NET_IP_ERR_INVALID_OPT_LEN      Invalid number of Route &/or Internet
*                                                                   Timestamp entries configured.
*                               NET_IP_ERR_INVALID_OPT_ROUTE    Invalid route address(s).
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_TxPktValidate().
*
* Note(s)     : (2) (a) See 'net_ip.h  IP HEADER OPTIONS DEFINES' for   supported IP options' summary.
*
*                   (b) See 'net_ip.c  Note #1f'                  for unsupported IP options.
*
*               (3) The following IP transmit options MUST be configured exclusively--i.e. only a single IP Route
*                   or Internet Timestamp option may be configured for any one IP datagram :
*
*                   (a) NET_IP_OPT_CFG_TYPE_ROUTE_STRICT
*                   (b) NET_IP_OPT_CFG_TYPE_ROUTE_LOOSE
*                   (c) NET_IP_OPT_CFG_TYPE_ROUTE_REC
*                   (d) NET_IP_OPT_CFG_TYPE_TS_ONLY
*                   (e) NET_IP_OPT_CFG_TYPE_TS_ROUTE_REC
*                   (f) NET_IP_OPT_CFG_TYPE_TS_ROUTE_SPEC
*
*                       (A) RFC #1122, Section 3.2.1.8.(c) prohibits "an IP header" from transmitting 
*                           with "more than one Source Route option".
*
*               (4) RFC #791, Section 3.1 'Options : Internet Timestamp' states that "each timestamp"
*                   may be "preceded with [an] internet address".
*********************************************************************************************************
*/
/*$PAGE*/
#if ((NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED))
static  void  NetIP_TxPktValidateOpt (void     *popts,
                                      NET_ERR  *perr)
{
    CPU_INT08U   opt_len_size;
    CPU_INT08U   opt_len;
    CPU_INT08U   opt_nbr_route_ts;
    NET_TYPE    *popt_cfg_type;
    void        *popt_cfg;
    void        *popt_next;


    opt_len_size     = 0;
    opt_nbr_route_ts = 0;
    popt_cfg         = popts;

    while (popt_cfg  != (void *)0) {
        popt_cfg_type = (NET_TYPE *)popt_cfg;
        switch (*popt_cfg_type) {
            case NET_IP_OPT_CFG_TYPE_ROUTE_STRICT:
            case NET_IP_OPT_CFG_TYPE_ROUTE_LOOSE:
            case NET_IP_OPT_CFG_TYPE_ROUTE_REC: 
            case NET_IP_OPT_CFG_TYPE_TS_ONLY:
            case NET_IP_OPT_CFG_TYPE_TS_ROUTE_REC:
            case NET_IP_OPT_CFG_TYPE_TS_ROUTE_SPEC:
                 if (opt_nbr_route_ts > 0) {                    /* If > 1 exclusive IP opt, rtn err (see Note #3A).     */
                     NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrOptCfgCtr);
                    *perr = NET_IP_ERR_INVALID_OPT_CFG;
                     return;
                 }
                 opt_nbr_route_ts++;

                 NetIP_TxPktValidateOptRouteTS(popt_cfg, &opt_len, &popt_next, perr);
                 break;

#if 0                                                           /* --------------- UNSUPPORTED IP OPTS ---------------- */
                                                                /* See Note #2b.                                        */
            case NET_IP_OPT_CFG_TYPE_SECURITY:
            case NET_IP_OPT_CFG_TYPE_SECURITY_EXTENDED:
                                                                /* 'break' intentionally omitted; do NOT move from the  */
                                                                /* ... following case : 'NET_IP_OPT_CFG_TYPE_NONE'.     */
#endif

            case NET_IP_OPT_CFG_TYPE_NONE:                      /* ----------------- INVALID IP OPTS ------------------ */
            default:                                            
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxOptTypeCtr);
                *perr = NET_IP_ERR_INVALID_OPT_TYPE;
                 return;                                        /* Prevent 'break NOT reachable' compiler warning.      */
        }

        if (*perr != NET_IP_ERR_NONE) {
            return;
        }

        opt_len_size += opt_len;
        if (opt_len_size > NET_IP_HDR_OPT_SIZE_MAX) {           /* If tot opt len exceeds max opt len, rtn err.         */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrOptLenCtr);
           *perr = NET_IP_ERR_INVALID_OPT_LEN;
            return;
        }

        popt_cfg = popt_next;                                   /* Validate next cfg opt.                               */
    }

   *perr = NET_IP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetIP_TxPktValidateOptRouteTS()
*
* Description : (1) Validate IP Route &/or Internet Timestamp option configuration :
*
*                   (a) See 'net_ip.h  IP ROUTE & INTERNET TIMESTAMP OPTIONS CONFIGURATION DATA TYPE' for 
*                       valid IP Route &/or Internet Timestamp option configuration.
*
*                   (b) Validate the following options' configuration parameters :
*
*                       (1) Type
*                       (2) Number
*                             * Less    than minimum
*                             * Greater than maximum
*                       (3) IP Route addresses
*                             * MUST be IP Class A, B, or C address     See 'net_ip.h  IP ADDRESS DEFINES  Note #2a1'
*                       (4) Internet Timestamps
*                             * Timestamp values are NOT validated
*
*                   (c) Return option values.
*
*
* Argument(s) : popt_route_ts   Pointer to IP Route &/or Internet Timestamp option configuration data structure.
*               -------------   Argument checked   in NetIP_TxPktValidateOpt().
*
*               popt_len        Pointer to variable that will receive the Route/Internet Timestamp option length
*               --------            (in octets).
*
*                               Argument validated in NetIP_TxPktValidateOpt().
*
*               popt_next       Pointer to variable that will receive the pointer to the next IP transmit option.
*               ---------       Argument validated in NetIP_TxPktValidateOpt().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 Route &/or Internet Timestamp option
*                                                                   configuration validated.
*                               NET_IP_ERR_INVALID_OPT_TYPE     Invalid IP option type.
*                               NET_IP_ERR_INVALID_OPT_LEN      Invalid number of Route &/or Internet
*                                                                   Timestamp entries configured.
*                               NET_IP_ERR_INVALID_OPT_ROUTE    Invalid route address(s).
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_TxPktValidateOpt().
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
#if ((NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED))
static  void  NetIP_TxPktValidateOptRouteTS (void         *popt_route_ts,
                                             CPU_INT08U   *popt_len,
                                             void        **popt_next,
                                             NET_ERR      *perr)
{
    NET_IP_OPT_CFG_ROUTE_TS  *popt_cfg_route_ts;
    CPU_INT08U                opt_nbr_min;
    CPU_INT08U                opt_nbr_max;
    CPU_INT08U                opt_len;
    CPU_INT08U                opt_len_opt;
    CPU_INT08U                opt_len_param;
    CPU_INT08U                opt_route_ix;
    CPU_BOOLEAN               opt_route_spec;
    NET_IP_ADDR               opt_route_addr;


    popt_cfg_route_ts = (NET_IP_OPT_CFG_ROUTE_TS *)popt_route_ts;

                                                                /* ------------------ VALIDATE TYPE ------------------- */
    switch (popt_cfg_route_ts->Type) {
        case NET_IP_OPT_CFG_TYPE_ROUTE_STRICT:
             opt_nbr_min    = NET_IP_OPT_PARAM_NBR_MIN;
             opt_nbr_max    = NET_IP_OPT_PARAM_NBR_MAX_ROUTE;
             opt_len_opt    = NET_IP_HDR_OPT_SIZE_ROUTE;
             opt_len_param  = sizeof(NET_IP_ADDR);
             opt_route_spec = DEF_YES;
             break;


        case NET_IP_OPT_CFG_TYPE_ROUTE_LOOSE:
             opt_nbr_min    = NET_IP_OPT_PARAM_NBR_MIN;
             opt_nbr_max    = NET_IP_OPT_PARAM_NBR_MAX_ROUTE;
             opt_len_opt    = NET_IP_HDR_OPT_SIZE_ROUTE;
             opt_len_param  = sizeof(NET_IP_ADDR);
             opt_route_spec = DEF_YES;
             break;


        case NET_IP_OPT_CFG_TYPE_ROUTE_REC: 
             opt_nbr_min    = NET_IP_OPT_PARAM_NBR_MIN;
             opt_nbr_max    = NET_IP_OPT_PARAM_NBR_MAX_ROUTE;
             opt_len_opt    = NET_IP_HDR_OPT_SIZE_ROUTE;
             opt_len_param  = sizeof(NET_IP_ADDR);
             opt_route_spec = DEF_NO;
             break;


        case NET_IP_OPT_CFG_TYPE_TS_ONLY:
             opt_nbr_min    = NET_IP_OPT_PARAM_NBR_MIN;
             opt_nbr_max    = NET_IP_OPT_PARAM_NBR_MAX_TS_ONLY;
             opt_len_opt    = NET_IP_HDR_OPT_SIZE_TS;
             opt_len_param  = sizeof(NET_TS);
             opt_route_spec = DEF_NO;
             break;


        case NET_IP_OPT_CFG_TYPE_TS_ROUTE_REC:
             opt_nbr_min    = NET_IP_OPT_PARAM_NBR_MIN;
             opt_nbr_max    = NET_IP_OPT_PARAM_NBR_MAX_TS_ROUTE;
             opt_len_opt    = NET_IP_HDR_OPT_SIZE_TS;
             opt_len_param  = sizeof(NET_IP_ADDR) + sizeof(NET_TS);
             opt_route_spec = DEF_NO;
             break;


        case NET_IP_OPT_CFG_TYPE_TS_ROUTE_SPEC:
             opt_nbr_min    = NET_IP_OPT_PARAM_NBR_MIN;
             opt_nbr_max    = NET_IP_OPT_PARAM_NBR_MAX_TS_ROUTE;
             opt_len_opt    = NET_IP_HDR_OPT_SIZE_TS;
             opt_len_param  = sizeof(NET_IP_ADDR) + sizeof(NET_TS);
             opt_route_spec = DEF_YES;
             break;


        case NET_IP_OPT_CFG_TYPE_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxOptTypeCtr);
            *perr = NET_IP_ERR_INVALID_OPT_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

/*$PAGE*/
                                                                /* ------------------- VALIDATE NBR ------------------- */
    if (popt_cfg_route_ts->Nbr < opt_nbr_min) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrOptLenCtr);
       *perr = NET_IP_ERR_INVALID_OPT_LEN;
        return;
    }
    if (popt_cfg_route_ts->Nbr > opt_nbr_max) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrOptLenCtr);
       *perr = NET_IP_ERR_INVALID_OPT_LEN;
        return;
    }

                                                                /* ------------------ VALIDATE ROUTE ------------------ */
    if (opt_route_spec == DEF_YES) {                            /* For specified routes ...                             */
                                                                /* ... validate all route addrs (see Note #1b3).        */
        for (opt_route_ix = 0; opt_route_ix < popt_cfg_route_ts->Nbr; opt_route_ix++) {

            opt_route_addr = popt_cfg_route_ts->Route[opt_route_ix];

            if ((opt_route_addr & NET_IP_ADDR_CLASS_A_MASK) != NET_IP_ADDR_CLASS_A) {
                if ((opt_route_addr & NET_IP_ADDR_CLASS_B_MASK) != NET_IP_ADDR_CLASS_B) {
                    if ((opt_route_addr & NET_IP_ADDR_CLASS_C_MASK) != NET_IP_ADDR_CLASS_C) {
                        NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrOptCfgCtr);
                       *perr = NET_IP_ERR_INVALID_OPT_ROUTE;
                        return;
                    }
                }
            }
        }
    }

                                                                /* ------------------- VALIDATE TS -------------------- */
                                                                /* See Note #1b4.                                       */


                                                                /* ------------------- RTN OPT VALS ------------------- */
    opt_len   = opt_len_opt + (popt_cfg_route_ts->Nbr * opt_len_param);
   *popt_len  = opt_len;
   *popt_next = popt_cfg_route_ts->NextOptPtr;
   *perr      = NET_IP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetIP_TxPkt()
*
* Description : (1) Prepare IP header & transmit IP packet :
*
*                   (a) Prepare   IP options (if any)
*                   (b) Calculate IP header buffer controls
*                   (c) Check for transmit fragmentation        See Note #2
*                   (d) Prepare   IP header
*                   (e) Transmit  IP packet datagram
*
*
* Argument(s) : pbuf        Pointer to network buffer to transmit IP packet.
*               ----        Argument checked   in NetIP_Tx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetIP_Tx().
*
*               addr_src    Source      IP address.
*               --------    Argument checked   in NetIP_TxPktValidate().
*
*               addr_dest   Destination IP address.
*               ---------   Argument checked   in NetIP_TxPktValidate().
*
*               TOS         Specific TOS to transmit IP packet
*               ---             (see 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
*
*                           Argument checked   in NetIP_TxPktValidate().
*
*               TTL         Specific TTL to transmit IP packet 
*               ---             (see 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES') :
*
*                               NET_IP_HDR_TTL_MIN        1     minimum TTL transmit value
*                               NET_IP_HDR_TTL_MAX      255     maximum TTL transmit value
*                               NET_IP_HDR_TTL_DFLT             default TTL transmit value
*                               NET_IP_HDR_TTL_NONE       0     replace with default TTL
*
*                           Argument validated in NetIP_TxPktValidate().
*
*               flags       Flags to select transmit options; bit-field flags logically OR'd :
*               -----
*                               NET_IP_FLAG_NONE                No  IP transmit flags selected.
*                               NET_IP_FLAG_TX_DONT_FRAG        Set IP 'Don't Frag' flag.
*
*                           Argument checked   in NetIP_TxPktValidate().
*
*               popts       Pointer to one or more IP options configuration data structures
*               -----           (see 'net_ip.h  IP HEADER OPTION CONFIGURATION DATA TYPES') :
*
*                               NULL                            NO IP transmit options configuration.
*                               NET_IP_OPT_CFG_ROUTE_TS         Route &/or Internet Timestamp options configuration.
*                               NET_IP_OPT_CFG_SECURITY         Security options configuration
*                                                                   (see 'net_ip.h  Note #1f').
*
*                           Argument checked   in NetIP_TxPktValidate().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_INVALID_LEN_HDR          Invalid IP header length.
*                               NET_IP_ERR_INVALID_FRAG             Invalid IP fragmentation.
*                               NET_ERR_INVALID_PROTOCOL            Invalid/unknown protocol type.
*
*                                                                   --- RETURNED BY NetIP_TxPktPrepareOpt() : ----
*                               NET_IP_ERR_INVALID_OPT_TYPE         Invalid IP option type.
*                               NET_IP_ERR_INVALID_OPT_LEN          Invalid IP option length.
*
*                                                                   --- RETURNED BY NetIP_TxPktPrepareHdr() : ----
*                               NET_BUF_ERR_INVALID_IX              Invalid/insufficient buffer index.
*                               NET_BUF_ERR_INVALID_LEN             Invalid buffer length.
*                               NET_UTIL_ERR_NULL_PTR               Check-sum passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE              Check-sum passed a zero size.
*
*                                                                   ---- RETURNED BY NetIP_TxPktDatagram() : -----
*                               NET_IP_ERR_NONE                     IP datagram successfully received & processed.
*                               NET_IF_ERR_NONE                     Packet successfully transmitted.
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host    address.
*                               NET_IP_ERR_INVALID_ADDR_NET         Invalid IP net     address.
*                               NET_IP_ERR_INVALID_ADDR_GATEWAY     Invalid IP gateway address.
*                               NET_IP_ERR_TX_DEST_INVALID          Invalid transmit destination.
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_ERR_RX                          Receive  error; packet discarded.
*                               NET_ERR_TX                          Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_Tx().
*
* Note(s)     : (2) IP transmit fragmentation NOT currently supported (see 'net_ip.c  Note #1e').
*
*               (3) Default case already invalidated in NetIP_TxPktValidate().  However, the default case
*                   is included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetIP_TxPkt (NET_BUF      *pbuf,
                           NET_BUF_HDR  *pbuf_hdr,
                           NET_IP_ADDR   addr_src,
                           NET_IP_ADDR   addr_dest,
                           NET_IP_TOS    TOS,
                           NET_IP_TTL    TTL,
                           CPU_INT16U    flags,
                           void         *popts,
                           NET_ERR      *perr)
{
#if 0                                                           /* #### NOT currently implemented (see Note #2).        */
    CPU_BOOLEAN      flag_dont_frag;
#endif
    CPU_INT08U       ip_opt_len_size;
    CPU_INT16U       ip_hdr_len_size;
    CPU_INT16U       protocol_ix;
    NET_MTU          ip_mtu;
    NET_IP_OPT_SIZE  ip_hdr_opts[NET_IP_HDR_OPT_NBR_MAX];
    CPU_BOOLEAN      ip_tx_frag;


                                                                /* ----------------- PREPARE IP OPTS ------------------ */
    if (popts != (void *)0) {
        ip_opt_len_size = NetIP_TxPktPrepareOpt((void       *) popts,
                                                (CPU_INT08U *)&ip_hdr_opts[0],
                                                (NET_ERR    *) perr);
        if (*perr != NET_IP_ERR_NONE) {
             return;
        }
    } else {
        ip_opt_len_size = 0;
    }


                                                                /* ---------------- CALC IP HDR CTRLS ----------------- */
                                                                /* Calc tot IP hdr len (in octets).                     */
    ip_hdr_len_size = (CPU_INT16U)(NET_IP_HDR_SIZE_MIN + ip_opt_len_size);
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (ip_hdr_len_size > NET_IP_HDR_SIZE_MAX) {
       *perr = NET_IP_ERR_INVALID_LEN_HDR;
        return;
    }
#endif

    switch (pbuf_hdr->ProtocolHdrType) {
        case NET_PROTOCOL_TYPE_ICMP:
             protocol_ix = pbuf_hdr->ICMP_MsgIx;
             break;


        case NET_PROTOCOL_TYPE_UDP:
#ifdef  NET_TCP_MODULE_PRESENT
        case NET_PROTOCOL_TYPE_TCP:
#endif
             protocol_ix = pbuf_hdr->TCP_UDP_HdrDataIx;
             break;


        case NET_PROTOCOL_TYPE_NONE:
        default:                                                /* See Note #3.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxInvalidProtocolCtr);
            *perr = NET_ERR_INVALID_PROTOCOL;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

/*$PAGE*/
                                                                /* ----------------- CHK FRAG REQUIRED ---------------- */
    ip_tx_frag = DEF_NO;
    if (protocol_ix < ip_hdr_len_size) {                        /* If hdr len > allowed rem ix, tx frag req'd.          */
        ip_tx_frag = DEF_YES;
    }

    ip_mtu = NET_IF_MTU  - ip_hdr_len_size;
    if (pbuf_hdr->TotLen > ip_mtu) {                            /* If tot len > MTU,            tx frag req'd.          */
        ip_tx_frag = DEF_YES;
    }


    if (ip_tx_frag == DEF_NO) {                                 /* If tx frag NOT required, ...                         */

        NetIP_TxPktPrepareHdr(pbuf,                             /* ... prepare IP hdr       ...                         */
                              pbuf_hdr,
                              ip_hdr_len_size,
                              ip_opt_len_size,
                              protocol_ix,
                              addr_src,
                              addr_dest,
                              TOS,
                              TTL,
                              flags,
                             &ip_hdr_opts[0],
                              perr);

        if (*perr != NET_IP_ERR_NONE) {
             return;
        }

        NetIP_TxPktDatagram(pbuf, pbuf_hdr, perr);              /* ... & tx IP datagram.                                */

    } else {
#if 0                                                           /* #### NOT currently implemented (see Note #2).        */
        flag_dont_frag = DEF_BIT_IS_SET(flags, NET_IP_FLAG_TX_DONT_FRAG);
        if (flag_dont_frag != DEF_NO) {
           *perr = NET_IP_ERR_INVALID_FRAG;
            return;
        }
#else
       *perr = NET_IP_ERR_INVALID_FRAG;
        return;
#endif
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_TxPktPrepareOpt()
*
* Description : (1) Prepare IP header with IP transmit options :
*
*                   (a) Prepare ALL IP options from configuration 
*                           data structure(s)
*                   (b) Pad remaining IP header octets              See RFC #791, Section 3.1 'Padding'
*
*               (2) IP transmit options MUST be configured by appropriate options configuration data structure(s) 
*                   passed via 'popts'; see 'net_ip.h  IP HEADER OPTION CONFIGURATION DATA TYPES' for IP options
*                   configuration.
*
*               (3) Convert ALL IP options' multi-octet words from host-order to network-order.
*
*
* Argument(s) : popts       Pointer to one or more IP options configuration data structures (see Note #2) :
*               -----
*                               NULL                            NO IP transmit options configuration.
*                               NET_IP_OPT_CFG_ROUTE_TS         Route &/or Internet Timestamp options configuration.
*                               NET_IP_OPT_CFG_SECURITY         Security options configuration
*                                                                   (see 'net_ip.h  Note #1f').
*
*                           Argument checked   in NetIP_TxPkt().
*
*               popt_hdr    Pointer to IP transmit option buffer to prepare IP options.
*               --------    Argument validated in NetIP_TxPkt().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 IP header options successfully prepared.
*                               NET_IP_ERR_INVALID_OPT_TYPE     Invalid IP option type.
*                               NET_IP_ERR_INVALID_OPT_LEN      Invalid IP option length.
*
* Return(s)   : Total IP option length (in octets), if NO errors.
*
*               0,                                  otherwise.
*
* Caller(s)   : NetIP_TxPkt().
*
* Note(s)     : (4) Transmit arguments & options validated in NetIP_TxPktValidate()/NetIP_TxPktValidateOpt() :
*
*                   (a) Assumes ALL   transmit arguments & options are valid
*                   (b) Assumes total transmit options' lengths    are valid
*
*               (5) IP header allows for a maximum option size of 40 octets (see 'net_ip.h  IP HEADER OPTIONS 
*                   DEFINES  Note #3').
*
*               (6) Default case already invalidated in NetIP_TxPktValidateOpt().  However, the default
*                   case is included as an extra precaution in case any of the IP transmit options types
*                   are incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
static  CPU_INT08U  NetIP_TxPktPrepareOpt (void        *popts,
                                           CPU_INT08U  *popt_hdr,
                                           NET_ERR     *perr)
{
    CPU_INT08U   ip_opt_len_tot;
    CPU_INT08U   ip_opt_len;
    CPU_INT08U  *popt_cfg_hdr;
    NET_TYPE    *popt_cfg_type;
    void        *popt_next;
    void        *popt_cfg;


    ip_opt_len_tot = 0;
    popt_cfg       = popts;
    popt_cfg_hdr   = popt_hdr;
                                                                /* ----------------- PREPARE IP OPTS ------------------ */
    while (popt_cfg  != (void *)0) {                            /* Prepare ALL cfg'd IP opts (see Note #1a).            */
        popt_cfg_type = (NET_TYPE *)popt_cfg;
        switch (*popt_cfg_type) {
            case NET_IP_OPT_CFG_TYPE_ROUTE_STRICT:
            case NET_IP_OPT_CFG_TYPE_ROUTE_LOOSE:
            case NET_IP_OPT_CFG_TYPE_ROUTE_REC: 
                 NetIP_TxPktPrepareOptRoute(popt_cfg, popt_cfg_hdr, &ip_opt_len, &popt_next, perr);
                 break;


            case NET_IP_OPT_CFG_TYPE_TS_ONLY:
                 NetIP_TxPktPrepareOptTS(popt_cfg, popt_cfg_hdr, &ip_opt_len, &popt_next, perr);
                 break;


            case NET_IP_OPT_CFG_TYPE_TS_ROUTE_REC:
            case NET_IP_OPT_CFG_TYPE_TS_ROUTE_SPEC:
                 NetIP_TxPktPrepareOptTSRoute(popt_cfg, popt_cfg_hdr, &ip_opt_len, &popt_next, perr);
                 break;

#if 0                                                           /* --------------- UNSUPPORTED IP OPTS ---------------- */
            case NET_IP_OPT_CFG_TYPE_SECURITY:
            case NET_IP_OPT_CFG_TYPE_SECURITY_EXTENDED:
                 break;
#endif
                                                                /* ----------------- INVALID IP OPTS ------------------ */
            case NET_IP_OPT_CFG_TYPE_NONE:
            default:                                            /* See Note #6.                                         */                      
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxOptTypeCtr);
                *perr =  NET_IP_ERR_INVALID_OPT_TYPE;
                 return (0);                                    /* Prevent 'break NOT reachable' compiler warning.      */
        }
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        if (*perr != NET_IP_ERR_NONE) {                         /* See Note #4a.                                        */
             return (0);
        }
        if (ip_opt_len_tot > NET_IP_HDR_OPT_SIZE_MAX) {         /* See Note #4b.                                        */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrOptLenCtr);
           *perr =  NET_IP_ERR_INVALID_OPT_LEN;
            return (0);
        }
#endif

        ip_opt_len_tot += ip_opt_len;
        popt_cfg_hdr   += ip_opt_len;

        popt_cfg        = popt_next;                            /* Prepare next cfg opt.                                */
    }


                                                                /* -------------------- PAD IP HDR -------------------- */
    if (ip_opt_len_tot > 0) {
                                                                /* Pad rem'ing IP hdr octets (see Note #1b).            */
        while ((ip_opt_len_tot  % NET_IP_HDR_OPT_SIZE_WORD) && 
               (ip_opt_len_tot <= NET_IP_HDR_OPT_SIZE_MAX )) {
           *popt_cfg_hdr = NET_IP_HDR_OPT_PAD;
            popt_cfg_hdr++;
            ip_opt_len_tot++;
        }
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        if (ip_opt_len_tot > NET_IP_HDR_OPT_SIZE_MAX) {         /* See Note #4b.                                        */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxHdrOptLenCtr);
           *perr =  NET_IP_ERR_INVALID_OPT_LEN;
            return (0);
        }
#endif
    }


   *perr =  NET_IP_ERR_NONE;

    return (ip_opt_len_tot);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetIP_TxPktPrepareOptRoute()
*
* Description : (1) Prepare IP header with IP Route transmit options :
*
*                   (a) Prepare IP Route option header
*                   (b) Prepare IP Route
*
*               (2) See RFC #791, Section 3.1 'Options : Loose/Strict Source & Record Route'.
*
*
* Argument(s) : popts       Pointer to IP Route option configuration data structure.
*               -----       Argument checked   in NetIP_TxPktPrepareOpt().
*
*               popt_hdr    Pointer to IP transmit option buffer to prepare IP Route option.
*               --------    Argument validated in NetIP_TxPkt().
*
*               popt_len    Pointer to variable that will receive the Route option length (in octets).
*               --------    Argument validated in NetIP_TxPktPrepareOpt().
*
*               popt_next   Pointer to variable that will receive the pointer to the next IP transmit option.
*               --------    Argument validated in NetIP_TxPktPrepareOpt().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 IP Route option successfully prepared.
*                               NET_IP_ERR_INVALID_OPT_TYPE     Invalid IP option type.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_TxPktPrepareOpt().
*
* Note(s)     : (3) Transmit arguments & options validated in NetIP_TxPktValidate()/NetIP_TxPktValidateOpt() :
*
*                   (a) Assumes ALL   transmit arguments & options are valid
*                   (b) Assumes total transmit options' lengths    are valid
*
*               (4) Default case already invalidated in NetIP_TxPktValidateOpt().  However, the default
*                   case is included as an extra precaution in case any of the IP transmit options types
*                   are incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetIP_TxPktPrepareOptRoute (void         *popts,
                                          CPU_INT08U   *popt_hdr,
                                          CPU_INT08U   *popt_len,
                                          void        **popt_next,
                                          NET_ERR      *perr)
{
    NET_IP_OPT_CFG_ROUTE_TS  *popt_cfg_route_ts;
    NET_IP_OPT_SRC_ROUTE     *popt_route;
    CPU_BOOLEAN               opt_route_spec;
    CPU_INT08U                opt_route_ix;
    NET_IP_ADDR               opt_route_addr;


                                                                /* -------------- PREPARE ROUTE OPT HDR --------------- */
    popt_cfg_route_ts = (NET_IP_OPT_CFG_ROUTE_TS *)popts;
    popt_route        = (NET_IP_OPT_SRC_ROUTE    *)popt_hdr;
    popt_route->Len   =  NET_IP_HDR_OPT_SIZE_ROUTE + (popt_cfg_route_ts->Nbr * sizeof(NET_IP_ADDR));
    popt_route->Ptr   =  NET_IP_OPT_ROUTE_PTR_ROUTE;
    popt_route->Pad   =  NET_IP_HDR_OPT_PAD;                    /* ???? PAD &/or NOP val?                               */

    switch (popt_cfg_route_ts->Type) {
        case NET_IP_OPT_CFG_TYPE_ROUTE_STRICT:
             popt_route->Type = NET_IP_HDR_OPT_ROUTE_SRC_STRICT;
             opt_route_spec   = DEF_YES;
             break;


        case NET_IP_OPT_CFG_TYPE_ROUTE_LOOSE:
             popt_route->Type = NET_IP_HDR_OPT_ROUTE_SRC_LOOSE;
             opt_route_spec   = DEF_YES;
             break;


        case NET_IP_OPT_CFG_TYPE_ROUTE_REC: 
             popt_route->Type = NET_IP_HDR_OPT_ROUTE_REC;
             opt_route_spec   = DEF_NO;
             break;


        case NET_IP_OPT_CFG_TYPE_NONE:
        default:                                                /* See Note #4.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxOptTypeCtr);
            *perr = NET_IP_ERR_INVALID_OPT_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ------------------ PREPARE ROUTE ------------------- */
    for (opt_route_ix = 0; opt_route_ix < popt_cfg_route_ts->Nbr; opt_route_ix++) {
                                                                /* Cfg specified or rec route addrs.                    */
        if (opt_route_spec == DEF_YES) {
            opt_route_addr =  popt_cfg_route_ts->Route[opt_route_ix];
        } else {
            opt_route_addr = (NET_IP_ADDR)NET_IP_ADDR_NONE;
        }
        NET_UTIL_VAL_COPY_SET_NET_32(&popt_route->Route[opt_route_ix], &opt_route_addr);
    }


   *popt_len  = popt_route->Len;
   *popt_next = popt_cfg_route_ts->NextOptPtr;
   *perr      = NET_IP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIP_TxPktPrepareOptTS()
*
* Description : (1) Prepare IP header with Internet Timestamp option :
*
*                   (a) Prepare Internet Timestamp option header
*                   (b) Prepare Internet Timestamps
*
*               (2) See RFC #791, Section 3.1 'Options : Internet Timestamp'.
*
*
* Argument(s) : popts       Pointer to Internet Timestamp option configuration data structure.
*               -----       Argument checked   in NetIP_TxPktPrepareOpt().
*
*               popt_hdr    Pointer to IP transmit option buffer to prepare Internet Timestamp option.
*               --------    Argument validated in NetIP_TxPkt().
*
*               popt_len    Pointer to variable that will receive the Internet Timestamp option length 
*               --------        (in octets).
*
*                           Argument validated in NetIP_TxPktPrepareOpt().
*
*               popt_next   Pointer to variable that will receive the pointer to the next IP transmit option.
*               --------    Argument validated in NetIP_TxPktPrepareOpt().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 Internet Timestamp option successfully prepared.
*                               NET_IP_ERR_INVALID_OPT_TYPE     Invalid IP option type.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_TxPktPrepareOpt().
*
* Note(s)     : (3) Transmit arguments & options validated in NetIP_TxPktValidate()/NetIP_TxPktValidateOpt() :
*
*                   (a) Assumes ALL   transmit arguments & options are valid
*                   (b) Assumes total transmit options' lengths    are valid
*
*               (4) Default case already invalidated in NetIP_TxPktValidateOpt().  However, the default
*                   case is included as an extra precaution in case any of the IP transmit options types
*                   are incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetIP_TxPktPrepareOptTS (void         *popts,
                                       CPU_INT08U   *popt_hdr,
                                       CPU_INT08U   *popt_len,
                                       void        **popt_next,
                                       NET_ERR      *perr)
{
    NET_IP_OPT_CFG_ROUTE_TS  *popt_cfg_route_ts;
    NET_IP_OPT_TS            *popt_ts;
    CPU_INT08U                opt_ts_ovf;
    CPU_INT08U                opt_ts_flags;
    CPU_INT08U                opt_ts_ix;
    NET_TS                    opt_ts;


                                                                /* ---------------- PREPARE TS OPT HDR ---------------- */
    popt_cfg_route_ts = (NET_IP_OPT_CFG_ROUTE_TS *)popts;

    switch (popt_cfg_route_ts->Type) {
        case NET_IP_OPT_CFG_TYPE_TS_ONLY:
             popt_ts            = (NET_IP_OPT_TS *)popt_hdr;
             popt_ts->Type      =  NET_IP_HDR_OPT_TS;
             popt_ts->Len       =  NET_IP_HDR_OPT_SIZE_TS + (popt_cfg_route_ts->Nbr * sizeof(NET_TS));
             popt_ts->Ptr       =  NET_IP_OPT_TS_PTR_TS;

             opt_ts_ovf         =  0;
             opt_ts_flags       =  NET_IP_OPT_TS_FLAG_TS_ONLY;
             popt_ts->Ovf_Flags =  opt_ts_ovf | opt_ts_flags;
             break;


        case NET_IP_OPT_CFG_TYPE_NONE:
        default:                                                /* See Note #4.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxOptTypeCtr);
            *perr = NET_IP_ERR_INVALID_OPT_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* -------------------- PREPARE TS -------------------- */
    for (opt_ts_ix = 0; opt_ts_ix < popt_cfg_route_ts->Nbr; opt_ts_ix++) {
        opt_ts = popt_cfg_route_ts->TS[opt_ts_ix];
        NET_UTIL_VAL_COPY_SET_NET_32(&popt_ts->TS[opt_ts_ix], &opt_ts);
    }


   *popt_len  = popt_ts->Len;
   *popt_next = popt_cfg_route_ts->NextOptPtr;
   *perr      = NET_IP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetIP_TxPktPrepareOptTSRoute()
*
* Description : (1) Prepare IP header with Internet Timestamp with IP Route option :
*
*                   (a) Prepare Internet Timestamp option header
*                   (b) Prepare Internet Timestamps
*
*               (2) See RFC #791, Section 3.1 'Options : Internet Timestamp'.
*
*
* Argument(s) : popts       Pointer to Internet Timestamp option configuration data structure.
*               -----       Argument checked   in NetIP_TxPktPrepareOpt().
*
*               popt_hdr    Pointer to IP transmit option buffer to prepare Internet Timestamp option.
*               --------    Argument validated in NetIP_TxPkt().
*
*               popt_len    Pointer to variable that will receive the Internet Timestamp option length 
*               --------    Argument validated in NetIP_TxPktPrepareOpt().
*                                   (in octets).
*
*               popt_next   Pointer to variable that will receive the pointer to the next IP transmit option.
*               --------    Argument validated in NetIP_TxPktPrepareOpt().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 Internet Timestamp with IP Route option 
*                                                                   successfully prepared.
*                               NET_IP_ERR_INVALID_OPT_TYPE     Invalid IP option type.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_TxPktPrepareOpt().
*
* Note(s)     : (3) Transmit arguments & options validated in NetIP_TxPktValidate()/NetIP_TxPktValidateOpt() :
*
*                   (a) Assumes ALL   transmit arguments & options are valid
*                   (b) Assumes total transmit options' lengths    are valid
*
*               (4) Default case already invalidated in NetIP_TxPktValidateOpt().  However, the default
*                   case is included as an extra precaution in case any of the IP transmit options types
*                   are incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetIP_TxPktPrepareOptTSRoute (void         *popts,
                                            CPU_INT08U   *popt_hdr,
                                            CPU_INT08U   *popt_len,
                                            void        **popt_next,
                                            NET_ERR      *perr)
{
    NET_IP_OPT_CFG_ROUTE_TS  *popt_cfg_route_ts;
    NET_IP_OPT_TS_ROUTE      *popt_ts_route;
    NET_IP_ROUTE_TS          *proute_ts;
    CPU_INT08U                opt_ts_ovf;
    CPU_INT08U                opt_ts_flags;
    CPU_INT08U                opt_ts_ix;
    NET_TS                    opt_ts;
    CPU_BOOLEAN               opt_route_spec;
    NET_IP_ADDR               opt_route_addr;


                                                                /* ---------------- PREPARE TS OPT HDR ---------------- */
    popt_cfg_route_ts   = (NET_IP_OPT_CFG_ROUTE_TS *)popts;
    popt_ts_route       = (NET_IP_OPT_TS_ROUTE     *)popt_hdr;
    popt_ts_route->Type =  NET_IP_HDR_OPT_TS;
    popt_ts_route->Len  =  NET_IP_HDR_OPT_SIZE_TS + (popt_cfg_route_ts->Nbr * (sizeof(NET_IP_ADDR) + sizeof(NET_TS)));
    popt_ts_route->Ptr  =  NET_IP_OPT_TS_PTR_TS;
    opt_ts_ovf          =  0;

    switch (popt_cfg_route_ts->Type) {
        case NET_IP_OPT_CFG_TYPE_TS_ROUTE_REC:
             opt_ts_flags             = NET_IP_OPT_TS_FLAG_TS_ROUTE_REC;
             popt_ts_route->Ovf_Flags = opt_ts_ovf | opt_ts_flags;
             opt_route_spec           = DEF_NO;
             break;


        case NET_IP_OPT_CFG_TYPE_TS_ROUTE_SPEC:
             opt_ts_flags             = NET_IP_OPT_TS_FLAG_TS_ROUTE_SPEC;
             popt_ts_route->Ovf_Flags = opt_ts_ovf | opt_ts_flags;
             opt_route_spec           = DEF_YES;
             break;


        case NET_IP_OPT_CFG_TYPE_NONE:
        default:                                                /* See Note #4.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxOptTypeCtr);
            *perr = NET_IP_ERR_INVALID_OPT_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ----------------- PREPARE ROUTE/TS ----------------- */
    proute_ts = &popt_ts_route->Route_TS[0];
    for (opt_ts_ix = 0; opt_ts_ix < popt_cfg_route_ts->Nbr; opt_ts_ix++) {
                                                                /* Cfg specified or rec route addrs.                    */
        if (opt_route_spec == DEF_YES) {
            opt_route_addr =  popt_cfg_route_ts->Route[opt_ts_ix];
        } else {
            opt_route_addr = (NET_IP_ADDR)NET_IP_ADDR_NONE;
        }
        NET_UTIL_VAL_COPY_SET_NET_32(&proute_ts->Route[opt_ts_ix], &opt_route_addr);

        opt_ts = popt_cfg_route_ts->TS[opt_ts_ix];
        NET_UTIL_VAL_COPY_SET_NET_32(&proute_ts->TS[opt_ts_ix],    &opt_ts);
    }


   *popt_len  = popt_ts_route->Len;
   *popt_next = popt_cfg_route_ts->NextOptPtr;
   *perr      = NET_IP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIP_TxPktPrepareHdr()
*
* Description : (1) Prepare IP header :
*
*                   (a) Update network buffer's protocol index & length controls
*
*                   (b) Prepare the transmit packet's following IP header fields :
*
*                       (1) Version
*                       (2) Header Length
*                       (3) Type of Service (TOS)
*                       (4) Total  Length
*                       (5) Identification  (ID)
*                       (6) Flags
*                       (7) Fragment Offset
*                       (8) Time-to-Live    (TTL)
*                       (9) Protocol
*                      (10) Check-Sum                                   See Note #6
*                      (11) Source      Address
*                      (12) Destination Address
*                      (13) Options
*
*                   (c) Convert the following IP header fields from host-order to network-order :
*
*                       (1) Total Length
*                       (2) Identification (ID)
*                       (3) Flags/Fragment Offset
*                       (4) Source      Address
*                       (5) Destination Address
*                       (6) Check-Sum                                   See Note #6c
*                       (7) Options                                     See Note #5
*
*
* Argument(s) : pbuf                Pointer to network buffer to transmit IP packet.
*               ----                Argument checked   in NetIP_Tx().
*
*               pbuf_hdr            Pointer to network buffer header.
*               --------            Argument validated in NetIP_Tx().
*
*               ip_hdr_len_tot      Total IP header length.
*               --------------      Argument checked   in NetIP_TxPkt().
*               
*               ip_opt_len_tot      Total IP header options' length.
*               --------------      Argument checked   in NetIP_TxPktPrepareOpt().
*
*               protocol_ix         Index to higher-layer protocol header.
*
*               addr_src            Source      IP address.
*               --------            Argument checked   in NetIP_TxPktValidate().
*
*               addr_dest           Destination IP address.
*               ---------           Argument checked   in NetIP_TxPktValidate().
*
*               TOS                 Specific TOS to transmit IP packet
*               ---                     (see 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
*
*                                   Argument checked   in NetIP_TxPktValidate().
*
*               TTL                 Specific TTL to transmit IP packet 
*               ---                     (see 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES') :
*
*                                       NET_IP_HDR_TTL_MIN        1     minimum TTL transmit value
*                                       NET_IP_HDR_TTL_MAX      255     maximum TTL transmit value
*                                       NET_IP_HDR_TTL_DFLT             default TTL transmit value
*                                       NET_IP_HDR_TTL_NONE       0     replace with default TTL
*
*                                   Argument validated in NetIP_TxPktValidate().
*
*               flags               Flags to select transmit options; bit-field flags logically OR'd :
*               -----
*                                       NET_IP_FLAG_NONE                No  IP transmit flags selected.
*                                       NET_IP_FLAG_TX_DONT_FRAG        Set IP 'Don't Frag' flag.
*
*                                   Argument checked   in NetIP_TxPktValidate().
*
*               pip_hdr_opts        Pointer to IP options buffer.
*               ------------        Argument checked   in NetIP_TxPktPrepareOpt().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 IP header successfully prepared.
*                               NET_ERR_INVALID_PROTOCOL        Invalid/unknown protocol type.
*
*                                                               ----------- RETURNED BY NetBuf_DataWr() : -----------
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index  for transmit options.
*                               NET_BUF_ERR_INVALID_LEN         Invalid buffer length for transmit options.
*
*                                                               - RETURNED BY NetUtil_16BitOnesCplChkSumHdrCalc() : -
*                               NET_UTIL_ERR_NULL_PTR           Check-sum passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Check-sum passed a zero size.
*$PAGE*
* Return(s)   : none.
*
* Caller(s)   : NetIP_TxPkt().
*
* Note(s)     : (2) See 'net_ip.h  IP HEADER' for IP header format.
*
*               (3) Supports ONLY the following protocols :
*
*                   (a) ICMP
*                   (b) UDP
*                   (c) TCP
*
*                   See also 'net.h  Note #1a'.
*
*               (4) Default case already invalidated in NetIP_TxPktValidate().  However, the default case is
*                   included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
*
*               (5) Assumes ALL IP options' multi-octet words previously converted from host-order to 
*                   network-order.
*
*               (6) (a) IP header Check-Sum MUST be calculated AFTER the entire IP header has been prepared.
*                       In addition, ALL multi-octet words are converted from host-order to network-order 
*                       since "the sum of 16-bit integers can be computed in either byte order" [RFC #1071, 
*                       Section 2.(B)].
*
*                   (b) IP header Check-Sum field MUST be cleared to '0' BEFORE the IP header Check-Sum is 
*                       calculated (see RFC #791, Section 3.1 'Header Checksum').
*
*                   (c) The IP header Check-Sum field is returned in network-order & MUST NOT be re-converted 
*                       back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()  Note #3b').
*********************************************************************************************************
*/

static  void  NetIP_TxPktPrepareHdr (NET_BUF      *pbuf,
                                     NET_BUF_HDR  *pbuf_hdr,
                                     CPU_INT16U    ip_hdr_len_tot,
                                     CPU_INT08U    ip_opt_len_tot,
                                     CPU_INT16U    protocol_ix,
                                     NET_IP_ADDR   addr_src,
                                     NET_IP_ADDR   addr_dest,
                                     NET_IP_TOS    TOS,
                                     NET_IP_TTL    TTL,
                                     CPU_INT16U    flags,
                                     CPU_INT32U   *pip_hdr_opts,
                                     NET_ERR      *perr)
{
    NET_IP_HDR  *pip_hdr;
    CPU_INT08U   ip_ver;
    CPU_INT08U   ip_hdr_len;
    CPU_INT16U   ip_id;
    CPU_INT16U   ip_flags;
    CPU_INT16U   ip_frag_offset;
    CPU_INT16U   ip_opt_ix;
    CPU_INT16U   ip_flags_frag_offset;
    CPU_INT16U   ip_chk_sum;


                                                                /* ----------------- UPDATE BUF CTRLS ----------------- */
    pbuf_hdr->IP_HdrLen       =  ip_hdr_len_tot;
    pbuf_hdr->IP_HdrIx        =  protocol_ix - pbuf_hdr->IP_HdrLen;

    pbuf_hdr->IP_DataLen      = (CPU_INT16U  ) pbuf_hdr->TotLen;
    pbuf_hdr->IP_DatagramLen  = (CPU_INT16U  ) pbuf_hdr->TotLen;
    pbuf_hdr->TotLen         += (NET_BUF_SIZE) pbuf_hdr->IP_HdrLen;
    pbuf_hdr->IP_TotLen       = (CPU_INT16U  ) pbuf_hdr->TotLen;



                                                                /* ------------------ PREPARE IP HDR ------------------ */
    pip_hdr = (NET_IP_HDR *)&pbuf->Data[pbuf_hdr->IP_HdrIx];



                                                                /* -------------- PREPARE IP VER/HDR LEN -------------- */
    ip_ver               = NET_IP_HDR_VER;
    ip_ver             <<= NET_IP_HDR_VER_SHIFT;

    ip_hdr_len           = pbuf_hdr->IP_HdrLen / NET_IP_HDR_LEN_WORD_SIZE;
    ip_hdr_len          &= NET_IP_HDR_LEN_MASK;

    pip_hdr->Ver_HdrLen  = ip_ver | ip_hdr_len;



/*$PAGE*/
                                                                /* ------------------ PREPARE IP TOS ------------------ */
    pip_hdr->TOS = TOS;



                                                                /* ---------------- PREPARE IP TOT LEN ---------------- */
    NET_UTIL_VAL_COPY_SET_NET_16(&pip_hdr->TotLen, &pbuf_hdr->TotLen);



                                                                /* ------------------ PREPARE IP ID ------------------- */
    NET_IP_TX_GET_ID(ip_id);
    NET_UTIL_VAL_COPY_SET_NET_16(&pip_hdr->ID, &ip_id);



                                                                /* --------------- PREPARE IP FLAGS/FRAG -------------- */
    ip_flags  = NET_IP_HDR_FLAG_NONE;
    ip_flags |= flags;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    ip_flags &= NET_IP_HDR_FLAG_MASK;
#endif

    ip_frag_offset       = NET_IP_HDR_FRAG_OFFSET_NONE;

    ip_flags_frag_offset = ip_flags | ip_frag_offset;
    NET_UTIL_VAL_COPY_SET_NET_16(&pip_hdr->Flags_FragOffset, &ip_flags_frag_offset);



                                                                /* ------------------ PREPARE IP TTL ------------------ */
    if (TTL != NET_IP_HDR_TTL_NONE) {
        pip_hdr->TTL = TTL;
    } else {
        pip_hdr->TTL = NET_IP_HDR_TTL_DFLT;
    }



                                                                /* --------------- PREPARE IP PROTOCOL ---------------- */
    switch (pbuf_hdr->ProtocolHdrType) {                        /* Demux IP protocol (see Note #3).                     */
        case NET_PROTOCOL_TYPE_ICMP:
             pip_hdr->Protocol = NET_IP_HDR_PROTOCOL_ICMP;
             break;


        case NET_PROTOCOL_TYPE_UDP:
             pip_hdr->Protocol = NET_IP_HDR_PROTOCOL_UDP;
             break;


#ifdef  NET_TCP_MODULE_PRESENT
        case NET_PROTOCOL_TYPE_TCP:
             pip_hdr->Protocol = NET_IP_HDR_PROTOCOL_TCP;
             break;
#endif

        case NET_PROTOCOL_TYPE_NONE:
        default:                                                /* See Note #4.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxInvalidProtocolCtr);
            *perr = NET_ERR_INVALID_PROTOCOL;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

    pbuf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V4;        /* Update buf protocol for IP.                          */



                                                                /* ----------------- PREPARE IP ADDRS ----------------- */
    pbuf_hdr->IP_AddrSrc  = addr_src;
    pbuf_hdr->IP_AddrDest = addr_dest;

    NET_UTIL_VAL_COPY_SET_NET_32(&pip_hdr->AddrSrc,  &addr_src);
    NET_UTIL_VAL_COPY_SET_NET_32(&pip_hdr->AddrDest, &addr_dest);



/*$PAGE*/
                                                                /* ----------------- PREPARE IP OPTS ------------------ */
    if (ip_opt_len_tot > 0) {
        ip_opt_ix = pbuf_hdr->IP_HdrIx + NET_IP_HDR_OPT_IX;
        NetBuf_DataWr((NET_BUF    *)pbuf,                       /* See Note #5.                                         */
                      (NET_BUF_SIZE)ip_opt_ix,
                      (NET_BUF_SIZE)ip_opt_len_tot,
                      (CPU_INT08U *)pip_hdr_opts,
                      (NET_ERR    *)perr);
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        if (*perr != NET_BUF_ERR_NONE) {
             return;
        }
#endif
    }


                                                                /* ---------------- PREPARE IP CHK SUM ---------------- */
                                                                /* See Note #6.                                         */
    NET_UTIL_VAL_SET_NET_16(&pip_hdr->ChkSum, 0x0000);          /* Clr  chk sum (see Note #6b).                         */
                                                                /* Calc chk sum.                                        */
    ip_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc((void     *)pip_hdr,
                                                   (CPU_INT16U)ip_hdr_len_tot,
                                                   (NET_ERR  *)perr);
    if (*perr != NET_UTIL_ERR_NONE) {
         return;
    }

    NET_UTIL_VAL_COPY_16(&pip_hdr->ChkSum, &ip_chk_sum);        /* Copy chk sum in net order (see Note #6c).            */



   *perr = NET_IP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIP_TxPktDatagram()
*
* Description : (1) Transmit IP packet datagram :
*
*                   (a) Select next-route IP address
*                   (b) Transmit IP packet datagram via next IP address route :
*
*                       (1) Destination is this host                Reflect back to IP Receive
*                           (A) Configured host address
*                           (B) Localhost       address
*
*                       (2) Limited Broadcast                       Send to Network Interface Transmit
*                       (3) Local   Host                            Send to Network Interface Transmit
*                       (4) Remote  Host                            Send to Network Interface Transmit
*
*
* Argument(s) : pbuf        Pointer to network buffer to transmit IP packet.
*               ----        Argument checked   in NetIP_Tx(),
*                                                 NetIP_ReTx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetIP_Tx(),
*                                                 NetIP_ReTx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                                   - RETURNED BY NetIP_TxPktDatagramRouteSel() : -
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host    address.
*                               NET_IP_ERR_INVALID_ADDR_NET         Invalid IP net     address.
*                               NET_IP_ERR_INVALID_ADDR_GATEWAY     Invalid IP gateway address.
*                               NET_IP_ERR_TX_DEST_INVALID          Invalid transmit destination.
*
*                                                                   ---------- RETURNED BY NetIP_Rx() : -----------
*                               NET_IP_ERR_NONE                     IP datagram successfully received & processed.
*                               NET_ERR_RX                          Receive  error; packet discarded.
*
*                                                                   ---------- RETURNED BY NetIF_Tx() : -----------
*                               NET_IF_ERR_NONE                     Packet successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_ERR_TX                          Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_TxPkt(),
*               NetIP_ReTxPkt().
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetIP_TxPktDatagram (NET_BUF      *pbuf,
                                   NET_BUF_HDR  *pbuf_hdr,
                                   NET_ERR      *perr)
{
                                                                /* --------------- SEL NEXT-ROUTE ADDR ---------------- */
    NetIP_TxPktDatagramRouteSel(pbuf_hdr, perr);


    switch (*perr) {                                            /* ---------------- TX IP PKT DATAGRAM ---------------- */
        case NET_IP_ERR_TX_DEST_LOCAL_HOST:
                                                                /* Re-cfg buf ctrls for loopback rx.                    */
             pbuf_hdr->DataLen         = (NET_BUF_SIZE)pbuf_hdr->IP_TotLen;
             pbuf_hdr->IP_HdrLen       = (CPU_INT16U  )0;
             pbuf_hdr->IP_TotLen       = (CPU_INT16U  )0;
             pbuf_hdr->IP_DataLen      = (CPU_INT16U  )0;
             pbuf_hdr->IP_DatagramLen  = (CPU_INT16U  )0;
             pbuf_hdr->ICMP_HdrLen     = (CPU_INT16U  )0;
             pbuf_hdr->ICMP_MsgLen     = (CPU_INT16U  )0;
             pbuf_hdr->TCP_UDP_HdrLen  = (CPU_INT16U  )0;
             pbuf_hdr->TCP_UDP_TotLen  = (CPU_INT16U  )0;
             pbuf_hdr->TCP_UDP_DataLen = (CPU_INT16U  )0;

             NetIP_Rx(pbuf, perr);
             break;


        case NET_IP_ERR_TX_DEST_BROADCAST:
        case NET_IP_ERR_TX_DEST_HOST_THIS_NET:
        case NET_IP_ERR_TX_DEST_DFLT_GATEWAY:
             NetIF_Tx(pbuf, perr);
             break;


        case NET_IP_ERR_TX_DEST_INVALID:
        case NET_IP_ERR_INVALID_ADDR_HOST:
        case NET_IP_ERR_INVALID_ADDR_NET:
        case NET_IP_ERR_INVALID_ADDR_GATEWAY:
        default:
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetIP_TxPktDatagramRouteSel()
*
* Description : (1) Configure  next-route IP address for transmit IP packet datagram :
*
*                   (a) Select next-route IP address :                  See Note #3
*                       (1) Destination is this host :
*                           (A) Configured host address                 See 'net_ip.c  Note #1a'
*                           (B) Localhost       address                 See RFC #1122, Section 3.2.1.3.(g)
*                       (2) Limited  Broadcast                          See Note #3b2a
*                       (3) Local  Link                                 See Note #3c
*                       (4) Local  Net Host                             See Note #3b1b
*                       (5) Remote Net Host                             See Note #3b1c
*
*                   (b) Configure next-route IP address in network-order.
*
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header of IP transmit packet.
*               --------    Argument validated in NetIP_Tx(),
*                                                 NetIP_ReTx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_TX_DEST_LOCAL_HOST       Destination is a local host address.
*                               NET_IP_ERR_TX_DEST_BROADCAST        Limited broadcast on local  network.
*                               NET_IP_ERR_TX_DEST_HOST_THIS_NET    Destination host  on local  network.
*                               NET_IP_ERR_TX_DEST_DFLT_GATEWAY     Destination host  on remote network.
*                               NET_IP_ERR_TX_DEST_INVALID          Invalid IP destination address.
*
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host        address.
*                               NET_IP_ERR_INVALID_ADDR_NET         Invalid IP net         address.
*                               NET_IP_ERR_INVALID_ADDR_GATEWAY     Invalid IP gateway     address.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_TxPktDatagram().
*$PAGE*
* Note(s)     : (2) See 'net_ip.h  IP ADDRESS DEFINES  Notes #2 & #3' for supported IP addresses.
*
*               (3) (a) (1) RFC #1122, Section 3.3.1 states that "the IP layer chooses the correct next hop 
*                           for each datagram it sends" :
*
*                           (A) "If the destination is on a connected network, the datagram is sent directly 
*                                to the destination host;" ...
*                           (B) "Otherwise, it has to be routed to a gateway on a connected network."
*
*                       (2) However, the IP layer should route datagrams destined to any internal host 
*                           to IP receive processing.
*
*                           (A) RFC #950, Section 2.2 states that :
*
*                                   "IF ip_net_number(dg.ip_dest) = ip_net_number(my_ip_addr)
*                                       THEN send_dg_locally(dg, dg.ip_dest)"
*
*                           (B) RFC #1122, Section 3.2.1.3.(g) states that the "internal host loopback 
*                               address ... MUST NOT appear outside a host".
*
*                               (1) However, this does NOT prevent the host loopback address from being 
*                                   used as an IP packet's source address as long as BOTH the packet's 
*                                   source AND destination addresses are internal host addresses, either 
*                                   the host's configured IP address or any host loopback address.
*
*                   (b) RFC #1122, Section 3.3.1.1 states that "to decide if the destination is on a 
*                       connected network, the following algorithm MUST be used" :
*
*                       (1) (b) "If the IP destination address bits extracted by the address mask match the 
*                                IP source address bits extracted by the same mask, then the destination is 
*                                on the corresponding connected network, and the datagram is ... transmitted
*                                directly to the destination host."
*
*                           (c) "If not, then the destination is accessible only through a gateway."
*
*                               (1) However, "the host IP layer MUST operate correctly in a minimal network 
*                                   environment, and in particular, when there are no gateways."
*
*                       (2) (a) "For a limited broadcast or a multicast address, simply pass the datagram 
*                                to the link layer for the appropriate interface."
*
*                           (b) "For a (network or subnet) directed broadcast, the datagram can use the 
*                                standard routing algorithms."
*
*                               (1) RFC #950, Section 2.1 'Special Addresses' confirms that the broadcast 
*                                   address for subnetted IP addresses is still "the address of all ones".
*
*                   (c) (1) RFC #3927, Section 2.6.2 states that :
*
*                           (A) (1) "If ... the host ... send[s] [a] packet with an IPv4 Link-Local source 
*                                    address ... [to] a unicast ... destination address ... outside the 
*                                    169.254/16 prefix, ... then it MUST ARP for the destination address 
*                                    and then send its packet ... directly to its destination on the same 
*                                    physical link."
*
*                               (2) "The host MUST NOT send the packet to any router for forwarding."
*
*                           (B) (1) (a) "If the destination address is in the 169.254/16 prefix ... then 
*                                        the sender MUST ARP for the destination address and then send its 
*                                        packet directly to the destination on the same physical link."
*
*                                   (b) "The host MUST NOT send a packet with an IPv4 Link-Local destination 
*                                        address to any router for forwarding."
*
*                               (2) "This MUST be done whether the interface is configured with a Link-Local 
*                                    or a routable IPv4 address."
*
*                       (2) (A) RFC #3927, Section 2.8 reiterates that "the non-forwarding rule means that 
*                               hosts may assume that all 169.254/16 destination addresses are 'on-link'
*                               and directly reachable".
*
*                           (B) RFC #3927, Section 2.6.2 concludes that "in the case of a device with a 
*                               single interface and only an Link-Local IPv4 address, this requirement 
*                               can be paraphrased as 'ARP for everything'".
*
*                       (3) (A) RFC #3927, Section 2.8   states that "the 169.254/16 address prefix MUST 
*                               NOT be subnetted".
*
*                           (B) RFC #3927, Section 2.6.2 states that "the broadcast address for the Link-
*                               Local prefix ... is ... 169.254.255.255".
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetIP_TxPktDatagramRouteSel (NET_BUF_HDR  *pbuf_hdr,
                                           NET_ERR      *perr)
{
    NET_IP_ADDR  addr_src;
    NET_IP_ADDR  addr_dest;
    CPU_BOOLEAN  addr_cfgd; 


                                                                        /* ------------ GET IP TX PKT ADDRS ----------- */
    addr_src  = pbuf_hdr->IP_AddrSrc;
    addr_dest = pbuf_hdr->IP_AddrDest;


                                                                        /* ------------ CHK CFG'D HOST ADDR ----------- */
                                                                        /* Chk cfg'd host addr     (see Note #3a2A).    */
    addr_cfgd = NetIP_IsAddrHostCfgdHandler(addr_dest);
    if (addr_cfgd == DEF_YES) {
        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestThisHostCtr);
        pbuf_hdr->IP_AddrNextRoute = addr_dest;
       *perr                       = NET_IP_ERR_TX_DEST_LOCAL_HOST;

                                                                        /* ----------- CHK LOCALHOST ADDRS ------------ */
                                                                        /* Chk localhost src  addr (see Note #3a2B1).   */
    } else if ((addr_src  & NET_IP_ADDR_LOCAL_HOST_MASK_NET) == NET_IP_ADDR_LOCAL_HOST_NET) {
        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestLocalHostCtr);
        pbuf_hdr->IP_AddrNextRoute = addr_dest;
       *perr                       = NET_IP_ERR_TX_DEST_LOCAL_HOST;
                                                                        /* Chk localhost dest addr (see Note #3a2B).    */
    } else if ((addr_dest & NET_IP_ADDR_LOCAL_HOST_MASK_NET) == NET_IP_ADDR_LOCAL_HOST_NET) {
        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestLocalHostCtr);
        pbuf_hdr->IP_AddrNextRoute = addr_dest;
       *perr                       = NET_IP_ERR_TX_DEST_LOCAL_HOST;

                                                                        /* ----------- CHK LINK-LOCAL ADDRS ----------- */
                                                                        /* Chk link-local src  addr (see Note #3c1A).   */
    } else if ((addr_src  & NET_IP_ADDR_LOCAL_LINK_MASK_NET) == NET_IP_ADDR_LOCAL_LINK_NET) {
        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestLocalLinkCtr);
        pbuf_hdr->IP_AddrNextRoute = addr_dest;
       *perr                       = NET_IP_ERR_TX_DEST_HOST_THIS_NET;
                                                                        /* Chk link-local dest addr (see Note #3c1B).   */
    } else if ((addr_dest & NET_IP_ADDR_LOCAL_LINK_MASK_NET) == NET_IP_ADDR_LOCAL_LINK_NET) {
                                                                        /* Chk link-local broadcast (see Note #3c3B).   */
        if ((addr_dest             & NET_IP_ADDR_LOCAL_LINK_MASK_HOST) ==
            (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_LOCAL_LINK_MASK_HOST)) {
             NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestBroadcastCtr);
             DEF_BIT_SET(pbuf_hdr->Flags, NET_BUF_FLAG_TX_BROADCAST);
             pbuf_hdr->IP_AddrNextRoute = addr_dest;
            *perr                       = NET_IP_ERR_TX_DEST_BROADCAST;

        } else {
             pbuf_hdr->IP_AddrNextRoute = addr_dest;
            *perr                       = NET_IP_ERR_TX_DEST_HOST_THIS_NET;
        }

        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestLocalLinkCtr);
        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestLocalNetCtr);


                                                                        /* ----------- CHK LIM'D BROADCAST ------------ */
    } else if (addr_dest == NET_IP_ADDR_BROADCAST) {                    /* Chk lim'd broadcast (see Note #3b2a).        */
        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestBroadcastCtr);
        NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestLocalNetCtr);
        DEF_BIT_SET(pbuf_hdr->Flags, NET_BUF_FLAG_TX_BROADCAST);
        pbuf_hdr->IP_AddrNextRoute = addr_dest;
       *perr                       = NET_IP_ERR_TX_DEST_BROADCAST;


/*$PAGE*/
                                                                        /* -------------- CHK LOCAL NET --------------- */
                                                                        /* Chk local subnet           (see Note #3b1b). */
    } else if ((addr_dest & NetIP_AddrThisHostSubnetMask) == NetIP_AddrThisHostSubnetNet) {
                                                                        /* If valid  subnet cfg'd, tx via subnet.       */
        if ((NetIP_AddrThisHostSubnetMask != NET_IP_ADDR_NONE) &&
            (NetIP_AddrThisHostSubnetNet  != NET_IP_ADDR_NONE)) {
                                                                        /* Chk local subnet broadcast (see Note #3b2b1).*/
            if ((addr_dest             & NetIP_AddrThisHostSubnetMaskHost) ==
                (NET_IP_ADDR_BROADCAST & NetIP_AddrThisHostSubnetMaskHost)) {
                 NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestBroadcastCtr);
                 DEF_BIT_SET(pbuf_hdr->Flags, NET_BUF_FLAG_TX_BROADCAST);
                 pbuf_hdr->IP_AddrNextRoute = addr_dest;
                *perr                       = NET_IP_ERR_TX_DEST_BROADCAST;

            } else {
                 pbuf_hdr->IP_AddrNextRoute = addr_dest;
                *perr                       = NET_IP_ERR_TX_DEST_HOST_THIS_NET;
            }

            NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestLocalNetCtr);

        } else {
           *perr = NET_IP_ERR_INVALID_ADDR_NET;
            return;
        }


                                                                        /* -------------- TX REMOTE NET --------------- */
    } else {
                                                                        /* Tx to remote net (see Note #3b1c).           */
        if ((addr_dest & NET_IP_ADDR_CLASS_A_MASK) == NET_IP_ADDR_CLASS_A) {
            if ((addr_dest             & NET_IP_ADDR_CLASS_A_MASK_HOST) ==
                (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_A_MASK_HOST)) {
                 NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestBroadcastCtr);
            }

        } else if ((addr_dest & NET_IP_ADDR_CLASS_B_MASK) == NET_IP_ADDR_CLASS_B) {
            if ((addr_dest             & NET_IP_ADDR_CLASS_B_MASK_HOST) ==
                (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_B_MASK_HOST)) {
                 NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestBroadcastCtr);
            }

        } else if ((addr_dest & NET_IP_ADDR_CLASS_C_MASK) == NET_IP_ADDR_CLASS_C) {
            if ((addr_dest             & NET_IP_ADDR_CLASS_C_MASK_HOST) ==
                (NET_IP_ADDR_BROADCAST & NET_IP_ADDR_CLASS_C_MASK_HOST)) {
                 NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestBroadcastCtr);
            }

        } else {                                                        /* Discard invalid addr class (see Note #2).    */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIP_ErrTxDestCtr);
           *perr = NET_IP_ERR_TX_DEST_INVALID;
            return;
        }


        if (NetIP_AddrDfltGateway != NET_IP_ADDR_NONE) {                /* If valid  dflt gateway cfg'd, ...            */
            NET_CTR_STAT_INC(Net_StatCtrs.NetIP_StatTxDestRemoteNetCtr);
            pbuf_hdr->IP_AddrNextRoute = NetIP_AddrDfltGateway;         /* ... tx via dflt gateway (see Note #3a1B).    */
           *perr                       = NET_IP_ERR_TX_DEST_DFLT_GATEWAY;

        } else {
           *perr                       = NET_IP_ERR_INVALID_ADDR_GATEWAY;
            return;
        }
    }


                                                                        /* ----- CFG IP NEXT ROUTE NET-ORDER ADDR ----- */
    pbuf_hdr->IP_AddrNextRouteNetOrder = NET_UTIL_HOST_TO_NET_32(pbuf_hdr->IP_AddrNextRoute);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIP_TxPktDiscard()
*
* Description : On any IP transmit packet errors, discard packet & buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_Tx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIP_TxPktDiscard (NET_BUF  *pbuf,
                                  NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetIP_ErrTxPktDiscardedCtr;
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
*                                           NetIP_ReTxPkt()
*
* Description : (1) Prepare & re-transmit IP packet :
*
*                   (a) Prepare     IP header
*                   (b) Re-transmit IP packet datagram
*
*
* Argument(s) : pbuf        Pointer to network buffer to re-transmit IP packet.
*               ----        Argument checked   in NetIP_ReTx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetIP_ReTx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                                   -- RETURNED BY NetIP_ReTxPktPrepareHdr() : ---
*                               NET_UTIL_ERR_NULL_PTR               Check-sum passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE              Check-sum passed a zero size.
*
*                                                                   ---- RETURNED BY NetIP_TxPktDatagram() : -----
*                               NET_IP_ERR_NONE                     IP datagram successfully received & processed.
*                               NET_IF_ERR_NONE                     Packet successfully transmitted.
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host    address.
*                               NET_IP_ERR_INVALID_ADDR_NET         Invalid IP net     address.
*                               NET_IP_ERR_INVALID_ADDR_GATEWAY     Invalid IP gateway address.
*                               NET_IP_ERR_TX_DEST_INVALID          Invalid transmit destination.
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_ERR_RX                          Receive  error; packet discarded.
*                               NET_ERR_TX                          Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_ReTx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIP_ReTxPkt (NET_BUF      *pbuf,
                             NET_BUF_HDR  *pbuf_hdr,
                             NET_ERR      *perr)
{
                                                                /* ------------------ PREPARE IP HDR ------------------ */
    NetIP_ReTxPktPrepareHdr(pbuf,
                            pbuf_hdr,
                            perr);
    if (*perr != NET_IP_ERR_NONE) {
         return;
    }

                                                                /* -------------- RE-TX IP PKT DATAGRAM --------------- */
    NetIP_TxPktDatagram(pbuf, pbuf_hdr, perr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIP_ReTxPktPrepareHdr()
*
* Description : (1) Prepare IP header for re-transmit IP packet :
*
*                   (a) Update network buffer's protocol & length controls
*
*                   (b) (1) Prepare the re-transmit packet's following IP header fields :
*
*                           (A) Identification  (ID)                        See Note #2
*                           (B) Check-Sum                                   See Note #3
*
*                       (2) Assumes the following IP header fields are already validated/prepared & 
*                           have NOT been modified :
*
*                           (A) Version
*                           (B) Header Length
*                           (C) Type of Service (TOS)
*                           (D) Total  Length
*                           (E) Flags
*                           (F) Fragment Offset
*                           (G) Time-to-Live    (TTL)
*                           (H) Protocol
*                           (I) Source      Address
*                           (J) Destination Address
*                           (K) Options
*
*                   (c) Convert the following IP header fields from host-order to network-order :
*
*                       (1) Identification (ID)
*                       (2) Check-Sum                                       See Note #3c
*
*
* Argument(s) : pbuf                Pointer to network buffer to transmit IP packet.
*               ----                Argument checked   in NetIP_Tx().
*
*               pbuf_hdr            Pointer to network buffer header.
*               --------            Argument validated in NetIP_Tx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IP_ERR_NONE                 IP header successfully prepared.
*
*                                                               - RETURNED BY NetUtil_16BitOnesCplChkSumHdrCalc() : -
*                               NET_UTIL_ERR_NULL_PTR           Check-sum passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Check-sum passed a zero size.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_ReTxPkt().
*
* Note(s)     : (2) RFC #1122, Section 3.2.1.5 states that "some Internet protocol experts have maintained
*                   that when a host sends an identical copy of an earlier datagram, the new copy should
*                   contain the same Identification value as the original ... However, ... we believe that
*                   retransmitting the same Identification field is not useful".
*
*               (3) (a) IP header Check-Sum MUST be calculated AFTER the entire IP header has been prepared.
*                       In addition, ALL multi-octet words are converted from host-order to network-order 
*                       since "the sum of 16-bit integers can be computed in either byte order" [RFC #1071, 
*                       Section 2.(B)].
*
*                   (b) IP header Check-Sum field MUST be cleared to '0' BEFORE the IP header Check-Sum is 
*                       calculated (see RFC #791, Section 3.1 'Header Checksum').
*
*                   (c) The IP header Check-Sum field is returned in network-order & MUST NOT be re-converted 
*                       back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()  Note #3b').
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetIP_ReTxPktPrepareHdr (NET_BUF      *pbuf,
                                       NET_BUF_HDR  *pbuf_hdr,
                                       NET_ERR      *perr)
{
    NET_IP_HDR  *pip_hdr;
    CPU_INT16U   ip_id;
    CPU_INT16U   ip_hdr_len_tot;
    CPU_INT16U   ip_chk_sum;


                                                                /* ----------------- UPDATE BUF CTRLS ----------------- */
    pbuf_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_IP_V4;       /* Update buf protocol for IP.                          */
                                                                /* Reset tot len for re-tx.                             */
    pbuf_hdr->TotLen          = (NET_BUF_SIZE)pbuf_hdr->IP_TotLen;


                                                                /* ------------------ PREPARE IP HDR ------------------ */
    pip_hdr = (NET_IP_HDR *)&pbuf->Data[pbuf_hdr->IP_HdrIx];


                                                                /* ------------------ PREPARE IP ID ------------------- */
    NET_IP_TX_GET_ID(ip_id);                                    /* Get new IP ID (see Note #2).                         */
    NET_UTIL_VAL_COPY_SET_NET_16(&pip_hdr->ID, &ip_id);

                                                                /* ---------------- PREPARE IP CHK SUM ---------------- */
                                                                /* See Note #3.                                         */
    NET_UTIL_VAL_SET_NET_16(&pip_hdr->ChkSum, 0x0000);          /* Clr  chk sum (see Note #3b).                         */
                                                                /* Calc chk sum.                                        */
    ip_hdr_len_tot = pbuf_hdr->IP_HdrLen;
    ip_chk_sum     = NetUtil_16BitOnesCplChkSumHdrCalc((void     *)pip_hdr,
                                                       (CPU_INT16U)ip_hdr_len_tot,
                                                       (NET_ERR  *)perr);
    if (*perr != NET_UTIL_ERR_NONE) {
         return;
    }

    NET_UTIL_VAL_COPY_16(&pip_hdr->ChkSum, &ip_chk_sum);        /* Copy chk sum in net order (see Note #3c).            */



   *perr = NET_IP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LAYER INTERFACE FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                   NetARP_GetHostAddrPtrProtocol()
*
* Description : Get pointer to this host's internet protocol address.
*
* Argument(s) : none.
*
* Return(s)   : Pointer to internet protocol address.
*
* Caller(s)   : NetARP_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) ARP protocol address MUST be in network-order (see 'net_arp.c  NetARP_CacheHandler()
*                   Note #2e3').
*********************************************************************************************************
*/

#ifdef  NET_ARP_MODULE_PRESENT
CPU_INT08U  *NetARP_GetHostAddrPtrProtocol (void)
{
    CPU_INT08U  *paddr;


    paddr = (CPU_INT08U *)&NetIP_AddrThisHostNetOrder;

    return (paddr);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetARP_IsValidAddrProtocol()
*
* Description : Validate an ARP protocol address.
*
* Argument(s) : paddr_protocol      Pointer to an ARP protocol address.
*               --------------      Argument validated in NetARP_RxPktValidate().
*
* Return(s)   : DEF_YES, if protocol address valid.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetARP_RxPktValidate().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_ARP_MODULE_PRESENT
CPU_BOOLEAN  NetARP_IsValidAddrProtocol (CPU_INT08U  *paddr_protocol)
{
    NET_IP_ADDR  addr;
    CPU_BOOLEAN  valid;


    Mem_Copy((void     *)&addr,
             (void     *)paddr_protocol,
             (CPU_SIZE_T)NET_ARP_CFG_PROTOCOL_ADDR_LEN);

    addr  = NET_UTIL_NET_TO_HOST_32(addr);
    valid = NetIP_IsValidAddrHost(addr);

    return (valid);
}
#endif

