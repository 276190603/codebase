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
*                                 PACKET-BASED NETWORK INTERFACE LAYER
*
* Filename      : net_if_pkt.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports packet-based Network Interface Cards/Layers.
*
*                 (2) Packet-based Network Interface Layer also referred to as 'Packet Interface' & also
*                     abbreviated as network interface.
*
*                 (3) Network Interface modules located in the following network directories :
*
*                     (a) (1) \<Network Protocol Suite>\IF\net_if_*.*
*                         (2) \<Network Protocol Suite>\IF\<if>\net_if.*
*
*                             where
*                                     <Network Protocol Suite>      directory path for network protocol suite
*                                      net_if_*.*                   Generic  Network Interface Management
*                                                                       module files
*                                      net_if.*                     Specific Network Interface(s) module files
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_IF_PKT_MODULE
#include  <net.h>


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) See 'net_if_pkt.h  MODULE'.
*********************************************************************************************************
*/

#ifdef  NET_IF_PKT_MODULE_PRESENT


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

                                                                    /* ------------------- RX FNCTS ------------------- */
static  void  NetIF_Pkt_Rx              (void);

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
static  void  NetIF_Pkt_RxHandlerLoadBal(void);
#endif

static  void  NetIF_Pkt_RxPktDiscard    (NET_BUF      *pbuf);


                                                                    /* ------------------- TX FNCTS ------------------- */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetIF_Pkt_TxPktValidate   (NET_BUF      *pbuf,
                                         NET_BUF_HDR  *pbuf_hdr,
                                         NET_ERR      *perr);
#endif

static  void  NetIF_Pkt_TxPktFree       (NET_BUF      *pbuf);

static  void  NetIF_Pkt_TxPktDiscard    (NET_BUF      *pbuf,
                                         NET_ERR      *perr);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetIF_Pkt_Init()
*
* Description : (1) Initialize  Packet-based Network Interface Layer :
*
*                   (a) Perform Packet Interface/OS initialization
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                     Network interface layer successfully
*                                                                       initialized.
*
*                                                                   --- RETURNED BY NetOS_IF_Init() : ----
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
* Caller(s)   : NetIF_Init().
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetIF_Pkt_Init (NET_ERR  *perr)
{
                                                                /* -------------- PERFORM PKT IF/OS INIT -------------- */
    NetOS_IF_Init(perr);                                        /* Create pkt IF obj(s)/task(s).                        */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

   *perr = NET_IF_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIF_RxTaskHandlerInit()
*
* Description : Initialize Packet-based Network Interface Receive Task Handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_IF_RxTask() [see 'net_os.c'].
*
*               This function is a network protocol suite to operating system (OS) function & SHOULD be
*               called only by appropriate network-operating system port function(s).
*
* Note(s)     : (1) Network Interface Receive Task Handler functions named 'NetIF_' ONLY to allow NIC & OS port
*                   interfaces to be generic for either packet-based or character-based network interface.
*********************************************************************************************************
*/

void  NetIF_RxTaskHandlerInit (void)
{
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIF_RxTaskHandler()
*
* Description : (1) Handle received data packets from network interface card :
*
*                   (a) Wait for packet receive signal from Network Interface Card (NIC)
*                   (b) Acquire  network  lock                                              See Note #4
*                   (c) Handle   received packet
*                   (d) Handle   network  load balancing
*                   (e) Release  network  lock
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_IF_RxTask() [see 'net_os.c'].
*
*               This function is a network protocol suite to operating system (OS) function & SHOULD be
*               called only by appropriate network-operating system port function(s).
*
* Note(s)     : (2) Network Interface Receive Task Handler functions named 'NetIF_' ONLY to allow NIC & OS port
*                   interfaces to be generic for either packet-based or character-based network interface.
*
*               (3) NetIF_RxTaskHandler() blocked until network initialization completes.
*
*               (4) NetIF_RxTaskHandler() blocks ALL other network protocol tasks by pending on & acquiring
*                   the global network lock (see 'net.h  Note #2').
*********************************************************************************************************
*/

void  NetIF_RxTaskHandler (void)
{
    NET_ERR  err_rx;


    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, ...                            */
        NetOS_InitWait(&err_rx);                                /* ... wait on net init (see Note #3).                  */
        if (err_rx != NET_OS_ERR_NONE) {
            return;
        }
    }


    while (DEF_ON) {
                                                                /* -------------- WAIT FOR NIC RX SIGNAL -------------- */
        do {
            NetOS_IF_RxTaskWait(&err_rx);
        } while (err_rx != NET_IF_ERR_NONE);

                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
        NetOS_Lock(&err_rx);                                    /* See Note #4.                                         */
        if (err_rx != NET_OS_ERR_NONE) {
            return;
        }

                                                                /* ------------------ HANDLE RX PKT ------------------- */
        NetIF_Pkt_Rx();


#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)                        /* --------------- HANDLE NET LOAD BAL ---------------- */
        NetIF_Pkt_RxHandlerLoadBal();
#endif

                                                                /* ----------------- RELEASE NET LOCK ----------------- */
        NetOS_Unlock();
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetIF_Pkt_Tx()
*
* Description : (1) Transmit data packets from network interface layer to network interface card :
*
*                   (a) Check    NIC status
*                   (b) Prepare  NIC transmit packet(s)
*                   (c) Wait for NIC transmit ready signal
*                   (d) Transmit packet(s)
*                   (e) Free   transmit packet buffer(s)
*                   (f) Update transmit statistics
*
*
* Argument(s) : pbuf        Pointer to network buffer with data packet to transmit.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 Packet successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               - RETURNED BY NetIF_Pkt_TxPktDiscard() : -
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_TxPkt().
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface function(s).
*
* Note(s)     : (2) NetIF_Pkt_Tx() blocked until network initialization completes.
*********************************************************************************************************
*/

void  NetIF_Pkt_Tx (NET_BUF  *pbuf,
                    NET_ERR  *perr)
{
    NET_BUF_HDR   *pbuf_hdr;
    NET_BUF_SIZE   size;
    CPU_INT08U    *p_data;
    CPU_BOOLEAN    conn_status;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit tx (see Note #2).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NetIF_Pkt_TxPktDiscard(pbuf, perr);
        return;
    }
#endif


                                                                /* ------------------ VALIDATE TX PKT ----------------- */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NetIF_Pkt_TxPktValidate(pbuf, pbuf_hdr, perr);
    switch (*perr) {
        case NET_IF_ERR_NONE:
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        default:
             NetIF_Pkt_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif


/*$PAGE*/
                                                                /* ------------------ CHK NIC STATUS ------------------ */
    conn_status = NetNIC_ConnStatusGet();
    switch (conn_status) {
        case DEF_ON:
             break;


        case DEF_OFF:
        default:
             NetIF_Pkt_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ---------------- PREPARE NIC TX PKT ---------------- */
    p_data = &pbuf->Data[pbuf_hdr->IF_HdrIx];
    size   =  pbuf_hdr->TotLen;

#if (NET_NIC_CFG_TX_PKT_PREPARE_EN == DEF_ENABLED)
    NetNIC_TxPktPrepare((void     *)p_data,
                        (CPU_INT16U)size,
                        (NET_ERR  *)perr);
    switch (*perr) {
        case NET_NIC_ERR_NONE:
             break;


        default:
             NetIF_Pkt_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif


                                                                /* ----- WAIT FOR NIC TX RDY SIGNAL & TX NIC PKT ------ */
    //NetOS_NIC_TxRdyWait(perr);    
    *perr = NET_NIC_ERR_NONE;
    
    switch (*perr) {
        case NET_NIC_ERR_NONE:
             NetNIC_TxPkt((void     *)p_data,
                          (CPU_INT16U)size,
                          (NET_ERR  *)perr);
             break;


        case NET_NIC_ERR_TX_RDY_SIGNAL_TIMEOUT:
        default:
             NetIF_Pkt_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ---------- FREE TX PKT / UPDATE TX STATS ----------- */
    switch (*perr) {                                            /* Chk err from NetNIC_TxPkt().                         */
        case NET_NIC_ERR_NONE:
             NetIF_Pkt_TxPktFree(pbuf);
             NET_CTR_STAT_INC(Net_StatCtrs.NetIF_Pkt_StatTxPktCtr);
            *perr = NET_IF_ERR_NONE;
             break;


        default:
             NetIF_Pkt_TxPktDiscard(pbuf, perr);
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
*                                           NetIF_Pkt_Rx()
*
* Description : (1) Receive data packets from network interface card & demultiplex to network interface layer :
*
*                   (a) Validate receive packet size
*                   (b) Get network buffer of sufficient size to receive packet
*                   (c) Read receive packet from NIC
*                   (d) Demultiplex to network interface layer
*                   (e) Update receive statistics
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
* Note(s)     : (2) When network buffer is demultiplexed to network IF receive, the buffer's reference  
*                   counter is NOT incremented since the packet interface layer does NOT maintain a 
*                   reference to the buffer.
*
*               (3) Network buffer already freed by higher layer; only increment error counter.
*********************************************************************************************************
*/

static  void  NetIF_Pkt_Rx (void)
{
    NET_ERR       err_rx;
    CPU_BOOLEAN   size_valid;
    CPU_INT16U    size;
    CPU_INT16U    ix_rx;
    NET_BUF      *pbuf;
    NET_BUF_HDR  *pbuf_hdr;
    CPU_INT08U   *p_data;



    NET_CTR_STAT_INC(Net_StatCtrs.NetIF_Pkt_StatRxPktCtr);


                                                                /* --------------- VALIDATE RX PKT SIZE --------------- */
    size       = NetNIC_RxPktGetSize();                         /* Get pkt size.                                        */

    size_valid = NetIF_IsValidPktSize(size);
    if (size_valid != DEF_YES) {
        NetNIC_RxPktDiscard(size, &err_rx);                     /* If size err, rd data from NIC & discard.             */
        NetIF_Pkt_RxPktDiscard((NET_BUF *)0);
        return;
    }


                                                                /* ----------------- GET BUF / RX PKT ----------------- */
    ix_rx = NET_IF_RX_IX;
    pbuf  = NetBuf_Get((NET_BUF_SIZE) size,                     /* Get buf large enough for pkt.                        */
                       (NET_BUF_SIZE) ix_rx,
                       (CPU_INT16U  ) NET_BUF_FLAG_NONE,
                       (NET_ERR    *)&err_rx);
    switch (err_rx) {
        case NET_BUF_ERR_NONE:                                  /* If NO errs, buf avail;    ...                        */
             p_data = &pbuf->Data[ix_rx];                       /* ... set appropriate rx ix ...                        */
             NetNIC_RxPkt((void *)p_data, size, &err_rx);       /* ...       & rd data from NIC into buf.               */
             break;


        case NET_BUF_ERR_NONE_AVAIL:
        case NET_BUF_ERR_INVALID_TYPE:
        case NET_BUF_ERR_INVALID_SIZE:
        default:
             NetNIC_RxPktDiscard(size, &err_rx);                /* If errs,    rd data from NIC & discard.              */
             NetIF_Pkt_RxPktDiscard(pbuf);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


/*$PAGE*/
                                                                /* ------------------- DEMUX RX PKT ------------------- */
    switch (err_rx) {                                           /* Chk err from NetNIC_RxPkt().                         */
        case NET_NIC_ERR_NONE:
             pbuf_hdr                  = &pbuf->Hdr;
             pbuf_hdr->TotLen          = (NET_BUF_SIZE)size;    /* Set pkt size as buf tot len & data len.              */
             pbuf_hdr->DataLen         = (NET_BUF_SIZE)pbuf_hdr->TotLen;
             pbuf_hdr->IF_HdrIx        =  ix_rx;
             pbuf_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_FRAME;
             DEF_BIT_SET(pbuf_hdr->Flags, NET_BUF_FLAG_RX_REMOTE);
                                                                /* See Note #2.                                         */
             NetIF_Rx(pbuf, &err_rx);
             break;


        case NET_ERR_INIT_INCOMPLETE:
        default:
             NetIF_Pkt_RxPktDiscard(pbuf);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ----------------- UPDATE RX STATS ------------------ */
    switch (err_rx) {                                           /* Chk err from NetIF_Rx().                             */
        case NET_IF_ERR_NONE:
             NET_CTR_STAT_INC(Net_StatCtrs.NetIF_Pkt_StatRxPktProcessedCtr);
             break;


        case NET_ERR_RX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_Pkt_ErrRxPktDiscardedCtr);   /* See Note #3.                             */
             break;


        case NET_ERR_INIT_INCOMPLETE:
        default:
             NetIF_Pkt_RxPktDiscard(pbuf);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetIF_Pkt_RxHandlerLoadBal()
*
* Description : Handle network receive versus transmit load balancing.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
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
*                       (2) Certain network connections MUST periodically suspend network transmit(s) 
*                           to handle network receive packet(s) :
*
*                           (A) Suspend network transmit(s) if network receive packets are available.
*
*                           (B) Signal or timeout network transmit suspend(s) to restart transmit(s).
*
*                   See also 'net.h  NETWORK RECEIVE PACKET MACRO'S  Note #1'.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
static  void  NetIF_Pkt_RxHandlerLoadBal (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


    NET_RX_PKT_DEC();                                           /* Dec nbr q'd rx pkts avail (see Note #1b1B).          */

    Net_TxSuspendSignal();                                      /* Signal net tx suspend     (see Note #1b2B).          */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIF_Pkt_RxPktDiscard()
*
* Description : On any Packet-based Network Interface Receive errors, discard packet & buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIF_Pkt_RxPktDiscard (NET_BUF  *pbuf)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetIF_Pkt_ErrRxPktDiscardedCtr;
#else
    pctr = (NET_CTR *) 0;
#endif
    NetBuf_FreeBuf((NET_BUF *)pbuf,
                   (NET_CTR *)pctr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIF_Pkt_TxPktValidate()
*
* Description : (1) Validate network interface transmit packet parameters :
*
*                   (a) Validate the following transmit packet parameters :
*
*                       (1) Supported protocols :
*                           (A) Frame
*
*
* Argument(s) : pbuf        Pointer to network buffer to transmit network interface packet.
*               ----        Argument checked   in NetIF_Pkt_Tx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetIF_Pkt_Tx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 Transmit packet validated.
*                               NET_ERR_INVALID_PROTOCOL        Invalid/unknown protocol type.
*                               NET_BUF_ERR_INVALID_IX          Invalid or insufficient buffer index.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetIF_Pkt_TxPktValidate (NET_BUF      *pbuf,
                                       NET_BUF_HDR  *pbuf_hdr,
                                       NET_ERR      *perr)
{
    CPU_INT16U  ix;

                                                                /* ----------------- VALIDATE PROTOCOL ---------------- */
    switch (pbuf_hdr->ProtocolHdrType) {
        case NET_PROTOCOL_TYPE_FRAME:
             ix = pbuf_hdr->IF_HdrIx;
             break;


        case NET_PROTOCOL_TYPE_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_Pkt_ErrTxProtocolCtr);
            *perr = NET_ERR_INVALID_PROTOCOL;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

    if (ix == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_Pkt_ErrTxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }


   *perr = NET_IF_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIF_Pkt_TxPktFree()
*
* Description : Free network buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIF_Pkt_TxPktFree (NET_BUF  *pbuf)
{
    NetBuf_FreeBuf((NET_BUF *)pbuf,
                   (NET_CTR *)0);
}


/*
*********************************************************************************************************
*                                        NetIF_Pkt_TxPktDiscard()
*
* Description : On any Transmit errors, discard packet & buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIF_Pkt_TxPktDiscard (NET_BUF  *pbuf,
                                      NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetIF_Pkt_ErrTxPktDiscardedCtr;
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
*                                             MODULE END
*
* Note(s) : (1) See 'net_if_pkt.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of pkt-based net IF module include (see Note #1).*/

