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
*                                       NETWORK INTERFACE LAYER
*
*                                              ETHERNET
*
* Filename      : net_if.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports Ethernet as described in RFC #894; supports IEEE 802 as described in RFC #1042.
*
*                 (2) Ethernet implementation conforms to RFC #1122, Section 2.3.3, bullets (a) & (b), but
*                     does NOT implement bullet (c) :
*
*                     RFC #1122                  LINK LAYER                  October 1989
*
*                     2.3.3  ETHERNET (RFC-894) and IEEE 802 (RFC-1042) ENCAPSULATION
*
*                            Every Internet host connected to a 10Mbps Ethernet cable :
*
*                            (a) MUST be able to send and receive packets using RFC-894 encapsulation;
*
*                            (b) SHOULD be able to receive RFC-1042 packets, intermixed with RFC-894 packets; and
*
*                            (c) MAY be able to send packets using RFC-1042 encapsulation.
*
*                 (3) REQUIREs the following network protocol files in network directories :
*
*                     (a) Packet-based Network Interface Layer located in the following network directory :
*
*                             \<Network Protocol Suite>\IF\
*
*                     (b) Address Resolution Protocol    Layer located in the following network directory :
*
*                             \<Network Protocol Suite>\
*
*                         See also 'net_arp.h  Note #1'.
*
*                              where
*                                      <Network Protocol Suite>    directory path for network protocol suite
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_IF_MODULE
#include  <net.h>


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) See this 'net_if.h  MODULE'.
*********************************************************************************************************
*/

#ifdef  NET_IF_ETHER_MODULE_PRESENT


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

                                                                    /* ----------------- ETHER ADDRs ------------------ */
static  const  CPU_INT08U  NetIF_AddrNull[NET_IF_ADDR_SIZE] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

static  const  CPU_INT08U  NetIF_AddrBroadcast[NET_IF_ADDR_SIZE] = {
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF
};


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

                                                                    /* ------------------- RX FCNTS ------------------- */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetIF_RxPktValidateBuf      (NET_BUF_HDR  *pbuf_hdr,
                                           NET_ERR      *perr);
#endif

static  void  NetIF_RxPktFrameDemux       (NET_BUF      *pbuf,
                                           NET_BUF_HDR  *pbuf_hdr,
                                           NET_IF_HDR   *pif_hdr,
                                           NET_ERR      *perr);

static  void  NetIF_RxPktFrameDemuxEther  (NET_BUF_HDR  *pbuf_hdr,
                                           NET_IF_HDR   *pif_hdr,
                                           NET_ERR      *perr);

static  void  NetIF_RxPktFrameDemuxIEEE802(NET_BUF_HDR  *pbuf_hdr,
                                           NET_IF_HDR   *pif_hdr,
                                           NET_ERR      *perr);


static  void  NetIF_RxPktDiscard          (NET_BUF      *pbuf,
                                           NET_ERR      *perr);


                                                                    /* ------------------- TX FNCTS ------------------- */

static  void  NetIF_TxPktHandler          (NET_BUF      *pbuf_q);


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetIF_TxPktValidate         (NET_BUF      *pbuf,
                                           NET_BUF_HDR  *pbuf_hdr,
                                           NET_ERR      *perr);
#endif

static  void  NetIF_TxPktPrepareFrame     (NET_BUF      *pbuf,
                                           NET_BUF_HDR  *pbuf_hdr,
                                           NET_ERR      *perr);


static  void  NetIF_TxPktDiscard          (NET_BUF      *pbuf,
                                           NET_ERR      *perr);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetIF_Init()
*
* Description : (1) Initialize Ethernet Network Interface Layer :
*
*                   (a) Initialize Packet-based Network Interface Layer
*                         (see this 'net_if_.c  Note #3a')
*
*                   (b) Initialize Address Resolution Protocol    Layer
*                         (see this 'net_if_.c  Note #3b')
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                     Ethernet network interface module
*                                                                       successfully initialized.
*
*                                                                   --- RETURNED BY NetIF_Pkt_Init() : ---
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
* Caller(s)   : Net_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) MAC address MUST be initialized & validated in NIC driver initialization.
*********************************************************************************************************
*/

void  NetIF_Init (NET_ERR  *perr)
{
                                                                /* ---------------- INIT PKT IF MODULE ---------------- */
    NetIF_Pkt_Init(perr);
    if (*perr != NET_IF_ERR_NONE) {
         return;
    }

                                                                /* ----------------- INIT ARP MODULE ------------------ */
    NetARP_Init();


   *perr = NET_IF_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetIF_MAC_AddrGet()
*
* Description : Get NIC's hardware (MAC) address.
*
* Argument(s) : paddr       Pointer to memory buffer to receive NIC's hardware address (see Note #2).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 NIC's hardware address successfully returned.
*                               NET_IF_ERR_INVALID_MAC          Invalid or uninitialized hardware address.
*
* Return(s)   : none.
*
* Caller(s)   : Application,
*               various NetNIC_Init() functions.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) NIC's hardware address also known as MAC (Media Access Control) or physical address.
*
*               (2) (a) The size of the memory buffer that will receive the return NIC hardware address 
*                       MUST be greater than or equal to NET_IF_ADDR_SIZE.
*
*                   (b) NIC's hardware address memory buffer array accessed by octets.
*
*                   (c) NIC's hardware address memory buffer array cleared in case of any error(s).
*********************************************************************************************************
*/

void  NetIF_MAC_AddrGet (CPU_INT08U  *paddr,
                         NET_ERR     *perr)
{
    if (NetIF_MAC_AddrValid != DEF_YES) {
        Mem_Clr((void     *)paddr,
                (CPU_SIZE_T)NET_IF_ADDR_SIZE);
       *perr = NET_IF_ERR_INVALID_MAC;
        return;
    }

    Mem_Copy((void     *) paddr,
             (void     *)&NetIF_MAC_Addr[0],
             (CPU_SIZE_T) NET_IF_ADDR_SIZE);

   *perr = NET_IF_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetIF_MAC_AddrSet()
*
* Description : Set NIC's hardware (MAC) address.
*
* Argument(s) : paddr       Pointer to a memory buffer that contains the NIC's hardware address (see Note #2).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 NIC's hardware address successfully configured.
*                               NET_IF_ERR_NULL_PTR             Argument 'paddr' passed a NULL pointer.
*
* Return(s)   : none.
*
* Caller(s)   : Application,
*               various NetNIC_Init() functions.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) NIC's hardware address also known as MAC (Media Access Control) or physical address.
*
*               (2) (a) The size of the memory buffer that contains the NIC hardware address MUST be greater
*                       than or equal to NET_IF_ADDR_SIZE.
*
*                   (b) NIC's hardware address memory buffer array accessed by octets.
*********************************************************************************************************
*/

void  NetIF_MAC_AddrSet (CPU_INT08U  *paddr,
                         NET_ERR     *perr)
{
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (paddr == (CPU_INT08U *)0) {
       *perr = NET_IF_ERR_NULL_PTR;
        return;
    }
#endif

    Mem_Copy((void     *)&NetIF_MAC_Addr[0],
             (void     *) paddr,
             (CPU_SIZE_T) NET_IF_ADDR_SIZE);

    NetIF_MAC_AddrValid = DEF_YES;

   *perr                = NET_IF_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIF_IsValidPktSize()
*
* Description : Validate an Ethernet packet size.
*
* Argument(s) : size        Number of Ethernet packet frame octets.
*
* Return(s)   : DEF_YES, if NIC packet size valid.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetIF_Pkt_Rx().
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIF_IsValidPktSize (CPU_INT16U  size)
{
    CPU_BOOLEAN  valid;


    valid = DEF_YES;

    if (size  < NET_IF_FRAME_MIN_SIZE) {
        valid = DEF_NO;
    }

    if (size  > NET_IF_FRAME_MAX_CRC_SIZE) {
        valid = DEF_NO;
    }

    return (valid);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIF_IsValidAddrSrc()
*
* Description : (1) Validate an Ethernet hardware address which MUST NOT be one of the following :
*
*                   (a) Ethernet broadcast address              See RFC #894, Section 'Address Mappings :
*                                                                   Broadcast Address'
*
*
* Argument(s) : paddr_src   Pointer to an Ethernet hardware address.
*
* Return(s)   : DEF_YES, if Ethernet hardware address valid.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetIF_RxPktFrameDemux(),
*               NetARP_IsValidAddrHW().
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetIF_IsValidAddrSrc (CPU_INT08U  *paddr_src)
{
    CPU_BOOLEAN  addr_null;
    CPU_BOOLEAN  addr_broadcast;
    CPU_BOOLEAN  addr_valid;

                                                                    /* ----------------- VALIDATE PTR ----------------- */
    if (paddr_src == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrNullPtrCtr);
        return (DEF_NO);
    }

                                                                /* -------------- VALIDATE ETHER HW ADDR -------------- */
    addr_null      = Mem_Cmp((void     *) paddr_src,
                             (void     *)&NetIF_AddrNull[0],
                             (CPU_SIZE_T) NET_IF_ADDR_SIZE);
    addr_broadcast = Mem_Cmp((void     *) paddr_src,
                             (void     *)&NetIF_AddrBroadcast[0],
                             (CPU_SIZE_T) NET_IF_ADDR_SIZE);

    addr_valid = ((addr_null      == DEF_YES) ||
                  (addr_broadcast == DEF_YES)) ? DEF_NO : DEF_YES;

    return (addr_valid);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             NetIF_Rx()
*
* Description : (1) Process received data packets from packet-based network interface layer & forward to
*                   network protocol layers :
*
*                   (a) Validate & demultiplex packet to higher-layer protocols
*                   (b) Update receive statistics
*
*
* Argument(s) : pbuf        Pointer to network buffer that received IF packet.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 IF packet successfully received & processed.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               ---- RETURNED BY NetIF_RxPktDiscard() : ----
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Rx() [see Note #2].
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface function(s).
*
* Note(s)     : (2) Network Interface Receive Handler implemented in Packet-based Network Interface
*                   [see '\<Network Protocol Suite>\IF\net_if_pkt.c  NetIF_Pkt_Rx()'].
*
*               (3) NetIF_Rx() blocked until network initialization completes.
*
*               (4) Network buffer already freed by higher layer; only increment error counter.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetIF_Rx (NET_BUF  *pbuf,
                NET_ERR  *perr)
{
    NET_BUF_HDR  *pbuf_hdr;
    NET_IF_HDR   *pif_hdr;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit rx (see Note #3).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NetIF_RxPktDiscard(pbuf, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrNullPtrCtr);
        return;
    }
#endif


    NET_CTR_STAT_INC(Net_StatCtrs.NetIF_StatRxPktCtr);


                                                                /* ------------- VALIDATE / DEMUX IF PKT -------------- */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NetIF_RxPktValidateBuf(pbuf_hdr, perr);                     /* Validate rx'd buf.                                   */
    switch (*perr) {
        case NET_IF_ERR_NONE:
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        default:
             NetIF_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif
    pif_hdr = (NET_IF_HDR *)&pbuf->Data[pbuf_hdr->IF_HdrIx];
    NetIF_RxPktFrameDemux(pbuf, pbuf_hdr, pif_hdr, perr);       /* Validate & demux pkt to appropriate net Q.           */


                                                                /* ----------------- UPDATE RX STATS ------------------ */
    switch (*perr) {
        case NET_ARP_ERR_NONE:
        case NET_IP_ERR_NONE:
             NET_CTR_STAT_INC(Net_StatCtrs.NetIF_StatRxPktProcessedCtr);
            *perr = NET_IF_ERR_NONE;
             break;


        case NET_ERR_RX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxPktDiscardedCtr);   /* See Note #4.                                 */
             break;


        case NET_ERR_INIT_INCOMPLETE:
        case NET_ERR_INVALID_PROTOCOL:
        case NET_IF_ERR_INVALID_ADDR_DEST:
        case NET_IF_ERR_INVALID_ADDR_SRC:
        case NET_IF_ERR_INVALID_ETHER_TYPE:
        case NET_IF_ERR_INVALID_LEN_FRAME:
        case NET_IF_ERR_INVALID_LLC_DSAP:
        case NET_IF_ERR_INVALID_LLC_SSAP:
        case NET_IF_ERR_INVALID_LLC_CTRL:
        case NET_IF_ERR_INVALID_SNAP_CODE:
        case NET_IF_ERR_INVALID_SNAP_TYPE:
        default:
             NetIF_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             NetIF_Tx()
*
* Description : (1) Prepare & transmit data packets from network protocol layers to packet-based network
*                   interface layer :
*
*                   (a) Prepare data packets with appropriate Ethernet frame format
*                   (b) Transmit packet via :
*                       (1) Broadcast
*                       (2) Bind hardware address
*
*
* Argument(s) : pbuf        Pointer to network buffer with data packet to transmit.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 Packet successfully transmitted  OR
*                                                                   buffered for later transmission.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               - RETURNED BY NetIF_TxPktDiscard() : -
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_TxPktDatagram(),
*               NetARP_Tx().
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface function(s).
*
* Note(s)     : (2) NetIF_Tx() blocked until network initialization completes.
*********************************************************************************************************
*/

void  NetIF_Tx (NET_BUF  *pbuf,
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
        NetIF_TxPktDiscard(pbuf, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrNullPtrCtr);
        return;
    }
#endif


                                                                /* ----------------- VALIDATE IF PKT ------------------ */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NetIF_TxPktValidate(pbuf, pbuf_hdr, perr);
    switch (*perr) {
        case NET_IF_ERR_NONE:
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        case NET_IF_ERR_INVALID_LEN_DATA:
        default:
             NetIF_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif


/*$PAGE*/
                                                                /* ----------------- PREPARE TX FRAME ----------------- */
    NetIF_TxPktPrepareFrame(pbuf, pbuf_hdr, perr);              /* Prepare Ethernet frame.                              */
    switch (*perr) {
        case NET_IF_ERR_TX_BROADCAST:
             break;


        case NET_IF_ERR_TX_ADDR_REQ:
             NetARP_CacheHandler(pbuf, perr);                   /* Bind dest hw addr & transmit data pkt.               */
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        case NET_BUF_ERR_INVALID_LEN:
        default:
             NetIF_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* -------------------- TX IF PKTS -------------------- */
    switch (*perr) {                                            /* Chk err from ...                                     */
        case NET_IF_ERR_TX_BROADCAST:                           /* ... NetIF_TxPktPrepareFrame().                       */
        case NET_ARP_ERR_CACHE_RESOLVED:                        /* ... NetARP_CacheHandler().                           */
             NetIF_TxPktHandler(pbuf);
            *perr = NET_IF_ERR_NONE;
             break;


        case NET_ARP_ERR_CACHE_PEND:
            *perr = NET_IF_ERR_NONE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_ARP_ERR_NULL_PTR:
        case NET_ARP_ERR_CACHE_NONE_AVAIL:
        case NET_ARP_ERR_CACHE_INVALID_TYPE:
        case NET_TMR_ERR_NULL_OBJ:
        case NET_TMR_ERR_NULL_FNCT:
        case NET_TMR_ERR_NONE_AVAIL:
        case NET_TMR_ERR_INVALID_TYPE:
        default:
             NetIF_TxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetIF_TxPkt()
*
* Description : (1) Transmit data packet to packet-based network interface layer :
*
*                   (a) Transmit data packet
*                   (b) Update transmit statistics
*
*
* Argument(s) : pbuf        Pointer to network buffer with data packet to transmit.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 Packet buffer(s) transmitted to driver.
*                               NET_ERR_TX                      Transmit error (see Note #2); packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_TxPktHandler(),
*               NetARP_CacheTxPktHandler().
*
*               This function is a network protocol suite to network interface (IF) function & SHOULD be 
*               called only by appropriate network interface function(s).
*
* Note(s)     : (2) Since the network interface layer & the packet interface layer return the same error
*                   codes, error code(s) do NOT need to be re-cast.
*
*               (3) Network buffer already freed by lower layer; only increment error counter.
*********************************************************************************************************
*/

void  NetIF_TxPkt (NET_BUF  *pbuf,
                   NET_ERR  *perr)
{

                                                                /* --------------------- TX PKT ----------------------- */
    NetIF_Pkt_Tx(pbuf, perr);                                   /* Tx pkt to pkt IF.                                    */


                                                                /* ----------------- UPDATE TX STATS ------------------ */
    switch (*perr) {                                            /* See Note #2.                                         */
        case NET_IF_ERR_NONE:
             NET_CTR_STAT_INC(Net_StatCtrs.NetIF_StatTxPktCtr);
             break;


        case NET_ERR_INIT_INCOMPLETE:
        case NET_ERR_TX:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrTxPktDiscardedCtr);   /* See Note #3.                                 */
            *perr = NET_ERR_TX;
             break;
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
*                                      NetIF_RxPktValidateBuf()
*
* Description : Validate received buffer header as IF protocol.
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header that received IF packet.
*               --------    Argument validated in NetIF_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 Received buffer's IF header validated.
*                               NET_ERR_INVALID_PROTOCOL        Buffer's protocol type is NOT IF.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetIF_RxPktValidateBuf (NET_BUF_HDR  *pbuf_hdr,
                                      NET_ERR      *perr)
{
                                                                /* ---------------- VALIDATE IP BUF HDR --------------- */
    if (pbuf_hdr->ProtocolHdrType != NET_PROTOCOL_TYPE_FRAME) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidProtocolCtr);
       *perr = NET_ERR_INVALID_PROTOCOL;
        return;
    }

    if (pbuf_hdr->IF_HdrIx == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

   *perr = NET_IF_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetIF_RxPktFrameDemux()
*
* Description : (1) Validate received packet frame & demultiplex to appropriate queue :
*
*                   (a) Validate destination address :
*                       (1) Check for broadcast address                 See RFC #1122, Section 2.4
*                       (2) Check for this host's hardware address
*                   (b) Validate source address                         See 'NetIF_IsValidAddrSrc()  Note #1'
*                   (c) Demultiplex & validate frame :
*                       (1) Ethernet   frame type 
*                       (2) IEEE 802.3 frame length
*                   (d) Demultiplex packet to appropriate queue :
*                       (1) IP  receive queue
*                       (2) ARP receive queue
*
*
* Argument(s) : pbuf        Pointer to network buffer that received data packet.
*               ----        Argument checked   in NetIF_Rx().
*
*               pbuf_hdr    Pointer to received packet frame's network buffer header.
*               --------    Argument validated in NetIF_Rx().
*
*               pif_hdr     Pointer to received packet frame's header.
*               -------     Argument validated in NetIF_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_INVALID_ADDR_DEST        Invalid destination address.
*                               NET_IF_ERR_INVALID_ADDR_SRC         Invalid source      address.
*                               NET_ERR_INVALID_PROTOCOL            Invalid network protocol.
*
*                                                                   -- RETURNED BY NetIF_RxPktFrameDemuxEther() : --
*                               NET_IF_ERR_INVALID_ETHER_TYPE       Invalid Ethernet frame type value.
*
*                                                                   - RETURNED BY NetIF_RxPktFrameDemuxIEEE802() : -
*                               NET_IF_ERR_INVALID_LEN_FRAME        Invalid IEEE 802.3 frame length.
*                               NET_IF_ERR_INVALID_LLC_DSAP         Invalid IEEE 802.2 LLC  DSAP    value.
*                               NET_IF_ERR_INVALID_LLC_SSAP         Invalid IEEE 802.2 LLC  SSAP    value.
*                               NET_IF_ERR_INVALID_LLC_CTRL         Invalid IEEE 802.2 LLC  Control value.
*                               NET_IF_ERR_INVALID_SNAP_CODE        Invalid IEEE 802.2 SNAP OUI     value.
*                               NET_IF_ERR_INVALID_SNAP_TYPE        Invalid IEEE 802.2 SNAP Type    value.
*
*                                                                   ----------- RETURNED BY NetARP_Rx() : ----------
*                               NET_ARP_ERR_NONE                    ARP message successfully demultiplexed.
*
*                                                                   ----------- RETURNED BY NetIP_Rx() : -----------
*                               NET_IP_ERR_NONE                     IP datagram successfully demultiplexed.
*
*
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_ERR_RX                          Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Rx().
*
* Note(s)     : (2) When network buffer is demultiplexed to higher-layer protocol receive, the buffer's reference 
*                   counter is NOT incremented since the network interface layer does NOT maintain a reference to
*                   the buffer.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetIF_RxPktFrameDemux (NET_BUF      *pbuf,
                                     NET_BUF_HDR  *pbuf_hdr,
                                     NET_IF_HDR   *pif_hdr,
                                     NET_ERR      *perr)
{
#if (NET_IF_CFG_ADDR_FLTR_EN == DEF_ENABLED)
    CPU_BOOLEAN  dest_this_host;
    CPU_BOOLEAN  valid;
#endif
    CPU_BOOLEAN  dest_broadcast;
    CPU_INT16U   frame_type_len;


                                                                /* ---------------- VALIDATE DEST ADDR ---------------- */
    dest_broadcast = Mem_Cmp((void     *)&pif_hdr->AddrDest[0],
                             (void     *)&NetIF_AddrBroadcast[0],
                             (CPU_SIZE_T) NET_IF_ADDR_SIZE);
    if (dest_broadcast == DEF_YES) {
        NET_CTR_STAT_INC(Net_StatCtrs.NetIF_StatRxPktBroadcastCtr);
        DEF_BIT_SET(pbuf_hdr->Flags, NET_BUF_FLAG_RX_BROADCAST);/* Flag rx'd broadcast pkt (see Note #1a1).             */

    } else {
#if (NET_IF_CFG_ADDR_FLTR_EN == DEF_ENABLED)
        dest_this_host = Mem_Cmp((void     *)&pif_hdr->AddrDest[0],
                                 (void     *)&NetIF_MAC_Addr[0],
                                 (CPU_SIZE_T) NET_IF_ADDR_SIZE);
        if (dest_this_host != DEF_YES) {                        /* Discard invalid dest addr (see Note #1a2).           */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidAddrDestCtr);
           *perr = NET_IF_ERR_INVALID_ADDR_DEST;
            return;
        }
#endif
    }


#if (NET_IF_CFG_ADDR_FLTR_EN == DEF_ENABLED)
                                                                /* ---------------- VALIDATE SRC  ADDR ---------------- */
    valid = NetIF_IsValidAddrSrc(&pif_hdr->AddrSrc[0]);

    if (valid != DEF_YES) {                                     /* Discard invalid src addr (see Note #1b).             */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidAddrSrcCtr);
       *perr = NET_IF_ERR_INVALID_ADDR_SRC;
        return;
    }
#endif


/*$PAGE*/
                                                                /* --------------- DEMUX/VALIDATE FRAME --------------- */
    frame_type_len = NET_UTIL_VAL_GET_NET_16(&pif_hdr->FrameType_Len);
    if (frame_type_len <= NET_IF_IEEE_802_FRAME_LEN_MAX) {
        NetIF_RxPktFrameDemuxIEEE802(pbuf_hdr, pif_hdr, perr);
    } else {
        NetIF_RxPktFrameDemuxEther(pbuf_hdr, pif_hdr, perr);
    }


                                                                /* -------------------- DEMUX PKT --------------------- */
    switch (*perr) {                                            /* See Note #2.                                         */
        case NET_IF_ERR_NONE:
             switch (pbuf_hdr->ProtocolHdrType) {               /* Demux buf to appropriate protocol.                   */
                 case NET_PROTOCOL_TYPE_ARP:
                      NetARP_Rx(pbuf, perr);
                      break;


                 case NET_PROTOCOL_TYPE_IP_V4:
                      NetIP_Rx(pbuf, perr);
                      break;


                 case NET_PROTOCOL_TYPE_NONE:
                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidProtocolCtr);
                     *perr = NET_ERR_INVALID_PROTOCOL;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_IF_ERR_INVALID_ETHER_TYPE:
        case NET_IF_ERR_INVALID_LEN_FRAME:
        case NET_IF_ERR_INVALID_LLC_DSAP:
        case NET_IF_ERR_INVALID_LLC_SSAP:
        case NET_IF_ERR_INVALID_LLC_CTRL:
        case NET_IF_ERR_INVALID_SNAP_CODE:
        case NET_IF_ERR_INVALID_SNAP_TYPE:
        default:
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetIF_RxPktFrameDemuxEther()
*
* Description : (1) Validate & demultiplex Ethernet packet frame :
*
*                   (a) Validate & demultiplex Ethernet packet frame
*                   (b) Update buffer controls
*
*
* Argument(s) : pbuf_hdr    Pointer to received packet frame's network buffer header.
*               --------    Argument validated in NetIF_RxPktFrameDemux().
*
*               pif_hdr     Pointer to received packet frame's header.
*               -------     Argument validated in NetIF_RxPktFrameDemux().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                     Valid Ethernet packet frame.
*                               NET_IF_ERR_INVALID_ETHER_TYPE       Invalid Ethernet frame type value.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxPktFrameDemux().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIF_RxPktFrameDemuxEther (NET_BUF_HDR  *pbuf_hdr,
                                          NET_IF_HDR   *pif_hdr,
                                          NET_ERR      *perr)
{
    NET_IF_HDR_ETHER  *pif_hdr_ether;
    CPU_INT16U         frame_type;
    CPU_INT16U         ix;


    pif_hdr_ether = (NET_IF_HDR_ETHER *)pif_hdr;

                                                                /* -------------- VALIDATE / DEMUX FRAME -------------- */
    frame_type    =  NET_UTIL_VAL_GET_NET_16(&pif_hdr_ether->FrameType);
    ix            =  NET_IF_RX_IX + NET_IF_FRAME_HDR_SIZE_ETHER;
    switch (frame_type) {                                       /* Validate & demux Ether frame type.                   */
        case NET_IF_ETHER_FRAME_TYPE_IP:
             pbuf_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_IP_V4;
             pbuf_hdr->IP_HdrIx        = (NET_BUF_SIZE)ix;
             break;


        case NET_IF_ETHER_FRAME_TYPE_ARP:
             pbuf_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_ARP;
             pbuf_hdr->ARP_MsgIx       = (NET_BUF_SIZE)ix;
             break;


        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidFrameCtr);
            *perr = NET_IF_ERR_INVALID_ETHER_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

                                                                /* ----------------- UPDATE BUF CTRLS ----------------- */
    pbuf_hdr->IF_HdrLen  =  NET_IF_FRAME_HDR_SIZE_ETHER;
    pbuf_hdr->DataLen   -= (NET_BUF_SIZE)pbuf_hdr->IF_HdrLen;


   *perr = NET_IF_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetIF_RxPktFrameDemuxIEEE802()
*
* Description : (1) Validate & demultiplex IEEE 802 packet frame :
*
*                   (a) Validate & demultiplex IEEE 802 packet frame
*                       (1) IEEE 802.2 LLC
*                       (2) IEEE 802.2 SNAP Organization Code
*                       (3) IEEE 802.2 SNAP Frame Type
*                   (b) Update buffer controls
*
*
* Argument(s) : pbuf_hdr    Pointer to received packet frame's network buffer header.
*               --------    Argument validated in NetIF_RxPktFrameDemux().
*
*               pif_hdr     Pointer to received packet frame's header.
*               -------     Argument validated in NetIF_RxPktFrameDemux().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                     Valid IEEE 802 packet frame.
*                               NET_IF_ERR_INVALID_LEN_FRAME        Invalid IEEE 802.3 frame length.
*                               NET_IF_ERR_INVALID_LLC_DSAP         Invalid IEEE 802.2 LLC  DSAP    value.
*                               NET_IF_ERR_INVALID_LLC_SSAP         Invalid IEEE 802.2 LLC  SSAP    value.
*                               NET_IF_ERR_INVALID_LLC_CTRL         Invalid IEEE 802.2 LLC  Control value.
*                               NET_IF_ERR_INVALID_SNAP_CODE        Invalid IEEE 802.2 SNAP OUI     value.
*                               NET_IF_ERR_INVALID_SNAP_TYPE        Invalid IEEE 802.2 SNAP Type    value.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxPktFrameDemux().
*
* Note(s)     : (2) The IEEE 802.3 Frame Length field specifies the number of frame data octets & does NOT
*                   include the trailing frame CRC field octets.  However, since some NICs MAY append the
*                   CRC field as part of a received packet frame, any validation of the minimum frame size
*                   MUST assume that the CRC field may be present.  Therefore, the minimum frame packet size
*                   for comparison MUST include the number of CRC field octets.
*********************************************************************************************************
*/

static  void  NetIF_RxPktFrameDemuxIEEE802 (NET_BUF_HDR  *pbuf_hdr,
                                            NET_IF_HDR   *pif_hdr,
                                            NET_ERR      *perr)
{
    NET_IF_HDR_IEEE_802  *pif_hdr_ieee_802;
    CPU_INT16U            frame_len;
    CPU_INT16U            frame_len_actual;
    CPU_INT16U            frame_type;
    CPU_INT16U            ix;


/*$PAGE*/
    pif_hdr_ieee_802 = (NET_IF_HDR_IEEE_802 *)pif_hdr;

                                                                    /* ------------- VALIDATE FRAME SIZE -------------- */
    if (pbuf_hdr->TotLen >=  NET_IF_FRAME_MIN_CRC_SIZE) {           /* If pkt size >= min frame pkt size (see Note #2)  */
        frame_len         =  NET_UTIL_VAL_GET_NET_16(&pif_hdr_ieee_802->FrameLen);
        frame_len_actual  = (CPU_INT16U)(pbuf_hdr->TotLen - NET_IF_FRAME_HDR_SIZE - NET_IF_FRAME_CRC_SIZE);
        if (frame_len != frame_len_actual) {                        /* ... & frame len != rem pkt len, rtn err.         */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidFrameCtr);
           *perr = NET_IF_ERR_INVALID_LEN_FRAME;
            return;
        }
    }

                                                                    /* ------------ VALIDATE IEEE 802.2 LLC ----------- */
    if (pif_hdr_ieee_802->LLC_DSAP != NET_IF_IEEE_802_LLC_DSAP) {   /* Validate IEEE 802.2 LLC DSAP.                    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidFrameCtr);
       *perr = NET_IF_ERR_INVALID_LLC_DSAP;
        return;
    }

    if (pif_hdr_ieee_802->LLC_SSAP != NET_IF_IEEE_802_LLC_SSAP) {   /* Validate IEEE 802.2 LLC SSAP.                    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidFrameCtr);
       *perr = NET_IF_ERR_INVALID_LLC_SSAP;
        return;
    }

    if (pif_hdr_ieee_802->LLC_Ctrl != NET_IF_IEEE_802_LLC_CTRL) {   /* Validate IEEE 802.2 LLC Ctrl.                    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidFrameCtr);
       *perr = NET_IF_ERR_INVALID_LLC_CTRL;
        return;
    }
                                                                    /* ----------- VALIDATE IEEE 802.2 SNAP ----------- */
                                                                    /* Validate IEEE 802.2 SNAP OUI.                    */
    if (pif_hdr_ieee_802->SNAP_OrgCode[0] != NET_IF_IEEE_802_SNAP_CODE_00) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidFrameCtr);
       *perr = NET_IF_ERR_INVALID_SNAP_CODE;
        return;
    }
    if (pif_hdr_ieee_802->SNAP_OrgCode[1] != NET_IF_IEEE_802_SNAP_CODE_01) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidFrameCtr);
       *perr = NET_IF_ERR_INVALID_SNAP_CODE;
        return;
    }
    if (pif_hdr_ieee_802->SNAP_OrgCode[2] != NET_IF_IEEE_802_SNAP_CODE_02) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidFrameCtr);
       *perr = NET_IF_ERR_INVALID_SNAP_CODE;
        return;
    }


    frame_type = NET_UTIL_VAL_GET_NET_16(&pif_hdr_ieee_802->SNAP_FrameType);
    ix         = NET_IF_RX_IX + NET_IF_FRAME_HDR_SIZE_IEEE_802;
    switch (frame_type) {                                           /* Validate & demux IEEE 802.2 SNAP frame type.     */
        case NET_IF_IEEE_802_SNAP_TYPE_IP:
             pbuf_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_IP_V4;
             pbuf_hdr->IP_HdrIx        = (NET_BUF_SIZE)ix;
             break;


        case NET_IF_IEEE_802_SNAP_TYPE_ARP:
             pbuf_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_ARP;
             pbuf_hdr->ARP_MsgIx       = (NET_BUF_SIZE)ix;
             break;


        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrRxInvalidFrameCtr);
            *perr = NET_IF_ERR_INVALID_SNAP_TYPE;
             return;                                                /* Prevent 'break NOT reachable' compiler warning.  */
    }

                                                                    /* --------------- UPDATE BUF CTRLS --------------- */
    pbuf_hdr->IF_HdrLen  =  NET_IF_FRAME_HDR_SIZE_IEEE_802;
    pbuf_hdr->DataLen   -= (NET_BUF_SIZE)pbuf_hdr->IF_HdrLen;


   *perr = NET_IF_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIF_RxPktDiscard()
*
* Description : On any Network Interface Receive errors, discard packet & buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIF_RxPktDiscard (NET_BUF  *pbuf,
                                  NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetIF_ErrRxPktDiscardedCtr;
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
*                                        NetIF_TxPktHandler()
*
* Description : Transmit data packets via network interface layer.
*
* Argument(s) : pbuf_q      Pointer to network buffer list with data packet(s) to transmit.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Tx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIF_TxPktHandler (NET_BUF  *pbuf_q)
{
    NET_BUF      *pbuf_list;
    NET_BUF      *pbuf_list_next;
    NET_BUF      *pbuf;
    NET_BUF      *pbuf_next;
    NET_BUF_HDR  *pbuf_hdr;
    NET_ERR       err;


    pbuf_list = pbuf_q;
    while (pbuf_list  != (NET_BUF *)0) {                        /* Tx ALL buf lists in Q.                               */
        pbuf_hdr       = &pbuf_list->Hdr;
        pbuf_list_next =  pbuf_hdr->NextSecListPtr;

        pbuf           = (NET_BUF *)pbuf_list;
        while (pbuf   != (NET_BUF *)0) {                        /* Tx ALL bufs in buf list.                             */
            pbuf_hdr   = &pbuf->Hdr;
            pbuf_next  =  pbuf_hdr->NextBufPtr;

            NetIF_TxPkt(pbuf, &err);                            /* Tx pkt to IF.                                        */

            pbuf  = pbuf_next;
        }

        pbuf_list = pbuf_list_next;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIF_TxPktValidate()
*
* Description : (1) Validate IF transmit packet parameters :
*
*                   (a) Validate the following transmit packet parameters :
*
*                       (1) Supported protocols :
*                           (A) ARP
*                           (B) IP
*
*                       (2) Total Length
*
*
* Argument(s) : pbuf        Pointer to network buffer to transmit IF packet.
*               ----        Argument checked   in NetIF_Tx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetIF_Tx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 Transmit packet validated.
*                               NET_ERR_INVALID_PROTOCOL        Invalid/unknown protocol type.
*                               NET_BUF_ERR_INVALID_IX          Invalid or insufficient buffer index.
*                               NET_IF_ERR_INVALID_LEN_DATA     Invalid protocol/data length.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Tx().
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetIF_TxPktValidate (NET_BUF      *pbuf,
                                   NET_BUF_HDR  *pbuf_hdr,
                                   NET_ERR      *perr)
{
    CPU_INT16U  ix;
    CPU_INT16U  len;


                                                                /* ----------------- VALIDATE PROTOCOL ---------------- */
    switch (pbuf_hdr->ProtocolHdrType) {
        case NET_PROTOCOL_TYPE_ARP:
             ix  = pbuf_hdr->ARP_MsgIx;
             len = pbuf_hdr->ARP_MsgLen;
             break;


        case NET_PROTOCOL_TYPE_IP_V4:
             ix  = pbuf_hdr->IP_HdrIx;
             len = pbuf_hdr->IP_TotLen;
             break;


        case NET_PROTOCOL_TYPE_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrTxProtocolCtr);
            *perr = NET_ERR_INVALID_PROTOCOL;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

    if (ix == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrTxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

    if (ix < NET_IF_HDR_SIZE_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrTxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }


                                                                /* -------------- VALIDATE TOT DATA LEN --------------- */
    if (len != pbuf_hdr->TotLen) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrTxHdrDataLenCtr);
       *perr = NET_IF_ERR_INVALID_LEN_DATA;
        return;
    }


   *perr = NET_IF_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetIF_TxPktPrepareFrame()
*
* Description : (1) Prepare data packet with Ethernet frame format :
*
*                   (a) Demultiplex Ethernet frame type
*                   (b) Update buffer controls
*                   (c) Write Ethernet values into packet frame
*                       (1) Ethernet destination broadcast address, if necessary
*                       (2) Ethernet source      MAC       address
*                       (3) Ethernet frame type
*                   (d) Clear Ethernet frame pad octets, if any
*
*
* Argument(s) : pbuf        Pointer to network buffer with data packet to encapsulate.
*               ----        Argument checked in NetIF_Tx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetIF_Tx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_TX_BROADCAST         Ethernet frame successfully prepared for
*                                                                   Ethernet broadcast on local network.
*                               NET_IF_ERR_TX_ADDR_REQ          Ethernet frame successfully prepared &
*                                                                   requires hardware address binding.
*                               NET_ERR_INVALID_PROTOCOL        Invalid network protocol.
*                               NET_BUF_ERR_INVALID_IX          Invalid or insufficient buffer index.
*                               NET_BUF_ERR_INVALID_LEN                    Insufficient buffer length.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Tx().
*
* Note(s)     : (2) Supports ONLY Ethernet frame format for network transmit (see 'net_if.c  Note #2a').
*
*               (3) Supports ONLY ARP & IP protocols (see 'net.h  Note #1a').
*
*               (4) To prepare the packet buffer for ARP resolution, the buffer's ARP protocol address
*                   pointer needs to be configured to the appropriate outbound address :
*
*                   (a) For ARP packets, the ARP layer will configure the ARP protocol address pointer
*                       (see 'net_arp.c  NetARP_TxPktPrepareHdr()  Note #1d').
*
*                   (b) For IP  packets, configure the ARP protocol address pointer to the IP's next-
*                       route address.
*
*               (5) RFC #894, Section 'Frame Format' states that :
*
*                   (a) "The minimum length of the data field of a packet sent over an Ethernet is 46
*                        octets."
*
*                   (b) (1) "If necessary, the data field should be padded (with octets of zero) to 
*                            meet the Ethernet minimum frame size."
*                       (2) "This padding is not part of the IP packet and is not included in the
*                            total length field of the IP header."
*********************************************************************************************************
*/

static  void  NetIF_TxPktPrepareFrame (NET_BUF      *pbuf,
                                       NET_BUF_HDR  *pbuf_hdr,
                                       NET_ERR      *perr)
{
    NET_IF_HDR_ETHER  *pif_hdr_ether;
    CPU_INT16U         protocol_ix;
    CPU_INT16U         frame_type;
    CPU_INT16U         clr_ix;
    CPU_INT16U         clr_len;
    CPU_INT16U         clr_size;
    CPU_BOOLEAN        clr_buf_mem;
    CPU_BOOLEAN        tx_broadcast;


/*$PAGE*/
                                                                /* ----------------- DEMUX FRAME TYPE ----------------- */
    switch (pbuf_hdr->ProtocolHdrType) {                        /* Demux protocol for frame type (see Note #3).         */
        case NET_PROTOCOL_TYPE_ARP:
             protocol_ix                   =  pbuf_hdr->ARP_MsgIx;
             frame_type                    =  NET_IF_ETHER_FRAME_TYPE_ARP;
             break;


        case NET_PROTOCOL_TYPE_IP_V4:
             protocol_ix                   =  pbuf_hdr->IP_HdrIx;
             frame_type                    =  NET_IF_ETHER_FRAME_TYPE_IP;
                                                                /* Cfg ARP addr ptr (see Note #4b).                     */
             pbuf_hdr->ARP_AddrProtocolPtr = (CPU_INT08U *)&pbuf_hdr->IP_AddrNextRouteNetOrder;
             break;


        case NET_PROTOCOL_TYPE_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrTxInvalidProtocolCtr);
            *perr = NET_ERR_INVALID_PROTOCOL;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                             /* ------------- VALIDATE IX -------------- */
    if (protocol_ix == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrTxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

    if (protocol_ix < NET_IF_HDR_SIZE_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrTxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }
#endif


                                                                            /* ----------- UPDATE BUF CTRLS ----------- */
    pbuf_hdr->IF_HdrLen        =  NET_IF_FRAME_HDR_SIZE_ETHER;
    pbuf_hdr->IF_HdrIx         =  protocol_ix - pbuf_hdr->IF_HdrLen;
    pbuf_hdr->TotLen          += (NET_BUF_SIZE) pbuf_hdr->IF_HdrLen;
    pbuf_hdr->ProtocolHdrType  =  NET_PROTOCOL_TYPE_FRAME;


                                                                            /* --------- PREPARE ETHER FRAME ---------- */
    pif_hdr_ether = (NET_IF_HDR_ETHER *)&pbuf->Data[pbuf_hdr->IF_HdrIx];

                                                                            /* --------- PREPARE FRAME ADDRS ---------- */
    tx_broadcast  =  DEF_BIT_IS_SET(pbuf_hdr->Flags, NET_BUF_FLAG_TX_BROADCAST);
    if (tx_broadcast == DEF_YES) {                                          /* If dest addr broadcast, ...              */
        Mem_Copy((void     *)&pif_hdr_ether->AddrDest[0],                   /* .. wr broadcast addr into frame.         */
                 (void     *)&NetIF_AddrBroadcast[0],
                 (CPU_SIZE_T) NET_IF_ADDR_SIZE);
        NET_CTR_STAT_INC(Net_StatCtrs.NetIF_StatTxPktBroadcastCtr);
       *perr                     =  NET_IF_ERR_TX_BROADCAST;

    } else {                                                                /* Else req ARP hw addr binding.            */
        pbuf_hdr->ARP_AddrHW_Ptr = (void *)&pif_hdr_ether->AddrDest[0];
       *perr                     =  NET_IF_ERR_TX_ADDR_REQ;
    }

    Mem_Copy((void     *)&pif_hdr_ether->AddrSrc[0],                        /* Wr src addr into frame.                  */
             (void     *)&NetIF_MAC_Addr[0],
             (CPU_SIZE_T) NET_IF_ADDR_SIZE);

                                                                            /* ---------- PREPARE FRAME TYPE ---------- */
    NET_UTIL_VAL_COPY_SET_NET_16(&pif_hdr_ether->FrameType, &frame_type);

                                                                            /* --------- CLR/PAD FRAME OCTETS --------- */
    if (pbuf_hdr->TotLen < NET_IF_FRAME_MIN_SIZE) {                         /* If tot len < min frame len (see Note #5a)*/
        clr_buf_mem = DEF_BIT_IS_SET(pbuf_hdr->Flags, NET_BUF_FLAG_CLR_MEM);
        if (clr_buf_mem != DEF_YES) {                                       /* ... & buf mem NOT clr,               ... */
            clr_ix   = pbuf_hdr->IF_HdrIx    + (CPU_INT16U)pbuf_hdr->TotLen;
            clr_len  = NET_IF_FRAME_MIN_SIZE - (CPU_INT16U)pbuf_hdr->TotLen;
            clr_size = clr_ix                +  clr_len;
            if (clr_size > pbuf_hdr->Size) {
                NET_CTR_ERR_INC(Net_ErrCtrs.NetIF_ErrTxInvalidBufLenCtr);
               *perr = NET_BUF_ERR_INVALID_LEN;
                return;
            }
            Mem_Clr((void     *)&pbuf->Data[clr_ix],                        /* ... clr rem'ing octets (see Note #5b1).  */
                    (CPU_SIZE_T) clr_len);
        }
        pbuf_hdr->TotLen = NET_IF_FRAME_MIN_SIZE;                           /* Update tot frame len.                    */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetIF_TxPktDiscard()
*
* Description : On any transmit handler errors, discard packet & buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Tx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetIF_TxPktDiscard (NET_BUF  *pbuf,
                                  NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetIF_ErrTxPktDiscardedCtr;
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
*********************************************************************************************************
*                                      LAYER INTERFACE FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      NetARP_GetHostAddrPtrHW()
*
* Description : Get pointer to this host's network interface hardware address.
*
* Argument(s) : none.
*
* Return(s)   : Pointer to network interface hardware address.
*
* Caller(s)   : NetARP_Init().
*
* Note(s)     : (1) ARP hardware address MUST be in network-order (see 'net_arp.c  NetARP_CacheHandler()
*                   Note #2e3').
*********************************************************************************************************
*/

CPU_INT08U  *NetARP_GetHostAddrPtrHW (void)
{
    CPU_INT08U  *paddr;


    paddr = (CPU_INT08U *)&NetIF_MAC_Addr[0];

    return (paddr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetARP_IsValidAddrHW()
*
* Description : Validate an ARP hardware address.
*
* Argument(s) : paddr_hw    Pointer to an ARP hardware address.
*               --------    Argument validated in NetARP_RxPktValidate().
*
* Return(s)   : DEF_YES, if hardware address valid.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetARP_RxPktValidate().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetARP_IsValidAddrHW (CPU_INT08U  *paddr_hw)
{
    CPU_BOOLEAN  valid;


    valid = NetIF_IsValidAddrSrc(paddr_hw);

    return (valid);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See this 'net_if.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of Ether IF module include (see Note #1).            */

