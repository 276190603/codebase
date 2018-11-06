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
*                                         NETWORK SOCKET LAYER
*
* Filename      : net_sock.c
* Version       : V1.92
* Programmer(s) : ITJ
*                 SR
*********************************************************************************************************
* Note(s)       : (1) Supports BSD 4.x Socket Layer with the following restrictions/constraints :
*
*                     (a) ONLY supports a single address family from the following families :
*                         (1) IPv4 (AF_INET)
*
*                     (b) ONLY supports the following socket types :
*                         (1) Datagram (SOCK_DGRAM)
*                         (2) Stream   (SOCK_STREAM)
*
*                     (c) ONLY supports a single protocol family from the following families :
*                         (1) IPv4 (PF_INET)
*                             (A) ONLY supports the following protocols :
*                                 (1) UDP (IPPROTO_UDP)
*                                 (2) TCP (IPPROTO_TCP)
*
*                     (d) ONLY supports the following socket options :
*                         (1) Global options
*                             #### ( )
*                         (2) Socket options
*                             #### ( )
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

#define    NET_SOCK_MODULE
#include  <net.h>


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) See 'net_sock.h  MODULE'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT


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

                                                                                    /* ----------- RX FNCTS ----------- */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void                NetSock_RxPktValidateBuf             (NET_BUF_HDR          *pbuf_hdr,
                                                                  NET_ERR              *perr);
#endif

static  void                NetSock_RxPktDemux                   (NET_BUF              *pbuf,
                                                                  NET_BUF_HDR          *pbuf_hdr,
                                                                  NET_ERR              *perr);


static  void                NetSock_RxPktDiscard                 (NET_BUF              *pbuf,
                                                                  NET_ERR              *perr);



                                                                                    /* ------ SOCK STATUS FNCTS ------- */

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
static  CPU_BOOLEAN         NetSock_IsValidAddrLocal             (NET_SOCK_ADDR        *paddr,
                                                                  NET_SOCK_ADDR_LEN     addr_len,
                                                                  NET_ERR              *perr);

static  CPU_BOOLEAN         NetSock_IsValidAddrRemote            (NET_SOCK_ADDR        *paddr,
                                                                  NET_SOCK_ADDR_LEN     addr_len,
                                                                  NET_SOCK             *psock,
                                                                  NET_ERR              *perr);
#endif



                                                                                    /* ------ SOCK HANDLER FNCTS ------ */

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_SOCK_RTN_CODE   NetSock_CloseHandlerStream           (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  NET_ERR              *perr);
#endif


static  NET_SOCK_RTN_CODE   NetSock_BindHandler                  (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK_ADDR        *paddr_local,
                                                                  NET_SOCK_ADDR_LEN     addr_len,
                                                                  CPU_BOOLEAN           addr_random_reqd,
                                                                  NET_ERR              *perr);



/*$PAGE*/
static  NET_SOCK_RTN_CODE   NetSock_ConnHandlerDatagram          (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  NET_SOCK_ADDR        *paddr_remote,
                                                                  NET_ERR              *perr);

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_SOCK_RTN_CODE   NetSock_ConnHandlerStream            (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  NET_SOCK_ADDR        *paddr_remote,
                                                                  NET_ERR              *perr);
#endif


static  void                NetSock_ConnHandlerAddrRemoteValidate(NET_SOCK             *psock,
                                                                  NET_SOCK_ADDR        *paddr_remote,
                                                                  NET_ERR              *perr);

static  void                NetSock_ConnHandlerAddrRemoteSet     (NET_SOCK             *psock,
                                                                  NET_SOCK_ADDR        *paddr_remote,
                                                                  CPU_BOOLEAN           addr_over_wr,
                                                                  NET_ERR              *perr);



#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  void                NetSock_ConnAcceptQ_Init             (NET_SOCK             *psock,
                                                                  NET_SOCK_Q_SIZE       sock_q_size);

static  void                NetSock_ConnAcceptQ_Clr              (NET_SOCK             *psock);

static  CPU_BOOLEAN         NetSock_ConnAcceptQ_IsAvail          (NET_SOCK             *psock,
                                                                  NET_ERR              *perr);

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static  CPU_BOOLEAN         NetSock_ConnAcceptQ_IsRdy            (NET_SOCK             *psock,
                                                                  NET_ERR              *perr);
#endif

static  void                NetSock_ConnAcceptQ_ConnID_Add       (NET_SOCK             *psock,
                                                                  NET_CONN_ID           conn_id,
                                                                  NET_ERR              *perr);

static  NET_CONN_ID         NetSock_ConnAcceptQ_ConnID_Get       (NET_SOCK             *psock,
                                                                  NET_ERR              *perr);

static  CPU_BOOLEAN         NetSock_ConnAcceptQ_ConnID_Srch      (NET_SOCK             *psock,
                                                                  NET_CONN_ID           conn_id,
                                                                  NET_SOCK_Q_SIZE      *pconn_ix,
                                                                  NET_SOCK_Q_SIZE      *pconn_nbr);

static  CPU_BOOLEAN         NetSock_ConnAcceptQ_ConnID_Remove    (NET_SOCK             *psock,
                                                                  NET_CONN_ID           conn_id);
#endif



/*$PAGE*/
static  NET_SOCK_RTN_CODE   NetSock_RxDataHandler                (NET_SOCK_ID           sock_id,
                                                                  void                 *pdata_buf,
                                                                  CPU_INT16U            data_buf_len,
                                                                  CPU_INT16S            flags,
                                                                  NET_SOCK_ADDR        *paddr_remote,
                                                                  NET_SOCK_ADDR_LEN    *paddr_len,
                                                                  void                 *pip_opts_buf,
                                                                  CPU_INT08U            ip_opts_buf_len,
                                                                  CPU_INT08U           *pip_opts_len,
                                                                  NET_ERR              *perr);


static  NET_SOCK_RTN_CODE   NetSock_RxDataHandlerDatagram        (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  void                 *pdata_buf,
                                                                  CPU_INT16U            data_buf_len,
                                                                  CPU_INT16S            flags,
                                                                  NET_SOCK_ADDR        *paddr_remote,
                                                                  NET_SOCK_ADDR_LEN    *paddr_len,
                                                                  void                 *pip_opts_buf,
                                                                  CPU_INT08U            ip_opts_buf_len,
                                                                  CPU_INT08U           *pip_opts_len,
                                                                  NET_ERR              *perr);

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_SOCK_RTN_CODE   NetSock_RxDataHandlerStream          (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  void                 *pdata_buf,
                                                                  CPU_INT16U            data_buf_len,
                                                                  CPU_INT16S            flags,
                                                                  NET_SOCK_ADDR        *paddr_remote,
                                                                  NET_SOCK_ADDR_LEN    *paddr_len,
                                                                  NET_ERR              *perr);
#endif



static  NET_SOCK_RTN_CODE   NetSock_TxDataHandler                (NET_SOCK_ID           sock_id,
                                                                  void                 *p_data,
                                                                  CPU_INT16U            data_len,
                                                                  CPU_INT16S            flags,
                                                                  NET_SOCK_ADDR        *paddr_remote,
                                                                  NET_SOCK_ADDR_LEN     addr_len,
                                                                  NET_ERR              *perr);


static  NET_SOCK_RTN_CODE   NetSock_TxDataHandlerDatagram        (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  void                 *p_data,
                                                                  CPU_INT16U            data_len,
                                                                  CPU_INT16S            flags,
                                                                  NET_SOCK_ADDR        *paddr_remote,
                                                                  NET_ERR              *perr);

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_SOCK_RTN_CODE   NetSock_TxDataHandlerStream          (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  void                 *p_data,
                                                                  CPU_INT16U            data_len,
                                                                  CPU_INT16S            flags,
                                                                  NET_ERR              *perr);
#endif



/*$PAGE*/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static  CPU_BOOLEAN         NetSock_SelDescHandlerRd             (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                                  NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                                  NET_ERR              *perr);

static  CPU_BOOLEAN         NetSock_SelDescHandlerRdDatagram     (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                                  NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                                  NET_ERR              *perr);

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN         NetSock_SelDescHandlerRdStream       (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                                  NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                                  NET_ERR              *perr);
#endif


static  CPU_BOOLEAN         NetSock_SelDescHandlerWr             (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                                  NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                                  NET_ERR              *perr);

static  CPU_BOOLEAN         NetSock_SelDescHandlerWrDatagram     (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                                  NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                                  NET_ERR              *perr);

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN         NetSock_SelDescHandlerWrStream       (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                                  NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                                  NET_ERR              *perr);
#endif


static  CPU_BOOLEAN         NetSock_SelDescHandlerErr            (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                                  NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                                  NET_ERR              *perr);

static  CPU_BOOLEAN         NetSock_SelDescHandlerErrDatagram    (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                                  NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                                  NET_ERR              *perr);

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN         NetSock_SelDescHandlerErrStream      (NET_SOCK_ID           sock_id,
                                                                  NET_SOCK             *psock,
                                                                  NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                                  NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                                  NET_ERR              *perr);
#endif



static  CPU_BOOLEAN         NetSock_IsAvailRxDatagram            (NET_SOCK             *psock,
                                                                  NET_ERR              *perr);

static  CPU_BOOLEAN         NetSock_IsRdyTxDatagram              (NET_SOCK             *psock,
                                                                  NET_ERR              *perr);


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN         NetSock_IsAvailRxStream              (NET_SOCK             *psock,
                                                                  NET_CONN_ID           conn_id_transport,
                                                                  NET_ERR              *perr);

static  CPU_BOOLEAN         NetSock_IsRdyTxStream                (NET_SOCK             *psock,
                                                                  NET_CONN_ID           conn_id_transport,
                                                                  NET_ERR              *perr);
#endif
#endif



/*$PAGE*/
                                                                                    /* ---------- SOCK FNCTS ---------- */

static  NET_SOCK           *NetSock_Get                          (NET_ERR              *perr);

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_CONN_ID         NetSock_GetConnTransport             (NET_SOCK             *psock,
                                                                  NET_ERR              *perr);
#endif



static  void                NetSock_CloseHandler                 (NET_SOCK             *psock,
                                                                  CPU_BOOLEAN           close_conn,
                                                                  CPU_BOOLEAN           close_conn_transport);

static  void                NetSock_CloseSock                    (NET_SOCK             *psock,
                                                                  CPU_BOOLEAN           close_conn,
                                                                  CPU_BOOLEAN           close_conn_transport);

static  void                NetSock_CloseSockHandler             (NET_SOCK             *psock,
                                                                  CPU_BOOLEAN           close_conn,
                                                                  CPU_BOOLEAN           close_conn_transport,
                                                                  NET_ERR              *perr);

static  void                NetSock_CloseSockFromClose           (NET_SOCK             *psock);


#if ((NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN  == DEF_ENABLED))
static  void                NetSock_CloseConn                    (NET_CONN_ID           conn_id);

static  void                NetSock_CloseConnFree                (NET_CONN_ID           conn_id);
#endif



static  void                NetSock_Free                         (NET_SOCK             *psock);

static  void                NetSock_FreeHandler                  (NET_SOCK             *psock,
                                                                  NET_ERR              *perr);

static  void                NetSock_FreeAddr                     (NET_SOCK             *psock);

static  void                NetSock_FreeBufQ                     (NET_BUF             **pbuf_q_head,
                                                                  NET_BUF             **pbuf_q_tail);



static  void                NetSock_Clr                          (NET_SOCK             *psock);

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  void                NetSock_Copy                         (NET_SOCK             *psock_dest,
                                                                  NET_SOCK             *psock_src);
#endif

static  void                NetSock_Discard                      (NET_SOCK             *psock);



                                                                                    /* ----- RANDOM PORT Q FNCTS ------ */

static  NET_PORT_NBR        NetSock_RandomPortNbrGet             (NET_ERR              *perr);

static  void                NetSock_RandomPortNbrFree            (NET_PORT_NBR          port_nbr,
                                                                  NET_ERR              *perr);

static  CPU_BOOLEAN         NetSock_RandomPortNbrChkRange        (NET_PORT_NBR          port_nbr);

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  CPU_BOOLEAN         NetSock_RandomPortNbrSrch            (NET_PORT_NBR          port_nbr);
#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetSock_Init()
*
* Description : (1) Initialize Network Socket Layer :
*
*                   (a) Perform    Socket/OS initialization
*                   (b) Initialize socket pool
*                   (c) Initialize socket table
*                   (d) Initialize random port number queue
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                       Socket layer successfully initialized.
*
*                                                                       ----- RETURNED BY NetOS_Sock_Init() : -----
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
* Caller(s)   : Net_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) The following network socket initialization MUST be sequenced as follows :
*
*                   (a) NetOS_Sock_Init()   MUST precede ALL other network socket initialization functions
*                   (b) Network socket pool MUST be initialized PRIOR to initializing the pool with pointers 
*                            to sockets
*********************************************************************************************************
*/
/*$PAGE*/
void  NetSock_Init (NET_ERR  *perr)
{
    NET_SOCK          *psock;
    NET_PORT_NBR      *pport_nbr_q;
    NET_PORT_NBR       port_nbr_q_nbr;
    NET_SOCK_QTY       i;
    NET_PORT_NBR_QTY   j;
    NET_ERR            stat_err;


                                                                /* --------------- PERFORM SOCK/OS INIT --------------- */
    NetOS_Sock_Init(perr);                                      /* Create sock obj(s) [see Note #2a].                   */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

                                                                /* --------------- INIT SOCK POOL/STATS --------------- */
    NetSock_PoolPtr = (NET_SOCK *)0;                            /* Init-clr sock pool (see Note #2b).                   */

    NetStat_PoolInit((NET_STAT_POOL   *)&NetSock_PoolStat,
                     (NET_STAT_POOL_QTY) NET_SOCK_CFG_NBR_SOCK,
                     (NET_ERR         *)&stat_err);


                                                                /* ------------------ INIT SOCK TBL ------------------- */
    psock = &NetSock_Tbl[0];
    for (i = 0; i < NET_SOCK_CFG_NBR_SOCK; i++) {
        psock->Type   =  NET_SOCK_TYPE_SOCK;                    /* Init each sock type/id--NEVER modify.                */
        psock->ID     = (NET_SOCK_ID)i;

        psock->State  =  NET_SOCK_STATE_FREE;                   /* Init each sock as free/NOT used.                     */
        psock->Flags  =  NET_SOCK_FLAG_NONE;

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
        NetSock_Clr(psock);
#endif

        psock->NextPtr  = NetSock_PoolPtr;                      /* Free each sock to sock pool (see Note #2).           */
        NetSock_PoolPtr = psock;

        psock++;
    }


                                                                /* -------------- INIT RANDOM PORT NBR Q -------------- */
    pport_nbr_q    = &NetSock_RandomPortNbrQ[0];
    port_nbr_q_nbr =  NET_SOCK_CFG_PORT_NBR_RANDOM_BASE;
    for (j = 0; j < NET_SOCK_PORT_NBR_RANDOM_NBR; j++) {
       *pport_nbr_q = port_nbr_q_nbr;

        port_nbr_q_nbr++;
        pport_nbr_q++;
    }

    NetSock_RandomPortNbrQ_HeadIx  = 0;
    NetSock_RandomPortNbrQ_TailIx  = 0;
    NetSock_RandomPortNbrQ_NbrUsed = 0;


   *perr = NET_SOCK_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetSock_Rx()
*
* Description : (1) Process received socket data & forward to application :
*
*                   (a) Demultiplex data to connection
*                   (b) Update receive statistics
*
*
* Argument(s) : pbuf        Pointer to network buffer that received socket data.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket data successfully received & processed.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               ----- RETURNED BY NetSock_RxPktDemux() : -----
*                               NET_ERR_RX_DEST                 Invalid destination; no socket connection
*                                                                   available for received packet.
*
*                                                               ---- RETURNED BY NetSock_RxPktDiscard() : ----
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_RxPktDemuxDatagram().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) NetSock_Rx() blocked until network initialization completes.
*
*               (3) Since RFC #792, Section 'Destination Unreachable Message : Description' states
*                   that "if, in the destination host, the IP module cannot deliver the datagram 
*                   because the indicated ... process port is not active, the destination host may 
*                   send a destination unreachable message to the source host"; the network buffer
*                   MUST NOT be freed by the socket layer but must be returned to the transport or
*                   internet layer(s) to send an appropriate ICMP error message.
*
*                   See also 'net_udp.c  NetUDP_Rx()  Note #5'.
*
*               (4) Network buffer freed by lower layer (see Note #3); only increment error counter.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetSock_Rx (NET_BUF  *pbuf,
                  NET_ERR  *perr)
{
    NET_BUF_HDR  *pbuf_hdr;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit rx (see Note #2).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NetSock_RxPktDiscard(pbuf, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNullPtrCtr);
        return;
    }
#endif


    NET_CTR_STAT_INC(Net_StatCtrs.NetSock_StatRxPktCtr);


                                                                /* ---------------- VALIDATE SOCK PKT ----------------- */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NetSock_RxPktValidateBuf(pbuf_hdr, perr);                   /* Validate rx'd buf.                                   */
    switch (*perr) {
        case NET_SOCK_ERR_NONE:
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        default:
             NetSock_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif


                                                                /* --------- DEMUX SOCK PKT / UPDATE RX STATS --------- */
    NetSock_RxPktDemux(pbuf, pbuf_hdr, perr);
    switch (*perr) {
        case NET_SOCK_ERR_NONE:
             NET_CTR_STAT_INC(Net_StatCtrs.NetSock_StatRxPktProcessedCtr);
             break;


        case NET_ERR_RX_DEST:                                           /* See Note #3.                                 */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrRxPktDiscardedCtr); /* See Note #4.                                 */
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_ERR_INIT_INCOMPLETE:
        case NET_SOCK_ERR_NOT_USED:
        case NET_SOCK_ERR_CLOSED:
        case NET_SOCK_ERR_INVALID_SOCK:
        case NET_SOCK_ERR_INVALID_FAMILY:
        case NET_SOCK_ERR_INVALID_PROTOCOL:
        case NET_SOCK_ERR_INVALID_OP:
        case NET_SOCK_ERR_RX_Q_FULL:
        case NET_SOCK_ERR_RX_Q_SIGNAL:
        case NET_CONN_ERR_INVALID_CONN:
        case NET_CONN_ERR_NOT_USED:
        case NET_CONN_ERR_CONN_NONE:
        default:
             NetSock_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetSock_Open()
*
* Description : (1) Open a socket :
*
*                   (a) Acquire  network lock
*                   (b) Validate socket arguments :
*                       (1) Socket protocol family
*                       (2) Socket protocol
*                       (3) Socket type
*
*                   (c) Get socket from socket pool
*                   (d) Initialize socket
*                   (e) Release network lock
*                   (f) Return  socket descriptor/handle identifier
*                         OR
*                       NET_SOCK_BSD_ERR_OPEN & error code, on failure
*
*
* Argument(s) : protocol_family     Socket protocol family :
*
*                                       NET_SOCK_FAMILY_IP_V4       Internet Protocol version 4 (IPv4).
*
*                                   See also 'net_sock.c  Note #1a'.
*
*               sock_type           Socket type :
*
*                                       NET_SOCK_TYPE_DATAGRAM      Datagram-type socket.
*                                       NET_SOCK_TYPE_STREAM        Stream  -type socket.
*
*                                   See also 'net_sock.c  Note #1b'.
*
*               protocol            Socket protocol :
*
*                                       NET_SOCK_PROTOCOL_DFLT      Default protocol for socket type.
*                                       NET_SOCK_PROTOCOL_UDP       User Datagram        Protocol (UDP).
*                                       NET_SOCK_PROTOCOL_TCP       Transmission Control Protocol (TCP).
*
*                                   See also 'net_sock.c  Note #1c'.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket successfully opened.
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*
*                                                                   --- RETURNED BY NetSock_Get() : ----
*                               NET_SOCK_ERR_NONE_AVAIL             NO available sockets to allocate.
*
*                                                                   ---- RETURNED BY NetOS_Lock() : ----
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*
* Return(s)   : Socket descriptor/handle identifier, if NO errors.
*
*               NET_SOCK_BSD_ERR_OPEN,               otherwise.
*
* Caller(s)   : socket().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_Open() blocked until network initialization completes.
*********************************************************************************************************
*/
/*$PAGE*/
NET_SOCK_ID  NetSock_Open (NET_SOCK_PROTOCOL_FAMILY   protocol_family,
                           NET_SOCK_TYPE              sock_type,
                           NET_SOCK_PROTOCOL          protocol,
                           NET_ERR                   *perr)
{
    NET_SOCK     *psock;
    NET_SOCK_ID   sock_id;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_OPEN);
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit (see Note #2).            */
        NetOS_Unlock();
       *perr =  NET_ERR_INIT_INCOMPLETE;
        return (NET_SOCK_BSD_ERR_OPEN);
    }
#endif


                                                                /* ---------------- VALIDATE SOCK ARGS ---------------- */
    switch (protocol_family) {
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
        case NET_SOCK_FAMILY_IP_V4:
             switch (sock_type) {
                 case NET_SOCK_TYPE_DATAGRAM:
                      switch (protocol) {
                          case NET_SOCK_PROTOCOL_UDP:
                               break;


                          case NET_SOCK_PROTOCOL_DFLT:
                               protocol = NET_SOCK_PROTOCOL_UDP;
                               break;


                          default:
                               NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
                               NetOS_Unlock();
                              *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;
                               return (NET_SOCK_BSD_ERR_OPEN);  /* Prevent 'break NOT reachable' compiler warning.      */
                      }
                      break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                 case NET_SOCK_TYPE_STREAM:
                      switch (protocol) {
#ifdef  NET_TCP_MODULE_PRESENT
                          case NET_SOCK_PROTOCOL_TCP:
                               break;


                          case NET_SOCK_PROTOCOL_DFLT:
                               protocol = NET_SOCK_PROTOCOL_TCP;
                               break;
#endif

                          default:
                               NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
                               NetOS_Unlock();
                              *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;
                               return (NET_SOCK_BSD_ERR_OPEN);  /* Prevent 'break NOT reachable' compiler warning.      */
                      }
                      break;
#endif

                 case NET_SOCK_TYPE_NONE:
                 case NET_SOCK_TYPE_FAULT:
                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
                      NetOS_Unlock();
                     *perr =  NET_SOCK_ERR_INVALID_TYPE;
                      return (NET_SOCK_BSD_ERR_OPEN);           /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;
#endif

        case NET_SOCK_FAMILY_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_FAMILY;
             return (NET_SOCK_BSD_ERR_OPEN);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }


/*$PAGE*/
                                                                /* --------------------- GET SOCK --------------------- */
    psock = NetSock_Get(perr);
    if (psock == (NET_SOCK *)0) {
        NetOS_Unlock();
        return (NET_SOCK_BSD_ERR_OPEN);                         /* Rtn err from NetSock_Get().                          */
    }


                                                                /* -------------------- INIT SOCK --------------------- */
    psock->ProtocolFamily = protocol_family;
    psock->Protocol       = protocol;
    psock->SockType       = sock_type;
    sock_id               = psock->ID;


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


   *perr =  NET_SOCK_ERR_NONE;

    return (sock_id);                                           /* ------------------- RTN SOCK ID -------------------- */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetSock_Close()
*
* Description : (1) Close a socket :
*
*                   (a) Acquire  network lock
*                   (b) Validate socket
*                       (1) Validate socket used
*                       (2) Validate socket connection state
*                   (c) Close    socket
*                   (d) Release  network lock
*
*               (2) Once an application closes its socket, NO further operations on the socket are allowed
*                   & the application MUST NOT continue to access the socket.
*                   
*                   #### Continued access to the closed socket by the application layer will likely corrupt
*                   the network socket layer.
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to close.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                       Socket successfully closed.
*                               NET_SOCK_ERR_NOT_USED                   Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                     Socket already closed.
*                               NET_SOCK_ERR_INVALID_STATE              Invalid socket state.
*
*                                                                       ------- RETURNED BY NetSock_IsUsed() : -------
*                               NET_ERR_INIT_INCOMPLETE                 Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK               Invalid socket number.
*
*                                                                       - RETURNED BY NetSock_CloseHandlerStream() : -
*                               NET_SOCK_ERR_FAULT                      Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_INVALID_FAMILY             Invalid socket protocol family.
*                               NET_SOCK_ERR_CONN_CLOSE_IN_PROGRESS     Socket close already in progress.
*                               NET_SOCK_ERR_CONN_FAIL                  Socket connection operation(s) failed.
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT        Socket connection close NOT signaled by
*                                                                           timeout.
*
*                               NET_CONN_ERR_INVALID_CONN               Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED                   Network connection NOT currently used.
*                               NET_CONN_ERR_NULL_PTR                   Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN           Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED              Network connection address NOT in use.
*
*                                                                       --------- RETURNED BY NetOS_Lock() : ---------
*                               NET_OS_ERR_LOCK                         Network access NOT acquired.
*
* Return(s)   : NET_SOCK_BSD_ERR_NONE,  if NO errors (see Note #4).
*
*               NET_SOCK_BSD_ERR_CLOSE, otherwise.
*
* Caller(s)   : close().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (3) NetSock_Close() blocked until network initialization completes.
*
*                   See 'NetSock_IsUsed()  Note #1'.
*
*               (4) NO BSD socket error is returned for any internal error while closing the socket.
*
*               (5) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'SockType' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
NET_SOCK_RTN_CODE  NetSock_Close (NET_SOCK_ID   sock_id,
                                  NET_ERR      *perr)
{
    NET_SOCK           *psock;
    NET_SOCK_RTN_CODE   rtn_code;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_CLOSE);
    }



#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_CLOSE);
    }
#endif


    psock = &NetSock_Tbl[sock_id];
                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (NET_SOCK_BSD_ERR_CLOSE);                   /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED:                             /* If CLOSED from init open  ...                        */
        case NET_SOCK_STATE_CLOSED_FAULT:                       /* ... OR internal fault(s), ...                        */
             NetSock_Free(psock);                               /* ... sock need ONLY be freed.                         */
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_CLOSED;                       /* Rtn net sock err but rtn NO BSD err (see Note #4).   */
             return (NET_SOCK_BSD_ERR_NONE);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
             break;

             
        case NET_SOCK_STATE_NONE:
        default:
             NetSock_CloseSockFromClose(psock);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_STATE;                /* Rtn net sock err but rtn NO BSD err (see Note #4).   */
             return (NET_SOCK_BSD_ERR_NONE);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }


/*$PAGE*/
                                                                /* -------------------- CLOSE SOCK -------------------- */
    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             NetSock_CloseHandler(psock, DEF_YES, DEF_YES);
             rtn_code = NET_SOCK_BSD_ERR_NONE;
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             rtn_code = NetSock_CloseHandlerStream(sock_id, psock, perr);
             break;
#endif

        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                /* See Note #5.                                         */
             NetSock_CloseSockFromClose(psock);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_TYPE;                 /* Rtn net sock err but rtn NO BSD err (see Note #4).   */
             return (NET_SOCK_BSD_ERR_NONE);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


   *perr =  NET_SOCK_ERR_NONE;

    return (rtn_code);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetSock_CloseFromConn()
*
* Description : Close a socket via a network connection.
*
*               (1) When a network connection closes a socket, the socket :
*
*                   (a) (1) Closes NO other network connection(s),
*                       (2) MUST NOT recursively re-close other network connection(s);
*
*                   (b) SHOULD clear network connection(s)' handle identifiers.
*
*                   See also             'NetSock_CloseSockHandler()  Note #2a',
*                            'net_tcp.c   NetTCP_ConnCloseFromConn()  Note #1',
*                          & 'net_conn.c  NetConn_CloseFromApp()      Note #1b'.
*
*               (2) Closes socket but does NOT free the socket since NO mechanism or API exists to close
*                   an application's reference to the socket.
*
*                   See also 'NetSock_CloseSock()  Note #2b'.
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to close.
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_CloseApp().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetSock_CloseFromConn (NET_SOCK_ID  sock_id)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NET_ERR    err;
#endif
    NET_SOCK  *psock;

                                                                /* ------------------ VALIDATE SOCK ------------------- */
    if (sock_id == NET_SOCK_ID_NONE) {
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, &err);
    if (err != NET_SOCK_ERR_NONE) {
        return;
    }
#endif


    psock = &NetSock_Tbl[sock_id];

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        default:
             break;
    }
#endif


                                                                /* -------------------- CLOSE SOCK -------------------- */
    NetSock_CloseSock((NET_SOCK  *)psock,
                      (CPU_BOOLEAN)DEF_YES,                     /* See Note #1b.                                        */
                      (CPU_BOOLEAN)DEF_NO);                     /* See Note #1a.                                        */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_FreeConnFromSock()
*
* Description : (1) Free/de-reference network connection from socket :
*
*                   (a) Remove connection handle identifier from socket's connection accept queue
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to free network connection.
*
*               conn_id     Handle identifier of network connection.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CloseConnFree(),
*               NetConn_CloseApp().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) (a) When a network connection is fully connected/established, it is queued to an
*                       application connection as a cloned network connection until the connection is
*                       accepted & a new application connection is created.
*
*                       See also 'net_tcp.c  NetTCP_RxPktConnHandlerListen()  Note #5a2A1'.
*
*                   (b) Therefore, network connections need only be de-referenced from cloned socket
*                       application connections.
*********************************************************************************************************
*/

void  NetSock_FreeConnFromSock (NET_SOCK_ID  sock_id,
                                NET_CONN_ID  conn_id)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NET_ERR    err;
#endif
    NET_SOCK  *psock;

                                                                /* ------------------ VALIDATE SOCK ------------------- */
    if (sock_id == NET_SOCK_ID_NONE) {
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, &err);
    if (err != NET_SOCK_ERR_NONE) {
        return;
    }
#endif


    psock = &NetSock_Tbl[sock_id];

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
        default:
             break;
    }
#endif


                                                                /* --------------- FREE/DE-REF CONN ID ---------------- */
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
    NetSock_ConnAcceptQ_ConnID_Remove(psock, conn_id);
#endif


   (void)&psock;                                                /* Prevent possible compiler warnings.                  */
   (void)&conn_id;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetSock_Bind()
*
* Description : (1) Bind a socket to a local address :
*
*                   (a) Acquire  network lock
*                   (b) Validate socket  used
*                   (c) Bind     socket  to a local address
*                   (d) Release  network lock
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to bind to a local address.
*
*               paddr_local     Pointer to socket address structure             (see Notes #3b1B, #3b2, & #4).
*
*               addr_len        Length  of socket address structure (in octets) [see Note  #3b1C].
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                                   ------- RETURNED BY NetSock_IsUsed() : --------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*
*                                                                   ----- RETURNED BY NetSock_BindHandler() : -----
*                               NET_SOCK_ERR_NONE                   Socket successfully bound to local address.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid local address.
*                               NET_SOCK_ERR_ADDR_IN_USE            Local address already in use.
*                               NET_SOCK_ERR_PORT_NBR_NONE_AVAIL    Port number NOT available.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host address.
*
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_NONE_AVAIL             NO available connections to allocate.
*                               NET_CONN_ERR_NOT_USED               Network connection(s) NOT currently used.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_TYPE           Invalid network connection type.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
*                                                                   --------- RETURNED BY NetOS_Lock() : ----------
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*
* Return(s)   : NET_SOCK_BSD_ERR_NONE, if NO errors (see Note #3c1).
*
*               NET_SOCK_BSD_ERR_BIND, otherwise    (see Note #3c2A).
*
* Caller(s)   : bind().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*$PAGE*
* Note(s)     : (2) NetSock_Bind() blocked until network initialization completes.
*
*                   See 'NetSock_IsUsed()  Note #1'.
*
*               (3) (a) (1) IEEE Std 1003.1, 2004 Edition, Section 'bind() : DESCRIPTION' states that "the bind() 
*                           function shall assign a local socket address ... to a socket".
*
*                       (2) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                           Section 4.4, Page 102 states that "bind() lets us specify the ... address, the port, 
*                           both, or neither".
*
*                   (b) (1) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the bind() 
*                           function takes the following arguments" :
*
*                           (A) 'socket' - "Specifies the file descriptor of the socket to be bound."
*
*                           (B) 'address' - "Points to a 'sockaddr' structure containing the address to be bound 
*                                to the socket.  The length and format of the address depend on the address family 
*                                of the socket."
*
*                           (C) 'address_len' - "Specifies the length of the 'sockaddr' structure pointed to by 
*                                the address argument."
*
*                       (2) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                           Section 4.4, Page 102 states that "if ... bind() is called" with :
*
*                           (A) "A port number of 0, the kernel chooses an ephemeral port."
*
*                               (1) "bind() does not return the chosen value ... [of] an ephemeral port ... Call 
*                                    getsockname() to return the protocol address ... to obtain the value of the 
*                                    ephemeral port assigned by the kernel."
*
*                           (B) "A wildcard ... address, the kernel does not choose the local ... address until 
*                                either the socket is connected (TCP) or a datagram is sent on the socket (UDP)."
*
*                               (1) "With IPv4, the wildcard address is specified by the constant INADDR_ANY, 
*                                    whose value is normally 0."
*
*                   (c) IEEE Std 1003.1, 2004 Edition, Section 'bind() : RETURN VALUE' states that :
*
*                       (1) "Upon successful completion, bind() shall return 0;" ...
*
*                       (2) (A) "Otherwise, -1 shall be returned," ...
*                           (B) "and 'errno' shall be set to indicate the error."
*                               #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   (d) (1) IEEE Std 1003.1, 2004 Edition, Section 'bind() : ERRORS' states that "the bind() 
*                           function shall fail if" :
*
*                           (A) "[EBADF] - The 'socket' argument is not a valid file descriptor."
*
*                           (B) "[EAFNOSUPPORT]  - The specified address is not a valid address for the address 
*                                family of the specified socket."
*
*                           (C) "[EADDRNOTAVAIL] - The specified address is not available from the local machine."
*
*                           (D) "[EADDRINUSE]    - The specified address is already in use."
*
*                           (E) "[EINVAL]" - 
*
*                               (1) (a) "The socket is already bound to an address,"                  ...
*                                   (b) "and the protocol does not support binding to a new address;" ...
*
*                               (2) "or the socket has been shut down."
*
*                       (2) IEEE Std 1003.1, 2004 Edition, Section 'bind() : ERRORS' states that "the bind() 
*                           function may fail if" :
*
*                           (A) "[EINVAL]  - The 'address_len' argument is not a valid length for the address 
*                                family."
*
*                           (B) "[EISCONN] - The socket is already connected."
*
*                           (C) "[ENOBUFS] - Insufficient resources were available to complete the call."
*
*                   See also 'NetSock_BindHandler()  Note #2'.
*
*               (4) (a) Socket address structure 'AddrFamily' member MUST be configured in host-order & 
*                       MUST NOT be converted to/from network-order.
*
*                   (b) Socket address structure addresses MUST be configured/converted from host-order
*                       to network-order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'
*                          & 'NetSock_BindHandler()  Note #3'.
*********************************************************************************************************
*/
/*$PAGE*/
NET_SOCK_RTN_CODE  NetSock_Bind (NET_SOCK_ID         sock_id,
                                 NET_SOCK_ADDR      *paddr_local,
                                 NET_SOCK_ADDR_LEN   addr_len,
                                 NET_ERR            *perr)
{
    NET_SOCK_RTN_CODE  rtn_code;

                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_BIND);
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_BIND);
    }
#endif

                                                                /* -------------------- BIND SOCK --------------------- */
    rtn_code = NetSock_BindHandler((NET_SOCK_ID      )sock_id,
                                   (NET_SOCK_ADDR   *)paddr_local,
                                   (NET_SOCK_ADDR_LEN)addr_len,
                                   (CPU_BOOLEAN      )DEF_NO,
                                   (NET_ERR         *)perr);

                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();

    return (rtn_code);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetSock_Conn()
*
* Description : (1) Connect a socket to a remote host :
*
*                   (a) Acquire  network lock
*                   (b) Validate socket  used
*                   (c) Validate remote  host address
*                   (d) Handle   socket  connection by socket type
*                   (e) Release  network lock
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to connect.
*
*               paddr_remote    Pointer to socket address structure (see Note #3).
*
*               addr_len        Length  of socket address structure (in octets).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket successfully connected to remote address.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid remote address.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*
*                                                                   --------- RETURNED BY NetSock_IsUsed() : ---------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*
*                                                                   -- RETURNED BY NetSock_ConnHandlerDatagram() : ---
*                                                                   --- RETURNED BY NetSock_ConnHandlerStream() : ----
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid socket address.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address structure length.
*                               NET_SOCK_ERR_ADDR_IN_USE            Socket address already in use.
*                               NET_SOCK_ERR_PORT_NBR_NONE_AVAIL    Port number NOT available.
*                               NET_SOCK_ERR_CONN_IN_USE            Socket connection already in use.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host address.
*
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_NONE_AVAIL             NO available connections to allocate.
*                               NET_CONN_ERR_NOT_USED               Network connection(s) NOT currently used.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_TYPE           Invalid network connection type.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
*                                                                   --- RETURNED BY NetSock_ConnHandlerStream() : ----
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT    Socket connection request NOT signaled by timeout.
*
*                                                                   ----------- RETURNED BY NetOS_Lock() : -----------
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*
* Return(s)   : NET_SOCK_BSD_ERR_NONE, if NO errors.
*
*               NET_SOCK_BSD_ERR_CONN, otherwise.
*
* Caller(s)   : connect().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_Conn() blocked until network initialization completes.
*
*                   See 'NetSock_IsUsed()  Note #1'.
*
*               (3) (a) Socket address structure 'AddrFamily' member MUST be configured in host-order & 
*                       MUST NOT be converted to/from network-order.
*
*                   (b) Socket address structure addresses MUST be configured/converted from host-order
*                       to network-order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (4) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'SockType' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
NET_SOCK_RTN_CODE  NetSock_Conn (NET_SOCK_ID         sock_id,
                                 NET_SOCK_ADDR      *paddr_remote,
                                 NET_SOCK_ADDR_LEN   addr_len,
                                 NET_ERR            *perr)
{
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    CPU_BOOLEAN         valid;
#endif
    NET_SOCK           *psock;
    NET_SOCK_RTN_CODE   rtn_code;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_CONN);
    }



#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_CONN);
    }
#endif


    psock = &NetSock_Tbl[sock_id];

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* --------------- VALIDATE REMOTE ADDR --------------- */
    valid = NetSock_IsValidAddrRemote(paddr_remote, addr_len, psock, perr);
    if (valid != DEF_YES) {
        NetOS_Unlock();
       *perr =  NET_SOCK_ERR_INVALID_ADDR;
        return (NET_SOCK_BSD_ERR_CONN);
    }
#endif


                                                                /* -------------------- CONN SOCK --------------------- */
    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             rtn_code = NetSock_ConnHandlerDatagram(sock_id, psock, paddr_remote, perr);
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             rtn_code = NetSock_ConnHandlerStream(sock_id, psock, paddr_remote, perr);
             break;
#endif

        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                /* See Note #4.                                         */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_TYPE;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


    return (rtn_code);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetSock_ConnSignalReq()
*
* Description : Signal socket that connection request complete; socket now connected.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to signal connection request.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection successfully signaled.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*
*                                                                   ------ RETURNED BY NetSock_IsUsed() : ------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*
*                                                                   - RETURNED BY NetOS_Sock_ConnReqSignal() : -
*                               NET_SOCK_ERR_CONN_SIGNAL_FAULT      Socket connection request signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_RxPktConnHandlerSignalConn().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) See 'NetSock_ConnHandlerStream()  Note #2c3'.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetSock_ConnSignalReq (NET_SOCK_ID   sock_id,
                             NET_ERR      *perr)
{
    NET_SOCK  *psock;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return;
    }
#endif

    psock = &NetSock_Tbl[sock_id];

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ---------------- VALIDATE SOCK TYPE ---------------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
       *perr = NET_SOCK_ERR_INVALID_TYPE;
        return;
    }

                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr = NET_SOCK_ERR_NOT_USED;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_LISTEN:                             /* See Note #1.                                         */
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
             break;


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr = NET_SOCK_ERR_CLOSED;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_DONE:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
            *perr = NET_SOCK_ERR_INVALID_OP;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_NONE:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr = NET_SOCK_ERR_INVALID_STATE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif


                                                                /* ----------------- SIGNAL SOCK CONN ----------------- */
    NetOS_Sock_ConnReqSignal(sock_id, perr);                    /* Signal sock conn req.                                */
    if (*perr != NET_SOCK_ERR_NONE) {
         return;
    }

    psock->State = NET_SOCK_STATE_CONN_DONE;                    /* Update sock state as conn done.                      */


   *perr = NET_SOCK_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_ConnSignalAccept()
*
* Description : Signal socket that connection request received; socket accept now available.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to signal connection accept.
*
*               conn_id     Handle identifier of network connection.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection successfully signaled.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*
*                                                                   --------- RETURNED BY NetSock_IsUsed() : ---------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*
*                                                                   - RETURNED BY NetSock_ConnAcceptQ_ConnID_Add() : -
*                               NET_SOCK_ERR_CONN_ACCEPT_Q_MAX      Socket maximum connection accept limit exceeded.
*                               NET_SOCK_ERR_CONN_ACCEPT_Q_DUP      Connection handle identifier already in socket
*                                                                       connection accept queue.
*
*                                                                   - RETURNED BY NetOS_Sock_ConnAcceptQ_Signal() : --
*                               NET_SOCK_ERR_CONN_SIGNAL_FAULT      Socket connection accept queue signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_RxPktConnHandlerSignalConn().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) On any faults, network connection NOT freed/closed; caller function(s) SHOULD handle
*                   fault condition(s).
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetSock_ConnSignalAccept (NET_SOCK_ID   sock_id,
                                NET_CONN_ID   conn_id,
                                NET_ERR      *perr)
{
    NET_SOCK  *psock;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return;
    }
#endif

    psock = &NetSock_Tbl[sock_id];

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ---------------- VALIDATE SOCK TYPE ---------------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
       *perr = NET_SOCK_ERR_INVALID_TYPE;
        return;
    }

                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr = NET_SOCK_ERR_NOT_USED;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_LISTEN:
             break;


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr = NET_SOCK_ERR_CLOSED;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
            *perr = NET_SOCK_ERR_INVALID_OP;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_NONE:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr = NET_SOCK_ERR_INVALID_STATE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif


                                                                /* --------- QUEUE & SIGNAL SOCK ACCEPT CONN ---------- */
    NetSock_ConnAcceptQ_ConnID_Add(psock, conn_id, perr);       /* Add conn id to sock accept Q.                        */
    if (*perr != NET_SOCK_ERR_NONE) {
         return;
    }

    NetOS_Sock_ConnAcceptQ_Signal(sock_id, perr);               /* Signal         sock accept Q.                        */
    if (*perr != NET_SOCK_ERR_NONE) {
         NetSock_ConnAcceptQ_Clr(psock);
         return;
    }


   *perr = NET_SOCK_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetSock_ConnSignalClose()
*
* Description : Signal socket that connection close complete; socket connection now closed.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to signal connection close.
*
*               data_avail      Indicate whether application data is still available for the socket connection :
*
*                                   DEF_YES                         Application data is     available for the
*                                                                       closing socket connection.
*                                   DEF_NO                          Application data is NOT available for the
*                                                                       closing socket connection.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection successfully signaled.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*
*                                                                   ------- RETURNED BY NetSock_IsUsed() : -------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*
*                                                                   - RETURNED BY NetOS_Sock_ConnCloseSignal() : -
*                               NET_SOCK_ERR_CONN_SIGNAL_FAULT      Socket connection close signal fault.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_RxPktConnHandlerSignalClose().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) NetSock_ConnSignalClose() blocked until network initialization completes.
*
*               (2) Once a socket connection has been signaled of its close :
*
*                   (a) Close socket connection
*                   (b) Close socket connection's reference to network connection
*                   (c) Do NOT close transport connection(s); transport layer responsible for closing its
*                           remaining connection(s)
*
*                   See also 'NetSock_CloseHandlerStream()  Note #2b'.
*
*               (3) (a) #### Since sockets that have already closed are NOT to be accessed (see 'NetSock_Close()
*                       Note #2'), non-blocking socket close may NOT require close completion.
*
*                   (b) #### 'data_avail' may NOT be necessary (remove if unnecessary).
*
*                   See Note 'NetSock_Close()  Note #2'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetSock_ConnSignalClose (NET_SOCK_ID   sock_id,
                               CPU_BOOLEAN   data_avail,
                               NET_ERR      *perr)
{
    NET_SOCK     *psock;
    CPU_BOOLEAN   block;

                                                                /* ------------------ VALIDATE SOCK ------------------- */
    if (sock_id == NET_SOCK_ID_NONE) {
       *perr = NET_SOCK_ERR_NONE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return;
    }
#endif

    psock = &NetSock_Tbl[sock_id];

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK TYPE ---------------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
       *perr = NET_SOCK_ERR_INVALID_TYPE;
        return;
    }
#endif

/*$PAGE*/
                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
            *perr = NET_SOCK_ERR_NOT_USED;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr = NET_SOCK_ERR_CLOSED;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
            *perr = NET_SOCK_ERR_INVALID_OP;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
             break;


        case NET_SOCK_STATE_NONE:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr = NET_SOCK_ERR_INVALID_STATE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ----------------- SIGNAL SOCK CONN ----------------- */
#if   (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_BLOCK)
    block = DEF_YES;
#elif (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_NO_BLOCK)
    block = DEF_NO;
#else
    block = DEF_YES;
#endif

    if (block != DEF_NO) {                                      /* If blocking sock conn, ...                           */
        if (data_avail  != DEF_YES) {
            psock->State = NET_SOCK_STATE_CLOSED;
        } else {
            psock->State = NET_SOCK_STATE_CLOSING_DATA_AVAIL;
        }

        NetOS_Sock_ConnCloseSignal(sock_id, perr);              /* ...  signal sock conn close.                         */
        if (*perr != NET_SOCK_ERR_NONE) {
             return;
        }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    } else {                                                    /* Else complete sock close (see Note #3a).             */
        if (data_avail != DEF_YES) {
            NetSock_CloseHandler((NET_SOCK  *)psock,            /* See Note #2a.                                        */
                                 (CPU_BOOLEAN)DEF_YES,          /* See Note #2b.                                        */
                                 (CPU_BOOLEAN)DEF_NO);          /* See Note #2c.                                        */
        } else {
            psock->State = NET_SOCK_STATE_CLOSING_DATA_AVAIL;
        }
#endif
    }


   *perr = NET_SOCK_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetSock_Listen()
*
* Description : (1) Set socket to listen for connection requests :
*
*                   (a) Acquire   network   lock
*                   (b) Validate  socket :
*                       (1) Validate socket used
*                       (2) Validate socket type
*                       (3) Validate socket connection state
*                   (c) Configure transport connection
*                   (d) Update    socket    connection state
*                   (e) Release   network   lock
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to listen.
*
*               sock_q_size     Maximum number of connection requests to accept & queue on listen socket.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket successfully set to listen.
*                               NET_SOCK_ERR_INVALID_FAMILY     Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP         Invalid socket operation.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection configuration failed.
*
*                                                               ------ RETURNED BY NetSock_IsUsed() : ------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*
*                                                               - RETURNED BY NetSock_GetConnTransport() : -
*                               NET_SOCK_ERR_INVALID_PROTOCOL   Invalid socket protocol.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
*                                                               -------- RETURNED BY NetOS_Lock() : --------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : NET_SOCK_BSD_ERR_NONE,   if NO errors.
*
*               NET_SOCK_BSD_ERR_LISTEN, otherwise.
*
* Caller(s)   : listen().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_Listen() blocked until network initialization completes.
*
*                   See 'NetSock_IsUsed()  Note #1'.
*
*               (3) Socket listen operation valid for stream-type sockets only.
*
*               (4) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'SockType' is incorrectly modified.
*
*               (5) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*
*               (6) On ANY errors after the transport connection is allocated, the transport connection
*                   MUST be freed.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
NET_SOCK_RTN_CODE  NetSock_Listen (NET_SOCK_ID       sock_id,
                                   NET_SOCK_Q_SIZE   sock_q_size,
                                   NET_ERR          *perr)
{
    NET_SOCK     *psock;
    NET_CONN_ID   conn_id_transport;
    NET_ERR       err;


/*$PAGE*/
                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_LISTEN);
    }



#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_LISTEN);
    }
#endif


    psock = &NetSock_Tbl[sock_id];
                                                                /* ---------------- VALIDATE SOCK TYPE ---------------- */
    switch (psock->SockType) {                                  /* Validate stream type sock(s) [see Note #3].          */
        case NET_SOCK_TYPE_DATAGRAM:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_TYPE;
             return (NET_SOCK_BSD_ERR_LISTEN);                  /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_TYPE_STREAM:
             break;


        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                /* See Note #4.                                         */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_TYPE;
             return (NET_SOCK_BSD_ERR_LISTEN);                  /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (NET_SOCK_BSD_ERR_LISTEN);                  /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_BOUND:
             break;


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;
             return (NET_SOCK_BSD_ERR_LISTEN);                  /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_OP;
             return (NET_SOCK_BSD_ERR_LISTEN);                  /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_NONE:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_STATE;
             return (NET_SOCK_BSD_ERR_LISTEN);                  /* Prevent 'break NOT reachable' compiler warning.      */
    }


/*$PAGE*/
                                                                /* ---------------- CFG TRANSPORT CONN ---------------- */
                                                                /* Get transport conn.                                  */
    conn_id_transport = NetSock_GetConnTransport(psock, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_LISTEN);
    }

                                                                /* Cfg transport to LISTEN.                             */
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
    NetTCP_ConnSetStateListen((NET_TCP_CONN_ID) conn_id_transport,
                              (NET_ERR       *)&err);
    if ( err != NET_TCP_ERR_NONE) {                             /* If any errs, free transport conn (see Note #6).      */
        NetTCP_ConnFree((NET_TCP_CONN_ID)conn_id_transport);
       *perr  = NET_SOCK_ERR_CONN_FAIL;
    } else {
       *perr  = NET_SOCK_ERR_NONE;
    }

#else
   (void)&conn_id_transport;                                    /* Prevent compiler warnings.                           */
   (void)&err;
   *perr = NET_SOCK_ERR_CONN_FAIL;
#endif

#else                                                           /* See Note #5.                                         */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
    NetOS_Unlock();
   *perr =  NET_SOCK_ERR_INVALID_FAMILY;
    return (NET_SOCK_BSD_ERR_LISTEN);
#endif

    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_LISTEN);
    }


                                                                /* -------------- UPDATE SOCK CONN STATE -------------- */
    NetSock_ConnAcceptQ_Init(psock, sock_q_size);               /* Init listen sock conn accept Q.                      */

    psock->State = NET_SOCK_STATE_LISTEN;


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


   *perr =  NET_SOCK_ERR_NONE;

    return (NET_SOCK_BSD_ERR_NONE);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetSock_ListenQ_IsAvail()
*
* Description : Check if socket's listen queue is available to queue a new connection.
*
*               (1) Socket connection accept queue synonymous with socket listen queue.
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to check listen queue.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket listen queue successfully checked; 
*                                                                   check return value for socket listen 
*                                                                   queue availability.
*
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP         Invalid socket operation.
*
*                                                               ---- RETURNED BY NetSock_IsUsed() : -----
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*
* Return(s)   : DEF_YES, if socket listen queue is available to queue a new connection.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetTCP_RxPktConnHandlerListenQ_IsAvail().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) (a) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 18.11, Pages 257-258 states
*                       that :
*
*                       (1) "Each listening end point has a fixed length queue of connections that have been
*                            accepted [i.e. connected] ... but not yet accepted by the application."
*
*                       (2) "The application specifies a limit to this queue, commonly called the backlog" :
*
*                           (A) "This backlog must be between 0 and 5, inclusive."
*                           (B) "(Most applications specify the maximum value of 5.)"
*
*                   (b) Wright/Stevens, TCP/IP Illustrated, Volume 2, 3rd Printing, Section 15.9, Page 455 
*                       reiterates that :
*
*                       (2) A "listen ... socket ... specifies a limit on the number of connections that can
*                           be queued on the socket," ...
*
*                       (5) "after which the socket layer refuses to queue additional connection requests."
*
*                   See also 'net_tcp.c  NetTCP_RxPktConnHandlerListenQ_IsAvail()  Note #1'.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
CPU_BOOLEAN  NetSock_ListenQ_IsAvail (NET_SOCK_ID   sock_id,
                                      NET_ERR      *perr)
{
    NET_SOCK     *psock;
    CPU_BOOLEAN   q_avail;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return (DEF_NO);
    }
#endif

    psock = &NetSock_Tbl[sock_id];

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ---------------- VALIDATE SOCK TYPE ---------------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
       *perr =  NET_SOCK_ERR_INVALID_TYPE;
        return (DEF_NO);
    }

                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (DEF_NO);                                   /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_LISTEN:
             break;


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;
             return (DEF_NO);                                   /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
            *perr =  NET_SOCK_ERR_INVALID_OP;
             return (DEF_NO);                                   /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_NONE:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr =  NET_SOCK_ERR_INVALID_STATE;
             return (DEF_NO);                                   /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif

                                                                /* ------------- CHK SOCK LISTEN Q AVAIL -------------- */
    q_avail = NetSock_ConnAcceptQ_IsAvail(psock, perr);         /* Chk if listen Q avail for new conns (see Note #2).   */

    return (q_avail);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetSock_Accept()
*
* Description : (1) Return a new socket accepted from a listen socket :
*
*                   (a) Validate address pointers
*                   (b) Acquire  network lock
*                   (c) Validate listen socket :
*                       (1) Validate listen socket used
*                       (2) Validate listen socket type
*                       (3) Validate listen socket connection state                         See Note #11
*
*                   (d) Wait on  listen socket's connection accept queue
*                   (e) Configure accept socket :
*                       (1) Get    accept socket from socket pool
*                       (2) Copy   accept socket from listen socket
*                       (3) Update network   connection's application layer handle identifier
*                               as accept socket's handle identifier
*                       (4) Update transport connection, if necessary
*                       (5) Update accept socket connection state
*
*                   (f) Return accept socket connection's remote address
*                   (g) Release network lock
*                   (h) Return  socket descriptor/handle identifier of new accept socket
*                         OR
*                       NET_SOCK_BSD_ERR_ACCEPT & error code, on failure
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of listen socket.
*
*               paddr_remote    Pointer to an address buffer that will receive the socket address structure
*                                   of the accepted socket's remote address (see Note #3).
*
*               paddr_len       Pointer to a variable to ... :
*
*                                   (a) Pass the size of the address buffer pointed to by 'paddr_remote'.
*                                   (b) (1) Return the actual size of socket address structure with the 
*                                               accepted socket's remote address, if NO errors;
*                                       (2) Return 0,                             otherwise.
*
*                               See also Note #4.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                       Socket successfully set to listen.
*                               NET_SOCK_ERR_NULL_PTR                   Argument 'paddr_remote'/'paddr_len' passed
*                                                                           a NULL pointer.
*                               NET_SOCK_ERR_NOT_USED                   Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                     Socket already closed.
*                               NET_SOCK_ERR_FAULT                      Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_INVALID_FAMILY             Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_TYPE               Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE              Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP                 Invalid socket operation.
*                               NET_SOCK_ERR_CONN_ACCEPT_Q_NONE_AVAIL   Accept connection handle identifier
*                                                                           NOT available.
*                               NET_SOCK_ERR_CONN_FAIL                  Socket connection operation(s) failed.
*
*                                                                       ------- RETURNED BY NetSock_IsUsed() : --------
*                               NET_ERR_INIT_INCOMPLETE                 Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK               Invalid socket number.
*
*                                                                       --------- RETURNED BY NetSock_Get() : ---------
*                               NET_SOCK_ERR_NONE_AVAIL                 NO available sockets to allocate.
*
*                                                                       - RETURNED BY NetOS_Sock_ConnAcceptQ_Wait() : -
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT        Socket connection accept queue still empty
*                                                                           by timeout.
*
*                                                                       --------- RETURNED BY NetOS_Lock() : ----------
*                               NET_OS_ERR_LOCK                         Network access NOT acquired.
*
* Return(s)   : Socket descriptor/handle identifier of new accepted socket, if NO errors.
*
*               NET_SOCK_BSD_ERR_ACCEPT,                                    otherwise.
*
* Caller(s)   : accept().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*$PAGE*
* Note(s)     : (2) NetSock_Accept() blocked until network initialization completes.
*
*                   See 'NetSock_IsUsed()  Note #1'.
*
*               (3) (a) Socket address structure 'AddrFamily' member returned in host-order & SHOULD NOT 
*                       be converted to network-order.
*
*                   (b) Socket address structure addresses MUST be converted from host-order to network-
*                       order.
*
*                       See also Note #10.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (4) Since 'paddr_len' parameter is both an input & output parameter (see 'Argument(s) :
*                   paddr_len'), ... :
*
*                   (a) Its input value SHOULD be validated prior to use.
*                       (1) In the case that the 'paddr_len' parameter is passed a null pointer,
*                           NO input value is validated or used.
*
*                   (b) While its output value MUST be initially configured to return a default value
*                       in case of any function exit due to error/fault conditions (see also Note #5).
*
*               (5) Pointers to variables that return values MUST be initialized to return PRIOR to all
*                   other validation or function handling in case of any error(s).
*
*               (6) Socket accept operation valid for stream-type sockets only.
*
*               (7) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'SockType' is incorrectly modified.
*
*               (8) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be compiled/linked
*                   since 'net_sock.h' ensures that the family type configuration constant (NET_SOCK_CFG_FAMILY)
*                   is configured with an appropriate family type value (see 'net_sock.h  CONFIGURATION ERRORS').
*                   The 'else'-conditional code is included for completeness & as an extra precaution in case
*                   'net_sock.h' is incorrectly modified.
*
*               (9) On ANY errors, network resources MUST be appropriately freed :
*
*                   (a) PRIOR to network connection handle identifier dequeued from listen socket's
*                       connection accept queue, only the listen socket need be freed for certain
*                       errors; NO network resources need be freed.
*
*                   (b) After    network connection handle identifier dequeued from listen socket's 
*                       connection accept queue, free network connection.
*
*                   (c) After new accept socket allocated, free network connection & new socket.
*                       (1) Socket close handler frees network connection (see 'NetSock_CloseSockHandler()  
*                           Note #2').
*
*              (10) (a) Socket connection addresses are maintained in network-order.
*
*                   (b) Since the port number & address are copied from a network-order multi-octet array 
*                       into local variables & then into a network-order socket address structure, they do
*                       NOT need to be converted from host-order to network-order.
*
*              (11) Socket descriptor read availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor read handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also  Note #1c3,
*                            'NetSock_SelDescHandlerRdStream()   Note #3',
*                          & 'NetSock_SelDescHandlerErrStream()  Note #3'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
NET_SOCK_ID  NetSock_Accept (NET_SOCK_ID         sock_id,
                             NET_SOCK_ADDR      *paddr_remote,
                             NET_SOCK_ADDR_LEN  *paddr_len,
                             NET_ERR            *perr)
{
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_SOCK_ADDR_IP   *paddr_ip;
    NET_IP_ADDR         remote_addr;
#endif
    NET_PORT_NBR        remote_port;
    CPU_INT08U          addr_remote[NET_SOCK_CFG_ADDR_LEN];
    NET_SOCK_ADDR_LEN   addr_len;
    NET_SOCK           *psock_listen;
    NET_SOCK           *psock_accept;
    NET_SOCK_ID         sock_id_accept;
    NET_CONN_ID         conn_id_accept;
    NET_CONN_ID         conn_id_transport;
    CPU_BOOLEAN         block;
    NET_ERR             err;


/*$PAGE*/
                                                                        /* ------------ VALIDATE ADDR PTRS ------------ */
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (paddr_len == (NET_SOCK_ADDR_LEN *)0) {                          /* See Note #4a1.                               */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNullPtrCtr);
       *perr =  NET_SOCK_ERR_NULL_PTR;
        return (NET_SOCK_BSD_ERR_ACCEPT);
    }

     addr_len = *paddr_len;
#endif
   *paddr_len =  0;                                                     /* Cfg dflt addr len for err (see Note #4b).    */

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (addr_len < NET_SOCK_ADDR_SIZE) {                                /* Validate initial addr len (see Note #4a).    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidAddrLenCtr);
       *perr =  NET_SOCK_ERR_INVALID_ADDR_LEN;
        return (NET_SOCK_BSD_ERR_ACCEPT);
    }

    if (paddr_remote == (NET_SOCK_ADDR *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNullPtrCtr);
       *perr =  NET_SOCK_ERR_NULL_PTR;
        return (NET_SOCK_BSD_ERR_ACCEPT);
    }
#endif


                                                                        /* ------------- ACQUIRE NET LOCK ------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_ACCEPT);
    }



#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                         /* -------- VALIDATE LISTEN SOCK USED --------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_ACCEPT);
    }
#endif


    psock_listen = &NetSock_Tbl[sock_id];
                                                                        /* -------- VALIDATE LISTEN SOCK TYPE --------- */
    switch (psock_listen->SockType) {                                   /* Validate stream type sock(s) [see Note #6].  */
        case NET_SOCK_TYPE_DATAGRAM:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_TYPE;
             return (NET_SOCK_BSD_ERR_ACCEPT);                          /* Prevent 'break NOT reachable' warning.       */


        case NET_SOCK_TYPE_STREAM:
             break;


        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                        /* See Note #7.                                 */
             NetSock_CloseSock(psock_listen, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_TYPE;
             return (NET_SOCK_BSD_ERR_ACCEPT);                          /* Prevent 'break NOT reachable' warning.       */
    }


/*$PAGE*/
                                                                        /* ----- VALIDATE LISTEN SOCK CONN STATE ------ */
    switch (psock_listen->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (NET_SOCK_BSD_ERR_ACCEPT);                          /* Prevent 'break NOT reachable' warning.       */


        case NET_SOCK_STATE_LISTEN:
             break;


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;
             return (NET_SOCK_BSD_ERR_ACCEPT);                          /* Prevent 'break NOT reachable' warning.       */
             

        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_OP;
             return (NET_SOCK_BSD_ERR_ACCEPT);                          /* Prevent 'break NOT reachable' warning.       */
             

        case NET_SOCK_STATE_NONE:
        default:
             NetSock_CloseSock(psock_listen, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_STATE;
             return (NET_SOCK_BSD_ERR_ACCEPT);                          /* Prevent 'break NOT reachable' warning.       */
    }



/*$PAGE*/
                                                                        /* ------- WAIT ON LISTEN SOCK ACCEPT Q ------- */
#if   (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_BLOCK)
    block = DEF_YES;
#elif (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_NO_BLOCK)
    block = DEF_NO;
#else
    block = DEF_YES;
#endif

    if (block != DEF_YES) {                                             /* If non-blocking sock rx  ...                 */
        if (psock_listen->ConnAcceptQ_SizeCur < 1) {                    /* ... & no conn req's q'd, ...                 */
            NetOS_Unlock();
           *perr =  NET_SOCK_ERR_CONN_ACCEPT_Q_NONE_AVAIL;              /* ... rtn  conn accept Q empty err.            */
            return (NET_SOCK_BSD_ERR_ACCEPT);
        }
    }

    NetOS_Unlock();
    NetOS_Sock_ConnAcceptQ_Wait(sock_id, perr);
    NetOS_Lock(&err);
    if ( err != NET_OS_ERR_NONE) {
       *perr  = err;                                                    /* Rtn err from NetOS_Lock().                   */
        return (NET_SOCK_BSD_ERR_ACCEPT);
    }
    switch (*perr) {
        case NET_SOCK_ERR_NONE:
             break;


        case NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT:
             NetOS_Unlock();
                                                                        /* Rtn err from NetOS_Sock_ConnAcceptQ_Wait().  */
             return (NET_SOCK_BSD_ERR_ACCEPT);                          /* Prevent 'break NOT reachable' warning.       */


        case NET_SOCK_ERR_CONN_SIGNAL_ABORT:
        case NET_SOCK_ERR_CONN_SIGNAL_FAULT:
        default:
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_FAULT;
             return (NET_SOCK_BSD_ERR_ACCEPT);                          /* Prevent 'break NOT reachable' warning.       */
    }

                                                                        /* Get conn id from sock conn accept Q.         */
    conn_id_accept = NetSock_ConnAcceptQ_ConnID_Get(psock_listen, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_ACCEPT);
    }
    if (conn_id_accept == NET_CONN_ID_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_ACCEPT);
    }

                                                                        /* Validate transport conn.                     */
    conn_id_transport = NetConn_ID_TransportGet(conn_id_accept, &err);
    if ( err != NET_CONN_ERR_NONE) {                                    /* See Note #9b.                                */
        NetSock_CloseConn(conn_id_accept);
        NetOS_Unlock();
       *perr  = NET_SOCK_ERR_CONN_FAIL;
        return (NET_SOCK_BSD_ERR_ACCEPT);
    }
    if (conn_id_transport == NET_CONN_ID_NONE) {
        NetSock_CloseConn(conn_id_accept);
        NetOS_Unlock();
       *perr  = NET_SOCK_ERR_CONN_FAIL;
        return (NET_SOCK_BSD_ERR_ACCEPT);
    }



/*$PAGE*/
                                                                        /* ----------- CFG NEW ACCEPT SOCK ------------ */
    psock_accept = NetSock_Get(perr);
    if (psock_accept == (NET_SOCK *)0) {                                /* See Note #9b.                                */
        NetSock_CloseConn(conn_id_accept);
        NetOS_Unlock();
        return (NET_SOCK_BSD_ERR_ACCEPT);                               /* Rtn err from NetSock_Get().                  */
    }

                                                                        /* Copy listen sock into new accept sock.       */
    NetSock_Copy(psock_accept, psock_listen);

                                                                        /* Set new conn & app id's.                     */
    psock_accept->ID_Conn = conn_id_accept;
    sock_id_accept        = psock_accept->ID;
    NetConn_ID_AppSet((NET_CONN_ID) conn_id_accept,
                      (NET_CONN_ID) sock_id_accept,
                      (NET_ERR   *)&err);
    if ( err != NET_CONN_ERR_NONE) {                                    /* See Note #9c.                                */
        NetSock_CloseHandler(psock_accept, DEF_YES, DEF_YES);
        NetOS_Unlock();
       *perr  = NET_SOCK_ERR_CONN_FAIL;
        return (NET_SOCK_BSD_ERR_ACCEPT);
    }

                                                                        /* Update accept sock conn state.               */
    psock_accept->State = NET_SOCK_STATE_CONN;


                                                                        /* ------ RTN ACCEPT CONN'S REMOTE ADDR ------- */
                                                                        /* Get conn's remote addr.                      */
    addr_len = sizeof(addr_remote);
    NetConn_AddrRemoteGet((NET_CONN_ID        ) conn_id_accept,
                          (CPU_INT08U        *)&addr_remote[0],
                          (NET_CONN_ADDR_LEN *)&addr_len,
                          (NET_ERR           *)&err);
    if ( err != NET_CONN_ERR_NONE) {                                    /* See Note #9c.                                */
        NetSock_CloseHandler(psock_accept, DEF_YES, DEF_YES);
        NetOS_Unlock();
       *perr  = NET_SOCK_ERR_CONN_FAIL;
        return (NET_SOCK_BSD_ERR_ACCEPT);
    }
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (addr_len != NET_SOCK_CFG_ADDR_LEN) {                            /* See Note #9c.                                */
        NetSock_CloseHandler(psock_accept, DEF_YES, DEF_YES);
       *perr =  NET_SOCK_ERR_INVALID_ADDR_LEN;
        return (NET_SOCK_BSD_ERR_ACCEPT);
    }
#endif

                                                                        /* Cfg & rtn sock conn's remote addr.           */
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    Mem_Copy((void     *)&remote_port,
             (void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_PORT],
             (CPU_SIZE_T) sizeof(remote_port));
    Mem_Copy((void     *)&remote_addr,
             (void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_ADDR],
             (CPU_SIZE_T) sizeof(remote_addr));

    paddr_ip  = (NET_SOCK_ADDR_IP *)paddr_remote;                       /* Cfg remote addr struct   (see Note #3).      */
    NET_UTIL_VAL_SET_HOST_16(&paddr_ip->AddrFamily, NET_SOCK_ADDR_FAMILY_IP_V4);
    NET_UTIL_VAL_COPY_16(&paddr_ip->Port, &remote_port);                /* Copy preserves net-order (see Note #10b).    */
    NET_UTIL_VAL_COPY_32(&paddr_ip->Addr, &remote_addr);                /* Copy preserves net-order (see Note #10b).    */
    Mem_Clr((void     *) &paddr_ip->Unused[0],
            (CPU_SIZE_T)  NET_SOCK_ADDR_IP_NBR_OCTETS_UNUSED);

   *paddr_len =  NET_SOCK_ADDR_IP_SIZE;

#else                                                                   /* See Notes #8 & #9c.                          */
    NetSock_CloseHandler(psock_accept, DEF_YES, DEF_YES);
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
    NetOS_Unlock();
   *perr =  NET_SOCK_ERR_INVALID_FAMILY;
    return (NET_SOCK_BSD_ERR_ACCEPT);
#endif


                                                                        /* ------------- RELEASE NET LOCK ------------- */
    NetOS_Unlock();


   *perr =  NET_SOCK_ERR_NONE;

    return (sock_id_accept);                                            /* ------------- RTN NEW SOCK ID -------------- */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetSock_RxDataFrom()
*
* Description : (1) Receive data from a socket :
*
*                   (a) Validate   receive address buffer                           See Notes #5 & 6
*                   (b) Validate & receive socket  data
*
*                   See also 'NetSock_RxDataHandler()  Note #1'.
*
*
* Argument(s) : sock_id             Socket descriptor/handle identifier of socket to receive data.
*
*               pdata_buf           Pointer to an application data buffer that will receive the socket's received
*                                       data.
*
*               data_buf_len        Size of the   application data buffer (in octets) [see Note #3].
*
*               flags               Flags to select receive options (see Note #4); bit-field flags logically OR'd :
*
*                                       NET_SOCK_FLAG_NONE              No socket flags selected.
*                                       NET_SOCK_FLAG_RX_DATA_PEEK      Receive socket data without consuming
*                                                                           the socket data; i.e. socket data 
*                                                                           NOT removed from application receive
*                                                                           queue(s).
*                                       NET_SOCK_FLAG_RX_NO_BLOCK       Receive socket data without blocking.
*
*               paddr_remote        Pointer to an address buffer that will receive the socket address structure 
*                                       with the received data's remote address (see Note #5), if NO errors.
*
*               paddr_len           Pointer to a variable to ... :
*
*                                       (a) Pass the size of the address buffer pointed to by 'paddr_remote'.
*                                       (b) (1) Return the actual size of socket address structure with the 
*                                                   received data's remote address, if NO errors;
*                                           (2) Return 0,                           otherwise.
*
*                                   See also Note #6.
*
*               pip_opts_buf        Pointer to buffer to receive possible IP options (see Note #7a), if NO errors.
*
*               ip_opts_buf_len     Size of IP options receive buffer (in octets)    [see Note #7b].
*
*               pip_opts_len        Pointer to variable that will receive the return size of any received IP options,
*                                       if NO errors.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*
*                                                                   ---- RETURNED BY NetSock_RxDataHandler() : ----
*                               NET_SOCK_ERR_NONE                   Socket data successfully received; check return
*                                                                       value for number of data octets received.
*
*                               NET_SOCK_ERR_INVALID_DATA_SIZE      Socket data receive buffer insufficient size;
*                                                                       some, but not all, socket data deframed
*                                                                       into receive buffer (see Note #3a2).
*
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_NULL_PTR               Argument 'paddr_len'/'paddr_remote'/
*                                                                       'pdata_buf' passed a NULL pointer.
*                               NET_SOCK_ERR_NULL_SIZE              Argument 'data_buf_len' passed a NULL size.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol/address family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_FLAG           Invalid socket flags.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address length.
*
*                               NET_SOCK_ERR_RX_Q_EMPTY             Socket receive queue empty.
*                               NET_SOCK_ERR_RX_Q_CLOSED            Socket receive queue closed.
*
*                               NET_ERR_RX                          Receive error.
*
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*$PAGE*
* Return(s)   : Number of positive data octets received, if NO errors                (see Note #8a).
*
*               NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,       if socket connection closed (see Note #8b).
*
*               NET_SOCK_BSD_ERR_RX,                     otherwise                   (see Note #8c1).
*
* Caller(s)   : recvfrom().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_RxDataFrom() blocked until network initialization completes.
*
*                   See 'NetSock_IsUsed()  Note #1'.
*
*               (3) (a) (1) (A) Datagram-type sockets transmit & receive all data atomically -- i.e. every 
*                               single, complete datagram transmitted MUST be received as a single, complete
*                               datagram.
*
*                           (B) IEEE Std 1003.1, 2004 Edition, Section 'recvfrom() : DESCRIPTION' summarizes 
*                               that "for message-based sockets, such as ... SOCK_DGRAM ... the entire message 
*                               shall be read in a single operation.  If a message is too long to fit in the 
*                               supplied buffer, and MSG_PEEK is not set in the flags argument, the excess 
*                               bytes shall be discarded".
*
*                       (2) Thus, if the socket's type is datagram & the receive data buffer size is 
*                           NOT large enough for the received data, the receive data buffer is maximally 
*                           filled with receive data but the remaining data octets are discarded & 
*                           NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
*
*                   (b) (1) (A) (1) Stream-type sockets transmit & receive all data octets in one or more 
*                                   non-distinct packets.  In other words, the application data is NOT 
*                                   bounded by any specific packet(s); rather, it is contiguous & sequenced 
*                                   from one packet to the next.
*
*                               (2) IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes
*                                   that "for stream-based sockets, such as SOCK_STREAM, message boundaries 
*                                   shall be ignored.  In this case, data shall be returned to the user as
*                                   soon as it becomes available, and no data shall be discarded".
*
*                           (B) Thus, if the socket's type is stream & the receive data buffer size is NOT 
*                               large enough for the received data, the receive data buffer is maximally 
*                               filled with receive data & the remaining data octets remain queued for 
*                               later application-socket receives.
*                   
*                       (2) Thus, it is typical -- but NOT absolutely required -- that a single application
*                           task ONLY receive or request to receive data from a stream-type socket.
*
*                   See also 'NetSock_RxDataHandler()  Note #2'.
*
*               (4) #### Only some socket receive flag options are implemented.  If other flag options are 
*                   requested, NetSock_RxData() handler function(s) abort & return appropriate error codes
*                   so that requested flag options are NOT silently ignored.
*
*               (5) (a) Socket address structure 'AddrFamily' member returned in host-order & SHOULD NOT 
*                       be converted to network-order.
*
*                   (b) Socket address structure addresses MUST be converted from host-order to network-
*                       order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*$PAGE*
*               (6) Since 'paddr_len' parameter is both an input & output parameter (see 'Argument(s) :
*                   paddr_len'), ... :
*
*                   (a) Its input value SHOULD be validated prior to use.
*                       (1) In the case that the 'paddr_len' parameter is passed a null pointer,
*                           NO input value is validated or used.
*
*                   (b) While its output value MUST be initially configured to return a default value
*                       in case of any function exit due to error/fault conditions (see also Note #9).
*
*               (7) (a) If ...
*
*                       (1) NO IP options were received
*                             OR
*                       (2) NO IP options receive buffer is provided
*                             OR
*                       (3) IP options receive buffer NOT large enough for the received IP options
*
*                       ... then NO IP options are returned & any received IP options are silently discarded.
*
*                   (b) The IP options receive buffer size SHOULD be large enough to receive the maximum
*                       IP options size, NET_IP_HDR_OPT_SIZE_MAX.
*
*                   (c) #### Received IP options should be provided/decoded via appropriate IP layer API.
*
*                   See also Note #10.
*
*               (8) IEEE Std 1003.1, 2004 Edition, Section 'recvfrom() : RETURN VALUE' states that :
*
*                   (a) "Upon successful completion, recvfrom() shall return the length of the message in
*                        bytes."
*
*                   (b) "If no messages are available to be received and the peer has performed an orderly
*                        shutdown, recvfrom() shall return 0."
*
*                   (c) (1) "Otherwise, [-1 shall be returned]" ...
*                       (2) "and 'errno' set to indicate the error."
*                           #### 'errno' NOT currently supported 
*                                (see 'net_bsd.h  Note #1b').
*
*                   See also 'NetSock_RxDataHandler()  Note #7'.
*
*               (9) Pointers to variables that return values MUST be initialized to return PRIOR to all
*                   other validation or function handling in case of any error(s).
*
*              (10) #### IP options arguments may NOT be necessary (remove if unnecessary).
*********************************************************************************************************
*/
/*$PAGE*/
NET_SOCK_RTN_CODE  NetSock_RxDataFrom (NET_SOCK_ID         sock_id,
                                       void               *pdata_buf,
                                       CPU_INT16U          data_buf_len,
                                       CPU_INT16S          flags,
                                       NET_SOCK_ADDR      *paddr_remote,
                                       NET_SOCK_ADDR_LEN  *paddr_len,
                                       void               *pip_opts_buf,
                                       CPU_INT08U          ip_opts_buf_len,
                                       CPU_INT08U         *pip_opts_len,
                                       NET_ERR            *perr)
{
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    NET_SOCK_ADDR_LEN  addr_len;
#endif
    NET_SOCK_RTN_CODE  rtn_code;


    if (pip_opts_len != (CPU_INT08U *)0) {                      /* Init len for err (see Note #9).                      */
       *pip_opts_len  =  0;
    }


                                                                /* --------------- VALIDATE RX ADDR BUF --------------- */
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (paddr_len == (NET_SOCK_ADDR_LEN *)0) {                  /* See Note #6a1.                                       */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNullPtrCtr);
       *perr =  NET_SOCK_ERR_NULL_PTR;
        return (NET_SOCK_BSD_ERR_RX);
    }

     addr_len = *paddr_len;
#endif
   *paddr_len =  0;                                             /* Cfg dflt addr len for err (see Note #6b).            */

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (addr_len < NET_SOCK_ADDR_SIZE) {                        /* Validate initial addr len (see Note #6a).            */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidAddrLenCtr);
       *perr =  NET_SOCK_ERR_INVALID_ADDR_LEN;
        return (NET_SOCK_BSD_ERR_RX);
    }

    if (paddr_remote == (NET_SOCK_ADDR *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNullPtrCtr);
       *perr =  NET_SOCK_ERR_NULL_PTR;
        return (NET_SOCK_BSD_ERR_RX);
    }
#endif


                                                                /* -------------- VALIDATE/RX SOCK DATA --------------- */
    rtn_code = NetSock_RxDataHandler((NET_SOCK_ID        )sock_id,
                                     (void              *)pdata_buf,
                                     (CPU_INT16U         )data_buf_len,
                                     (CPU_INT16S         )flags,
                                     (NET_SOCK_ADDR     *)paddr_remote,
                                     (NET_SOCK_ADDR_LEN *)paddr_len,
                                     (void              *)pip_opts_buf,
                                     (CPU_INT08U         )ip_opts_buf_len,
                                     (CPU_INT08U        *)pip_opts_len,
                                     (NET_ERR           *)perr);

    return (rtn_code);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetSock_RxData()
*
* Description : (1) Receive data from a socket :
*
*                   (a) Validate & receive socket data
*
*                   See also 'NetSock_RxDataHandler()  Note #1'.
*
*
* Argument(s) : sock_id             Socket descriptor/handle identifier of socket to receive data.
*
*               pdata_buf           Pointer to an application data buffer that will receive the socket's received
*                                       data.
*
*               data_buf_len        Size of the   application data buffer (in octets) [see Note #3].
*
*               flags               Flags to select receive options (see Note #4); bit-field flags logically OR'd :
*
*                                       NET_SOCK_FLAG_NONE              No socket flags selected.
*                                       NET_SOCK_FLAG_RX_DATA_PEEK      Receive socket data without consuming
*                                                                           the socket data; i.e. socket data 
*                                                                           NOT removed from application receive
*                                                                           queue(s).
*                                       NET_SOCK_FLAG_RX_NO_BLOCK       Receive socket data without blocking.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                                   ---- RETURNED BY NetSock_RxDataHandler() : ----
*                               NET_SOCK_ERR_NONE                   Socket data successfully received; check return
*                                                                       value for number of data octets received.
*
*                               NET_SOCK_ERR_INVALID_DATA_SIZE      Socket data receive buffer insufficient size;
*                                                                       some, but not all, socket data deframed
*                                                                       into receive buffer (see Note #3a2).
*
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_NULL_PTR               Argument 'pdata_buf'    passed a NULL pointer.
*                               NET_SOCK_ERR_NULL_SIZE              Argument 'data_buf_len' passed a NULL size.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol/address family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_FLAG           Invalid socket flags.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address length.
*
*                               NET_SOCK_ERR_RX_Q_EMPTY             Socket receive queue empty.
*                               NET_SOCK_ERR_RX_Q_CLOSED            Socket receive queue closed.
*
*                               NET_ERR_RX                          Receive error.
*
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*
* Return(s)   : Number of positive data octets received, if NO errors                (see Note #5a).
*
*               NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,       if socket connection closed (see Note #5b).
*
*               NET_SOCK_BSD_ERR_RX,                     otherwise                   (see Note #5c1).
*
* Caller(s)   : recv().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*$PAGE*
* Note(s)     : (2) NetSock_RxData() blocked until network initialization completes.
*
*                   See 'NetSock_IsUsed()  Note #1'.
*
*               (3) (a) (1) (A) Datagram-type sockets transmit & receive all data atomically -- i.e. every 
*                               single, complete datagram transmitted MUST be received as a single, complete
*                               datagram.
*
*                           (B) IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes
*                               that "for message-based sockets, such as SOCK_DGRAM ... the entire message 
*                               shall be read in a single operation.  If a message is too long to fit in 
*                               the supplied buffer, and MSG_PEEK is not set in the flags argument, the 
*                               excess bytes shall be discarded".
*
*                       (2) Thus, if the socket's type is datagram & the receive data buffer size is 
*                           NOT large enough for the received data, the receive data buffer is maximally 
*                           filled with receive data but the remaining data octets are discarded & 
*                           NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
*
*                   (b) (1) (A) (1) Stream-type sockets transmit & receive all data octets in one or more 
*                                   non-distinct packets.  In other words, the application data is NOT 
*                                   bounded by any specific packet(s); rather, it is contiguous & sequenced 
*                                   from one packet to the next.
*
*                               (2) IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes
*                                   that "for stream-based sockets, such as SOCK_STREAM, message boundaries 
*                                   shall be ignored.  In this case, data shall be returned to the user as
*                                   soon as it becomes available, and no data shall be discarded".
*
*                           (B) Thus, if the socket's type is stream & the receive data buffer size is NOT 
*                               large enough for the received data, the receive data buffer is maximally 
*                               filled with receive data & the remaining data octets remain queued for 
*                               later application-socket receives.
*                   
*                       (2) Thus, it is typical -- but NOT absolutely required -- that a single application
*                           task ONLY receive or request to receive data from a stream-type socket.
*
*                   See also 'NetSock_RxDataHandler()  Note #2'.
*
*               (4) #### Only some socket receive flag options are implemented.  If other flag options are 
*                   requested, NetSock_RxData() handler function(s) abort & return appropriate error codes
*                   so that requested flag options are NOT silently ignored.
*
*               (5) IEEE Std 1003.1, 2004 Edition, Section 'recv() : RETURN VALUE' states that :
*
*                   (a) "Upon successful completion, recv() shall return the length of the message in bytes."
*
*                   (b) "If no messages are available to be received and the peer has performed an orderly
*                        shutdown, recv() shall return 0."
*
*                   (c) (1) "Otherwise, -1 shall be returned" ...
*                       (2) "and 'errno' set to indicate the error."
*                           #### 'errno' NOT currently supported 
*                                (see 'net_bsd.h  Note #1b').
*
*                   See also 'NetSock_RxDataHandler()  Note #7'.
*********************************************************************************************************
*/
/*$PAGE*/
NET_SOCK_RTN_CODE  NetSock_RxData (NET_SOCK_ID   sock_id,
                                   void         *pdata_buf,
                                   CPU_INT16U    data_buf_len,
                                   CPU_INT16S    flags,
                                   NET_ERR      *perr)
{
    NET_SOCK_RTN_CODE  rtn_code;

                                                                /* -------------- VALIDATE/RX SOCK DATA --------------- */
    rtn_code = NetSock_RxDataHandler((NET_SOCK_ID        )sock_id,
                                     (void              *)pdata_buf,
                                     (CPU_INT16U         )data_buf_len,
                                     (CPU_INT16S         )flags,
                                     (NET_SOCK_ADDR     *)0,
                                     (NET_SOCK_ADDR_LEN *)0,
                                     (void              *)0,
                                     (CPU_INT08U         )0,
                                     (CPU_INT08U        *)0,
                                     (NET_ERR           *)perr);

    return (rtn_code);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetSock_TxDataTo()
*
* Description : (1) Transmit data through a socket :
*
*                   (a) Validate & transmit application data
*
*                   See also 'NetSock_TxDataHandler()  Note #1'.
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to transmit data.
*
*               p_data          Pointer to application data to transmit.
*
*               data_len        Length of  application data to transmit (in octets) [see Note #3].
*
*               flags           Flags to select transmit options (see Note #4); bit-field flags logically OR'd :
*
*                                   NET_SOCK_FLAG_NONE              No socket flags selected.
*                                   NET_SOCK_FLAG_TX_NO_BLOCK       Transmit socket data without blocking.
*
*               paddr_remote    Pointer to destination address buffer (see Note #5).
*
*               addr_len        Length of  destination address buffer (in octets).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                                   ----- RETURNED BY NetSock_TxDataHandler() : -------
*                               NET_SOCK_ERR_NONE                   Socket data successfully transmitted &/or prepared 
*                                                                       for transmission; check return value for number 
*                                                                       of data octets transmitted (see Note #6a1).
*
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_NULL_PTR               Argument 'p_data' passed a NULL pointer.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol/address family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_FLAG           Invalid socket flags.
*                               NET_SOCK_ERR_INVALID_DATA_SIZE      Invalid data size (see Notes #3b & #3a1B2).
*
*                               NET_SOCK_ERR_INVALID_CONN           Invalid socket connection.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address structure length.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid socket address.
*                               NET_SOCK_ERR_INVALID_PORT_NBR       Invalid socket port number.
*                               NET_SOCK_ERR_PORT_NBR_NONE_AVAIL    Port number NOT available.
*                               NET_SOCK_ERR_ADDR_IN_USE            Socket address already in use.
*
*                               NET_SOCK_ERR_TX_Q_CLOSED            Socket transmit queue closed.
*
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host address.
*
*                               NET_ERR_TX                          Transitory transmit error.
*
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_TYPE           Invalid network connection type.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_NONE_AVAIL             NO available connections to allocate.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*
* Return(s)   : Number of positive data octets transmitted, if NO errors                (see Note #6a1).
*
*               NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,          if socket connection closed (see Note #6b).
*
*               NET_SOCK_BSD_ERR_TX,                        otherwise                   (see Note #6a2A).
*
* Caller(s)   : sendto().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*$PAGE*
* Note(s)     : (2) NetSock_TxDataTo() blocked until network initialization completes.
*
*                   See 'NetSock_IsUsed()  Note #1'.
*
*               (3) (a) (1) (A) Datagram-type sockets transmit & receive all data atomically -- i.e. every
*                               single, complete datagram transmitted MUST be received as a single, complete
*                               datagram.  Thus, each call to transmit data MUST be transmitted in a single, 
*                               complete datagram.
*
*                           (B) (1) IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states
*                                   that "if the message is too long to pass through the underlying protocol,
*                                   send() shall fail and no data shall be transmitted".
*
*                               (2) Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h 
*                                   Note #1e'), if the socket's type is datagram & the requested transmit 
*                                   data length is greater than the socket/transport layer MTU, then NO data 
*                                   is transmitted & NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
*
*                       (2) (A) (1) Stream-type sockets transmit & receive all data octets in one or more 
*                                   non-distinct packets.  In other words, the application data is NOT 
*                                   bounded by any specific packet(s); rather, it is contiguous & sequenced 
*                                   from one packet to the next.
*
*                               (2) Thus, if the socket's type is stream & the socket's transmit data queue(s)
*                                   are NOT large enough for the transmitted data, the  transmit data queue(s) 
*                                   are maximally filled with transmit data & the remaining data octets are
*                                   discarded but may be re-transmitted by later application-socket transmits.
*
*                               (3) Therefore, NO stream-type socket transmit data length should be "too long
*                                   to pass through the underlying protocol" & cause the socket transmit to 
*                                   "fail ... [with] no data ... transmitted" (see Note #3a1B1).
*
*                           (B) Thus, it is typical -- but NOT absolutely required -- that a single application
*                               task ONLY transmit or request to transmit data to a stream-type socket.
*
*                   (b) 'data_len' of 0 octets NOT allowed.
*
*                   See also 'NetSock_TxDataHandler()  Note #2'.
*
*               (4) #### Only some socket transmit flag options are implemented.  If other flag options are 
*                   requested, NetSock_TxData() handler function(s) abort & return appropriate error codes
*                   so that requested flag options are NOT silently ignored.
*
*               (5) (a) Socket address structure 'AddrFamily' member MUST be configured in host-order & 
*                       MUST NOT be converted to/from network-order.
*
*                   (b) Socket address structure addresses MUST be configured/converted from host-order
*                       to network-order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (6) (a) IEEE Std 1003.1, 2004 Edition, Section 'sendto() : RETURN VALUE' states that :
*
*                       (1) "Upon successful completion, sendto() shall return the number of bytes sent."
*
*                           (A) Section 'sendto() : DESCRIPTION' elaborates that "successful completion 
*                               of a call to sendto() does not guarantee delivery of the message".
*
*                           (B) (1) Thus, applications SHOULD verify the actual returned number of data 
*                                   octets transmitted &/or prepared for transmission.
*
*                               (2) In addition, applications MAY desire verification of receipt &/or
*                                   acknowledgement of transmitted data to the remote host -- either 
*                                   inherently by the transport layer or explicitly by the application.
*
*                       (2) (A) "Otherwise, -1 shall be returned" ...
*                               (1) Section 'sendto() : DESCRIPTION' elaborates that "a return value of 
*                                   -1 indicates only locally-detected errors".
*
*                           (B) "and 'errno' set to indicate the error."
*                               #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   (b) ???? Although NO socket send() specification states to return '0' when the socket's
*                       connection is closed, it seems reasonable to return '0' since it is possible for the
*                       socket connection to be close()'d or shutdown() by the remote host.
*
*                   See also 'NetSock_TxDataHandler()  Note #5'.
**********************************************************************************************************
*/
/*$PAGE*/
NET_SOCK_RTN_CODE  NetSock_TxDataTo (NET_SOCK_ID         sock_id,
                                     void               *p_data,
                                     CPU_INT16U          data_len,
                                     CPU_INT16S          flags,
                                     NET_SOCK_ADDR      *paddr_remote,
                                     NET_SOCK_ADDR_LEN   addr_len,
                                     NET_ERR            *perr)
{
    NET_SOCK_RTN_CODE  rtn_code;

                                                                /* --------------- VALIDATE/TX APP DATA --------------- */
    rtn_code = NetSock_TxDataHandler((NET_SOCK_ID        )sock_id,
                                     (void              *)p_data,
                                     (CPU_INT16U         )data_len,
                                     (CPU_INT16S         )flags,
                                     (NET_SOCK_ADDR     *)paddr_remote,
                                     (NET_SOCK_ADDR_LEN  )addr_len,
                                     (NET_ERR           *)perr);

    return (rtn_code);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetSock_TxData()
*
* Description : (1) Transmit data through a socket :
*
*                   (a) Validate & transmit application data
*
*                   See also 'NetSock_TxDataHandler()  Note #1'.
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to transmit data.
*
*               p_data          Pointer to application data to transmit.
*
*               data_len        Length of  application data to transmit (in octets) [see Note #3].
*
*               flags           Flags to select transmit options (see Note #4); bit-field flags logically OR'd :
*
*                                   NET_SOCK_FLAG_NONE              No socket flags selected.
*                                   NET_SOCK_FLAG_TX_NO_BLOCK       Transmit socket data without blocking.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                                   ----- RETURNED BY NetSock_TxDataHandler() : -------
*                               NET_SOCK_ERR_NONE                   Socket data successfully transmitted &/or prepared 
*                                                                       for transmission; check return value for number 
*                                                                       of data octets transmitted (see Note #5a1).
*
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_NULL_PTR               Argument 'p_data' passed a NULL pointer.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol/address family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_FLAG           Invalid socket flags.
*                               NET_SOCK_ERR_INVALID_DATA_SIZE      Invalid data size (see Notes #3b & #3a1B2).
*
*                               NET_SOCK_ERR_INVALID_CONN           Invalid socket connection.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address structure length.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid socket address.
*                               NET_SOCK_ERR_INVALID_PORT_NBR       Invalid socket port number.
*                               NET_SOCK_ERR_PORT_NBR_NONE_AVAIL    Port number NOT available.
*                               NET_SOCK_ERR_ADDR_IN_USE            Socket address already in use.
*
*                               NET_SOCK_ERR_TX_Q_CLOSED            Socket transmit queue closed.
*
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host address.
*
*                               NET_ERR_TX                          Transitory transmit error.
*
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_TYPE           Invalid network connection type.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_NONE_AVAIL             NO available connections to allocate.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*
* Return(s)   : Number of positive data octets transmitted, if NO errors                (see Note #5a1).
*
*               NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,          if socket connection closed (see Note #5b).
*
*               NET_SOCK_BSD_ERR_TX,                        otherwise                   (see Note #5a2A).
*
* Caller(s)   : send().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*$PAGE*
* Note(s)     : (2) NetSock_TxData() blocked until network initialization completes.
*
*                   See 'NetSock_IsUsed()  Note #1'.
*
*               (3) (a) (1) (A) Datagram-type sockets transmit & receive all data atomically -- i.e. every
*                               single, complete datagram transmitted MUST be received as a single, complete
*                               datagram.  Thus, each call to transmit data MUST be transmitted in a single, 
*                               complete datagram.
*
*                           (B) (1) IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states
*                                   that "if the message is too long to pass through the underlying protocol,
*                                   send() shall fail and no data shall be transmitted".
*
*                               (2) Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h 
*                                   Note #1e'), if the socket's type is datagram & the requested transmit 
*                                   data length is greater than the socket/transport layer MTU, then NO data 
*                                   is transmitted & NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
*
*                       (2) (A) (1) Stream-type sockets transmit & receive all data octets in one or more 
*                                   non-distinct packets.  In other words, the application data is NOT 
*                                   bounded by any specific packet(s); rather, it is contiguous & sequenced 
*                                   from one packet to the next.
*
*                               (2) Thus, if the socket's type is stream & the socket's transmit data queue(s)
*                                   are NOT large enough for the transmitted data, the  transmit data queue(s) 
*                                   are maximally filled with transmit data & the remaining data octets are
*                                   discarded but may be re-transmitted by later application-socket transmits.
*
*                               (3) Therefore, NO stream-type socket transmit data length should be "too long
*                                   to pass through the underlying protocol" & cause the socket transmit to 
*                                   "fail ... [with] no data ... transmitted" (see Note #3a1B1).
*
*                           (B) Thus, it is typical -- but NOT absolutely required -- that a single application
*                               task ONLY transmit or request to transmit data to a stream-type socket.
*
*                   (b) 'data_len' of 0 octets NOT allowed.
*
*                   See also 'NetSock_TxDataHandler()  Note #2'.
*
*               (4) #### Only some socket transmit flag options are implemented.  If other flag options are 
*                   requested, NetSock_TxData() handler function(s) abort & return appropriate error codes
*                   so that requested flag options are NOT silently ignored.
*
*               (5) (a) IEEE Std 1003.1, 2004 Edition, Section 'send() : RETURN VALUE' states that :
*
*                       (1) "Upon successful completion, send() shall return the number of bytes sent."
*
*                           (A) Section 'send() : DESCRIPTION' elaborates that "successful completion 
*                               of a call to sendto() does not guarantee delivery of the message".
*
*                           (B) (1) Thus, applications SHOULD verify the actual returned number of data 
*                                   octets transmitted &/or prepared for transmission.
*
*                               (2) In addition, applications MAY desire verification of receipt &/or
*                                   acknowledgement of transmitted data to the remote host -- either 
*                                   inherently by the transport layer or explicitly by the application.
*
*                       (2) (A) "Otherwise, -1 shall be returned" ...
*                               (1) Section 'send() : DESCRIPTION' elaborates that "a return value of 
*                                   -1 indicates only locally-detected errors".
*
*                           (B) "and 'errno' set to indicate the error."
*                               #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   (b) ???? Although NO socket send() specification states to return '0' when the socket's
*                       connection is closed, it seems reasonable to return '0' since it is possible for the
*                       socket connection to be close()'d or shutdown() by the remote host.
*
*                   See also 'NetSock_TxDataHandler()  Note #5'.
**********************************************************************************************************
*/
/*$PAGE*/
NET_SOCK_RTN_CODE  NetSock_TxData (NET_SOCK_ID   sock_id,
                                   void         *p_data,
                                   CPU_INT16U    data_len,
                                   CPU_INT16S    flags,
                                   NET_ERR      *perr)
{
    NET_SOCK_RTN_CODE  rtn_code;

                                                                /* --------------- VALIDATE/TX APP DATA --------------- */
    rtn_code = NetSock_TxDataHandler((NET_SOCK_ID        )sock_id,
                                     (void              *)p_data,
                                     (CPU_INT16U         )data_len,
                                     (CPU_INT16S         )flags,
                                     (NET_SOCK_ADDR     *)0,
                                     (NET_SOCK_ADDR_LEN  )0,
                                     (NET_ERR           *)perr);

    return (rtn_code);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetSock_Sel()
*
* Description : (1) Check multiple sockets for available resources &/or operations :
*
*                   (a) Acquire network lock
*                   (b) Check sockets for available resources &/or operations :
*                       (1) Validate socket descriptors' sockets
*                       (2) Check    sockets for available resources &/or operations        See Note #3b2
*                       (3) Prepare  sockets for wait on appropriate socket events
*
*                   (c) Handle timeout :                                                    See Note #3b3
*                       (1) Prepare timeout
*                       (2) Handle  time delay, if any
*
*                   (d) Wait on socket descriptor events :
*                       (1) Demultiplex ready socket event(s) to appropriate socket descriptor(s)
*
*                   (e) Release network lock
*
*
* Argument(s) : sock_nbr_max        Maximum number of socket descriptors/handle identifiers in the socket
*                                       descriptor sets (see Note #3b1).
*
*               psock_desc_rd       Pointer to a set of socket descriptors/handle identifiers to :
*
*                                       (a) Check for available read  operation(s) [see Note #3b2A1].
*
*                                       (b) (1) Return the actual socket descriptors/handle identifiers
*                                                   ready for available read  operation(s), if NO errors
*                                                   (see Note #3b2B1a1);
*                                           (2) Return the initial, non-modified set of socket descriptors/
*                                                   handle identifiers, on any errors (see Note #3c2A);
*                                           (3) Return a null-valued (i.e. zero-cleared) descriptor set,
*                                                   if any timeout expires (see Note #3c2B).
*
*               psock_desc_wr       Pointer to a set of socket descriptors/handle identifiers to :
*
*                                       (a) Check for available write operation(s) [see Note #3b2A2].
*
*                                       (b) (1) Return the actual socket descriptors/handle identifiers
*                                                   ready for available write operation(s), if NO errors
*                                                   (see Note #3b2B1a1);
*                                           (2) Return the initial, non-modified set of socket descriptors/
*                                                   handle identifiers, on any errors (see Note #3c2A);
*                                           (3) Return a null-valued (i.e. zero-cleared) descriptor set,
*                                                   if any timeout expires (see Note #3c2B).
*
*               psock_desc_err      Pointer to a set of socket descriptors/handle identifiers to :
*
*                                       (a) Check for any error(s) &/or exception(s) [see Note #3b2A3].
*
*                                       (b) (1) Return the actual socket descriptors/handle identifiers
*                                                   flagged with any error(s) &/or exception(s), if NO 
*                                                   non-descriptor-related errors (see Note #3b2B1a1);
*                                           (2) Return the initial, non-modified set of socket descriptors/
*                                                   handle identifiers, on any errors (see Note #3c2A);
*                                           (3) Return a null-valued (i.e. zero-cleared) descriptor set,
*                                                   if any timeout expires (see Note #3c2B).
*
*               ptimeout            Pointer to a timeout (see Note #3b3).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket descriptors successfully checked for 
*                                                                   available resources &/or operations; 
*                                                                   check return value for the number of 
*                                                                   ready socket descriptors with available 
*                                                                   resources &/or operations.
*
*                               NET_SOCK_ERR_TIMEOUT            Socket descriptors NOT ready by timeout 
*                                                                   (see Note #3b3A1a2).
*
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_DESC       Invalid socket descriptor/number.
*                               NET_SOCK_ERR_INVALID_TIMEOUT    Invalid timeout.
*                               
*                                                               - RETURNED BY NetSock_SelDescHandlerRd() : --
*                                                               - RETURNED BY NetSock_SelDescHandlerWr() : --
*                                                               - RETURNED BY NetSock_SelDescHandlerErr() : -
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*                               NET_SOCK_ERR_EVENTS_NBR_MAX     Number of configured socket events is greater
*                                                                   than the maximum number of socket events
*                                                                   (see Note #4b).
*
*                                                               -------- RETURNED BY NetOS_Lock() : ---------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*$PAGE*
* Return(s)   : Number of socket descriptors with available resources &/or operations, if any     (see Note #3c1A1).
*
*               NET_SOCK_BSD_RTN_CODE_TIMEOUT,                                         on timeout (see Note #3c1B).
*
*               NET_SOCK_BSD_ERR_SEL,                                                  otherwise  (see Note #3c1A2a).
*
* Caller(s)   : select().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_Sel() blocked until network initialization completes.
*
*               (3) (a) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that :
*
*                       (1) (A) "select() ... shall support" the following file descriptor types :
*
*                               (1) "regular files,"                        ...
*                               (2) "terminal and pseudo-terminal devices," ...
*                               (3) "STREAMS-based files,"                  ...
*                               (4) "FIFOs,"                                ...
*                               (5) "pipes,"                                ...
*                               (6) "sockets."
*                   
*                           (B) "The behavior of ... select() on ... other types of ... file descriptors
*                                ... is unspecified."
*
*                       (2) Network Socket Layer supports BSD 4.x select() functionality with the following 
*                           restrictions/constraints :
*
*                           (A) ONLY supports the following file descriptor types :
*                               (1) Sockets
*
*
*                   (b) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that :
*
*                       (1) (A) "The 'nfds' argument ('sock_nbr_max') specifies the range of descriptors to be 
*                                tested.  The first 'nfds' descriptors shall be checked in each set; that is,
*                                the [following] descriptors ... in the descriptor sets shall be examined" :
*
*                               (1) "from zero" ...
*                               (2) "through nfds-1."
*
*                           (B) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                               6th Printing, Section 6.3, Page 163 states that :
*
*                               (1) ... since "descriptors start at 0" ...
*
*                               (2) "the ['nfds'] argument" specifies :
*                                   (a) "the number of descriptors," ...
*                                   (b) "not the largest value."
*$PAGE*
*                       (2) "The select() function shall ... examine the file descriptor sets whose addresses 
*                            are passed in the 'readfds' ('psock_desc_rd'), 'writefds' ('psock_desc_wr'), and 
*                            'errorfds' ('psock_desc_err') parameters to see whether some of their descriptors 
*                            are ready for reading, are ready for writing, or have an exceptional condition 
*                            pending, respectively."
*
*                           (A) (1) (a) "If the 'readfds'  argument ('psock_desc_rd')  is not a null pointer, 
*                                        it points to an object of type 'fd_set' ('NET_SOCK_DESC') that on
*                                        input specifies the file descriptors to be checked for being ready 
*                                        to read,  and on output indicates which file descriptors are ready 
*                                        to read."
*
*                                   (b) "A descriptor shall be considered ready for reading when a call to 
*                                        an input function ... would not block, whether or not the function 
*                                        would transfer data successfully.  (The function might return data, 
*                                        an end-of-file indication, or an error other than one indicating 
*                                        that it is blocked, and in each of these cases the descriptor shall 
*                                        be considered ready for reading.)" :
*
*                                       (1) "If the socket is currently listening, then it shall be marked 
*                                            as readable if an incoming connection request has been received, 
*                                            and a call to the accept() function shall complete without 
*                                            blocking."
*
*                                   (c) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                                       6th Printing, Section 6.3, Pages 164-165 states that "a socket is ready 
*                                       for reading if any of the following ... conditions is true" :
*
*                                       (1) "A read operation on the socket will not block and will return a 
*                                            value greater than 0 (i.e., the data that is ready to be read)."
*                                            
*                                       (2) "The read half of the connection is closed (i.e., a TCP connection 
*                                            that has received a FIN).  A read operation ... will not block and 
*                                            will return 0 (i.e., EOF)."
*
*                                       (3) "The socket is a listening socket and the number of completed 
*                                            connections is nonzero.  An accept() on the listening socket 
*                                            will ... not block."
*
*                                       (4) "A socket error is pending.  A read operation on the socket will 
*                                            not block and will return an error (-1) with 'errno' set to the 
*                                            specific error condition."
*
*                                           (A) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 
*                                               3rd Edition, 6th Printing, Section 6.3, Page 165 states "that
*                                               when an error occurs on a socket, it is [also] marked as both
*                                               readable and writeable by select()".
*
*                                               See also Notes #3b2A2c4A & #3b2A3d.
*
*                               (2) (a) "If the 'writefds' argument ('psock_desc_wr')  is not a null pointer, 
*                                        it points to an object of type 'fd_set' ('NET_SOCK_DESC') that on 
*                                        input specifies the file descriptors to be checked for being ready 
*                                        to write, and on output indicates which file descriptors are ready 
*                                        to write."
*
*                                   (b) "A descriptor shall be considered ready for writing when a call to 
*                                        an output function ... would not block, whether or not the function 
*                                        would transfer data successfully" :
*
*                                       (1) "If a non-blocking call to the connect() function has been made 
*                                            for a socket, and the connection attempt has either succeeded 
*                                            or failed leaving a pending error, the socket shall be marked 
*                                            as writable."
*
*                                   (c) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                                       6th Printing, Section 6.3, Page 165 states that "a socket is ready for 
*                                       writing if any of the following ... conditions is true" :
*
*                                       (1) "A write operation will not block and will return a positive value 
*                                            (e.g., the number of bytes accepted by the transport layer)."
*
*                                       (2) "The write half of the connection is closed."
*
*                                       (3) "A socket using a non-blocking connect() has completed the
*                                            connection, or the connect() has failed."
*
*                                       (4) "A socket error is pending.  A write operation on the socket will 
*                                            not block and will return an error (-1) with 'errno' set to the 
*                                            specific error condition."
*
*                                           (A) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 
*                                               3rd Edition, 6th Printing, Section 6.3, Page 165 states "that
*                                               when an error occurs on a socket, it is [also] marked as both
*                                               readable and writeable by select()".
*
*                                               See also Notes #3b2A1c4A & #3b2A3d.
*$PAGE*
*                               (3) (a) "If the 'errorfds' argument ('psock_desc_err') is not a null pointer, 
*                                        it points to an object of type 'fd_set' ('NET_SOCK_DESC') that on 
*                                        input specifies the file descriptors to be checked for error 
*                                        conditions pending, and on output indicates which file descriptors 
*                                        have error conditions pending."
*
*                                   (b) "A file descriptor ... shall be considered to have an exceptional 
*                                        condition pending ... as noted below" :
*
*                                       (1) (A) "A socket ... receive operation ... [that] would return 
*                                                out-of-band data without blocking."
*                                           (B) "A socket ... [with] out-of-band data ... present in the 
*                                                receive queue."
*
*                                           #### Out-of-band data NOT supported (see 'net_tcp.h  Note #1b').
*
*                                       (2) "If a socket has a pending error."
*
*                                       (3) "Other circumstances under which a socket may be considered to 
*                                            have an exceptional condition pending are protocol-specific 
*                                            and implementation-defined."
*
*                                   (c) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                                       6th Printing, Section 6.3, Page 165 states that "a socket has an 
*                                       exception condition pending if ... any of the following ... conditions 
*                                       is true" :
*
*                                       (1) (A) "Out-of-band data for the socket" is currently available; ...
*                                           (B) "Or the socket is still at the out-of-band mark."
*
*                                           #### Out-of-band data NOT supported (see 'net_tcp.h  Note #1b').
*
*                                   (d) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                                       6th Printing, Section 6.3, Page 165 states "that when an error occurs on
*                                       a socket, it is [also] marked as both readable and writeable by select()".
*
*                                       See also Notes #3b2A1c4A & #3b2A2c4A.
*
*                           (B) (1) (a) "Upon successful completion, ... select() ... shall" :
*                           
*                                       (1) "modify the objects pointed to by the 'readfds' ('psock_desc_rd'), 
*                                            'writefds' ('psock_desc_wr'), and 'errorfds' ('psock_desc_err') 
*                                            arguments to indicate which file descriptors are ready for 
*                                            reading, ready for writing, or have an error condition pending,
*                                            respectively," ...
*
*                                       (2) "and shall return the total number of ready descriptors in all
*                                            the output sets."
*
*                                            See also Note #3c1A1.
*
*                                   (b) (1) "For each file descriptor less than nfds ('sock_nbr_max'), the
*                                            corresponding bit shall be set on successful completion" :
*
*                                           (A) "if it was set on input" ...
*                                           (B) "and the associated condition is true for that file descriptor."
*
*                                       (2) Conversely, "for each file descriptor ... the corresponding bit
*                                           shall be ... [clear] ... on ... completion" :
*
*                                           (A) If it was clear on input;                                         ...
*                                           (B) If the associated condition is NOT true for that file descriptor; ...
*                                           (C) Or it was set on input, but any timeout occurs (see Note #3c2B).
*
*                               (2) select() can NOT absolutely guarantee that descriptors returned as ready 
*                                   will still be ready during subsequent operations since any higher priority 
*                                   tasks or processes may asynchronously consume the descriptors' operations 
*                                   &/or resources.  This can occur since select() functionality & subsequent 
*                                   operations are NOT atomic operations protected by network, file system, 
*                                   or operating system mechanisms.
*
*                                   However, as long as no higher priority tasks or processes access any of 
*                                   the same descriptors, then a single task or process can assume that all 
*                                   descriptors returned as ready by select() will still be ready during 
*                                   subsequent operations.
*$PAGE*
*                       (3) (A) "The 'timeout' parameter ('ptimeout') controls how long ... select() ... shall
*                                take before timing out."
*                                
*                               (1) (a) "If the 'timeout' parameter ('ptimeout') is not a null pointer, it 
*                                        specifies a maximum interval to wait for the selection to complete."
*
*                                       (1) "If none of the selected descriptors are ready for the requested 
*                                            operation, ... select() ... shall block until at least one of the 
*                                            requested operations becomes ready ... or ... until the timeout 
*                                            occurs."
*
*                                       (2) "If the specified time interval expires without any requested 
*                                            operation becoming ready, the function shall return."
*
*                                       (3) "To effect a poll, the 'timeout' parameter ('ptimeout') should not be
*                                            a null pointer, and should point to a zero-valued timespec structure
*                                            ('NET_SOCK_TIMEOUT')."
*
*                                   (b) (1) (A) "If the 'readfds' ('psock_desc_rd'), 'writefds' ('psock_desc_wr'), 
*                                                and 'errorfds' ('psock_desc_err') arguments are"                ...
*                                               (1) "all null pointers"                                          ...
*                                               (2) [or all null-valued (i.e. no file descriptors set)]          ...
*                                           (B) "and the 'timeout' argument ('ptimeout') is not a null pointer," ...
*
*                                       (2) ... then "select() ... shall block for the time specified".
*
*                               (2) "If the 'timeout' parameter ('ptimeout') is a null pointer, then the call to 
*                                    ... select() shall block indefinitely until at least one descriptor meets the
*                                    specified criteria."
*
*                                   (a) (1) Although IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' 
*                                           states that select() may "block indefinitely until ... one of the 
*                                           requested operations becomes ready ... or until interrupted by a signal", 
*                                           it does NOT explicity specify how to handle the case where the descriptor 
*                                           arguments & the timeout parameter argument are all NULL pointers.
*
*                                           Therefore, it seems reasonable that select() should "block indefinitely 
*                                           ... until interrupted by a signal" if the descriptor arguments & the 
*                                           timeout parameter argument are all NULL pointers.
*
*                                       (2) However, since inter-process signals are NOT currently supported, it 
*                                           does NOT seem reasonable to block a task or process indefinitely (i.e. 
*                                           forever) if the descriptor arguments & the timeout parameter argument 
*                                           are all NULL pointers.  Instead, an 'invalid timeout interval' error 
*                                           should be returned.
*
*                                           See also Note #3d2B.
*
*                           (B) (1) "For the select() function, the timeout period is given ... in an argument
*                                   ('ptimeout') of type struct 'timeval' ('NET_SOCK_TIMEOUT')" ... :
*
*                                   (a) "in seconds" ...
*                                   (b) "and microseconds."
*
*                               (2) (a) (1) "Implementations may place limitations on the maximum timeout interval 
*                                            supported" :
*
*                                           (A) "All implementations shall support a maximum timeout interval of 
*                                                at least 31 days."
*
*                                               (1) However, since maximum timeout interval values are dependent
*                                                   on the specific OS implementation; a maximum timeout interval
*                                                   can NOT be guaranteed.
*
*                                           (B) "If the 'timeout' argument ('ptimeout') specifies a timeout interval 
*                                                greater than the implementation-defined maximum value, the maximum 
*                                                value shall be used as the actual timeout value."
*
*                                       (2) "Implementations may also place limitations on the granularity of 
*                                            timeout intervals" :
*
*                                           (A) "If the requested 'timeout' interval requires a finer granularity 
*                                                than the implementation supports, the actual timeout interval 
*                                                shall be rounded up to the next supported value."
*
*                                   (b) Operating systems may have different minimum/maximum ranges & resolutions 
*                                       for timeouts while pending or waiting on an operating system resource to 
*                                       become available (see Note #3b3A1a) versus time delays for suspending a 
*                                       task or process that is NOT pending or waiting for an operating system 
*                                       resource (see Note #3b3A1b).
*$PAGE*
*                   (c) (1) (A) IEEE Std 1003.1, 2004 Edition, Section 'select() : RETURN VALUE' states that :
*
*                               (1) "Upon successful completion, ... select() ... shall return the total 
*                                    number of bits set in the bit masks."
*
*                               (2) (a) "Otherwise, -1 shall be returned," ...
*                                   (b) "and 'errno' shall be set to indicate the error."
*                                       #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                           (B) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition,
*                               6th Printing, Section 6.3, Page 161 states that BSD select() function
*                               "returns ... 0 on timeout".
*
*                       (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that :
*
*                           (A) "On failure, the objects pointed to by the 'readfds' ('psock_desc_rd'), 
*                                'writefds' ('psock_desc_wr'), and 'errorfds' ('psock_desc_err') arguments 
*                                shall not be modified."
*
*                               (1) Since 'psock_desc_rd', 'psock_desc_wr', & 'psock_desc_err' arguments
*                                   are both input & output parameters; their input values, prior to use, 
*                                   MUST be copied to return their initial input values in case of any 
*                                   function exit due to error/fault conditions.
*
*                           (B) "If the 'timeout' interval expires without the specified condition being 
*                                true for any of the specified file descriptors, the objects pointed to 
*                                by the 'readfds' ('psock_desc_rd'), 'writefds' ('psock_desc_wr'), and 
*                                'errorfds' ('psock_desc_err') arguments shall have all bits set to 0."
*
*                   (d) IEEE Std 1003.1, 2004 Edition, Section 'select() : ERRORS' states that "under the
*                       following conditions, ... select() shall fail and set 'errno' to" :
*
*                       (1) "[EBADF] - One or more of the file descriptor sets specified a file descriptor 
*                            that is not a valid open file descriptor."
*
*                       (2) "[EINVAL]" -
*
*                           (A) "The 'nfds' argument ('sock_nbr_max') is" :
*                               (1) "less than 0 or" ...
*                               (2) "greater than FD_SETSIZE."
*
*                               See also Note #3b1.
*
*                           (B) "An invalid timeout interval was specified."
*
*                               See also Note #3b3.
*
*                       #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*               (4) A socket events table lists requested socket or connection ID numbers/handle identifiers
*                   & their respective socket event operations.
*
*                   (a) Socket event tables are terminated with NULL table entry values.
*
*                   (b) (1) NET_SOCK_CFG_SEL_NBR_EVENTS_MAX configures socket event tables' maximum number 
*                           of socket events/operations.
*
*                       (2) This value is used to declare the size of the socket events tables.  Note that 
*                           one additional table entry is added for a terminating NULL table entry at a 
*                           maximum table index 'NET_SOCK_CFG_SEL_NBR_EVENTS_MAX' (see Note #4a).
*
*               (5) Since datagram-type sockets typically never wait on transmit operations, no socket 
*                   event should wait on datagram-type socket transmit operations or transmit errors.
*
*                   See also 'NetSock_IsRdyTxDatagram()            Note #3'
*                          & 'NetSock_SelDescHandlerErrDatagram()  Note #4'.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
NET_SOCK_RTN_CODE  NetSock_Sel (NET_SOCK_QTY       sock_nbr_max,
                                NET_SOCK_DESC     *psock_desc_rd,
                                NET_SOCK_DESC     *psock_desc_wr,
                                NET_SOCK_DESC     *psock_desc_err,
                                NET_SOCK_TIMEOUT  *ptimeout,
                                NET_ERR           *perr)
{
    NET_SOCK_QTY         sock_nbr_max_actual;
    NET_SOCK_QTY         sock_nbr_rdy;
    NET_SOCK_DESC        sock_desc_rtn_rd;
    NET_SOCK_DESC        sock_desc_rtn_wr;
    NET_SOCK_DESC        sock_desc_rtn_err;
    NET_SOCK_EVENT_QTY   sock_events_nbr_rdy;
    NET_SOCK_EVENT_QTY   sock_events_nbr_rem;
    NET_SOCK_EVENT_QTY   sock_events_nbr_cfgd;
    NET_SOCK_EVENT_TBL   sock_events_cfg[NET_SOCK_CFG_SEL_NBR_EVENTS_MAX + 1];  /* See Note #4.                         */
    NET_SOCK_EVENT_TBL   sock_events_rtn[NET_SOCK_CFG_SEL_NBR_EVENTS_MAX + 1];
    NET_SOCK_EVENT_TBL  *psock_events_cfg;
    NET_SOCK_EVENT_TBL  *psock_events_rtn;
    NET_SOCK_ID          sock_id;
    CPU_BOOLEAN          sock_rdy;
    CPU_BOOLEAN          sock_reqd;
    CPU_BOOLEAN          sock_desc_used;
    CPU_INT32U           timeout_sec;
    CPU_INT32U           timeout_us;
    NET_ERR              err;



#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
                                                                    /* ------------ VALIDATE MAX SOCK DESC ------------ */
#if (NET_SOCK_DESC_NBR_MIN_DESC > DEF_INT_16U_MIN_VAL)
    if (sock_nbr_max < NET_SOCK_DESC_NBR_MIN_DESC) {                /* If max nbr sock desc < min, ...                  */
       *perr =  NET_SOCK_ERR_INVALID_DESC;                          /* ... rtn err (see Note #3d2A1).                   */
        return (NET_SOCK_BSD_ERR_SEL);
    }
#endif
    if (sock_nbr_max > NET_SOCK_DESC_NBR_MAX_DESC) {                /* If max nbr sock desc > max (see Note #3b1B2a),   */
       *perr =  NET_SOCK_ERR_INVALID_DESC;                          /* ... rtn err (see Note #3d2A2).                   */
        return (NET_SOCK_BSD_ERR_SEL);
    }

                                                                    /* --------------- VALIDATE TIMEOUT --------------- */
    if (ptimeout != (NET_SOCK_TIMEOUT *)0) {
        if (ptimeout->timeout_sec < 0) {
           *perr =  NET_SOCK_ERR_INVALID_TIMEOUT;
            return (NET_SOCK_BSD_ERR_SEL);
        }
        if (ptimeout->timeout_us  < 0) {
           *perr =  NET_SOCK_ERR_INVALID_TIMEOUT;
            return (NET_SOCK_BSD_ERR_SEL);
        }
    }
#endif


                                                                    /* --------------- ACQUIRE NET LOCK --------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_SEL);
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                                  /* If init NOT complete, exit (see Note #2).        */
        NetOS_Unlock();
       *perr =  NET_ERR_INIT_INCOMPLETE;
        return (NET_SOCK_BSD_ERR_SEL);
    }
#endif



/*$PAGE*/
                                                                    /* -- VALIDATE / CHK / CFG SOCK DESC(S) FOR RDY --- */
    sock_nbr_max_actual  =  DEF_MIN(sock_nbr_max, NET_SOCK_DESC_NBR_MAX_DESC);
    sock_nbr_rdy         =  0;
    sock_events_nbr_cfgd =  0;
    psock_events_cfg     = &sock_events_cfg[0];

    if (sock_nbr_max_actual > 0) {                                  /* If max nbr desc's > 0, chk ALL sock desc's.      */

        if (psock_desc_rd != (NET_SOCK_DESC *)0) {                  /* If avail, chk rd sock desc's (see Note #3b2A1).  */
                                                                    /* Copy    req'd rd sock desc's (see Note #3c2A1).  */
            NET_SOCK_DESC_COPY(&sock_desc_rtn_rd, psock_desc_rd);
                                                                    /* Chk ALL avail rd sock desc's (see Note #3b1).    */
            for (sock_id = 0; sock_id < sock_nbr_max_actual; sock_id++) {
                sock_desc_used = NET_SOCK_DESC_IS_SET(sock_id, psock_desc_rd);
                if (sock_desc_used != 0) {                          /* If rd sock desc req'd, ...                       */
                                                                    /* ... chk/cfg for rdy rd op(s) [see Note #3b2A1].  */
                    sock_rdy = NetSock_SelDescHandlerRd((NET_SOCK_ID          ) sock_id,
                                                        (NET_SOCK_EVENT_TBL **)&psock_events_cfg,
                                                        (NET_SOCK_EVENT_QTY  *)&sock_events_nbr_cfgd,
                                                        (NET_ERR             *) perr);
                    switch (*perr) {
                        case NET_SOCK_ERR_NONE:
                        case NET_SOCK_ERR_CLOSED:
                        case NET_SOCK_ERR_FAULT:
                        case NET_SOCK_ERR_CONN_FAIL:
                        case NET_SOCK_ERR_INVALID_STATE:
                        case NET_SOCK_ERR_INVALID_OP:
                             if (sock_rdy != DEF_YES) {             /* If sock NOT rdy, ...                             */
                                                                    /* ... clr from rtn rd sock desc's (see #3b2B1b2B). */
                                 NET_SOCK_DESC_CLR(sock_id, &sock_desc_rtn_rd);
                             } else {                               /* Else inc nbr rdy    sock desc's (see #3b2B1a2).  */
                                 sock_nbr_rdy++;
                             }
                             break;


                        case NET_ERR_INIT_INCOMPLETE:
                        case NET_SOCK_ERR_INVALID_TYPE:             /* See Notes #3a2A1 & #3d1.                         */
                        case NET_SOCK_ERR_INVALID_SOCK:
                        case NET_SOCK_ERR_NOT_USED:
                        case NET_SOCK_ERR_EVENTS_NBR_MAX:
                        default:
                             NetOS_Unlock();
                             return (NET_SOCK_BSD_ERR_SEL);         /* Prevent 'break NOT reachable' compiler warning.  */
                    }
                }
            }

        } else {                                                    /* Else if NO  rd sock desc's avail, ...            */
            NET_SOCK_DESC_INIT(&sock_desc_rtn_rd);                  /* ... clr rtn rd sock desc's.                      */
        }

/*$PAGE*/
        if (psock_desc_wr != (NET_SOCK_DESC *)0) {                  /* If avail, chk wr sock desc's (see Note #3b2A2).  */
                                                                    /* Copy    req'd wr sock desc's (see Note #3c2A1).  */
            NET_SOCK_DESC_COPY(&sock_desc_rtn_wr, psock_desc_wr);
                                                                    /* Chk ALL avail wr sock desc's (see Note #3b1).    */
            for (sock_id = 0; sock_id < sock_nbr_max_actual; sock_id++) {
                sock_desc_used = NET_SOCK_DESC_IS_SET(sock_id, psock_desc_wr);
                if (sock_desc_used != 0) {                          /* If wr sock desc req'd, ...                       */
                                                                    /* ... chk/cfg for rdy wr op(s) [see Note #3b2A2].  */
                    sock_rdy = NetSock_SelDescHandlerWr((NET_SOCK_ID          ) sock_id,
                                                        (NET_SOCK_EVENT_TBL **)&psock_events_cfg,
                                                        (NET_SOCK_EVENT_QTY  *)&sock_events_nbr_cfgd,
                                                        (NET_ERR             *) perr);
                    switch (*perr) {
                        case NET_SOCK_ERR_NONE:
                        case NET_SOCK_ERR_CLOSED:
                        case NET_SOCK_ERR_FAULT:
                        case NET_SOCK_ERR_CONN_FAIL:
                        case NET_SOCK_ERR_INVALID_STATE:
                        case NET_SOCK_ERR_INVALID_OP:
                             if (sock_rdy != DEF_YES) {             /* If sock NOT rdy, ...                             */
                                                                    /* ... clr from rtn wr sock desc's (see #3b2B1b2B). */
                                 NET_SOCK_DESC_CLR(sock_id, &sock_desc_rtn_wr);
                             } else {                               /* Else inc nbr rdy    sock desc's (see #3b2B1a2).  */
                                 sock_nbr_rdy++;
                             }
                             break;


                        case NET_ERR_INIT_INCOMPLETE:
                        case NET_SOCK_ERR_INVALID_TYPE:             /* See Notes #3a2A1 & #3d1.                         */
                        case NET_SOCK_ERR_INVALID_SOCK:
                        case NET_SOCK_ERR_NOT_USED:
                        case NET_SOCK_ERR_EVENTS_NBR_MAX:
                        default:
                             NetOS_Unlock();
                             return (NET_SOCK_BSD_ERR_SEL);         /* Prevent 'break NOT reachable' compiler warning.  */
                    }
                }
            }

        } else {                                                    /* Else if NO  wr sock desc's avail, ...            */
            NET_SOCK_DESC_INIT(&sock_desc_rtn_wr);                  /* ... clr rtn wr sock desc's.                      */
        }

/*$PAGE*/
        if (psock_desc_err != (NET_SOCK_DESC *)0) {                 /* If avail, chk err sock desc's (see Note #3b2A3). */
                                                                    /* Copy    req'd err sock desc's (see Note #3c2A1). */
            NET_SOCK_DESC_COPY(&sock_desc_rtn_err, psock_desc_err);
                                                                    /* Chk ALL avail err sock desc's (see Note #3b1).   */
            for (sock_id = 0; sock_id < sock_nbr_max_actual; sock_id++) {
                sock_desc_used = NET_SOCK_DESC_IS_SET(sock_id, psock_desc_err);
                if (sock_desc_used != 0) {                          /* If err sock desc req'd, ...                      */
                                                                    /* ... chk/cfg for avail err(s) [see Note #3b2A3].  */
                    sock_rdy = NetSock_SelDescHandlerErr((NET_SOCK_ID          ) sock_id,
                                                         (NET_SOCK_EVENT_TBL **)&psock_events_cfg,
                                                         (NET_SOCK_EVENT_QTY  *)&sock_events_nbr_cfgd,
                                                         (NET_ERR             *) perr);
                    switch (*perr) {
                        case NET_SOCK_ERR_NONE:
                        case NET_SOCK_ERR_CLOSED:
                        case NET_SOCK_ERR_FAULT:
                        case NET_SOCK_ERR_CONN_FAIL:
                        case NET_SOCK_ERR_INVALID_STATE:
                        case NET_SOCK_ERR_INVALID_OP:
                             if (sock_rdy != DEF_YES) {             /* If sock NOT rdy, ...                             */
                                                                    /* ... clr from rtn err sock desc's (see #3b2B1b2B).*/
                                 NET_SOCK_DESC_CLR(sock_id, &sock_desc_rtn_err);
                             } else {                               /* Else inc nbr rdy     sock desc's (see #3b2B1a2). */
                                 sock_nbr_rdy++;
                             }
                             break;


                        case NET_ERR_INIT_INCOMPLETE:
                        case NET_SOCK_ERR_INVALID_TYPE:             /* See Notes #3a2A1 & #3d1.                         */
                        case NET_SOCK_ERR_INVALID_SOCK:
                        case NET_SOCK_ERR_NOT_USED:
                        case NET_SOCK_ERR_EVENTS_NBR_MAX:
                        default:
                             NetOS_Unlock();
                             return (NET_SOCK_BSD_ERR_SEL);         /* Prevent 'break NOT reachable' compiler warning.  */
                    }
                }
            }

        } else {                                                    /* Else if NO  err sock desc's avail, ...           */
            NET_SOCK_DESC_INIT(&sock_desc_rtn_err);                 /* ... clr rtn err sock desc's.                     */
        }

    }


    if (sock_nbr_rdy > 0) {                                         /* If any     sock desc's rdy,                  ..  */
        NetOS_Unlock();
                                                                    /* .. rtn rdy sock desc's (see Note #3b2B1a1) & ..  */
        NET_SOCK_DESC_COPY(psock_desc_rd,  &sock_desc_rtn_rd );
        NET_SOCK_DESC_COPY(psock_desc_wr,  &sock_desc_rtn_wr );
        NET_SOCK_DESC_COPY(psock_desc_err, &sock_desc_rtn_err);

       *perr =   NET_SOCK_ERR_NONE;
        return ((NET_SOCK_RTN_CODE)sock_nbr_rdy);                   /* .. nbr rdy sock desc's (see Note #3c1A1).        */
    }

                                                                    /* Append terminating NULL tbl vals (see Note #4a). */
    psock_events_cfg->EventType = NET_SOCK_EVENT_NONE;
    psock_events_cfg->SockID    = NET_SOCK_ID_NONE;
    psock_events_cfg->ConnID    = NET_CONN_ID_NONE;



/*$PAGE*/
                                                                    /* -------- CFG TIMEOUT / HANDLE TIME DLY --------- */
    if (ptimeout != (NET_SOCK_TIMEOUT *)0) {                        /* If avail, cfg timeout vals    (see Note #3b3A1); */
        timeout_sec = (CPU_INT32U)ptimeout->timeout_sec;
        timeout_us  = (CPU_INT32U)ptimeout->timeout_us;
    } else {                                                        /* .. else  cfg timeout to block (see Note #3b3A2). */
        timeout_sec = (CPU_INT32U)NET_TMR_TIME_INFINITE;
        timeout_us  = (CPU_INT32U)NET_TMR_TIME_INFINITE;
    }


    if ((timeout_sec == 0) && (timeout_us == 0)) {                  /* If timeout = 0, handle as non-blocking poll ..   */
                                                                    /* .. timeout (see Note #3b3A1a3).                  */
        NetOS_Unlock();
                                                                    /* Zero-clr rtn sock desc's (see Note #3c2B).       */
        NET_SOCK_DESC_INIT(psock_desc_rd );
        NET_SOCK_DESC_INIT(psock_desc_wr );
        NET_SOCK_DESC_INIT(psock_desc_err);

       *perr =  NET_SOCK_ERR_TIMEOUT;
        return (NET_SOCK_BSD_RTN_CODE_TIMEOUT);
    }


    if (sock_events_nbr_cfgd < 1) {                                 /* If NO sock events cfg'd to wait on ...           */

        NetOS_Unlock();
        if (ptimeout == (NET_SOCK_TIMEOUT *)0) {                    /* ... & NO timeout req'd,            ...           */
           *perr =  NET_SOCK_ERR_INVALID_TIMEOUT;                   /* ... rtn err (see Note #3b3A2a2).                 */
            return (NET_SOCK_BSD_ERR_SEL);
        }

        NetOS_TimeDly(timeout_sec, timeout_us, &err);               /* Else dly for timeout (see Note #3b3A1b2).        */
        switch (err) {
            case NET_OS_ERR_NONE:
            case NET_OS_ERR_TIME_DLY_MAX:
                 break;


            case NET_OS_ERR_TIME_DLY_FAULT:
            case NET_OS_ERR_INVALID_TIME:
            default:
                *perr =  NET_SOCK_ERR_INVALID_TIMEOUT;
                 return (NET_SOCK_BSD_ERR_SEL);                     /* Prevent 'break NOT reachable' compiler warning.  */
        }
                                                                    /* Zero-clr rtn sock desc's (see Note #3c2B).       */
        NET_SOCK_DESC_INIT(psock_desc_rd );
        NET_SOCK_DESC_INIT(psock_desc_wr );
        NET_SOCK_DESC_INIT(psock_desc_err);

       *perr =  NET_SOCK_ERR_TIMEOUT;
        return (NET_SOCK_BSD_RTN_CODE_TIMEOUT);
    }



/*$PAGE*/
                                                                    /* ----- WAIT ON MULTIPLE SOCK DESC'S / EVENTS ---- */
    NetOS_Unlock();
    sock_events_nbr_rdy = NetOS_Sock_SelWait((NET_SOCK_EVENT_QTY  ) sock_events_nbr_cfgd,
                                             (NET_SOCK_EVENT_TBL *)&sock_events_cfg[0],
                                             (NET_SOCK_EVENT_TBL *)&sock_events_rtn[0],
                                             (CPU_INT32U          ) timeout_sec,
                                             (CPU_INT32U          ) timeout_us,
                                             (NET_ERR            *) perr);
    NetOS_Lock(&err);
    if ( err != NET_OS_ERR_NONE) {
       *perr  = err;                                                /* Rtn err from NetOS_Lock().                       */
        return (NET_SOCK_BSD_ERR_SEL);
    }

    switch (*perr) {
        case NET_SOCK_ERR_NONE:
             if (sock_events_nbr_rdy > 0) {                         /* If any sock event(s) rdy,     ...                */
                                                                    /* Zero-clr rtn sock desc's (see Note #3c2B).       */
                 NET_SOCK_DESC_INIT(&sock_desc_rtn_rd );
                 NET_SOCK_DESC_INIT(&sock_desc_rtn_wr );
                 NET_SOCK_DESC_INIT(&sock_desc_rtn_err);

                 sock_events_nbr_rem =  sock_events_nbr_rdy;
                 psock_events_rtn    = &sock_events_rtn[0];
                                                                    /* ... srch every rdy sock event ...                */
                 while ((psock_events_rtn->EventType != NET_SOCK_EVENT_NONE) &&
                         (sock_events_nbr_rem > 0)) {

                     sock_id = psock_events_rtn->SockID;

                     switch (psock_events_rtn->EventType) {         /* ... & demux to appropriate rtn sock desc's.      */
                         case NET_SOCK_EVENT_SOCK_RX:               /* See Notes #3b2A1c1 & #3b2A1c2.                   */
                         case NET_SOCK_EVENT_TRANSPORT_RX:
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                         case NET_SOCK_EVENT_SOCK_ACCEPT:           /* See Note  #3b2A1c3.                              */
#endif
                              if (psock_desc_rd != (NET_SOCK_DESC *)0) {
                                  sock_reqd = NET_SOCK_DESC_IS_SET(sock_id, psock_desc_rd);
                                  if (sock_reqd == DEF_YES) {       /* If rd  sock desc req'd                        .. */
                                      sock_rdy = NET_SOCK_DESC_IS_SET(sock_id, &sock_desc_rtn_rd);
                                      if (sock_rdy == DEF_NO) {     /* .. & NOT already rdy'd;                       .. */
                                                                    /* ..   rtn     rdy rd  sock desc   (see #3b2B1a1)  */
                                          NET_SOCK_DESC_SET(sock_id, &sock_desc_rtn_rd);
                                          sock_nbr_rdy++;           /* .. & inc nbr rdy     sock desc's (see #3b2B1a2). */
                                      }
                                  }
                              }
                              break;


                         case NET_SOCK_EVENT_TRANSPORT_TX:          /* See Notes #3b2A2c1 & #3b2A2c2.                   */
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                         case NET_SOCK_EVENT_SOCK_CONN:             /* See Note  #3b2A2c3.                              */
#endif
                              if (psock_desc_wr != (NET_SOCK_DESC *)0) {
                                  sock_reqd = NET_SOCK_DESC_IS_SET(sock_id, psock_desc_wr);
                                  if (sock_reqd == DEF_YES) {       /* If wr  sock desc req'd                        .. */
                                      sock_rdy = NET_SOCK_DESC_IS_SET(sock_id, &sock_desc_rtn_wr);
                                      if (sock_rdy == DEF_NO) {     /* .. & NOT already rdy'd;                       .. */
                                                                    /* ..   rtn     rdy wr  sock desc   (see #3b2B1a1)  */
                                          NET_SOCK_DESC_SET(sock_id, &sock_desc_rtn_wr);
                                          sock_nbr_rdy++;           /* .. & inc nbr rdy     sock desc's (see #3b2B1a2). */
                                      }
                                  }
                              }
                              break;

/*$PAGE*/
                         case NET_SOCK_EVENT_ERR:
                         case NET_SOCK_EVENT_SOCK_ERR_RX:           /* See Note  #3b2A1c4.                              */
                         case NET_SOCK_EVENT_TRANSPORT_ERR_RX:
                         case NET_SOCK_EVENT_TRANSPORT_ERR_TX:      /* See Note  #3b2A2c4.                              */
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)                    /* See Notes #3b2A3b2 & #3b2A3b3.                   */
                         case NET_SOCK_EVENT_SOCK_ERR_ACCEPT:
                         case NET_SOCK_EVENT_SOCK_ERR_CONN:
#endif
                              if (psock_desc_err != (NET_SOCK_DESC *)0) {
                                  sock_reqd = NET_SOCK_DESC_IS_SET(sock_id, psock_desc_err);
                                  if (sock_reqd == DEF_YES) {       /* If err sock desc req'd                        .. */
                                      sock_rdy = NET_SOCK_DESC_IS_SET(sock_id, &sock_desc_rtn_err);
                                      if (sock_rdy == DEF_NO) {     /* .. & NOT already rdy'd;                       .. */
                                                                    /* ..   rtn     rdy err sock desc   (see #3b2B1a1)  */
                                          NET_SOCK_DESC_SET(sock_id, &sock_desc_rtn_err);
                                          sock_nbr_rdy++;           /* .. & inc nbr rdy     sock desc's (see #3b2B1a2). */
                                      }
                                  }
                              }
                                                                    /* Err(s)  rdy rd/wr sock desc's (see #3b2A3d).     */
                              if (psock_desc_rd != (NET_SOCK_DESC *)0) {
                                  sock_reqd = NET_SOCK_DESC_IS_SET(sock_id, psock_desc_rd);
                                  if (sock_reqd == DEF_YES) {       /* If rd  sock desc req'd                        .. */
                                      sock_rdy = NET_SOCK_DESC_IS_SET(sock_id, &sock_desc_rtn_rd);
                                      if (sock_rdy == DEF_NO) {     /* .. & NOT already rdy'd;                       .. */
                                                                    /* ..   rtn     rdy rd  sock desc   (see #3b2B1a1)  */
                                          NET_SOCK_DESC_SET(sock_id, &sock_desc_rtn_rd);
                                          sock_nbr_rdy++;           /* .. & inc nbr rdy     sock desc's (see #3b2B1a2). */
                                      }
                                  }
                              }
                              if (psock_desc_wr != (NET_SOCK_DESC *)0) {
                                  sock_reqd = NET_SOCK_DESC_IS_SET(sock_id, psock_desc_wr);
                                  if (sock_reqd == DEF_YES) {       /* If wr  sock desc req'd                        .. */
                                      sock_rdy = NET_SOCK_DESC_IS_SET(sock_id, &sock_desc_rtn_wr);
                                      if (sock_rdy == DEF_NO) {     /* .. & NOT already rdy'd;                       .. */
                                                                    /* ..   rtn     rdy wr  sock desc   (see #3b2B1a1)  */
                                          NET_SOCK_DESC_SET(sock_id, &sock_desc_rtn_wr);
                                          sock_nbr_rdy++;           /* .. & inc nbr rdy     sock desc's (see #3b2B1a2). */
                                      }
                                  }
                              }
                              break;


                         case NET_SOCK_EVENT_NONE:
                         case NET_SOCK_EVENT_SOCK_TX:               /* See Note #5.                                     */
                         case NET_SOCK_EVENT_SOCK_ERR_TX:
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                         case NET_SOCK_EVENT_SOCK_CLOSE:
                         case NET_SOCK_EVENT_SOCK_ERR_CLOSE:
#endif
                         default:
                              break;
                     }

                     psock_events_rtn++;
                     sock_events_nbr_rem--;
                 }

                                                                    /* Rtn rdy sock desc's (see Note #3b2B1a1).         */
                 NET_SOCK_DESC_COPY(psock_desc_rd,  &sock_desc_rtn_rd );
                 NET_SOCK_DESC_COPY(psock_desc_wr,  &sock_desc_rtn_wr );
                 NET_SOCK_DESC_COPY(psock_desc_err, &sock_desc_rtn_err);

             } else {                                               /* Else NO sock event(s) rdy, handle as timeout.    */
                 NetOS_Unlock();
                                                                    /* Zero-clr rtn sock desc's (see Note #3c2B).       */
                 NET_SOCK_DESC_INIT(psock_desc_rd );
                 NET_SOCK_DESC_INIT(psock_desc_wr );
                 NET_SOCK_DESC_INIT(psock_desc_err);

                *perr =  NET_SOCK_ERR_TIMEOUT;
                 return (NET_SOCK_BSD_RTN_CODE_TIMEOUT);
             }
             break;

/*$PAGE*/
        case NET_SOCK_ERR_TIMEOUT:
             NetOS_Unlock();
                                                                    /* Zero-clr rtn sock desc's (see Note #3c2B).       */
             NET_SOCK_DESC_INIT(psock_desc_rd );
             NET_SOCK_DESC_INIT(psock_desc_wr );
             NET_SOCK_DESC_INIT(psock_desc_err);

             return (NET_SOCK_BSD_RTN_CODE_TIMEOUT);                /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_ERR_INVALID_EVENT:
        case NET_SOCK_ERR_CONN_FAIL:
        default:
             NetOS_Unlock();
             return (NET_SOCK_BSD_ERR_SEL);                         /* Prevent 'break NOT reachable' compiler warning.  */
    }


                                                                    /* --------------- RELEASE NET LOCK --------------- */
    NetOS_Unlock();


   *perr =   NET_SOCK_ERR_NONE;

    return ((NET_SOCK_RTN_CODE)sock_nbr_rdy);                       /* Rtn nbr rdy sock desc's (see Note #3c1A1).       */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetSock_CfgTimeoutRxQ_Dflt()
*
* Description : (1) Set socket receive queue configured-default timeout value :
*
*                   (a) Acquire   network lock                                          See Note #2
*                   (b) Validate  socket  used
*                   (c) Configure socket  receive queue default timeout
*                   (d) Release   network lock
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to set receive queue configured-
*                               default timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_PROTOCOL   Invalid socket protocol.
*
*                                                               - RETURNED BY NetOS_Sock_RxQ_TimeoutDflt() : -
*                                                               - RETURNED BY NetOS_TCP_RxQ_TimeoutDflt() : --
*                               NET_SOCK_ERR_NONE               Socket receive queue configured-default 
*                                                                   timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               ------- RETURNED BY NetSock_IsUsed() : -------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ----- RETURNED BY NetTCP_ConnIsUsed() : ------
*                               NET_TCP_ERR_INVALID_CONN        Invalid TCP connection number.
*                               NET_TCP_ERR_CONN_NOT_USED       TCP connection NOT currently used.
*
*                                                               -- RETURNED BY NetConn_ID_TransportGet() : ---
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
*                                                               --------- RETURNED BY NetOS_Lock() : ---------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_CfgTimeoutRxQ_Dflt() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_RxQ_TimeoutDflt()  Note #2'
*                          & 'net_os.c  NetOS_TCP_RxQ_TimeoutDflt()   Note #1'.
*
*               (3) (a) Default case already invalidated in NetSock_Open().  However, the default case is 
*                       included as an extra precaution in case 'SockType' is incorrectly modified.
*
*                   (b) Default case already invalidated in NetSock_Open().  However, the default case is
*                       included as an extra precaution in case 'Protocol' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetSock_CfgTimeoutRxQ_Dflt (NET_SOCK_ID   sock_id,
                                  NET_ERR      *perr)
{
    NET_SOCK     *psock;
    NET_CONN_ID   conn_id;
    NET_CONN_ID   conn_id_transport;
    NET_ERR       err;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return;
    }
#endif


                                                                /* ------------ CFG SOCK RX Q DFLT TIMEOUT ------------ */
    psock = &NetSock_Tbl[sock_id];

    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             NetOS_Sock_RxQ_TimeoutDflt(sock_id, perr);
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             switch (psock->Protocol) {
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
                 case NET_SOCK_PROTOCOL_TCP:
                      conn_id           = psock->ID_Conn;
                      conn_id_transport = NetConn_ID_TransportGet(conn_id, perr);
                      if (*perr != NET_CONN_ERR_NONE) {
                           NetOS_Unlock();
                           return;
                      }

                     (void)NetTCP_ConnIsUsed((NET_TCP_CONN_ID)conn_id_transport,
                                             (NET_ERR       *)perr);
                      if (*perr != NET_TCP_ERR_NONE) {
                           NetOS_Unlock();
                           return;
                      }

                      NetOS_TCP_RxQ_TimeoutDflt((NET_TCP_CONN_ID) conn_id_transport,
                                                (NET_ERR       *)&err);
                     *perr = (err == NET_TCP_ERR_NONE) ? NET_SOCK_ERR_NONE : err;
                      break;
#endif
#endif
                 case NET_SOCK_PROTOCOL_NONE:
                 default:                                       /* See Note #3b.                                        */
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
                      NetOS_Unlock();
                     *perr = NET_SOCK_ERR_INVALID_PROTOCOL;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;
#endif


        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                /* See Note #3a.                                        */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr = NET_SOCK_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


   (void)&conn_id;                                              /* Prevent possible compiler warnings.                  */
   (void)&conn_id_transport;
   (void)&err;


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_CfgTimeoutRxQ_Set()
*
* Description : (1) Set socket receive queue timeout value :
*
*                   (a) Acquire   network lock                                          See Note #2
*                   (b) Validate  socket  used
*                   (c) Configure socket  receive queue timeout
*                   (d) Release   network lock
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set receive queue timeout.
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_PROTOCOL   Invalid socket protocol.
*
*                                                               -- RETURNED BY NetOS_Sock_RxQ_TimeoutSet() : -
*                                                               -- RETURNED BY NetOS_TCP_RxQ_TimeoutSet() : --
*                               NET_SOCK_ERR_NONE               Socket receive queue timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               ------- RETURNED BY NetSock_IsUsed() : -------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ----- RETURNED BY NetTCP_ConnIsUsed() : ------
*                               NET_TCP_ERR_INVALID_CONN        Invalid TCP connection number.
*                               NET_TCP_ERR_CONN_NOT_USED       TCP connection NOT currently used.
*
*                                                               -- RETURNED BY NetConn_ID_TransportGet() : ---
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
*                                                               --------- RETURNED BY NetOS_Lock() : ---------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_CfgTimeoutRxQ_Set() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_RxQ_TimeoutSet()  Note #2'
*                          & 'net_os.c  NetOS_TCP_RxQ_TimeoutSet()   Note #1'.
*
*               (3) (a) Default case already invalidated in NetSock_Open().  However, the default case is 
*                       included as an extra precaution in case 'SockType' is incorrectly modified.
*
*                   (b) Default case already invalidated in NetSock_Open().  However, the default case is
*                       included as an extra precaution in case 'Protocol' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetSock_CfgTimeoutRxQ_Set (NET_SOCK_ID   sock_id,
                                 CPU_INT32U    timeout_ms,
                                 NET_ERR      *perr)
{
    NET_SOCK     *psock;
    NET_CONN_ID   conn_id;
    NET_CONN_ID   conn_id_transport;
    NET_ERR       err;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return;
    }
#endif


                                                                /* -------------- CFG SOCK RX Q TIMEOUT --------------- */
    psock = &NetSock_Tbl[sock_id];

    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             NetOS_Sock_RxQ_TimeoutSet(sock_id, timeout_ms, perr);
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             switch (psock->Protocol) {
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
                 case NET_SOCK_PROTOCOL_TCP:
                      conn_id           = psock->ID_Conn;
                      conn_id_transport = NetConn_ID_TransportGet(conn_id, perr);
                      if (*perr != NET_CONN_ERR_NONE) {
                           NetOS_Unlock();
                           return;
                      }

                     (void)NetTCP_ConnIsUsed((NET_TCP_CONN_ID)conn_id_transport,
                                             (NET_ERR       *)perr);
                      if (*perr != NET_TCP_ERR_NONE) {
                           NetOS_Unlock();
                           return;
                      }

                      NetOS_TCP_RxQ_TimeoutSet((NET_TCP_CONN_ID) conn_id_transport,
                                               (CPU_INT32U     ) timeout_ms,
                                               (NET_ERR       *)&err);
                     *perr = (err == NET_TCP_ERR_NONE) ? NET_SOCK_ERR_NONE : err;
                      break;
#endif
#endif
                 case NET_SOCK_PROTOCOL_NONE:
                 default:                                       /* See Note #3b.                                        */
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
                      NetOS_Unlock();
                     *perr = NET_SOCK_ERR_INVALID_PROTOCOL;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;
#endif


        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                /* See Note #3a.                                        */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr = NET_SOCK_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


   (void)&conn_id;                                              /* Prevent possible compiler warnings.                  */
   (void)&conn_id_transport;
   (void)&err;


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetSock_CfgTimeoutRxQ_Get_ms()
*
* Description : (1) Get socket receive queue timeout value :
*
*                   (a) Acquire  network lock                                           See Note #3
*                   (b) Validate socket  used
*                   (c) Get      socket  receive queue timeout
*                   (d) Release  network lock
*                   (e) Return   socket  receive queue timeout
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get receive queue timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_PROTOCOL   Invalid socket protocol.
*
*                                                               -- RETURNED BY NetOS_Sock_RxQ_TimeoutGet_ms() : ---
*                                                               -- RETURNED BY NetOS_TCP_RxQ_TimeoutGet_ms() : ----
*                               NET_SOCK_ERR_NONE               Socket receive queue timeout successfully returned.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               --------- RETURNED BY NetSock_IsUsed() : ----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               -------- RETURNED BY NetTCP_ConnIsUsed() : --------
*                               NET_TCP_ERR_INVALID_CONN        Invalid TCP connection number.
*                               NET_TCP_ERR_CONN_NOT_USED       TCP connection NOT currently used.
*
*                                                               ----- RETURNED BY NetConn_ID_TransportGet() : -----
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
*                                                               ----------- RETURNED BY NetOS_Lock() : ------------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : Socket receive queue network timeout value :
*
*                   0,                         on any errors.
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*
* Note(s)     : (2) Despite inconsistency with other 'Get' status functions,
*                   NetSock_CfgTimeoutRxQ_Get_ms() includes 'Cfg' for consistency with other
*                   NetSock_CfgTimeout???() functions.
*
*               (3) NetSock_CfgTimeoutRxQ_Get_ms() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_RxQ_TimeoutGet_ms()  Note #2'
*                          & 'net_os.c  NetOS_TCP_RxQ_TimeoutGet_ms()   Note #1'.
*
*               (4) (a) Default case already invalidated in NetSock_Open().  However, the default case is 
*                       included as an extra precaution in case 'SockType' is incorrectly modified.
*
*                   (b) Default case already invalidated in NetSock_Open().  However, the default case is
*                       included as an extra precaution in case 'Protocol' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
CPU_INT32U  NetSock_CfgTimeoutRxQ_Get_ms (NET_SOCK_ID   sock_id,
                                          NET_ERR      *perr)
{
    NET_SOCK     *psock;
    NET_CONN_ID   conn_id;
    NET_CONN_ID   conn_id_transport;
    NET_ERR       err;
    CPU_INT32U    timeout_ms;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (0);
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (0);
    }
#endif


                                                                /* -------------- GET SOCK RX Q TIMEOUT --------------- */
    psock = &NetSock_Tbl[sock_id];

    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             timeout_ms = NetOS_Sock_RxQ_TimeoutGet_ms(sock_id, perr);
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             switch (psock->Protocol) {
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
                 case NET_SOCK_PROTOCOL_TCP:
                      conn_id           = psock->ID_Conn;
                      conn_id_transport = NetConn_ID_TransportGet(conn_id, perr);
                      if (*perr != NET_CONN_ERR_NONE) {
                           NetOS_Unlock();
                           return (0);
                      }

                     (void)NetTCP_ConnIsUsed((NET_TCP_CONN_ID)conn_id_transport,
                                             (NET_ERR       *)perr);
                      if (*perr != NET_TCP_ERR_NONE) {
                           NetOS_Unlock();
                           return (0);
                      }

                      timeout_ms = NetOS_TCP_RxQ_TimeoutGet_ms((NET_TCP_CONN_ID) conn_id_transport,
                                                               (NET_ERR       *)&err);
                     *perr = (err == NET_TCP_ERR_NONE) ? NET_SOCK_ERR_NONE : err;
                      break;
#endif
#endif
                 case NET_SOCK_PROTOCOL_NONE:
                 default:                                       /* See Note #4b.                                        */
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
                      NetOS_Unlock();
                     *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;
                      return (0);                               /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;
#endif


        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                /* See Note #4a.                                        */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_TYPE;
             return (0);                                        /* Prevent 'break NOT reachable' compiler warning.      */
    }

/*$PAGE*/
   (void)&conn_id;                                              /* Prevent possible compiler warnings.                  */
   (void)&conn_id_transport;
   (void)&err;


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


    return (timeout_ms);                                        /* -------------- RTN SOCK RX Q TIMEOUT --------------- */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetSock_CfgTimeoutTxQ_Dflt()
*
* Description : (1) Set socket transmit queue configured-default timeout value :
*
*                   (a) Acquire   network lock                                          See Note #3
*                   (b) Validate  socket  used
*                   (c) Configure socket  transmit queue default timeout
*                   (d) Release   network lock
*
*               (2) Socket transmit queues apply to the following socket type(s)/protocol(s) :
*
*                   (a) Stream
*                       (1) TCP
*
*                   (b) #### Datagram sockets NOT blocked during transmit since socket receive/transmit 
*                       buffer queue sizes NOT yet implemented.
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to set transmit queue configured-
*                               default timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_PROTOCOL   Invalid socket protocol.
*
*                                                               - RETURNED BY NetOS_TCP_TxQ_TimeoutDflt() : -
*                               NET_SOCK_ERR_NONE               Socket transmit queue configured-default 
*                                                                   timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               ------ RETURNED BY NetSock_IsUsed() : -------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ----- RETURNED BY NetTCP_ConnIsUsed() : -----
*                               NET_TCP_ERR_INVALID_CONN        Invalid TCP connection number.
*                               NET_TCP_ERR_CONN_NOT_USED       TCP connection NOT currently used.
*
*                                                               -- RETURNED BY NetConn_ID_TransportGet() : --
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
*                                                               -------- RETURNED BY NetOS_Lock() : ---------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (3) NetSock_CfgTimeoutTxQ_Dflt() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_TCP_TxQ_TimeoutDflt()  Note #1'.
*
*               (4) (a) Default case already invalidated in NetSock_Open().  However, the default case is 
*                       included as an extra precaution in case 'SockType' is incorrectly modified.
*
*                   (b) Default case already invalidated in NetSock_Open().  However, the default case is
*                       included as an extra precaution in case 'Protocol' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetSock_CfgTimeoutTxQ_Dflt (NET_SOCK_ID   sock_id,
                                  NET_ERR      *perr)
{
    NET_SOCK     *psock;
    NET_CONN_ID   conn_id;
    NET_CONN_ID   conn_id_transport;
    NET_ERR       err;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return;
    }
#endif


                                                                /* ------------ CFG SOCK TX Q DFLT TIMEOUT ------------ */
    psock = &NetSock_Tbl[sock_id];

    switch (psock->SockType) {
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             switch (psock->Protocol) {
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
                 case NET_SOCK_PROTOCOL_TCP:
                      conn_id           = psock->ID_Conn;
                      conn_id_transport = NetConn_ID_TransportGet(conn_id, perr);
                      if (*perr != NET_CONN_ERR_NONE) {
                           NetOS_Unlock();
                           return;
                      }

                     (void)NetTCP_ConnIsUsed((NET_TCP_CONN_ID)conn_id_transport,
                                             (NET_ERR       *)perr);
                      if (*perr != NET_TCP_ERR_NONE) {
                           NetOS_Unlock();
                           return;
                      }

                      NetOS_TCP_TxQ_TimeoutDflt((NET_TCP_CONN_ID) conn_id_transport,
                                                (NET_ERR       *)&err);
                     *perr = (err == NET_TCP_ERR_NONE) ? NET_SOCK_ERR_NONE : err;
                      break;
#endif
#endif
                 case NET_SOCK_PROTOCOL_NONE:
                 default:                                       /* See Note #4b.                                        */
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
                      NetOS_Unlock();
                     *perr = NET_SOCK_ERR_INVALID_PROTOCOL;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;
#endif


        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        case NET_SOCK_TYPE_DATAGRAM:                            /* See Note #2b.                                        */
        default:                                                /* See Note #4a.                                        */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
                                                                /* NetOS_Unlock() intentionally omitted       ...       */
            *perr = NET_SOCK_ERR_INVALID_TYPE;
             break;                                             /* ... in order to 'break' & prevent possible ...       */
                                                                /* ... 'unreachable code' compiler warning.             */
    }


   (void)&conn_id;                                              /* Prevent possible compiler warnings.                  */
   (void)&conn_id_transport;
   (void)&err;


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_CfgTimeoutTxQ_Set()
*
* Description : (1) Set socket transmit queue timeout value :
*
*                   (a) Acquire   network lock                                          See Note #3
*                   (b) Validate  socket  used
*                   (c) Configure socket  transmit queue timeout
*                   (d) Release   network lock
*
*               (2) Socket transmit queues apply to the following socket type(s)/protocol(s) :
*
*                   (a) Stream
*                       (1) TCP
*
*                   (b) #### Datagram sockets NOT blocked during transmit since socket receive/transmit 
*                       buffer queue sizes NOT yet implemented.
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set transmit queue timeout.
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_PROTOCOL   Invalid socket protocol.
*
*                                                               -- RETURNED BY NetOS_TCP_TxQ_TimeoutSet() : ---
*                               NET_SOCK_ERR_NONE               Socket transmit queue timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               ------- RETURNED BY NetSock_IsUsed() : --------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ------ RETURNED BY NetTCP_ConnIsUsed() : ------
*                               NET_TCP_ERR_INVALID_CONN        Invalid TCP connection number.
*                               NET_TCP_ERR_CONN_NOT_USED       TCP connection NOT currently used.
*
*                                                               --- RETURNED BY NetConn_ID_TransportGet() : ---
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
*                                                               --------- RETURNED BY NetOS_Lock() : ----------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (3) NetSock_CfgTimeoutTxQ_Set() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_TCP_TxQ_TimeoutSet()  Note #1'.
*
*               (4) (a) Default case already invalidated in NetSock_Open().  However, the default case is 
*                       included as an extra precaution in case 'SockType' is incorrectly modified.
*
*                   (b) Default case already invalidated in NetSock_Open().  However, the default case is
*                       included as an extra precaution in case 'Protocol' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetSock_CfgTimeoutTxQ_Set (NET_SOCK_ID   sock_id,
                                 CPU_INT32U    timeout_ms,
                                 NET_ERR      *perr)
{
    NET_SOCK     *psock;
    NET_CONN_ID   conn_id;
    NET_CONN_ID   conn_id_transport;
    NET_ERR       err;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return;
    }
#endif


                                                                /* -------------- CFG SOCK TX Q TIMEOUT --------------- */
    psock = &NetSock_Tbl[sock_id];

    switch (psock->SockType) {
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             switch (psock->Protocol) {
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
                 case NET_SOCK_PROTOCOL_TCP:
                      conn_id           = psock->ID_Conn;
                      conn_id_transport = NetConn_ID_TransportGet(conn_id, perr);
                      if (*perr != NET_CONN_ERR_NONE) {
                           NetOS_Unlock();
                           return;
                      }

                     (void)NetTCP_ConnIsUsed((NET_TCP_CONN_ID)conn_id_transport,
                                             (NET_ERR       *)perr);
                      if (*perr != NET_TCP_ERR_NONE) {
                           NetOS_Unlock();
                           return;
                      }

                      NetOS_TCP_TxQ_TimeoutSet((NET_TCP_CONN_ID) conn_id_transport,
                                               (CPU_INT32U     ) timeout_ms,
                                               (NET_ERR       *)&err);
                     *perr = (err == NET_TCP_ERR_NONE) ? NET_SOCK_ERR_NONE : err;
                      break;
#endif
#endif
                 case NET_SOCK_PROTOCOL_NONE:
                 default:                                       /* See Note #4b.                                        */
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
                      NetOS_Unlock();
                     *perr = NET_SOCK_ERR_INVALID_PROTOCOL;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;
#endif


        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        case NET_SOCK_TYPE_DATAGRAM:                            /* See Note #2b.                                        */
        default:                                                /* See Note #4a.                                        */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
                                                                /* NetOS_Unlock() intentionally omitted       ...       */
            *perr = NET_SOCK_ERR_INVALID_TYPE;
             break;                                             /* ... in order to 'break' & prevent possible ...       */
                                                                /* ... 'unreachable code' compiler warning.             */
    }


   (void)&conn_id;                                              /* Prevent possible compiler warnings.                  */
   (void)&conn_id_transport;
   (void)&err;
   (void)&timeout_ms;


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetSock_CfgTimeoutTxQ_Get_ms()
*
* Description : (1) Get socket transmit queue timeout value :
*
*                   (a) Acquire  network lock                                           See Note #4
*                   (b) Validate socket  used
*                   (c) Get      socket  transmit queue timeout
*                   (d) Release  network lock
*                   (e) Return   socket  transmit queue timeout
*
*               (2) Socket transmit queues apply to the following socket type(s)/protocol(s) :
*
*                   (a) Stream
*                       (1) TCP
*
*                   (b) #### Datagram sockets NOT blocked during transmit since socket receive/transmit 
*                       buffer queue sizes NOT yet implemented.
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get transmit queue timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_PROTOCOL   Invalid socket protocol.
*
*                                                               -- RETURNED BY NetOS_Sock_RxQ_TimeoutGet_ms() : ---
*                                                               -- RETURNED BY NetOS_TCP_TxQ_TimeoutGet_ms() : ----
*                               NET_SOCK_ERR_NONE               Socket transmit queue timeout successfully returned.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               --------- RETURNED BY NetSock_IsUsed() : ----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               -------- RETURNED BY NetTCP_ConnIsUsed() : --------
*                               NET_TCP_ERR_INVALID_CONN        Invalid TCP connection number.
*                               NET_TCP_ERR_CONN_NOT_USED       TCP connection NOT currently used.
*
*                                                               ----- RETURNED BY NetConn_ID_TransportGet() : -----
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
*                                                               ----------- RETURNED BY NetOS_Lock() : ------------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : Socket transmit queue network timeout value :
*
*                   0,                         on any errors.
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*
* Note(s)     : (3) Despite inconsistency with other 'Get' status functions,
*                   NetSock_CfgTimeoutTxQ_Get_ms() includes 'Cfg' for consistency with other
*                   NetSock_CfgTimeout???() functions.
*
*               (4) NetSock_CfgTimeoutTxQ_Get_ms() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_TCP_TxQ_TimeoutGet_ms()  Note #1'.
*
*               (5) (a) Default case already invalidated in NetSock_Open().  However, the default case is 
*                       included as an extra precaution in case 'SockType' is incorrectly modified.
*
*                   (b) Default case already invalidated in NetSock_Open().  However, the default case is
*                       included as an extra precaution in case 'Protocol' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
CPU_INT32U  NetSock_CfgTimeoutTxQ_Get_ms (NET_SOCK_ID   sock_id,
                                          NET_ERR      *perr)
{
    NET_SOCK     *psock;
    NET_CONN_ID   conn_id;
    NET_CONN_ID   conn_id_transport;
    NET_ERR       err;
    CPU_INT32U    timeout_ms;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (0);
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (0);
    }
#endif


                                                                /* -------------- GET SOCK TX Q TIMEOUT --------------- */
    psock = &NetSock_Tbl[sock_id];

    switch (psock->SockType) {
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             switch (psock->Protocol) {
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
                 case NET_SOCK_PROTOCOL_TCP:
                      conn_id           = psock->ID_Conn;
                      conn_id_transport = NetConn_ID_TransportGet(conn_id, perr);
                      if (*perr != NET_CONN_ERR_NONE) {
                           NetOS_Unlock();
                           return (0);
                      }

                     (void)NetTCP_ConnIsUsed((NET_TCP_CONN_ID)conn_id_transport,
                                             (NET_ERR       *)perr);
                      if (*perr != NET_TCP_ERR_NONE) {
                           NetOS_Unlock();
                           return (0);
                      }

                      timeout_ms = NetOS_TCP_TxQ_TimeoutGet_ms((NET_TCP_CONN_ID) conn_id_transport,
                                                               (NET_ERR       *)&err);
                     *perr = (err == NET_TCP_ERR_NONE) ? NET_SOCK_ERR_NONE : err;
                      break;
#endif
#endif
                 case NET_SOCK_PROTOCOL_NONE:
                 default:                                       /* See Note #5b.                                        */
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
                      NetOS_Unlock();
                     *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;
                      return (0);                               /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;
#endif


        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        case NET_SOCK_TYPE_DATAGRAM:                            /* See Note #2b.                                        */
        default:                                                /* See Note #5a.                                        */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
                                                                /* NetOS_Unlock() intentionally omitted       ...       */
             timeout_ms = 0;
            *perr       = NET_SOCK_ERR_INVALID_TYPE;
             break;                                             /* ... in order to 'break' & prevent possible ...       */
                                                                /* ... 'unreachable code' compiler warning.             */
    }


   (void)&conn_id;                                              /* Prevent possible compiler warnings.                  */
   (void)&conn_id_transport;
   (void)&err;


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


    return (timeout_ms);                                        /* -------------- RTN SOCK TX Q TIMEOUT --------------- */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetSock_CfgTimeoutConnReqDflt()
*
* Description : (1) Set socket connection request configured-default timeout value :
*
*                   (a) Acquire   network lock                                          See Note #2
*                   (b) Validate  socket  used
*                   (c) Configure socket  connection request default timeout
*                   (d) Release   network lock
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to set connection request 
*                               configured-default timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetOS_Sock_ConnReqTimeoutDflt() : -
*                               NET_SOCK_ERR_NONE               Socket connection request configured-default
*                                                                    timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               -------- RETURNED BY NetSock_IsUsed() : ---------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ---------- RETURNED BY NetOS_Lock() : -----------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_CfgTimeoutConnReqDflt() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_ConnReqTimeoutDflt()  Note #1'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetSock_CfgTimeoutConnReqDflt (NET_SOCK_ID   sock_id,
                                     NET_ERR      *perr)
{
                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return;
    }
#endif
                                                                /* ---------- CFG SOCK CONN REQ DFLT TIMEOUT ---------- */
    NetOS_Sock_ConnReqTimeoutDflt(sock_id, perr);
                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetSock_CfgTimeoutConnReqSet()
*
* Description : (1) Set socket connection request timeout value :
*
*                   (a) Acquire   network lock                                          See Note #2
*                   (b) Validate  socket  used
*                   (c) Configure socket  connection request timeout
*                   (d) Release   network lock
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set connection request timeout.
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               -- RETURNED BY NetOS_Sock_ConnReqTimeoutSet() : ---
*                               NET_SOCK_ERR_NONE               Socket connection request timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               --------- RETURNED BY NetSock_IsUsed() : ----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ----------- RETURNED BY NetOS_Lock() : ------------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_CfgTimeoutConnReqSet() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_ConnReqTimeoutSet()  Note #1'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetSock_CfgTimeoutConnReqSet (NET_SOCK_ID   sock_id,
                                    CPU_INT32U    timeout_ms,
                                    NET_ERR      *perr)
{
                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return;
    }
#endif
                                                                /* ------------ CFG SOCK CONN REQ TIMEOUT ------------- */
    NetOS_Sock_ConnReqTimeoutSet(sock_id, timeout_ms, perr);
                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetSock_CfgTimeoutConnReqGet_ms()
*
* Description : (1) Get socket connection request timeout value :
*
*                   (a) Acquire  network lock                                           See Note #3
*                   (b) Validate socket  used
*                   (c) Get      socket  connection request timeout
*                   (d) Release  network lock
*                   (e) Return   socket  connection request timeout
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get connection request timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetOS_Sock_ConnReqTimeoutGet_ms() : -
*                               NET_SOCK_ERR_NONE               Socket connection request timeout successfully
*                                                                   returned.
*
*                                                               --------- RETURNED BY NetSock_IsUsed() : ----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ----------- RETURNED BY NetOS_Lock() : ------------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : Socket connection request network timeout value :
*
*                   0,                         on any errors.
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*
* Note(s)     : (2) Despite inconsistency with other 'Get' status functions,
*                   NetSock_CfgTimeoutConnReqGet_ms() includes 'Cfg' for consistency with other
*                   NetSock_CfgTimeout???() functions.
*
*               (3) NetSock_CfgTimeoutConnReqGet_ms() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_ConnReqTimeoutGet_ms()  Note #1'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
CPU_INT32U  NetSock_CfgTimeoutConnReqGet_ms (NET_SOCK_ID   sock_id,
                                             NET_ERR      *perr)
{
    CPU_INT32U  timeout_ms;

                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (0);
    }

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (0);
    }
#endif
                                                                /* ------------ GET SOCK CONN REQ TIMEOUT ------------- */
    timeout_ms = NetOS_Sock_ConnReqTimeoutGet_ms(sock_id, perr);
                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();

    return (timeout_ms);                                        /* ------------ RTN SOCK CONN REQ TIMEOUT ------------- */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetSock_CfgTimeoutConnAcceptDflt()
*
* Description : (1) Set socket connection accept configured-default timeout value :
*
*                   (a) Acquire   network lock                                          See Note #2
*                   (b) Validate  socket  used
*                   (c) Configure socket  connection accept default timeout
*                   (d) Release   network lock
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to set connection accept 
*                               configured-default timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetOS_Sock_ConnAcceptQ_TimeoutDflt() : -
*                               NET_SOCK_ERR_NONE               Socket connection accept configured-default
*                                                                    timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               ----------- RETURNED BY NetSock_IsUsed() : -----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ------------- RETURNED BY NetOS_Lock() : -------------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_CfgTimeoutConnAcceptDflt() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_ConnAcceptQ_TimeoutDflt()  Note #2'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetSock_CfgTimeoutConnAcceptDflt (NET_SOCK_ID   sock_id,
                                        NET_ERR      *perr)
{
                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return;
    }
#endif
                                                                /* --------- CFG SOCK CONN ACCEPT DFLT TIMEOUT -------- */
    NetOS_Sock_ConnAcceptQ_TimeoutDflt(sock_id, perr);
                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetSock_CfgTimeoutConnAcceptSet()
*
* Description : (1) Set socket connection accept timeout value :
*
*                   (a) Acquire   network lock                                          See Note #2
*                   (b) Validate  socket  used
*                   (c) Configure socket  connection accept timeout
*                   (d) Release   network lock
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set connection accept timeout.
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetOS_Sock_ConnAcceptQ_TimeoutSet() : -
*                               NET_SOCK_ERR_NONE               Socket connection accept timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               ---------- RETURNED BY NetSock_IsUsed() : -----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ------------ RETURNED BY NetOS_Lock() : -------------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_CfgTimeoutConnAcceptSet() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_ConnAcceptQ_TimeoutSet()  Note #2'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetSock_CfgTimeoutConnAcceptSet (NET_SOCK_ID   sock_id,
                                       CPU_INT32U    timeout_ms,
                                       NET_ERR      *perr)
{
                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return;
    }
#endif
                                                                /* ----------- CFG SOCK CONN ACCEPT TIMEOUT ----------- */
    NetOS_Sock_ConnAcceptQ_TimeoutSet(sock_id, timeout_ms, perr);
                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetSock_CfgTimeoutConnAcceptGet_ms()
*
* Description : (1) Get socket connection accept timeout value :
*
*                   (a) Acquire  network lock                                           See Note #3
*                   (b) Validate socket  used
*                   (c) Get      socket  connection accept timeout
*                   (d) Release  network lock
*                   (e) Return   socket  connection accept timeout
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get connection accept timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetOS_Sock_ConnAcceptQ_TimeoutGet_ms() : -
*                               NET_SOCK_ERR_NONE               Socket connection accept timeout successfully
*                                                                   returned.
*
*                                                               ------------ RETURNED BY NetSock_IsUsed() : ------------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               -------------- RETURNED BY NetOS_Lock() : --------------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : Socket connection accept network timeout value :
*
*                   0,                         on any errors.
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*
* Note(s)     : (2) Despite inconsistency with other 'Get' status functions,
*                   NetSock_CfgTimeoutConnAcceptGet_ms() includes 'Cfg' for consistency with other
*                   NetSock_CfgTimeout???() functions.
*
*               (3) NetSock_CfgTimeoutConnAcceptGet_ms() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_ConnAcceptQ_TimeoutGet_ms()  Note #2'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
CPU_INT32U  NetSock_CfgTimeoutConnAcceptGet_ms (NET_SOCK_ID   sock_id,
                                                NET_ERR      *perr)
{
    CPU_INT32U  timeout_ms;

                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (0);
    }

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (0);
    }
#endif
                                                                /* ----------- GET SOCK CONN ACCEPT TIMEOUT ----------- */
    timeout_ms = NetOS_Sock_ConnAcceptQ_TimeoutGet_ms(sock_id, perr);
                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();

    return (timeout_ms);                                        /* ----------- RTN SOCK CONN ACCEPT TIMEOUT ----------- */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetSock_CfgTimeoutConnCloseDflt()
*
* Description : (1) Set socket connection close configured-default timeout value :
*
*                   (a) Acquire   network lock                                          See Note #2
*                   (b) Validate  socket  used
*                   (c) Configure socket  connection close default timeout
*                   (d) Release   network lock
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to set connection close 
*                               configured-default timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetOS_Sock_ConnCloseTimeoutDflt() : -
*                               NET_SOCK_ERR_NONE               Socket connection close configured-default
*                                                                    timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               --------- RETURNED BY NetSock_IsUsed() : ----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ----------- RETURNED BY NetOS_Lock() : ------------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_CfgTimeoutConnCloseDflt() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_ConnCloseTimeoutDflt()  Note #1'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetSock_CfgTimeoutConnCloseDflt (NET_SOCK_ID   sock_id,
                                       NET_ERR      *perr)
{
                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return;
    }
#endif
                                                                /* --------- CFG SOCK CONN CLOSE DFLT TIMEOUT --------- */
    NetOS_Sock_ConnCloseTimeoutDflt(sock_id, perr);
                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetSock_CfgTimeoutConnCloseSet()
*
* Description : (1) Set socket connection close timeout value :
*
*                   (a) Acquire   network lock                                          See Note #2
*                   (b) Validate  socket  used
*                   (c) Configure socket  connection close timeout
*                   (d) Release   network lock
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set connection close timeout.
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetOS_Sock_ConnCloseTimeoutSet() : -
*                               NET_SOCK_ERR_NONE               Socket connection close timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
*                                                               -------- RETURNED BY NetSock_IsUsed() : ----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ---------- RETURNED BY NetOS_Lock() : ------------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (2) NetSock_CfgTimeoutConnCloseSet() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_ConnCloseTimeoutSet()  Note #1'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetSock_CfgTimeoutConnCloseSet (NET_SOCK_ID   sock_id,
                                      CPU_INT32U    timeout_ms,
                                      NET_ERR      *perr)
{
                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return;
    }
#endif
                                                                /* ----------- CFG SOCK CONN CLOSE TIMEOUT ------------ */
    NetOS_Sock_ConnCloseTimeoutSet(sock_id, timeout_ms, perr);
                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetSock_CfgTimeoutConnCloseGet_ms()
*
* Description : (1) Get socket connection close timeout value :
*
*                   (a) Acquire  network lock                                           See Note #3
*                   (b) Validate socket  used
*                   (c) Get      socket  connection close timeout
*                   (d) Release  network lock
*                   (e) Return   socket  connection close timeout
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get connection close timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetOS_Sock_ConnCloseTimeoutGet_ms() : -
*                               NET_SOCK_ERR_NONE               Socket connection close timeout successfully
*                                                                   returned.
*
*                                                               ---------- RETURNED BY NetSock_IsUsed() : -----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
*                                                               ------------ RETURNED BY NetOS_Lock() : -------------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : Socket connection close network timeout value :
*
*                   0,                         on any errors.
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*
* Note(s)     : (2) Despite inconsistency with other 'Get' status functions,
*                   NetSock_CfgTimeoutConnCloseGet_ms() includes 'Cfg' for consistency with other
*                   NetSock_CfgTimeout???() functions.
*
*               (3) NetSock_CfgTimeoutConnCloseGet_ms() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'net_os.c  NetOS_Sock_ConnCloseTimeoutGet_ms()  Note #1'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
CPU_INT32U  NetSock_CfgTimeoutConnCloseGet_ms (NET_SOCK_ID   sock_id,
                                               NET_ERR      *perr)
{
    CPU_INT32U  timeout_ms;

                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (0);
    }

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (0);
    }
#endif
                                                                /* ----------- GET SOCK CONN CLOSE TIMEOUT ------------ */
    timeout_ms = NetOS_Sock_ConnCloseTimeoutGet_ms(sock_id, perr);
                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();

    return (timeout_ms);                                        /* ----------- RTN SOCK CONN CLOSE TIMEOUT ------------ */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetSock_IsUsed()
*
* Description : Validate socket in use.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to validate.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket successfully validated as in use.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
* Return(s)   : DEF_YES, socket   valid &      in use.
*
*               DEF_NO,  socket invalid or NOT in use.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) NetSock_IsUsed() blocked until network initialization completes.
*
*               (2) NetSock_IsUsed() MUST be called with the global network lock already acquired.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetSock_IsUsed (NET_SOCK_ID   sock_id,
                             NET_ERR      *perr)
{
    NET_SOCK     *psock;
    CPU_BOOLEAN   used;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit (see Note #1).            */
       *perr =  NET_ERR_INIT_INCOMPLETE;
        return (DEF_NO);
    }
#endif

                                                                /* ----------------- VALIDATE SOCK ID ----------------- */
    if (sock_id < NET_SOCK_ID_MIN) {
       *perr =  NET_SOCK_ERR_INVALID_SOCK;
        return (DEF_NO);
    }
    if (sock_id > NET_SOCK_ID_MAX) {
       *perr =  NET_SOCK_ERR_INVALID_SOCK;
        return (DEF_NO);
    }

                                                                /* ---------------- VALIDATE SOCK USED ---------------- */
    psock = &NetSock_Tbl[sock_id];
    used  =  DEF_BIT_IS_SET(psock->Flags, NET_SOCK_FLAG_USED);
    if (used != DEF_YES) {
       *perr =  NET_SOCK_ERR_NOT_USED;
        return (DEF_NO);
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (DEF_YES);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetSock_IsConn()
*
* Description : Validate socket in use & connected.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to validate.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket successfully checked; check return 
*                                                                   value for socket connection status.
*
*                                                               ---- RETURNED BY NetSock_IsUsed() : -----
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
* Return(s)   : DEF_YES, socket   valid &      connected.
*
*               DEF_NO,  socket invalid or NOT connected.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) NetSock_IsConn() MUST be called with the global network lock already acquired.
*
*               (2) #### NetSock_IsConn() may NOT be necessary (remove if unnecessary).
*********************************************************************************************************
*/

CPU_BOOLEAN  NetSock_IsConn (NET_SOCK_ID   sock_id,
                             NET_ERR      *perr)
{
    NET_SOCK     *psock;
    CPU_BOOLEAN   conn;

                                                                /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return (DEF_NO);
    }

                                                                /* ------------ DETERMINE SOCK CONN STATUS ------------ */
    psock = &NetSock_Tbl[sock_id];
    switch (psock->State) {
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
             conn = DEF_YES;
             break;
             

        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_CLOSED_FAULT:
        case NET_SOCK_STATE_BOUND:
        default:
             conn = DEF_NO;
             break;
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (conn);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetSock_PoolStatGet()
*
* Description : Get socket statistics pool.
*
* Argument(s) : none.
*
* Return(s)   : Socket statistics pool, if NO errors.
*
*               NULL   statistics pool, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) NetSock_PoolStatGet() blocked until network initialization completes; return NULL
*                   statistics pool.
*
*               (2) 'NetSock_PoolStat' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

NET_STAT_POOL  NetSock_PoolStatGet (void)
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
    stat_pool = NetSock_PoolStat;
    CPU_CRITICAL_EXIT();

    return (stat_pool);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetSock_PoolStatResetMaxUsed()
*
* Description : Reset socket statistics pool's maximum number of entries used.
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

void  NetSock_PoolStatResetMaxUsed (void)
{
    NET_ERR  err;


    NetStat_PoolResetUsedMax(&NetSock_PoolStat, &err);
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
*                                     NetSock_RxPktValidateBuf()
*
* Description : Validate received buffer header as socket layer.
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header that received socket packet.
*               --------    Argument validated in NetSock_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Received buffer's socket/data header validated.
*                               NET_ERR_INVALID_PROTOCOL        Buffer's protocol type is NOT socket.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Rx().
*
* Note(s)     : (1) Since lower network layers eventually demultiplex data to the application layer, the
*                   socket layer must validate received packet's as application type, not socket type.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetSock_RxPktValidateBuf (NET_BUF_HDR  *pbuf_hdr,
                                        NET_ERR      *perr)
{
                                                                /* -------------- VALIDATE SOCK/DATA HDR -------------- */
    if (pbuf_hdr->ProtocolHdrType != NET_PROTOCOL_TYPE_APP) {   /* See Note #1.                                         */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrRxInvalidProtocolCtr);
       *perr = NET_ERR_INVALID_PROTOCOL;
        return;
    }

    if (pbuf_hdr->DataIx == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrRxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

   *perr = NET_SOCK_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetSock_RxPktDemux()
*
* Description : (1) Demultiplex received packet to appropriate socket :
*
*                   (a) Determine appropriate receive socket :
*
*                       (1) Packet's socket demultiplexed in previous transport layer.
*
*                       (2) Search connection list for socket identifier whose local &/or remote addresses 
*                           are identical to the received packet's destination & source addresses.
*
*                   (b) Validate socket connection
*                   (c) Demultiplex received packet to appropriate socket
*
*
* Argument(s) : pbuf        Pointer to network buffer that received socket data.
*               ----        Argument checked   in NetSock_Rx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetSock_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Received packet successfully demultiplexed
*                                                                       to appropriate socket.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_ERR_RX_DEST                     Invalid destination; no socket connection
*                                                                       available for received packet.
*
*                                                                   ----- RETURNED BY NetConn_IsConn() : -----
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*                               NET_CONN_ERR_CONN_NONE              NO connection.                              
*
*                                                                   -- RETURNED BY NetOS_Sock_RxQ_Signal() : -
*                               NET_SOCK_ERR_RX_Q_FULL              Socket receive queue full.
*                               NET_SOCK_ERR_RX_Q_SIGNAL            Socket receive queue signal failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Rx().
*
* Note(s)     : (2) (a) Assumes packet buffer's internet protocol controls configured in previous layer(s).
*
*                   (b) Assumes packet's addresses & ports demultiplexed & decoded  in previous layer(s).
*
*               (3) (a) Since datagram-type sockets transmit & receive all data atomically, each datagram
*                       socket receive MUST always receive exactly one datagram.  Therefore, the socket
*                       receive queue  MUST be signaled for each datagram packet received.
*
*                   (b) Stream-type sockets transmit & receive all data octets in one or more non-distinct 
*                       packets.  In other words, the application data is NOT bounded by any specific 
*                       packet(s); rather, it is contiguous & sequenced from one packet to the next.
*                   
*                       Therefore, the socket receive queue is signaled ONLY when data is received for a
*                       connection where data was previously unavailable.
*
*               (4) Default case already invalidated in earlier internet protocol layer function(s).
*                   However, the default case is included as an extra precaution in case 'Protocol'
*                   is incorrectly modified.
*
*               (5) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*
*               (6) Default case already invalidated in NetSock_Open().  However, the default case 
*                   is included as an extra precaution in case 'SockType' is incorrectly modified.
*
*               (7) Socket connection addresses are maintained in network-order.
*
*               (8) Some buffer controls were previously initialized in NetBuf_Get() when the packet
*                   was received at the network interface layer.  These buffer controls do NOT need
*                   to be re-initialized but are shown for completeness.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetSock_RxPktDemux (NET_BUF      *pbuf,
                                  NET_BUF_HDR  *pbuf_hdr,
                                  NET_ERR      *perr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN            used;
    CPU_BOOLEAN            conn;
#endif
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
    CPU_BOOLEAN            q_prevly_empty;
#endif
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_IP_HDR            *pip_hdr;
    NET_IP_ADDR            addr_ip;
#endif
    CPU_INT08U             addr_local[NET_SOCK_CFG_ADDR_LEN];
    CPU_INT08U             addr_remote[NET_SOCK_CFG_ADDR_LEN];
    NET_PORT_NBR           port_nbr;
    NET_CONN_PROTOCOL_IX   protocol_ix;
    NET_CONN_FAMILY        family;
    NET_CONN_ID            conn_id;
    NET_SOCK_ID            sock_id;
    NET_SOCK              *psock;
    NET_BUF               *pbuf_tail;
    NET_BUF_HDR           *pbuf_hdr_tail;
    NET_ERR                err;


    sock_id = (NET_SOCK_ID)pbuf_hdr->Conn_ID_App;
    conn_id = (NET_CONN_ID)pbuf_hdr->Conn_ID;

                                                                /* --------------- CHK PREV SOCK DEMUX ---------------- */
    if (sock_id != (NET_SOCK_ID)NET_CONN_ID_NONE) {             /* If sock id demux'd by prev layer (see Note #1a1), ...*/
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ... validate sock conn.                              */
        NetConn_IsConn(conn_id, perr);
        switch (*perr) {
            case NET_CONN_ERR_CONN_HALF:
                 conn = DEF_NO;
                 break;


            case NET_CONN_ERR_CONN_FULL:
                 conn = DEF_YES;
                 break;


            case NET_CONN_ERR_INVALID_CONN:
            case NET_CONN_ERR_NOT_USED:
            case NET_CONN_ERR_CONN_NONE:
            default:
                                                                /* Rtn err from NetConn_IsConn().                       */
                 return;                                        /* Prevent 'break NOT reachable' compiler warning.      */
        }
#endif

/*$PAGE*/
    } else {                                                        /* ---- SRCH CONN LIST(S) FOR PKT/SOCK ADDR(S) ---- */

#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
        family  =  NET_CONN_FAMILY_IP_V4_SOCK;
        pip_hdr = (NET_IP_HDR *)&pbuf->Data[pbuf_hdr->IP_HdrIx];
        switch (pip_hdr->Protocol) {
            case NET_IP_HDR_PROTOCOL_UDP:
                 protocol_ix = NET_CONN_PROTOCOL_IX_IP_V4_UDP;
                 break;


#ifdef  NET_TCP_MODULE_PRESENT
            case NET_IP_HDR_PROTOCOL_TCP:
                 protocol_ix = NET_CONN_PROTOCOL_IX_IP_V4_TCP;
                 break;
#endif

            default:                                                /* See Note #4.                                     */
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
                *perr = NET_SOCK_ERR_INVALID_PROTOCOL;
                 return;                                            /* Prevent 'break NOT reachable' compiler warning.  */
        }

                                                                    /* Cfg srch local  addr as pkt dest addr.           */
        port_nbr = NET_UTIL_HOST_TO_NET_16(pbuf_hdr->TCP_UDP_PortDest);
        addr_ip  = NET_UTIL_HOST_TO_NET_32(pbuf_hdr->IP_AddrDest);

        Mem_Copy((void     *)&addr_local[NET_SOCK_ADDR_IP_IX_PORT],
                 (void     *)&port_nbr,
                 (CPU_SIZE_T) sizeof(port_nbr));
        Mem_Copy((void     *)&addr_local[NET_SOCK_ADDR_IP_IX_ADDR],
                 (void     *)&addr_ip,
                 (CPU_SIZE_T) sizeof(addr_ip));

                                                                    /* Cfg srch remote addr as pkt src  addr.           */
        port_nbr = NET_UTIL_HOST_TO_NET_16(pbuf_hdr->TCP_UDP_PortSrc);
        addr_ip  = NET_UTIL_HOST_TO_NET_32(pbuf_hdr->IP_AddrSrc);

        Mem_Copy((void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_PORT],
                 (void     *)&port_nbr,
                 (CPU_SIZE_T) sizeof(port_nbr));
        Mem_Copy((void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_ADDR],
                 (void     *)&addr_ip,
                 (CPU_SIZE_T) sizeof(addr_ip));

#else                                                               /* See Note #5.                                     */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
       *perr = NET_SOCK_ERR_INVALID_FAMILY;
        return;
#endif


        conn_id = NetConn_Srch((NET_CONN_FAMILY     ) family,       /* Srch for conn'd OR non-conn'd sock.               */
                               (NET_CONN_PROTOCOL_IX) protocol_ix,
                               (CPU_INT08U         *)&addr_local[0],
                               (CPU_INT08U         *)&addr_remote[0],
                               (NET_CONN_ADDR_LEN   ) NET_SOCK_CFG_ADDR_LEN,
                               (NET_CONN_ID        *) 0,
                               (NET_CONN_ID        *)&sock_id,
                               (NET_ERR            *)&err);
        switch (err) {
            case NET_CONN_ERR_CONN_FULL:                            /* Fully-conn'd sock found.                         */
            case NET_CONN_ERR_CONN_FULL_WILDCARD:
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                 conn = DEF_YES;                                    /* Validate full-conn.                              */
#endif
                 break;


            case NET_CONN_ERR_CONN_HALF:                            /* Non-  conn'd sock found.                         */
            case NET_CONN_ERR_CONN_HALF_WILDCARD:
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                 conn = DEF_NO;                                     /* Validate half-conn.                              */
#endif
                 break;


            case NET_CONN_ERR_CONN_NONE:                            /* If NO conn'd sock found, ...                     */
            case NET_CONN_ERR_INVALID_FAMILY:
            case NET_CONN_ERR_INVALID_PROTOCOL_IX:
            case NET_CONN_ERR_INVALID_ADDR:
            case NET_CONN_ERR_INVALID_ADDR_LEN:
            default:
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrRxDestCtr);
                *perr = NET_ERR_RX_DEST;                            /* ... rtn dest err.                                */
                 return;                                            /* Prevent 'break NOT reachable' compiler warning.  */
        }
    }


/*$PAGE*/
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
    used = NetSock_IsUsed(sock_id, &err);
    if (used != DEF_YES) {
        NetSock_CloseConn(conn_id);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockCtr);
       *perr = NET_SOCK_ERR_INVALID_SOCK;
        return;
    }
#else
   (void)&conn_id;                                              /* Prevent possible compiler warning.                   */
#endif

    psock = &NetSock_Tbl[sock_id];

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK CONN ---------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr = NET_SOCK_ERR_NOT_USED;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr = NET_SOCK_ERR_CLOSED;
             break;


        case NET_SOCK_STATE_CLOSED:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
            *perr = NET_SOCK_ERR_INVALID_OP;
             break;


        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_LISTEN:
             if (conn != DEF_NO) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
                *perr = NET_SOCK_ERR_INVALID_OP;
                 return;
             }
             break;


        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
             if (conn != DEF_YES) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
                *perr = NET_SOCK_ERR_INVALID_OP;
                 return;
             }
             break;
             

        case NET_SOCK_STATE_NONE:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr = NET_SOCK_ERR_INVALID_STATE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif
                     

/*$PAGE*/
                                                                /* ---------------- DEMUX PKT TO SOCK ----------------- */
    pbuf_tail = psock->RxQ_Tail;
    if (pbuf_tail != (NET_BUF *)0) {                            /* If sock rx Q NOT empty, insert pkt after tail.       */
        pbuf_hdr_tail                  = &pbuf_tail->Hdr;
        pbuf_hdr_tail->NextPrimListPtr =  pbuf;
#if (NET_SOCK_CFG_TYPE_STREAM_EN       == DEF_ENABLED)
        q_prevly_empty                 =  DEF_NO;
#endif

    } else {                                                    /* Else add first pkt to sock rx Q.                     */
        psock->RxQ_Head                =  pbuf;
#if (NET_SOCK_CFG_TYPE_STREAM_EN       == DEF_ENABLED)
        q_prevly_empty                 =  DEF_YES;
#endif
    }

    psock->RxQ_Tail           = (NET_BUF *)pbuf;                /* Insert pkt @ Q tail.                                 */
    pbuf_hdr->PrevPrimListPtr = (NET_BUF *)pbuf_tail;
#if 0                                                           /* Init'd in NetBuf_Get() [see Note #8].                */
    pbuf_hdr->NextPrimListPtr = (NET_BUF *)0;
#endif


    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             NetOS_Sock_RxQ_Signal(sock_id, perr);              /* Signal rx Q for each datagram pkt (see Note #3a).    */
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             if (q_prevly_empty == DEF_YES) {                   /* If stream rx Q prev'ly empty, ...                    */
                 NetOS_Sock_RxQ_Signal(sock_id, perr);          /* .. signal rx Q now non-empty (see Note #3b).         */
             }
             break;
#endif

        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                /* See Note #6.                                         */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
            *perr = NET_SOCK_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

    if (*perr != NET_SOCK_ERR_NONE) {                           /* If sock rx Q signal failed, unlink      pkt from Q.  */
        pbuf_tail = pbuf_hdr->PrevPrimListPtr;
        if (pbuf_tail != (NET_BUF *)0) {                        /* If sock rx Q NOT yet empty, unlink last pkt from Q.  */
            pbuf_hdr_tail                  = (NET_BUF_HDR *)&pbuf_tail->Hdr;
            pbuf_hdr_tail->NextPrimListPtr = (NET_BUF     *) 0;
        } else {                                                /* Else unlink last pkt from Q.                         */
            psock->RxQ_Head                = (NET_BUF     *) 0;
        }
        psock->RxQ_Tail           = (NET_BUF *)pbuf_tail;       /* Set new sock rx Q tail.                              */
        pbuf_hdr->PrevPrimListPtr = (NET_BUF *)0;
        return;                                                 /* Rtn err from NetOS_Sock_RxQ_Signal().                */
    }


   *perr = NET_SOCK_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetSock_RxPktDiscard()
*
* Description : On any socket receive errors, discard socket packet(s) & buffer(s).
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Rx(),
*               NetSock_RxDataHandlerDatagram().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetSock_RxPktDiscard (NET_BUF  *pbuf,
                                    NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetSock_ErrRxPktDiscardedCtr;
#else
    pctr = (NET_CTR *) 0;
#endif
    NetBuf_FreeBufQ_PrimList((NET_BUF *)pbuf,
                             (NET_CTR *)pctr);

   *perr = NET_ERR_RX;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_IsValidAddrLocal()
*
* Description : (1) Validate a socket address as a local address :
*
*                   (a) Validate socket address family type
*                   (b) Validate socket address
*                   (c) Validate socket port number
*
*
* Argument(s) : paddr       Pointer to socket address structure             (see Notes #2a1B, #2a2, & #3).
*
*               addr_len    Length  of socket address structure (in octets) [see Note  #2a1C].
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket address successfully validated.
*                               NET_SOCK_ERR_NULL_PTR               Argument 'paddr' passed a NULL pointer.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address structure length.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket address family.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid socket address.
*                               NET_SOCK_ERR_INVALID_PORT_NBR       Invalid socket port number.
*
* Return(s)   : DEF_YES, if a valid local socket address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_BindHandler().
*
* Note(s)     : (2) (a) (1) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the bind() 
*                           function takes the following arguments" :
*
*                           (A) 'socket' - "Specifies the file descriptor of the socket to be bound."
*
*                           (B) 'address' - "Points to a 'sockaddr' structure containing the address to be bound 
*                                to the socket.  The length and format of the address depend on the address family 
*                                of the socket."
*
*                           (C) 'address_len' - "Specifies the length of the 'sockaddr' structure pointed to by 
*                                the address argument."
*
*                       (2) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                           Section 4.4, Page 102 states that "if ... bind() is called" with :
*
*                           (A) "A port number of 0, the kernel chooses an ephemeral port."
*
*                           (B) "A wildcard ... address, the kernel does not choose the local ... address."
*
*                               (1) "With IPv4, the wildcard address is specified by the constant INADDR_ANY, 
*                                    whose value is normally 0."
*
*                   (b) (1) IEEE Std 1003.1, 2004 Edition, Section 'bind() : ERRORS' states that "the bind() 
*                           function shall fail if" :
*
*                           (B) "[EAFNOSUPPORT]  - The specified address is not a valid address for the address 
*                                family of the specified socket."
*
*                           (C) "[EADDRNOTAVAIL] - The specified address is not available from the local machine."
*
*                       (2) IEEE Std 1003.1, 2004 Edition, Section 'bind() : ERRORS' states that "the bind() 
*                           function may fail if" :
*
*                           (A) "[EINVAL] - The 'address_len' argument is not a valid length for the address 
*                                family."
*
*                   See also 'NetSock_BindHandler()  Note #2'.
*
*               (3) (a) Socket address structure 'AddrFamily' member MUST be configured in host-order & MUST 
*                       NOT be converted to/from network-order.
*
*                   (b) Socket address structure addresses MUST be configured/converted from host-order to 
*                       network-order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (4) Socket connection addresses are maintained in network-order.
*
*               (5) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be compiled/linked 
*                   since 'net_sock.h' ensures that the family type configuration constant (NET_SOCK_CFG_FAMILY) 
*                   is configured with an appropriate family type value (see 'net_sock.h  CONFIGURATION ERRORS').  
*                   The 'else'-conditional code is included for completeness & as an extra precaution in case 
*                   'net_sock.h' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_IsValidAddrLocal (NET_SOCK_ADDR      *paddr,
                                               NET_SOCK_ADDR_LEN   addr_len,
                                               NET_ERR            *perr)
{
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_SOCK_ADDR_IP      *paddr_ip;
    NET_IP_ADDR            addr_ip;
#endif
    NET_SOCK_ADDR_FAMILY   addr_family;
    NET_PORT_NBR           port_nbr;
    CPU_BOOLEAN            port_nbr_random;
    CPU_BOOLEAN            addr_host;

                                                                /* ------------------- VALIDATE PTR ------------------- */
    if (paddr == (NET_SOCK_ADDR *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNullPtrCtr);
       *perr =  NET_SOCK_ERR_NULL_PTR;
        return (DEF_NO);
    }
                                                                /* ---------------- VALIDATE ADDR LEN ----------------- */
    if (addr_len != NET_SOCK_ADDR_SIZE) {                       /* Validate addr len (see Notes #2a1C & #2b2A).         */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidAddrLenCtr);
       *perr =  NET_SOCK_ERR_INVALID_ADDR_LEN;
        return (DEF_NO);
    }


                                                                /* ------------------ VALIDATE ADDR ------------------- */
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    paddr_ip    = (NET_SOCK_ADDR_IP *)paddr;
                                                                /* Validate addr family (see Note #3a).                 */
    addr_family =  NET_UTIL_VAL_GET_HOST_16(&paddr_ip->AddrFamily);
    if (addr_family != NET_SOCK_ADDR_FAMILY_IP_V4) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
       *perr =  NET_SOCK_ERR_INVALID_FAMILY;
        return (DEF_NO);
    }
                                                                /* Validate addr     (see Note #3b).                    */
    addr_ip = NET_UTIL_VAL_GET_NET_32(&paddr_ip->Addr);
    if (addr_ip  != NET_SOCK_ADDR_IP_WILD_CARD) {               /* If req'd addr NOT wildcard addr (see Note #2a2B) ... */
        addr_host = NetIP_IsAddrHostHandler(addr_ip);
        if (addr_host != DEF_YES) {                             /* ... or any of this host's  addrs,                ... */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidAddrCtr);
           *perr =  NET_SOCK_ERR_INVALID_ADDR;                  /* ... rtn err                     (see Note #2b1C).    */
            return (DEF_NO);
        }
    }
                                                                /* Validate port nbr (see Note #3b).                    */
    port_nbr = NET_UTIL_VAL_GET_NET_16(&paddr_ip->Port);
#if (NET_SOCK_PORT_NBR_RESERVED != NET_SOCK_PORT_NBR_RANDOM)
    if (port_nbr == NET_SOCK_PORT_NBR_RESERVED) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidPortNbrCtr);
       *perr =  NET_SOCK_ERR_INVALID_PORT_NBR;
        return (DEF_NO);
    }
#endif
    port_nbr_random = NetSock_RandomPortNbrChkRange(port_nbr);
    if (port_nbr_random != DEF_NO) {                            /* If port nbr in random port nbr Q, rtn err.           */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidPortNbrCtr);
       *perr =  NET_SOCK_ERR_INVALID_PORT_NBR;
        return (DEF_NO);
    }

#else                                                           /* See Note #5.                                         */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   (void)&addr_family;                                          /* Prevent compiler warnings.                           */
   (void)&port_nbr;
   (void)&addr_host;
   (void)&port_nbr_random;
   *perr =  NET_SOCK_ERR_INVALID_FAMILY;
    return (DEF_NO);
#endif


   *perr =  NET_SOCK_ERR_NONE;

    return (DEF_YES);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_IsValidAddrRemote()
*
* Description : (1) Validate a socket address as an appopriate remote address :
*
*                   (a) Validate remote socket address :
*
*                       (1) Validate the following socket address fields :
*
*                           (A) Validate socket address family type
*                           (B) Validate socket port number
*
*                       (2) Validation ignores the following socket address fields :
*
*                           (A) Address field(s)                    Addresses will be validated by other 
*                                                                       network layers
*
*                   (b) Validate remote socket address to socket's connection address
*
*
* Argument(s) : paddr       Pointer to socket address structure (see Note #2).
*
*               addr_len    Length  of socket address structure (in octets).
*
*               psock       Pointer to socket.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket address successfully validated.
*                               NET_SOCK_ERR_NULL_PTR               Argument 'paddr' passed a NULL pointer.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket address family.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_CONN           Invalid socket connection.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address structure length.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid socket address.
*                               NET_SOCK_ERR_INVALID_PORT_NBR       Invalid socket port number.
*
* Return(s)   : DEF_YES, if a valid remote socket address.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_Conn(),
*               NetSock_TxDataHandler().
*
* Note(s)     : (2) (a) Socket address structure 'AddrFamily' member MUST be configured in host-order & 
*                       MUST NOT be converted to/from network-order.
*
*                   (b) Socket address structure addresses MUST be configured/converted from host-order
*                       to network-order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (3) (a) Socket connection addresses are maintained in network-order.
*
*                   (b) However, since the port number & address are copied from a network-order socket 
*                       address structure into a multi-octet array, they do NOT need to be converted 
*                       from host-order to network-order.
*
*               (4) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*
*               (5) (a) For datagram-type sockets, the remote address is NOT required to be static --
*                       even if the socket is in a connected state.  In other words, any datagram-type 
*                       socket may receive or transmit from or to different remote addresses on each
*                       or any separate socket operation.
*
*                   (b) (1) For stream-type sockets, the remote address MUST be static.  In other words,
*                           a stream-type socket MUST be connected to & use the same remote address for 
*                           ALL socket operations.
*
*                       (2) However, if the socket is NOT yet connected; then any valid remote address 
*                           may be validated for the socket connection.
*
*               (6) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'SockType' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_IsValidAddrRemote (NET_SOCK_ADDR      *paddr,
                                                NET_SOCK_ADDR_LEN   addr_len,
                                                NET_SOCK           *psock,
                                                NET_ERR            *perr)
{
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_SOCK_ADDR_IP      *paddr_ip;
#endif
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
    NET_CONN_ID            conn_id;
    CPU_INT08U             addr[NET_SOCK_CFG_ADDR_LEN];
    CPU_BOOLEAN            cmp;
    NET_ERR                err;
#endif
    NET_SOCK_ADDR_FAMILY   addr_family;
    NET_PORT_NBR           port_nbr;


                                                                /* ------------------- VALIDATE PTR ------------------- */
    if (paddr == (NET_SOCK_ADDR *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNullPtrCtr);
       *perr =  NET_SOCK_ERR_NULL_PTR;
        return (DEF_NO);
    }
                                                                /* ---------------- VALIDATE ADDR LEN ----------------- */
    if (addr_len != NET_SOCK_ADDR_SIZE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidAddrLenCtr);
       *perr =  NET_SOCK_ERR_INVALID_ADDR_LEN;
        return (DEF_NO);
    }


                                                                /* ------------------ VALIDATE ADDR ------------------- */
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    paddr_ip    = (NET_SOCK_ADDR_IP *)paddr;
                                                                /* Validate addr family (see Note #2a).                 */
    addr_family =  NET_UTIL_VAL_GET_HOST_16(&paddr_ip->AddrFamily);
    if (addr_family != NET_SOCK_ADDR_FAMILY_IP_V4) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
       *perr =  NET_SOCK_ERR_INVALID_FAMILY;
        return (DEF_NO);
    }
                                                                /* Validate port nbr    (see Note #2b).                 */
    port_nbr = NET_UTIL_VAL_GET_NET_16(&paddr_ip->Port);
    if (port_nbr == NET_SOCK_PORT_NBR_RESERVED) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidPortNbrCtr);
       *perr =  NET_SOCK_ERR_INVALID_PORT_NBR;
        return (DEF_NO);
    }

#else                                                           /* See Note #4.                                         */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   *perr =  NET_SOCK_ERR_INVALID_FAMILY;
    return (DEF_NO);
#endif


                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    if (psock != (NET_SOCK *)0) {                               /* If sock avail, chk conn status/addr.                 */
        switch (psock->SockType) {
            case NET_SOCK_TYPE_DATAGRAM:
                 switch (psock->State) {
                     case NET_SOCK_STATE_FREE:
                     case NET_SOCK_STATE_DISCARD:
                          NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
                         *perr =  NET_SOCK_ERR_NOT_USED;
                          return (DEF_NO);                      /* Prevent 'break NOT reachable' compiler warning.      */


                     case NET_SOCK_STATE_CLOSED:
                     case NET_SOCK_STATE_BOUND:
                     case NET_SOCK_STATE_CONN:
                                                                /* Remote addr validation NOT req'd (see Note #5a).     */
                          break;


                     case NET_SOCK_STATE_NONE:
                     case NET_SOCK_STATE_CLOSED_FAULT:
                     case NET_SOCK_STATE_LISTEN:
                     case NET_SOCK_STATE_CONN_IN_PROGRESS:
                     case NET_SOCK_STATE_CONN_DONE:
                     case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
                     case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
                     default:
                          NetSock_CloseSock(psock, DEF_YES, DEF_YES);
                          NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
                         *perr =  NET_SOCK_ERR_INVALID_STATE;
                          return (DEF_NO);                      /* Prevent 'break NOT reachable' compiler warning.      */
                 }
                 break;

/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
            case NET_SOCK_TYPE_STREAM:
                 switch (psock->State) {
                     case NET_SOCK_STATE_FREE:
                     case NET_SOCK_STATE_DISCARD:
                          NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
                         *perr =  NET_SOCK_ERR_NOT_USED;
                          return (DEF_NO);                      /* Prevent 'break NOT reachable' compiler warning.      */


                     case NET_SOCK_STATE_CLOSED_FAULT:
                         *perr =  NET_SOCK_ERR_CLOSED;
                          return (DEF_NO);                      /* Prevent 'break NOT reachable' compiler warning.      */
                          

                     case NET_SOCK_STATE_CLOSED:
                     case NET_SOCK_STATE_BOUND:
                     case NET_SOCK_STATE_LISTEN:
                                                                /* Remote addr validation NOT req'd (see Note #5b2).    */
                          break;


                     case NET_SOCK_STATE_CONN:                  /* Validate sock's conn remote addr (see Note #5b1).    */
                     case NET_SOCK_STATE_CONN_IN_PROGRESS:
                     case NET_SOCK_STATE_CONN_DONE:
                     case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
                     case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
                          conn_id = psock->ID_Conn;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                          if (conn_id == NET_CONN_ID_NONE) {
                              NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidConnCtr);
                             *perr =  NET_SOCK_ERR_INVALID_CONN;
                              return (DEF_NO);
                          }
#endif

#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
                                                                /* Cfg cmp addr in net-order (see Note #3).             */
                          Mem_Copy((void     *)&addr[NET_SOCK_ADDR_IP_IX_PORT],
                                   (void     *)&paddr_ip->Port,
                                   (CPU_SIZE_T) sizeof(paddr_ip->Port));
                          Mem_Copy((void     *)&addr[NET_SOCK_ADDR_IP_IX_ADDR],
                                   (void     *)&paddr_ip->Addr,
                                   (CPU_SIZE_T) sizeof(paddr_ip->Addr));

#else                                                           /* See Note #4.                                         */
                          NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
                         *perr =  NET_SOCK_ERR_INVALID_FAMILY;
                          return (DEF_NO);
#endif

                          cmp = NetConn_AddrRemoteCmp((NET_CONN_ID      ) conn_id,
                                                      (CPU_INT08U      *)&addr[0],
                                                      (NET_CONN_ADDR_LEN) NET_SOCK_CFG_ADDR_LEN,
                                                      (NET_ERR         *)&err);
                          if (cmp != DEF_YES) {                 /* If sock's remote addr does NOT cmp, rtn err.         */
                              NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidAddrCtr);
                             *perr =  NET_SOCK_ERR_INVALID_ADDR;
                              return (DEF_NO);
                          }
                          break;


                     case NET_SOCK_STATE_NONE:
                     default:
                          NetSock_CloseSock(psock, DEF_YES, DEF_YES);
                          NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
                         *perr =  NET_SOCK_ERR_INVALID_STATE;
                          return (DEF_NO);                      /* Prevent 'break NOT reachable' compiler warning.      */
                 }
                 break;
#endif

            case NET_SOCK_TYPE_NONE:
            case NET_SOCK_TYPE_FAULT:
            default:                                            /* See Note #6.                                         */
                 NetSock_CloseSock(psock, DEF_YES, DEF_YES);
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
                *perr =  NET_SOCK_ERR_INVALID_TYPE;
                 return (DEF_NO);                               /* Prevent 'break NOT reachable' compiler warning.      */
        }
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (DEF_YES);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetSock_CloseHandlerStream()
*
* Description : (1) Close a stream-type socket :
*
*                   (a) Validate socket connection state
*                   (b) Request transport layer connection close
*                   (c) Wait on transport layer connection close
*                   (d) Close socket
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to close.
*               -------     Argument checked   in NetSock_Close().
*
*               psock       Pointer to socket.
*               -----       Argument validated in NetSock_Close().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                       Socket successfully closed.
*                               NET_SOCK_ERR_NOT_USED                   Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                     Socket already closed.
*                               NET_SOCK_ERR_FAULT                      Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_INVALID_STATE              Invalid socket state.
*                               NET_SOCK_ERR_INVALID_FAMILY             Invalid socket protocol family.
*                               NET_SOCK_ERR_CONN_CLOSE_IN_PROGRESS     Socket close already in progress.
*                               NET_SOCK_ERR_CONN_FAIL                  Socket connection operation(s) failed.
*
*                                                                       - RETURNED BY NetOS_Sock_ConnCloseWait() : -
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT        Socket connection close NOT signaled by
*                                                                           timeout.
*
*                                                                       - RETURNED BY NetConn_ID_TransportGet() : --
*                               NET_ERR_INIT_INCOMPLETE                 Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN               Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED                   Network connection NOT currently used.
*
*                                                                       -------- RETURNED BY NetOS_Lock() : --------
*                               NET_OS_ERR_LOCK                         Network access NOT acquired.
*
* Return(s)   : NET_SOCK_BSD_ERR_NONE,  if NO errors (see Note #3).
*
*               NET_SOCK_BSD_ERR_CLOSE, otherwise.
*
* Caller(s)   : NetSock_Close().
*
* Note(s)     : (2) Network resources MUST be appropriately closed :
*
*                   (a) For the following socket connection close conditions, close ALL socket connections :
*
*                       (1) Non-connected socket states
*                       (2) On any socket          fault(s)
*                       (3) On any transport layer fault(s)
*
*                   (b) #### For connection-closing socket states :
*
*                       (1) Close socket connection
*                       (2) Do NOT close socket    connection's reference to network connection
*                       (3) Do NOT close transport connection(s); transport layer responsible for
*                            closing its remaining connection(s)
*
*                   (c) (1) For the following socket connection close conditions ... :
*
*                           (A) For non-blocking, connected socket states
*                           (B) For connection-closed       socket states
*
*                       (2) ... perform the following close actions :
*
*                           (A) Close socket connection
*                           (B) Close socket connection's reference to network connection
*                           (C) Do NOT close transport connection(s); transport layer responsible for
*                                closing its remaining connection(s)
*
*               (3) NO BSD socket error is returned for any internal error while closing the socket.
*
*               (4) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be compiled/linked
*                   since 'net_sock.h' ensures that the family type configuration constant (NET_SOCK_CFG_FAMILY)
*                   is configured with an appropriate family type value (see 'net_sock.h  CONFIGURATION ERRORS').
*                   The 'else'-conditional code is included for completeness & as an extra precaution in case
*                   'net_sock.h' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_SOCK_RTN_CODE  NetSock_CloseHandlerStream (NET_SOCK_ID   sock_id,
                                                       NET_SOCK     *psock,
                                                       NET_ERR      *perr)
{
    NET_CONN_ID  conn_id;
    NET_CONN_ID  conn_id_transport;
    CPU_BOOLEAN  block;
    NET_ERR      err;


                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (NET_SOCK_BSD_ERR_CLOSE);                   /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:                       /* If already CLOSED from internal fault(s), ...        */
             NetSock_Free(psock);                               /* ... sock need ONLY be freed.                         */
            *perr =  NET_SOCK_ERR_CLOSED;                       /* Rtn net sock err but rtn NO BSD err.                 */
             return (NET_SOCK_BSD_ERR_NONE);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_BOUND:
             NetSock_CloseHandler(psock, DEF_YES, DEF_YES);     /* See Note #2a1.                                       */
            *perr =  NET_SOCK_ERR_NONE;
             return (NET_SOCK_BSD_ERR_NONE);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
             psock->State = NET_SOCK_STATE_CLOSE_IN_PROGRESS;
             break;


        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:                  /* See Note #2b.                                        */
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:                 /* #### Net conn(s) prev'ly closed? (See Note #2b)      */
             NetSock_CloseHandler((NET_SOCK  *)psock,           /* See Note #2b1.                                       */
                                  (CPU_BOOLEAN)DEF_NO,          /* See Note #2b2.                                       */
                                  (CPU_BOOLEAN)DEF_NO);         /* See Note #2b3.                                       */
            *perr =  NET_SOCK_ERR_CONN_CLOSE_IN_PROGRESS;       /* #### Rtn net sock err ...                            */
                                                                /* ...  but rtn NO BSD err (see Note #3).               */
             return (NET_SOCK_BSD_ERR_NONE);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_NONE:
        default:                                                /* See Note #2a2.                                       */
             NetSock_CloseSockFromClose(psock);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr =  NET_SOCK_ERR_INVALID_STATE;                /* Rtn net sock err but rtn NO BSD err (see Note #3).   */
             return (NET_SOCK_BSD_ERR_NONE);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                /* ------------- REQ TRANSPORT CONN CLOSE ------------- */
    conn_id           = psock->ID_Conn;
    conn_id_transport = NetConn_ID_TransportGet(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {                           /* See Note #2a2.                                       */
         NetSock_CloseSockFromClose(psock);
         return (NET_SOCK_BSD_ERR_NONE);
    }
    if  (conn_id_transport == NET_CONN_ID_NONE) {               /* See Note #2a2.                                       */
         NetSock_CloseSockFromClose(psock);
        *perr = NET_SOCK_ERR_CONN_FAIL;
         return (NET_SOCK_BSD_ERR_NONE);
    }



#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
    NetTCP_TxConnReqClose((NET_TCP_CONN_ID) conn_id_transport,
                          (CPU_INT08U     ) NET_CONN_CLOSE_FULL,
                          (NET_ERR       *)&err);
   *perr = (err != NET_TCP_ERR_NONE) ? NET_SOCK_ERR_CONN_FAIL
                                     : NET_SOCK_ERR_NONE;
#else
   (void)&conn_id_transport;                                    /* Prevent compiler warning.                            */
   *perr =  NET_SOCK_ERR_CONN_FAIL;
#endif

#else                                                           /* See Notes #4 & #2a2.                                 */
    NetSock_CloseSockFromClose(psock);
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   *perr =  NET_SOCK_ERR_INVALID_FAMILY;                        /* Rtn net sock err ...                                 */
    return (NET_SOCK_BSD_ERR_NONE);                             /* ... but rtn NO BSD err (see Note #3).                */
#endif

    if (*perr != NET_SOCK_ERR_NONE) {                           /* See Note #2a3.                                       */
         NetSock_CloseSockFromClose(psock);
                                                                /* Rtn net sock err ...                                 */
         return (NET_SOCK_BSD_ERR_NONE);                        /* ... but rtn NO BSD err (see Note #3).                */
    }



                                                                /* ----------- WAIT ON TRANSPORT CONN CLOSE ----------- */
#if   (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_BLOCK)
    block = DEF_YES;
#elif (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_NO_BLOCK)
    block = DEF_NO;
#else
    block = DEF_YES;
#endif

    if (block != DEF_YES) {                                     /* If non-blocking sock conn, ...                       */
        NetSock_CloseHandler((NET_SOCK  *)psock,                /* ... close sock (see Note #2c1A).                     */
                             (CPU_BOOLEAN)DEF_YES,              /* See Note #2c2B.                                      */
                             (CPU_BOOLEAN)DEF_NO);              /* See Note #2c2C.                                      */
       *perr =  NET_SOCK_ERR_NONE;
        return (NET_SOCK_BSD_ERR_NONE);
    }

    NetOS_Unlock();
    NetOS_Sock_ConnCloseWait(sock_id, perr);
    NetOS_Lock(&err);
    if ( err != NET_OS_ERR_NONE) {                              /* See Note #2a2.                                       */
        NetSock_CloseSockFromClose(psock);
       *perr  = err;                                            /* Rtn err from NetOS_Lock() ...                        */
        return (NET_SOCK_BSD_ERR_NONE);                         /* ... but rtn NO BSD err (see Note #3).                */
    }
    switch (*perr) {
        case NET_SOCK_ERR_NONE:
             break;


        case NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT:
             NetSock_CloseSockFromClose(psock);                 /* See Note #2a2.                                       */
                                                                /* Rtn err from NetOS_Sock_ConnCloseWait() ...          */
                                                                /* ... but rtn NO BSD err (see Note #3).                */
             return (NET_SOCK_BSD_ERR_NONE);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_ERR_CONN_SIGNAL_ABORT:
        case NET_SOCK_ERR_CONN_SIGNAL_FAULT:
        default:
             NetSock_CloseSockFromClose(psock);                 /* See Note #2a2.                                       */
            *perr =  NET_SOCK_ERR_FAULT;                        /* Rtn net sock err but rtn NO BSD err (see Note #3).   */
             return (NET_SOCK_BSD_ERR_NONE);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                /* -------------------- CLOSE SOCK -------------------- */
    NetSock_CloseHandler((NET_SOCK  *)psock,                    /* See Note #2c1B.                                      */
                         (CPU_BOOLEAN)DEF_YES,                  /* See Note #2c2B.                                      */
                         (CPU_BOOLEAN)DEF_NO);                  /* See Note #2c2C.                                      */


   *perr =  NET_SOCK_ERR_NONE;

    return (NET_SOCK_BSD_ERR_NONE);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetSock_BindHandler()
*
* Description : (1) Bind a local address to a socket :
*
*                   (a) Handle    socket type connection
*                   (b) Validate  socket local address
*                   (c) Configure socket local address
*                   (c) Search for other socket(s) with same local address              See Note #8b
*                   (d) Add local address into socket connection
*                       (1) Get & configure socket connection, if necessary
*                       (2) Set socket connection local address
*                       (3) Add socket connection into connection list
*                   (e) Update socket connection state
*
*
* Argument(s) : sock_id             Socket descriptor/handle identifier of socket to bind to a local address.
*               -------             Argument checked in NetSock_Bind(),
*                                                       NetSock_ConnHandlerDatagram(),
*                                                       NetSock_ConnHandlerStream(),
*                                                       NetSock_TxDataHandlerDatagram().
*
*               paddr_local         Pointer to socket address structure             (see Notes #2b1B, #2b2, & #3).
*
*               addr_len            Length  of socket address structure (in octets) [see Note  #2b1C].
*
*               addr_random_reqd    Indicate whether a random address is requested  (see Note  #5) :
*
*                                       DEF_NO                      Random address NOT requested.
*                                       DEF_YES                     Random address is  requested.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket successfully bound to local address.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid local address.
*                               NET_SOCK_ERR_ADDR_IN_USE            Local address already in use (see Note #8a).
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*
*                                                                   --- RETURNED BY NetSock_RandomPortNbrGet() : ---
*                               NET_SOCK_ERR_PORT_NBR_NONE_AVAIL    Port number NOT available.
*
*                                                                   - RETURNED BY NetIP_GetAddrThisHostHandler() : -
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host address.
*
*                                                                   --------- RETURNED BY NetConn_Get() : ----------
*                               NET_CONN_ERR_NONE_AVAIL             NO available connections to allocate.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_TYPE           Invalid network connection type.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*
*                                                                   ------ RETURNED BY NetConn_ID_AppSet() : -------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection(s) NOT currently used.
*
*                                                                   ----- RETURNED BY NetConn_AddrLocalSet() : -----
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
*                                                                   ---- RETURNED BY NetConn_AddrRemoteGet() : -----
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
* Return(s)   : NET_SOCK_BSD_ERR_NONE, if NO errors (see Note #2c1).
*
*               NET_SOCK_BSD_ERR_BIND, otherwise    (see Note #2c2A).
*
* Caller(s)   : NetSock_Bind(),
*               NetSock_ConnHandlerDatagram(),
*               NetSock_ConnHandlerStream(),
*               NetSock_TxDataHandlerDatagram().
*$PAGE*
* Note(s)     : (2) (a) (1) IEEE Std 1003.1, 2004 Edition, Section 'bind() : DESCRIPTION' states that "the bind() 
*                           function shall assign a local socket address ... to a socket".
*
*                       (2) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                           Section 4.4, Page 102 states that "bind() lets us specify the ... address, the port, 
*                           both, or neither".
*
*                   (b) (1) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the bind() 
*                           function takes the following arguments" :
*
*                           (A) 'socket' - "Specifies the file descriptor of the socket to be bound."
*
*                           (B) 'address' - "Points to a 'sockaddr' structure containing the address to be bound 
*                                to the socket.  The length and format of the address depend on the address family 
*                                of the socket."
*
*                           (C) 'address_len' - "Specifies the length of the 'sockaddr' structure pointed to by 
*                                the address argument."
*
*                       (2) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                           Section 4.4, Page 102 states that "if ... bind() is called" with :
*
*                           (A) "A port number of 0, the kernel chooses an ephemeral port."
*
*                               (1) "bind() does not return the chosen value ... [of] an ephemeral port ... Call 
*                                    getsockname() to return the protocol address ... to obtain the value of the 
*                                    ephemeral port assigned by the kernel."
*
*                           (B) "A wildcard ... address, the kernel does not choose the local ... address until 
*                                either the socket is connected (TCP) or a datagram is sent on the socket (UDP)."
*
*                               (1) "With IPv4, the wildcard address is specified by the constant INADDR_ANY, 
*                                    whose value is normally 0."
*
*                   (c) IEEE Std 1003.1, 2004 Edition, Section 'bind() : RETURN VALUE' states that :
*
*                       (1) "Upon successful completion, bind() shall return 0;" ...
*
*                       (2) (A) "Otherwise, -1 shall be returned," ...
*                           (B) "and 'errno' shall be set to indicate the error."
*                               #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   (d) (1) IEEE Std 1003.1, 2004 Edition, Section 'bind() : ERRORS' states that "the bind() 
*                           function shall fail if" :
*
*                           (A) "[EBADF] - The 'socket' argument is not a valid file descriptor."
*
*                           (B) "[EAFNOSUPPORT]  - The specified address is not a valid address for the address 
*                                family of the specified socket."
*
*                           (C) "[EADDRNOTAVAIL] - The specified address is not available from the local machine."
*
*                           (D) "[EADDRINUSE]    - The specified address is already in use."
*
*                               See also Note #8a.
*
*                           (E) "[EINVAL]" - 
*
*                               (1) (a) "The socket is already bound to an address,"                  ...
*                                   (b) "and the protocol does not support binding to a new address;" ...
*
*                               (2) "or the socket has been shut down."
*
*                       (2) IEEE Std 1003.1, 2004 Edition, Section 'bind() : ERRORS' states that "the bind() 
*                           function may fail if" :
*
*                           (A) "[EINVAL]  - The 'address_len' argument is not a valid length for the address 
*                                family."
*
*                           (B) "[EISCONN] - The socket is already connected."
*
*                           (C) "[ENOBUFS] - Insufficient resources were available to complete the call."
*$PAGE*
*               (3) (a) Socket connection addresses MUST be maintained in network-order.
*
*                   (b) However, since the port number & address are copied from a network-order socket address
*                       structure into local variables & then into a multi-octet array, they do NOT need to be
*                       converted from host-order to network-order.
*
*               (4) (a) For datagram-type sockets, the local & remote addresses are NOT required to be static --
*                       even if the socket is in a "connected" state.  In other words, any datagram-type socket 
*                       may bind to different local addresses on each or any separate socket operation.
*
*                   (b) For stream-type sockets, the local & remote addresses MUST be static.  In other words, 
*                       a stream-type socket may bind only once to a single local address.
*
*               (5) If a random local address is requested, configure the local address with ...
*
*                   (a) A random port number obtained from the random port number queue; ...
*                   (b) This host's protocol address.
*
*               (6) (a) Default case already invalidated in NetSock_Open().  However, the default case is included
*                       as an extra precaution in case 'SockType' is incorrectly modified.
*
*                   (b) Default case already invalidated in NetSock_Open().  However, the default case is included
*                       as an extra precaution in case 'Protocol' is incorrectly modified.
*
*               (7) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be compiled/linked
*                   since 'net_sock.h' ensures that the family type configuration constant (NET_SOCK_CFG_FAMILY)
*                   is configured with an appropriate family type value (see 'net_sock.h  CONFIGURATION ERRORS').
*                   The 'else'-conditional code is included for completeness & as an extra precaution in case
*                   'net_sock.h' is incorrectly modified.
*
*               (8) (a) (1) Multiple socket connections with the same local & remote address -- both 
*                           addresses & port numbers -- is NOT currently supported.
*
*                       (2) Therefore when updating a socket connection, it is necessary to search the 
*                           connection lists for any other connection with the same local & remote address.
*
*                       (3) Since datagram-type sockets' remote address is NOT required to be static, 
*                           datagram-type sockets in a "connected" state MUST search the connection lists 
*                           for a connection with the same local & remote address.
*
*                           See also 'NetSock_ConnHandlerDatagram()  Note #2b'.
*
*                   (b) (1) Also, multiple socket connections with only a local address but the same local 
*                           address -- both address & port number -- is NOT currently supported.
*
*                       (2) Therefore when adding or updating a socket connection with only a local address, 
*                           it is necessary to search the connection lists for any other connection with the 
*                           same local address.
*
*                       (3) Thus, the option for sockets to reuse the same local address is NOT currently 
*                           supported even if the socket reuse option (SO_REUSEADDR) is requested.
*
*                           See 'net_sock.c  Note #1d1'.
*
*                   See also 'NetSock_ConnHandlerAddrRemoteValidate()  Note #5'.
*********************************************************************************************************
*/

static  NET_SOCK_RTN_CODE  NetSock_BindHandler (NET_SOCK_ID         sock_id,
                                                NET_SOCK_ADDR      *paddr_local,
                                                NET_SOCK_ADDR_LEN   addr_len,
                                                CPU_BOOLEAN         addr_random_reqd,
                                                NET_ERR            *perr)
{
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_SOCK_ADDR_IP      *paddr_ip;
    NET_IP_ADDR            addr_ip_host;
    NET_IP_ADDR            addr_ip_net;
#endif
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    CPU_BOOLEAN            valid;
#endif
    CPU_BOOLEAN            conn_avail;
    CPU_BOOLEAN            addr_over_wr;
    CPU_BOOLEAN            port_random_reqd;
    NET_PORT_NBR           port_nbr_host;
    NET_PORT_NBR           port_nbr_net;
    CPU_INT08U             addr_local[NET_SOCK_CFG_ADDR_LEN];
    CPU_INT08U             addr_remote[NET_SOCK_CFG_ADDR_LEN];
    NET_SOCK_ADDR_LEN      addr_remote_len;
    CPU_INT08U            *paddr_remote;
    NET_SOCK              *psock;
    NET_SOCK_STATE         sock_state;
    NET_CONN_FAMILY        conn_family;
    NET_CONN_PROTOCOL_IX   conn_protocol_ix;
    NET_CONN_ID            conn_id;
    NET_CONN_ID            conn_id_srch;
    NET_ERR                err;


/*$PAGE*/
                                                                /* ----------------- HANDLE SOCK TYPE ----------------- */
    psock = &NetSock_Tbl[sock_id];
    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:                            /* See Note #4a.                                        */
             switch (psock->State) {
                 case NET_SOCK_STATE_FREE:
                 case NET_SOCK_STATE_DISCARD:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
                     *perr =  NET_SOCK_ERR_NOT_USED;
                      return (NET_SOCK_BSD_ERR_BIND);           /* Prevent 'break NOT reachable' compiler warning.      */


                 case NET_SOCK_STATE_CLOSED_FAULT:
                     *perr =  NET_SOCK_ERR_CLOSED;
                      return (NET_SOCK_BSD_ERR_BIND);           /* Prevent 'break NOT reachable' compiler warning.      */


                 case NET_SOCK_STATE_CLOSED:
                      conn_avail      =  DEF_NO;
                      addr_over_wr    =  DEF_NO;
                      paddr_remote    = (CPU_INT08U *)0;
                      sock_state      =  NET_SOCK_STATE_BOUND;
                      break;


                 case NET_SOCK_STATE_BOUND:
                      conn_avail      =  DEF_YES;
                      addr_over_wr    =  DEF_YES;
                      paddr_remote    = (CPU_INT08U *)0;
                      sock_state      =  NET_SOCK_STATE_BOUND;
                      break;


                 case NET_SOCK_STATE_CONN:
                      conn_avail      =  DEF_YES;
                      addr_over_wr    =  DEF_YES;
                                                                /* Get sock's remote addr.                              */
                      conn_id         =  psock->ID_Conn;
                      addr_remote_len =  sizeof(addr_remote);
                      NetConn_AddrRemoteGet((NET_CONN_ID        ) conn_id,
                                            (CPU_INT08U        *)&addr_remote[0],
                                            (NET_CONN_ADDR_LEN *)&addr_remote_len,
                                            (NET_ERR           *) perr);
                      if (*perr != NET_CONN_ERR_NONE) {
                           return (NET_SOCK_BSD_ERR_BIND);
                      }
                      paddr_remote = &addr_remote[0];

                      sock_state   =  NET_SOCK_STATE_CONN;
                      break;
                      

                 case NET_SOCK_STATE_NONE:
                 case NET_SOCK_STATE_LISTEN:
                 case NET_SOCK_STATE_CONN_IN_PROGRESS:
                 case NET_SOCK_STATE_CONN_DONE:
                 case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
                 case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
                 default:
                      NetSock_CloseSock(psock, DEF_YES, DEF_YES);
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
                     *perr =  NET_SOCK_ERR_INVALID_STATE;
                      return (NET_SOCK_BSD_ERR_BIND);           /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;

/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:                              /* See Note #4b.                                        */
             switch (psock->State) {
                 case NET_SOCK_STATE_FREE:
                 case NET_SOCK_STATE_DISCARD:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
                     *perr =  NET_SOCK_ERR_NOT_USED;
                      return (NET_SOCK_BSD_ERR_BIND);           /* Prevent 'break NOT reachable' compiler warning.      */


                 case NET_SOCK_STATE_CLOSED_FAULT:
                     *perr =  NET_SOCK_ERR_CLOSED;
                      return (NET_SOCK_BSD_ERR_BIND);           /* Prevent 'break NOT reachable' compiler warning.      */
                      

                 case NET_SOCK_STATE_CLOSED:
                      conn_avail   =  DEF_NO;
                      addr_over_wr =  DEF_NO;
                      paddr_remote = (CPU_INT08U *)0;
                      sock_state   =  NET_SOCK_STATE_BOUND;
                      break;


                 case NET_SOCK_STATE_BOUND:                     /* See Note #2d1E1.                                     */
                 case NET_SOCK_STATE_LISTEN:
                 case NET_SOCK_STATE_CONN:                      /* See Note #2d2B.                                      */
                 case NET_SOCK_STATE_CONN_IN_PROGRESS:
                 case NET_SOCK_STATE_CONN_DONE:
                 case NET_SOCK_STATE_CLOSE_IN_PROGRESS:         /* See Note #2d1E2.                                     */
                 case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
                     *perr =  NET_SOCK_ERR_INVALID_OP;
                      return (NET_SOCK_BSD_ERR_BIND);           /* Prevent 'break NOT reachable' compiler warning.      */
                      

                 case NET_SOCK_STATE_NONE:
                 default:
                      NetSock_CloseSock(psock, DEF_YES, DEF_YES);
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
                     *perr =  NET_SOCK_ERR_INVALID_STATE;
                      return (NET_SOCK_BSD_ERR_BIND);           /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;
#endif

        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                /* See Note #6a.                                        */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
            *perr =  NET_SOCK_ERR_INVALID_TYPE;
             return (NET_SOCK_BSD_ERR_BIND);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* --------------- VALIDATE LOCAL ADDR ---------------- */
    if (addr_random_reqd != DEF_YES) {                          /* If random addr NOT req'd, ...                        */
                                                                /* ... validate local addr (see Note #2b1B).            */
        valid = NetSock_IsValidAddrLocal(paddr_local, addr_len, perr);
        if (valid != DEF_YES) {
           *perr   = NET_SOCK_ERR_INVALID_ADDR;
            return  (NET_SOCK_BSD_ERR_BIND);
        }
    }
#endif


                                                                /* ----------------- CFG LOCAL ADDR ------------------- */
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    if (addr_random_reqd != DEF_YES) {                          /* If random addr NOT req'd, cfg req'd local addr.      */
        paddr_ip = (NET_SOCK_ADDR_IP *)paddr_local;
        NET_UTIL_VAL_COPY_16(&port_nbr_net, &paddr_ip->Port);   /* Copy preserves net-order  (see Note #3b).            */
        NET_UTIL_VAL_COPY_32(&addr_ip_net,  &paddr_ip->Addr);   /* Copy preserves net-order  (see Note #3b).            */
                                                                /* Chk random port nbr req'd (see Note #2b2A).          */
        port_nbr_host    =  NET_UTIL_NET_TO_HOST_16(port_nbr_net);
        port_random_reqd = (port_nbr_host == NET_SOCK_PORT_NBR_RANDOM) ? DEF_YES : DEF_NO;

    } else {                                                    /* Else cfg random port/this host's addr (see Note #5). */
        port_random_reqd =  DEF_YES;
        addr_ip_host     =  NetIP_GetAddrThisHostHandler(perr);
        if (*perr != NET_IP_ERR_NONE) {
             return (NET_SOCK_BSD_ERR_BIND);
        }
        addr_ip_net      =  NET_UTIL_HOST_TO_NET_32(addr_ip_host);
    }

#else                                                           /* See Note #7.                                         */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   *perr =  NET_SOCK_ERR_INVALID_FAMILY;
    return (NET_SOCK_BSD_ERR_BIND);
#endif

    if (port_random_reqd == DEF_YES) {                          /* If random port req'd, ...                            */
        port_nbr_host = NetSock_RandomPortNbrGet(perr);         /* ... get random port nbr (see Note #5a).              */
        if (*perr != NET_SOCK_ERR_NONE) {
             return (NET_SOCK_BSD_ERR_BIND);
        }
        port_nbr_net  = NET_UTIL_HOST_TO_NET_16(port_nbr_host);
    }



/*$PAGE*/
                                                                /* ------- SRCH FOR LOCAL ADDR IN CONN LIST(S) -------- */
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
                                                                /* Cfg local addr as net-order sock addr (see Note #3). */
    Mem_Copy((void     *)&addr_local[NET_SOCK_ADDR_IP_IX_PORT],
             (void     *)&port_nbr_net,
             (CPU_SIZE_T) sizeof(port_nbr_net));
    Mem_Copy((void     *)&addr_local[NET_SOCK_ADDR_IP_IX_ADDR],
             (void     *)&addr_ip_net,
             (CPU_SIZE_T) sizeof(addr_ip_net));

                                                                /* Cfg conn srch.                                       */
    conn_family = NET_CONN_FAMILY_IP_V4_SOCK;
    switch (psock->Protocol) {
        case NET_SOCK_PROTOCOL_UDP:
             conn_protocol_ix = NET_CONN_PROTOCOL_IX_IP_V4_UDP;
             break;


#ifdef  NET_TCP_MODULE_PRESENT
        case NET_SOCK_PROTOCOL_TCP:
             conn_protocol_ix = NET_CONN_PROTOCOL_IX_IP_V4_TCP;
             break;
#endif

        case NET_SOCK_PROTOCOL_NONE:
        default:                                                /* See Note #6b.                                        */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
            *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;
             return (NET_SOCK_BSD_ERR_BIND);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }

#else                                                           /* See Note #7.                                         */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   *perr =  NET_SOCK_ERR_INVALID_FAMILY;
    return (NET_SOCK_BSD_ERR_BIND);
#endif

                                                                /* Srch for sock conn with identical ...                */
                                                                /* ... local &/or remote addrs (see Note #8).           */
    conn_id_srch = NetConn_Srch((NET_CONN_FAMILY     ) conn_family,
                                (NET_CONN_PROTOCOL_IX) conn_protocol_ix,
                                (CPU_INT08U         *)&addr_local[0],
                                (CPU_INT08U         *)paddr_remote,
                                (NET_CONN_ADDR_LEN   ) NET_SOCK_CFG_ADDR_LEN,
                                (NET_CONN_ID        *) 0,
                                (NET_CONN_ID        *) 0,
                                (NET_ERR            *)&err);
    switch (err) {
        case NET_CONN_ERR_CONN_NONE:                            /* NO sock with identical local &/or addrs found.       */
        case NET_CONN_ERR_CONN_HALF_WILDCARD:
        case NET_CONN_ERR_CONN_FULL_WILDCARD:
             break;


        case NET_CONN_ERR_CONN_HALF:                            /* If  half- conn'd sock found                ...       */
             if (paddr_remote != (CPU_INT08U *)0) {             /* ... but remote addr avail (see Note #8b2), ...       */
                 break;                                         /* ... allow valid bind.                                */
             }
                                                                /* 'break' intentionally omitted; do NOT move from the  */
                                                                /* ... following case : 'NET_CONN_ERR_CONN_FULL'.       */

        case NET_CONN_ERR_CONN_FULL:                            /* If  fully conn'd sock found; ...                     */
             if (conn_id_srch == psock->ID_Conn) {              /* ... but = sock's conn,       ...                     */
                *perr =  NET_SOCK_ERR_NONE;
                 return (NET_SOCK_BSD_ERR_NONE);                /* ... rtn valid bind;          ...                     */
             }
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidAddrInUseCtr);
            *perr =  NET_SOCK_ERR_ADDR_IN_USE;                  /* ... else rtn err (see Note #8a).                     */
             return (NET_SOCK_BSD_ERR_BIND);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_CONN_ERR_INVALID_ADDR:
        case NET_CONN_ERR_INVALID_ADDR_LEN:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidAddrCtr);
            *perr =  NET_SOCK_ERR_CONN_FAIL;
             return (NET_SOCK_BSD_ERR_BIND);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }


/*$PAGE*/
                                                                /* ----------- ADD LOCAL ADDR TO SOCK CONN ------------ */
    if (conn_avail != DEF_YES) {                                /* If NO conn prev'ly avail, get/cfg sock conn.         */
        conn_id = NetConn_Get(conn_family, conn_protocol_ix, perr);
        if (*perr != NET_CONN_ERR_NONE) {
             return (NET_SOCK_BSD_ERR_BIND);
        }

        psock->ID_Conn = conn_id;                               /* Set sock's conn id.                                  */
        NetConn_ID_AppSet((NET_CONN_ID)conn_id,
                          (NET_CONN_ID)psock->ID,
                          (NET_ERR   *)perr);
        if (*perr != NET_CONN_ERR_NONE) {
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             return (NET_SOCK_BSD_ERR_BIND);
        }

    } else {                                                    
        conn_id = psock->ID_Conn;                               /* Else get sock's conn id.                             */
        NetSock_FreeAddr(psock);                                /* Free sock's prev  addr.                              */
    }

                                                                /* Set  sock's local addr.                              */
    NetConn_AddrLocalSet((NET_CONN_ID      ) conn_id,
                         (CPU_INT08U      *)&addr_local[0],
                         (NET_CONN_ADDR_LEN) NET_SOCK_CFG_ADDR_LEN,
                         (CPU_BOOLEAN      ) addr_over_wr,
                         (NET_ERR         *) perr);
    if (*perr != NET_CONN_ERR_NONE) {
         NetSock_CloseSock(psock, DEF_YES, DEF_YES);
         return (NET_SOCK_BSD_ERR_BIND);
    }


    NetConn_ListUnlink(conn_id, &err);                          /* Unlink sock conn from conn list, if necessary.       */
    if ( err != NET_CONN_ERR_NONE) {
        NetSock_CloseSock(psock, DEF_YES, DEF_YES);
       *perr  = NET_SOCK_ERR_CONN_FAIL;
        return (NET_SOCK_BSD_ERR_BIND);
    }

    NetConn_ListAdd(conn_id, &err);                             /* Add    sock conn into conn list (see Note #8b).      */
    if ( err != NET_CONN_ERR_NONE) {
        NetSock_CloseSock(psock, DEF_YES, DEF_YES);
       *perr  = NET_SOCK_ERR_CONN_FAIL;
        return (NET_SOCK_BSD_ERR_BIND);
    }


                                                                /* -------------- UPDATE SOCK CONN STATE -------------- */
    psock->State = sock_state;


   *perr =  NET_SOCK_ERR_NONE;

    return (NET_SOCK_BSD_ERR_NONE);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetSock_ConnHandlerDatagram()
*
* Description : (1) Connect a datagram-type socket to a remote address :
*
*                   (a) Validate socket connection state
*                   (b) Validate remote address for socket connection
*                   (c) Prepare  socket for remote connection
*                       (1) Bind to local address, if necessary
*                   (d) Add remote address into socket connection
*                   (e) Update socket connection state
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to connect.
*               -------         Argument checked   in NetSock_Conn().
*
*               psock           Pointer to socket.
*               -----           Argument validated in NetSock_Conn().
*
*               paddr_remote    Pointer to socket address structure.
*               ------------    Argument checked   in NetSock_Conn().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket successfully connected.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*
*                                                                   - RETURNED BY NetSock_ConnHandlerAddrRemoteValidate() : -
*                                                                   --- RETURNED BY NetSock_ConnHandlerAddrRemoteSet() : ----
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address structure length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*                               NET_SOCK_ERR_CONN_IN_USE            Socket connection already in use.
*
*                                                                   ---------- RETURNED BY NetSock_BindHandler() : ----------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid socket address.
*                               NET_SOCK_ERR_ADDR_IN_USE            Socket address already in use.
*                               NET_SOCK_ERR_PORT_NBR_NONE_AVAIL    Port number NOT available.
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host address.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_NONE_AVAIL             NO available connections to allocate.
*                               NET_CONN_ERR_NOT_USED               Network connection(s) NOT currently used.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_TYPE           Invalid network connection type.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
* Return(s)   : NET_SOCK_BSD_ERR_NONE, if NO errors.
*
*               NET_SOCK_BSD_ERR_CONN, otherwise.
*
* Caller(s)   : NetSock_Conn().
*
* Note(s)     : (2) (a) For datagram-type sockets, the remote address does NOT require any connection.  The
*                       pseudo-connection provides a remote address to allow datagram-type sockets to use
*                       stream-type sockets' send & receive functions -- NetSock_RxData() & NetSock_TxData().
*
*                   (b) In addition, the remote address is NOT required to be static -- even if the socket 
*                       is in a "connected" state.  In other words, any datagram-type socket may "connect" 
*                       to different remote addresses on each or any separate socket operation.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_SOCK_RTN_CODE  NetSock_ConnHandlerDatagram (NET_SOCK_ID     sock_id,
                                                        NET_SOCK       *psock,
                                                        NET_SOCK_ADDR  *paddr_remote,
                                                        NET_ERR        *perr)
{
    CPU_BOOLEAN  addr_remote_validate;


                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */
                      

        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_BOUND:
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
             addr_remote_validate = DEF_YES;
#else
             addr_remote_validate = DEF_NO;
#endif
             break;


        case NET_SOCK_STATE_CONN:                               /* See Note #2b.                                        */
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
             addr_remote_validate = DEF_YES;
             break;


        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr =  NET_SOCK_ERR_INVALID_STATE;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ---------- VALIDATE SOCK CONN REMOTE ADDR ---------- */
    if (addr_remote_validate == DEF_YES) {
        NetSock_ConnHandlerAddrRemoteValidate(psock, paddr_remote, perr);
        if (*perr != NET_SOCK_ERR_NONE) {
             return (NET_SOCK_BSD_ERR_CONN);
        }
    }


                                                                /* ----------- PREPARE SOCK FOR REMOTE CONN ----------- */
    if (psock->State == NET_SOCK_STATE_CLOSED) {                /* If sock closed, bind to random local addr.           */
       (void)NetSock_BindHandler((NET_SOCK_ID      )sock_id,
                                 (NET_SOCK_ADDR   *)0,
                                 (NET_SOCK_ADDR_LEN)0,
                                 (CPU_BOOLEAN      )DEF_YES,
                                 (NET_ERR         *)perr);
        if (*perr != NET_SOCK_ERR_NONE) {
             return (NET_SOCK_BSD_ERR_CONN);
        }
    }

                                                                /* ----------- ADD REMOTE ADDR TO SOCK CONN ----------- */
                                                                /* Set sock's remote addr.                              */
    NetSock_ConnHandlerAddrRemoteSet(psock, paddr_remote, DEF_YES, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_CONN);
    }


                                                                /* -------------- UPDATE SOCK CONN STATE -------------- */
    psock->State = NET_SOCK_STATE_CONN;


   *perr =  NET_SOCK_ERR_NONE;

    return (NET_SOCK_BSD_ERR_NONE);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_ConnHandlerStream()
*
* Description : (1) Connect a stream-type socket to a remote address :
*
*                   (a) Validate socket connection state                                See Note #4
*                   (b) Validate remote address for socket connection
*                   (c) Prepare  socket for remote connection
*                       (1) Bind to local address, if necessary
*
*                   (d) Add remote address into socket connection
*                   (e) Initiate transport layer connection
*                       (1) Get      transport connection
*                       (2) Transmit transport connection request
*                       (3) Wait on  transport connection to connect
*
*                   (f) Update socket connection state
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to connect.
*               -------         Argument checked   in NetSock_Conn().
*
*               psock           Pointer to socket.
*               -----           Argument validated in NetSock_Conn().
*
*               paddr_remote    Pointer to socket address structure.
*               ------------    Argument checked   in NetSock_Conn().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket successfully connected.
*                               NET_SOCK_ERR_CONN_IN_PROGRESS       Socket connection in progress.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address length.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid socket address.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*
*                                                                   --------- RETURNED BY NetConn_AddrLocalGet() : ----------
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection(s) NOT currently used.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
*                                                                   - RETURNED BY NetSock_ConnHandlerAddrRemoteValidate() : -
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address structure length.
*                               NET_SOCK_ERR_CONN_IN_USE            Socket connection already in use.
*
*                                                                   ---------- RETURNED BY NetSock_BindHandler() : ----------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid socket address.
*                               NET_SOCK_ERR_ADDR_IN_USE            Socket address already in use.
*                               NET_SOCK_ERR_PORT_NBR_NONE_AVAIL    Port number NOT available.
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host address.
*                               NET_CONN_ERR_NONE_AVAIL             NO available connections to allocate.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_TYPE           Invalid network connection type.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
*                                                                   -------- RETURNED BY NetOS_Sock_ConnReqWait() : ---------
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT    Socket connection request NOT signaled by timeout.
*
*                                                                   -------------- RETURNED BY NetOS_Lock() : ---------------
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*
* Return(s)   : NET_SOCK_BSD_ERR_NONE, if NO errors.
*
*               NET_SOCK_BSD_ERR_CONN, otherwise.
*
* Caller(s)   : NetSock_Conn().
*$PAGE*
* Note(s)     : (2) (a) For stream-type sockets, the remote address MUST be static.  In other words, 
*                       a stream-type socket MUST be connected to & use the same remote address for 
*                       ALL socket operations.
*
*                   (b) In addition, the socket MUST be connected to the remote address PRIOR to any  
*                       data transmit or receive operation.
*
*                   (c) Stream-type sockets may connect to remote addresses from the following states :
*
*                       (1) CLOSED
*                       (2) BOUND                                          See Note #2d
*                       (3) LISTEN                                         See Note #2d
*
*                   (d) Stream-type sockets MUST be bound to a valid local address that is NOT a 
*                       protocol's wildcard address.
*
*               (3) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*
*               (4) Socket descriptor write availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor write handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also  Note #1a,
*                            'NetSock_SelDescHandlerWrStream()   Note #3',
*                          & 'NetSock_SelDescHandlerErrStream()  Note #3'.
*
*               (5) (a) (1) RFC #1122, Section 4.2.4.2 states that :
*
*                           (A) "The application layer MUST be able to specify the Type-of-Service 
*                                (TOS) for segments that are sent on a connection."
*
*                           (B) "It not required [sic], but the application SHOULD be able to change 
*                                the TOS during the connection lifetime."
*
*                       (2) RFC #1122, Section 4.2.2.19 states that "the TTL value used to send ...
*                           segments MUST be configurable".
*
*                       (3) RFC #1122, Section 4.2.3.8 states that :
*
*                           (A) "An application MUST be able to specify a source route when it 
*                                actively opens a TCP connection, and this MUST take precedence 
*                                over a source route received in a datagram."
*
*                           (B) (1) "When a ... connection is OPENed passively and a packet arrives 
*                                    with a completed IP Source Route option (containing a return 
*                                    route), ... all segments sent on this connection ... MUST save 
*                                    ... and use ... [this] return route."
*
*                               (2) "If a different source route arrives in a later segment, the 
*                                    later definition SHOULD override the earlier one."
*
*                   (b) #### IP layer parameters/options NOT yet implemented.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_SOCK_RTN_CODE  NetSock_ConnHandlerStream (NET_SOCK_ID     sock_id,
                                                      NET_SOCK       *psock,
                                                      NET_SOCK_ADDR  *paddr_remote,
                                                      NET_ERR        *perr)
{
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_IP_ADDR        addr_src;
    CPU_INT08U         addr_local[NET_SOCK_CFG_ADDR_LEN];
    NET_SOCK_ADDR_LEN  addr_len;
#endif
    NET_CONN_ID        conn_id;
    NET_CONN_ID        conn_id_transport;
    NET_SOCK_STATE     sock_state;
    CPU_BOOLEAN        block;
    NET_ERR            err;


/*$PAGE*/
                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED:                             /* See Note #2c1.                                       */
             break;


        case NET_SOCK_STATE_BOUND:                              /* Chk valid local addr (see Note #2d).                 */
        case NET_SOCK_STATE_LISTEN:
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
             conn_id  = psock->ID_Conn;
             addr_len = sizeof(addr_local);
             NetConn_AddrLocalGet((NET_CONN_ID        ) conn_id,
                                  (CPU_INT08U        *)&addr_local[0],
                                  (NET_CONN_ADDR_LEN *)&addr_len,
                                  (NET_ERR           *) perr);
             if (*perr != NET_CONN_ERR_NONE) {
                  return (NET_SOCK_BSD_ERR_CONN);
             }
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
             if (addr_len != NET_SOCK_CFG_ADDR_LEN) {
                 NetSock_CloseSock(psock, DEF_YES, DEF_YES);
                *perr =  NET_SOCK_ERR_INVALID_ADDR_LEN;
                 return (NET_SOCK_BSD_ERR_CONN);
             }
#endif
             Mem_Copy((void     *)&addr_src,
                      (void     *)&addr_local[NET_SOCK_ADDR_IP_IX_ADDR],
                      (CPU_SIZE_T) sizeof(addr_src));

             addr_src = NET_UTIL_NET_TO_HOST_32(addr_src);
             if (addr_src == NET_SOCK_ADDR_IP_WILD_CARD) {      /* If wildcard addr, ...                                */
                *perr =  NET_SOCK_ERR_INVALID_ADDR;             /* ... rtn invalid addr.                                */
                 return (NET_SOCK_BSD_ERR_CONN);
             }
             break;

#else                                                           /* See Note #3.                                         */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
            *perr =  NET_SOCK_ERR_INVALID_FAMILY;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */
#endif
             

        case NET_SOCK_STATE_CONN_IN_PROGRESS:
            *perr =  NET_SOCK_ERR_CONN_IN_PROGRESS;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CONN_DONE:
             psock->State = NET_SOCK_STATE_CONN;
            *perr         = NET_SOCK_ERR_NONE;
             return (NET_SOCK_BSD_ERR_NONE);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
            *perr =  NET_SOCK_ERR_INVALID_OP;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_NONE:
        default:
            (void)&conn_id;                                     /* Prevent possible compiler warning.                   */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr =  NET_SOCK_ERR_INVALID_STATE;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------- VALIDATE SOCK CONN REMOTE ADDR ---------- */
    NetSock_ConnHandlerAddrRemoteValidate(psock, paddr_remote, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_CONN);
    }
#endif



                                                                /* ----------- PREPARE SOCK FOR REMOTE CONN ----------- */
    if (psock->State == NET_SOCK_STATE_CLOSED) {                /* If sock closed, bind to random local addr.           */
       (void)NetSock_BindHandler((NET_SOCK_ID      )sock_id,
                                 (NET_SOCK_ADDR   *)0,
                                 (NET_SOCK_ADDR_LEN)0,
                                 (CPU_BOOLEAN      )DEF_YES,
                                 (NET_ERR         *)perr);
        if (*perr != NET_SOCK_ERR_NONE) {
             return (NET_SOCK_BSD_ERR_CONN);
        }
    }


                                                                /* ----------- ADD REMOTE ADDR TO SOCK CONN ----------- */
                                                                /* Set sock's remote addr.                              */
    NetSock_ConnHandlerAddrRemoteSet(psock, paddr_remote, DEF_NO, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_CONN);
    }



/*$PAGE*/
                                                                /* ---------------- GET TRANSPORT CONN ---------------- */
    conn_id_transport = NetSock_GetConnTransport(psock, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetSock_CloseSock(psock, DEF_YES, DEF_YES);
         return (NET_SOCK_BSD_ERR_CONN);
    }



                                                                /* --------------- INIT TRANSPORT CONN ---------------- */
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
    NetTCP_TxConnReq((NET_TCP_CONN_ID) conn_id_transport,
                     (NET_IP_TOS     ) NET_IP_TOS_DFLT,         /* #### See Note #5b.                                   */
                     (NET_IP_TTL     ) NET_IP_TTL_DFLT,
                     (CPU_INT16U     ) NET_IP_FLAG_NONE,
                     (void          *) 0,
                     (NET_ERR       *)&err);
    switch (err) {
        case NET_TCP_ERR_NONE:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case NET_TCP_ERR_CONN_NOT_USED:
        case NET_TCP_ERR_CONN_FAULT:
        case NET_TCP_ERR_CONN_CLOSE:
        case NET_TCP_ERR_INVALID_CONN:
        case NET_TCP_ERR_INVALID_CONN_STATE:
        case NET_TCP_ERR_TX:
            *perr = NET_SOCK_ERR_FAULT;
             break;


        default:
            *perr = NET_ERR_TX;
             break;
    }

#else
   (void)&conn_id_transport;                                    /* Prevent compiler warning.                            */
   *perr =  NET_ERR_TX;
#endif

#else                                                           /* See Note #3.                                         */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   *perr =  NET_SOCK_ERR_INVALID_FAMILY;
    return (NET_SOCK_BSD_ERR_CONN);
#endif


    switch (*perr) {
        case NET_SOCK_ERR_NONE:
        case NET_ERR_TX:
             break;


        case NET_SOCK_ERR_FAULT:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */


        default:
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                /* -------------- WAIT ON TRANSPORT CONN -------------- */
#if   (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_BLOCK)
    block = DEF_YES;
#elif (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_NO_BLOCK)
    block = DEF_NO;
#else
    block = DEF_YES;
#endif

    sock_state   = psock->State;
    psock->State = NET_SOCK_STATE_CONN_IN_PROGRESS;
    if (block != DEF_YES) {                                     /* If non-blocking sock conn ...                        */
       *perr =  NET_SOCK_ERR_CONN_IN_PROGRESS;                  /* ... rtn not-yet-conn'd err.                          */
        return (NET_SOCK_BSD_ERR_CONN);
    }

    NetOS_Unlock();
    NetOS_Sock_ConnReqWait(sock_id, perr);
    NetOS_Lock(&err);
    if ( err != NET_OS_ERR_NONE) {
       *perr  = err;                                            /* Rtn err from NetOS_Lock().                           */
        return (NET_SOCK_BSD_ERR_CONN);
    }
    switch (*perr) {
        case NET_SOCK_ERR_NONE:
             break;


        case NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT:
             psock->State = sock_state;
                                                                /* Rtn err from NetOS_Sock_ConnReqWait().               */
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_ERR_CONN_SIGNAL_ABORT:
        case NET_SOCK_ERR_CONN_SIGNAL_FAULT:
        default:
             psock->State = sock_state;
            *perr         = NET_SOCK_ERR_FAULT;
             return (NET_SOCK_BSD_ERR_CONN);                    /* Prevent 'break NOT reachable' compiler warning.      */
    }



                                                                /* -------------- UPDATE SOCK CONN STATE -------------- */
    psock->State = NET_SOCK_STATE_CONN;


   *perr =  NET_SOCK_ERR_NONE;

    return (NET_SOCK_BSD_ERR_NONE);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                               NetSock_ConnHandlerAddrRemoteValidate()
*
* Description : (1) Validate socket remote address :
*
*                   (a) Validate socket connection                                  See Note #5c
*                   (b) Search for other socket connection(s) with same 
*                           local/remote addresses                                  See Note #5
*
*
* Argument(s) : psock           Pointer to socket.
*               -----           Argument validated in NetSock_Conn().
*
*               paddr_remote    Pointer to socket address structure.
*               ------------    Argument checked   in NetSock_Conn().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket remote address validated.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address structure length.
*                               NET_SOCK_ERR_CONN_IN_USE            Socket connection already in use.
*
*                                                                   - RETURNED BY NetConn_AddrLocalGet() : -
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnHandlerDatagram(),
*               NetSock_ConnHandlerStream().
*
* Note(s)     : (2) (a) Socket connection addresses MUST be maintained in network-order.
*
*                   (b) However, since the port number & address are copied from a network-order socket 
*                       address structure into local variables & then into a multi-octet array, they do 
*                       NOT need to be converted from host-order to network-order.
*
*               (3) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*
*               (4) Default case already invalidated in NetSock_Open().  However, the default case is 
*                   included as an extra precaution in case 'Protocol' is incorrectly modified.
*
*               (5) (a) (1) Multiple socket connections with the same local & remote address -- both 
*                           addresses & port numbers -- is NOT currently supported.
*
*                       (2) Therefore when updating a socket connection, it is necessary to search the 
*                           connection lists for any other connection with the same local & remote address.
*
*                       (3) Since datagram-type sockets' remote address is NOT required to be static, 
*                           datagram-type sockets in a "connected" state MUST search the connection lists 
*                           for a connection with the same local & remote address.
*
*                           See also 'NetSock_ConnHandlerDatagram()  Note #2b'.
*
*                   (b) (1) Also, multiple socket connections with only a local address but the same local 
*                           address -- both address & port number -- is NOT currently supported.
*
*                       (2) Therefore when adding or updating a socket connection with only a local address, 
*                           it is necessary to search the connection lists for any other connection with the 
*                           same local address.
*
*                       (3) Thus, the option for sockets to reuse the same local address is NOT currently 
*                           supported even if the socket reuse option (SO_REUSEADDR) is requested.
*
*                           See 'net_sock.c  Note #1d1'.
*
*                   See also 'NetSock_BindHandler()  Note #8'.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetSock_ConnHandlerAddrRemoteValidate (NET_SOCK       *psock,
                                                     NET_SOCK_ADDR  *paddr_remote,
                                                     NET_ERR        *perr)
{
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_SOCK_ADDR_IP      *paddr_ip;
    NET_IP_ADDR            addr_ip;
#endif
    CPU_INT08U             addr_remote[NET_SOCK_CFG_ADDR_LEN];
    CPU_INT08U             addr_local[NET_SOCK_CFG_ADDR_LEN];
    NET_SOCK_ADDR_LEN      addr_local_len;
    NET_PORT_NBR           port_nbr;
    NET_CONN_ID            conn_id;
    NET_CONN_ID            conn_id_srch;
    NET_CONN_FAMILY        conn_family;
    NET_CONN_PROTOCOL_IX   conn_protocol_ix;
    NET_ERR                err;


                                                                /* ---------------- VALIDATE SOCK CONN ---------------- */
    conn_id = psock->ID_Conn;
    if (conn_id == NET_CONN_ID_NONE) {                          /* If NO sock conn, rtn no err (see Note #5c).          */
       *perr = NET_SOCK_ERR_NONE;
        return;
    }


                                                                /* --------------- CFG SOCK REMOTE ADDR --------------- */
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
                                                                /* Cfg remote addr as net-order sock addr (see Note #2).*/
    paddr_ip = (NET_SOCK_ADDR_IP *)paddr_remote;
    NET_UTIL_VAL_COPY_16(&port_nbr, &paddr_ip->Port);           /* Copy preserves net-order (see Note #2b).             */
    NET_UTIL_VAL_COPY_32(&addr_ip,  &paddr_ip->Addr);           /* Copy preserves net-order (see Note #2b).             */

    Mem_Copy((void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_PORT],
             (void     *)&port_nbr,
             (CPU_SIZE_T) sizeof(port_nbr));
    Mem_Copy((void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_ADDR],
             (void     *)&addr_ip,
             (CPU_SIZE_T) sizeof(addr_ip));

#else                                                           /* See Note #3.                                         */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   *perr = NET_SOCK_ERR_INVALID_FAMILY;
    return;
#endif


/*$PAGE*/
                                                                /* ---- SRCH FOR LOCAL/REMOTE CONN IN CONN LIST(S) ---- */
                                                                /* Get local  addr from sock conn.                      */
    addr_local_len = sizeof(addr_local);
    NetConn_AddrLocalGet((NET_CONN_ID        ) conn_id,
                         (CPU_INT08U        *)&addr_local[0],
                         (NET_CONN_ADDR_LEN *)&addr_local_len,
                         (NET_ERR           *) perr);
    if (*perr != NET_CONN_ERR_NONE) {
        return;
    }
    if (addr_local_len != NET_SOCK_CFG_ADDR_LEN) {
       *perr = NET_SOCK_ERR_INVALID_ADDR_LEN;
        return;
    }

                                                                /* Cfg conn srch.                                       */
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    conn_family = NET_CONN_FAMILY_IP_V4_SOCK;
    switch (psock->Protocol) {
        case NET_SOCK_PROTOCOL_UDP:
             conn_protocol_ix = NET_CONN_PROTOCOL_IX_IP_V4_UDP;
             break;


#ifdef  NET_TCP_MODULE_PRESENT
        case NET_SOCK_PROTOCOL_TCP:
             conn_protocol_ix = NET_CONN_PROTOCOL_IX_IP_V4_TCP;
             break;
#endif

        case NET_SOCK_PROTOCOL_NONE:
        default:                                                /* See Note #4.                                         */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
            *perr = NET_SOCK_ERR_INVALID_PROTOCOL;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

#else                                                           /* See Note #3.                                         */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   *perr = NET_SOCK_ERR_INVALID_FAMILY;
    return;
#endif

                                                                /* Srch for sock conn with identical local/remote addrs.*/
    conn_id_srch = NetConn_Srch((NET_CONN_FAMILY     ) conn_family,
                                (NET_CONN_PROTOCOL_IX) conn_protocol_ix,
                                (CPU_INT08U         *)&addr_local[0],
                                (CPU_INT08U         *)&addr_remote[0],
                                (NET_CONN_ADDR_LEN   ) NET_SOCK_CFG_ADDR_LEN,
                                (NET_CONN_ID        *) 0,
                                (NET_CONN_ID        *) 0,
                                (NET_ERR            *)&err);
    if ( err == NET_CONN_ERR_CONN_FULL) {                       /* If local/remote addrs already conn'd, ...            */
        if (conn_id_srch == psock->ID_Conn) {                   /* ... but = sock's conn,                ...            */
           *perr = NET_SOCK_ERR_NONE;                           /* ... rtn valid conn remote addr;       ...            */
            return;                                             
        }
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidConnInUseCtr);
       *perr = NET_SOCK_ERR_CONN_IN_USE;                        /* ... else rtn err (see Note #5).                      */
        return;
    }


   *perr = NET_SOCK_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetSock_ConnHandlerAddrRemoteSet()
*
* Description : Set socket remote address.
*
* Argument(s) : psock               Pointer to socket.
*               -----               Argument validated in NetSock_Conn().
*
*               paddr_remote        Pointer to socket address structure.
*               ------------        Argument checked   in NetSock_ConnHandlerAddrRemoteValidate().
*
*               addr_over_wr        Allow remote address overwrite :
*               ------------
*                                       DEF_NO                      Do NOT overwrite remote address.
*                                       DEF_YES                            Overwrite remote address.
*
*                                   Argument validated in NetSock_ConnHandlerDatagram(),
*                                                         NetSock_ConnHandlerStream().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket remote address successfully set.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*
*                                                                   - RETURNED BY NetConn_AddrRemoteSet() : -
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnHandlerDatagram(),
*               NetSock_ConnHandlerStream().
*
* Note(s)     : (1) (a) Socket connection addresses MUST be maintained in network-order.
*
*                   (b) However, since the port number & address are copied from a network-order socket 
*                       address structure into local variables & then into a multi-octet array, they do 
*                       NOT need to be converted from host-order to network-order.
*
*               (2) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetSock_ConnHandlerAddrRemoteSet (NET_SOCK       *psock,
                                                NET_SOCK_ADDR  *paddr_remote,
                                                CPU_BOOLEAN     addr_over_wr,
                                                NET_ERR        *perr)
{
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_SOCK_ADDR_IP  *paddr_ip;
    NET_IP_ADDR        addr_ip;
#endif
    CPU_INT08U         addr_remote[NET_SOCK_CFG_ADDR_LEN];
    NET_PORT_NBR       port_nbr;
    NET_CONN_ID        conn_id;

                                                                /* --------------- CFG SOCK REMOTE ADDR --------------- */
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
                                                                /* Cfg remote addr as net-order sock addr (see Note #1).*/
    paddr_ip = (NET_SOCK_ADDR_IP *)paddr_remote;
    NET_UTIL_VAL_COPY_16(&port_nbr, &paddr_ip->Port);           /* Copy preserves net-order (see Note #1b).             */
    NET_UTIL_VAL_COPY_32(&addr_ip,  &paddr_ip->Addr);           /* Copy preserves net-order (see Note #1b).             */

    Mem_Copy((void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_PORT],
             (void     *)&port_nbr,
             (CPU_SIZE_T) sizeof(port_nbr));
    Mem_Copy((void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_ADDR],
             (void     *)&addr_ip,
             (CPU_SIZE_T) sizeof(addr_ip));

#else                                                           /* See Note #2.                                         */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   *perr = NET_SOCK_ERR_INVALID_FAMILY;
    return;
#endif

                                                                /* --------------- SET SOCK REMOTE ADDR --------------- */
    conn_id = psock->ID_Conn;
    NetConn_AddrRemoteSet((NET_CONN_ID      ) conn_id,    
                          (CPU_INT08U      *)&addr_remote[0],
                          (NET_CONN_ADDR_LEN) NET_SOCK_CFG_ADDR_LEN,
                          (CPU_BOOLEAN      ) addr_over_wr,
                          (NET_ERR         *) perr);
    if (*perr != NET_CONN_ERR_NONE) {
         NetSock_CloseSock(psock, DEF_YES, DEF_YES);
         return;
    }


   *perr = NET_SOCK_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_ConnAcceptQ_Init()
*
* Description : Initialize a stream-type socket's connection accept queue.
*
* Argument(s) : psock           Pointer to socket.
*               -----           Argument validated in NetSock_Listen().
*
*               sock_q_size     Maximum number of connection requests to accept & queue on listen socket.
*
*                                   NET_SOCK_Q_SIZE_NONE                    NO custom configuration for socket's 
*                                                                               connection accept queue maximum
*                                                                               size; configure to default maximum :
*                                                                               NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX.
*
*                                <= NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX     Custom configure socket's connection 
*                                                                               accept queue maximum size.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Listen().
*
* Note(s)     : (1) Some socket controls were previously initialized in NetSock_Clr() when the socket
*                   was allocated.  These socket controls do NOT need to be re-initialized but are
*                   shown for completeness.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  void  NetSock_ConnAcceptQ_Init (NET_SOCK         *psock,
                                        NET_SOCK_Q_SIZE   sock_q_size)
{
    NET_SOCK_Q_SIZE  accept_q_size;


    if (sock_q_size != NET_SOCK_Q_SIZE_NONE) {                      /* If     conn accept Q size cfg'd, ..              */
        if (sock_q_size > 0) {                                      /* .. lim conn accept Q size;       ..              */
            accept_q_size = (sock_q_size > NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX) ? NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX
                                                                                : sock_q_size;
        } else {
            accept_q_size =  NET_SOCK_Q_SIZE_MIN;
        }
    } else {                                                        /* .. else cfg to dflt max size.                    */
        accept_q_size = NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX;
    }

    psock->ConnAcceptQ_SizeMax = accept_q_size;                     /* Cfg listen sock conn accept Q max size.          */

                                                                    /* Init conn accept Q ctrls.                        */
#if 0                                                               /* Init'd in NetSock_Clr() [see Note #1].           */
    psock->ConnAcceptQ_SizeCur = 0;
    psock->ConnAcceptQ_HeadIx  = 0;
    psock->ConnAcceptQ_TailIx  = 0;
#endif
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetSock_ConnAcceptQ_Clr()
*
* Description : Clear a stream-type socket's connection accept queue.
*
* Argument(s) : psock       Pointer to socket.
*               -----       Argument validated in NetSock_ConnSignalAccept(),
*                                                 NetSock_FreeHandler().
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnSignalAccept(),
*               NetSock_FreeHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  void  NetSock_ConnAcceptQ_Clr (NET_SOCK  *psock)
{
    CPU_BOOLEAN  done;
    NET_CONN_ID  conn_id;
    NET_ERR      err;

                                                                    /* -------------- VALIDATE SOCK TYPE -------------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
        return;
    }

                                                                    /* -------------- CLR CONN ACCEPT Q --------------- */
    done = DEF_NO;
    while (done == DEF_NO) {                                        /* Clr sock's conn accept Q ...                     */
        if (psock->ConnAcceptQ_SizeCur > 0) {                       /* ... while non-empty.                             */
            conn_id  = NetSock_ConnAcceptQ_ConnID_Get(psock, &err);
            if (err == NET_SOCK_ERR_NONE) {
                NetConn_CloseFromApp(conn_id, DEF_YES);
            } else {
                done = DEF_YES;
            }
        } else {
            done = DEF_YES;
        }
    }
                                                                    /* Clr sock's conn accept Q ix's.                   */
    psock->ConnAcceptQ_HeadIx = 0;
    psock->ConnAcceptQ_TailIx = 0;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetSock_ConnAcceptQ_IsAvail()
*
* Description : Check if socket's connection accept queue is available to queue a new connection.
*
* Argument(s) : psock       Pointer to socket.
*               -----       Argument validated in NetSock_ListenQ_IsAvail().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection accept queue successfully 
*                                                                   checked; check return value for socket 
*                                                                   connection accept queue availability.
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type; i.e. NOT stream-type.
*
* Return(s)   : DEF_YES, if socket connection accept queue is available to queue a new connection.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_ListenQ_IsAvail().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_ConnAcceptQ_IsAvail (NET_SOCK  *psock,
                                                  NET_ERR   *perr)
{
    CPU_BOOLEAN  q_avail;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                     /* -------------- VALIDATE SOCK TYPE -------------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
       *perr =  NET_SOCK_ERR_INVALID_TYPE;
        return (DEF_NO);
    }
#endif

                                                                    /* --------- CHK SOCK CONN ACCEPT Q AVAIL --------- */
    q_avail = (psock->ConnAcceptQ_SizeCur >= psock->ConnAcceptQ_SizeMax) ? DEF_NO : DEF_YES;
   *perr    =  NET_SOCK_ERR_NONE;

    return (q_avail);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_ConnAcceptQ_IsRdy()
*
* Description : Check if socket's connection accept queue is ready with any available queued connection(s).
*
* Argument(s) : psock       Pointer to socket.
*               -----       Argument validated in NetSock_SelDescHandlerRd().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection accept queue successfully 
*                                                                   checked; check return value for socket 
*                                                                   connection accept queue availability.
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type; i.e. NOT stream-type.
*
* Return(s)   : DEF_YES, if socket connection accept queue has any available queued connection(s).
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerRdStream().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if ((NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED) && \
     (NET_SOCK_CFG_SEL_EN         == DEF_ENABLED))
static  CPU_BOOLEAN  NetSock_ConnAcceptQ_IsRdy (NET_SOCK  *psock,
                                                NET_ERR   *perr)
{
    CPU_BOOLEAN  q_rdy;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                     /* -------------- VALIDATE SOCK TYPE -------------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
       *perr =  NET_SOCK_ERR_INVALID_TYPE;
        return (DEF_NO);
    }
#endif

                                                                    /* ---------- CHK SOCK CONN ACCEPT Q RDY ---------- */
    q_rdy = (psock->ConnAcceptQ_SizeCur > 0) ? DEF_YES : DEF_NO;
   *perr  =  NET_SOCK_ERR_NONE;

    return (q_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetSock_ConnAcceptQ_ConnID_Add()
*
* Description : Add a connection handle identifier into a stream-type socket's connection accept queue.
*
*               (1) A stream-type socket's connection accept queue is a FIFO Q implemented as a circular 
*                   ring array :
*
*                   (a) Sockets' 'ConnAcceptQ_HeadIx' points to the next available connection handle
*                       identifier to accept.
*
*                   (b) Sockets' 'ConnAcceptQ_TailIx' points to the next available queue entry to insert
*                       a connection handle identifier.
*
*                   (c) Sockets' 'ConnAcceptQ_HeadIx'/'ConnAcceptQ_TailIx' advance :
*
*                       (1) By increment;
*                       (2) Reset to minimum index value when maximum index value reached.
*
*                           (A) Although a specific maximum array-size/index-value is configured for 
*                               each socket connection accept queue ('ConnAcceptQ_SizeMax'), the global 
*                               maximum array-size/index-value ('NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX') 
*                               is used as the maximum index value.  Although this uses the entire
*                               queue array (not just a subset) for adding & removing connection handle
*                               identifiers, it eliminates the need to redundantly validate the socket's
*                               configured connection accept queue maximum array-size/index-value.
*
*
*                                  Index to next available          Index to next available entry        
*                               connection handle identifier         to insert accept connection         
*                                      in accept queue                    handle identifier              
*                                      (see Note #1a)                      (see Note #1b)                
*
*                                             |                                   |
*                                             |                                   |
*                                             v                                   v
*                              -------------------------------------------------------------
*                              |     |     |     |     |     |     |     |     |     |     |
*                              |     |     |     |     |     |     |     |     |     |     |
*                              |     |     |     |     |     |     |     |     |     |     |
*                              -------------------------------------------------------------
*
*                                                       ---------->
*                                                 FIFO indices advance by
*                                                increment (see Note #1c1)
*
*                              |                                                           |
*                              |<----------------- Circular Ring FIFO Q ------------------>|
*                              |                      (see Note #1)                        |
*
*
* Argument(s) : psock       Pointer to socket.
*               -----       Argument validated in NetSock_ConnSignalAccept().
*
*               conn_id     Handle identifier of network connection to insert into connection accept queue.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Accept connection handle identifier
*                                                                       successfully added.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type; i.e. NOT stream-type.
*                               NET_SOCK_ERR_CONN_ACCEPT_Q_DUP      Connection handle identifier already in socket
*                                                                       connection accept queue.
*                               NET_SOCK_ERR_CONN_ACCEPT_Q_MAX      Maximum or invalid number queue entries used.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnSignalAccept().
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  void  NetSock_ConnAcceptQ_ConnID_Add (NET_SOCK     *psock,
                                              NET_CONN_ID   conn_id,
                                              NET_ERR      *perr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN  found;
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                            /* ---------- VALIDATE SOCK TYPE ---------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
       *perr = NET_SOCK_ERR_INVALID_TYPE;
        return;
    }
                                                                            /* ----------- VALIDATE CONN ID ----------- */
    found = NetSock_ConnAcceptQ_ConnID_Srch((NET_SOCK        *)psock,
                                            (NET_CONN_ID      )conn_id,
                                            (NET_SOCK_Q_SIZE *)0,
                                            (NET_SOCK_Q_SIZE *)0);
    if (found != DEF_NO) {
       *perr = NET_SOCK_ERR_CONN_ACCEPT_Q_DUP;
        return;
    }
#endif
                                                                            /* ---------- VALIDATE NBR USED ----------- */
                                                                            /* Chk sock max conn accept Q lim.          */
    if (psock->ConnAcceptQ_SizeCur >= psock->ConnAcceptQ_SizeMax) {
       *perr = NET_SOCK_ERR_CONN_ACCEPT_Q_MAX;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (psock->ConnAcceptQ_SizeCur >= NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrConnAcceptQ_MaxCtr);
       *perr = NET_SOCK_ERR_CONN_ACCEPT_Q_MAX;
        return;
    }
#endif

                                                                            /* ---- ADD CONN ID INTO CONN ACCEPT Q ---- */
    psock->ConnAcceptQ[psock->ConnAcceptQ_TailIx] = conn_id;                /* Add conn id @ tail ix (see Note #1b).    */

    psock->ConnAcceptQ_TailIx++;                                            /* Inc tail ix to next avail Q entry.       */
    if (psock->ConnAcceptQ_TailIx >= NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX) { /* If  tail ix > max ix, ...                */
        psock->ConnAcceptQ_TailIx  = 0;                                     /* ... reset to min ix (see Note #1c2).     */
    }

    psock->ConnAcceptQ_SizeCur++;


   *perr = NET_SOCK_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetSock_ConnAcceptQ_ConnID_Get()
*
* Description : Get a connection handle identifier from a stream-type socket's connection accept queue.
*
*               (1) A stream-type socket's connection accept queue is a FIFO Q implemented as a circular 
*                   ring array :
*
*                   (a) Sockets' 'ConnAcceptQ_HeadIx' points to the next available connection handle
*                       identifier to accept.
*
*                   See also 'NetSock_ConnAcceptQ_ConnID_Add()  Note #1'.
*
*
* Argument(s) : psock       Pointer to socket.
*               -----       Argument validated in NetSock_Accept().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                       Accept connection handle identifier
*                                                                           successfully retrieved.
*                               NET_SOCK_ERR_INVALID_TYPE               Invalid socket type; i.e. NOT stream-type.
*                               NET_SOCK_ERR_CONN_ACCEPT_Q_NONE_AVAIL   Accept connection handle identifier NOT
*                                                                           available.
*
* Return(s)   : Accept connection handle identifier, if NO errors.
*
*               NET_CONN_ID_NONE,                    otherwise.
*
* Caller(s)   : NetSock_Accept().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_CONN_ID  NetSock_ConnAcceptQ_ConnID_Get (NET_SOCK  *psock,
                                                     NET_ERR   *perr)
{
    NET_CONN_ID  conn_id;

                                                                            /* ---------- VALIDATE SOCK TYPE ---------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
       *perr =  NET_SOCK_ERR_INVALID_TYPE;
        return (NET_CONN_ID_NONE);
    }
                                                                            /* ---------- VALIDATE NBR USED ----------- */
    if (psock->ConnAcceptQ_SizeCur < 1) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrConnAcceptQ_NoneAvailCtr);
       *perr =  NET_SOCK_ERR_CONN_ACCEPT_Q_NONE_AVAIL;
        return (NET_CONN_ID_NONE);
    }

                                                                            /* ---- GET CONN ID FROM CONN ACCEPT Q ---- */
                                                                            /* Get conn id @ head ix (see Note #1a).    */
    conn_id                                       = psock->ConnAcceptQ[psock->ConnAcceptQ_HeadIx];
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    psock->ConnAcceptQ[psock->ConnAcceptQ_HeadIx] = NET_CONN_ID_NONE;
#endif

    psock->ConnAcceptQ_HeadIx++;                                            /* Inc head ix to next avail Q conn id.     */
    if (psock->ConnAcceptQ_HeadIx >= NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX) { /* If  head ix > max ix, ...                */
        psock->ConnAcceptQ_HeadIx  = 0;                                     /* ... reset to min ix.                     */
    }

    psock->ConnAcceptQ_SizeCur--;


   *perr =  NET_SOCK_ERR_NONE;

    return (conn_id);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetSock_ConnAcceptQ_ConnID_Srch()
*
* Description : Saerch for a connection handle identifier in a stream-type socket's connection accept queue.
*
* Argument(s) : psock       Pointer to socket.
*               -----       Argument validated in NetSock_ConnAcceptQ_ConnID_Add(),
*                                                 NetSock_ConnAcceptQ_ConnID_Remove().
*
*               conn_id     Handle identifier of network connection to search for in connection accept queue.
*
*               pconn_ix    Pointer to a variable to ... :
*
*                               (a) Return the connection accept queue array index of the network connection
*                                       handle identifier,     if found;
*                               (b) Return NET_SOCK_Q_IX_NONE, otherwise.
*
*               pconn_nbr   Pointer to a variable to ... :
*
*                               (a) Return the number of connection accept queue handle identifiers ahead 
*                                       of the desired network connection handle identifier, if found;
*                               (b) Return 0, otherwise.
*
* Return(s)   : DEF_YES, connection handle identifier found in socket's connection accept queue.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_ConnAcceptQ_ConnID_Add(),
*               NetSock_ConnAcceptQ_ConnID_Remove().
*
* Note(s)     : (1) Pointers to variables that return values MUST be initialized to return PRIOR to all
*                   other validation or function handling in case of any error(s).
*
*               (2) Assumes queue indices valid.
*
*               (3) If ALL connection handle identifiers in queue searched & queue tail index NOT found,
*                   tail index MUST be invalid -- outside the range of table indices.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_ConnAcceptQ_ConnID_Srch (NET_SOCK         *psock,
                                                      NET_CONN_ID       conn_id,
                                                      NET_SOCK_Q_SIZE  *pconn_ix,
                                                      NET_SOCK_Q_SIZE  *pconn_nbr)
{
    CPU_BOOLEAN      found;
    CPU_BOOLEAN      done;
    NET_SOCK_Q_SIZE  ix_cur;
    NET_SOCK_Q_SIZE  nbr;

                                                                            /* Init rtn vals for err (see Note #1).     */
    if (pconn_ix  != (NET_SOCK_Q_SIZE *)0) {
       *pconn_ix   =  NET_SOCK_Q_IX_NONE;
    }
    if (pconn_nbr != (NET_SOCK_Q_SIZE *)0) {
       *pconn_nbr  =  0;
    }

                                                                            /* ----- VALIDATE CONN ACCEPT Q SIZE ------ */
    if (psock->ConnAcceptQ_SizeCur < 1) {
        return (DEF_NO);
    }

                                                                            /* ---- SRCH CONN ID IN CONN ACCEPT Q ----- */
    found  = DEF_NO;
    done   = DEF_NO;
    ix_cur = psock->ConnAcceptQ_HeadIx;
    nbr    = 0;

    while (done == DEF_NO) {                                                /* Srch sock conn accept Q for conn id.     */
        if (ix_cur == psock->ConnAcceptQ_TailIx) {                          /* If ix @ tail, conn id NOT found.         */
            done = DEF_YES;

        } else {
            found = (psock->ConnAcceptQ[ix_cur] == conn_id) ? DEF_YES : DEF_NO;

            if (found == DEF_YES) {                                         /* If conn id found, ...                    */
                done  =  DEF_YES;                                           /* ... abort successful srch.               */

            } else {
                nbr++;
                if (nbr >= psock->ConnAcceptQ_SizeCur) {                    /* If nbr srch'd > nbr of Q's conn id's, .. */
                    done = DEF_YES;                                         /* .. abort srch; Q corrupted (see Note #3).*/

                } else {
                    ix_cur++;
                    if (ix_cur >= NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX) {    /* If ix     >  max ix, ...                 */
                        ix_cur  = 0;                                        /* ... reset to min ix.                     */
                    }
                }
            }
        }
    }

                                                                            /* -------- RTN SRCH CONN ID FOUND -------- */
    if (found == DEF_YES) {                                                 /* If  srch conn id found, ...              */
        if (pconn_ix  != (NET_SOCK_Q_SIZE *)0) {
           *pconn_ix   =  ix_cur;                                           /* ... rtn  conn id ix.                     */
        }
        if (pconn_nbr != (NET_SOCK_Q_SIZE *)0) {
           *pconn_nbr  =  nbr;
        }
    }

    return (found);                                                         /* Rtn srch conn id found status.           */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetSock_ConnAcceptQ_ConnID_Remove()
*
* Description : Remove a connection handle identifier from a stream-type socket's connection accept queue.
*
* Argument(s) : psock       Pointer to socket.
*               -----       Argument validated in NetSock_FreeConnFromSock().
*
*               conn_id     Handle identifier of network connection to remove from connection accept queue.
*
* Return(s)   : DEF_YES, connection handle identifier found & successfully removed from socket's connection
*                   accept queue.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_FreeConnFromSock().
*
* Note(s)     : (1) Assumes queue indices valid.
*
*               (2) If ALL connection handle identifiers in queue searched & queue tail index NOT found,
*                   tail index MUST be invalid -- outside the range of table indices.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_ConnAcceptQ_ConnID_Remove (NET_SOCK     *psock,
                                                        NET_CONN_ID   conn_id)
{
    CPU_BOOLEAN      found;
    NET_SOCK_Q_SIZE  ix_id;
    NET_SOCK_Q_SIZE  ix_next;
    NET_SOCK_Q_SIZE  nbr;

                                                                            /* ---------- VALIDATE SOCK TYPE ---------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
        return (DEF_NO);
    }
                                                                            /* ----- VALIDATE CONN ACCEPT Q SIZE ------ */
    if (psock->ConnAcceptQ_SizeCur < 1) {
        return (DEF_NO);
    }

                                                                            /* ---- SRCH CONN ID IN CONN ACCEPT Q ----- */
    found = NetSock_ConnAcceptQ_ConnID_Srch(psock, conn_id, &ix_id, &nbr);
    if (found != DEF_YES) {                                                 /* If conn id NOT found, exit remove.       */
        return  (DEF_NO);
    }

                                                                            /* -- REMOVE CONN ID FROM CONN ACCEPT Q --- */
    psock->ConnAcceptQ_SizeCur--;                                           /* Dec conn accept Q cur size.              */

    ix_next = ix_id;
    while (nbr < psock->ConnAcceptQ_SizeCur) {                              /* Shift ALL rem'ing conn id's following ...*/
                                                                            /* ... removed conn id.                     */
        ix_next++;
        if (ix_next >= NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX) {               /* If next ix > max ix, ...                 */
            ix_next  = 0;                                                   /* ... reset to min ix.                     */
        }

        psock->ConnAcceptQ[ix_id] = psock->ConnAcceptQ[ix_next];            /* Shift conn id @ next ix to cur ix.       */

        ix_id++;
        if (ix_id >= NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX) {                 /* If cur  ix > max ix, ...                 */
            ix_id  = 0;                                                     /* ... reset to min ix.                     */
        }

        nbr++;
    }

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    psock->ConnAcceptQ[ix_id] = NET_CONN_ID_NONE;
#endif

    if (psock->ConnAcceptQ_TailIx == 0) {                                   /* If tail ix @ min ix, ...                 */
        psock->ConnAcceptQ_TailIx  = NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX;   /* ... reset to max ix.                     */
    }
    psock->ConnAcceptQ_TailIx--;                                            /* Dec tail ix.                             */


    return (DEF_YES);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetSock_RxDataHandler()
*
* Description : (1) Receive data from a socket :
*
*                   (a) Validate receive data buffer                                    See Note #2
*                   (b) Validate receive flags                                          See Note #3
*                   (c) Acquire  network lock
*                   (d) Validate socket  used
*                   (e) Receive  socket  data
*                   (f) Release  network lock
*
*
* Argument(s) : sock_id             Socket descriptor/handle identifier of socket to receive data.
*
*               pdata_buf           Pointer to an application data buffer that will receive the socket's received
*                                       data.
*
*               data_buf_len        Size of the   application data buffer (in octets) [see Note #2].
*
*               flags               Flags to select receive options (see Note #3); bit-field flags logically OR'd :
*
*                                       NET_SOCK_FLAG_NONE              No socket flags selected.
*                                       NET_SOCK_FLAG_RX_DATA_PEEK      Receive socket data without consuming
*                                                                           the socket data; i.e. socket data 
*                                                                           NOT removed from application receive
*                                                                           queue(s).
*                                       NET_SOCK_FLAG_RX_NO_BLOCK       Receive socket data without blocking.
*
*               paddr_remote        Pointer to an address buffer that will receive the socket address structure 
*                                       with the received data's remote address (see Note #4), if NO errors.
*
*               paddr_len           Pointer to a variable, if available, to ... :
*
*                                       (a) Pass the size of the address buffer pointed to by 'paddr_remote'.
*                                       (b) (1) Return the actual size of socket address structure with the 
*                                                   received data's remote address, if NO errors;
*                                           (2) Return 0,                           otherwise.
*
*                                   See also Note #5.
*
*               pip_opts_buf        Pointer to buffer to receive possible IP options (see Note #6a), if NO errors.
*
*               ip_opts_buf_len     Size of IP options receive buffer (in octets)    [see Note #6b].
*
*               pip_opts_len        Pointer to variable that will receive the return size of any received IP options,
*                                       if NO errors.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NULL_PTR               Argument 'pdata_buf'    passed a NULL pointer.
*                               NET_SOCK_ERR_NULL_SIZE              Argument 'data_buf_len' passed a NULL size.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_FLAG           Invalid socket flags.
*
*                                                                   -------- RETURNED BY NetSock_IsUsed() : ---------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*
*                                                                   - RETURNED BY NetSock_RxDataHandlerDatagram() : -
*                                                                   -- RETURNED BY NetSock_RxDataHandlerStream() : --
*                               NET_SOCK_ERR_NONE                   Socket data successfully received; check return
*                                                                       value for number of data octets received.
*
*                               NET_SOCK_ERR_RX_Q_EMPTY             Socket receive queue empty.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_ERR_RX                          Receive error.
*
*                                                                   - RETURNED BY NetSock_RxDataHandlerDatagram() : -
*                               NET_SOCK_ERR_INVALID_DATA_SIZE      Socket data receive buffer insufficient size;
*                                                                       some, but not all, socket data deframed
*                                                                       into receive buffer (see Note #2a2).
*
*                                                                   -- RETURNED BY NetSock_RxDataHandlerStream() : --
*                               NET_SOCK_ERR_RX_Q_CLOSED            Socket receive queue closed.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address length.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
*                                                                   ---------- RETURNED BY NetOS_Lock() : -----------
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*$PAGE*
* Return(s)   : Number of positive data octets received, if NO errors                (see Note #7a).
*
*               NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,       if socket connection closed (see Note #7b).
*
*               NET_SOCK_BSD_ERR_RX,                     otherwise                   (see Note #7c1).
*
* Caller(s)   : NetSock_RxDataFrom(),
*               NetSock_RxData().
*
* Note(s)     : (2) (a) (1) (A) Datagram-type sockets transmit & receive all data atomically -- i.e. every 
*                               single, complete datagram transmitted MUST be received as a single, complete
*                               datagram.
*
*                           (B) IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes
*                               that "for message-based sockets, such as SOCK_DGRAM ... the entire message 
*                               shall be read in a single operation.  If a message is too long to fit in 
*                               the supplied buffer, and MSG_PEEK is not set in the flags argument, the 
*                               excess bytes shall be discarded".
*
*                       (2) Thus, if the socket's type is datagram & the receive data buffer size is 
*                           NOT large enough for the received data, the receive data buffer is maximally 
*                           filled with receive data but the remaining data octets are discarded & 
*                           NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
*
*                   (b) (1) (A) (1) Stream-type sockets transmit & receive all data octets in one or more 
*                                   non-distinct packets.  In other words, the application data is NOT 
*                                   bounded by any specific packet(s); rather, it is contiguous & sequenced 
*                                   from one packet to the next.
*
*                               (2) IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes
*                                   that "for stream-based sockets, such as SOCK_STREAM, message boundaries 
*                                   shall be ignored.  In this case, data shall be returned to the user as
*                                   soon as it becomes available, and no data shall be discarded".
*
*                           (B) Thus, if the socket's type is stream & the receive data buffer size is NOT 
*                               large enough for the received data, the receive data buffer is maximally 
*                               filled with receive data & the remaining data octets remain queued for 
*                               later application-socket receives.
*                   
*                       (2) Thus, it is typical -- but NOT absolutely required -- that a single application
*                           task ONLY receive or request to receive data from a stream-type socket.
*
*                   See also 'NetSock_RxDataHandlerDatagram()  Note #2',
*                          & 'NetSock_RxDataHandlerStream()    Note #2'.
*
*               (3) #### Only some socket receive flag options are implemented.  If other flag options 
*                   are  requested, NetSock_RxDataHandler() aborts & returns appropriate error codes so 
*                   that requested flag options are NOT silently ignored.
*
*               (4) (a) Socket address structure 'AddrFamily' member returned in host-order & SHOULD NOT 
*                       be converted to network-order.
*
*                   (b) Socket address structure addresses MUST be converted from host-order to network-
*                       order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (5) (a) Since 'paddr_len' parameter is both an input & output parameter (see 'Argument(s) :
*                       paddr_len'), ...
*
*                       (1) its input value SHOULD be validated prior to use; ...
*                       (2) while its output value MUST be initially configured to return a default value 
*                           in case of any function exit due to error/fault conditions (see also Note #8).
*
*                   (b) However, if 'paddr_len' is available, it SHOULD already be validated & initialized 
*                       by previous NetSock_RxData() function(s).
*
*               (6) (a) If ...
*
*                       (1) NO IP options were received
*                             OR
*                       (2) NO IP options receive buffer is provided
*                             OR
*                       (3) IP options receive buffer NOT large enough for the received IP options
*
*                       ... then NO IP options are returned & any received IP options are silently discarded.
*
*                   (b) The IP options receive buffer size SHOULD be large enough to receive the maximum 
*                       IP options size, NET_IP_HDR_OPT_SIZE_MAX.
*
*                   (c) #### Received IP options should be provided/decoded via appropriate IP layer API.
*
*                   See also Note #10.
*$PAGE*
*               (7) IEEE Std 1003.1, 2004 Edition, Section 'recv() : RETURN VALUE' states that :
*
*                   (a) "Upon successful completion, recv() shall return the length of the message in bytes."
*
*                   (b) "If no messages are available to be received and the peer has performed an orderly
*                        shutdown, recv() shall return 0."
*
*                   (c) (1) "Otherwise, -1 shall be returned" ...
*                       (2) "and 'errno' set to indicate the error."
*                           #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   See also 'NetSock_RxDataHandlerDatagram()  Note #7'
*                          & 'NetSock_RxDataHandlerStream()    Note #7'.
*
*               (8) Pointers to variables that return values MUST be initialized to return PRIOR to all
*                   other validation or function handling in case of any error(s).
*
*                   However, these pointed-to variables SHOULD already be validated & initialized by
*                   previous NetSock_RxData() function(s).
*
*               (9) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'SockType' is incorrectly modified.
*
*              (10) #### IP options arguments may NOT be necessary (remove if unnecessary).
*********************************************************************************************************
*/

static  NET_SOCK_RTN_CODE  NetSock_RxDataHandler (NET_SOCK_ID         sock_id,
                                                  void               *pdata_buf,
                                                  CPU_INT16U          data_buf_len,
                                                  CPU_INT16S          flags,
                                                  NET_SOCK_ADDR      *paddr_remote,
                                                  NET_SOCK_ADDR_LEN  *paddr_len,
                                                  void               *pip_opts_buf,
                                                  CPU_INT08U          ip_opts_buf_len,
                                                  CPU_INT08U         *pip_opts_len,
                                                  NET_ERR            *perr)
{
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    CPU_INT16U          flag_mask;
#endif
    NET_SOCK           *psock;
    NET_SOCK_RTN_CODE   rtn_code;



#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
                                                                /* --------------- VALIDATE RX DATA BUF --------------- */
    if (pdata_buf == (void *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNullPtrCtr);
       *perr =  NET_SOCK_ERR_NULL_PTR;
        return (NET_SOCK_BSD_ERR_RX);
    }
    if (data_buf_len < 1) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNullSizeCtr);
       *perr =  NET_SOCK_ERR_NULL_SIZE;
        return (NET_SOCK_BSD_ERR_RX);
    }

                                                                /* ----------------- VALIDATE RX FLAGS ---------------- */
    flag_mask = NET_SOCK_FLAG_NONE         |
                NET_SOCK_FLAG_RX_DATA_PEEK |
                NET_SOCK_FLAG_RX_NO_BLOCK;
                                                                /* If any invalid flags req'd, rtn err (see Note #3).   */
    if (((CPU_INT16U)flags & ~flag_mask) != NET_SOCK_FLAG_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFlagsCtr);
       *perr =  NET_SOCK_ERR_INVALID_FLAG;
        return (NET_SOCK_BSD_ERR_RX);
    }
#endif


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_RX);
    }



#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_RX);
    }
#endif


/*$PAGE*/
                                                                /* ------------------- RX SOCK DATA ------------------- */
    psock = &NetSock_Tbl[sock_id];

    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             rtn_code = NetSock_RxDataHandlerDatagram((NET_SOCK_ID        )sock_id,
                                                      (NET_SOCK          *)psock,
                                                      (void              *)pdata_buf,
                                                      (CPU_INT16U         )data_buf_len,
                                                      (CPU_INT16S         )flags,
                                                      (NET_SOCK_ADDR     *)paddr_remote,
                                                      (NET_SOCK_ADDR_LEN *)paddr_len,
                                                      (void              *)pip_opts_buf,
                                                      (CPU_INT08U         )ip_opts_buf_len,
                                                      (CPU_INT08U        *)pip_opts_len,
                                                      (NET_ERR           *)perr);
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             rtn_code = NetSock_RxDataHandlerStream((NET_SOCK_ID        )sock_id,
                                                    (NET_SOCK          *)psock,
                                                    (void              *)pdata_buf,
                                                    (CPU_INT16U         )data_buf_len,
                                                    (CPU_INT16S         )flags,
                                                    (NET_SOCK_ADDR     *)paddr_remote,
                                                    (NET_SOCK_ADDR_LEN *)paddr_len,
                                                    (NET_ERR           *)perr);
             break;
#endif

        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                /* See Note #9.                                         */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_TYPE;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


    return (rtn_code);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetSock_RxDataHandlerDatagram()
*
* Description : (1) Receive data from a datagram-type socket :
*
*                   (a) Validate socket connection state                                See Note #13
*                   (b) Wait on socket receive queue for packet buffer(s)
*                   (c) Get remote host address                                         See Notes #4 & #5
*                   (d) Configure socket transmit :
*                       (1) Configure socket flags
*                   (e) Receive socket data from appropriate transport layer
*                   (f) Free    socket receive packet buffer(s)
*                   (g) Return  socket data received
*
*
* Argument(s) : sock_id             Socket descriptor/handle identifier of socket to receive data.
*               -------             Argument checked   in NetSock_RxDataHandler().
*
*               psock               Pointer to socket.
*               -----               Argument validated in NetSock_RxDataHandler().
*
*               pdata_buf           Pointer to an application data buffer that will receive the socket's received
*               ---------               data.
*
*                                   Argument checked   in NetSock_RxDataHandler().
*
*               data_buf_len        Size of the   application data buffer (in octets) [see Note #2b].
*               ------------        Argument checked   in NetSock_RxDataHandler().
*
*               flags               Flags to select receive options; bit-field flags logically OR'd :
*               -----
*                                       NET_SOCK_FLAG_NONE              No socket flags selected.
*                                       NET_SOCK_FLAG_RX_DATA_PEEK      Receive socket data without consuming
*                                                                           the socket data; i.e. socket data 
*                                                                           NOT removed from application receive
*                                                                           queue(s).
*                                       NET_SOCK_FLAG_RX_NO_BLOCK       Receive socket data without blocking
*                                                                           (see Note #3).
*
*                                   Argument checked   in NetSock_RxDataHandler().
*
*               paddr_remote        Pointer to an address buffer that will receive the socket address structure 
*                                       with the received data's remote address (see Notes #4 & #5), if NO errors.
*
*               paddr_len           Pointer to a variable to ... :
*
*                                       (a) Pass the size of the address buffer pointed to by 'paddr_remote'.
*                                       (b) (1) Return the actual size of socket address structure with the 
*                                                   received data's remote address, if NO errors;
*                                           (2) Return 0,                           otherwise.
*
*                                   See also Note #4b.
*
*               pip_opts_buf        Pointer to buffer to receive possible IP options (see Note #6a), if NO errors.
*
*               ip_opts_buf_len     Size of IP options receive buffer (in octets)    [see Note #6b].
*
*               pip_opts_len        Pointer to variable that will receive the return size of any received IP options,
*                                       if NO errors.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket data successfully received; check return
*                                                                       value for number of data octets received.
*
*                               NET_SOCK_ERR_INVALID_DATA_SIZE      Socket data receive buffer insufficient size;
*                                                                       some, but not all, socket data deframed
*                                                                       into receive buffer (see Note #2b).
*
*                               NET_SOCK_ERR_RX_Q_EMPTY             Socket receive queue empty.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*
*                               NET_ERR_RX                          Receive error.
*
*                                                                   --------- RETURNED BY NetOS_Lock() : ----------
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*
* Return(s)   : Number of positive data octets received, if NO errors (see Note #7a).
*
*               NET_SOCK_BSD_ERR_RX,                     otherwise    (see Note #7c).
*
* Caller(s)   : NetSock_RxDataHandler().
*$PAGE*
* Note(s)     : (2) (a) (1) Datagram-type sockets transmit & receive all data atomically -- i.e. every single, 
*                           complete datagram transmitted MUST be received as a single, complete datagram.
*
*                       (2) IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes that
*                           "for message-based sockets, such as SOCK_DGRAM ... the entire message shall be
*                           read in a single operation.  If a message is too long to fit in the supplied
*                           buffer, and MSG_PEEK is not set in the flags argument, the excess bytes shall
*                           be discarded".
*
*                   (b) Thus, if the socket's type is datagram & the receive data buffer size is NOT large
*                       enough for the received data, the receive data buffer is maximally filled with receive
*                       data but the remaining data octets are discarded & NET_SOCK_ERR_INVALID_DATA_SIZE
*                       error is returned.
*
*                   See also 'NetSock_RxDataHandler()  Note #2a'.
*
*               (3) If global socket blocking ('NET_SOCK_CFG_BLOCK_SEL') configured for blocking operation 
*                   ('NET_SOCK_BLOCK_SEL_BLOCK' or 'NET_SOCK_BLOCK_SEL_DFLT') but 'flags' argument set to 
*                    'NET_SOCK_FLAG_RX_NO_BLOCK'; socket receive does NOT block, regardless if the socket
*                   is configured to block.
*
*               (4) (a) If a pointer to remote address buffer is provided, it is assumed that the remote 
*                       address buffer has been previously validated for the remote address to be returned.
*
*                   (b) If a pointer to remote address buffer is provided, it is assumed that a pointer to
*                       an address length buffer is also available & has been previously validated.
*
*                   (c) The remote address is obtained from the first packet buffer.  In other words, if
*                       multiple packet buffers are received for a fragmented datagram, the remote address 
*                       is obtained from the first fragment of the datagram.
*
*               (5) (a) Socket address structure 'AddrFamily' member returned in host-order & SHOULD NOT 
*                       be converted to network-order.
*
*                   (b) Socket address structure addresses MUST be converted from host-order to network-
*                       order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (6) (a) If ...
*
*                       (1) NO IP options were received
*                             OR
*                       (2) NO IP options receive buffer is provided
*                             OR
*                       (3) IP options receive buffer NOT large enough for the received IP options
*
*                       ... then NO IP options are returned & any received IP options are silently discarded.
*
*                   (b) The IP options receive buffer size SHOULD be large enough to receive the maximum 
*                       IP options size, NET_IP_HDR_OPT_SIZE_MAX.
*
*                   (c) #### Received IP options should be provided/decoded via appropriate IP layer API.
*
*                   See also Note #14.
*$PAGE*
*               (7) IEEE Std 1003.1, 2004 Edition, Section 'recv() : RETURN VALUE' states that :
*
*                   (a) "Upon successful completion, recv() shall return the length of the message in bytes."
*
*                   (b) "If no messages are available to be received and the peer has performed an orderly
*                        shutdown, recv() shall return 0."
*
*                       (1) Since the socket receive return value of '0' is reserved for socket connection
*                           closes; NO socket receive -- fault or non-fault -- should ever return '0' octets
*                           received.
*
*                       (2) However, since NO actual connections are implemented for datagram-type sockets
*                           (see 'NetSock_ConnHandlerDatagram()  Note #2a'), NO actual socket connections
*                           can be closed on datagram-type sockets.  Therefore, datagram-type socket
*                           receives MUST NEVER return '0'.
*
*                   (c) (1) "Otherwise, -1 shall be returned" ...
*                       (2) "and 'errno' set to indicate the error."
*                           #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   See also 'NetSock_RxDataHandler()  Note #7'.
*
*               (8) Pointers to variables that return values MUST be initialized to return PRIOR to 
*                   all other validation or function handling in case of any error(s).
*
*                   However, these pointed-to variables SHOULD already be validated & initialized by
*                   previous NetSock_RxData() function(s).
*
*               (9) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*
*              (10) Default case already invalidated in NetSock_Open().  However, the default case 
*                   is included as an extra precaution in case 'Protocol' is incorrectly modified.
*
*              (11) (a) Transport layer SHOULD typically free &/or discard packet buffer(s) after
*                       receiving application data.
*
*                   (b) However, if received packet buffer(s) NOT consumed AND any socket receive
*                       queue error(s) occur; socket receive MUST free/discard packet buffer(s).
*
*              (12) On ANY errors, network resources MUST be appropriately freed :
*
*                   (a) NetSock_RxDataHandlerDatagram() assumes that datagram-type sockets have been 
*                       previously validated as configured by caller function(s).  Therefore, on any 
*                       internal socket connection error(s), the socket MUST be closed.
*
*              (13) Socket descriptor read availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor read handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also  Note #1a,
*                            'NetSock_SelDescHandlerRdDatagram()   Note #3',
*                          & 'NetSock_SelDescHandlerErrDatagram()  Note #3'.
*
*              (14) #### IP options arguments may NOT be necessary (remove if unnecessary).
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_SOCK_RTN_CODE  NetSock_RxDataHandlerDatagram (NET_SOCK_ID         sock_id,
                                                          NET_SOCK           *psock,
                                                          void               *pdata_buf,
                                                          CPU_INT16U          data_buf_len,
                                                          CPU_INT16S          flags,
                                                          NET_SOCK_ADDR      *paddr_remote,
                                                          NET_SOCK_ADDR_LEN  *paddr_len,
                                                          void               *pip_opts_buf,
                                                          CPU_INT08U          ip_opts_buf_len,
                                                          CPU_INT08U         *pip_opts_len,
                                                          NET_ERR            *perr)
{
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_SOCK_ADDR_IP  *paddr_ip;
#endif
#if (NET_SOCK_CFG_BLOCK_SEL != NET_SOCK_BLOCK_SEL_NO_BLOCK)
    CPU_BOOLEAN        no_block;
#endif
    CPU_BOOLEAN        block;
    CPU_BOOLEAN        peek;
    CPU_BOOLEAN        free;
    NET_BUF           *pbuf_head;
    NET_BUF           *pbuf_head_next;
    NET_BUF_HDR       *pbuf_head_hdr;
    NET_BUF_HDR       *pbuf_head_next_hdr;
    CPU_INT16U         flags_transport;
    CPU_INT16U         data_len_tot;
    NET_ERR            err;
    NET_ERR            err_rtn;


                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_CLOSED:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
            *perr =  NET_SOCK_ERR_INVALID_OP;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_BOUND:                              /* If sock NOT conn'd to remote addr       ...          */
             if (paddr_remote == (NET_SOCK_ADDR *)0) {          /* ... & remote addr rtn buf NOT provided, ...          */
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
                *perr =  NET_SOCK_ERR_INVALID_OP;               /* ... rtn invalid op err.                              */
                 return (NET_SOCK_BSD_ERR_RX);
             }
             break;


        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
             psock->State = NET_SOCK_STATE_CONN;
             break;


        case NET_SOCK_STATE_CONN:
             break;


        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr =  NET_SOCK_ERR_INVALID_STATE;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                /* ---------------- WAIT ON SOCK RX Q ----------------- */
#if (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_NO_BLOCK)
    block    = DEF_NO;

#else
    no_block = DEF_BIT_IS_SET(flags, NET_SOCK_FLAG_RX_NO_BLOCK);
    if (no_block == DEF_YES) {                                  /* If 'No Block' flag set, ...                          */
        block = DEF_NO;                                         /* ... do NOT block (see Note #3).                      */

    } else {                                                    
#if (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_BLOCK)        /* Else block if cfg'd, ...                             */
        block = DEF_YES;
#else                                                           /* ... or chk sock's block flag.                        */
        block = DEF_BIT_IS_SET(psock->Flags, NET_SOCK_FLAG_BLOCK);
#endif
    }
#endif


    if (block != DEF_YES) {                                     /* If non-blocking sock rx  ...                         */
        if (psock->RxQ_Head == (NET_BUF *)0) {                  /* ... & no rx'd data pkts, ...                         */
           *perr =  NET_SOCK_ERR_RX_Q_EMPTY;                    /* ... rtn  rx Q empty err.                             */
            return (NET_SOCK_BSD_ERR_RX);
        }
    }

    NetOS_Unlock();
    NetOS_Sock_RxQ_Wait(sock_id, perr);
    NetOS_Lock(&err);
    if ( err != NET_OS_ERR_NONE) {
       *perr  = err;                                            /* Rtn err from NetOS_Lock().                           */
        return (NET_SOCK_BSD_ERR_RX);
    }
    switch (*perr) {
        case NET_SOCK_ERR_NONE:
             break;


        case NET_SOCK_ERR_RX_Q_EMPTY:
                                                                /* Rtn err from NetOS_Sock_RxQ_Wait().                  */
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_ERR_RX_Q_SIGNAL_ABORT:
        case NET_SOCK_ERR_RX_Q_SIGNAL_FAULT:
        default:
            *perr =  NET_SOCK_ERR_FAULT;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }

    if (psock->RxQ_Head == (NET_BUF *)0) {                      /* If still NO rx'd data pkts, ...                      */
       *perr =  NET_SOCK_ERR_RX_Q_EMPTY;                        /* ... rtn rx Q empty err.                              */
        return (NET_SOCK_BSD_ERR_RX);
    }

                                                                /* Cfg rx pkt buf ptrs.                                 */
    pbuf_head      =  psock->RxQ_Head;
    pbuf_head_hdr  = &pbuf_head->Hdr;
    pbuf_head_next =  pbuf_head_hdr->NextPrimListPtr;



                                                                /* ----------------- GET REMOTE ADDR ------------------ */
    if (paddr_remote != (NET_SOCK_ADDR *)0) {                   /* If remote addr buf avail, ...                        */
                                                                /* ... rtn datagram src addr (see Note #4).             */
#if (NET_SOCK_CFG_FAMILY ==  NET_SOCK_FAMILY_IP_V4)
        paddr_ip         =  (NET_SOCK_ADDR_IP *)paddr_remote;   /* Cfg remote addr struct    (see Note #5).             */
        NET_UTIL_VAL_SET_HOST_16(&paddr_ip->AddrFamily, NET_SOCK_ADDR_FAMILY_IP_V4);
        NET_UTIL_VAL_COPY_SET_NET_16(&paddr_ip->Port,  &pbuf_head_hdr->TCP_UDP_PortSrc);
        NET_UTIL_VAL_COPY_SET_NET_32(&paddr_ip->Addr,  &pbuf_head_hdr->IP_AddrSrc);
        Mem_Clr((void     *)&paddr_ip->Unused[0],
                (CPU_SIZE_T) NET_SOCK_ADDR_IP_NBR_OCTETS_UNUSED);

       *paddr_len = NET_SOCK_ADDR_IP_SIZE;

#else                                                           /* See Notes #9 & #12a.                                 */
        NetSock_CloseSock(psock, DEF_YES, DEF_YES);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
       *perr =  NET_SOCK_ERR_INVALID_FAMILY;
        return (NET_SOCK_BSD_ERR_RX);
#endif
    }


/*$PAGE*/
                                                                /* ------------------- CFG SOCK RX -------------------- */
                                                                /* Cfg sock rx flags.                                   */
    peek = DEF_BIT_IS_SET(flags, NET_SOCK_FLAG_RX_DATA_PEEK);


                                                                /* ------------------- RX SOCK DATA ------------------- */
    data_len_tot = 0;
    err_rtn      = NET_SOCK_ERR_FAULT;

    switch (psock->Protocol) {                                  /* Rx app data from transport layer rx.                 */
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
        case NET_SOCK_PROTOCOL_UDP:
                                                                /* Cfg transport rx flags.                              */
             flags_transport = NET_UDP_FLAG_NONE;
             if (peek == DEF_YES) {
                 DEF_BIT_SET(flags_transport, NET_UDP_FLAG_RX_DATA_PEEK);
             }

             data_len_tot = NetUDP_RxAppData((NET_BUF    *) pbuf_head,
                                             (void       *) pdata_buf,
                                             (CPU_INT16U  ) data_buf_len,
                                             (CPU_INT16U  ) flags_transport,
                                             (void       *) pip_opts_buf,
                                             (CPU_INT08U  ) ip_opts_buf_len,
                                             (CPU_INT08U *) pip_opts_len,
                                             (NET_ERR    *)&err);
             switch (err) {
                 case NET_UDP_ERR_NONE:
                      err_rtn = NET_SOCK_ERR_NONE;
                      break;


                 case NET_UDP_ERR_INVALID_DATA_SIZE:
                      err_rtn = NET_SOCK_ERR_INVALID_DATA_SIZE;
                      break;


                 case NET_ERR_INIT_INCOMPLETE:
                 case NET_UDP_ERR_NULL_PTR:
                 case NET_UDP_ERR_INVALID_FLAG:
                 case NET_ERR_RX:
                 default:
                      err_rtn = NET_ERR_RX;
                      break;
             }
             break;
#endif


        case NET_SOCK_PROTOCOL_NONE:
        default:                                                /* See Notes #10 & #12a.                                */
            (void)&flags_transport;                             /* Prevent possible compiler warnings.                  */
            (void)&peek;
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
            *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }


    switch (err_rtn) {                                          /* Demux transport-to-sock layer err.                   */
        case NET_SOCK_ERR_NONE:
        case NET_SOCK_ERR_INVALID_DATA_SIZE:
             break;


        case NET_SOCK_ERR_FAULT:
        case NET_ERR_RX:
        default:
            *perr =  err_rtn;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                /* ---------------- FREE SOCK RX PKTS ----------------- */
    free = DEF_NO;                                              /* Transport layer SHOULD free pkts (see Note #11a).    */

    if (peek == DEF_YES) {                                      /* Signal sock rx Q to negate non-consuming peek.       */
        NetOS_Sock_RxQ_Signal(sock_id, &err);
        if (err != NET_SOCK_ERR_NONE) {                         /* If sock rx Q signal failed, ...                      */
            peek = DEF_NO;                                      /* ... consume pkt buf(s);     ...                      */
            free = DEF_YES;                                     /* ... i.e. free/discard pkt buf(s) [see Note #11b].    */
        }
    }

    if (peek != DEF_YES) {                                      /* If peek opt NOT req'd, pkt buf(s) consumed : ...     */
        if (pbuf_head_next != (NET_BUF *)0) {                   /* ... if rem'ing rx Q non-empty,               ...     */
                                                                /* ... unlink from prev'ly q'd pkt buf(s);      ...     */
            pbuf_head_next_hdr                  = (NET_BUF_HDR *)&pbuf_head_next->Hdr;
            pbuf_head_next_hdr->PrevPrimListPtr = (NET_BUF     *) 0;
        }
                                                                /* ... & set new sock rx Q head.                        */
        psock->RxQ_Head = (NET_BUF *)pbuf_head_next;
        if (psock->RxQ_Head == (NET_BUF *)0) {                  /* If head now  points to NULL, ..                      */
            psock->RxQ_Tail  = (NET_BUF *)0;                    /* .. tail also points to NULL.                         */
        }

        if (free == DEF_YES) {                                  /* If NOT freed by transport layer, ...                 */
            NetSock_RxPktDiscard(pbuf_head, perr);              /* ... free/discard pkt buf(s) [see Note #11b].         */
            return (NET_SOCK_BSD_ERR_RX);
        }
    }



                                                                /* ---------------- RTN RX'D SOCK DATA ---------------- */
    if (data_len_tot < 1) {                                     /* If rx'd data len < 1, ...                            */
       *perr =  NET_SOCK_ERR_RX_Q_EMPTY;                        /* ... rtn rx Q empty err (see Note #7b1).              */
        return (NET_SOCK_BSD_ERR_RX);
    }


   *perr =   err_rtn;

    return ((NET_SOCK_RTN_CODE)data_len_tot);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetSock_RxDataHandlerStream()
*
* Description : (1) Receive data from a stream-type socket :
*
*                   (a) Validate socket connection state                                See Note #11
*                   (b) Get remote host address                                         See Notes #4 & #5
*                   (c) Configure socket receive :
*                       (1) Get       socket's transport connection identification handler
*                       (2) Configure socket flags
*                   (d) Receive socket data from appropriate transport layer
*                   (e) Return  socket data received
*
*
* Argument(s) : sock_id             Socket descriptor/handle identifier of socket to receive data.
*               -------             Argument checked   in NetSock_RxDataHandler().
*
*               psock               Pointer to socket.
*               -----               Argument validated in NetSock_RxDataHandler().
*
*               pdata_buf           Pointer to an application data buffer that will receive the socket's received
*               ---------               data.
*
*                                   Argument checked   in NetSock_RxDataHandler().
*
*               data_buf_len        Size of the   application data buffer (in octets).
*               ------------        Argument checked   in NetSock_RxDataHandler().
*
*               flags               Flags to select receive options; bit-field flags logically OR'd :
*               -----
*                                       NET_SOCK_FLAG_NONE              No socket flags selected.
*                                       NET_SOCK_FLAG_RX_DATA_PEEK      Receive socket data without consuming
*                                                                           the socket data; i.e. socket data 
*                                                                           NOT removed from application receive
*                                                                           queue(s).
*                                       NET_SOCK_FLAG_RX_NO_BLOCK       Receive socket data without blocking
*                                                                           (see Note #3).
*
*                                   Argument checked   in NetSock_RxDataHandler().
*
*               paddr_remote        Pointer to an address buffer that will receive the socket address structure 
*                                       with the received data's remote address (see Notes #4 & #5), if NO errors.
*
*               paddr_len           Pointer to a variable to ... :
*                              
*                                       (a) Pass the size of the address buffer pointed to by 'paddr_remote'.
*                                       (b) (1) Return the actual size of socket address structure with the 
*                                                   received data's remote address, if NO errors;
*                                           (2) Return 0,                           otherwise.
*
*                                   See Note #4b.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket data successfully received; check return
*                                                                       value for number of data octets received.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address length.
*                               NET_SOCK_ERR_RX_Q_EMPTY             Socket receive queue empty.
*                               NET_SOCK_ERR_RX_Q_CLOSED            Socket receive queue closed.
*
*                               NET_ERR_RX                          Receive error.
*
*                                                                   ---- RETURNED BY NetConn_AddrRemoteGet() : ----
*                                                                   --- RETURNED BY NetConn_ID_TransportGet() : ---
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
* Return(s)   : Number of positive data octets received, if NO errors                (see Note #7a).
*
*               NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,       if socket connection closed (see Note #7b).
*
*               NET_SOCK_BSD_ERR_RX,                     otherwise                   (see Note #7c1).
*
* Caller(s)   : NetSock_RxDataHandler().
*$PAGE*
* Note(s)     : (2) (a) (1) Stream-type sockets transmit & receive all data octets in one or more non-
*                           distinct packets.  In other words, the application data is NOT bounded by 
*                           any specific packet(s); rather, it is contiguous & sequenced from one packet
*                           to the next.
*                   
*                       (2) IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes
*                           that "for stream-based sockets, such as SOCK_STREAM, message boundaries 
*                           shall be ignored.  In this case, data shall be returned to the user as
*                           soon as it becomes available, and no data shall be discarded".
*
*                   (b) (1) Thus, if the socket's type is stream & the receive data buffer size is NOT 
*                           large enough for the received data, the receive data buffer is maximally 
*                           filled with receive data & the remaining data octets remain queued for 
*                           later application-socket receives.
*
*                       (2) Therefore, a stream-type socket receive is signaled ONLY when data is 
*                           received for a socket connection where data was previously unavailable.
*
*                   (c) Consequently, it is typical -- but NOT absolutely required -- that a single 
*                       application task only receive or request to receive application data from a
*                       stream-type socket.
*
*                   See also 'NetSock_RxDataHandler()  Note #2b'.
*
*               (3) If global socket blocking ('NET_SOCK_CFG_BLOCK_SEL') configured for blocking operation 
*                   ('NET_SOCK_BLOCK_SEL_BLOCK' or 'NET_SOCK_BLOCK_SEL_DFLT') but 'flags' argument set to 
*                    'NET_SOCK_FLAG_TX_NO_BLOCK'; socket receive does NOT configure the transport layer 
*                   receive to block, regardless if the socket is configured to block.
*
*               (4) (a) If a pointer to remote address buffer is provided, it is assumed that the remote 
*                       address buffer has been previously validated for the remote address to be returned.
*
*                   (b) If a pointer to remote address buffer is provided, it is assumed that a pointer to
*                       an address length buffer is also available & has been previously validated.
*
*                   (c) The remote address is obtained from the socket connection's remote address.
*
*               (5) (a) Socket address structure 'AddrFamily' member returned in host-order & SHOULD NOT 
*                       be converted to network-order.
*
*                   (b) Socket address structure addresses MUST be converted from host-order to network-
*                       order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (6) (a) Socket connection addresses are maintained in network-order.
*
*                   (b) Since the port number & address are copied from a network-order multi-octet array 
*                       into local variables & then into a network-order socket address structure, they do
*                       NOT need to be converted from host-order to network-order.
*
*               (7) IEEE Std 1003.1, 2004 Edition, Section 'recv() : RETURN VALUE' states that :
*
*                   (a) "Upon successful completion, recv() shall return the length of the message in bytes."
*
*                   (b) "If no messages are available to be received and the peer has performed an orderly
*                        shutdown, recv() shall return 0."
*
*                       (1) Since the socket receive return value of '0' is reserved for socket connection
*                           closes; NO socket receive -- fault or non-fault -- should ever return '0' octets
*                           received.
*
*                   (c) (1) "Otherwise, -1 shall be returned" ...
*                       (2) "and 'errno' set to indicate the error."
*                           #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   See also 'NetSock_RxDataHandler()  Note #7'.
*
*               (8) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*
*               (9) Default case already invalidated in NetSock_Open().  However, the default case 
*                   is included as an extra precaution in case 'Protocol' is incorrectly modified.
*
*              (10) On ANY errors, network resources MUST be appropriately freed :
*
*                   (a) NetSock_RxDataHandlerStream() assumes that stream-type sockets have been previously
*                       validated as connected by caller function(s).  Therefore, on any internal socket
*                       connection error(s), the socket MUST be closed.
*
*                   (b) Since transport layer error(s) may NOT be critical &/or may be transitory, NO network
*                       or socket resource(s) are closed/freed.
*
*                   (c) If transport layer reports closed receive queue, socket layer is free to close/free
*                       ALL network or transport connection resource(s).
*
*                       See also 'net_tcp.c  NetTCP_RxAppData()  Note #3e3A1'.
*
*              (11) Socket descriptor read availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor read handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also  Note #1a,
*                            'NetSock_SelDescHandlerRdStream()   Note #3',
*                          & 'NetSock_SelDescHandlerErrStream()  Note #3'.
*
*              (12) 'sock_id' may NOT be necessary but included for consistency.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_SOCK_RTN_CODE  NetSock_RxDataHandlerStream (NET_SOCK_ID         sock_id,
                                                        NET_SOCK           *psock,
                                                        void               *pdata_buf,
                                                        CPU_INT16U          data_buf_len,
                                                        CPU_INT16S          flags,
                                                        NET_SOCK_ADDR      *paddr_remote,
                                                        NET_SOCK_ADDR_LEN  *paddr_len,
                                                        NET_ERR            *perr)
{
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_SOCK_ADDR_IP   *paddr_ip;
    NET_IP_ADDR         remote_addr;
#endif
#if (NET_SOCK_CFG_BLOCK_SEL != NET_SOCK_BLOCK_SEL_NO_BLOCK)
    CPU_BOOLEAN         no_block;
#endif
    CPU_BOOLEAN         block;
    CPU_BOOLEAN         peek;
    NET_CONN_ID         conn_id;
    NET_CONN_ID         conn_id_transport;
    NET_PORT_NBR        remote_port;
    NET_SOCK_ADDR_LEN   addr_len;
    CPU_INT08U          addr_remote[NET_SOCK_CFG_ADDR_LEN];
    CPU_INT16U          flags_transport;
    CPU_INT16U          data_len_tot;
    NET_ERR             err;
    NET_ERR             err_rtn;


   (void)&sock_id;                                              /* Prevent compiler warning (see Note #12).             */

                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_LISTEN:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
            *perr =  NET_SOCK_ERR_INVALID_OP;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_CONN_DONE:
             psock->State = NET_SOCK_STATE_CONN;
             break;


        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
             break;


        case NET_SOCK_STATE_NONE:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr =  NET_SOCK_ERR_INVALID_STATE;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                /* ----------------- GET REMOTE ADDR ------------------ */
    conn_id = psock->ID_Conn;
    if (paddr_remote != (NET_SOCK_ADDR *)0) {                   /* If remote addr buf avail, ...                        */
                                                                /* ... rtn sock conn's remote addr (see Note #4c).      */
        addr_len = sizeof(addr_remote);
        NetConn_AddrRemoteGet((NET_CONN_ID        ) conn_id,
                              (CPU_INT08U        *)&addr_remote[0],
                              (NET_CONN_ADDR_LEN *)&addr_len,
                              (NET_ERR           *) perr);
        if (*perr != NET_CONN_ERR_NONE) {                       /* See Note #10a.                                       */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             return (NET_SOCK_BSD_ERR_RX);
        }
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        if (addr_len != NET_SOCK_CFG_ADDR_LEN) {                /* See Note #10a.                                       */
            NetSock_CloseSock(psock, DEF_YES, DEF_YES);
           *perr =  NET_SOCK_ERR_INVALID_ADDR_LEN;
            return (NET_SOCK_BSD_ERR_RX);
        }
#endif

#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
        Mem_Copy((void     *)&remote_port,
                 (void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_PORT],
                 (CPU_SIZE_T) sizeof(remote_port));
        Mem_Copy((void     *)&remote_addr,
                 (void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_ADDR],
                 (CPU_SIZE_T) sizeof(remote_addr));


        paddr_ip  = (NET_SOCK_ADDR_IP *)paddr_remote;           /* Cfg remote addr struct   (see Note #5).              */
        NET_UTIL_VAL_SET_HOST_16(&paddr_ip->AddrFamily, NET_SOCK_ADDR_FAMILY_IP_V4);
        NET_UTIL_VAL_COPY_16(&paddr_ip->Port, &remote_port);    /* Copy preserves net-order (see Note #6b).             */
        NET_UTIL_VAL_COPY_32(&paddr_ip->Addr, &remote_addr);    /* Copy preserves net-order (see Note #6b).             */
        Mem_Clr((void     *) &paddr_ip->Unused[0],
                (CPU_SIZE_T)  NET_SOCK_ADDR_IP_NBR_OCTETS_UNUSED);

       *paddr_len =  NET_SOCK_ADDR_IP_SIZE;

#else                                                           /* See Notes #8 & #10a.                                 */
        NetSock_CloseSock(psock, DEF_YES, DEF_YES);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
       *perr =  NET_SOCK_ERR_INVALID_FAMILY;
        return (NET_SOCK_BSD_ERR_RX);
#endif
    }


                                                                /* ------------------- CFG SOCK RX -------------------- */
                                                                /* Get transport conn id.                               */
    conn_id_transport = NetConn_ID_TransportGet(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {                           /* See Note #10a.                                       */
         NetSock_CloseSock(psock, DEF_YES, DEF_YES);
         return (NET_SOCK_BSD_ERR_RX);
    }
    if  (conn_id_transport == NET_CONN_ID_NONE) {               /* See Note #10a.                                       */
         NetSock_CloseSock(psock, DEF_YES, DEF_YES);
        *perr =  NET_SOCK_ERR_CONN_FAIL;
         return (NET_SOCK_BSD_ERR_RX);
    }


                                                                /* Cfg sock rx flags.                                   */
#if (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_NO_BLOCK)
    block    = DEF_NO;

#else
    no_block = DEF_BIT_IS_SET(flags, NET_SOCK_FLAG_RX_NO_BLOCK);
    if (no_block == DEF_YES) {                                  /* If 'No Block' flag set, ...                          */
        block = DEF_NO;                                         /* ... do NOT block (see Note #3).                      */

    } else {                                                    
#if (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_BLOCK)        /* Else block if cfg'd, ...                             */
        block = DEF_YES;
#else                                                           /* ... or chk sock's block flag.                        */
        block = DEF_BIT_IS_SET(psock->Flags, NET_SOCK_FLAG_BLOCK);
#endif
    }
#endif

    peek = DEF_BIT_IS_SET(flags, NET_SOCK_FLAG_RX_DATA_PEEK);



/*$PAGE*/
                                                                /* ------------------- RX SOCK DATA ------------------- */
    data_len_tot = 0;
    err_rtn      = NET_SOCK_ERR_FAULT;

    switch (psock->Protocol) {                                  /* Rx app data from transport layer rx.                 */
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
        case NET_SOCK_PROTOCOL_TCP:
                                                                /* Cfg transport rx flags.                              */
             flags_transport = NET_TCP_FLAG_NONE;
             if (block == DEF_YES) {
                 DEF_BIT_SET(flags_transport, NET_TCP_FLAG_RX_BLOCK);
             }
             if (peek  == DEF_YES) {
                 DEF_BIT_SET(flags_transport, NET_TCP_FLAG_RX_DATA_PEEK);
             }

             data_len_tot = NetTCP_RxAppData((NET_TCP_CONN_ID) conn_id_transport,
                                             (void          *) pdata_buf,
                                             (CPU_INT16U     ) data_buf_len,
                                             (CPU_INT16U     ) flags_transport,
                                             (NET_ERR       *)&err);
             switch (err) {
                 case NET_TCP_ERR_NONE:
                      err_rtn = NET_SOCK_ERR_NONE;
                      break;


                 case NET_TCP_ERR_RX_Q_EMPTY:
                      err_rtn = NET_SOCK_ERR_RX_Q_EMPTY;
                      break;


                 case NET_TCP_ERR_RX_Q_CLOSED:
                      err_rtn = NET_SOCK_ERR_RX_Q_CLOSED;
                      break;


                 case NET_TCP_ERR_INVALID_CONN:
                 case NET_TCP_ERR_INVALID_CONN_OP:
                 case NET_TCP_ERR_INVALID_CONN_STATE:
                 case NET_TCP_ERR_CONN_NOT_USED:
                 case NET_TCP_ERR_CONN_FAIL:
                 case NET_TCP_ERR_CONN_DATA_INVALID:
                 case NET_TCP_ERR_RX_Q_SIGNAL_ABORT:
                 case NET_TCP_ERR_RX_Q_SIGNAL_FAULT:
                      err_rtn = NET_SOCK_ERR_FAULT;
                      break;


                 case NET_ERR_INIT_INCOMPLETE:
                 case NET_OS_ERR_LOCK:
                 default:
                      err_rtn = NET_ERR_RX;
                      break;
             }
             break;
#endif
#endif

        case NET_SOCK_PROTOCOL_NONE:
        default:                                                /* See Notes #9 & #10a.                                 */
            (void)&conn_id_transport;                           /* Prevent possible compiler warnings.                  */
            (void)&flags_transport;
            (void)&err;
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
            *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }


/*$PAGE*/
    switch (err_rtn) {                                          /* Demux transport-to-sock layer err.                   */
        case NET_SOCK_ERR_NONE:
             break;


        case NET_SOCK_ERR_RX_Q_CLOSED:                          /* If transport layer rx Q closed,               ...    */
             if (psock->State == NET_SOCK_STATE_CLOSING_DATA_AVAIL) {
                 NetSock_CloseHandler(psock, DEF_YES, DEF_YES); /* ... close/free sock's conn(s) [see Note #10c] ...    */
             }
            *perr =  err_rtn;                                   /* ... & rtn closed code         (see Note #7b).        */
             return (NET_SOCK_BSD_RTN_CODE_CONN_CLOSED);        /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_ERR_RX_Q_EMPTY:
        case NET_SOCK_ERR_FAULT:
        case NET_ERR_RX:
        default:
            *perr =  err_rtn;
             return (NET_SOCK_BSD_ERR_RX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }



                                                                /* ---------------- RTN RX'D SOCK DATA ---------------- */
    if (data_len_tot < 1) {                                     /* If rx'd data len < 1, ...                            */
       *perr =  NET_SOCK_ERR_RX_Q_EMPTY;                        /* ... rtn rx Q empty err (see Note #7b1).              */
        return (NET_SOCK_BSD_ERR_RX);
    }


   *perr =   err_rtn;

    return ((NET_SOCK_RTN_CODE)data_len_tot);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetSock_TxDataHandler()
*
* Description : (1) Transmit data through a socket :
*
*                   (a) Validate transmit data                                          See Note #2
*                   (b) Validate transmit flags                                         See Note #3
*                   (c) Acquire  network  lock
*                   (d) Validate socket   used
*                   (e) Validate remote   address                                       See Note #4
*                   (f) Transmit socket   data
*                   (g) Release  network  lock
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to transmit data.
*
*               p_data          Pointer to application data to transmit.
*
*               data_len        Length of  application data to transmit (in octets) [see Note #2].
*
*               flags           Flags to select transmit options (see Note #3); bit-field flags logically OR'd :
*
*                                   NET_SOCK_FLAG_NONE              No socket flags selected.
*                                   NET_SOCK_FLAG_TX_NO_BLOCK       Transmit socket data without blocking.
*
*               paddr_remote    Pointer to destination address buffer (see Note #4).
*
*               addr_len        Length of  destination address buffer (in octets).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NULL_PTR               Argument 'p_data' passed a NULL pointer.
*                               NET_SOCK_ERR_INVALID_TYPE           Invalid socket type.
*                               NET_SOCK_ERR_INVALID_FLAG           Invalid socket flags.
*                               NET_SOCK_ERR_INVALID_DATA_SIZE      Invalid data size (see Notes #2b & #2a1B2).
*
*                                                                   --------- RETURNED BY NetSock_IsUsed() : ---------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK           Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*
*                                                                   --- RETURNED BY NetSock_IsValidAddrRemote() : ----
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket address family.
*                               NET_SOCK_ERR_INVALID_CONN           Invalid socket connection.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address structure length.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid socket address.
*                               NET_SOCK_ERR_INVALID_PORT_NBR       Invalid socket port number.
*
*                                                                   - RETURNED BY NetSock_TxDataHandlerDatagram() : --
*                                                                   -- RETURNED BY NetSock_TxDataHandlerStream() : ---
*                               NET_SOCK_ERR_NONE                   Socket data successfully transmitted; check return
*                                                                       value for number of data octets transmitted.
*
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*
*                               NET_ERR_TX                          Transitory transmit error.
*
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*
*                                                                   - RETURNED BY NetSock_TxDataHandlerDatagram() : --
*                               NET_SOCK_ERR_ADDR_IN_USE            Socket address already in use.
*                               NET_SOCK_ERR_PORT_NBR_NONE_AVAIL    Port number NOT available.
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host address.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_NONE_AVAIL             NO available connections to allocate.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_TYPE           Invalid network connection type.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
*                                                                   -- RETURNED BY NetSock_TxDataHandlerStream() : ---
*                               NET_SOCK_ERR_TX_Q_CLOSED            Socket transmit queue closed.
*
*                                                                   ----------- RETURNED BY NetOS_Lock() : -----------
*                               NET_OS_ERR_LOCK                     Network access NOT acquired.
*$PAGE*
* Return(s)   : Number of positive data octets transmitted, if NO errors                (see Note #5a1).
*
*               NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,          if socket connection closed (see Note #5b).
*
*               NET_SOCK_BSD_ERR_TX,                        otherwise                   (see Note #5a2A).
*
* Caller(s)   : sendto().
*
* Note(s)     : (2) (a) (1) (A) Datagram-type sockets transmit & receive all data atomically -- i.e. every
*                               single, complete datagram transmitted MUST be received as a single, complete
*                               datagram.  Thus, each call to transmit data MUST be transmitted in a single, 
*                               complete datagram.
*
*                           (B) (1) IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states
*                                   that "if the message is too long to pass through the underlying protocol,
*                                   send() shall fail and no data shall be transmitted".
*
*                               (2) Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h 
*                                   Note #1e'), if the socket's type is datagram & the requested transmit 
*                                   data length is greater than the socket/transport layer MTU, then NO data 
*                                   is transmitted & NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
*
*                       (2) (A) (1) Stream-type sockets transmit & receive all data octets in one or more 
*                                   non-distinct packets.  In other words, the application data is NOT 
*                                   bounded by any specific packet(s); rather, it is contiguous & sequenced 
*                                   from one packet to the next.
*
*                               (2) Thus, if the socket's type is stream & the socket's transmit data queue(s)
*                                   are NOT large enough for the transmitted data, the  transmit data queue(s) 
*                                   are maximally filled with transmit data & the remaining data octets are
*                                   discarded but may be re-transmitted by later application-socket transmits.
*
*                               (3) Therefore, NO stream-type socket transmit data length should be "too long
*                                   to pass through the underlying protocol" & cause the socket transmit to 
*                                   "fail ... [with] no data ... transmitted" (see Note #2a1B1).
*
*                           (B) Thus, it is typical -- but NOT absolutely required -- that a single application
*                               task ONLY transmit or request to transmit data to a stream-type socket.
*
*                   (b) 'data_len' of 0 octets NOT allowed.
*
*                   See also 'NetSock_TxDataHandlerDatagram()  Note #2',
*                          & 'NetSock_TxDataHandlerStream()    Note #3'.
*
*               (3) #### Only some socket transmit flag options are implemented.  If other flag options are 
*                   requested, NetSock_TxData() handler function(s) abort & return appropriate error codes
*                   so that requested flag options are NOT silently ignored.
*
*               (4) (a) Socket address structure 'AddrFamily' member MUST be configured in host-order & 
*                       MUST NOT be converted to/from network-order.
*
*                   (b) Socket address structure addresses MUST be configured/converted from host-order
*                       to network-order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (5) (a) IEEE Std 1003.1, 2004 Edition, Section 'send() : RETURN VALUE' states that :
*
*                       (1) "Upon successful completion, send() shall return the number of bytes sent."
*
*                           (A) Section 'send() : DESCRIPTION' elaborates that "successful completion 
*                               of a call to sendto() does not guarantee delivery of the message".
*
*                           (B) (1) Thus, applications SHOULD verify the actual returned number of data 
*                                   octets transmitted &/or prepared for transmission.
*
*                               (2) In addition, applications MAY desire verification of receipt &/or
*                                   acknowledgement of transmitted data to the remote host -- either 
*                                   inherently by the transport layer or explicitly by the application.
*
*                       (2) (A) "Otherwise, -1 shall be returned" ...
*                               (1) Section 'send() : DESCRIPTION' elaborates that "a return value of 
*                                   -1 indicates only locally-detected errors".
*
*                           (B) "and 'errno' set to indicate the error."
*                               #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   (b) ???? Although NO socket send() specification states to return '0' when the socket's
*                       connection is closed, it seems reasonable to return '0' since it is possible for the
*                       socket connection to be close()'d or shutdown() by the remote host.
*
*                       (1) Since the socket transmit return value of '0' is reserved for socket connection
*                           closes; NO socket transmit -- fault or non-fault -- should ever return '0' octets
*                           transmitted.
*
*                   See also 'NetSock_TxDataHandlerDatagram()  Note #6'
*                          & 'NetSock_TxDataHandlerStream()    Note #5'.
*
*               (6) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'SockType' is incorrectly modified.
**********************************************************************************************************
*/
/*$PAGE*/
static  NET_SOCK_RTN_CODE  NetSock_TxDataHandler (NET_SOCK_ID         sock_id,
                                                  void               *p_data,
                                                  CPU_INT16U          data_len,
                                                  CPU_INT16S          flags,
                                                  NET_SOCK_ADDR      *paddr_remote,
                                                  NET_SOCK_ADDR_LEN   addr_len,
                                                  NET_ERR            *perr)
{
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    CPU_BOOLEAN         valid;
    CPU_INT16U          flag_mask;
#endif
    NET_SOCK           *psock;
    NET_SOCK_RTN_CODE   rtn_code;



#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
                                                                /* ----------------- VALIDATE TX DATA ----------------- */
    if (p_data == (void *)0) {                                  /* Validate data ptr.                                   */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNullPtrCtr);
       *perr =  NET_SOCK_ERR_NULL_PTR;
        return (NET_SOCK_BSD_ERR_TX);
    }
    if (data_len < 1) {                                         /* Validate data len (see Note #2b).                    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrTxInvalidSizeCtr);
       *perr =  NET_SOCK_ERR_INVALID_DATA_SIZE;
        return (NET_SOCK_BSD_ERR_TX);
    }

                                                                /* ----------------- VALIDATE TX FLAGS ---------------- */
    flag_mask = NET_SOCK_FLAG_NONE       |
                NET_SOCK_FLAG_TX_NO_BLOCK;
                                                                /* If any invalid flags req'd, rtn err (see Note #3).   */
    if (((CPU_INT16U)flags & ~flag_mask) != NET_SOCK_FLAG_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFlagsCtr);
       *perr =  NET_SOCK_ERR_INVALID_FLAG;
        return (NET_SOCK_BSD_ERR_TX);
    }
#endif


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_TX);
    }



#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SOCK USED ---------------- */
   (void)NetSock_IsUsed(sock_id, perr);
    if (*perr != NET_SOCK_ERR_NONE) {
         NetOS_Unlock();
         return (NET_SOCK_BSD_ERR_TX);
    }
#endif


    psock = &NetSock_Tbl[sock_id];

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* --------------- VALIDATE REMOTE ADDR --------------- */
    if (paddr_remote != (NET_SOCK_ADDR *)0) {
        valid = NetSock_IsValidAddrRemote(paddr_remote, addr_len, psock, perr);
        if (valid != DEF_YES) {
            NetOS_Unlock();
            return (NET_SOCK_BSD_ERR_TX);
        }
    }
#endif


/*$PAGE*/
                                                                /* ------------------- TX SOCK DATA ------------------- */
    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             rtn_code = NetSock_TxDataHandlerDatagram((NET_SOCK_ID    )sock_id,
                                                      (NET_SOCK      *)psock,
                                                      (void          *)p_data,
                                                      (CPU_INT16U     )data_len,
                                                      (CPU_INT16S     )flags,
                                                      (NET_SOCK_ADDR *)paddr_remote,
                                                      (NET_ERR       *)perr);
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             rtn_code = NetSock_TxDataHandlerStream((NET_SOCK_ID  )sock_id,
                                                    (NET_SOCK    *)psock,
                                                    (void        *)p_data,
                                                    (CPU_INT16U   )data_len,
                                                    (CPU_INT16S   )flags,
                                                    (NET_ERR     *)perr);
             break;
#endif

        case NET_SOCK_TYPE_NONE:
        case NET_SOCK_TYPE_FAULT:
        default:                                                /* See Note #6.                                         */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidSockTypeCtr);
             NetOS_Unlock();
            *perr =  NET_SOCK_ERR_INVALID_TYPE;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


    return (rtn_code);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetSock_TxDataHandlerDatagram()
*
* Description : (1) Transmit data through a datagram-type socket :
*
*                   (a) Validate  socket connection state                               See Note #10
*                   (b) Configure socket transmit :
*                       (1) Configure source/destination addresses for transmit
*                       (2) Configure socket flags
*                   (c) Transmit  socket data via appropriate transport layer
*                   (d) Return    socket data transmitted length
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to receive data.
*               -------         Argument checked   in NetSock_TxDataHandler().
*
*               psock           Pointer to socket.
*               -----           Argument validated in NetSock_TxDataHandler().
*
*               p_data          Pointer to application data.
*               ------          Argument checked   in NetSock_TxDataHandler().
*
*               data_len        Length of  application data (in octets) [see Note #2].
*               --------        Argument checked   in NetSock_TxDataHandler().
*
*               flags           Flags to select transmit options; bit-field flags logically OR'd :
*               -----
*                                   NET_SOCK_FLAG_NONE              No socket flags selected.
*                                   NET_SOCK_FLAG_TX_NO_BLOCK       Transmit socket data without blocking
*                                                                       (see Note #3).
*
*                               Argument checked   in NetSock_TxDataHandler().
*
*               paddr_remote    Pointer to destination address buffer (see Note #4).
*               ------------    Argument checked   in NetSock_TxDataHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket data successfully transmitted.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_INVALID_FAMILY         Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_ADDR_LEN       Invalid socket address length.
*                               NET_SOCK_ERR_INVALID_DATA_SIZE      Invalid data size (see Notes #2b & #2a2B).
*
*                               NET_ERR_TX                          Transitory transmit error.
*
*                                                                   ----- RETURNED BY NetSock_BindHandler() : -----
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_ADDR           Invalid socket address.
*                               NET_SOCK_ERR_ADDR_IN_USE            Socket address already in use.
*                               NET_SOCK_ERR_PORT_NBR_NONE_AVAIL    Port number NOT available.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*                               NET_IP_ERR_INVALID_ADDR_HOST        Invalid IP host address.
*                               NET_CONN_ERR_NONE_AVAIL             NO available connections to allocate.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_TYPE           Invalid network connection type.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
*                                                                   ---- RETURNED BY NetConn_AddrLocalGet() : -----
*                                                                   ---- RETURNED BY NetConn_AddrRemoteGet() : ----
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection(s) NOT currently used.
*                               NET_CONN_ERR_NULL_PTR               Argument(s) passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
* Return(s)   : Number of positive data octets transmitted, if NO errors                (see Note #6a1).
*
*               NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,          if socket connection closed (see Note #6b).
*
*               NET_SOCK_BSD_ERR_TX,                        otherwise                   (see Note #6a2A).
*
* Caller(s)   : NetSock_TxDataHandler().
*$PAGE*
* Note(s)     : (2) (a) (1) Datagram-type sockets transmit & receive all data atomically -- i.e. every
*                           single, complete datagram transmitted MUST be received as a single, complete
*                           datagram.  Thus, each call to transmit data MUST be transmitted in a single, 
*                           complete datagram.
*
*                       (2) (A) IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states that
*                               "if the message is too long to pass through the underlying protocol, send() 
*                               shall fail and no data shall be transmitted".
*
*                           (B) Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h 
*                               Note #1e'), if the socket's type is datagram & the requested transmit 
*                               data length is greater than the socket/transport layer MTU, then NO data 
*                               is transmitted & NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
*
*                   (b) 'data_len' of 0 octets NOT allowed.
*
*                   See also 'NetSock_TxDataHandler()  Note #2a'.
*
*               (3) If global socket blocking ('NET_SOCK_CFG_BLOCK_SEL') configured for blocking operation 
*                   ('NET_SOCK_BLOCK_SEL_BLOCK' or 'NET_SOCK_BLOCK_SEL_DFLT') but 'flags' argument set to 
*                    'NET_SOCK_FLAG_TX_NO_BLOCK'; socket transmit does NOT configure the transport layer 
*                   transmit to block, regardless if the socket is configured to block.
*
*               (4) If a pointer to remote address buffer is provided, it is assumed that the remote 
*                   address buffer & remote address buffer length have been previously validated.
*
*               (5) (a) A socket's local address MUST be available in order to transmit.
*
*                   (b) Since only a single host address is supported (see 'net_ip.h  Note #1a'), always
*                       use this single host address for the socket's local address when transmitting
*                       packets, regardless of whether a protocol's wildcard address is specified in the 
*                       socket connection's local address structure.
*
*               (6) (a) IEEE Std 1003.1, 2004 Edition, Section 'send() : RETURN VALUE' states that :
*
*                       (1) "Upon successful completion, send() shall return the number of bytes sent."
*
*                           (A) Section 'send() : DESCRIPTION' elaborates that "successful completion 
*                               of a call to sendto() does not guarantee delivery of the message".
*
*                           (B) (1) Thus, applications SHOULD verify the actual returned number of data 
*                                   octets transmitted &/or prepared for transmission.
*
*                               (2) In addition, applications MAY desire verification of receipt &/or
*                                   acknowledgement of transmitted data to the remote host -- either 
*                                   inherently by the transport layer or explicitly by the application.
*
*                       (2) (A) "Otherwise, -1 shall be returned" ...
*                               (1) Section 'send() : DESCRIPTION' elaborates that "a return value of 
*                                   -1 indicates only locally-detected errors".
*
*                           (B) "and 'errno' set to indicate the error."
*                               #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   (b) ???? Although NO socket send() specification states to return '0' when the socket's
*                       connection is closed, it seems reasonable to return '0' since it is possible for the
*                       socket connection to be close()'d or shutdown() by the remote host.
*
*                       (1) Since the socket transmit return value of '0' is reserved for socket connection
*                           closes; NO socket transmit -- fault or non-fault -- should ever return '0' octets
*                           transmitted.
*
*                       (2) However, since NO actual connections are implemented for datagram-type sockets
*                           (see 'NetSock_ConnHandlerDatagram()  Note #2a'), NO actual socket connections
*                           can be closed on datagram-type sockets.  Therefore, datagram-type socket
*                           transmits MUST NEVER return '0'.
*
*                   See also 'NetSock_TxDataHandler()  Note #5'.
*$PAGE*
*               (7) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*
*               (8) Default case already invalidated in NetSock_Open().  However, the default case 
*                   is included as an extra precaution in case 'Protocol' is incorrectly modified.
*
*               (9) On ANY errors, network resources MUST be appropriately freed :
*
*                   (a) NetSock_TxDataHandlerDatagram() assumes that internal socket configuration
*                       has been previously validated by caller function(s).  Therefore, on any
*                       internal socket configuration error(s), the socket MUST be closed.
*
*                   (b) NetSock_TxDataHandlerDatagram() assumes that any internal socket connection 
*                       error(s) on datagram-type sockets may NOT be critical &/or may be transitory;
*                       thus NO network or socket resource(s) are closed/freed.
*
*                   (c) Since transport layer error(s) may NOT be critical &/or may be transitory, NO 
*                       network or socket resource(s) are closed/freed.
*
*              (10) Socket descriptor write availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor write handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also  Note #1a,
*                            'NetSock_SelDescHandlerWrDatagram()   Note #3',
*                          & 'NetSock_SelDescHandlerErrDatagram()  Note #3'.
*
*              (11) (a) RFC #1122, Section 4.1.4 states that "an application-layer program MUST be able 
*                       to set the TTL and TOS values as well as IP options for sending a ... datagram,
*                       and these values must be passed transparently to the IP layer".
*
*                   (b) #### IP layer parameters/options NOT yet implemented.
*
*              (12) 'sock_id' may NOT be necessary but included for consistency.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_SOCK_RTN_CODE  NetSock_TxDataHandlerDatagram (NET_SOCK_ID     sock_id,
                                                          NET_SOCK       *psock,
                                                          void           *p_data,
                                                          CPU_INT16U      data_len,
                                                          CPU_INT16S      flags,
                                                          NET_SOCK_ADDR  *paddr_remote,
                                                          NET_ERR        *perr)
{
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    NET_SOCK_ADDR_IP   *paddr_ip;
    NET_IP_ADDR         src_addr;
    NET_IP_ADDR         dest_addr;
#endif
#if (NET_SOCK_CFG_BLOCK_SEL != NET_SOCK_BLOCK_SEL_NO_BLOCK)
    CPU_BOOLEAN         no_block;
#endif
    CPU_BOOLEAN         block;
    NET_CONN_ID         conn_id;
    CPU_INT08U          addr_local[NET_SOCK_CFG_ADDR_LEN];
    CPU_INT08U          addr_remote[NET_SOCK_CFG_ADDR_LEN];
    NET_SOCK_ADDR_LEN   addr_len;
    NET_PORT_NBR        src_port;
    NET_PORT_NBR        dest_port;
    CPU_INT16U          flags_transport;
    CPU_INT16U          data_len_tot;
    NET_ERR             err;
    NET_ERR             err_rtn;


   (void)&sock_id;                                              /* Prevent compiler warning (see Note #12).             */

                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_CLOSED:                             /* If sock closed, bind to random local addr.           */
            (void)NetSock_BindHandler((NET_SOCK_ID      )sock_id,
                                      (NET_SOCK_ADDR   *)0,
                                      (NET_SOCK_ADDR_LEN)0,
                                      (CPU_BOOLEAN      )DEF_YES,
                                      (NET_ERR         *)perr);
             if (*perr != NET_SOCK_ERR_NONE) {
                  return (NET_SOCK_BSD_ERR_TX);
             }
             break;


        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_CONN:
             break;


        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
             psock->State = NET_SOCK_STATE_CONN;
             break;


        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr =  NET_SOCK_ERR_INVALID_STATE;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                /* -------------- CFG TX SRC/DEST ADDRS --------------- */
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    conn_id  = psock->ID_Conn;

                                                                /* Cfg local/src addr (see Note #5).                    */
    addr_len = sizeof(addr_local);
    NetConn_AddrLocalGet((NET_CONN_ID        ) conn_id,
                         (CPU_INT08U        *)&addr_local[0],
                         (NET_CONN_ADDR_LEN *)&addr_len,
                         (NET_ERR           *) perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return (NET_SOCK_BSD_ERR_TX);
    }
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (addr_len != NET_SOCK_CFG_ADDR_LEN) {                    /* See Note #9a.                                        */
        NetSock_CloseSock(psock, DEF_YES, DEF_YES);
       *perr =  NET_SOCK_ERR_INVALID_ADDR_LEN;
        return (NET_SOCK_BSD_ERR_TX);
    }
#endif
    Mem_Copy((void     *)&src_port,
             (void     *)&addr_local[NET_SOCK_ADDR_IP_IX_PORT],
             (CPU_SIZE_T) sizeof(src_port));
    Mem_Copy((void     *)&src_addr,
             (void     *)&addr_local[NET_SOCK_ADDR_IP_IX_ADDR],
             (CPU_SIZE_T) sizeof(src_addr));

    src_port = NET_UTIL_NET_TO_HOST_16(src_port);
    src_addr = NET_UTIL_NET_TO_HOST_32(src_addr);

    if (src_addr == NET_SOCK_ADDR_IP_WILD_CARD) {               /* If wildcard addr, ...                                */
        src_addr  = NetIP_GetAddrThisHostHandler(perr);         /* ... subst this host's IP addr (see Note #5b).        */
#if 0                                                           /* #### DHCPc IP addr init req's NULL src addr.         */
        if (*perr != NET_IP_ERR_NONE) {
             return (NET_SOCK_BSD_ERR_TX);
        }
#endif
    }
    
                                                                /* Cfg  remote/dest addr.                               */
    if (paddr_remote == (NET_SOCK_ADDR *)0) {                   /* If remote addr NOT provided, ...                     */
                                                                /* ... get sock conn's remote addr.                     */
        addr_len = sizeof(addr_remote);
        NetConn_AddrRemoteGet((NET_CONN_ID        ) conn_id,
                              (CPU_INT08U        *)&addr_remote[0],
                              (NET_CONN_ADDR_LEN *)&addr_len,
                              (NET_ERR           *) perr);
        if (*perr != NET_CONN_ERR_NONE) {
             return (NET_SOCK_BSD_ERR_TX);
        }
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        if (addr_len != NET_SOCK_CFG_ADDR_LEN) {                /* See Note #9a.                                        */
            NetSock_CloseSock(psock, DEF_YES, DEF_YES);
           *perr =  NET_SOCK_ERR_INVALID_ADDR_LEN;
            return (NET_SOCK_BSD_ERR_TX);
        }
#endif
        Mem_Copy((void     *)&dest_port,
                 (void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_PORT],
                 (CPU_SIZE_T) sizeof(dest_port));
        Mem_Copy((void     *)&dest_addr,
                 (void     *)&addr_remote[NET_SOCK_ADDR_IP_IX_ADDR],
                 (CPU_SIZE_T) sizeof(dest_addr));

        dest_port =  NET_UTIL_NET_TO_HOST_16(dest_port);
        dest_addr =  NET_UTIL_NET_TO_HOST_32(dest_addr);


    } else {                                                    /* Else cfg remote addr (see Note #4).                  */
        paddr_ip  = (NET_SOCK_ADDR_IP *)paddr_remote;
        dest_port =  NET_UTIL_VAL_GET_NET_16(&paddr_ip->Port);
        dest_addr =  NET_UTIL_VAL_GET_NET_32(&paddr_ip->Addr);
    }


#else                                                           /* See Notes #7 & #9a.                                  */
    NetSock_CloseSock(psock, DEF_YES, DEF_YES);
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   *perr =  NET_SOCK_ERR_INVALID_FAMILY;
    return (NET_SOCK_BSD_ERR_TX);
#endif


/*$PAGE*/
                                                                /* ------------------- CFG SOCK TX -------------------- */
                                                                /* Cfg sock tx flags.                                   */
#if (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_NO_BLOCK)
    block    = DEF_NO;

#else
    no_block = DEF_BIT_IS_SET(flags, NET_SOCK_FLAG_TX_NO_BLOCK);
    if (no_block == DEF_YES) {                                  /* If 'No Block' flag set, ...                          */
        block = DEF_NO;                                         /* ... do NOT block (see Note #3).                      */

    } else {                                                    
#if (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_BLOCK)        /* Else block if cfg'd, ...                             */
        block = DEF_YES;
#else                                                           /* ... or chk sock's block flag.                        */
        block = DEF_BIT_IS_SET(psock->Flags, NET_SOCK_FLAG_BLOCK);
#endif
    }
#endif


                                                                /* ------------------- TX SOCK DATA ------------------- */
    data_len_tot = 0;
    err_rtn      = NET_SOCK_ERR_FAULT;

    switch (psock->Protocol) {                                  /* Tx app data via transport layer tx.                  */
#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
        case NET_SOCK_PROTOCOL_UDP:
                                                                /* Cfg transport tx flags.                              */
             flags_transport = NET_UDP_FLAG_NONE;
             if (block == DEF_YES) {
                 DEF_BIT_SET(flags_transport, NET_UDP_FLAG_TX_BLOCK);
             }

             data_len_tot = NetUDP_TxAppDataHandler((void           *) p_data,
                                                    (CPU_INT16U      ) data_len,
                                                    (NET_IP_ADDR     ) src_addr,
                                                    (NET_UDP_PORT_NBR) src_port,
                                                    (NET_IP_ADDR     ) dest_addr,
                                                    (NET_UDP_PORT_NBR) dest_port,
                                                    (NET_IP_TOS      ) NET_IP_TOS_DFLT,     /* #### See Note #11b.      */
                                                    (NET_IP_TTL      ) NET_IP_TTL_DFLT,
                                                    (CPU_INT16U      ) flags_transport,
                                                    (CPU_INT16U      ) NET_IP_FLAG_NONE,
                                                    (void           *) 0,
                                                    (NET_ERR        *)&err);
             switch (err) {
                 case NET_UDP_ERR_NONE:
                      err_rtn = NET_SOCK_ERR_NONE; 
                      break;


                 case NET_UDP_ERR_INVALID_DATA_SIZE:
                      err_rtn = NET_SOCK_ERR_INVALID_DATA_SIZE;
                      break;


                 case NET_ERR_INIT_INCOMPLETE:
                 case NET_ERR_INVALID_PROTOCOL:
                 case NET_UDP_ERR_NULL_PTR:
                 case NET_UDP_ERR_INVALID_LEN_DATA:
                 case NET_UDP_ERR_INVALID_PORT_NBR:
                 case NET_UDP_ERR_INVALID_FLAG:
                 case NET_BUF_ERR_NULL_PTR:
                 case NET_BUF_ERR_NONE_AVAIL:
                 case NET_BUF_ERR_INVALID_TYPE:
                 case NET_BUF_ERR_INVALID_SIZE:
                 case NET_BUF_ERR_INVALID_LEN:
                 case NET_BUF_ERR_INVALID_IX:
                 case NET_UTIL_ERR_NULL_PTR:
                 case NET_UTIL_ERR_NULL_SIZE:
                 case NET_UTIL_ERR_INVALID_PROTOCOL:
                 case NET_ERR_TX:
                 default:
                      err_rtn = NET_ERR_TX;
                      break;
             }
             break;
#endif


        case NET_SOCK_PROTOCOL_NONE:
        default:                                                /* See Notes #8 & #9a.                                  */
            (void)&err;                                         /* Prevent possible compiler warning.                   */
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
            *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }


/*$PAGE*/
    switch (err_rtn) {                                          /* Demux transport-to-sock layer err.                   */
        case NET_SOCK_ERR_NONE:
             break;


        case NET_SOCK_ERR_INVALID_DATA_SIZE:
        case NET_SOCK_ERR_FAULT:
        case NET_ERR_TX:
        default:
            *perr =  err_rtn;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }



                                                                /* -------------- RTN TX'D SOCK DATA LEN -------------- */
    if (data_len_tot < 1) {                                     /* If tx'd data len < 1, ...                            */
       *perr =  NET_SOCK_ERR_INVALID_DATA_SIZE;                 /* ... rtn invalid tx data size err (see Note #6b1).    */
        return (NET_SOCK_BSD_ERR_TX);
    }


   *perr =   err_rtn;

    return ((NET_SOCK_RTN_CODE)data_len_tot);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetSock_TxDataHandlerStream()
*
* Description : (1) Transmit data through a stream-type socket :
*
*                   (a) Validate  socket connection state                                   See Note #8
*                   (b) Configure socket transmit :
*                       (1) Get       socket's transport connection identification handler
*                       (2) Configure socket flags
*                   (c) Transmit  socket data via appropriate transport layer
*                   (d) Return    socket data transmitted length
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to receive data.
*               -------         Argument checked   in NetSock_TxDataHandler().
*
*               psock           Pointer to socket.
*               -----           Argument validated in NetSock_TxDataHandler().
*
*               p_data          Pointer to application data.
*               ------          Argument checked   in NetSock_TxDataHandler().
*
*               data_len        Length of  application data (in octets) [see Note #3].
*               --------        Argument checked   in NetSock_TxDataHandler().
*
*               flags           Flags to select transmit options; bit-field flags logically OR'd :
*               -----
*                                   NET_SOCK_FLAG_NONE              No socket flags selected.
*                                   NET_SOCK_FLAG_TX_NO_BLOCK       Transmit socket data without blocking
*                                                                       (see Note #4).
*
*                               Argument checked   in NetSock_TxDataHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket data successfully transmitted.
*                               NET_SOCK_ERR_NOT_USED               Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED                 Socket already closed.
*                               NET_SOCK_ERR_FAULT                  Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_CONN_FAIL              Socket connection operation(s) failed.
*                               NET_SOCK_ERR_INVALID_PROTOCOL       Invalid socket protocol.
*                               NET_SOCK_ERR_INVALID_STATE          Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP             Invalid socket operation.
*                               NET_SOCK_ERR_INVALID_DATA_SIZE      Invalid data size (see Note #3b).
*                               NET_SOCK_ERR_TX_Q_CLOSED            Socket transmit queue closed.
*
*                               NET_ERR_TX                          Transitory transmit error.
*
*                                                                   - RETURNED BY NetConn_ID_TransportGet() : -
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*
* Return(s)   : Number of positive data octets transmitted, if NO errors                (see Note #5a1).
*
*               NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,          if socket connection closed (see Note #5b).
*
*               NET_SOCK_BSD_ERR_TX,                        otherwise                   (see Note #5a2A).
*
* Caller(s)   : NetSock_TxDataHandler().
*$PAGE*
* Note(s)     : (2) Stream-type sockets may transmit from the following states :
*
*                   (a) Unconnected states :
*
*                           (A) #### Unconnected state(s) transmit NOT yet implemented;
*                               see also 'net_tcp.c  NetTCP_TxConnAppData()  Note #3bA'.  
*
*                       (1) CLOSED
*                           (A) Bind to local port
*                           (B) Connect to remote host
*                           (C) (1) Transmit data with connection request(s)?
*                                     OR
*                               (2) Queue transmit data until connected to remote host?
*
*                       (2) BOUND
*                           (A) Connect to remote host
*                           (B) (1) Transmit data with connection request(s)?
*                                     OR
*                               (2) Queue transmit data until connected to remote host?
*
*                       (3) LISTEN
*                           (A) Connect to remote host
*                           (B) (1) Transmit data with connection request(s)?
*                                     OR
*                               (2) Queue transmit data until connected to remote host?
*
*                       (4) CONNECTION-IN-PROGRESS
*                           (A) Queue transmit data until connected to remote host?
*
*                   (b) Connected states :
*
*                       (1) CONNECTED
*
*               (3) (a) (1) (A) Stream-type sockets transmit & receive all data octets in one or more 
*                               non-distinct packets.  In other words, the application data is NOT 
*                               bounded by any specific packet(s); rather, it is contiguous & sequenced 
*                               from one packet to the next.
*
*                           (B) Thus, if the socket's type is stream & the socket's transmit data queue(s)
*                               are NOT large enough for the transmitted data, the  transmit data queue(s) 
*                               are maximally filled with transmit data & the remaining data octets are
*                               discarded but may be re-transmitted by later application-socket transmits.
*
*                           (C) Therefore, NO stream-type socket transmit data length should be "too long
*                               to pass through the underlying protocol" & cause the socket transmit to
*                               "fail ... [with] no data ... transmitted" (IEEE Std 1003.1, 2004 Edition,
*                               Section 'send() : DESCRIPTION').
*
*                       (2) Thus, it is typical -- but NOT absolutely required -- that a single application
*                           task ONLY transmit or request to transmit data to a stream-type socket.
*
*                   (b) 'data_len' of 0 octets NOT allowed.
*
*                   See also 'NetSock_TxDataHandler()  Note #2'.
*$PAGE*
*               (4) If global socket blocking ('NET_SOCK_CFG_BLOCK_SEL') configured for blocking operation 
*                   ('NET_SOCK_BLOCK_SEL_BLOCK' or 'NET_SOCK_BLOCK_SEL_DFLT') but 'flags' argument set to 
*                    'NET_SOCK_FLAG_TX_NO_BLOCK'; socket transmit does NOT configure the transport layer 
*                   transmit to block, regardless if the socket is configured to block.
*
*               (5) (a) IEEE Std 1003.1, 2004 Edition, Section 'send() : RETURN VALUE' states that :
*
*                       (1) "Upon successful completion, send() shall return the number of bytes sent."
*
*                           (A) Section 'send() : DESCRIPTION' elaborates that "successful completion 
*                               of a call to sendto() does not guarantee delivery of the message".
*
*                           (B) (1) Thus, applications SHOULD verify the actual returned number of data 
*                                   octets transmitted &/or prepared for transmission.
*
*                               (2) In addition, applications MAY desire verification of receipt &/or
*                                   acknowledgement of transmitted data to the remote host -- either 
*                                   inherently by the transport layer or explicitly by the application.
*
*                       (2) (A) "Otherwise, -1 shall be returned" ...
*                               (1) Section 'send() : DESCRIPTION' elaborates that "a return value of 
*                                   -1 indicates only locally-detected errors".
*
*                           (B) "and 'errno' set to indicate the error."
*                               #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   (b) ???? Although NO socket send() specification states to return '0' when the socket's
*                       connection is closed, it seems reasonable to return '0' since it is possible for the
*                       socket connection to be close()'d or shutdown() by the remote host.
*
*                       (1) Since the socket transmit return value of '0' is reserved for socket connection
*                           closes; NO socket transmit -- fault or non-fault -- should ever return '0' octets
*                           transmitted.
*
*                   See also 'NetSock_TxDataHandler()  Note #5'.
*
*               (6) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'Protocol' is incorrectly modified.
*
*               (7) On ANY errors, network resources MUST be appropriately freed :
*
*                   (a) NetSock_TxDataHandlerStream() assumes that internal socket configuration
*                       has been previously validated by caller function(s).  Therefore, on any
*                       internal socket configuration error(s), the socket MUST be closed.
*
*                   (b) NetSock_TxDataHandlerStream() assumes that any internal socket connection 
*                       error(s) on stream-type sockets may NOT be critical &/or may be transitory;
*                       thus NO network or socket resource(s) are closed/freed.
*
*                   (c) Since transport layer error(s) may NOT be critical &/or may be transitory, NO 
*                       network or socket resource(s) are closed/freed.
*
*               (8) Socket descriptor write availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor write handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also  Note #1a,
*                            'NetSock_SelDescHandlerWrStream()   Note #3',
*                          & 'NetSock_SelDescHandlerErrStream()  Note #3'.
*
*               (9) 'sock_id' may NOT be necessary but included for consistency.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_SOCK_RTN_CODE  NetSock_TxDataHandlerStream (NET_SOCK_ID   sock_id,
                                                        NET_SOCK     *psock,
                                                        void         *p_data,
                                                        CPU_INT16U    data_len,
                                                        CPU_INT16S    flags,
                                                        NET_ERR      *perr)
{
#if (NET_SOCK_CFG_BLOCK_SEL != NET_SOCK_BLOCK_SEL_NO_BLOCK)
    CPU_BOOLEAN  no_block;
#endif
    CPU_BOOLEAN  block;
    NET_CONN_ID  conn_id;
    NET_CONN_ID  conn_id_transport;
    CPU_INT16U   flags_transport;
    CPU_INT16U   data_len_tot;
    NET_ERR      err;
    NET_ERR      err_rtn;


   (void)&sock_id;                                              /* Prevent compiler warning (see Note #9).              */

                                                                /* ------------- VALIDATE SOCK CONN STATE ------------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
            *perr =  NET_SOCK_ERR_NOT_USED;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED:                             /* #### Bind to local port & ...                        */
        case NET_SOCK_STATE_BOUND:                              /* ...  tx conn req to remote host; Q tx data?          */
        case NET_SOCK_STATE_LISTEN:                             /* #### Q/tx data with  conn req?                       */
        case NET_SOCK_STATE_CONN_IN_PROGRESS:                   /* #### Q tx data until conn complete?                  */
                                                                /* #### NOT yet implemented (see Note #2aA).            */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidOpCtr);
            *perr =  NET_SOCK_ERR_INVALID_OP;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CONN_DONE:
             psock->State = NET_SOCK_STATE_CONN;
             break;


        case NET_SOCK_STATE_CONN:
             break;
             

        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:                  /* If sock already closing, ...                         */
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
            *perr =  NET_SOCK_ERR_TX_Q_CLOSED;                  /* ... rtn closed code (see Note #5b).                  */
             return (NET_SOCK_BSD_RTN_CODE_CONN_CLOSED);        /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_NONE:
        default:
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidStateCtr);
            *perr =  NET_SOCK_ERR_INVALID_STATE;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }


/*$PAGE*/
                                                                /* ------------------- CFG SOCK TX -------------------- */
                                                                /* Get transport conn id.                               */
    conn_id           = psock->ID_Conn;
    conn_id_transport = NetConn_ID_TransportGet(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {                           /* See Note #7a.                                        */
         NetSock_CloseSock(psock, DEF_YES, DEF_YES);
         return (NET_SOCK_BSD_ERR_TX);
    }
    if  (conn_id_transport == NET_CONN_ID_NONE) {               /* See Note #7a.                                        */
         NetSock_CloseSock(psock, DEF_YES, DEF_YES);
        *perr =  NET_SOCK_ERR_CONN_FAIL;
         return (NET_SOCK_BSD_ERR_TX);
    }


                                                                /* Cfg sock tx flags.                                   */
#if (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_NO_BLOCK)
    block    = DEF_NO;

#else
    no_block = DEF_BIT_IS_SET(flags, NET_SOCK_FLAG_TX_NO_BLOCK);
    if (no_block == DEF_YES) {                                  /* If 'No Block' flag set, ...                          */
        block = DEF_NO;                                         /* ... do NOT block (see Note #4).                      */

    } else {                                                    
#if (NET_SOCK_CFG_BLOCK_SEL == NET_SOCK_BLOCK_SEL_BLOCK)        /* Else block if cfg'd, ...                             */
        block = DEF_YES;
#else                                                           /* ... or chk sock's block flag.                        */
        block = DEF_BIT_IS_SET(psock->Flags, NET_SOCK_FLAG_BLOCK);
#endif
    }
#endif



/*$PAGE*/
                                                                /* ------------------- TX SOCK DATA ------------------- */
    data_len_tot = 0;
    err_rtn      = NET_SOCK_ERR_FAULT;

    switch (psock->Protocol) {                                  /* Tx app data via transport layer tx.                  */
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
        case NET_SOCK_PROTOCOL_TCP:
                                                                /* Cfg transport tx flags.                              */
             flags_transport = NET_TCP_FLAG_NONE;
             if (block == DEF_YES) {
                 DEF_BIT_SET(flags_transport, NET_TCP_FLAG_TX_BLOCK);
             }

             data_len_tot = NetTCP_TxConnAppData((NET_TCP_CONN_ID) conn_id_transport,
                                                 (void          *) p_data,
                                                 (CPU_INT16U     ) data_len,
                                                 (CPU_INT16U     ) flags_transport,
                                                 (NET_ERR       *)&err);
             switch (err) {
                 case NET_TCP_ERR_NONE:
                      err_rtn = NET_SOCK_ERR_NONE;
                      break;


                 case NET_TCP_ERR_INVALID_DATA_SIZE:
                      err_rtn = NET_SOCK_ERR_INVALID_DATA_SIZE;
                      break;


                 case NET_TCP_ERR_CONN_NOT_USED:
                 case NET_TCP_ERR_CONN_CLOSE:
                 case NET_TCP_ERR_CONN_FAULT:
                 case NET_TCP_ERR_CONN_DATA_INVALID:
                 case NET_TCP_ERR_INVALID_CONN:
                 case NET_TCP_ERR_INVALID_CONN_STATE:
                 case NET_TCP_ERR_INVALID_CONN_OP:
                 case NET_TCP_ERR_TX_Q_SIGNAL_ABORT:
                 case NET_TCP_ERR_TX_Q_SIGNAL_FAULT:
                 case NET_TCP_ERR_TX:
                 case NET_CONN_ERR_INVALID_FAMILY:
                 case NET_CONN_ERR_INVALID_ADDR:
                 case NET_CONN_ERR_INVALID_ADDR_LEN:
                      err_rtn = NET_SOCK_ERR_FAULT;
                      break;


                 case NET_ERR_INIT_INCOMPLETE:
                 case NET_TCP_ERR_NULL_PTR:
                 case NET_TCP_ERR_INVALID_LEN_SEG:
                 case NET_TCP_ERR_CONN_ACK_NONE:
                 case NET_TCP_ERR_CONN_ACK_DLYD:
                 case NET_TCP_ERR_CONN_ACK_PREVLY_TXD:
                 case NET_TCP_ERR_CONN_ACK_INVALID:
                 case NET_TCP_ERR_NONE_AVAIL:
                 case NET_TCP_ERR_TX_Q_FULL:
                 case NET_TCP_ERR_TX_Q_SUSPEND:
                 case NET_OS_ERR_LOCK:
                 case NET_ERR_TX:
                 default:
                      err_rtn = NET_ERR_TX;
                      break;
             }
             break;
#endif
#endif

        case NET_SOCK_PROTOCOL_NONE:
        default:                                                /* See Notes #6 & #7a.                                  */
            (void)&conn_id_transport;                           /* Prevent possible compiler warnings.                  */
            (void)&flags_transport;
            (void)&err;
             NetSock_CloseSock(psock, DEF_YES, DEF_YES);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
            *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }


    switch (err_rtn) {                                          /* Demux transport-to-sock layer err.                   */
        case NET_SOCK_ERR_NONE:
             break;


        case NET_SOCK_ERR_INVALID_DATA_SIZE:
        case NET_SOCK_ERR_FAULT:
        case NET_ERR_TX:
        default:
            *perr =  err_rtn;
             return (NET_SOCK_BSD_ERR_TX);                      /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                /* -------------- RTN TX'D SOCK DATA LEN -------------- */
    if (data_len_tot < 1) {                                     /* If tx'd data len < 1, ...                            */
       *perr =  NET_SOCK_ERR_INVALID_DATA_SIZE;                 /* ... rtn invalid tx data size err (see Note #5b1).    */
        return (NET_SOCK_BSD_ERR_TX);
    }


   *perr =   err_rtn;

    return ((NET_SOCK_RTN_CODE)data_len_tot);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_SelDescHandlerRd()
*
* Description : (1) Handle socket descriptor for read operation(s) :
*
*                   (a) Check socket for available read operation(s) :
*                       (1) Read data                                                   See Note #2b1
*                       (2) Read connection closed                                      See Note #2b2
*                       (3) Read connection(s) available                                See Note #2b3
*                       (4) Socket error(s)                                             See Note #2b4
*
*                   (b) Configure socket event table to wait on appropriate read 
*                           operation(s), if necessary
*
*
* Argument(s) : sock_id                 Socket descriptor/handle identifier of socket to check for 
*                                           available read operation(s).
*
*               psock_event_tbl         Pointer to a socket event table to configure socket events
*               ---------------             to wait on.
*
*                                       Argument validated in NetSock_Sel().
*
*               psock_event_nbr_cfgd    Pointer to the number of configured socket events.
*               --------------------    Argument validated in NetSock_Sel().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetSock_SelDescHandlerRdDatagram() : -
*                                                               -- RETURNED BY NetSock_SelDescHandlerRdStream() : --
*                               NET_SOCK_ERR_NONE               Socket successfully checked for read operation(s);
*                                                                   check return value for read operation(s)
*                                                                   availability.
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP         Invalid socket operation.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*                               NET_SOCK_ERR_FAULT              Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
*                               NET_SOCK_ERR_EVENTS_NBR_MAX     Number of configured socket events is greater
*                                                                   than the maximum number of socket events.
*
*                                                               ---------- RETURNED BY NetSock_IsUsed() : ----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
* Return(s)   : DEF_YES, if socket has any available read operation(s) [see Notes #2b1 & #2b3]; OR ...
*
*                        if socket's read connection is closed         [see Note  #2b2];        OR ...
*
*                        if socket has any available socket error(s)   [see Note  #2b4].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_Sel().
*$PAGE*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the 'readfds' ... parameter 
*                   ... to see whether ... [any] descriptors are ready for reading" :
*
*                   (a) "A descriptor shall be considered ready for reading when a call to an input function 
*                        ... would not block, whether or not the function would transfer data successfully.  
*                        (The function might return data, an end-of-file indication, or an error other than 
*                        one indicating that it is blocked, and in each of these cases the descriptor shall 
*                        be considered ready for reading.)" :
*
*                       (1) "If the socket is currently listening, then it shall be marked as readable if 
*                            an incoming connection request has been received, and a call to the accept() 
*                            function shall complete without blocking."
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Pages 164-165 states that "a socket is ready for reading if any of the 
*                       following ... conditions is true" :
*
*                       (1) "A read operation on the socket will not block and will return a value greater 
*                            than 0 (i.e., the data that is ready to be read)."
*                            
*                       (2) "The read half of the connection is closed (i.e., a TCP connection that has 
*                            received a FIN).  A read operation ... will not block and will return 0 (i.e., 
*                            EOF)."
*
*                       (3) "The socket is a listening socket and the number of completed connections is 
*                            nonzero.  An accept() on the listening socket will ... not block."
*
*                       (4) "A socket error is pending.  A read operation on the socket will not block and 
*                            will return an error (-1) with 'errno' set to the specific error condition."
*
*                   See also 'NetSock_Sel()  Note #3b2A1'.
*
*               (3) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'SockType' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_SelDescHandlerRd (NET_SOCK_ID           sock_id,
                                               NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                               NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                               NET_ERR              *perr)
{
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    CPU_BOOLEAN   sock_used;
#endif
    CPU_BOOLEAN   sock_rdy;
    NET_SOCK     *psock;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                     /* -------------- VALIDATE SOCK USED -------------- */
    sock_used = NetSock_IsUsed(sock_id, perr);
    if (sock_used != DEF_YES) {
        return (DEF_YES);                                           /* Rtn sock err (see Note #2b4).                    */
    }
#endif

                                                                    /* ------------- HANDLE SOCK DESC RD -------------- */
    psock = &NetSock_Tbl[sock_id];

    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             sock_rdy = NetSock_SelDescHandlerRdDatagram((NET_SOCK_ID          )sock_id,
                                                         (NET_SOCK            *)psock,
                                                         (NET_SOCK_EVENT_TBL **)psock_event_tbl,
                                                         (NET_SOCK_EVENT_QTY  *)psock_event_nbr_cfgd,
                                                         (NET_ERR             *)perr);
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             sock_rdy = NetSock_SelDescHandlerRdStream((NET_SOCK_ID          )sock_id,
                                                       (NET_SOCK            *)psock,
                                                       (NET_SOCK_EVENT_TBL **)psock_event_tbl,
                                                       (NET_SOCK_EVENT_QTY  *)psock_event_nbr_cfgd,
                                                       (NET_ERR             *)perr);
             break;
#endif


        case NET_SOCK_TYPE_FAULT:
            *perr =  NET_SOCK_ERR_FAULT;                            /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_TYPE_NONE:
        default:                                                    /* See Note #3.                                     */
            *perr =  NET_SOCK_ERR_INVALID_TYPE;                     /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


    return (sock_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetSock_SelDescHandlerRdDatagram()
*
* Description : (1) Handle datagram-type socket descriptor for read operation(s) :
*
*                   (a) Check datagram-type socket for available read operation(s) :
*                       (1) Read data                                                   See Note #2b1
*                       (2) Socket error(s)                                             See Note #2b4
*
*                   (b) Configure socket event table to wait on appropriate read 
*                           operation(s) :
*
*                       (1) Socket receive data
*
*
* Argument(s) : sock_id                 Socket descriptor/handle identifier of datagram-type socket to 
*               -------                     check for available read operation(s).
*
*                                       Argument checked   in NetSock_SelDescHandlerRd().
*
*               psock                   Pointer to socket.
*               -----                   Argument validated in NetSock_SelDescHandlerRd().
*
*               psock_event_tbl         Pointer to a socket event table to configure socket events
*               ---------------             to wait on.
*
*                                       Argument validated in NetSock_Sel().
*
*               psock_event_nbr_cfgd    Pointer to the number of configured socket events.
*               --------------------    Argument validated in NetSock_Sel().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Datagram-type socket successfully checked for
*                                                                   read operation(s); check return value for
*                                                                   read operation(s) availability.
*
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP         Invalid socket operation.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
*                               NET_SOCK_ERR_EVENTS_NBR_MAX     Number of configured socket events is greater
*                                                                   than the maximum number of socket events.
*
*                                                               - RETURNED BY NetSock_IsAvailRxDatagram() : -
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type; i.e. NOT datagram-type.
*
* Return(s)   : DEF_YES, if datagram-type socket has any available read operation(s) [see Note #2b1]; OR ...
*
*                        if datagram-type socket has any available socket error(s)   [see Note #2b4].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerRd().
*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the 'readfds' ... parameter
*                   ... to see whether ... [any] descriptors are ready for reading" :
*
*                   (a) "A descriptor shall be considered ready for reading when a call to an input function 
*                        ... would not block, whether or not the function would transfer data successfully.  
*                        (The function might return data, an end-of-file indication, or an error other than 
*                        one indicating that it is blocked, and in each of these cases the descriptor shall 
*                        be considered ready for reading.)"
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Pages 164-165 states that "a socket is ready for reading if any of the 
*                       following ... conditions is true" :
*
*                       (1) "A read operation on the socket will not block and will return a value greater 
*                            than 0 (i.e., the data that is ready to be read)."
*
*                       (4) "A socket error is pending.  A read operation on the socket will not block and
*                            will return an error (-1) with 'errno' set to the specific error condition."
*
*                   See also 'NetSock_Sel()  Note #3b2A1'.
*
*               (3) Socket descriptor read availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor read handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also 'NetSock_RxDataHandlerDatagram()  Note #13'.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_SelDescHandlerRdDatagram (NET_SOCK_ID           sock_id,
                                                       NET_SOCK             *psock,
                                                       NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                       NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                       NET_ERR              *perr)
{
    CPU_BOOLEAN  sock_rdy;
    NET_ERR      err;


                                                                    /* --------- HANDLE DATAGRAM SOCK DESC RD --------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
            *perr =  NET_SOCK_ERR_NOT_USED;                         /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;                           /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED:
            *perr =  NET_SOCK_ERR_INVALID_OP;                       /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_BOUND:                                  /* Chk non-conn'd datagram socks ...                */
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN:                                   /* ... &   conn'd datagram socks ...                */
        case NET_SOCK_STATE_CONN_DONE:
                                                                    /* ... for rx data avail (see Note #2b1).           */
             sock_rdy = NetSock_IsAvailRxDatagram(psock, &err);
             switch (err) {
                 case NET_SOCK_ERR_NONE:
                      if (sock_rdy != DEF_YES) {                    /* If sock NOT rdy, ...                             */
                                                                    /* ... cfg sock event to wait on sock rx.           */
                        (*psock_event_nbr_cfgd)++;
                          if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                              *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                               return (DEF_NO);
                          }
                        (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_SOCK_RX;
                        (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
                        (*psock_event_tbl)->ConnID    = (NET_CONN_ID)sock_id;
                        (*psock_event_tbl)++;
                      }
                      break;


                 case NET_SOCK_ERR_INVALID_TYPE:
                     *perr =  err;                                  /* Rtn sock err (see Note #2b4).                    */
                      return (DEF_YES);                             /* Prevent 'break NOT reachable' compiler warning.  */

                      
                 default:
                     *perr =  NET_SOCK_ERR_CONN_FAIL;               /* Rtn sock err (see Note #2b4).                    */
                      return (DEF_YES);                             /* Prevent 'break NOT reachable' compiler warning.  */
             }
             break;


        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_LISTEN:                                 /* Listen  datagram socks NOT allowed.              */
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:                      /* Closing datagram socks NOT allowed.              */
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        default:
            *perr =  NET_SOCK_ERR_INVALID_STATE;                    /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (sock_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetSock_SelDescHandlerRdStream()
*
* Description : (1) Handle stream-type socket descriptor for read operation(s) :
*
*                   (a) Check stream-type socket for available read operation(s) :
*                       (1) Read data                                                   See Note #2b1
*                       (2) Read connection closed                                      See Note #2b2
*                       (3) Read connection(s) available                                See Note #2b3
*                       (4) Socket error(s)                                             See Note #2b4
*
*                   (b) Configure socket event table to wait on appropriate read 
*                           operation(s) :
*
*                       (1) Socket connection's transport layer receive operation(s)
*                       (2) Socket accept connection(s) available
*
*
* Argument(s) : sock_id                 Socket descriptor/handle identifier of stream-type socket to 
*               -------                     check for available read operation(s).
*
*                                       Argument checked   in NetSock_SelDescHandlerRd().
*
*               psock                   Pointer to socket.
*               -----                   Argument validated in NetSock_SelDescHandlerRd().
*
*               psock_event_tbl         Pointer to a socket event table to configure socket events
*               ---------------             to wait on.
*
*                                       Argument validated in NetSock_Sel().
*
*               psock_event_nbr_cfgd    Pointer to the number of configured socket events.
*               --------------------    Argument validated in NetSock_Sel().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Stream-type socket successfully checked for 
*                                                                   read operation(s); check return value for 
*                                                                   read operation(s) availability.
*
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP         Invalid socket operation.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
*                               NET_SOCK_ERR_EVENTS_NBR_MAX     Number of configured socket events is greater
*                                                                   than the maximum number of socket events.
*
*                                                               -- RETURNED BY NetSock_IsAvailRxStream() : --
*                                                               - RETURNED BY NetSock_ConnAcceptQ_IsRdy() : -
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type; i.e. NOT stream-type.
*
* Return(s)   : DEF_YES, if stream-type socket has any available read operation(s) [see Notes #2b1 & #2b3]; OR ...
*
*                        if stream-type socket's read connection is closed         [see Note  #2b2];        OR ...
*
*                        if stream-type socket has any available socket error(s)   [see Note  #2b4].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerRd().
*$PAGE*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the 'readfds' ... parameter
*                   ... to see whether ... [any] descriptors are ready for reading" :
*
*                   (a) "A descriptor shall be considered ready for reading when a call to an input function 
*                        ... would not block, whether or not the function would transfer data successfully.  
*                        (The function might return data, an end-of-file indication, or an error other than 
*                        one indicating that it is blocked, and in each of these cases the descriptor shall 
*                        be considered ready for reading.)" :
*
*                       (1) "If the socket is currently listening, then it shall be marked as readable if 
*                            an incoming connection request has been received, and a call to the accept() 
*                            function shall complete without blocking."
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Pages 164-165 states that "a socket is ready for reading if any of the 
*                       following ... conditions is true" :
*
*                       (1) "A read operation on the socket will not block and will return a value greater 
*                            than 0 (i.e., the data that is ready to be read)."
*                            
*                       (2) "The read half of the connection is closed (i.e., a TCP connection that has 
*                            received a FIN).  A read operation ... will not block and will return 0 (i.e.,
*                            EOF)."
*
*                       (3) "The socket is a listening socket and the number of completed connections is
*                            nonzero.  An accept() on the listening socket will ... not block."
*
*                       (4) "A socket error is pending.  A read operation on the socket will not block and
*                            will return an error (-1) with 'errno' set to the specific error condition."
*
*                   See also 'NetSock_Sel()  Note #3b2A1'.
*
*               (3) Socket descriptor read availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor read handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also 'NetSock_RxDataHandlerStream()  Note #11'
*                          & 'NetSock_Accept()               Note #11'.
*********************************************************************************************************
*/

#if ((NET_SOCK_CFG_SEL_EN         == DEF_ENABLED) && \
     (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED))
static  CPU_BOOLEAN  NetSock_SelDescHandlerRdStream (NET_SOCK_ID           sock_id,
                                                     NET_SOCK             *psock,
                                                     NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                     NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                     NET_ERR              *perr)
{
    NET_CONN_ID  conn_id;
    NET_CONN_ID  conn_id_transport;
    CPU_BOOLEAN  sock_rdy;
    NET_ERR      err;

                                                                    /* ---------- HANDLE STREAM SOCK DESC RD ---------- */
                                                                    /* Get transport conn id.                           */
    conn_id           = psock->ID_Conn;
    conn_id_transport = NetConn_ID_TransportGet(conn_id, &err);
    if (  err != NET_CONN_ERR_NONE) {
        *perr  = NET_SOCK_ERR_CONN_FAIL;
         return (DEF_YES);                                          /* Rtn sock err (see Note #2b4).                    */
    }
    if  (conn_id_transport == NET_CONN_ID_NONE) {
        *perr  = NET_SOCK_ERR_CONN_FAIL;
         return (DEF_YES);                                          /* Rtn sock err (see Note #2b4).                    */
    }


    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
            *perr =  NET_SOCK_ERR_NOT_USED;                         /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;                           /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED:                                 /* Closed     stream socks NOT allowed.             */
        case NET_SOCK_STATE_BOUND:                                  /* Non-conn'd stream socks NOT allowed.             */
            *perr =  NET_SOCK_ERR_INVALID_OP;                       /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */

/*$PAGE*/
        case NET_SOCK_STATE_LISTEN:                                 /* Chk listen stream socks ...                      */
             sock_rdy = NetSock_ConnAcceptQ_IsRdy(psock, &err);     /* ... for any avail conn (see Note #2b3).          */
             switch (err) {
                 case NET_SOCK_ERR_NONE:
                      if (sock_rdy != DEF_YES) {                    /* If sock NOT rdy, ...                             */
                                                                    /* ... cfg sock event to wait on sock accept.       */
                        (*psock_event_nbr_cfgd)++;
                          if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                              *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                               return (DEF_NO);
                          }
                        (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_SOCK_ACCEPT;
                        (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
                        (*psock_event_tbl)->ConnID    = (NET_CONN_ID)sock_id;
                        (*psock_event_tbl)++;
                      }
                      break;


                 case NET_SOCK_ERR_INVALID_TYPE:
                     *perr =  err;                                  /* Rtn sock err (see Note #2b4).                    */
                      return (DEF_YES);                             /* Prevent 'break NOT reachable' compiler warning.  */

                      
                 default:
                     *perr =  NET_SOCK_ERR_CONN_FAIL;               /* Rtn sock err (see Note #2b4).                    */
                      return (DEF_YES);                             /* Prevent 'break NOT reachable' compiler warning.  */
             }
             break;


        case NET_SOCK_STATE_CONN_IN_PROGRESS:                       /* Rtn conn-in-progress stream socks ...            */
             sock_rdy = DEF_NO;                                     /* ... as NOT rdy.                                  */
                                                                    /* Cfg sock event to wait on transport rx.          */
           (*psock_event_nbr_cfgd)++;
             if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                 *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                  return (DEF_NO);
             }
           (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_TRANSPORT_RX;
           (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
           (*psock_event_tbl)->ConnID    = (NET_CONN_ID)conn_id_transport;
           (*psock_event_tbl)++;
             break;


        case NET_SOCK_STATE_CONN:                                   /* Chk   conn'd  stream socks             ...       */
        case NET_SOCK_STATE_CONN_DONE:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:                      /* ... & closing stream socks;            ...       */
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
                                                                    /* ... for rx data avail  (see Note #2b1) ...       */       
                                                                    /* ... OR  rx conn closed (see Note #2b2).          */
             sock_rdy = NetSock_IsAvailRxStream(psock, conn_id_transport, &err);
             switch (err) {
                 case NET_SOCK_ERR_NONE:
                      if (sock_rdy != DEF_YES) {                    /* If sock NOT rdy, ...                             */
                                                                    /* ... cfg sock event to wait on transport rx.      */
                        (*psock_event_nbr_cfgd)++;
                          if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                              *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                               return (DEF_NO);
                          }
                        (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_TRANSPORT_RX;
                        (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
                        (*psock_event_tbl)->ConnID    = (NET_CONN_ID)conn_id_transport;
                        (*psock_event_tbl)++;
                      }
                      break;


                 case NET_SOCK_ERR_INVALID_TYPE:
                     *perr =  err;                                  /* Rtn sock err (see Note #2b4).                    */
                      return (DEF_YES);                             /* Prevent 'break NOT reachable' compiler warning.  */

                      
                 case NET_ERR_INIT_INCOMPLETE:
                 case NET_SOCK_ERR_INVALID_PROTOCOL:
                 case NET_SOCK_ERR_CONN_FAIL:
                 case NET_CONN_ERR_INVALID_CONN:
                 case NET_CONN_ERR_NOT_USED:
                 default:
                     *perr =  NET_SOCK_ERR_CONN_FAIL;               /* Rtn sock err (see Note #2b4).                    */
                      return (DEF_YES);                             /* Prevent 'break NOT reachable' compiler warning.  */
             }
             break;


        case NET_SOCK_STATE_NONE:
        default:
            *perr =  NET_SOCK_ERR_INVALID_STATE;                    /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (sock_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_SelDescHandlerWr()
*
* Description : (1) Handle socket descriptor for write operation(s) :
*
*                   (a) Check socket for available write operation(s) :
*                       (1) Write data                                                  See Note #2b1
*                       (2) Write connection closed                                     See Note #2b2
*                       (3) Write connection(s) available                               See Note #2b3
*                       (4) Socket error(s)                                             See Note #2b4
*
*                   (b) Configure socket event table to wait on appropriate write 
*                           operation(s), if necessary
*
*
* Argument(s) : sock_id                 Socket descriptor/handle identifier of socket to check for 
*                                           available write operation(s).
*
*               psock_event_tbl         Pointer to a socket event table to configure socket events
*               ---------------             to wait on.
*
*                                       Argument validated in NetSock_Sel().
*
*               psock_event_nbr_cfgd    Pointer to the number of configured socket events.
*               --------------------    Argument validated in NetSock_Sel().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetSock_SelDescHandlerWrDatagram() : -
*                                                               -- RETURNED BY NetSock_SelDescHandlerWrStream() : --
*                               NET_SOCK_ERR_NONE               Socket successfully checked for write operation(s);
*                                                                   check return value for write operation(s)
*                                                                   availability.
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*                               NET_SOCK_ERR_FAULT              Socket fault; connection(s) aborted.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
*                               NET_SOCK_ERR_EVENTS_NBR_MAX     Number of configured socket events is greater
*                                                                   than the maximum number of socket events.
*
*                                                               -- RETURNED BY NetSock_SelDescHandlerWrStream() : --
*                               NET_SOCK_ERR_INVALID_OP         Invalid socket operation.
*
*                                                               ---------- RETURNED BY NetSock_IsUsed() : ----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
* Return(s)   : DEF_YES, if socket has any available write operation(s) [see Notes #2b1 & #2b3]; OR ...
*
*                        if socket's write connection is closed         [see Note  #2b2];        OR ...
*
*                        if socket has any available socket error(s)    [see Note  #2b4].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_Sel().
*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the ... 'writefds' ... 
*                   parameter ... to see whether ... [any] descriptors ... are ready for writing" :
*
*                   (a) "A descriptor shall be considered ready for writing when a call to an output function 
*                        ... would not block, whether or not the function would transfer data successfully" :
*
*                       (1) "If a non-blocking call to the connect() function has been made for a socket, and 
*                            the connection attempt has either succeeded or failed leaving a pending error, 
*                            the socket shall be marked as writable."
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Page 165 states that "a socket is ready for writing if any of the 
*                       following ... conditions is true" :
*
*                       (1) "A write operation will not block and will return a positive value (e.g., the 
*                            number of bytes accepted by the transport layer)."
*
*                       (2) "The write half of the connection is closed."
*
*                       (3) "A socket using a non-blocking connect() has completed the connection, or the 
*                            connect() has failed."
*
*                       (4) "A socket error is pending.  A write operation on the socket will not block and 
*                            will return an error (-1) with 'errno' set to the specific error condition."
*
*                   See also 'NetSock_Sel()  Note #3b2A2'.
*
*               (3) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'SockType' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_SelDescHandlerWr (NET_SOCK_ID           sock_id,
                                               NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                               NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                               NET_ERR              *perr)
{
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    CPU_BOOLEAN   sock_used;
#endif
    CPU_BOOLEAN   sock_rdy;
    NET_SOCK     *psock;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                     /* -------------- VALIDATE SOCK USED -------------- */
    sock_used = NetSock_IsUsed(sock_id, perr);
    if (sock_used != DEF_YES) {
        return (DEF_YES);                                           /* Rtn sock err (see Note #2b4).                    */
    }
#endif

                                                                    /* ------------- HANDLE SOCK DESC WR -------------- */
    psock = &NetSock_Tbl[sock_id];

    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             sock_rdy = NetSock_SelDescHandlerWrDatagram((NET_SOCK_ID          )sock_id,
                                                         (NET_SOCK            *)psock,
                                                         (NET_SOCK_EVENT_TBL **)psock_event_tbl,
                                                         (NET_SOCK_EVENT_QTY  *)psock_event_nbr_cfgd,
                                                         (NET_ERR             *)perr);
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             sock_rdy = NetSock_SelDescHandlerWrStream((NET_SOCK_ID          )sock_id,
                                                       (NET_SOCK            *)psock,
                                                       (NET_SOCK_EVENT_TBL **)psock_event_tbl,
                                                       (NET_SOCK_EVENT_QTY  *)psock_event_nbr_cfgd,
                                                       (NET_ERR             *)perr);
             break;
#endif


        case NET_SOCK_TYPE_FAULT:
            *perr =  NET_SOCK_ERR_FAULT;                            /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_TYPE_NONE:
        default:                                                    /* See Note #3.                                     */
            *perr =  NET_SOCK_ERR_INVALID_TYPE;                     /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


    return (sock_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetSock_SelDescHandlerWrDatagram()
*
* Description : (1) Handle datagram-type socket descriptor for write operation(s) :
*
*                   (a) Check datagram-type socket for available write operation(s) :
*                       (1) Write data                                                  See Note #2b1
*                       (2) Socket error(s)                                             See Note #2b4
*
*                   (b) Configure socket event table to wait on appropriate write 
*                           operation(s) :
*
*                       (1) Socket transmit data
*
*
* Argument(s) : sock_id                 Socket descriptor/handle identifier of datagram-type socket to 
*               -------                     check for available write operation(s).
*
*                                       Argument checked   in NetSock_SelDescHandlerWr().
*
*               psock                   Pointer to socket.
*               -----                   Argument validated in NetSock_SelDescHandlerWr().
*
*               psock_event_tbl         Pointer to a socket event table to configure socket events
*               ---------------             to wait on.
*
*                                       Argument validated in NetSock_Sel().
*
*               psock_event_nbr_cfgd    Pointer to the number of configured socket events.
*               --------------------    Argument validated in NetSock_Sel().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Datagram-type socket successfully checked for
*                                                                   write operation(s); check return value for
*                                                                   write operation(s) availability.
*
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
*                               NET_SOCK_ERR_EVENTS_NBR_MAX     Number of configured socket events is greater
*                                                                   than the maximum number of socket events.
*
*                                                               -- RETURNED BY NetSock_IsRdyTxDatagram() : ---
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type; i.e. NOT datagram-type.
*
* Return(s)   : DEF_YES, if datagram-type socket has any available write operation(s) [see Note #2b1]; OR ...
*
*                        if datagram-type socket has any available socket error(s)    [see Note #2b4].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerWr().
*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the ... 'writefds' ... 
*                   parameter ... to see whether ... [any] descriptors ... are ready for writing" :
*
*                   (a) "A descriptor shall be considered ready for writing when a call to an output function 
*                        ... would not block, whether or not the function would transfer data successfully."
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Page 165 states that "a socket is ready for writing if any of the 
*                       following ... conditions is true" :
*
*                       (1) "A write operation will not block and will return a positive value (e.g., the 
*                            number of bytes accepted by the transport layer)."
*
*                       (4) "A socket error is pending.  A write operation on the socket will not block and 
*                            will return an error (-1) with 'errno' set to the specific error condition."
*
*                   See also 'NetSock_Sel()  Note #3b2A2'.
*
*               (3) Socket descriptor write availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor write handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also 'NetSock_TxDataHandlerDatagram()  Note #10'.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_SelDescHandlerWrDatagram (NET_SOCK_ID           sock_id,
                                                       NET_SOCK             *psock,
                                                       NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                       NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                       NET_ERR              *perr)
{
    CPU_BOOLEAN  sock_rdy;
    NET_ERR      err;


                                                                    /* --------- HANDLE DATAGRAM SOCK DESC WR --------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
            *perr =  NET_SOCK_ERR_NOT_USED;                         /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;                           /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED:                                 /* Chk non-conn'd datagram socks ...                */
        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN:                                   /* ... &   conn'd datagram socks ...                */
        case NET_SOCK_STATE_CONN_DONE:
                                                                    /* ... if rdy for tx (see Note #2b1).               */
             sock_rdy = NetSock_IsRdyTxDatagram(psock, &err);
             switch (err) {
                 case NET_SOCK_ERR_NONE:
                      if (sock_rdy != DEF_YES) {                    /* If sock NOT rdy, ...                             */
                                                                    /* ... cfg sock event to wait on sock tx.           */
                        (*psock_event_nbr_cfgd)++;
                          if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                              *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                               return (DEF_NO);
                          }
                        (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_SOCK_TX;
                        (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
                        (*psock_event_tbl)->ConnID    = (NET_CONN_ID)sock_id;
                        (*psock_event_tbl)++;
                      }
                      break;


                 case NET_SOCK_ERR_INVALID_TYPE:
                     *perr =  err;                                  /* Rtn sock err (see Note #2b4).                    */
                      return (DEF_YES);                             /* Prevent 'break NOT reachable' compiler warning.  */

                      
                 default:
                     *perr =  NET_SOCK_ERR_CONN_FAIL;               /* Rtn sock err (see Note #2b4).                    */
                      return (DEF_YES);                             /* Prevent 'break NOT reachable' compiler warning.  */
             }
             break;


        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_LISTEN:                                 /* Listen  datagram socks NOT allowed.              */
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:                      /* Closing datagram socks NOT allowed.              */
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        default:
            *perr =  NET_SOCK_ERR_INVALID_STATE;                    /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (sock_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetSock_SelDescHandlerWrStream()
*
* Description : (1) Handle stream-type socket descriptor for write operation(s) :
*
*                   (a) Check stream-type socket for available write operation(s) :
*                       (1) Write data                                                  See Note #2b1
*                       (2) Write connection closed                                     See Note #2b2
*                       (3) Write connection(s) available                               See Note #2b3
*                       (4) Socket error(s)                                             See Note #2b4
*
*                   (b) Configure socket event table to wait on appropriate write 
*                           operation(s) :
*
*                       (1) Socket connection's transport layer transmit operation(s)
*                       (2) Socket connection(s) complete
*
*
* Argument(s) : sock_id                 Socket descriptor/handle identifier of stream-type socket to 
*               -------                     check for available write operation(s).
*
*                                       Argument checked   in NetSock_SelDescHandlerWr().
*
*               psock                   Pointer to socket.
*               -----                   Argument validated in NetSock_SelDescHandlerWr().
*
*               psock_event_tbl         Pointer to a socket event table to configure socket events
*               ---------------             to wait on.
*
*                                       Argument validated in NetSock_Sel().
*
*               psock_event_nbr_cfgd    Pointer to the number of configured socket events.
*               --------------------    Argument validated in NetSock_Sel().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Stream-type socket successfully checked for 
*                                                                   write operation(s); check return value for 
*                                                                   write operation(s) availability.
*
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_INVALID_OP         Invalid socket operation.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
*                               NET_SOCK_ERR_EVENTS_NBR_MAX     Number of configured socket events is greater
*                                                                   than the maximum number of socket events.
*
*                                                               --- RETURNED BY NetSock_IsRdyTxStream() : ----
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type; i.e. NOT stream-type.
*
* Return(s)   : DEF_YES, if stream-type socket has any available write operation(s) [see Notes #2b1 & #2b3]; OR ...
*
*                        if stream-type socket's write connection is closed         [see Note  #2b2];        OR ...
*
*                        if stream-type socket has any available socket error(s)    [see Note  #2b4].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerWr().
*$PAGE*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the ... 'writefds' ... 
*                   parameter ... to see whether ... [any] descriptors ... are ready for writing" :
*
*                   (a) "A descriptor shall be considered ready for writing when a call to an output function 
*                        ... would not block, whether or not the function would transfer data successfully" :
*
*                       (1) "If a non-blocking call to the connect() function has been made for a socket, and 
*                            the connection attempt has either succeeded or failed leaving a pending error, 
*                            the socket shall be marked as writable."
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Page 165 states that "a socket is ready for writing if any of the 
*                       following ... conditions is true" :
*
*                       (1) "A write operation will not block and will return a positive value (e.g., the 
*                            number of bytes accepted by the transport layer)."
*
*                       (2) "The write half of the connection is closed."
*
*                       (3) "A socket using a non-blocking connect() has completed the connection, or the 
*                            connect() has failed."
*
*                       (4) "A socket error is pending.  A write operation on the socket will not block and 
*                            will return an error (-1) with 'errno' set to the specific error condition."
*
*                   See also 'NetSock_Sel()  Note #3b2A2'.
*
*               (3) Socket descriptor write availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor write handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also 'NetSock_TxDataHandlerStream()  Note #8'
*                          & 'NetSock_ConnHandlerStream()    Note #4'.
*********************************************************************************************************
*/

#if ((NET_SOCK_CFG_SEL_EN         == DEF_ENABLED)) && \
     (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_SelDescHandlerWrStream (NET_SOCK_ID           sock_id,
                                                     NET_SOCK             *psock,
                                                     NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                     NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                     NET_ERR              *perr)
{
    NET_CONN_ID  conn_id;
    NET_CONN_ID  conn_id_transport;
    CPU_BOOLEAN  sock_rdy;
    NET_ERR      err;

                                                                    /* ---------- HANDLE STREAM SOCK DESC WR ---------- */
                                                                    /* Get transport conn id.                           */
    conn_id           = psock->ID_Conn;
    conn_id_transport = NetConn_ID_TransportGet(conn_id, &err);
    if (  err != NET_CONN_ERR_NONE) {
        *perr  = NET_SOCK_ERR_CONN_FAIL;
         return (DEF_YES);                                          /* Rtn sock err (see Note #2b4).                    */
    }
    if  (conn_id_transport == NET_CONN_ID_NONE) {
        *perr  = NET_SOCK_ERR_CONN_FAIL;
         return (DEF_YES);                                          /* Rtn sock err (see Note #2b4).                    */
    }


    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
            *perr =  NET_SOCK_ERR_NOT_USED;                         /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;                           /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED:                                 /* Closed     stream socks NOT allowed.             */
        case NET_SOCK_STATE_BOUND:                                  /* Non-conn'd stream socks NOT allowed.             */
        case NET_SOCK_STATE_LISTEN:
            *perr =  NET_SOCK_ERR_INVALID_OP;                       /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */

/*$PAGE*/
        case NET_SOCK_STATE_CONN_IN_PROGRESS:                       /* Rtn conn-in-progress stream socks ...            */
             sock_rdy = DEF_NO;                                     /* ... as NOT rdy.                                  */
                                                                    /* Cfg sock event to wait on sock conn.             */
           (*psock_event_nbr_cfgd)++;
             if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                 *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                  return (DEF_NO);
             }
           (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_SOCK_CONN;
           (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
           (*psock_event_tbl)->ConnID    = (NET_CONN_ID)sock_id;
           (*psock_event_tbl)++;
             break;


        case NET_SOCK_STATE_CONN_DONE:                              /* Rtn conn-completed stream socks ...              */
             sock_rdy = DEF_YES;                                    /* ... as     rdy (see Note #2b3).                  */
             break;


        case NET_SOCK_STATE_CONN:                                   /* Chk conn'd stream socks               ...        */
                                                                    /* ... if rdy for tx     (see Note #2b1) ...        */
                                                                    /* ... OR tx conn closed (see Note #2b2).           */
             sock_rdy = NetSock_IsRdyTxStream(psock, conn_id_transport, &err);
             switch (err) {
                 case NET_SOCK_ERR_NONE:
                      if (sock_rdy != DEF_YES) {                    /* If sock NOT rdy, ...                             */
                                                                    /* ... cfg sock event to wait on transport tx.      */
                        (*psock_event_nbr_cfgd)++;
                          if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                              *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                               return (DEF_NO);
                          }
                        (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_TRANSPORT_TX;
                        (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
                        (*psock_event_tbl)->ConnID    = (NET_CONN_ID)conn_id_transport;
                        (*psock_event_tbl)++;
                      }
                      break;


                 case NET_SOCK_ERR_INVALID_TYPE:
                     *perr =  err;                                  /* Rtn sock err (see Note #2b4).                    */
                      return (DEF_YES);                             /* Prevent 'break NOT reachable' compiler warning.  */

                      
                 case NET_ERR_INIT_INCOMPLETE:
                 case NET_SOCK_ERR_INVALID_PROTOCOL:
                 case NET_SOCK_ERR_CONN_FAIL:
                 case NET_CONN_ERR_INVALID_CONN:
                 case NET_CONN_ERR_NOT_USED:
                 default:
                     *perr =  NET_SOCK_ERR_CONN_FAIL;               /* Rtn sock err (see Note #2b4).                    */
                      return (DEF_YES);                             /* Prevent 'break NOT reachable' compiler warning.  */
             }
             break;


        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:                      /* Rtn closing stream socks ...                     */
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
             sock_rdy = DEF_YES;                                    /* ... as rdy   (see Note #2b2).                    */
             break;


        case NET_SOCK_STATE_NONE:
        default:
            *perr =  NET_SOCK_ERR_INVALID_STATE;                    /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (sock_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetSock_SelDescHandlerErr()
*
* Description : (1) Handle socket descriptor for error(s) &/or exception(s) :
*
*                   (a) Check socket for available error(s) &/or exception(s) :
*                       (1) Socket error(s)                                         See Notes #2a2 & #2a3
*
*                   (b) Configure socket event table to wait on appropriate error 
*                           operation(s), if necessary
*
*
* Argument(s) : sock_id                 Socket descriptor/handle identifier of socket to check for 
*                                           available error(s).
*
*               psock_event_tbl         Pointer to a socket event table to configure socket events
*               ---------------             to wait on.
*
*                                       Argument validated in NetSock_Sel().
*
*               psock_event_nbr_cfgd    Pointer to the number of configured socket events.
*               --------------------    Argument validated in NetSock_Sel().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetSock_SelDescHandlerErrDatagram() : -
*                                                               -- RETURNED BY NetSock_SelDescHandlerErrStream() : --
*                               NET_SOCK_ERR_NONE               Socket successfully checked for error(s); check 
*                                                                   return value for error availability.
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type.
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*                               NET_SOCK_ERR_FAULT              Socket fault; connection(s) aborted.
*
*                               NET_SOCK_ERR_EVENTS_NBR_MAX     Number of configured socket events is greater
*                                                                   than the maximum number of socket events.
*
*                                                               -- RETURNED BY NetSock_SelDescHandlerErrStream() : --
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
*                                                               ---------- RETURNED BY NetSock_IsUsed() : -----------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*
* Return(s)   : DEF_YES, if socket has any available socket error(s) [see Note #2a].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_Sel().
*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the ... 'errorfds' ... 
*                   parameter ... to see whether ... [any] descriptors ... have an exceptional condition 
*                   pending" :
*
*                   (a) "A file descriptor ... shall be considered to have an exceptional condition pending 
*                        ... as noted below" :
*
*                       (1) (A) "A socket ... receive operation ... [that] would return out-of-band data 
*                                without blocking."
*                           (B) "A socket ... [with] out-of-band data ... present in the receive queue."
*
*                       (2) "If a socket has a pending error."
*
*                       (3) "Other circumstances under which a socket may be considered to have an exceptional 
*                            condition pending are protocol-specific and implementation-defined."
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Page 165 states that "a socket has an exception condition pending if ... 
*                       any of the following ... conditions is true" :
*
*                       (1) (A) "Out-of-band data for the socket" is currently available; ...
*                           (B) "Or the socket is still at the out-of-band mark."
*
*                   (c) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Page 165 states "that when an error occurs on a socket, it is [also] 
*                       marked as both readable and writeable by select()".
*
*                   See also 'NetSock_Sel()  Note #3b2A3'.
*
*               (3) Default case already invalidated in NetSock_Open().  However, the default case is
*                   included as an extra precaution in case 'SockType' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_SelDescHandlerErr (NET_SOCK_ID           sock_id,
                                                NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                NET_ERR              *perr)
{
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    CPU_BOOLEAN   sock_used;
#endif
    CPU_BOOLEAN   sock_rdy;
    NET_SOCK     *psock;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                     /* -------------- VALIDATE SOCK USED -------------- */
    sock_used = NetSock_IsUsed(sock_id, perr);
    if (sock_used != DEF_YES) {
        return (DEF_YES);                                           /* Rtn sock err (see Note #2a3).                    */
    }
#endif

                                                                    /* ------------- HANDLE SOCK DESC ERR ------------- */
    psock = &NetSock_Tbl[sock_id];

    switch (psock->SockType) {
        case NET_SOCK_TYPE_DATAGRAM:
             sock_rdy = NetSock_SelDescHandlerErrDatagram((NET_SOCK_ID          )sock_id,
                                                          (NET_SOCK            *)psock,
                                                          (NET_SOCK_EVENT_TBL **)psock_event_tbl,
                                                          (NET_SOCK_EVENT_QTY  *)psock_event_nbr_cfgd,
                                                          (NET_ERR             *)perr);
             break;


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
        case NET_SOCK_TYPE_STREAM:
             sock_rdy = NetSock_SelDescHandlerErrStream((NET_SOCK_ID          )sock_id,
                                                        (NET_SOCK            *)psock,
                                                        (NET_SOCK_EVENT_TBL **)psock_event_tbl,
                                                        (NET_SOCK_EVENT_QTY  *)psock_event_nbr_cfgd,
                                                        (NET_ERR             *)perr);
             break;
#endif


        case NET_SOCK_TYPE_FAULT:
            *perr =  NET_SOCK_ERR_FAULT;                            /* Rtn sock err (see Note #2a3).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_TYPE_NONE:
        default:                                                    /* See Note #3.                                     */
            *perr =  NET_SOCK_ERR_INVALID_TYPE;                     /* Rtn sock err (see Note #2a3).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


    return (sock_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetSock_SelDescHandlerErrDatagram()
*
* Description : (1) Handle datagram-type socket descriptor for error(s) &/or exception(s) :
*
*                   (a) Check datagram-type socket for available error(s) &/or exception(s) :
*
*                       (1) Socket error(s)                                         See Notes #2a2 & #2a3
*
*                   (b) Configure socket event table to wait on appropriate error 
*                           operation(s), if necessary
*
*
* Argument(s) : sock_id                 Socket descriptor/handle identifier of datagram-type socket to 
*               -------                     check for available error(s).
*
*                                       Argument checked   in NetSock_SelDescHandlerErr().
*
*               psock                   Pointer to socket.
*               -----                   Argument validated in NetSock_SelDescHandlerErr().
*
*               psock_event_tbl         Pointer to a socket event table to configure socket events
*               ---------------             to wait on.
*
*                                       Argument validated in NetSock_Sel().
*
*               psock_event_nbr_cfgd    Pointer to the number of configured socket events.
*               --------------------    Argument validated in NetSock_Sel().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Datagram-type socket successfully checked for
*                                                                   error(s); check return value for error
*                                                                   availability.
*
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*
*                               NET_SOCK_ERR_EVENTS_NBR_MAX     Number of configured socket events is greater
*                                                                   than the maximum number of socket events.
*
* Return(s)   : DEF_YES, if datagram-type socket has any available socket error(s) [see Note #2a].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerErr().
*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the ... 'errorfds' ... 
*                   parameter ... to see whether ... [any] descriptors ... have an exceptional condition 
*                   pending" :
*
*                   (a) "A file descriptor ... shall be considered to have an exceptional condition pending 
*                        ... as noted below" :
*
*                       (2) "If a socket has a pending error."
*
*                       (3) "Other circumstances under which a socket may be considered to have an exceptional 
*                            condition pending are protocol-specific and implementation-defined."
*
*                   See also 'NetSock_Sel()  Note #3b2A3'.
*
*               (3) Socket descriptor error availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor error handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also 'NetSock_RxDataHandlerDatagram()  Note #13' 
*                          & 'NetSock_TxDataHandlerDatagram()  Note #10'.
*
*               (4) Since datagram-type sockets typically never wait on transmit operations, no socket 
*                   event need be configured to wait on datagram-type socket transmit errors.
*
*                   See also 'NetSock_IsRdyTxDatagram()  Note #3'.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_SelDescHandlerErrDatagram (NET_SOCK_ID           sock_id,
                                                        NET_SOCK             *psock,
                                                        NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                        NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                        NET_ERR              *perr)
{
    CPU_BOOLEAN  sock_rdy;


                                                                    /* -------- HANDLE DATAGRAM SOCK DESC ERR --------- */
    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
            *perr =  NET_SOCK_ERR_NOT_USED;                         /* Rtn sock err (see Note #2a3).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;                           /* Rtn sock err (see Note #2a2).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED:                                 /* Rtn closed     datagram socks, ...               */
        case NET_SOCK_STATE_BOUND:                                  /* ... non-conn'd datagram socks, ...               */
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN:                                   /* ... &   conn'd datagram socks  ...               */
        case NET_SOCK_STATE_CONN_DONE:
             sock_rdy = DEF_NO;                                     /* ... as NOT rdy w/ any err(s).                    */
                                                                    /* Cfg sock event to wait on sock err(s).           */
           (*psock_event_nbr_cfgd)++;
             if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                 *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                  return (DEF_NO);
             }
           (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_SOCK_ERR_RX;
           (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
           (*psock_event_tbl)->ConnID    = (NET_CONN_ID)sock_id;
           (*psock_event_tbl)++;

#if 0                                                               /* See Note #4.                                     */
           (*psock_event_nbr_cfgd)++;
             if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                 *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                  return (DEF_NO);
             }
           (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_SOCK_ERR_TX;
           (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
           (*psock_event_tbl)->ConnID    = (NET_CONN_ID)sock_id;
           (*psock_event_tbl)++;
#endif
             break;


        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_LISTEN:                                 /* Listen  datagram socks NOT allowed.              */
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:                      /* Closing datagram socks NOT allowed.              */
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        default:
            *perr =  NET_SOCK_ERR_INVALID_STATE;                    /* Rtn sock err (see Note #2a3).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (sock_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetSock_SelDescHandlerErrStream()
*
* Description : (1) Handle stream-type socket descriptor for error(s) &/or exception(s) :
*
*                   (a) Check stream-type socket for available error(s) &/or exception(s) :
*
*                       (1) Out-of-band data                                        See Note  #2b1
*                       (2) Socket error(s)                                         See Notes #2a2 & #2a3
*
*                   (b) Configure socket event table to wait on appropriate error 
*                           operation(s), if necessary
*
*
* Description : (1) Check stream-type socket for available error(s) :
*
*
*
* Argument(s) : sock_id                 Socket descriptor/handle identifier of stream-type socket to 
*               -------                     check for available error(s).
*
*                                       Argument checked   in NetSock_SelDescHandlerErr().
*
*               psock                   Pointer to socket.
*               -----                   Argument validated in NetSock_SelDescHandlerErr().
*
*               psock_event_tbl         Pointer to a socket event table to configure socket events
*               ---------------             to wait on.
*
*                                       Argument validated in NetSock_Sel().
*
*               psock_event_nbr_cfgd    Pointer to the number of configured socket events.
*               --------------------    Argument validated in NetSock_Sel().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Stream-type socket successfully checked for
*                                                                   error(s); check return value for error
*                                                                   availability.
*
*                               NET_SOCK_ERR_INVALID_STATE      Invalid socket state.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
*                               NET_SOCK_ERR_EVENTS_NBR_MAX     Number of configured socket events is greater
*                                                                   than the maximum number of socket events.
*
* Return(s)   : DEF_YES, if stream-type socket has any available socket error(s) [see Note #2a].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerErr().
*$PAGE*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the ... 'errorfds' ... 
*                   parameter ... to see whether ... [any] descriptors ... have an exceptional condition 
*                   pending" :
*
*                   (a) "A file descriptor ... shall be considered to have an exceptional condition pending 
*                        ... as noted below" :
*
*                       (1) (A) "A socket ... receive operation ... [that] would return out-of-band data 
*                                without blocking."
*                           (B) "A socket ... [with] out-of-band data ... present in the receive queue."
*
*                           #### Out-of-band data NOT supported (see 'net_tcp.h  Note #1b').
*
*                       (2) "If a socket has a pending error."
*
*                       (3) "Other circumstances under which a socket may be considered to have an exceptional 
*                            condition pending are protocol-specific and implementation-defined."
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Page 165 states that "a socket has an exception condition pending if ... 
*                       any of the following ... conditions is true" :
*
*                       (1) (A) "Out-of-band data for the socket" is currently available; ...
*                           (B) "Or the socket is still at the out-of-band mark."
*
*                           #### Out-of-band data NOT supported (see 'net_tcp.h  Note #1b').
*
*                   See also 'NetSock_Sel()  Note #3b2A3'.
*
*               (3) Socket descriptor error availability determined by other socket handler function(s).  
*                   For correct interoperability between socket descriptor error handler functionality & 
*                   all other appropriate socket handler function(s); ANY modification to any of these 
*                   functions MUST be appropriately synchronized.
*
*                   See also 'NetSock_RxDataHandlerStream()  Note #11',
*                            'NetSock_TxDataHandlerStream()  Note #8' ,
*                            'NetSock_Accept()               Note #11',
*                          & 'NetSock_ConnHandlerStream()    Note #4'.
*********************************************************************************************************
*/

#if ((NET_SOCK_CFG_SEL_EN         == DEF_ENABLED)) && \
     (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_SelDescHandlerErrStream (NET_SOCK_ID           sock_id,
                                                      NET_SOCK             *psock,
                                                      NET_SOCK_EVENT_TBL  **psock_event_tbl,
                                                      NET_SOCK_EVENT_QTY   *psock_event_nbr_cfgd,
                                                      NET_ERR              *perr)
{
    NET_CONN_ID  conn_id;
    NET_CONN_ID  conn_id_transport;
    CPU_BOOLEAN  sock_rdy;
    NET_ERR      err;

                                                                    /* --------- HANDLE STREAM SOCK DESC ERR ---------- */
                                                                    /* Get transport conn id.                           */
    conn_id           = psock->ID_Conn;
    conn_id_transport = NetConn_ID_TransportGet(conn_id, &err);
    if (  err != NET_CONN_ERR_NONE) {
        *perr  = NET_SOCK_ERR_CONN_FAIL;
         return (DEF_YES);                                          /* Rtn sock err (see Note #2a3).                    */
    }
    if  (conn_id_transport == NET_CONN_ID_NONE) {
        *perr  = NET_SOCK_ERR_CONN_FAIL;
         return (DEF_YES);                                          /* Rtn sock err (see Note #2a3).                    */
    }


    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
            *perr =  NET_SOCK_ERR_NOT_USED;                         /* Rtn sock err (see Note #2a3).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr =  NET_SOCK_ERR_CLOSED;                           /* Rtn sock err (see Note #2a2).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */


        case NET_SOCK_STATE_CLOSED:                                 /* Rtn closed       stream socks ...                */
        case NET_SOCK_STATE_BOUND:                                  /* ... & non-conn'd stream socks ...                */
             sock_rdy = DEF_NO;                                     /* ... as NOT rdy w/ any err(s).                    */
             break;

/*$PAGE*/
        case NET_SOCK_STATE_LISTEN:                                 /* Rtn listen stream socks ...                      */
             sock_rdy = DEF_NO;                                     /* ... as NOT rdy w/ any err(s).                    */
                                                                    /* Cfg sock event to wait on sock accept err(s).    */
           (*psock_event_nbr_cfgd)++;
             if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                 *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                  return (DEF_NO);
             }
           (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_SOCK_ERR_ACCEPT;
           (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
           (*psock_event_tbl)->ConnID    = (NET_CONN_ID)sock_id;
           (*psock_event_tbl)++;
             break;


        case NET_SOCK_STATE_CONN_IN_PROGRESS:                       /* Rtn conn-in-progress stream socks ...            */
             sock_rdy = DEF_NO;                                     /* ... as NOT rdy w/ any err(s).                    */
                                                                    /* Cfg sock event to wait on sock conn err(s).      */
           (*psock_event_nbr_cfgd)++;
             if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                 *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                  return (DEF_NO);
             }
           (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_SOCK_ERR_CONN;
           (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
           (*psock_event_tbl)->ConnID    = (NET_CONN_ID)sock_id;
           (*psock_event_tbl)++;
             break;


        case NET_SOCK_STATE_CONN:                                   /* Rtn conn'd stream socks ...                      */
        case NET_SOCK_STATE_CONN_DONE:
             sock_rdy = DEF_NO;                                     /* ... as NOT rdy w/ any err(s).                    */
                                                                    /* Cfg sock event to wait on transport rx/tx err(s).*/
           (*psock_event_nbr_cfgd)++;
             if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                 *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                  return (DEF_NO);
             }
           (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_TRANSPORT_ERR_RX;
           (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
           (*psock_event_tbl)->ConnID    = (NET_CONN_ID)conn_id_transport;
           (*psock_event_tbl)++;

           (*psock_event_nbr_cfgd)++;
             if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                 *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                  return (DEF_NO);
             }
           (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_TRANSPORT_ERR_TX;
           (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
           (*psock_event_tbl)->ConnID    = (NET_CONN_ID)conn_id_transport;
           (*psock_event_tbl)++;
             break;


        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:                      /* Rtn closing stream socks ...                     */
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
             sock_rdy = DEF_NO;                                     /* ... as NOT rdy w/ any err(s).                    */
                                                                    /* Cfg sock event to wait on transport rx err(s).   */
           (*psock_event_nbr_cfgd)++;
             if (*psock_event_nbr_cfgd > NET_SOCK_CFG_SEL_NBR_EVENTS_MAX) {
                 *perr =  NET_SOCK_ERR_EVENTS_NBR_MAX;
                  return (DEF_NO);
             }
           (*psock_event_tbl)->EventType =  NET_SOCK_EVENT_TRANSPORT_ERR_RX;
           (*psock_event_tbl)->SockID    = (NET_SOCK_ID)sock_id;
           (*psock_event_tbl)->ConnID    = (NET_CONN_ID)conn_id_transport;
           (*psock_event_tbl)++;
             break;


        case NET_SOCK_STATE_NONE:
        default:
            *perr =  NET_SOCK_ERR_INVALID_STATE;                    /* Rtn sock err (see Note #2a3).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (sock_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetSock_IsAvailRxDatagram()
*
* Description : (1) Check datagram-type socket for available receive operation(s) :
*
*                   (a) Receive data                                                    See Note #2b1
*                   (b) Socket error(s)                                                 See Note #2b4
*
*
* Argument(s) : psock       Pointer to socket.
*               -----       Argument validated in NetSock_SelDescHandlerRd().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Datagram-type socket successfully checked for 
*                                                                   available receive data; check return value 
*                                                                   for receive data availability.
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type; i.e. NOT datagram-type.
*
* Return(s)   : DEF_YES, if datagram-type socket has any available receive data    [see Note #2b1]; OR ...
*
*                        if datagram-type socket has any available socket error(s) [see Note #2b4].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerRdDatagram().
*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the 'readfds' ... parameter 
*                   ... to see whether ... [any] descriptors are ready for reading" :
*
*                   (a) "A descriptor shall be considered ready for reading when a call to an input function 
*                        ... would not block, whether or not the function would transfer data successfully.  
*                        (The function might return data, an end-of-file indication, or an error other than 
*                        one indicating that it is blocked, and in each of these cases the descriptor shall 
*                        be considered ready for reading.)"
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Pages 164-165 states that "a socket is ready for reading if any of the 
*                       following ... conditions is true" :
*
*                       (1) "A read operation on the socket will not block and will return a value greater 
*                            than 0 (i.e., the data that is ready to be read)."
*
*                       (4) "A socket error is pending.  A read operation on the socket will not block and
*                            will return an error (-1) with 'errno' set to the specific error condition."
*
*                   See also 'NetSock_Sel()  Note #3b2A1'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_IsAvailRxDatagram (NET_SOCK  *psock,
                                                NET_ERR   *perr)
{
    CPU_BOOLEAN  rx_avail;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                     /* -------------- VALIDATE SOCK TYPE -------------- */
    if (psock->SockType != NET_SOCK_TYPE_DATAGRAM) {
       *perr =  NET_SOCK_ERR_INVALID_TYPE;
        return (DEF_YES);                                           /* Rtn sock err (see Note #2b4).                    */
    }
#endif

                                                                    /* -------------- CHK SOCK RX AVAIL --------------- */
    rx_avail = (psock->RxQ_Head != (NET_BUF *)0) ? DEF_YES : DEF_NO;
   *perr     =  NET_SOCK_ERR_NONE;

    return (rx_avail);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetSock_IsAvailRxStream()
*
* Description : (1) Check stream-type socket for available receive operation(s) :
*
*                   (a) Receive data                                                    See Note #2b1
*                   (b) Receive connection closed                                       See Note #2b2
*                   (c) Socket  error(s)                                                See Note #2b4
*
*
* Argument(s) : psock               Pointer to socket.
*               -----               Argument validated in NetSock_SelDescHandlerRd().
*
*               conn_id_transport   Connection's transport layer handle identifier.
*               -----------------   Argument checked   in NetSock_SelDescHandlerRdStream().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Stream-type socket successfully checked for 
*                                                                   available receive data; check return 
*                                                                   value for receive data availability.
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type; i.e. NOT stream-type.
*                               NET_SOCK_ERR_INVALID_PROTOCOL   Invalid socket protocol.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
* Return(s)   : DEF_YES, if stream-type socket has any available receive data    [see Note #2b1]; OR ...
*
*                        if stream-type socket's receive connection is closed    [see Note #2b2]; OR ...
*
*                        if stream-type socket has any available socket error(s) [see Note #2b4].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerRdStream().
*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the 'readfds' ... parameter 
*                   ... to see whether ... [any] descriptors are ready for reading" :
*
*                   (a) "A descriptor shall be considered ready for reading when a call to an input function 
*                        ... would not block, whether or not the function would transfer data successfully.  
*                        (The function might return data, an end-of-file indication, or an error other than 
*                        one indicating that it is blocked, and in each of these cases the descriptor shall 
*                        be considered ready for reading.)"
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Pages 164-165 states that "a socket is ready for reading if any of the 
*                       following ... conditions is true" :
*
*                       (1) "A read operation on the socket will not block and will return a value greater 
*                            than 0 (i.e., the data that is ready to be read)."
*                            
*                       (2) "The read half of the connection is closed (i.e., a TCP connection that has 
*                            received a FIN).  A read operation ... will not block and will return 0 (i.e.,
*                            EOF)."
*
*                       (4) "A socket error is pending.  A read operation on the socket will not block and
*                            will return an error (-1) with 'errno' set to the specific error condition."
*
*                   See also 'NetSock_Sel()  Note #3b2A1'.
*
*               (3) Default case already invalidated in NetSock_Open().  However, the default case is 
*                   included as an extra precaution in case 'Protocol' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
#if ((NET_SOCK_CFG_SEL_EN         == DEF_ENABLED)) && \
     (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_IsAvailRxStream (NET_SOCK     *psock,
                                              NET_CONN_ID   conn_id_transport,
                                              NET_ERR      *perr)
{
    CPU_BOOLEAN  rx_avail;
    NET_ERR      err;
    NET_ERR      err_rtn;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                     /* -------------- VALIDATE SOCK TYPE -------------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
       *perr =  NET_SOCK_ERR_INVALID_TYPE;
        return (DEF_YES);                                           /* Rtn sock err (see Note #2b4).                    */
    }
#endif


                                                                    /* -------------- CHK SOCK RX AVAIL --------------- */
    rx_avail = DEF_NO;
    err_rtn  = NET_SOCK_ERR_CONN_FAIL;

    switch (psock->Protocol) {                                      /* Chk rx avail from transport layer.               */
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
        case NET_SOCK_PROTOCOL_TCP:
             rx_avail = NetTCP_ConnIsAvailRx((NET_TCP_CONN_ID) conn_id_transport,
                                             (NET_ERR       *)&err);
             switch (err) {
                 case NET_TCP_ERR_NONE:
                      err_rtn = NET_SOCK_ERR_NONE;
                      break;


                 case NET_ERR_INIT_INCOMPLETE:
                 case NET_TCP_ERR_CONN_NOT_USED:
                 case NET_TCP_ERR_INVALID_CONN:
                 case NET_TCP_ERR_INVALID_CONN_STATE:
                 case NET_TCP_ERR_INVALID_CONN_OP:
                 default:
                      err_rtn = NET_SOCK_ERR_CONN_FAIL;
                      break;
             }
             break;
#endif
#endif

        case NET_SOCK_PROTOCOL_NONE:
        default:                                                    /* See Note #3.                                     */
            (void)&err;                                             /* Prevent possible compiler warning.               */
            *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;                 /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


    switch (err_rtn) {                                              /* Demux transport-to-sock layer err.               */
        case NET_SOCK_ERR_NONE:                                     /* Sock rx data avail  (see Note #2b1) OR ...       */
                                                                    /* ...  rx conn closed (see Note #2b2).             */
             break;


        case NET_SOCK_ERR_CONN_FAIL:
        default:
            *perr =  err_rtn;                                       /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (rx_avail);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetSock_IsRdyTxDatagram()
*
* Description : (1) Check datagram-type socket ready for transmit operation(s) :
*
*                   (a) Transmit data                                                   See Note #2b1
*                   (b) Socket error(s)                                                 See Note #2b4
*
*
* Argument(s) : psock       Pointer to socket.
*               -----       Argument validated in NetSock_SelDescHandlerWr().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Datagram-type socket successfully checked if 
*                                                                   ready for transmit; check return value 
*                                                                   for transmit ready availability.
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type; i.e. NOT datagram-type.
*
* Return(s)   : DEF_YES, if datagram-type socket is ready to transmit data         [see Note #2b1]; OR ...
*
*                        if datagram-type socket has any available socket error(s) [see Note #2b4].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerWrDatagram().
*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the ... 'writefds' ... 
*                   parameter ... to see whether ... [any] descriptors ... are ready for writing" :
*
*                   (a) "A descriptor shall be considered ready for writing when a call to an output function
*                        ... would not block, whether or not the function would transfer data successfully."
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Page 165 states that "a socket is ready for writing if any of the
*                       following ... conditions is true" :
*
*                       (1) "A write operation will not block and will return a positive value (e.g., the
*                            number of bytes accepted by the transport layer)."
*
*                       (4) "A socket error is pending.  A write operation on the socket will not block and
*                            will return an error (-1) with 'errno' set to the specific error condition."
*
*                   See also 'NetSock_Sel()  Note #3b2A2'.
*
*               (3) Datagram-type sockets are typically always available for transmit operations.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_IsRdyTxDatagram (NET_SOCK  *psock,
                                              NET_ERR   *perr)
{
    CPU_BOOLEAN  tx_rdy;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                     /* -------------- VALIDATE SOCK TYPE -------------- */
    if (psock->SockType != NET_SOCK_TYPE_DATAGRAM) {
       *perr =  NET_SOCK_ERR_INVALID_TYPE;
        return (DEF_YES);                                           /* Rtn sock err (see Note #2b4).                    */
    }
#endif

                                                                    /* --------------- CHK SOCK TX RDY ---------------- */
    tx_rdy = DEF_YES;                                               /* See Note #3.                                     */
   *perr   = NET_SOCK_ERR_NONE;

    return (tx_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetSock_IsRdyTxStream()
*
* Description : (1) Check stream-type socket ready for transmit operation(s) :
*
*                   (a) Transmit data                                                   See Note #2b1
*                   (b) Transmit connection closed                                      See Note #2b2
*                   (c) Socket error(s)                                                 See Note #2b4
*
*
* Argument(s) : psock               Pointer to socket.
*               -----               Argument validated in NetSock_SelDescHandlerWr().
*
*               conn_id_transport   Connection's transport layer handle identifier.
*               -----------------   Argument checked   in NetSock_SelDescHandlerWrStream().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Stream-type socket successfully checked if 
*                                                                   ready for transmit; check return value 
*                                                                   for transmit ready availability.
*
*                               NET_SOCK_ERR_INVALID_TYPE       Invalid socket type; i.e. NOT stream-type.
*                               NET_SOCK_ERR_INVALID_PROTOCOL   Invalid socket protocol.
*                               NET_SOCK_ERR_CONN_FAIL          Socket connection operation(s) failed.
*
* Return(s)   : DEF_YES, if stream-type socket is ready to transmit data         [see Note #2b1]; OR ...
*
*                        if stream-type socket's transmit connection is closed   [see Note #2b2]; OR ...
*
*                        if stream-type socket has any available socket error(s) [see Note #2b4].
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_SelDescHandlerWrStream().
*
* Note(s)     : (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the select() 
*                   function shall ... examine the file descriptor[s] ... passed in the ... 'writefds' ... 
*                   parameter ... to see whether ... [any] descriptors ... are ready for writing" :
*
*                   (a) "A descriptor shall be considered ready for writing when a call to an output function 
*                        ... would not block, whether or not the function would transfer data successfully."
*
*                   (b) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                       Section 6.3, Page 165 states that "a socket is ready for writing if any of the 
*                       following ... conditions is true" :
*
*                       (1) "A write operation will not block and will return a positive value (e.g., the 
*                            number of bytes accepted by the transport layer)."
*
*                       (2) "The write half of the connection is closed."
*
*                       (4) "A socket error is pending.  A write operation on the socket will not block and 
*                            will return an error (-1) with 'errno' set to the specific error condition."
*
*                   See also 'NetSock_Sel()  Note #3b2A2'.
*
*               (3) Default case already invalidated in NetSock_Open().  However, the default case is 
*                   included as an extra precaution in case 'Protocol' is incorrectly modified.
*********************************************************************************************************
*/
/*$PAGE*/
#if ((NET_SOCK_CFG_SEL_EN         == DEF_ENABLED)) && \
     (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_IsRdyTxStream (NET_SOCK     *psock,
                                            NET_CONN_ID   conn_id_transport,
                                            NET_ERR      *perr)
{
    CPU_BOOLEAN  tx_rdy;
    NET_ERR      err;
    NET_ERR      err_rtn;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                     /* -------------- VALIDATE SOCK TYPE -------------- */
    if (psock->SockType != NET_SOCK_TYPE_STREAM) {
       *perr =  NET_SOCK_ERR_INVALID_TYPE;
        return (DEF_YES);                                           /* Rtn sock err (see Note #2b4).                    */
    }
#endif


                                                                    /* --------------- CHK SOCK TX RDY ---------------- */
    tx_rdy  = DEF_NO;
    err_rtn = NET_SOCK_ERR_CONN_FAIL;

    switch (psock->Protocol) {                                      /* Chk tx rdy status from transport layer.          */
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
        case NET_SOCK_PROTOCOL_TCP:
             tx_rdy = NetTCP_ConnIsRdyTx((NET_TCP_CONN_ID) conn_id_transport,
                                         (NET_ERR       *)&err);
             switch (err) {
                 case NET_TCP_ERR_NONE:
                      err_rtn = NET_SOCK_ERR_NONE;
                      break;


                 case NET_ERR_INIT_INCOMPLETE:
                 case NET_TCP_ERR_CONN_NOT_USED:
                 case NET_TCP_ERR_INVALID_CONN:
                 case NET_TCP_ERR_INVALID_CONN_STATE:
                 case NET_TCP_ERR_INVALID_CONN_OP:
                 default:
                      err_rtn = NET_SOCK_ERR_CONN_FAIL;
                      break;
             }
             break;
#endif
#endif

        case NET_SOCK_PROTOCOL_NONE:
        default:                                                    /* See Note #3.                                     */
            (void)&err;                                             /* Prevent possible compiler warning.               */
            *perr =  NET_SOCK_ERR_INVALID_PROTOCOL;                 /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


    switch (err_rtn) {                                              /* Demux transport-to-sock layer err.               */
        case NET_SOCK_ERR_NONE:                                     /* Sock rx data avail  (see Note #2b1) OR ...       */
                                                                    /* ...  rx conn closed (see Note #2b2).             */
             break;


        case NET_SOCK_ERR_CONN_FAIL:
        default:
            *perr =  err_rtn;                                       /* Rtn sock err (see Note #2b4).                    */
             return (DEF_YES);                                      /* Prevent 'break NOT reachable' compiler warning.  */
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (tx_rdy);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetSock_Get()
*
* Description : (1) Allocate & initialize a socket :
*
*                   (a) Get      a socket
*                   (b) Validate   socket
*                   (c) Initialize socket
*                   (d) Update     socket pool statistics
*                   (e) Return pointer to socket
*                         OR
*                       Null pointer & error code, on failure
*
*               (2) The socket pool is implemented as a stack :
*
*                   (a) 'NetSock_PoolPtr' points to the head of the socket pool.
*
*                   (b) Sockets' 'NextPtr's link each socket to form  the socket pool stack.
*
*                   (c) Sockets are inserted & removed at the head of the socket pool stack.
*
*
*                                        Sockets are
*                                     inserted & removed
*                                        at the head
*                                       (see Note #2c)
*
*                                             |                 NextPtr
*                                             |             (see Note #2b)
*                                             v                    |
*                                                                  |
*                                          -------       -------   v   -------       -------
*                         Socket Pool ---->|     |------>|     |------>|     |------>|     |
*                           Pointer        |     |       |     |       |     |       |     |
*                                          |     |       |     |       |     |       |     |
*                        (see Note #2a)    -------       -------       -------       -------
*
*                                          |                                               |
*                                          |<----------- Pool of Free Sockets ------------>|
*                                          |                (see Note #2)                  |
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket successfully allocated & initialized.
*                               NET_SOCK_ERR_NONE_AVAIL             NO available sockets to allocate.
*                               NET_SOCK_ERR_INVALID_TYPE           Socket is NOT a valid type.
*
* Return(s)   : Pointer to socket, if NO errors.
*
*               Pointer to NULL,   otherwise.
*
* Caller(s)   : NetSock_Open().
*
* Note(s)     : (3) (a) Socket pool is accessed by 'NetSock_PoolPtr' during execution of
*
*                       (1) NetSock_Init()
*                       (2) NetSock_Get()
*                       (3) NetSock_Free()
*
*                   (b) Since the primary tasks of the network protocol suite are prevented from running 
*                       concurrently (see 'net.h  Note #2'), it is NOT necessary to protect the shared 
*                       resources of the socket pool since no asynchronous access from other network
*                       tasks is possible.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_SOCK  *NetSock_Get (NET_ERR  *perr)
{
    NET_SOCK  *psock;
    NET_ERR    stat_err;


                                                                /* --------------------- GET SOCK --------------------- */
    if (NetSock_PoolPtr != (NET_SOCK *)0) {                     /* If sock pool NOT empty, get sock from pool.          */
        psock            = (NET_SOCK *)NetSock_PoolPtr;
        NetSock_PoolPtr  = (NET_SOCK *)psock->NextPtr;

    } else {                                                    /* If none avail, rtn err.                              */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNoneAvailCtr);
       *perr =   NET_SOCK_ERR_NONE_AVAIL;
        return ((NET_SOCK *)0);
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------ VALIDATE SOCK ------------------- */
    if (psock->Type != NET_SOCK_TYPE_SOCK) {
        NetSock_Discard(psock);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidTypeCtr);
       *perr =   NET_SOCK_ERR_INVALID_TYPE;
        return ((NET_SOCK *)0);
    }
#endif

                                                                /* -------------------- INIT SOCK --------------------- */
    NetSock_Clr(psock);
    DEF_BIT_SET(psock->Flags, NET_SOCK_FLAG_USED);              /* Set sock as used.                                    */
    psock->State = NET_SOCK_STATE_CLOSED;

                                                                /* -------------- UPDATE SOCK POOL STATS -------------- */
    NetStat_PoolEntryUsedInc(&NetSock_PoolStat, &stat_err);

   *perr =  NET_SOCK_ERR_NONE;

    return (psock);                                             /* -------------------- RTN SOCK ---------------------- */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_GetConnTransport()
*
* Description : (1) Allocate a transport layer connection :
*
*                   (a) Get a transport connection
*                   (b) Set connection handle identifiers
*
*
* Argument(s) : psock       Pointer to a socket.
*               -----       Argument validated in NetSock_Listen(),
*                                                 NetSock_ConnHandlerStream().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket transport connection successfully
*                                                                   allocated.
*                               NET_SOCK_ERR_CONN_FAIL          Socket transport connection allocation or
*                                                                   configuration failed.
*                               NET_SOCK_ERR_INVALID_FAMILY     Invalid socket protocol family.
*                               NET_SOCK_ERR_INVALID_PROTOCOL   Invalid socket protocol.
*
*                                                               - RETURNED BY NetConn_ID_TransportSet() : -
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : Transport connection handler identifer, if NO errors.
*
*               NET_CONN_ID_NONE,                       otherwise.
*
* Caller(s)   : NetSock_Listen(),
*               NetSock_ConnHandlerStream().
*
* Note(s)     : (2) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*
*               (3) On ANY errors after the transport connection is allocated, the transport connection
*                   MUST be freed.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  NET_CONN_ID  NetSock_GetConnTransport (NET_SOCK  *psock,
                                               NET_ERR   *perr)
{
    NET_CONN_ID  conn_id_transport;
    NET_CONN_ID  conn_id;
    CPU_BOOLEAN  conn_err;
    NET_ERR      err;


                                                                    /* -------------- GET TRANSPORT CONN -------------- */
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
    conn_id_transport = (NET_CONN_ID)NetTCP_ConnGet(&err);
   *perr              = (err == NET_TCP_ERR_NONE) ? NET_SOCK_ERR_NONE
                                                  : NET_SOCK_ERR_CONN_FAIL;
#else
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidProtocolCtr);
   (void)&conn_id_transport;                                        /* Prevent compiler warnings.                       */
   (void)&err;
   *perr =  NET_SOCK_ERR_CONN_FAIL;
#endif

#else                                                               /* See Note #2.                                     */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
   *perr =  NET_SOCK_ERR_INVALID_FAMILY;
    return (NET_CONN_ID_NONE);
#endif

    if (*perr != NET_SOCK_ERR_NONE) {
         return (NET_CONN_ID_NONE);
    }


                                                                    /* ----------------- SET CONN IDs ----------------- */
    conn_id  = psock->ID_Conn;
    conn_err = DEF_NO;
                                                                    /* Set conn's transport id.                         */
    NetConn_ID_TransportSet(conn_id, conn_id_transport, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         conn_err = DEF_YES;
    }

                                                                    /* Set transport's conn id.                         */
    if (conn_err == DEF_NO) {
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
        NetTCP_ConnSetID_Conn((NET_TCP_CONN_ID) conn_id_transport,
                              (NET_CONN_ID    ) conn_id,
                              (NET_ERR       *)&err);
       *perr = (err != NET_TCP_ERR_NONE) ? NET_SOCK_ERR_CONN_FAIL
                                         : NET_SOCK_ERR_NONE;
#endif

#else                                                               /* See Note #2.                                     */
        conn_err = DEF_YES;
#endif
    }

    if (*perr != NET_SOCK_ERR_NONE) {
         conn_err = DEF_YES;
    }


    if (conn_err != DEF_NO) {                                       /* If any errs, free transport conn (see Note #3).  */
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
#ifdef  NET_TCP_MODULE_PRESENT
        NetTCP_ConnFree((NET_TCP_CONN_ID)conn_id_transport);
#endif

#else                                                               /* See Note #2.                                     */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidFamilyCtr);
       *perr =  NET_SOCK_ERR_INVALID_FAMILY;
#endif

        return (NET_CONN_ID_NONE);
    }


   *perr =  NET_SOCK_ERR_NONE;

    return (conn_id_transport);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetSock_CloseHandler()
*
* Description : (1) Close a socket for valid socket close operations :
*
*                   (a) Close socket
*                   (b) Free  socket
*
*
* Argument(s) : psock       Pointer to a socket.
*               -----       Argument validated in NetSock_Close(),
*                                                 NetSock_CloseHandlerStream(),
*                                                 NetSock_CloseSockFromClose(),
*                                                 various.
*
*               close_conn              Indicate whether to close network   connection (see Note #2) :
*
*                                           DEF_YES                    Close network   connection.
*                                           DEF_NO              Do NOT close network   connection.
*
*               close_conn_transport    Indicate whether to close transport connection (see Note #2) :
*
*                                           DEF_YES                    Close transport connection.
*                                           DEF_NO              Do NOT close transport connection.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Close(),
*               NetSock_CloseHandlerStream(),
*               NetSock_CloseSockFromClose(),
*               various.
*
* Note(s)     : (2) Except when closed from the network connection layer, a socket SHOULD close its
*                   network connection(s).
*
*                   See also 'NetSock_CloseSockHandler()  Note #2a'.
*
*               (3) On any socket close handler errors, socket already discarded in close handler (see
*                   'NetSock_FreeHandler()  Note #3').
*
*                   #### Continued access to the discarded socket by the application layer will likely
*                   corrupt the network socket layer.
*
*                   See also 'NetSock_Close()  Note #2'.
*********************************************************************************************************
*/

static  void  NetSock_CloseHandler (NET_SOCK     *psock,
                                    CPU_BOOLEAN   close_conn,
                                    CPU_BOOLEAN   close_conn_transport)
{
    NET_ERR  err;

                                                                /* ------------------- CLOSE SOCK --------------------- */
    NetSock_CloseSockHandler(psock, close_conn, close_conn_transport, &err);
    if (err != NET_SOCK_ERR_NONE) {                             /* See Note #3.                                         */
        return;
    }
                                                                /* -------------------- FREE SOCK --------------------- */
    NetSock_Free(psock);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetSock_CloseSock()
*
* Description : (1) Close a socket due to socket connection fault(s) :
*
*                   (a) Update socket close statistic(s)
*                   (b) Close  socket
*
*
*               (2) (a) Socket connection closed when :
*
*                       (1) Socket parameters are corrupted :
*                           (A) Family
*                           (B) Protocol
*                           (C) Type
*                           (D) State
*
*                       (2) Certain valid socket operations fail
*
*                       (3) Socket connection(s) close internally (see Note #1c)
*
*                           See also 'NetSock_CloseFromConn()  Note #1'.
*
*                   (b) Frees socket resources, closes socket connection(s), & returns socket to CLOSED 
*                       state -- but does NOT free the socket since NO mechanism or API exists to close
*                       an application's reference to the socket.
*
*                       When a socket is internally closed, ALL application-to-socket access fails &
*                       forces the application to close the socket.
*
*                   (c) #### When socket connection operations fail & close the socket, the socket MUST
*                       abort continued/further socket operations.  In other words, when certain fatal
*                       socket errors are returned, the application MUST immediately close the socket :
*
*                       (1) NET_SOCK_ERR_NOT_USED
*                       (2) NET_SOCK_ERR_INVALID_FAMILY
*                       (3) NET_SOCK_ERR_INVALID_PROTOCOL
*                       (4) NET_SOCK_ERR_INVALID_TYPE
*                       (5) NET_SOCK_ERR_INVALID_STATE
*                       (6) NET_SOCK_ERR_FAULT
*
*                       See also 'NetSock_Close()  Note #2'.
*
*
* Argument(s) : psock                   Pointer to a socket.
*               -----                   Argument validated in various.
*
*               close_conn              Indicate whether to close network   connection (see Note #3) :
*
*                                           DEF_YES                    Close network   connection.
*                                           DEF_NO              Do NOT close network   connection.
*
*               close_conn_transport    Indicate whether to close transport connection (see Note #3) :
*
*                                           DEF_YES                    Close transport connection.
*                                           DEF_NO              Do NOT close transport connection.
*
* Return(s)   : none.
*
* Caller(s)   : various,
*               NetSock_CloseFromConn().
*
* Note(s)     : (3) Except when closed from the network connection layer, a socket SHOULD close its
*                   network connection(s).
*
*                   See also 'NetSock_CloseSockHandler()  Note #2a'.
*
*               (4) If socket close handler fails, socket already discarded in close handler (see
*                   'NetSock_FreeHandler()  Note #3').
*
*                   #### Continued access to the discarded socket by the application layer will likely
*                   corrupt the network socket layer (see Note #2c).
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetSock_CloseSock (NET_SOCK     *psock,
                                 CPU_BOOLEAN   close_conn,
                                 CPU_BOOLEAN   close_conn_transport)
{
    NET_ERR  err;

                                                                /* ---------------- UPDATE CLOSE STATS ---------------- */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrCloseCtr);

                                                                /* -------------------- CLOSE SOCK -------------------- */
    NetSock_CloseSockHandler(psock, close_conn, close_conn_transport, &err);
    if (err != NET_SOCK_ERR_NONE) {                             /* See Note #4.                                         */
        return;
    }
                                                                /* Close sock (see Note #2b).                           */
    psock->ProtocolFamily = NET_SOCK_PROTOCOL_FAMILY_NONE;
    psock->Protocol       = NET_SOCK_PROTOCOL_NONE;
    psock->SockType       = NET_SOCK_TYPE_FAULT;
    psock->State          = NET_SOCK_STATE_CLOSED_FAULT;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_CloseSockHandler()
*
* Description : (1) Close a socket :
*
*                   (a) Free  socket
*                   (b) Close socket connection(s)                          See Note #2
*
*
* Argument(s) : psock       Pointer to a socket.
*               -----       Argument validated in NetSock_CloseHandler(),
*                                                 NetSock_CloseSockHandler().
*
*               close_conn              Indicate whether to close network   connection (see Note #2a) :
*
*                                           DEF_YES                    Close network   connection.
*                                           DEF_NO              Do NOT close network   connection.
*
*               close_conn_transport    Indicate whether to close transport connection (see Note #2a) :
*
*                                           DEF_YES                    Close transport connection.
*                                           DEF_NO              Do NOT close transport connection.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection successfully closed.
*                               NET_SOCK_ERR_NOT_USED           Socket NOT currently used.
*                               NET_SOCK_ERR_CLOSED             Socket already closed.
*
*                                                               - RETURNED BY NetSock_FreeHandler() : -
*                               NET_SOCK_ERR_INVALID_TYPE       Socket is NOT a valid socket type.
*                               NET_SOCK_ERR_CONN_SIGNAL_CLR    Socket connection    signal clear failed.
*                               NET_SOCK_ERR_RX_Q_SIGNAL_CLR    Socket receive queue signal clear failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CloseHandler(),
*               NetSock_CloseSock().
*
* Note(s)     : (2) (a) When a socket closes, specified socket network connection(s) MAY be closed
*                       &/or network connection handle identifier(s) MAY be cleared :
*
*                       (1) Network   connection
*                       (2) Transport connection
*
*                       See also 'NetSock_CloseHandler()        Note #2',
*                                'NetSock_CloseSock()           Note #3',
*                                'NetSock_CloseSockFromClose()  Note #3',
*                              & 'NetSock_CloseFromConn()       Note #1'.
*
*                           (A) A socket may maintain its network connection despite closing the transport
*                               connection, but a socket MUST NOT close the network connection but expect 
*                               to maintain the transport connection.
*
*                           (B) Network connection(s) MAY be closed even if socket close fails (see also
*                               Note #4).
*
*                   (b) Socket's network connection handle identifier MUST be :
*
*                       (1) Obtained PRIOR to any socket free operation
*                       (2) Cleared  after        socket connection(s) closed
*
*               (3) #### To prevent closing a socket already closed via previous socket close,
*                   NetSock_CloseSockHandler() checks the socket's 'USED' flag BEFORE closing
*                   the socket.
*
*                   This prevention is only best-effort since any invalid duplicate socket closes
*                   MAY be asynchronous to potentially valid socket opens.  Thus the invalid socket 
*                   close(s) MAY corrupt the socket's valid operation(s).
*
*                   However, since the primary tasks of the network protocol suite are prevented 
*                   from running concurrently (see 'net.h  Note #2'), it is NOT necessary to protect 
*                   socket resources from possible corruption since no asynchronous access from
*                   other network tasks is possible.
*
*               (4) On any socket close handler errors, socket already discarded in close handler (see
*                   'NetSock_FreeHandler()  Note #3a').
*
*                   #### Continued access to the discarded socket by the application layer will likely
*                   corrupt the network socket layer.
*
*                   See also 'NetSock_Close()  Note #2'.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetSock_CloseSockHandler (NET_SOCK     *psock,
                                        CPU_BOOLEAN   close_conn,
                                        CPU_BOOLEAN   close_conn_transport,
                                        NET_ERR      *perr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN  used;
#endif
    NET_CONN_ID  conn_id;

                                                                /* --------------- VALIDATE SOCK CLOSE ---------------- */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    used = DEF_BIT_IS_SET(psock->Flags, NET_SOCK_FLAG_USED);
    if (used != DEF_YES) {                                      /* If sock NOT used, ...                                */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrNotUsedCtr);
       *perr = NET_SOCK_ERR_NOT_USED;                           /* ... rtn but do NOT close (see Note #3).              */
        return;
    }
#endif

    switch (psock->State) {
        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
        default:
             break;

                                                                /* Sock NOT re-closed if already free/closed.           */
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
            *perr = NET_SOCK_ERR_NOT_USED;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_SOCK_STATE_CLOSED_FAULT:
            *perr = NET_SOCK_ERR_NONE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* -------------------- FREE SOCK --------------------- */
    conn_id = psock->ID_Conn;                                   /* Get sock's net conn id  (see Note #2b1).             */
    NetSock_FreeHandler(psock, perr);

                                                                /* ------------------ CLOSE CONN(S) ------------------- */
    if (close_conn == DEF_YES) {                                /* If req'd, close conn(s) [see Note #2a].              */
        NetConn_CloseFromApp(conn_id, close_conn_transport);
    }

                                                                /* Chk free err(s) AFTER conn close(s) [see Note #2aB]. */
    if (*perr != NET_SOCK_ERR_NONE) {                           /* On  free err(s), abort sock close   (see Note #4).   */
         return;
    }

                                                                /* ------------------ CLR SOCK CONN ------------------- */
    psock->ID_Conn = NET_CONN_ID_NONE;                          /* Clr sock's net conn id (see Note #2b2).              */


   *perr = NET_SOCK_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetSock_CloseSockFromClose()
*
* Description : (1) Close a socket due to socket close/closing faults :
*
*                   (a) Update socket close statistic(s)
*                   (b) Close  socket
*
*
*               (2) (a) (1) MUST ONLY be called by socket closing functions (see Note #2b).
*
*                       (2) Close socket connection for the following socket close/closing faults :
*
*                           (A) Socket close parameters are corrupted :
*                               (1) Type
*                               (2) State
*
*                   (b) Frees socket resources, closes socket connection(s), returns socket to CLOSED 
*                       state AND frees the socket since the close was initiated by the application.
*
*                       See also 'NetSock_CloseHandler()  Note #1'.
*
*                   See also 'NetSock_CloseSock()  Note #2'.
*
*
* Argument(s) : psock       Pointer to a socket.
*               -----       Argument validated in NetSock_Close(),
*                                                 NetSock_CloseHandlerStream().
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Close(),
*               NetSock_CloseHandlerStream().
*
* Note(s)     : (3) A socket closing with fault(s) SHOULD close its network connection(s).
*
*                   See also 'NetSock_CloseSockHandler()  Note #2a'.
*
*               (4) If socket close handler fails, socket already discarded in close handler (see
*                   'NetSock_FreeHandler()  Note #3').
*
*                   #### Continued access to the discarded socket by the application layer will likely
*                   corrupt the network socket layer.
*
*                   See also 'NetSock_Close()  Note #2'.
*********************************************************************************************************
*/

static  void  NetSock_CloseSockFromClose (NET_SOCK  *psock)
{
                                                                /* ---------------- UPDATE CLOSE STATS ---------------- */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrCloseCtr);
                                                                /* -------------------- CLOSE SOCK -------------------- */
    NetSock_CloseHandler(psock, DEF_YES, DEF_YES);              /* See Note #3.                                         */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetSock_CloseConn()
*
* Description : (1) Close a socket's network connection(s) :
*
*                   (a) Remove connection handle identifier from socket's connection accept queue
*                   (b) Close  network connection(s)
*
*
*               (2) Closes socket network connection(s) ONLY; does NOT free or close socket(s).
*
*
* Argument(s) : conn_id     Handle identifier of network connection to close.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_RxPktDemux(),
*               NetSock_Accept().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if ((NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN  == DEF_ENABLED))
static  void  NetSock_CloseConn (NET_CONN_ID  conn_id)
{
    NetSock_CloseConnFree(conn_id);
    NetConn_CloseFromApp(conn_id, DEF_YES);                     /* See Note #1b.                                        */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetSock_CloseConnFree()
*
* Description : Close/free network connection from socket.
*
* Argument(s) : conn_id     Handle identifier of network connection.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CloseConn().
*
* Note(s)     : (1) Network connections are de-referenced from cloned socket application connections.
*
*                   See also 'NetSock_FreeConnFromSock()  Note #2b'.
*********************************************************************************************************
*/

#if ((NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN  == DEF_ENABLED))
static  void  NetSock_CloseConnFree (NET_CONN_ID  conn_id)
{
    NET_SOCK_ID  sock_id;
    NET_ERR      err;

                                                                    /* ----------------- GET SOCK ID ------------------ */
    sock_id = (NET_SOCK_ID)NetConn_ID_AppCloneGet(conn_id, &err);   /* Get clone sock app conn (see Note #1).           */
    if (err != NET_CONN_ERR_NONE) {
        return;
    }
                                                                    /* ------------- FREE CONN FROM SOCK -------------- */
    NetSock_FreeConnFromSock(sock_id, conn_id);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetSock_Free()
*
* Description : (1) Free a socket :
*
*                   (a) Clear  socket controls
*                   (b) Free   socket back to socket pool
*                   (c) Update socket pool statistics
*
*
* Argument(s) : psock       Pointer to a socket.
*               -----       Argument validated in NetSock_Close(),
*                                                 NetSock_CloseHandler(),
*                                                 NetSock_CloseHandlerStream().
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Close(),
*               NetSock_CloseHandler(),
*               NetSock_CloseHandlerStream().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetSock_Free (NET_SOCK  *psock)
{
    NET_ERR  err;

                                                                /* --------------------- CLR SOCK --------------------- */
    psock->State = NET_SOCK_STATE_FREE;                         /* Set sock as freed/NOT used.                          */
    DEF_BIT_CLR(psock->Flags, NET_SOCK_FLAG_USED);

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    NetSock_Clr(psock);
#endif    

                                                                /* -------------------- FREE SOCK --------------------- */
    psock->NextPtr  = NetSock_PoolPtr;
    NetSock_PoolPtr = psock;                         

                                                                /* -------------- UPDATE SOCK POOL STATS -------------- */
    NetStat_PoolEntryUsedDec(&NetSock_PoolStat, &err);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetSock_FreeHandler()
*
* Description : (1) Free a socket :
*
*                   (a) Free  socket address                                    See Note #2
*                   (b) Clear socket connection
*                   (c) Free  socket packet buffer queues
*
*
* Argument(s) : psock       Pointer to a socket.
*               -----       Argument checked in NetSock_CloseSockHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function
*                               (see Note #3) :
*
*                               NET_SOCK_ERR_NONE               Socket connection successfully freed.
*                               NET_SOCK_ERR_INVALID_TYPE       Socket is NOT a valid socket type.
*
*                                                               --- RETURNED BY NetOS_Sock_ConnReqClr() : ----
*                                                               - RETURNED BY NetOS_Sock_ConnAcceptQ_Clr() : -
*                                                               -- RETURNED BY NetOS_Sock_ConnCloseClr() : ---
*                               NET_SOCK_ERR_CONN_SIGNAL_CLR    Socket connection    signal clear failed.
*
*                                                               ----- RETURNED BY NetOS_Sock_RxQ_Clr() : -----
*                               NET_SOCK_ERR_RX_Q_SIGNAL_CLR    Socket receive queue signal clear failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CloseSockHandler().
*
* Note(s)     : (2) Since connection address information is required to free the socket address,
*                   the socket address MUST be freed PRIOR to the network connection.
*
*                   See also 'NetSock_FreeAddr()  Note #1'
*                          & 'NetSock_CloseSockHandler()  Notes #1a & #1b'.
*
*               (3) (a) On ANY free socket connection handler error, socket is discarded & caller 
*                       function MUST NOT further handle or re-discard the socket.
*
*                   (b) ALL network resources linked to the socket MUST be freed PRIOR to socket 
*                       free or discard so that no network resources are lost.
*
*                   (c) The error code returned by 'perr' for a socket discard refers to the last 
*                       free socket error ONLY.
*
*               (4) See 'net_sock.h  NETWORK SOCKET DATA TYPE  Note #2'.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetSock_FreeHandler (NET_SOCK  *psock,
                                   NET_ERR   *perr)
{
    CPU_BOOLEAN  discard;
    NET_ERR      err_discard;
    NET_ERR      err;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------ VALIDATE TYPE ------------------- */
    if (psock->Type != NET_SOCK_TYPE_SOCK) {
        NetSock_Discard(psock);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrInvalidTypeCtr);
       *perr = NET_SOCK_ERR_INVALID_TYPE;
        return;
    }
#endif


                                                                /* ------------------ FREE SOCK ADDR ------------------ */
    NetSock_FreeAddr(psock);                                    /* See Note #2.                                         */


                                                                /* ------------------- CLR SOCK CONN ------------------ */
    discard = DEF_NO;

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
    NetOS_Sock_ConnReqClr(psock->ID, &err);
    if (err != NET_SOCK_ERR_NONE) {                             /* If  sock conn req    NOT clr'd, ...                  */
        discard     = DEF_YES;                                  /* ... discard sock (see Note #3b).                     */
        err_discard = err;
    }

    NetOS_Sock_ConnReqAbort(psock->ID, &err);                   /* Abort wait on sock conn req.                         */


    NetSock_ConnAcceptQ_Clr(psock);                             /* Clr sock conn accept Q.                              */
    NetOS_Sock_ConnAcceptQ_Clr(psock->ID, &err);
    if (err != NET_SOCK_ERR_NONE) {                             /* If  sock conn accept Q NOT clr'd, ...                */
        discard     = DEF_YES;                                  /* ... discard sock (see Note #3b).                     */
        err_discard = err;
    }

    NetOS_Sock_ConnAcceptQ_Abort(psock->ID, &err);              /* Abort wait on sock conn accept Q.                    */


    NetOS_Sock_ConnCloseClr(psock->ID, &err);
    if (err != NET_SOCK_ERR_NONE) {                             /* If  sock conn close  NOT clr'd, ...                  */
        discard     = DEF_YES;                                  /* ... discard sock (see Note #3b).                     */
        err_discard = err;
    }

    NetOS_Sock_ConnCloseAbort(psock->ID, &err);                 /* Abort wait on sock conn close.                       */
#endif


                                                                /* ------------------ FREE SOCK Q's ------------------- */
    NetSock_FreeBufQ(&psock->RxQ_Head, &psock->RxQ_Tail);
#if 0                                                           /* See Note #4.                                         */
    NetSock_FreeBufQ(&psock->TxQ_Head, &psock->TxQ_Tail);
#endif


    NetOS_Sock_RxQ_Clr(psock->ID, &err);                        /* Clr sock rx Q.                                       */
    if (err != NET_SOCK_ERR_NONE) {                             /* If  sock rx Q NOT clr'd, ...                         */
        discard     = DEF_YES;                                  /* ... discard sock (see Note #3b).                     */
        err_discard = err;
    }

    NetOS_Sock_RxQ_Abort(psock->ID, &err);                      /* Abort wait on sock rx Q.                             */


                                                                /* -------------- DISCARD SOCK ON ERR(S) -------------- */
    if (discard != DEF_NO) {                                    /* On sock free err(s), ...                             */
        psock->State = NET_SOCK_STATE_DISCARD;
        NetSock_Discard(psock);                                 /* ... discard sock (see Note #3a).                     */
       *perr = err_discard;                                     /* See Note #3c.                                        */
        return;
    }


   *perr = NET_SOCK_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetSock_FreeAddr()
*
* Description : Free socket address.
*
*               (1) Since connection address information is required to free the socket address,
*                   the socket address MUST be freed PRIOR to closing the network connection.
*
*
* Argument(s) : psock       Pointer to a socket.
*               -----       Argument validated in NetSock_FreeHandler(),
*                                                 NetSock_BindHandler().
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler(),
*               NetSock_BindHandler().
*
* Note(s)     : (2) The 'NET_SOCK_CFG_FAMILY' pre-processor 'else'-conditional code will never be
*                   compiled/linked since 'net_sock.h' ensures that the family type configuration
*                   constant (NET_SOCK_CFG_FAMILY) is configured with an appropriate family type
*                   value (see 'net_sock.h  CONFIGURATION ERRORS').  The 'else'-conditional code
*                   is included for completeness & as an extra precaution in case 'net_sock.h' is
*                   incorrectly modified.
*********************************************************************************************************
*/

static  void  NetSock_FreeAddr (NET_SOCK  *psock)
{
    NET_CONN_ID        conn_id;
    CPU_INT08U         addr[NET_SOCK_CFG_ADDR_LEN];
    NET_SOCK_ADDR_LEN  addr_len;
    NET_PORT_NBR       port_nbr;
    NET_ERR            err;


    switch (psock->State) {
        case NET_SOCK_STATE_FREE:
        case NET_SOCK_STATE_DISCARD:
        case NET_SOCK_STATE_CLOSED_FAULT:
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
             

        case NET_SOCK_STATE_NONE:
        case NET_SOCK_STATE_CLOSED:
        case NET_SOCK_STATE_CLOSE_IN_PROGRESS:
        case NET_SOCK_STATE_CLOSING_DATA_AVAIL:
        case NET_SOCK_STATE_BOUND:
        case NET_SOCK_STATE_LISTEN:
        case NET_SOCK_STATE_CONN:
        case NET_SOCK_STATE_CONN_IN_PROGRESS:
        case NET_SOCK_STATE_CONN_DONE:
        default:
             break;
    }


    conn_id  = psock->ID_Conn;
    addr_len = sizeof(addr);
    NetConn_AddrLocalGet((NET_CONN_ID        ) conn_id,
                         (CPU_INT08U        *)&addr[0],
                         (NET_CONN_ADDR_LEN *)&addr_len,
                         (NET_ERR           *)&err);
    if (err != NET_CONN_ERR_NONE) {
        return;
    }
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (addr_len != NET_SOCK_CFG_ADDR_LEN) {
        return;
    }
#endif


#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    Mem_Copy((void     *)&port_nbr,
             (void     *)&addr[NET_SOCK_ADDR_IP_IX_PORT],
             (CPU_SIZE_T) sizeof(port_nbr));

    port_nbr = NET_UTIL_NET_TO_HOST_16(port_nbr);

#else                                                           /* See Note #2.                                         */
   (void)&port_nbr;                                             /* Prevent compiler warning.                            */
    return;
#endif


    NetSock_RandomPortNbrFree(port_nbr, &err);                  /* Free port nbr to random port nbr Q (if necessary).   */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetSock_FreeBufQ()
*
* Description : Free a socket's buffer queue.
*
* Argument(s) : pbuf_q_head     Pointer to a socket buffer queue's head pointer.
*
*               pbuf_q_tail     Pointer to a socket buffer queue's tail pointer.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetSock_FreeBufQ (NET_BUF  **pbuf_q_head,
                                NET_BUF  **pbuf_q_tail)
{
    NET_BUF  *pbuf_q;

                                                                /* Free buf Q.                                          */
    pbuf_q = *pbuf_q_head;
    NetBuf_FreeBufQ_PrimList((NET_BUF *)pbuf_q,
                             (NET_CTR *)0);
                                                                /* Clr  buf Q ptrs to NULL.                             */
   *pbuf_q_head = (NET_BUF *)0;
   *pbuf_q_tail = (NET_BUF *)0;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetSock_Clr()
*
* Description : Clear socket controls.
*
* Argument(s) : psock       Pointer to a socket.
*               -----       Argument validated in NetSock_Init(),
*                                                 NetSock_Get(),
*                                                 NetSock_Free().
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Init(),
*               NetSock_Get(),
*               NetSock_Free().
*
* Note(s)     : (1) See 'net_sock.h  NETWORK SOCKET DATA TYPE  Note #2'.
*********************************************************************************************************
*/

static  void  NetSock_Clr (NET_SOCK  *psock)
{
#if ((NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED) && \
     (NET_DBG_CFG_MEM_CLR_EN      == DEF_ENABLED))
    NET_CONN_ID      *pconn_id;
    NET_SOCK_Q_SIZE   i;
#endif
    NET_SOCK_ID       sock_id;
    NET_ERR           err;


    psock->NextPtr        = (NET_SOCK *)0;
    psock->RxQ_Head       = (NET_BUF  *)0;
    psock->RxQ_Tail       = (NET_BUF  *)0;
#if 0                                                           /* See Note #1.                                         */
    psock->TxQ_Head       = (NET_BUF  *)0;
    psock->TxQ_Tail       = (NET_BUF  *)0;
#endif
    psock->ID_Conn        =  NET_CONN_ID_NONE;
    psock->ProtocolFamily =  NET_SOCK_PROTOCOL_FAMILY_NONE;
    psock->Protocol       =  NET_SOCK_PROTOCOL_NONE;
    psock->SockType       =  NET_SOCK_TYPE_NONE;
    psock->State          =  NET_SOCK_STATE_FREE;
    psock->Flags          =  NET_SOCK_FLAG_NONE;

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                                                                /* ------------ CLR SOCK CONN ACCEPT Q VALS ----------- */
    psock->ConnAcceptQ_SizeMax = NET_SOCK_Q_SIZE_NONE;
    psock->ConnAcceptQ_SizeCur = 0;

    psock->ConnAcceptQ_HeadIx  = 0;
    psock->ConnAcceptQ_TailIx  = 0;

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    pconn_id = &psock->ConnAcceptQ[0];
    for (i = 0; i < NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX; i++) {
       *pconn_id = NET_CONN_ID_NONE;
        pconn_id++;
    }
#endif
#endif
                                                                /* ------------ CFG SOCK DFLT TIMEOUT VALS ------------ */
    sock_id = psock->ID;
    NetOS_Sock_RxQ_TimeoutDflt(sock_id, &err);

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
    NetOS_Sock_ConnReqTimeoutDflt(sock_id, &err);
    NetOS_Sock_ConnAcceptQ_TimeoutDflt(sock_id, &err);
    NetOS_Sock_ConnCloseTimeoutDflt(sock_id, &err);
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetSock_Copy()
*
* Description : Copy a socket.
*
* Argument(s) : psock_dest      Pointer to socket to receive socket copy.
*               ----------      Argument validated in NetSock_Accept().
*
*               psock_src       Pointer to socket to copy.
*               ---------       Argument validated in NetSock_Accept().
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Accept().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  void  NetSock_Copy (NET_SOCK  *psock_dest,
                            NET_SOCK  *psock_src)
{
    psock_dest->ProtocolFamily = psock_src->ProtocolFamily;
    psock_dest->Protocol       = psock_src->Protocol;
    psock_dest->SockType       = psock_src->SockType;

    psock_dest->ID_Conn        = psock_src->ID_Conn;

    psock_dest->State          = psock_src->State;
                  
    psock_dest->Flags          = psock_src->Flags;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetSock_Discard()
*
* Description : (1) Discard an invalid/corrupted socket :
*
*                   (a) Discard socket from available socket pool               See Note #2
*                   (b) Update  socket pool statistics
*
*               (2) Assumes socket is invalid/corrupt & MUST be removed.  Socket removed simply by NOT
*                   returning the socket back to the socket pool.
*
*
* Argument(s) : psock       Pointer to an invalid/corrupted socket.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Get(),
*               NetSock_FreeHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetSock_Discard (NET_SOCK  *psock)
{
    NET_ERR  stat_err;

                                                                /* ------------------- DISCARD SOCK ------------------- */
   (void)&psock;                                                /* Prevent compiler warning (see Note #2).              */

                                                                /* --------------- UPDATE DISCARD STATS --------------- */
    NetStat_PoolEntryLostInc(&NetSock_PoolStat, &stat_err);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_RandomPortNbrGet()
*
* Description : Get a random port number from the random port number queue.
*
*               (1) Random port number queue is a FIFO Q implemented as a circular ring array :
*
*                   (a) 'NetSock_RandomPortNbrQ_HeadIx' points to the next available port number.
*
*                   (b) 'NetSock_RandomPortNbrQ_TailIx' points to the next available queue entry
*                       to insert freed port numbers.
*
*                   (c) 'NetSock_RandomPortNbrQ_HeadIx'/'NetSock_RandomPortNbrQ_TailIx' advance :
*
*                       (1) By increment;
*                       (2) Reset to minimum index value when maximum index value reached.
*
*
*                                  Index to next available
*                                 port number in random port     Index to next available          
*                                       number queue            entry to free port number                          
*                                      (see Note #1a)                (see Note #1b)           
*
*                                             |                             |
*                                             |                             |
*                                             v                             v
*                              -------------------------------------------------------
*                              |     |     |     |     |     |     |     |     |     |
*                              |     |     |     |     |     |     |     |     |     |
*                              |     |     |     |     |     |     |     |     |     |
*                              -------------------------------------------------------
*
*                                                    ---------->
*                                               FIFO indices advance by
*                                              increment (see Note #1c1)
*
*                              |                                                     |
*                              |<-------------- Circular Ring FIFO Q --------------->|
*                              |                   (see Note #1)                     |
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Port number successfully retrieved.
*                               NET_SOCK_ERR_PORT_NBR_NONE_AVAIL    Port number NOT available.
*
* Return(s)   : Random port number,     if NO errors.
*
*               NET_SOCK_PORT_NBR_NONE, otherwise.
*
* Caller(s)   : NetSock_BindHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_PORT_NBR  NetSock_RandomPortNbrGet (NET_ERR  *perr)
{
    NET_PORT_NBR  port_nbr;


                                                                            /* ---------- VALIDATE NBR USED ----------- */
    if (NetSock_RandomPortNbrQ_NbrUsed >= NET_SOCK_PORT_NBR_RANDOM_NBR) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrRandomPortNbrNoneAvailCtr);
       *perr =  NET_SOCK_ERR_PORT_NBR_NONE_AVAIL;
        return (NET_SOCK_PORT_NBR_NONE);
    }

                                                                            /* - GET PORT NBR FROM RANDOM PORT NBR Q -- */
    port_nbr                                              = NetSock_RandomPortNbrQ[NetSock_RandomPortNbrQ_HeadIx];
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    NetSock_RandomPortNbrQ[NetSock_RandomPortNbrQ_HeadIx] = NET_SOCK_PORT_NBR_NONE;
#endif

    NetSock_RandomPortNbrQ_HeadIx++;
    if (NetSock_RandomPortNbrQ_HeadIx >= NET_SOCK_PORT_NBR_RANDOM_NBR) {    /* If head ix > max ix, ...                 */
        NetSock_RandomPortNbrQ_HeadIx  = 0;                                 /* ... reset to min ix (see Note #1c2).     */
    }

    NetSock_RandomPortNbrQ_NbrUsed++;


   *perr =  NET_SOCK_ERR_NONE;

    return (port_nbr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_RandomPortNbrFree()
*
* Description : (1) Free a port number back to the random port number queue :
*
*                   (a) Validate port number      in random port number range & NOT currently in queue
*                   (b) Free     port number back to random port number queue
*
*
* Argument(s) : port_nbr    Port number to verify.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                       Port number freed successfully.
*                               NET_SOCK_ERR_INVALID_PORT_NBR           Port number NOT in random port range.
*                               NET_SOCK_ERR_INVALID_PORT_Q_NBR_USED    Invalid number queue entries used.
*                               NET_SOCK_ERR_PORT_NBR_IN_Q              Port number already in random port
*                                                                            number queue.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeAddr().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetSock_RandomPortNbrFree (NET_PORT_NBR   port_nbr,
                                         NET_ERR       *perr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN  random_port_nbr_in_q;
#endif
    CPU_BOOLEAN  random_port_nbr_range;


                                                                            /* ---- VALIDATE RANDOM PORT NBR RANGE ---- */
    random_port_nbr_range = NetSock_RandomPortNbrChkRange(port_nbr);
    if (random_port_nbr_range != DEF_YES) {
       *perr = NET_SOCK_ERR_INVALID_PORT_NBR;
        return;
    }


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                            /* ---------- VALIDATE NBR USED ----------- */
    if (NetSock_RandomPortNbrQ_NbrUsed < 1) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrRandomPortNbrQ_UsedCtr);
       *perr = NET_SOCK_ERR_INVALID_PORT_Q_NBR_USED;
        return;
    }
                                                                            /* ------ VALIDATE PORT NBR NOT IN Q ------ */
    random_port_nbr_in_q = NetSock_RandomPortNbrSrch(port_nbr);
    if (random_port_nbr_in_q != DEF_NO) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetSock_ErrRandomPortNbrQ_NbrInQ_Ctr);
       *perr = NET_SOCK_ERR_PORT_NBR_IN_Q;
        return;
    }
#endif

                                                                            /* -- FREE PORT NBR TO RANDOM PORT NBR Q -- */
    NetSock_RandomPortNbrQ[NetSock_RandomPortNbrQ_TailIx] = port_nbr;

    NetSock_RandomPortNbrQ_TailIx++;
    if (NetSock_RandomPortNbrQ_TailIx >= NET_SOCK_PORT_NBR_RANDOM_NBR) {    /* If tail ix > max ix, ...                 */
        NetSock_RandomPortNbrQ_TailIx  = 0;                                 /* ... reset to min ix.                     */
    }

    NetSock_RandomPortNbrQ_NbrUsed--;


   *perr = NET_SOCK_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetSock_RandomPortNbrChkRange()
*
* Description : Check if port number within random port number range.
*
* Argument(s) : port_nbr    Port number to verify.
*
* Return(s)   : DEF_NO,  if port number NOT within random port number range.
*
*               DEF_YES, if port number     within random port number range.
*
* Caller(s)   : NetSock_IsValidAddrLocal(),
*               NetSock_RandomPortNbrFree().
*
* Note(s)     : (1) Random port numbers reserved for sockets that transmit before or without previously
*                   having binded to a local address.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  NetSock_RandomPortNbrChkRange (NET_PORT_NBR  port_nbr)
{
    CPU_BOOLEAN  random_port_nbr_range;


    random_port_nbr_range = DEF_YES;

    if (port_nbr < NET_SOCK_PORT_NBR_RANDOM_MIN) {
        random_port_nbr_range = DEF_NO;
    }
    if (port_nbr > NET_SOCK_PORT_NBR_RANDOM_MAX) {
        random_port_nbr_range = DEF_NO;
    }

    return (random_port_nbr_range);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetSock_RandomPortNbrSrch()
*
* Description : Search random port number queue for specific port number.
*
* Argument(s) : port_nbr    Port number to search.
*
* Return(s)   : DEF_NO,  if port number NOT in random port number queue.
*
*               DEF_YES, if port number     in random port number queue.
*
* Caller(s)   : NetSock_RandomPortNbrFree().
*
* Note(s)     : (1) Assumes queue indices valid.
*
*               (2) If ALL port numbers in queue searched & queue tail index NOT found, tail index MUST
*                   be invalid -- outside the range of table indices.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  CPU_BOOLEAN  NetSock_RandomPortNbrSrch (NET_PORT_NBR  port_nbr)
{
    CPU_BOOLEAN       found;
    CPU_BOOLEAN       done;
    NET_PORT_NBR      ix;
    NET_PORT_NBR_QTY  nbr;


    found = DEF_NO;
    done  = DEF_NO;
    ix    = NetSock_RandomPortNbrQ_HeadIx;                      /* See Note #1.                                         */
    nbr   = 0;

    while (done == DEF_NO) {                                    /* Srch random port nbr Q for port nbr.                 */
        if (ix  == NetSock_RandomPortNbrQ_TailIx) {             /* If ix @ tail, port nbr NOT found.                    */
            done = DEF_YES;

        } else {
            found = (NetSock_RandomPortNbrQ[ix] == port_nbr) ? DEF_YES : DEF_NO;

            if (found == DEF_YES) {                             /* If port nbr found, ...                               */
                done  =  DEF_YES;                               /* ... abort successful srch.                           */

            } else {
                nbr++;
                if (nbr >= NET_SOCK_PORT_NBR_RANDOM_NBR) {      /* If nbr srch'd > nbr of port nbr's, ...               */
                    done = DEF_YES;                             /* ... abort srch; port nbr Q corrupted (see Note #2).  */

                } else {
                    ix++;
                    if (ix >= NET_SOCK_PORT_NBR_RANDOM_NBR) {   /* If ix     >  max ix, ...                             */
                        ix  = 0;                                /* ... reset to min ix.                                 */
                    }
                }
            }
        }
    }

    return (found);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_sock.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of net sock module include (see Note #1).        */

