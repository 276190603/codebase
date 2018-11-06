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
*                                      NETWORK BUFFER MANAGEMENT
*
* Filename      : net_buf.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_BUF_MODULE
#define  NET_BUF_EXT
#else
#define  NET_BUF_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     NETWORK BUFFER FLAG DEFINES
*********************************************************************************************************
*/

                                                                /* ------------------- NET BUF FLAGS ------------------ */
#define  NET_BUF_FLAG_NONE                        DEF_BIT_NONE
#define  NET_BUF_FLAG_USED                        DEF_BIT_00    /* Buf cur used; i.e. NOT in free buf pool.             */
#define  NET_BUF_FLAG_CLR_MEM                     DEF_BIT_01    /* Buf mem clr'd.                                       */

#define  NET_BUF_FLAG_RX_BROADCAST                DEF_BIT_04    /* Indicates pkts rx'd via  broadcast.                  */
#define  NET_BUF_FLAG_RX_REMOTE                   DEF_BIT_05    /* Indicates pkts rx'd from remote host.                */
#define  NET_BUF_FLAG_RX_UDP_CHK_SUM_VALID        DEF_BIT_06    /* Indicates UDP  rx chk sum valid.                     */

#define  NET_BUF_FLAG_TX_BROADCAST                DEF_BIT_12    /* Indicates pkts to tx via broadcast.                  */


/*
*********************************************************************************************************
*                                     NETWORK BUFFER TYPE DEFINES
*
* Note(s) : (1) NET_BUF_TYPE_??? #define values specifically chosen as ASCII representations of the network
*               buffer types.  Memory displays of network buffers will display the network buffer TYPEs with 
*               their chosen ASCII names.
*********************************************************************************************************
*/

                                                                /* ------------------ NET BUF TYPES ------------------- */
#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  NET_BUF_TYPE_NONE                        0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_BUF_TYPE_SMALL                       0x534D414C    /* "SMAL" in ASCII.                                     */
#define  NET_BUF_TYPE_LARGE                       0x4C415247    /* "LARG" in ASCII.                                     */

#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)
#define  NET_BUF_TYPE_NONE                        0x454E4F4E    /* "NONE" in ASCII.                                     */
#define  NET_BUF_TYPE_SMALL                       0x4C414D53    /* "SMAL" in ASCII.                                     */
#define  NET_BUF_TYPE_LARGE                       0x4752414C    /* "LARG" in ASCII.                                     */

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  NET_BUF_TYPE_NONE                        0x4F4E454E    /* "NONE" in ASCII.                                     */
#define  NET_BUF_TYPE_SMALL                       0x4D534C41    /* "SMAL" in ASCII.                                     */
#define  NET_BUF_TYPE_LARGE                       0x414C4752    /* "LARG" in ASCII.                                     */

#else                                                           /* Dflt CPU_WORD_SIZE_08.                               */
#define  NET_BUF_TYPE_NONE                        0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_BUF_TYPE_SMALL                       0x534D414C    /* "SMAL" in ASCII.                                     */
#define  NET_BUF_TYPE_LARGE                       0x4C415247    /* "LARG" in ASCII.                                     */

#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NETWORK BUFFER INDEX & SIZE DEFINES
*
* Note(s) : (1) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX's  value is pre-#define'd in 'net_cfg_net.h' :
*
*               (a) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX's ideal #define'tion :
*
*                       (A) IF Hdr  +  max(Protocol Headers)
*
*               (b) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX  #define'd with hard-coded knowledge that IF, IP &
*                   TCP headers have the largest combined maximum size of all the protocol headers :
*
*                             IP Hdr   60     ARP Hdr   68      IP Hdr   60      IP Hdr   60
*                           ICMP Hdr    0                      UDP Hdr    8     TCP Hdr   60
*                           -------------     ------------     ------------     ------------
*                           Total      60     Total     68     Total     68     Total    120
*
*               (c) Assumes maximum ARP header size of 68 octets based on maximum length hardware & protocol
*                   addresses.  Actual ARP header size depends on actual hardware & protocol address lengths.
*                   See 'net_arp.h  Note #2' for supported hardware & protocol types.
*
*               (d) The minimum network buffer size MUST be configured greater than the maximum network packet
*                   header sizes so that the first of any fragmented packets always contains a complete receipt 
*                   of all frame & network packet headers.
*
*           (2) (a) Since network data value macro's appropriately convert data values from any CPU addresses,
*                   word-aligned or not; network receive & transmit packets are NOT required to ensure that 
*                   network packet headers (ARP/IP/UDP/TCP/etc.) & header members will locate on CPU word-
*                   aligned addresses.  Therefore, network receive & transmit packets are NOT required to 
*                   start on any specific network buffer indices.
*
*                   See also 'net_util.h  NETWORK DATA VALUE MACRO'S  Note #2b'.
*
*               (b) However, many processors may be more efficient & may even REQUIRE that memory transfers
*                   occur on CPU word-aligned addresses [e.g. processors or NICs with direct memory access
*                   (DMA) capability].  Therefore, network receive & transmit packets SHOULD start on CPU
*                   word-aligned network buffer indices.
*
*                   (1) Receive  packet index SHOULD be configured for the first CPU word-aligned 
*                       network  buffer index.
*
*                   (2) Transmit data   index SHOULD be configured to ensure that the transmit packet 
*                       network  buffer index is CPU word-aligned.
*
*                       (A) However, this assumes that a single data index may be configured that can handle
*                           all possible combinations of network packet header lengths while still ensuring
*                           that each transmit packet's network buffer index is CPU word-aligned.
*
*                           This assumption is valid if & only if all combinations of network packet header
*                           lengths are multiples of the CPU's word size.
*
*                       See also Note #1d.
*
*                   See also 'lib_mem.h  MEMORY DATA VALUE MACRO'S  Note #1a'.
*********************************************************************************************************
*/

                                                                /* --------------- NET BUF HDR/DATA IXs --------------- */
#define  NET_BUF_DATA_BASE                                 0

                                                                /* See Note #2b1.                                       */
#define  NET_BUF_DATA_RX_IX                              NET_BUF_DATA_BASE

                                                                /* Tx ix/sizes based on max pkt hdr sizes ...           */
                                                                /* ... (see Note #2b2).                                 */
#define  NET_BUF_DATA_PROTOCOL_HDR_BASE                  NET_BUF_DATA_BASE

#define  NET_BUF_DATA_TX_IX                             (NET_BUF_DATA_PROTOCOL_HDR_BASE + NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX)

#define  NET_BUF_DATA_SIZE_MIN                           NET_BUF_DATA_TX_IX
#define  NET_BUF_DATA_PROTOCOL_HDR_SIZE                  NET_BUF_DATA_TX_IX


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                  NETWORK BUFFER QUANTITY DATA TYPE
*
* Note(s) : (1) NET_BUF_NBR_MAX  SHOULD be #define'd based on 'NET_BUF_QTY' data type declared.
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_BUF_QTY;                               /* Defines max qty of net bufs to support.              */

#define  NET_BUF_NBR_MIN                                   0
#define  NET_BUF_NBR_MAX                 DEF_INT_16U_MAX_VAL    /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                    NETWORK BUFFER SIZE DATA TYPE
*
* Note(s) : (1) NET_BUF_IX_NONE  SHOULD be #define'd based on 'NET_BUF_SIZE' data type declared.
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_BUF_SIZE;                              /* Defines   max size of net buf data.                  */

#define  NET_BUF_IX_NONE                 DEF_INT_16U_MAX_VAL    /* Define as max unsigned val (see Note #1).            */


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NETWORK BUFFER HEADER DATA TYPE
*
*                                            NET_BUF_HDR
*                                          |-------------|
*                                          | Buffer Type |
*                                          |-------------|
*                                          | Buffer Size |
*                                          |-------------|
*                                          |    Flags    |
*                              Previous    |-------------|
*                               Buffer <----------O      |
*                                Lists     |-------------|      Next
*                                          |      O----------> Buffer
*                              Previous    |-------------|      Lists
*                               Buffer <----------O      |
*                                          |-------------|      Next
*                                          |      O----------> Buffer
*                                          |-------------|
*                                          |      O----------> Buffer
*                                          |-------------|      Timer
*                                          |             |
*                                          |      O----------> Unlink Fnct
*                                          |             |      & Obj Ptrs
*                                          |-------------|
*                                          |  Reference  |
*                                          |   Counter   |
*                                          |-------------|
*                                          |             |
*                                          |             |
*                                          |  Protocol   |
*                                          |   Header    |
*                                          |  Indices/   |
*                                          |  Lengths    |
*                                          |             |
*                                          |             |
*                                          |-------------|
*                                          |             |
*                                          |  Protocol   |
*                                          |  Controls   |
*                                          |             |
*                                          | ----------- |
*                                          |     ARP     |
*                                          | ----------- |
*                                          |     IP      |
*                                          | ----------- |
*                                          |   TCP/UDP   |
*                                          | ----------- |
*                                          |     TCP     |
*                                          | ----------- |
*                                          |    Conn     |
*                                          |-------------|
*
*
* Note(s) : (1) Protocol Header & Data indices into DATA area declared as 'CPU_INT16U' because current 
*               TCP/IPv4 header sizes do NOT exceed 'CPU_INT16U' index space.
*
*           (2) 'TotLen'/'DataLen' calculate total buffer data length & 'Application' data length.
*
*               For received buffers, total buffer data length is set to the incoming packet length; 
*               'Application' data length (& index) is calculated as total buffer data lengths minus
*               all protocol header lengths found in the buffer.
*
*               For transmitted buffers, 'Application' data length (& index) is set by higher-layer
*               network protocols or the application layer; total buffer data length is calculated 
*               as the 'Application' data length plus all protocol header lengths inserted into the
*               buffer.
*
*           (3) The following variables ideally declared as specific TCP data types; declared as CPU 
*               data types because TCP data types NOT defined until 'net_tcp.h' (see 'net.h  NETWORK
*               INCLUDE FILES') :
*
*               (a) TCP transmit acknowledgement code variables ideally declared as 'NET_TCP_ACK_CODE';
*                       declared as 'CPU_INT08U'.
*********************************************************************************************************
*/
/*$PAGE*/
                                                    /* ------------------------- NET BUF HDR -------------------------- */
typedef  struct  net_buf_hdr {
    NET_TYPE               Type;                    /* Buf type cfg'd @ init : NET_BUF_TYPE_SMALL / NET_BUF_TYPE_LARGE. */
    NET_BUF_SIZE           Size;                    /* Buf size cfg'd @ init : NET_BUF_SIZE_SMALL / NET_BUF_SIZE_LARGE. */
    CPU_INT16U             Flags;                   /* Buf flags.                                                       */

    NET_BUF_QTY            ID;                      /* Buf id.                                                          */
    CPU_INT08U             RefCtr;                  /* Nbr of ext refs pointing to this buf.                            */


    NET_BUF               *PrevPrimListPtr;         /* Ptr to PREV prim list.                                           */
    NET_BUF               *NextPrimListPtr;         /* Ptr to NEXT prim list.                                           */

    NET_BUF               *PrevSecListPtr;          /* Ptr to PREV sec  list.                                           */
    NET_BUF               *NextSecListPtr;          /* Ptr to NEXT sec  list.                                           */

    NET_BUF               *PrevBufPtr;              /* Ptr to PREV buf.                                                 */
    NET_BUF               *NextBufPtr;              /* Ptr to NEXT buf.                                                 */

    NET_TMR               *TmrPtr;                  /* Ptr to buf TMR.                                                  */

    CPU_FNCT_PTR           UnlinkFnctPtr;           /* Ptr to fnct used to unlink buf from multiple refs.               */
    void                  *UnlinkObjPtr;            /* Ptr to obj       to unlink buf from.                             */



    NET_PROTOCOL_TYPE      ProtocolHdrType;         /* Hdr protocol type.                                               */

    CPU_INT16U             IF_HdrIx;                /* IF                   hdr  ix  (in DATA area).                    */
    CPU_INT16U             IF_HdrLen;               /* IF                   hdr  len (in octets   ).                    */

#ifdef  NET_ARP_MODULE_PRESENT
    CPU_INT16U             ARP_MsgIx;               /* ARP                  msg  ix  (in DATA area).                    */
    CPU_INT16U             ARP_MsgLen;              /* ARP                  msg  len (in octets   ).                    */
#endif

    CPU_INT16U             IP_HdrIx;                /* IP                   hdr  ix  (in DATA area).                    */
    CPU_INT16U             IP_HdrLen;               /* IP                   hdr  len (in octets   ).                    */
    CPU_INT16U             IP_TotLen;               /* IP                   tot  len (in octets   ).                    */
    CPU_INT16U             IP_DataLen;              /* IP pkt/frag          data len (in octets   ).                    */
    CPU_INT16U             IP_DatagramLen;          /* IP complete datagram data len (in octets   ).                    */

    CPU_INT16U             ICMP_MsgIx;              /* ICMP                 msg  ix  (in DATA area).                    */
    CPU_INT16U             ICMP_MsgLen;             /* ICMP                 msg  len (in octets   ).                    */
    CPU_INT16U             ICMP_HdrLen;             /* ICMP                 hdr  len (in octets   ).                    */

    CPU_INT16U             TCP_UDP_HdrDataIx;       /* TCP/UDP pkt               ix  (in DATA area).                    */
    CPU_INT16U             TCP_UDP_HdrLen;          /* TCP/UDP hdr               len (in octets   ).                    */
    CPU_INT16U             TCP_UDP_TotLen;          /* TCP/UDP tot               len (in octets   ).                    */
    CPU_INT16U             TCP_UDP_DataLen;         /* TCP/UDP tot          data len (in octets   ).                    */


    CPU_INT16U             DataIx;                  /* App                  DATA ix  (in DATA area).                    */
    NET_BUF_SIZE           DataLen;                 /* App                  DATA len (in octets   ).                    */

    NET_BUF_SIZE           TotLen;                  /* ALL                  DATA len (in octets   ).                    */



#ifdef  NET_ARP_MODULE_PRESENT
    CPU_INT08U            *ARP_AddrHW_Ptr;          /* Ptr to ARP hw       addr.                                        */
    CPU_INT08U            *ARP_AddrProtocolPtr;     /* Ptr to ARP protocol addr.                                        */
#endif



    CPU_INT16U             IP_ID;                   /* IP datagram id.                                                  */

    CPU_INT16U             IP_Flags_FragOffset;     /* IP rx flags/frag offset.                                         */

    CPU_INT16U             IP_FragSizeTot;          /* Tot IP rx frag size.                                             */
    CPU_INT16U             IP_FragSizeCur;          /* Cur IP rx frag size.                                             */

    NET_IP_ADDR            IP_AddrSrc;              /* IP src             addr.                                         */
    NET_IP_ADDR            IP_AddrDest;             /* IP dest            addr.                                         */
    NET_IP_ADDR            IP_AddrNextRoute;        /* IP tx 'Next-Route' addr.                                         */
    NET_IP_ADDR            IP_AddrNextRouteNetOrder;/* IP tx 'Next-Route' addr in net-order.                            */

    NET_BUF               *IP_OptPtr;               /* Ptr to IP rx opts.                                               */



/*$PAGE*/
    NET_PORT_NBR           TCP_UDP_PortSrc;         /* TCP/UDP rx src  port.                                            */
    NET_PORT_NBR           TCP_UDP_PortDest;        /* TCP/UDP rx dest port.                                            */


#ifdef  NET_TCP_MODULE_PRESENT
    CPU_INT16U             TCP_HdrLen_Flags;        /* TCP hdr len/flags.                                               */

    NET_TCP_SEG_SIZE       TCP_SegLenInit;          /* TCP init  seg len.                                               */
    NET_TCP_SEG_SIZE       TCP_SegLen;              /* TCP       seg len [i.e. data len & possible sync/close ctrl(s)]. */
    NET_TCP_SEG_SIZE       TCP_SegLenLast;          /* TCP last  seg len [i.e. last len tx'd].                          */
    NET_TCP_SEG_SIZE       TCP_SegLenData;          /* TCP data  seg len [i.e. data len].                               */
    CPU_INT16U             TCP_SegReTxCtr;          /* Indicates nbr seg re-tx's.                                       */
    CPU_BOOLEAN            TCP_SegSync;             /* Indicates TCP sync  seg.                                         */
    CPU_BOOLEAN            TCP_SegClose;            /* Indicates TCP close seg.                                         */
    CPU_BOOLEAN            TCP_SegReset;            /* Indicates TCP reset seg.                                         */
    CPU_BOOLEAN            TCP_SegAck;              /* Indicates TCP ack   seg.                                         */
    CPU_BOOLEAN            TCP_SegAckTxd;           /* Indicates TCP ack tx'd        for rx'd seg.                      */
    CPU_INT08U             TCP_SegAckTxReqCode;     /* Indicates TCP ack tx req code for rx'd seg (see Note #3a).       */

    NET_TCP_SEQ_NBR        TCP_SeqNbrInit;          /* TCP init  seq nbr.                                               */
    NET_TCP_SEQ_NBR        TCP_SeqNbrLast;          /* TCP last  seq nbr                                                */
    NET_TCP_SEQ_NBR        TCP_SeqNbr;              /* TCP       seq nbr                                                */
    NET_TCP_SEQ_NBR        TCP_AckNbr;              /* TCP       ack nbr                                                */
    NET_TCP_SEQ_NBR        TCP_AckNbrLast;          /* TCP last  ack nbr                                                */
    NET_TCP_SEG_SIZE       TCP_MaxSegSize;          /* TCP max   seg size.                                              */
    NET_TCP_WIN_SIZE       TCP_WinSize;             /* TCP       win size.                                              */
    NET_TCP_WIN_SIZE       TCP_WinSizeLast;         /* TCP last  win size.                                              */

    NET_TCP_TX_RTT_TS_MS   TCP_RTT_TS_Rxd_ms;       /* TCP RTT TS @ seg rx'd (in ms).                                   */
    NET_TCP_TX_RTT_TS_MS   TCP_RTT_TS_Txd_ms;       /* TCP RTT TS @ seg tx'd (in ms).                                   */

    CPU_INT16U             TCP_Flags;               /* TCP tx flags.                                                    */
#endif


#ifdef  NET_CONN_MODULE_PRESENT
    NET_CONN_ID            Conn_ID;                 /* Net             conn id.                                         */
    NET_CONN_ID            Conn_ID_Transport;       /* Transport layer conn id.                                         */
    NET_CONN_ID            Conn_ID_App;             /* App       layer conn id.                                         */

    CPU_INT08U             ConnType;                /* Conn type.                                                       */
#endif


} NET_BUF_HDR;


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NETWORK BUFFER DATA TYPE
*
*                                              NET_BUF
*                                          |-------------|
*                                          |             |
*                                          |             |
*                                          |   Buffer    |
*                                          |   Header    |
*                                          |             |
*                                          |             |
*                                          |-------------|
*                                          |  Alignment  |
*                                          |    Word     |
*                                          |-------------|
*                                          |             |
*                                          |             |
*                                          |             |
*                                          |   Buffer    |
*                                          |    Data     |
*                                          |             |
*                                          |             |
*                                          |             |
*                                          |             |
*                                          |-------------|
*
*
* Note(s) : (1) Forced word-alignment at start of network buffer NOT required since first data member 
*               'Hdr.Type' is declared as 'CPU_INT32U'.
*
*           (2) 'Dummy' declared to force 'Data' buffer word-alignment.
*
*           (3) NET_BUF_CFG_DATA_SIZE_SMALL & NET_BUF_CFG_DATA_SIZE_LARGE  MUST be declared with values ...
*
*               (a) that are greater than or equal to NET_BUF_DATA_SIZE_MIN (see 'NETWORK BUFFER INDEX & SIZE DEFINES')
*               (b) such that NET_BUF_CFG_DATA_SIZE_LARGE has a value greater than NET_BUF_CFG_DATA_SIZE_SMALL
*
*               ... and SHOULD be declared with values ...
*
*               (c) that are integer multiples of 'CPU_ALIGN' word size (to force word-alignment of buffer arrays)
*
*           (4) (a) 'Data' buffer is storage for both Protocol Headers & Application data.
*
*               (b) 'NET_BUF_SMALL'/'NET_BUF_LARGE' 'Data' buffer declared with an additional CPU data word size so 
*                   that NICs may efficiently & safely read or write data without overflowing the 'Data' buffer.
*
*                   This additional CPU data word size does NOT increase the overall useable network buffer 'Data' 
*                   size (see also 'net_buf.c  NetBuf_GetMaxSize()  Note #3').
*
*               (c) 'NET_BUF's 'Data' declared with 1 entry; prevents removal by compiler optimization.
*
*           (5) Whenever generic 'NET_BUF' pointer 'pbuf' is used to index NetBuf_SmallTbl[] or NetBuf_LargeTbl[],
*               'pbuf' CANNOT be pointer-incremented to access consecutive table buffers; 'pbuf' MUST load the 
*               address of each table buffer entry.
*********************************************************************************************************
*/

                                                        /* ---------------------- NET BUF SMALL ----------------------- */
typedef  struct  net_buf_small {
    NET_BUF_HDR     Hdr;
    CPU_ALIGN       Dummy;                              /* See Note  #2.                                                */
    CPU_INT08U      Data[NET_BUF_CFG_DATA_SIZE_SMALL    /* See Notes #4a & #3.                                          */
                        + sizeof(CPU_DATA)];            /* See Note  #4b.                                               */
} NET_BUF_SMALL;


                                                        /* ---------------------- NET BUF LARGE ----------------------- */
typedef  struct  net_buf_large {
    NET_BUF_HDR     Hdr;
    CPU_ALIGN       Dummy;                              /* See Note  #2.                                                */
    CPU_INT08U      Data[NET_BUF_CFG_DATA_SIZE_LARGE    /* See Notes #4a & #3.                                          */
                        + sizeof(CPU_DATA)];            /* See Note  #4b.                                               */
} NET_BUF_LARGE;


                                                        /* ------------------------- NET BUF -------------------------- */
struct  net_buf {                                       /* Used to point to _SMALL/_LARGE bufs (see Note #5).           */
    NET_BUF_HDR     Hdr;
    CPU_ALIGN       Dummy;                              /* See Note #2.                                                 */
    CPU_INT08U      Data[1];                            /* See Note #4c.                                                */
};


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

#if (NET_BUF_CFG_NBR_SMALL > 0)                                         /* -------------- SMALL NET BUFS -------------- */
NET_BUF_EXT  NET_BUF_SMALL   NetBuf_SmallTbl[NET_BUF_CFG_NBR_SMALL];
NET_BUF_EXT  NET_BUF_SMALL  *NetBuf_SmallPoolPtr;                       /* Ptr to pool of free small net bufs.          */
NET_BUF_EXT  NET_STAT_POOL   NetBuf_SmallPoolStat;
#endif

#if (NET_BUF_CFG_NBR_LARGE > 0)                                         /* -------------- LARGE NET BUFS -------------- */
NET_BUF_EXT  NET_BUF_LARGE   NetBuf_LargeTbl[NET_BUF_CFG_NBR_LARGE];
NET_BUF_EXT  NET_BUF_LARGE  *NetBuf_LargePoolPtr;                       /* Ptr to pool of free large net bufs.          */
NET_BUF_EXT  NET_STAT_POOL   NetBuf_LargePoolStat;
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void           NetBuf_Init                     (void);


                                                                            /* ----------- BUF ALLOC FNCTS ------------ */
NET_BUF_SIZE   NetBuf_GetMaxSize               (NET_BUF       *pbuf,
                                                NET_BUF_SIZE   ix_start);

NET_BUF       *NetBuf_Get                      (NET_BUF_SIZE   size,
                                                NET_BUF_SIZE   ix_start,
                                                CPU_INT16U     flags,
                                                NET_ERR       *perr);


void           NetBuf_Free                     (NET_BUF       *pbuf);

void           NetBuf_FreeBuf                  (NET_BUF       *pbuf,
                                                NET_CTR       *pctr);

void           NetBuf_FreeBufList              (NET_BUF       *pbuf_list,
                                                NET_CTR       *pctr);

void           NetBuf_FreeBufQ_PrimList        (NET_BUF       *pbuf_q,
                                                NET_CTR       *pctr);

void           NetBuf_FreeBufQ_SecList         (NET_BUF       *pbuf_q,
                                                NET_CTR       *pctr,
                                                CPU_FNCT_PTR   pfnct_unlink);


                                                                            /* ------------ BUF API FNCTS ------------- */
void           NetBuf_DataRd                   (NET_BUF       *pbuf,
                                                NET_BUF_SIZE   ix,
                                                NET_BUF_SIZE   len,
                                                CPU_INT08U    *pdest,
                                                NET_ERR       *perr);

void           NetBuf_DataWr                   (NET_BUF       *pbuf,
                                                NET_BUF_SIZE   ix,
                                                NET_BUF_SIZE   len,
                                                CPU_INT08U    *psrc,
                                                NET_ERR       *perr);


                                                                            /* ----------- BUF STATUS FNCTS ----------- */
CPU_BOOLEAN    NetBuf_IsUsed                   (NET_BUF       *pbuf);

#if (NET_BUF_CFG_NBR_SMALL > 0)
NET_STAT_POOL  NetBuf_SmallPoolStatGet         (void);

void           NetBuf_SmallPoolStatResetMaxUsed(void);
#endif

#if (NET_BUF_CFG_NBR_LARGE > 0)
NET_STAT_POOL  NetBuf_LargePoolStatGet         (void);

void           NetBuf_LargePoolStatResetMaxUsed(void);
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_BUF_CFG_NBR_SMALL
#error  "NET_BUF_CFG_NBR_SMALL              not #define'd in 'net_cfg.h'                 "
#error  "                             [NET_BUF_CFG_NBR_SMALL  MUST be >= NET_BUF_NBR_MIN]"
#error  "                             [                            && <= NET_BUF_NBR_MAX]"

#elif  ((NET_BUF_CFG_NBR_SMALL < NET_BUF_NBR_MIN) || \
        (NET_BUF_CFG_NBR_SMALL > NET_BUF_NBR_MAX))
#error  "NET_BUF_CFG_NBR_SMALL        illegally #define'd in 'net_cfg.h'       "
#error  "                             [NET_BUF_CFG_NBR_SMALL  MUST be >= NET_BUF_NBR_MIN]"
#error  "                             [                            && <= NET_BUF_NBR_MAX]"
#elif   (NET_BUF_CFG_NBR_SMALL > 0)

#ifndef  NET_BUF_CFG_DATA_SIZE_SMALL
#error  "NET_BUF_CFG_DATA_SIZE_SMALL        not #define'd in 'net_cfg.h'                             "
#error  "                             [NET_BUF_CFG_DATA_SIZE_SMALL  MUST be >= NET_BUF_DATA_SIZE_MIN]"
#error  "                             [(see 'net_buf.h  NETWORK BUFFER DATA TYPE  Note #3a')        ]"

#elif   (NET_BUF_CFG_DATA_SIZE_SMALL < NET_BUF_DATA_SIZE_MIN)
#error  "NET_BUF_CFG_DATA_SIZE_SMALL  illegally #define'd in 'net_cfg.h'                             "
#error  "                             [NET_BUF_CFG_DATA_SIZE_SMALL  MUST be >= NET_BUF_DATA_SIZE_MIN]"
#error  "                             [(see 'net_buf.h  NETWORK BUFFER DATA TYPE  Note #3a')        ]"
#endif

#if 0                                                                   /* Alignment is NOT absolutely required.        */
#if   (((CPU_CFG_DATA_SIZE          == CPU_WORD_SIZE_32)        && \
       ((NET_BUF_CFG_DATA_SIZE_SMALL % CPU_WORD_SIZE_32) != 0)) || \
       ((CPU_CFG_DATA_SIZE          == CPU_WORD_SIZE_16)        && \
       ((NET_BUF_CFG_DATA_SIZE_SMALL % CPU_WORD_SIZE_16) != 0)))
#error  "NET_BUF_CFG_DATA_SIZE_SMALL  illegally #define'd in 'net_cfg.h'                     "
#error  "                             [NET_BUF_CFG_DATA_SIZE_SMALL  SHOULD be an integer    ]"
#error  "                             [    multiple of 'CPU_ALIGN' word size                ]"
#error  "                             [(see 'net_buf.h  NETWORK BUFFER DATA TYPE  Note #3c')]"
#endif
#endif

#endif



#ifndef  NET_BUF_CFG_NBR_LARGE
#error  "NET_BUF_CFG_NBR_LARGE              not #define'd in 'net_cfg.h'                 "
#error  "                             [NET_BUF_CFG_NBR_LARGE  MUST be >= NET_BUF_NBR_MIN]"
#error  "                             [                            && <= NET_BUF_NBR_MAX]"

#elif  ((NET_BUF_CFG_NBR_LARGE < NET_BUF_NBR_MIN) || \
        (NET_BUF_CFG_NBR_LARGE > NET_BUF_NBR_MAX))
#error  "NET_BUF_CFG_NBR_LARGE        illegally #define'd in 'net_cfg.h'                 "
#error  "                             [NET_BUF_CFG_NBR_LARGE  MUST be >= NET_BUF_NBR_MIN]"
#error  "                             [                            && <= NET_BUF_NBR_MAX]"
#elif   (NET_BUF_CFG_NBR_LARGE > 0)

#ifndef  NET_BUF_CFG_DATA_SIZE_LARGE
#error  "NET_BUF_CFG_DATA_SIZE_LARGE        not #define'd in 'net_cfg.h'                             "
#error  "                             [NET_BUF_CFG_DATA_SIZE_LARGE  MUST be >= NET_BUF_DATA_SIZE_MIN]"
#error  "                             [(see 'net_buf.h  NETWORK BUFFER DATA TYPE  Note #3a')        ]"

#elif   (NET_BUF_CFG_DATA_SIZE_LARGE < NET_BUF_DATA_SIZE_MIN)
#error  "NET_BUF_CFG_DATA_SIZE_LARGE  illegally #define'd in 'net_cfg.h'                             "
#error  "                             [NET_BUF_CFG_DATA_SIZE_LARGE  MUST be >= NET_BUF_DATA_SIZE_MIN]"
#error  "                             [(see 'net_buf.h  NETWORK BUFFER DATA TYPE  Note #3a')        ]"
#endif

#if 0                                                                   /* Alignment is NOT absolutely required.        */
#if   (((CPU_CFG_DATA_SIZE          == CPU_WORD_SIZE_32)        && \
       ((NET_BUF_CFG_DATA_SIZE_LARGE % CPU_WORD_SIZE_32) != 0)) || \
       ((CPU_CFG_DATA_SIZE          == CPU_WORD_SIZE_16)        && \
       ((NET_BUF_CFG_DATA_SIZE_LARGE % CPU_WORD_SIZE_16) != 0)))
#error  "NET_BUF_CFG_DATA_SIZE_LARGE  illegally #define'd in 'net_cfg.h'                     "
#error  "                             [NET_BUF_CFG_DATA_SIZE_LARGE  SHOULD be an integer    ]"
#error  "                             [    multiple of 'CPU_ALIGN' word size                ]"
#error  "                             [(see 'net_buf.h  NETWORK BUFFER DATA TYPE  Note #3c')]"
#endif
#endif

#endif



/*$PAGE*/
#if    ((NET_BUF_CFG_NBR_SMALL < 1) && \
        (NET_BUF_CFG_NBR_LARGE < 1))
#error  "NET_BUF_CFG_NBR_SMALL &&                                         "
#error  "NET_BUF_CFG_NBR_LARGE        illegally #define'd in 'net_cfg.h'  "
#error  "                             [NET_BUF_CFG_NBR_SMALL  ||         ]"
#error  "                             [NET_BUF_CFG_NBR_LARGE  MUST be > 0]"
#endif



#if    ((NET_BUF_CFG_NBR_SMALL       > 0                          ) && \
        (NET_BUF_CFG_NBR_LARGE       > 0                          ) && \
        (NET_BUF_CFG_DATA_SIZE_LARGE < NET_BUF_CFG_DATA_SIZE_SMALL))
#error  "NET_BUF_CFG_DATA_SIZE_LARGE  illegally #define'd in 'net_cfg.h'                                   "
#error  "                             [NET_BUF_CFG_DATA_SIZE_LARGE  MUST be >= NET_BUF_CFG_DATA_SIZE_SMALL]"
#error  "                             [(see 'net_buf.h  NETWORK BUFFER DATA TYPE  Note #3b')              ]"
#endif

