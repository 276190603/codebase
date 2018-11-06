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
*                                          NETWORK ICMP LAYER
*                                 (INTERNET CONTROL MESSAGE PROTOCOL)
*
* Filename      : net_icmp.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports Internet Control Message Protocol as described in RFC #792 with the following
*                     restrictions/constraints :
*
*                     (a) Does NOT support IP forwarding/routing               RFC #1122, Section 3.3.1
*
*                     (b) Does NOT support IP multicasting                     RFC #1122, Section 3.3.7
*
*                     (c) Does NOT support ICMP Address Mask Agent/Server      RFC #1122, Section 3.2.2.9
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_ICMP_MODULE
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

                                                                    /* ------------------- RX FNCTS ------------------- */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void                     NetICMP_RxPktValidateBuf         (NET_BUF_HDR             *pbuf_hdr,
                                                                   NET_ERR                 *perr);
#endif

static  void                     NetICMP_RxPktValidate            (NET_BUF                 *pbuf,
                                                                   NET_BUF_HDR             *pbuf_hdr,
                                                                   NET_ICMP_HDR            *picmp_hdr,
                                                                   NET_ERR                 *perr);

static  void                     NetICMP_RxPktFree                (NET_BUF                 *pbuf);

static  void                     NetICMP_RxPktDiscard             (NET_BUF                 *pbuf,
                                                                   NET_ERR                 *perr);


                                                                    /* ------------------- TX FNCTS ------------------- */

static  void                     NetICMP_TxMsgErrValidate         (NET_BUF                 *pbuf,
                                                                   NET_BUF_HDR             *pbuf_hdr,
                                                                   NET_IP_HDR              *pip_hdr,
                                                                   CPU_INT08U               type,
                                                                   CPU_INT08U               code,
                                                                   CPU_INT08U               ptr,
                                                                   NET_ERR                 *perr);


static  void                     NetICMP_TxMsgReqValidate         (CPU_INT08U               type,
                                                                   CPU_INT08U               code,
                                                                   NET_ERR                 *perr);


static  void                     NetICMP_TxMsgReply               (NET_BUF                 *pbuf,
                                                                   NET_BUF_HDR             *pbuf_hdr,
                                                                   NET_ICMP_HDR            *picmp_hdr,
                                                                   NET_ERR                 *perr);



static  void                     NetICMP_TxPktFree                (NET_BUF                 *pbuf);

static  void                     NetICMP_TxPktDiscard             (NET_BUF                 *pbuf,
                                                                   NET_ERR                 *perr);


                                                                    /* ---------------- HANDLER FNCTS ----------------- */

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
static  void                     NetICMP_HandlerTxSrcQuench       (NET_BUF                 *pbuf);

static  void                     NetICMP_HandlerTxSrcQuenchClrList(void);

static  void                     NetICMP_HandlerTxSrcQuenchClr    (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench);

static  NET_ICMP_TX_SRC_QUENCH  *NetICMP_HandlerTxSrcQuenchGet    (NET_ERR                 *perr);

static  void                     NetICMP_HandlerTxSrcQuenchFree   (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench);

static  void                     NetICMP_HandlerTxSrcQuenchRemove (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench);

static  void                     NetICMP_HandlerTxSrcQuenchInsert (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench);

static  void                     NetICMP_HandlerTxSrcQuenchUnlink (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench);

#if (NET_ERR_CFG_ARG_CHK_DBG_EN  == DEF_ENABLED)
static  void                     NetICMP_HandlerTxSrcQuenchDiscard(NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench);
#endif
#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetICMP_Init()
*
* Description : (1) Initialize Internet Control Message Protocol Layer :
*
*                   (a) Initialize ICMP transmit source quench :
*                       (1) Initialize ICMP transmit source quench pool
*                       (2) Initialize ICMP transmit source quench table
*                       (3) Initialize ICMP transmit source quench list pointers
*
*                   (b) Initialize ICMP transmit sequence number counter
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
* Note(s)     : (2) ICMP transmit source quench pool MUST be initialized PRIOR to initializing the pool 
*                   with pointers to ICMP transmit source quench entries.
*********************************************************************************************************
*/

void  NetICMP_Init (void)
{
#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
    NET_ICMP_TX_SRC_QUENCH   *ptx_src_quench_tbl;
    NET_ICMP_SRC_QUENCH_QTY   i;
    NET_ERR                   err;


                                                                /* -------- INIT ICMP TX SRC QUENCH POOL/STATS -------- */
    NetICMP_TxSrcQuenchPoolPtr = (NET_ICMP_TX_SRC_QUENCH *)0;   /* Init-clr ICMP tx src quench pool (see Note #2).      */

    NetStat_PoolInit((NET_STAT_POOL   *)&NetICMP_TxSrcQuenchPoolStat,
                     (NET_STAT_POOL_QTY) NET_ICMP_CFG_TX_SRC_QUENCH_SIZE,
                     (NET_ERR         *)&err);


                                                                /* ----------- INIT ICMP TX SRC QUENCH TBL ------------ */
    ptx_src_quench_tbl = &NetICMP_TxSrcQuenchTbl[0];
    for (i = 0; i < NET_ICMP_CFG_TX_SRC_QUENCH_SIZE; i++) {
                                                                /* Init each ICMP tx src quench type--NEVER modify.     */
        ptx_src_quench_tbl->Type  = NET_ICMP_TYPE_TX_SRC_QUENCH;

        ptx_src_quench_tbl->Flags = NET_ICMP_FLAG_NONE;         /* Init each ICMP tx src quench as NOT used.            */

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
        NetICMP_HandlerTxSrcQuenchClr(ptx_src_quench_tbl);
#endif
                                                                /* Free each entry to pool (see Note #2).               */
        ptx_src_quench_tbl->NextPtr = NetICMP_TxSrcQuenchPoolPtr;    
        NetICMP_TxSrcQuenchPoolPtr  = ptx_src_quench_tbl;                         

        ptx_src_quench_tbl++;
    }


                                                                /* -------- INIT ICMP TX SRC QUENCH LIST PTRS --------- */
    NetICMP_TxSrcQuenchListHead = (NET_ICMP_TX_SRC_QUENCH *)0;
    NetICMP_TxSrcQuenchListTail = (NET_ICMP_TX_SRC_QUENCH *)0;
#endif


                                                                /* ------------- INIT ICMP TX SEQ NBR CTR ------------- */
    NetICMP_TxSeqNbrCtr = 0;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetICMP_CfgTxSrcQuenchTh()
*
* Description : Configure ICMP transmit source quench entry's access transmit threshold.
*
* Argument(s) : th          Desired number of received IP packets from a specific IP source host that trips
*                               the transmission of an additional ICMP Source Quench Error Message.
*
* Return(s)   : DEF_OK,   ICMP transmit source quench threshold configured.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) 'NetICMP_TxSrcQuenchTxTh_nbr' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
CPU_BOOLEAN  NetICMP_CfgTxSrcQuenchTh (CPU_INT16U  th)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_ICMP_TX_SRC_QUENCH_TH_MIN > DEF_INT_16U_MIN_VAL)
    if (th < NET_ICMP_TX_SRC_QUENCH_TH_MIN) {
        return (DEF_FAIL);
    }
#endif
#if (NET_ICMP_TX_SRC_QUENCH_TH_MAX < DEF_INT_16U_MAX_VAL)
    if (th > NET_ICMP_TX_SRC_QUENCH_TH_MAX) {
        return (DEF_FAIL);
    }
#endif

    CPU_CRITICAL_ENTER();
    NetICMP_TxSrcQuenchTxTh_nbr = th;
    CPU_CRITICAL_EXIT();

    return (DEF_OK);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetICMP_Rx()
*
* Description : (1) Process received messages :
*
*                   (a) Validate ICMP packet
*                   (b) Demultiplex ICMP message
*                   (c) Free ICMP packet
*                   (d) Update receive statistics
*
*               (2) Although ICMP data units are typically referred to as 'messages' (see RFC #792, Section
*                   'Introduction'), the term 'ICMP packet' (see RFC #1983, 'packet') is used for ICMP 
*                   Receive until the packet is validated as an ICMP message.
*
*
* Argument(s) : pbuf        Pointer to network buffer that received ICMP packet.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ICMP_ERR_NONE               ICMP message successfully received & processed.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               ---- RETURNED BY NetICMP_RxPktDiscard() : -----
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_RxPktDemuxDatagram().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (3) NetICMP_Rx() blocked until network initialization completes.
*
*               (4) #### ICMP Receive Error/Reply Messages NOT yet implemented :
*
*                   (a) Define "User Process" to report ICMP Error Messages to Transport &/or Application Layers.
*
*                   (b) Define procedure to demultiplex & enqueue ICMP Reply Messages to Application.
*
*                       (1) MUST implement mechanism to de-queue ICMP message data from single, complete
*                           datagram packet buffers or multiple, fragmented packet buffers.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetICMP_Rx (NET_BUF  *pbuf,
                  NET_ERR  *perr)
{
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
    NET_CTR       *pctr;
#endif
    NET_BUF_HDR   *pbuf_hdr;
    NET_ICMP_HDR  *picmp_hdr;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit rx (see Note #3).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NetICMP_RxPktDiscard(pbuf, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrNullPtrCtr);
        return;
    }
#endif


    NET_CTR_STAT_INC(Net_StatCtrs.NetICMP_StatRxMsgCtr);


                                                                /* ----------------- VALIDATE ICMP PKT ---------------- */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NetICMP_RxPktValidateBuf(pbuf_hdr, perr);                   /* Validate rx'd buf.                                   */
    switch (*perr) {
        case NET_ICMP_ERR_NONE:                                 
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        default:
             NetICMP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif
    picmp_hdr = (NET_ICMP_HDR *)&pbuf->Data[pbuf_hdr->ICMP_MsgIx];
    NetICMP_RxPktValidate(pbuf, pbuf_hdr, picmp_hdr, perr);     /* Validate rx'd pkt.                                   */


/*$PAGE*/
                                                                /* ------------------ DEMUX ICMP MSG ------------------ */
    switch (*perr) {
        case NET_ICMP_ERR_MSG_TYPE_ERR:
                                                                /* See Note #4a.                                        */

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
             pctr = &Net_StatCtrs.NetICMP_StatRxMsgErrProcessedCtr;
#endif
             break;


        case NET_ICMP_ERR_MSG_TYPE_REQ:
             NetICMP_TxMsgReply(pbuf, pbuf_hdr, picmp_hdr, perr);

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
             pctr = &Net_StatCtrs.NetICMP_StatRxMsgReqProcessedCtr;
#endif
             break;


        case NET_ICMP_ERR_MSG_TYPE_REPLY:
                                                                /* See Note #4b.                                        */

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
             pctr = &Net_StatCtrs.NetICMP_StatRxMsgReplyProcessedCtr;
#endif
             break;


        case NET_ICMP_ERR_INVALID_TYPE:
        case NET_ICMP_ERR_INVALID_CODE:
        case NET_ICMP_ERR_INVALID_PTR:
        case NET_ICMP_ERR_INVALID_LEN:
        case NET_ICMP_ERR_INVALID_LEN_DATA:
        case NET_ICMP_ERR_INVALID_CHK_SUM:
        default:
             NetICMP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* --------- FREE ICMP PKT / UPDATE RX STATS ---------- */
    switch (*perr) {                                            /* Chk err from NetICMP_TxMsg???().                     */
        case NET_ICMP_ERR_NONE:
             NetICMP_RxPktFree(pbuf);
             NET_CTR_STAT_INC(Net_StatCtrs.NetICMP_StatRxMsgProcessedCtr);
             NET_CTR_STAT_INC(*pctr);
             break;


        default:
             NetICMP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetICMP_TxMsgErr()
*
* Description : (1) Transmit ICMP Error Message in response to received packet with one or more errors :
*
*                   (a) Validate ICMP Error Message
*
*                   (b) Get buffer for ICMP Error Message :
*
*                       (1) Calculate  ICMP Error Message buffer size
*                       (2) Copy received packet's IP header & data into ICMP Error Message
*                       (3) Initialize ICMP Error Message buffer controls
*
*                   (c) Prepare ICMP Error Message :
*
*                       (1) Type                           See 'net_icmp.h  ICMP MESSAGE TYPES & CODES'
*                       (2) Code                           See 'net_icmp.h  ICMP MESSAGE TYPES & CODES'
*                       (3) Pointer
*                       (4) Unused
*                       (5) Check-Sum
*
*                   (d) Transmit ICMP Error Message
*
*                       (1) RFC #1122, Section 3.2.2 specifies that "an ICMP error message SHOULD be sent
*                           with normal (i.e., zero) TOS bits".  RFC #1349, Section 5.1 confirms that "an
*                           ICMP error message is always sent with the default TOS (0000)".
*
*                   (e) Free ICMP Error Message buffer
*
*                   (f) Update transmit statistics
*
*
* Argument(s) : pbuf        Pointer to network buffer that received a packet with error(s).
*
*               type        ICMP Error Message type (see Note #1cA) :
*
*                               NET_ICMP_MSG_TYPE_DEST_UNREACH
*                               NET_ICMP_MSG_TYPE_SRC_QUENCH
*                               NET_ICMP_MSG_TYPE_TIME_EXCEED
*                               NET_ICMP_MSG_TYPE_PARAM_PROB
*
*               code        ICMP Error Message code (see Note #1cA).
*
*               ptr         Pointer to received packet's ICMP error (optional).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ICMP_ERR_NONE               ICMP Error Message successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               --- RETURNED BY NetICMP_TxPktDiscard() : ---
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) NetICMP_TxMsgErr() blocked until network initialization completes.
*
*               (3) (a) The following IP header fields MUST be decoded &/or converted from network-order to host-
*                       order BEFORE any ICMP Error Messages are transmitted (see 'net_ip.c  NetIP_RxPktValidate()
*                       Note #3') :
*
*                       (1) Header Length                           
*                       (2) Total  Length
*                       (3) Source      Address
*                       (4) Destination Address
*
*                   (b) The following IP header fields were NOT previously decoded &/or converted from network-
*                       order to host-order & are NOT available :
*
*                       (1) IP Data Length
*
*               (4) Default case already invalidated in NetICMP_TxMsgErrValidate().  However, the default 
*                   case is included as an extra precaution in case 'type' is incorrectly modified.
*
*               (5) Assumes network buffer's protocol header size is large enough to accommodate ICMP header
*                   size (see 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1').
*$PAGE*
*               (6) Some buffer controls were previously initialized in NetBuf_Get() when the buffer
*                   was allocated earlier in this function.  These buffer controls do NOT need to be
*                   re-initialized but are shown for completeness.
*
*               (7) (a) ICMP message Check-Sum MUST be calculated AFTER the entire ICMP message has been
*                       prepared.  In addition, ALL multi-octet words are converted from host-order to 
*                       network-order since "the sum of 16-bit integers can be computed in either byte 
*                       order" [RFC #1071, Section 2.(B)].
*
*                   (b) ICMP message Check-Sum field MUST be cleared to '0' BEFORE the ICMP message Check-Sum
*                       is calculated (see RFC #792, Sections 'Destination Unreachable Message : Checksum',
*                       'Time Exceeded Message : Checksum', 'Source Quench Message : Checksum', & 'Parameter
*                       Problem Message : Checksum').
*
*                   (c) The ICMP message Check-Sum field is returned in network-order & MUST NOT be re-converted 
*                       back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()  Note #3b').
*
*               (8) Network buffer already freed by lower layer; only increment error counter.
*********************************************************************************************************
*/

void  NetICMP_TxMsgErr (NET_BUF     *pbuf,
                        CPU_INT08U   type,
                        CPU_INT08U   code,
                        CPU_INT08U   ptr,
                        NET_ERR     *perr)
{
    NET_BUF_HDR              *pbuf_hdr;
    NET_IP_HDR               *pip_hdr;
    NET_BUF                  *pmsg_err;
    NET_BUF_HDR              *pmsg_err_hdr;
    NET_ICMP_HDR_ERR         *picmp_hdr_err;
    NET_ICMP_HDR_PARAM_PROB  *picmp_hdr_param_prob;
    CPU_INT16U                msg_size_hdr;
    CPU_INT16U                msg_size_data_ip;
    CPU_INT16U                msg_size_data;
    CPU_INT16U                msg_size_tot;
    CPU_INT16U                msg_ix;
    CPU_INT16U                msg_ix_data;
    CPU_INT16U                msg_chk_sum;
    NET_ERR                   err;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit tx (see Note #2).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NetICMP_TxPktDiscard((NET_BUF *)0,
                             (NET_ERR *)perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrNullPtrCtr);
        return;
    }
#endif



                                                                /* ------------- VALIDATE ICMP TX ERR MSG ------------- */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (pbuf_hdr->IP_HdrIx == NET_BUF_IX_NONE) {
        NetICMP_TxPktDiscard((NET_BUF *)0,
                             (NET_ERR *)perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxInvalidBufIxCtr);
        return;
    }
#endif
    pip_hdr = (NET_IP_HDR *)&pbuf->Data[pbuf_hdr->IP_HdrIx];


    NetICMP_TxMsgErrValidate(pbuf, pbuf_hdr, pip_hdr, type, code, ptr, &err);

    if (err != NET_ICMP_ERR_NONE) {
        NetICMP_TxPktDiscard((NET_BUF *)0,
                             (NET_ERR *)perr);
        return;
    }


/*$PAGE*/
                                                                /* --------------- GET ICMP ERR MSG BUF --------------- */
                                                                /* Calc err msg buf size.                               */
    switch (type) {
        case NET_ICMP_MSG_TYPE_DEST_UNREACH:
             msg_size_hdr = NET_ICMP_HDR_SIZE_DEST_UNREACH;
             break;


        case NET_ICMP_MSG_TYPE_SRC_QUENCH:
             msg_size_hdr = NET_ICMP_HDR_SIZE_SRC_QUENCH;
             break;


        case NET_ICMP_MSG_TYPE_TIME_EXCEED:
             msg_size_hdr = NET_ICMP_HDR_SIZE_TIME_EXCEED;
             break;


        case NET_ICMP_MSG_TYPE_PARAM_PROB:
             msg_size_hdr = NET_ICMP_HDR_SIZE_PARAM_PROB;
             break;


        default:                                                /* See Note #4.                                         */
             NetICMP_TxPktDiscard((NET_BUF *)0,
                                  (NET_ERR *)perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


    if (pbuf_hdr->IP_TotLen >= pbuf_hdr->IP_HdrLen) {                       /* If IP tot len > IP hdr len &      ...    */
        msg_size_data_ip = pbuf_hdr->IP_TotLen - pbuf_hdr->IP_HdrLen;       /* Calc IP data len (see Note #3b1).        */
        if (msg_size_data_ip >= NET_ICMP_MSG_ERR_DATA_SIZE_MIN_OCTETS) {    /* ... ip data >= min ICMP data len, ...    */
                                                                            /* ... get max ICMP err msg len.            */
            msg_size_data = pbuf_hdr->IP_HdrLen + NET_ICMP_MSG_ERR_DATA_SIZE_MIN_OCTETS; 
        } else {                                                            /* Else get max IP tot len.                 */
            msg_size_data = pbuf_hdr->IP_TotLen;
        }
    } else {                                                                /* Else get max IP tot len.                 */
        msg_size_data = pbuf_hdr->IP_TotLen;
    }

    msg_size_tot = msg_size_hdr + msg_size_data;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (NET_BUF_DATA_TX_IX < msg_size_hdr) {                    /* See Note #5.                                         */
        NetICMP_TxPktDiscard((NET_BUF *)0,
                             (NET_ERR *)perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxInvalidBufIxCtr);
        return;
    }
#endif
    msg_ix   = NET_BUF_DATA_TX_IX - msg_size_hdr;

                                                                /* Get err msg buf.                                     */
    pmsg_err = NetBuf_Get((NET_BUF_SIZE) msg_size_tot,
                          (NET_BUF_SIZE) msg_ix,
                          (CPU_INT16U  ) NET_BUF_FLAG_NONE,
                          (NET_ERR    *)&err);
    if (err != NET_BUF_ERR_NONE) {
        NetICMP_TxPktDiscard(pmsg_err, perr);
        return;
    }


    msg_ix_data = msg_ix + msg_size_hdr;
    NetBuf_DataWr((NET_BUF    *) pmsg_err,                      /* Copy rx'd IP hdr & data into ICMP err tx buf.        */
                  (NET_BUF_SIZE) msg_ix_data,
                  (NET_BUF_SIZE) msg_size_data,
                  (CPU_INT08U *) pip_hdr,
                  (NET_ERR    *)&err);
    if (err != NET_BUF_ERR_NONE) {
        NetICMP_TxPktDiscard(pmsg_err, perr);
        return;
    }

                                                                /* Init err msg buf ctrls.                              */
    pmsg_err_hdr                  = &pmsg_err->Hdr;
    pmsg_err_hdr->ICMP_MsgIx      = (CPU_INT16U  )msg_ix;
    pmsg_err_hdr->ICMP_MsgLen     = (CPU_INT16U  )msg_size_tot;
    pmsg_err_hdr->ICMP_HdrLen     = (CPU_INT16U  )msg_size_hdr;
    pmsg_err_hdr->TotLen          = (NET_BUF_SIZE)pmsg_err_hdr->ICMP_MsgLen;
    pmsg_err_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_ICMP;
#if 0                                                           /* Init'd in NetBuf_Get() [see Note #6].                */
    pmsg_err_hdr->DataIx          =  NET_BUF_IX_NONE;
    pmsg_err_hdr->DataLen         =  0;
#endif



/*$PAGE*/
                                                                /* --------------- PREPARE ICMP ERR MSG --------------- */
    switch (type) {
        case NET_ICMP_MSG_TYPE_DEST_UNREACH:
        case NET_ICMP_MSG_TYPE_SRC_QUENCH:
        case NET_ICMP_MSG_TYPE_TIME_EXCEED:
             picmp_hdr_err       = (NET_ICMP_HDR_ERR *)&pmsg_err->Data[pmsg_err_hdr->ICMP_MsgIx];
             picmp_hdr_err->Type =  type;
             picmp_hdr_err->Code =  code;
                                                                            /* Clr unused octets.                       */
             Mem_Clr((void     *)picmp_hdr_err->Unused,
                     (CPU_SIZE_T)NET_ICMP_HDR_NBR_OCTETS_UNUSED);
                                                                            /* Calc ICMP msg chk sum (see Note #7).     */
             NET_UTIL_VAL_SET_NET_16(&picmp_hdr_err->ChkSum, 0x0000);       /* Clr           chk sum (see Note #7b).    */
             msg_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc((void     *) picmp_hdr_err,
                                                             (CPU_INT16U) pmsg_err_hdr->ICMP_MsgLen,
                                                             (NET_ERR  *)&err);
             NET_UTIL_VAL_COPY_16(&picmp_hdr_err->ChkSum, &msg_chk_sum);    /* Copy chk sum in net order (see Note #7c).*/
             break;


        case NET_ICMP_MSG_TYPE_PARAM_PROB:
             picmp_hdr_param_prob       = (NET_ICMP_HDR_PARAM_PROB *)&pmsg_err->Data[pmsg_err_hdr->ICMP_MsgIx];
             picmp_hdr_param_prob->Type =  type;
             picmp_hdr_param_prob->Code =  code;
             picmp_hdr_param_prob->Ptr  =  ptr;
                                                                            /* Clr unused octets.                       */
             Mem_Clr((void     *)picmp_hdr_param_prob->Unused,
                     (CPU_SIZE_T)NET_ICMP_HDR_NBR_OCTETS_UNUSED_PARAM_PROB);
                                                                            /* Calc ICMP msg chk sum (see Note #7).     */
             NET_UTIL_VAL_SET_NET_16(&picmp_hdr_param_prob->ChkSum, 0x0000);/* Clr           chk sum (see Note #7b).    */
             msg_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc((void     *) picmp_hdr_param_prob,
                                                             (CPU_INT16U) pmsg_err_hdr->ICMP_MsgLen,
                                                             (NET_ERR  *)&err);
                                                                            /* Copy chk sum in net order (see Note #7c).*/
             NET_UTIL_VAL_COPY_16(&picmp_hdr_param_prob->ChkSum, &msg_chk_sum);
             break;


        default:                                                /* See Note #4.                                         */
             NetICMP_TxPktDiscard(pmsg_err, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

    if (err != NET_UTIL_ERR_NONE) {                             /* Chk err from NetUtil_16BitOnesCplChkSumHdrCalc().    */
        NetICMP_TxPktDiscard(pmsg_err, perr);
        return;
    }



                                                                /* ----------------- TX ICMP ERR MSG ------------------ */
    NetIP_Tx((NET_BUF   *)pmsg_err,
             (NET_IP_ADDR)pbuf_hdr->IP_AddrDest,
             (NET_IP_ADDR)pbuf_hdr->IP_AddrSrc,
             (NET_IP_TOS )NET_IP_TOS_DFLT,                      /* See Note #1d1.                                       */
             (NET_IP_TTL )NET_IP_TTL_DFLT,
             (CPU_INT16U )NET_IP_FLAG_NONE,
             (void      *)0,
             (NET_ERR   *)perr);



                                                                /* ------- FREE ICMP ERR MSG / UPDATE TX STATS -------- */
    switch (*perr) {
        case NET_IP_ERR_NONE:
             NetICMP_TxPktFree(pmsg_err);
             NET_CTR_STAT_INC(Net_StatCtrs.NetICMP_StatTxMsgCtr);
             NET_CTR_STAT_INC(Net_StatCtrs.NetICMP_StatTxMsgErrCtr);
             break;


        case NET_ERR_TX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxPktDiscardedCtr);     /* See Note #8.                             */
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
        
                       
        case NET_ERR_INIT_INCOMPLETE:
        default:
             NetICMP_TxPktDiscard(pmsg_err, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


   *perr = NET_ICMP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetICMP_TxMsgReq()
*
* Description : (1) Transmit ICMP Request Message :
*
*                   (a) Acquire  network lock
*
*                   (b) Validate ICMP Request Message :
*
*                       (1) Validate the following arguments :
*
*                           (A) Type
*                           (B) Code
*                           (C) Source Address
*
*                       (2) Validation of the following arguments deferred to NetIP_Tx() :
*
*                           (A) Type of Service (TOS)
*
*                               (1) RFC #1349, Section 5.1 states that "an ICMP request message may 
*                                   be sent with any value in the TOS field.  A mechanism to allow 
*                                   the user to specify the TOS value to be used would be a useful 
*                                   feature in many applications that generate ICMP request messages".
*
*                           (B) Time-to-Live    (TTL)
*                           (C) Destination Address
*                           (D) IP flags
*                           (E) IP options
*
*                       (3) Validation ignores the following arguments :
*
*                           (A) Data
*                           (B) Data length
*
*                   (c) Get buffer for ICMP Request Message :
*
*                       (1) Calculate      ICMP Request Message buffer size
*                       (2) Copy data into ICMP Request Message
*                       (3) Initialize     ICMP Request Message buffer controls
*
*                   (d) Prepare ICMP Request Message :
*
*                       (1) Type                               See 'net_icmp.h  ICMP MESSAGE TYPES & CODES'
*                       (2) Code                               See 'net_icmp.h  ICMP MESSAGE TYPES & CODES'
*                       (3) Identification (ID)
*                       (4) Sequence Number
*                       (5) Data
*                       (6) Timestamps
*                           (A) Timestamp Request Message
*                               (1) "The Originate Timestamp is the time the sender last touched the message
*                                    before sending it" (RFC #792, Section 'Timestamp or Timestamp Reply Message :
*                                    Description').
*
*                   (e) Transmit ICMP Request Message
*                   (f) Free     ICMP Request Message buffer
*                   (g) Update   ICMP transmit statistics
*                   (h) Release  network lock
*                   (i) Return   ICMP Request Message Identification & Sequence Number
*                         OR
*                       NULL id & sequence number structure, on failure
*
*
*$PAGE*
* Argument(s) : type        ICMP Request Message type (see Note #1d1) :
*
*                               NET_ICMP_MSG_TYPE_ECHO_REQ
*                               NET_ICMP_MSG_TYPE_TS_REQ
*                               NET_ICMP_MSG_TYPE_ADDR_MASK_REQ
*
*               code        ICMP Request Message code (see Note #1d2).
*
*               id          ICMP Request Message id (see 'net_os.c  NetOS_ICMP_TxMsgReq()  Note #3b').
*
*               addr_src    Source      IP address.
*
*               addr_dest   Destination IP address.
*
*               TOS         Specific TOS to transmit ICMP/IP packet (see Notes #1b2A
*                                  & 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
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
*                               NET_ICMP_ERR_NONE               ICMP Request Message successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               ---- RETURNED BY NetICMP_TxPktDiscard() : ----
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
*                                                               --------- RETURNED BY NetOS_Lock() : ---------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : ICMP Request Message's Identification (ID) & Sequence Numbers, if NO errors.
*
*               NULL                   Identification (ID) & Sequence Numbers, otherwise.
*
* Caller(s)   : NetOS_ICMP_TxMsgReq().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) NetICMP_TxMsgReq() blocked until network initialization completes.
*
*               (3) NetICMP_TxMsgReq() is the internal ICMP handler for ICMP Request Messages.  Its
*                   global declaration is required since NetOS_ICMP_TxMsgReq() calls the handler function 
*                   from the OS port file (see also 'net_os.c  NetOS_ICMP_TxMsgReq()  Note #1').
*
*               (4) Default case already invalidated in NetICMP_TxMsgReqValidate().  However, the default 
*                   case is included as an extra precaution in case 'type' is incorrectly modified.
*
*               (5) Assumes network buffer's protocol header size is large enough to accommodate ICMP header
*                   size (see 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1').
*
*               (6) Some buffer controls were previously initialized in NetBuf_Get() when the buffer
*                   was allocated earlier in this function.  These buffer controls do NOT need to be
*                   re-initialized but are shown for completeness.
*
*               (7) (a) ICMP message Check-Sum MUST be calculated AFTER the entire ICMP message has been
*                       prepared.  In addition, ALL multi-octet words are converted from host-order to 
*                       network-order since "the sum of 16-bit integers can be computed in either byte 
*                       order" [RFC #1071, Section 2.(B)].
*
*                   (b) ICMP message Check-Sum field MUST be cleared to '0' BEFORE the ICMP message
*                       Check-Sum is calculated (see RFC #792, Sections 'Echo or Echo Reply Message :
*                       Checksum', 'Timestamp or Timestamp Reply Message : Checksum'; & RFC #950,
*                       Appendix I 'Address Mask ICMP', Section 'ICMP Fields : Checksum').
*
*                   (c) The ICMP message Check-Sum field is returned in network-order & MUST NOT be re-
*                       converted back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()
*                       Note #3b' & 'net_util.c  NetUtil_16BitOnesCplChkSumDataCalc()  Note #4b').
*
*               (8) Network buffer already freed by lower layer; only increment error counter.
*********************************************************************************************************
*/
/*$PAGE*/
NET_ICMP_REQ_ID_SEQ  NetICMP_TxMsgReq (CPU_INT08U    type,
                                       CPU_INT08U    code,
                                       CPU_INT16U    id,
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
    NET_ICMP_REQ_ID_SEQ      id_seq;
    CPU_BOOLEAN              msg_addr_valid;
    CPU_INT16U               msg_size_hdr;
    CPU_INT16U               msg_size_data;
    CPU_INT16U               msg_size_tot;
    CPU_INT16U               msg_seq_nbr;
    CPU_INT16U               msg_ix;
    CPU_INT16U               msg_ix_data;
    CPU_INT16U               msg_chk_sum;
    NET_TS                   ts;
    NET_BUF                 *pmsg_req;
    NET_BUF_HDR             *pmsg_req_hdr;
    NET_ICMP_HDR_ECHO       *picmp_hdr_echo;
    NET_ICMP_HDR_TS         *picmp_hdr_ts;
    NET_ICMP_HDR_ADDR_MASK  *picmp_hdr_addr;
    NET_ERR                  err;

                                                                /* Prepare err rtn val.                                 */
    id_seq.ID     = 0;
    id_seq.SeqNbr = 0;


                                                                /* ----------------- ACQUIRE NET LOCK ----------------- */
    NetOS_Lock(perr);
    if (*perr != NET_OS_ERR_NONE) {
         return (id_seq);
    }


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit tx (see Note #2).         */
        NetOS_Unlock();
       *perr =  NET_ERR_INIT_INCOMPLETE;
        return (id_seq);
    }
#endif


                                                                /* ------------- VALIDATE ICMP TX REQ MSG ------------- */
    NetICMP_TxMsgReqValidate(type, code, &err);

    if (err != NET_ICMP_ERR_NONE) {
        NetICMP_TxPktDiscard((NET_BUF *)0,
                             (NET_ERR *)perr);
        NetOS_Unlock();
        return (id_seq);
    }


                                                                /* -------- VALIDATE ICMP TX REQ MSG SRC ADDR --------- */
    msg_addr_valid = NetIP_IsAddrHostHandler(addr_src);
    if (msg_addr_valid != DEF_YES) {
        NetICMP_TxPktDiscard((NET_BUF *)0,
                             (NET_ERR *)perr);
        NetOS_Unlock();
        return (id_seq);
    }


/*$PAGE*/
                                                                /* --------------- GET ICMP REQ MSG BUF --------------- */
                                                                /* Calc req msg buf size.                               */
    switch (type) {
        case NET_ICMP_MSG_TYPE_ECHO_REQ:
             msg_size_hdr = NET_ICMP_HDR_SIZE_ECHO;

             if (p_data != (void *)0) {
                 msg_size_data = data_len;
             } else {
                 msg_size_data = 0;
             }
             break;


        case NET_ICMP_MSG_TYPE_TS_REQ:
#if 1                                                           /* ???? Rtn err OR ignore data?                         */
             if (p_data != (void *)0) {
                 NetICMP_TxPktDiscard((NET_BUF *)0,
                                      (NET_ERR *)perr);
                 NetOS_Unlock();
                 return (id_seq);
             }
             if (data_len > 0) {
                 NetICMP_TxPktDiscard((NET_BUF *)0,
                                      (NET_ERR *)perr);
                 NetOS_Unlock();
                 return (id_seq);
             }
#endif

             msg_size_hdr  = NET_ICMP_HDR_SIZE_TS;
             msg_size_data = 0;
             break;


        case NET_ICMP_MSG_TYPE_ADDR_MASK_REQ:
#if 1                                                           /* ???? Rtn err OR ignore data?                         */
             if (p_data != (void *)0) {
                 NetICMP_TxPktDiscard((NET_BUF *)0,
                                      (NET_ERR *)perr);
                 NetOS_Unlock();
                 return (id_seq);
             }
             if (data_len > 0) {
                 NetICMP_TxPktDiscard((NET_BUF *)0,
                                      (NET_ERR *)perr);
                 NetOS_Unlock();
                 return (id_seq);
             }
#endif

             msg_size_hdr  = NET_ICMP_HDR_SIZE_ADDR_MASK;
             msg_size_data = 0;
             break;


        default:                                                /* See Note #4.                                         */
             NetICMP_TxPktDiscard((NET_BUF *)0,
                                  (NET_ERR *)perr);
             NetOS_Unlock();
             return (id_seq);                                   /* Prevent 'break NOT reachable' compiler warning.      */
    }


    msg_size_tot = msg_size_hdr + msg_size_data;
/*$PAGE*/


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (NET_BUF_DATA_TX_IX < msg_size_hdr) {                    /* See Note #5.                                         */
        NetICMP_TxPktDiscard((NET_BUF *)0,
                             (NET_ERR *)perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxInvalidBufIxCtr);
        NetOS_Unlock();
        return (id_seq);
    }
#endif
    msg_ix   = NET_BUF_DATA_TX_IX - msg_size_hdr;

                                                                /* Get req msg buf.                                     */
    pmsg_req = NetBuf_Get((NET_BUF_SIZE) msg_size_tot,
                          (NET_BUF_SIZE) msg_ix,
                          (CPU_INT16U  ) NET_BUF_FLAG_NONE,
                          (NET_ERR    *)&err);
    if (err != NET_BUF_ERR_NONE) {
        NetICMP_TxPktDiscard(pmsg_req, perr);
        NetOS_Unlock();
        return (id_seq);
    }


                                                                /* Init req msg buf ctrls.                              */
    pmsg_req_hdr                  = &pmsg_req->Hdr;
    pmsg_req_hdr->ICMP_MsgIx      = (CPU_INT16U  )msg_ix;
    pmsg_req_hdr->ICMP_MsgLen     = (CPU_INT16U  )msg_size_tot;
    pmsg_req_hdr->ICMP_HdrLen     = (CPU_INT16U  )msg_size_hdr;
    pmsg_req_hdr->TotLen          = (NET_BUF_SIZE)pmsg_req_hdr->ICMP_MsgLen;
    pmsg_req_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_ICMP;
    msg_ix_data                   =  msg_ix + msg_size_hdr;


    if (msg_size_data > 0) {                                    /* Copy data into ICMP req tx buf.                      */
        NetBuf_DataWr((NET_BUF    *) pmsg_req,
                      (NET_BUF_SIZE) msg_ix_data,
                      (NET_BUF_SIZE) msg_size_data,
                      (CPU_INT08U *) p_data,
                      (NET_ERR    *)&err);
        if (err != NET_BUF_ERR_NONE) {
            NetICMP_TxPktDiscard(pmsg_req, perr);
            NetOS_Unlock();
            return (id_seq);
        }

        pmsg_req_hdr->DataIx  = (CPU_INT16U  )msg_ix_data;
        pmsg_req_hdr->DataLen = (NET_BUF_SIZE)msg_size_data;
#if 0                                                           /* Init'd in NetBuf_Get() [see Note #6].                */
    } else {
        pmsg_req_hdr->DataIx  =  NET_BUF_IX_NONE;
        pmsg_req_hdr->DataLen =  0;
#endif
    }


/*$PAGE*/
                                                                /* --------------- PREPARE ICMP REQ MSG --------------- */
    NET_ICMP_TX_GET_SEQ_NBR(msg_seq_nbr);

    switch (type) {
        case NET_ICMP_MSG_TYPE_ECHO_REQ:
             picmp_hdr_echo       = (NET_ICMP_HDR_ECHO *)&pmsg_req->Data[pmsg_req_hdr->ICMP_MsgIx];
             picmp_hdr_echo->Type =  NET_ICMP_MSG_TYPE_ECHO_REQ;
             picmp_hdr_echo->Code =  NET_ICMP_MSG_CODE_ECHO_REQ;
             NET_UTIL_VAL_COPY_SET_NET_16(&picmp_hdr_echo->ID,     &id);
             NET_UTIL_VAL_COPY_SET_NET_16(&picmp_hdr_echo->SeqNbr, &msg_seq_nbr);
                                                                            /* Calc ICMP msg chk sum (see Note #7).     */
             NET_UTIL_VAL_SET_NET_16(&picmp_hdr_echo->ChkSum, 0x0000);      /* Clr           chk sum (see Note #7b).    */
             msg_chk_sum = NetUtil_16BitOnesCplChkSumDataCalc((void     *) pmsg_req,
                                                              (void     *) 0,
                                                              (CPU_INT16U) 0,
                                                              (NET_ERR  *)&err);
             NET_UTIL_VAL_COPY_16(&picmp_hdr_echo->ChkSum, &msg_chk_sum);   /* Copy chk sum in net order (see Note #7c).*/
             break;


        case NET_ICMP_MSG_TYPE_TS_REQ:
             picmp_hdr_ts       = (NET_ICMP_HDR_TS   *)&pmsg_req->Data[pmsg_req_hdr->ICMP_MsgIx];
             picmp_hdr_ts->Type =  NET_ICMP_MSG_TYPE_TS_REQ;
             picmp_hdr_ts->Code =  NET_ICMP_MSG_CODE_TS_REQ;
             ts                 =  NetUtil_TS_Get();                        /* See Note #1d6A1.                         */
             NET_UTIL_VAL_COPY_SET_NET_16(&picmp_hdr_ts->ID,           &id);
             NET_UTIL_VAL_COPY_SET_NET_16(&picmp_hdr_ts->SeqNbr,       &msg_seq_nbr);
             NET_UTIL_VAL_COPY_SET_NET_32(&picmp_hdr_ts->TS_Originate, &ts);
             NET_UTIL_VAL_SET_NET_32(&picmp_hdr_ts->TS_Rx,  NET_TS_NONE);
             NET_UTIL_VAL_SET_NET_32(&picmp_hdr_ts->TS_Tx,  NET_TS_NONE);
                                                                            /* Calc ICMP msg chk sum (see Note #7).     */
             NET_UTIL_VAL_SET_NET_16(&picmp_hdr_ts->ChkSum, 0x0000);        /* Clr           chk sum (see Note #7b).    */
             msg_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc((void     *) picmp_hdr_ts,
                                                             (CPU_INT16U) pmsg_req_hdr->ICMP_MsgLen,
                                                             (NET_ERR  *)&err);
             NET_UTIL_VAL_COPY_16(&picmp_hdr_ts->ChkSum, &msg_chk_sum);     /* Copy chk sum in net order (see Note #7c).*/
             break;


        case NET_ICMP_MSG_TYPE_ADDR_MASK_REQ:
             picmp_hdr_addr       = (NET_ICMP_HDR_ADDR_MASK *)&pmsg_req->Data[pmsg_req_hdr->ICMP_MsgIx];
             picmp_hdr_addr->Type =  NET_ICMP_MSG_TYPE_ADDR_MASK_REQ;
             picmp_hdr_addr->Code =  NET_ICMP_MSG_CODE_ADDR_MASK_REQ;
             NET_UTIL_VAL_COPY_SET_NET_16(&picmp_hdr_addr->ID,     &id);
             NET_UTIL_VAL_COPY_SET_NET_16(&picmp_hdr_addr->SeqNbr, &msg_seq_nbr);
             NET_UTIL_VAL_SET_NET_32(&picmp_hdr_addr->AddrMask, NET_IP_ADDR_NONE);
                                                                            /* Calc ICMP msg chk sum (see Note #7).     */
             NET_UTIL_VAL_SET_NET_16(&picmp_hdr_addr->ChkSum,   0x0000);    /* Clr           chk sum (see Note #7b).    */
             msg_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc((void     *) picmp_hdr_addr,
                                                             (CPU_INT16U) pmsg_req_hdr->ICMP_MsgLen,
                                                             (NET_ERR  *)&err);
             NET_UTIL_VAL_COPY_16(&picmp_hdr_addr->ChkSum, &msg_chk_sum);   /* Copy chk sum in net order (see Note #7c).*/
             break;


        default:                                                /* See Note #4.                                         */
             NetICMP_TxPktDiscard(pmsg_req, perr);
             NetOS_Unlock();
             return (id_seq);                                   /* Prevent 'break NOT reachable' compiler warning.      */
    }

    if (err != NET_UTIL_ERR_NONE) {                             /* Chk err from NetUtil_16BitOnesCplChkSum()'s.         */
        NetICMP_TxPktDiscard(pmsg_req, perr);
        NetOS_Unlock();
        return (id_seq);
    }



/*$PAGE*/
                                                                /* ----------------- TX ICMP REQ MSG ------------------ */
    NetIP_Tx((NET_BUF   *)pmsg_req,
             (NET_IP_ADDR)addr_src,
             (NET_IP_ADDR)addr_dest,
             (NET_IP_TOS )TOS,
             (NET_IP_TTL )TTL,
             (CPU_INT16U )flags,
             (void      *)popts,
             (NET_ERR   *)perr);



                                                                /* ------- FREE ICMP REQ MSG / UPDATE TX STATS -------- */
    switch (*perr) {
        case NET_IP_ERR_NONE:
             NetICMP_TxPktFree(pmsg_req);
             NET_CTR_STAT_INC(Net_StatCtrs.NetICMP_StatTxMsgCtr);
             NET_CTR_STAT_INC(Net_StatCtrs.NetICMP_StatTxMsgReqCtr);
             break;


        case NET_ERR_TX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxPktDiscardedCtr);     /* See Note #8.                             */
             NetOS_Unlock();
             return (id_seq);                                   /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_ERR_INIT_INCOMPLETE:
        default:
             NetICMP_TxPktDiscard(pmsg_req, perr);
             NetOS_Unlock();
             return (id_seq);                                   /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ----------------- RELEASE NET LOCK ----------------- */
    NetOS_Unlock();


                                                                /* ----------- RTN ICMP REQ MSG ID/SEQ NBR ------------ */
    id_seq.ID     = id;
    id_seq.SeqNbr = msg_seq_nbr;

   *perr          = NET_ICMP_ERR_NONE;

    return (id_seq);                                            
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetICMP_HandlerRx()
*
* Description : Handle & reply to incoming IP packets for ICMP error &/or status conditions.
*
*               (1) (a) When network low-resources status is set, transmit  ICMP Source Quench Error Messages.
*
*                   (b) When network low-resources status clears, clear the ICMP Transmit Source Quench List.
*
*
* Argument(s) : pbuf        Pointer to network buffer that received an IP packet.
*               ----        Argument validated in NetIP_RxPktValidate()
*                                                   by NetIP_Rx().
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_RxPktValidate().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetICMP_HandlerRx (NET_BUF  *pbuf)
{
#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
    NET_DBG_STATUS  rsrc_lo;


    rsrc_lo = NetDbg_MonTaskStatGetHandlerLo();                 /* Get lo rsrc's status.                                */

    if (rsrc_lo != NET_DBG_STATUS_OK) {                         /* If net rsrc's lo, ...                                */
        NetICMP_HandlerTxSrcQuench(pbuf);                       /* ... tx ICMP src quench err to IP pkt src host.       */

    } else {                                                    /* Else if ICMP tx src quench list avail, ...           */
        if (NetICMP_TxSrcQuenchListHead != (NET_ICMP_TX_SRC_QUENCH *)0) {
            NetICMP_HandlerTxSrcQuenchClrList();                /* ... clr tx src quench list.                          */
        }
    }

#else
   (void)&pbuf;                                                 /* Prevent compiler warning.                            */
#endif
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
*                                     NetICMP_RxPktValidateBuf()
*
* Description : Validate received buffer header as ICMP protocol.
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header that received ICMP packet.
*               --------    Argument validated in NetICMP_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ICMP_ERR_NONE               Received buffer's ICMP header validated.
*                               NET_ERR_INVALID_PROTOCOL        Buffer's protocol type is NOT ICMP.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetICMP_RxPktValidateBuf (NET_BUF_HDR  *pbuf_hdr,
                                        NET_ERR      *perr)
{
                                                                /* --------------- VALIDATE ICMP BUF HDR -------------- */
    if (pbuf_hdr->ProtocolHdrType != NET_PROTOCOL_TYPE_ICMP) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxInvalidProtocolCtr);
       *perr = NET_ERR_INVALID_PROTOCOL;
        return;
    }

    if (pbuf_hdr->ICMP_MsgIx == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

   *perr = NET_ICMP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetICMP_RxPktValidate()
*
* Description : (1) Validate received ICMP packet :
*
*                   (a) Validate the received packet's destination                  See Note #3
*
*                   (b) (1) Validate the received packet's following ICMP header fields :
*
*                           (A) Type
*                           (B) Code
*                           (C) Message Length
*                           (D) Pointer                         See RFC  #792, Section 'Parameter Problem Message'
*                           (E) Check-Sum                       See Note #7a
*
*                       (2) Validation ignores the following ICMP header fields :
*
*                           (A) Unused                          See RFC # 792, Section 'Message Formats'
*                           (B) Data                            See RFC #1122, Sections 3.2.2 & 3.2.2.6
*                           (C) Identification (ID)             See RFC # 792, Sections 'Echo or Echo Reply Message'
*                                                                   & 'Timestamp or Timestamp Reply Message'
*                           (D) Sequence Number                 See RFC # 792, Sections 'Echo or Echo Reply Message'
*                                                                   & 'Timestamp or Timestamp Reply Message'
*
*                   (c) Convert the following ICMP message fields from network-order to host-order :
*
*                       (1) Check-Sum                                               See Note #7c
*
*                           (A) These fields are NOT converted directly in the received packet buffer's 
*                               data area but are converted in local or network buffer variables ONLY.
*
*                   (d) Update network buffer's length controls
*
*                   (e) Demultiplex ICMP message type
*
*
* Argument(s) : pbuf        Pointer to network buffer that received ICMP packet.
*               ----        Argument checked   in NetICMP_Rx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetICMP_Rx().
*
*               picmp_hdr   Pointer to received packet's ICMP header.
*               ---------   Argument validated in NetICMP_Rx()/NetICMP_RxPktValidateBuf().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ICMP_ERR_MSG_TYPE_ERR       Received packet validated as ICMP Error   Message.
*                               NET_ICMP_ERR_MSG_TYPE_REQ       Received packet validated as ICMP Request Message.
*                               NET_ICMP_ERR_MSG_TYPE_REPLY     Received packet validated as ICMP Reply   Message.
*
*                               NET_ICMP_ERR_RX_BROADCAST       Packet received via broadcast.
*
*                               NET_ICMP_ERR_INVALID_TYPE       Invalid/unknown ICMP message type.
*                               NET_ICMP_ERR_INVALID_CODE       Invalid/unknown ICMP message code.
*                               NET_ICMP_ERR_INVALID_PTR        Invalid ICMP message pointer outside ICMP message.
*                               NET_ICMP_ERR_INVALID_LEN        Invalid ICMP message      length.
*                               NET_ICMP_ERR_INVALID_LEN_DATA   Invalid ICMP message data length.
*                               NET_ICMP_ERR_INVALID_CHK_SUM    Invalid ICMP check-sum.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_Rx().
*
* Note(s)     : (2) See 'net_icmp.h  ICMP HEADER' for ICMP header format.
*
*               (3) (a) RFC #1122, Sections 3.2.2.6 & 3.2.2.8 state that "an ICMP ... Request destined to an
*                       IP broadcast ... address MAY be silently discarded".
*
*                       However, NO RFC specifies how OTHER ICMP messages should handle messages received by 
*                       broadcast.  ???? Therefore, it is assumed that ALL ICMP messages destined to an IP 
*                       broadcast address SHOULD be silently discarded.
*
*                   (b) Since a packet destined to a valid IP broadcast address MUST also have been received as
*                       a link-layer broadcast (see RFC #1122, Section 3.3.6 & 'net_ip.c  NetIP_RxPktValidate()
*                       Note #9d2B1b'), the determination of an IP broadcast destination address need only verify
*                       that the received packet was received as a link-layer broadcast packet.
*
*               (4) (a) RFC #1122, Section 3.2.2 requires that ICMP messages with the following invalid ICMP
*                       header fields be "silently discarded" :
*
*                       (1) Type                                    RFC #1122, Section 3.2.2
*
*                   (b) Assumes that ICMP messages with the following invalid ICMP header fields should also
*                       be "silently discarded" :
*
*                       (1) Code
*                       (2) Message Length
*                       (3) Pointer
*                       (4) Check-Sum
*$PAGE*
*               (5) See 'net_icmp.h  ICMP MESSAGE TYPES & CODES' for supported ICMP message types/codes.
*
*               (6) Since ICMP message headers do NOT contain a message length field, the ICMP Message Length
*                   is assumed to be the remaining IP Datagram Length.
*
*               (7) (a) ICMP message Check-Sum field MUST be validated AFTER (or BEFORE) any multi-octet
*                       words are converted from network-order to host-order since "the sum of 16-bit
*                       integers can be computed in either byte order" [RFC #1071, Section 2.(B)].
*
*                       In other words, the ICMP message Check-Sum CANNOT be validated AFTER SOME but NOT
*                       ALL multi-octet words have been converted from network-order to host-order.
*
*                       However, ALL received packets' multi-octet words are converted in local or network 
*                       buffer variables ONLY (see Note #1cA).  Therefore, ICMP message Check-Sum may be 
*                       validated at any point.
*
*                   (b) RFC #792, Section 'Echo or Echo Reply Message : Checksum' specifies that "if the
*                       total length is odd, the received data is padded with one octet of zeros for
*                       computing the checksum".
*
*                       However, NO RFC specifies how OTHER ICMP message types should handle odd-length 
*                       check-sums.  Therefore, it is assumed that ICMP Echo Request & Echo Reply Messages
*                       should handle odd-length check-sums according to RFC #792's 'Echo or Echo Reply
*                       Message : Checksum' specification, while ALL other ICMP message types should handle
*                       odd-length check-sums according to RFC #1071, Section 4.1.
*
*                       See also 'net_util.c  NetUtil_16BitSumDataCalc()  Note #8'.
*
*                   (c) After the ICMP message Check-Sum is validated, it is NOT necessary to convert the
*                       Check-Sum from network-order to host-order since it is NOT required for further
*                       processing.
*
*               (8) Default case already invalidated earlier in this function.  However, the default case
*                   is included as an extra precaution in case 'Type' is incorrectly modified.
*
*               (9) (a) (1) Except for ICMP Echo & Echo Reply Messages (see Note #9b), most ICMP messages do
*                           NOT permit user &/or application data (see RFC #792 & RFC #1122, Sections 3.2.2).
*
*                       (2) Most ICMP messages that do NOT contain user &/or application data will NOT be
*                           received in separate packet buffers since most ICMP messages are NOT large enough
*                           to be fragmented since the minimum network buffer size MUST be configured such
*                           that most ICMP messages fit within a single packet buffer (see 'net_buf.h
*                           NETWORK BUFFER INDEX & SIZE DEFINES  Note #1d').
*
*                           However, RFC #1122, Section 3.2.2 states that "every ICMP error message includes
*                           the Internet header and at least the first 8 data octets of the datagram that 
*                           triggered the error; more than 8 octets MAY be sent".  Thus, it is possible that 
*                           some received ICMP error messages MAY contain more than 8 octets of the Internet
*                           header & may therefore be received in one or more fragmented packet buffers.
*
*                           Furthermore, these additional error message octets SHOULD NOT contain user &/or
*                           application data.
*
*                       (3) ICMP data index value to clear was previously initialized in NetBuf_Get() when 
*                           the buffer was allocated.  This index value does NOT need to be re-cleared but 
*                           is shown for completeness.
*
*                   (b) (1) ICMP Echo & Echo Reply Messages permit the transmission & receipt of user &/or
*                           application data (see RFC #792, Section 'Echo or Echo Reply Message' & RFC #1122,
*                           Section 3.2.2.6).
*
*                           Since the minimum network buffer size MUST be configured such that the entire
*                           ICMP Echo Message header MUST be received in a single packet (see 'net_buf.h
*                           NETWORK BUFFER INDEX & SIZE DEFINES  Note #1d'), after the ICMP Echo Message 
*                           header size is decremented from the first packet buffer's remaining number of 
*                           data octets, any remaining octets MUST be user &/or application data octets.
*
*                           (A) Note that the 'Data' index is updated regardless of a null-size data length.
*
*                       (2) If additional packet buffers exist, the remaining IP datagram 'Data' MUST be
*                           user &/or application data.  Therefore, the 'Data' length does NOT need to
*                           be adjusted but the 'Data' index MUST be updated.
*
*                   (c) #### Total ICMP Message Length is duplicated in ALL fragmented packet buffers
*                       (may NOT be necessary; remove if unnecessary).
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetICMP_RxPktValidate (NET_BUF       *pbuf,
                                     NET_BUF_HDR   *pbuf_hdr,
                                     NET_ICMP_HDR  *picmp_hdr,
                                     NET_ERR       *perr)
{
    CPU_BOOLEAN               rx_broadcast;
    CPU_BOOLEAN               icmp_chk_sum_valid;
    CPU_INT16U                icmp_msg_len_hdr;
    CPU_INT16U                icmp_msg_len_min;
    CPU_INT16U                icmp_msg_len_max;
    CPU_INT16U                icmp_msg_len;
    NET_ICMP_HDR_PARAM_PROB  *picmp_param_prob;
    NET_BUF                  *pbuf_next;
    NET_BUF_HDR              *pbuf_next_hdr;


                                                                /* -------------- VALIDATE ICMP RX DEST --------------- */
    rx_broadcast = DEF_BIT_IS_SET(pbuf_hdr->Flags, NET_BUF_FLAG_RX_BROADCAST);
    if (rx_broadcast != DEF_NO) {                               /* If ICMP rx'd as broadcast, rtn err (see Note #3).    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxBroadcastCtr);
       *perr = NET_ICMP_ERR_RX_BROADCAST;
        return;
    }



                                                                /* ------------- VALIDATE ICMP TYPE/CODE -------------- */
    switch (picmp_hdr->Type) {                                  /* See Notes #4a1 & #4b1.                               */
        case NET_ICMP_MSG_TYPE_DEST_UNREACH:
             switch (picmp_hdr->Code) {
                 case NET_ICMP_MSG_CODE_DEST_NET:
                 case NET_ICMP_MSG_CODE_DEST_HOST:
                 case NET_ICMP_MSG_CODE_DEST_PROTOCOL:
                 case NET_ICMP_MSG_CODE_DEST_PORT:
                 case NET_ICMP_MSG_CODE_DEST_FRAG_NEEDED:
                 case NET_ICMP_MSG_CODE_DEST_ROUTE_FAIL:
                 case NET_ICMP_MSG_CODE_DEST_NET_UNKNOWN:
                 case NET_ICMP_MSG_CODE_DEST_HOST_UNKNOWN:
                 case NET_ICMP_MSG_CODE_DEST_HOST_ISOLATED:
                 case NET_ICMP_MSG_CODE_DEST_NET_TOS:
                 case NET_ICMP_MSG_CODE_DEST_HOST_TOS:
                      icmp_msg_len_hdr = NET_ICMP_HDR_SIZE_DEST_UNREACH;
                      icmp_msg_len_min = NET_ICMP_MSG_LEN_MIN_DEST_UNREACH;
                      icmp_msg_len_max = NET_ICMP_MSG_LEN_MAX_DEST_UNREACH;
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_ICMP_MSG_TYPE_SRC_QUENCH:
             switch (picmp_hdr->Code) {
                 case NET_ICMP_MSG_CODE_SRC_QUENCH:
                      icmp_msg_len_hdr = NET_ICMP_HDR_SIZE_SRC_QUENCH;
                      icmp_msg_len_min = NET_ICMP_MSG_LEN_MIN_SRC_QUENCH;
                      icmp_msg_len_max = NET_ICMP_MSG_LEN_MAX_SRC_QUENCH;
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_ICMP_MSG_TYPE_TIME_EXCEED:
             switch (picmp_hdr->Code) {
                 case NET_ICMP_MSG_CODE_TIME_EXCEED_TTL:
                 case NET_ICMP_MSG_CODE_TIME_EXCEED_FRAG_REASM:
                      icmp_msg_len_hdr = NET_ICMP_HDR_SIZE_TIME_EXCEED;
                      icmp_msg_len_min = NET_ICMP_MSG_LEN_MIN_TIME_EXCEED;
                      icmp_msg_len_max = NET_ICMP_MSG_LEN_MAX_TIME_EXCEED;
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;

/*$PAGE*/
        case NET_ICMP_MSG_TYPE_PARAM_PROB:
             switch (picmp_hdr->Code) {
                 case NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR:
                 case NET_ICMP_MSG_CODE_PARAM_PROB_OPT_MISSING:
                      icmp_msg_len_hdr = NET_ICMP_HDR_SIZE_PARAM_PROB;
                      icmp_msg_len_min = NET_ICMP_MSG_LEN_MIN_PARAM_PROB;
                      icmp_msg_len_max = NET_ICMP_MSG_LEN_MAX_PARAM_PROB;

                      picmp_param_prob = (NET_ICMP_HDR_PARAM_PROB *)picmp_hdr;
                      if (picmp_param_prob->Ptr < NET_ICMP_MSG_PTR_MIN_PARAM_PROB) {    /* If ptr val < min ptr val, .. */
                          NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrPtrCtr);
                         *perr = NET_ICMP_ERR_INVALID_PTR;                              /* ... rtn err (see Note #4b3). */
                          return;
                      }
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_ICMP_MSG_TYPE_ECHO_REQ:
        case NET_ICMP_MSG_TYPE_ECHO_REPLY:
             switch (picmp_hdr->Code) {
                 case NET_ICMP_MSG_CODE_ECHO:
                      icmp_msg_len_hdr = NET_ICMP_HDR_SIZE_ECHO;
                      icmp_msg_len_min = NET_ICMP_MSG_LEN_MIN_ECHO;
                      icmp_msg_len_max = NET_ICMP_MSG_LEN_MAX_ECHO;
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_ICMP_MSG_TYPE_TS_REQ:
        case NET_ICMP_MSG_TYPE_TS_REPLY:
             switch (picmp_hdr->Code) {
                 case NET_ICMP_MSG_CODE_TS:
                      icmp_msg_len_hdr = NET_ICMP_HDR_SIZE_TS;
                      icmp_msg_len_min = NET_ICMP_MSG_LEN_MIN_TS;
                      icmp_msg_len_max = NET_ICMP_MSG_LEN_MAX_TS;
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;

/*$PAGE*/
        case NET_ICMP_MSG_TYPE_ADDR_MASK_REPLY:
             switch (picmp_hdr->Code) {
                 case NET_ICMP_MSG_CODE_ADDR_MASK:
                      icmp_msg_len_hdr = NET_ICMP_HDR_SIZE_ADDR_MASK;
                      icmp_msg_len_min = NET_ICMP_MSG_LEN_MIN_ADDR_MASK;
                      icmp_msg_len_max = NET_ICMP_MSG_LEN_MAX_ADDR_MASK;
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;

#if 0                                                           /* ---------------- UNSUPPORTED TYPES ----------------- */
                                                                /* See Note #5.                                         */
        case NET_ICMP_MSG_TYPE_REDIRECT:
        case NET_ICMP_MSG_TYPE_ROUTE_REQ:
        case NET_ICMP_MSG_TYPE_ROUTE_AD:
        case NET_ICMP_MSG_TYPE_ADDR_MASK_REQ:
                                                                /* 'break' intentionally omitted; do NOT move from the  */
                                                                /* ... following case : 'default'.                      */
#endif

        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrTypeCtr);
            *perr = NET_ICMP_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                /* -------------- VALIDATE ICMP MSG LEN --------------- */
    icmp_msg_len          = pbuf_hdr->IP_DatagramLen;           /* See Note #6.                                         */
    pbuf_hdr->ICMP_MsgLen = icmp_msg_len;
    if (pbuf_hdr->ICMP_MsgLen < icmp_msg_len_min) {             /* If msg len < min msg len, rtn err.                   */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrMsgLenCtr);
       *perr = NET_ICMP_ERR_INVALID_LEN;
        return;
    }
    if (icmp_msg_len_max != NET_ICMP_MSG_LEN_MAX_NONE) {
        if (pbuf_hdr->ICMP_MsgLen > icmp_msg_len_max) {         /* If msg len > max msg len, rtn err.                   */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrMsgLenCtr);
           *perr = NET_ICMP_ERR_INVALID_LEN;
            return;
        }
    }

    if (picmp_hdr->Type == NET_ICMP_MSG_TYPE_PARAM_PROB) {      /* For ICMP Param Prob msg,   ...                       */
        if (picmp_param_prob->Ptr >= pbuf_hdr->ICMP_MsgLen) {   /* ... if ptr val >= msg len, ...                       */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrPtrCtr);
           *perr = NET_ICMP_ERR_INVALID_PTR;                    /* ... rtn err (see Note #4b3).                         */
            return;
        }
    }


                                                                /* -------------- VALIDATE ICMP CHK SUM --------------- */
                                                                /* See Note #7.                                         */
    switch (picmp_hdr->Type) {                                  /* See Note #7b.                                        */
        case NET_ICMP_MSG_TYPE_DEST_UNREACH:
        case NET_ICMP_MSG_TYPE_SRC_QUENCH:
        case NET_ICMP_MSG_TYPE_TIME_EXCEED:
        case NET_ICMP_MSG_TYPE_PARAM_PROB:
        case NET_ICMP_MSG_TYPE_TS_REQ:
        case NET_ICMP_MSG_TYPE_TS_REPLY:
        case NET_ICMP_MSG_TYPE_ADDR_MASK_REPLY:
             icmp_chk_sum_valid = NetUtil_16BitOnesCplChkSumHdrVerify((void     *)picmp_hdr,
                                                                      (CPU_INT16U)pbuf_hdr->ICMP_MsgLen,
                                                                      (NET_ERR  *)perr);
             break;


        case NET_ICMP_MSG_TYPE_ECHO_REQ:
        case NET_ICMP_MSG_TYPE_ECHO_REPLY:
             icmp_chk_sum_valid = NetUtil_16BitOnesCplChkSumDataVerify((void     *)pbuf,
                                                                       (void     *)0,
                                                                       (CPU_INT16U)0,
                                                                       (NET_ERR  *)perr);
             break;


        default:                                                /* See Note #8.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrTypeCtr);
            *perr = NET_ICMP_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

    if (icmp_chk_sum_valid != DEF_OK) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrChkSumCtr);
       *perr = NET_ICMP_ERR_INVALID_CHK_SUM;
        return;
    }
#if 0                                                           /* Conv to host-order NOT necessary (see Note #7c).     */
   (void)NET_UTIL_VAL_GET_NET_16(&picmp_hdr->ChkSum);
#endif



/*$PAGE*/
                                                                /* ----------------- UPDATE BUF CTRLS ----------------- */
    pbuf_hdr->ICMP_HdrLen = icmp_msg_len_hdr;

    switch (picmp_hdr->Type) {
        case NET_ICMP_MSG_TYPE_DEST_UNREACH:
        case NET_ICMP_MSG_TYPE_SRC_QUENCH:
        case NET_ICMP_MSG_TYPE_TIME_EXCEED:
        case NET_ICMP_MSG_TYPE_PARAM_PROB:
        case NET_ICMP_MSG_TYPE_TS_REQ:
        case NET_ICMP_MSG_TYPE_TS_REPLY:
        case NET_ICMP_MSG_TYPE_ADDR_MASK_REPLY:
             pbuf_hdr->DataLen = 0;                             /* Clr data len/ix       (see Note #9a1).               */
#if 0                                                           /* Clr'd in NetBuf_Get() [see Note #9a3].               */
             pbuf_hdr->DataIx  = NET_BUF_IX_NONE;
#endif

             pbuf_next = pbuf_hdr->NextBufPtr;
             while (pbuf_next != (NET_BUF *)0) {                /* Clr ALL pkt bufs' data len/ix (see Note #9a2).       */
                 pbuf_next_hdr              = &pbuf_next->Hdr;
                 pbuf_next_hdr->DataLen     =  0;
#if 0                                                           /* Clr'd in NetBuf_Get() [see Note #9a3].               */
                 pbuf_next_hdr->DataIx      =  NET_BUF_IX_NONE;
#endif
                 pbuf_next_hdr->ICMP_HdrLen =  0;               /* NULL ICMP hdr len in each pkt buf.                   */
                 pbuf_next_hdr->ICMP_MsgLen =  icmp_msg_len;    /* Dup  ICMP msg len in each pkt buf (see Note #9c).    */
                 pbuf_next                  =  pbuf_next_hdr->NextBufPtr;
             }
             break;


        case NET_ICMP_MSG_TYPE_ECHO_REQ:
        case NET_ICMP_MSG_TYPE_ECHO_REPLY:
                                                                /* Calc ICMP Echo Msg data len/ix (see Note #9b).       */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
             if (pbuf_hdr->ICMP_HdrLen > pbuf_hdr->DataLen) {
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrDataLenCtr);
                *perr = NET_ICMP_ERR_INVALID_LEN_DATA;
                 return;
             }
#endif
             pbuf_hdr->DataLen -= (NET_BUF_SIZE) pbuf_hdr->ICMP_HdrLen;  
             pbuf_hdr->DataIx   = (CPU_INT16U  )(pbuf_hdr->ICMP_MsgIx + NET_ICMP_MSG_LEN_MIN_ECHO);

             pbuf_next = pbuf_hdr->NextBufPtr;
             while (pbuf_next != (NET_BUF *)0) {                /* Calc ALL pkt bufs' data len/ix   (see Note #9b2).    */
                 pbuf_next_hdr              = &pbuf_next->Hdr;
                 pbuf_next_hdr->DataIx      =  pbuf_next_hdr->ICMP_MsgIx;
                 pbuf_next_hdr->ICMP_HdrLen =  0;               /* NULL ICMP hdr len in each pkt buf.                   */
                 pbuf_next_hdr->ICMP_MsgLen =  icmp_msg_len;    /* Dup  ICMP msg len in each pkt buf (see Note #9c).    */
                 pbuf_next                  =  pbuf_next_hdr->NextBufPtr;
             }
             break;


        default:                                                /* See Note #8.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrTypeCtr);
            *perr = NET_ICMP_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }



                                                                /* --------------- DEMUX ICMP MSG TYPE ---------------- */
    switch (picmp_hdr->Type) {
        case NET_ICMP_MSG_TYPE_DEST_UNREACH:
        case NET_ICMP_MSG_TYPE_SRC_QUENCH:
        case NET_ICMP_MSG_TYPE_TIME_EXCEED:
        case NET_ICMP_MSG_TYPE_PARAM_PROB:
            *perr = NET_ICMP_ERR_MSG_TYPE_ERR;
             break;


        case NET_ICMP_MSG_TYPE_ECHO_REQ:
        case NET_ICMP_MSG_TYPE_TS_REQ:
            *perr = NET_ICMP_ERR_MSG_TYPE_REQ;
             break;


        case NET_ICMP_MSG_TYPE_ECHO_REPLY:
        case NET_ICMP_MSG_TYPE_TS_REPLY:
        case NET_ICMP_MSG_TYPE_ADDR_MASK_REPLY:
            *perr = NET_ICMP_ERR_MSG_TYPE_REPLY;
             break;


        default:                                                /* See Note #8.                                         */
             NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxHdrTypeCtr);
            *perr = NET_ICMP_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetICMP_RxPktFree()
*
* Description : Free network buffer(s).
*
* Argument(s) : pbuf        Pointer to network buffer.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetICMP_RxPktFree (NET_BUF  *pbuf)
{
    NetBuf_FreeBufList((NET_BUF *)pbuf,
                       (NET_CTR *)0);
}


/*
*********************************************************************************************************
*                                       NetICMP_RxPktDiscard()
*
* Description : On any ICMP receive errors, discard ICMP packet(s) & buffer(s).
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetICMP_RxPktDiscard (NET_BUF  *pbuf,
                                    NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetICMP_ErrRxPktDiscardedCtr;
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
*                                     NetICMP_TxMsgErrValidate()
*
* Description : (1) Validate received packet & transmit error parameters for ICMP Error Message transmit :
*
*                   (a) RFC #1122, Section 3.2.2 specifies that "an ICMP error message MUST NOT be sent as
*                       the result of receiving" :
*
*                       (1) "An ICMP Error Message",                                          ...
*
*                       (2) "A datagram destined to an IP broadcast or IP multicast address", ...
*
*                           (A) Any packet received as an IP broadcast or IP multicast destination address MUST
*                               also have been received as a link-layer broadcast (see RFC #1122, Section 3.3.6
*                               & 'net_ip.c  NetIP_RxPktValidate()  Note #9d2B1b').
*
*                               Therefore, it is NOT necessary to re-validate the IP destination address as a 
*                               non-broadcast address since it has ALREADY been validated as a non-broadcast 
*                               at the link-layer (see Note #1a3).
*
*                           (B) See also 'net_ip.h  Note #1d'.
*
*                       (3) "A datagram sent as a link-layer broadcast",                      ...
*
*                       (4) "A non-initial fragment",                                         ...
*
*                       (5) "A datagram whose source address does not define a single host -- e.g., a zero address,
*                            a loopback address, a broadcast address, a multicast address, or a Class E address"
*
*                           (A) ALL IP source addresses already validated (see 'net_ip.c  NetIP_RxPktValidate()
*                               Note #9c') except 'This Host' & 'Specified Host' addresses.
*
*                           (B) See also 'net_ip.h  Note #1d'.
*
*                   (b) Validate ICMP Error Message transmit parameters :
*
*                       (1) Type
*                       (2) Code
*                       (3) Pointer                                         See Note #6
*
*
* Argument(s) : pbuf        Pointer to network buffer that received a packet with error(s).
*               ----        Argument checked   in NetICMP_TxMsgErr().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetICMP_TxMsgErr().
*
*               pip_hdr     Pointer to received packet's IP header.
*               -------     Argument validated in NetICMP_TxMsgErr().
*
*               type        ICMP Error Message type (see Note #5) :
*
*                               NET_ICMP_MSG_TYPE_DEST_UNREACH
*                               NET_ICMP_MSG_TYPE_SRC_QUENCH
*                               NET_ICMP_MSG_TYPE_TIME_EXCEED
*                               NET_ICMP_MSG_TYPE_PARAM_PROB
*
*               code        ICMP Error Message code (see Note #5).
*
*               ptr         Pointer to received packet's ICMP error (optional).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ICMP_ERR_NONE                   Received packet successfully validated 
*                                                                       for ICMP transmit error message.
*                               NET_ICMP_ERR_TX_INVALID_BROADCAST   Packet received as a broadcast packet.
*                               NET_ICMP_ERR_TX_INVALID_ADDR_SRC    Packet received with a non-single host
*                                                                       IP source address.
*                               NET_ICMP_ERR_TX_INVALID_FRAG        Packet received as a non-initial fragment.
*                               NET_ICMP_ERR_TX_INVALID_ERR_MSG     Packet received as an ICMP Error Message.
*                               NET_ICMP_ERR_INVALID_TYPE           Invalid/unknown ICMP message type.
*                               NET_ICMP_ERR_INVALID_CODE           Invalid/unknown ICMP message code.
*                               NET_ICMP_ERR_INVALID_PTR            Invalid message pointer outside error message.
*                               NET_BUF_ERR_INVALID_IX              Invalid buffer index.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_TxMsgErr().
*$PAGE*
* Note(s)     : (2) The following IP header fields MUST be converted from network-order to host-order BEFORE 
*                   any transmit ICMP Error Messages are validated :
*
*                   (a) Total  Length                           See 'net_ip.c  NetIP_RxPktValidate()  Note #3b'
*                   (b) Source Address                          See 'net_ip.c  NetIP_RxPktValidate()  Note #3c'
*
*               (3) See 'net_ip.h  IP ADDRESS DEFINES  Notes #2 & #3' for supported IP addresses.
*
*               (4) Default case already invalidated in NetIP_RxPktValidate().  However, the default case
*                   is included as an extra precaution in case 'IP_AddrSrc' is incorrectly modified.
*
*               (5) See 'net_icmp.h  ICMP MESSAGE TYPES & CODES' for supported ICMP message types & codes.
*
*               (6) (a) ICMP Parameter Problem Messages' pointer fields validated by comparing the pointer
*                       field value to minimum & maximum pointer field values.
*
*                   (b) Since an ICMP Parameter Problem Message's minimum pointer field value is NOT less
*                       than zero (see 'net_icmp.h  ICMP POINTER DEFINES'), a minimum pointer field value
*                       check is NOT required unless native data type 'CPU_INT16U' is incorrectly configured
*                       as a signed integer in 'cpu.h'.
*
*                   (c) Since an ICMP Parameter Problem Message may be received for an IP or higher-layer
*                       protocol, the maximum pointer field value is specific to each received ICMP packets'
*                       IP header length & demultiplexed protocol header length :
*
*                           Pointer Field Value < Maximum Pointer Field Value = IP Header Length
*                                                                           [ + Other Protocol Header Length ]
*
*                   (d) See 'net_icmp.h  ICMP POINTER DEFINES  Note #2' for supported ICMP Parameter Problem
*                       Message protocols.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetICMP_TxMsgErrValidate (NET_BUF      *pbuf,
                                        NET_BUF_HDR  *pbuf_hdr,
                                        NET_IP_HDR   *pip_hdr,
                                        CPU_INT08U    type,
                                        CPU_INT08U    code,
                                        CPU_INT08U    ptr,
                                        NET_ERR      *perr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_INT16U     ptr_max;
    CPU_INT16U     ptr_max_protocol;
#endif
    CPU_BOOLEAN    rx_broadcast;
    CPU_BOOLEAN    ip_flag_frags_more;
    CPU_INT16U     ip_flags;
    CPU_INT16U     ip_frag_offset;
    NET_ICMP_HDR  *picmp_hdr;



                                                                /* ------------- CHK LINK-LAYER BROADCAST ------------- */
    rx_broadcast = DEF_BIT_IS_SET(pbuf_hdr->Flags, NET_BUF_FLAG_RX_BROADCAST);
    if (rx_broadcast != DEF_NO) {                               /* If pkt rx'd via broadcast, ...                       */
       *perr = NET_ICMP_ERR_TX_INVALID_BROADCAST;               /* ... rtn err (see Note #1a3).                         */
        return;
    }


                                                                /* ----------------- CHK IP DEST ADDR ----------------- */
                                                                /* See Note #1a2.                                       */
                                                                /* Dest addr already validated (see Note #1a2A).        */


                                                                /* ----------------- CHK IP SRC ADDR ------------------ */
                                                                /* See Note #1a5.                                       */
    if (pbuf_hdr->IP_AddrSrc == NET_IP_ADDR_THIS_HOST) {        /* Chk invalid         'This Host' (see Note #1a5A).    */
       *perr = NET_ICMP_ERR_TX_INVALID_ADDR_SRC;
        return;
    }

    if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_CLASS_A_MASK) == NET_IP_ADDR_CLASS_A) {
                                                                /* Chk invalid Class-A 'This Host' (see Note #1a5A).    */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_CLASS_A_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_A_MASK_HOST)) {
            *perr = NET_ICMP_ERR_TX_INVALID_ADDR_SRC;
             return;
        }

    } else if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_CLASS_B_MASK) == NET_IP_ADDR_CLASS_B) {
                                                                /* Chk invalid Class-B 'This Host' (see Note #1a5A).    */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_CLASS_B_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_B_MASK_HOST)) {
            *perr = NET_ICMP_ERR_TX_INVALID_ADDR_SRC;
             return;
        }

    } else if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_CLASS_C_MASK) == NET_IP_ADDR_CLASS_C) {
                                                                /* Chk invalid Class-C 'This Host' (see Note #1a5A).    */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_CLASS_C_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_C_MASK_HOST)) {
            *perr = NET_ICMP_ERR_TX_INVALID_ADDR_SRC;
             return;
        }

    } else {                                                    /* Discard invalid addr class (see Notes #3 & #4).      */
       *perr = NET_ICMP_ERR_TX_INVALID_ADDR_SRC;
        return;
    }


                                                                /* ------------------- CHK IP FRAG -------------------- */

    ip_flags           = pbuf_hdr->IP_Flags_FragOffset & NET_IP_HDR_FLAG_MASK;
    ip_flag_frags_more = DEF_BIT_IS_SET(ip_flags, NET_IP_HDR_FLAG_FRAG_MORE);
    if (ip_flag_frags_more != DEF_NO) {                         /* If 'More Frags' flag set ...                         */
        ip_frag_offset = pbuf_hdr->IP_Flags_FragOffset & NET_IP_HDR_FRAG_OFFSET_MASK;
        if (ip_frag_offset != NET_IP_HDR_FRAG_OFFSET_NONE) {    /* ... & frag offset != 0,  ...                         */
           *perr = NET_ICMP_ERR_TX_INVALID_FRAG;                /* ... rtn err for non-initial frag (see Note #1a4).    */
            return;
        }
    }



/*$PAGE*/
                                                                /* ----------------- CHK ICMP ERR MSG ----------------- */
    if (pip_hdr->Protocol == NET_IP_HDR_PROTOCOL_ICMP) {        /* If rx'd IP datagram is ICMP, ...                     */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        if (pbuf_hdr->ICMP_MsgIx == NET_BUF_IX_NONE) {
            NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrRxInvalidBufIxCtr);
           *perr = NET_BUF_ERR_INVALID_IX;
            return;
        }
#endif
        picmp_hdr = (NET_ICMP_HDR *)&pbuf->Data[pbuf_hdr->ICMP_MsgIx];

        switch (picmp_hdr->Type) {                              /* ... chk ICMP msg type & ...                          */
            case NET_ICMP_MSG_TYPE_DEST_UNREACH:
            case NET_ICMP_MSG_TYPE_SRC_QUENCH:
            case NET_ICMP_MSG_TYPE_TIME_EXCEED:
            case NET_ICMP_MSG_TYPE_PARAM_PROB:
                *perr = NET_ICMP_ERR_TX_INVALID_ERR_MSG;        /* ... rtn err for ICMP err msgs (see Note #1a1).       */
                 return;                                        /* Prevent 'break NOT reachable' compiler warning.      */


            case NET_ICMP_MSG_TYPE_ECHO_REQ:
            case NET_ICMP_MSG_TYPE_ECHO_REPLY:
            case NET_ICMP_MSG_TYPE_TS_REQ:
            case NET_ICMP_MSG_TYPE_TS_REPLY:
            case NET_ICMP_MSG_TYPE_ADDR_MASK_REPLY:
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                 ptr_max_protocol = pbuf_hdr->ICMP_MsgLen;      /* See Note #6c.                                        */
#endif
                 break;

#if 0                                                           /* ---------------- UNSUPPORTED TYPES ----------------- */
                                                                /* See Note #5.                                         */
            case NET_ICMP_MSG_TYPE_REDIRECT:
            case NET_ICMP_MSG_TYPE_ROUTE_REQ:
            case NET_ICMP_MSG_TYPE_ROUTE_AD:
            case NET_ICMP_MSG_TYPE_ADDR_MASK_REQ:
                                                                /* 'break' intentionally omitted; do NOT move from the  */
                                                                /* ... following case : 'default'.                      */
#endif

            default:
                *perr = NET_ICMP_ERR_INVALID_TYPE;
                 return;                                        /* Prevent 'break NOT reachable' compiler warning.      */
        }


    } else {                                                    /* #### See Note #6d.                                   */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        ptr_max_protocol = 0;
#endif
    }



/*$PAGE*/
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------- VALIDATE ICMP ERR MSG TYPE/CODE/PTR -------- */
    switch (type) {
        case NET_ICMP_MSG_TYPE_DEST_UNREACH:
             switch (code) {
                 case NET_ICMP_MSG_CODE_DEST_PROTOCOL:
                 case NET_ICMP_MSG_CODE_DEST_PORT:
                      break;


                 case NET_ICMP_MSG_CODE_DEST_NET:
                 case NET_ICMP_MSG_CODE_DEST_HOST:
                 case NET_ICMP_MSG_CODE_DEST_FRAG_NEEDED:
                 case NET_ICMP_MSG_CODE_DEST_ROUTE_FAIL:
                 case NET_ICMP_MSG_CODE_DEST_NET_UNKNOWN:
                 case NET_ICMP_MSG_CODE_DEST_HOST_UNKNOWN:
                 case NET_ICMP_MSG_CODE_DEST_HOST_ISOLATED:
                 case NET_ICMP_MSG_CODE_DEST_NET_TOS:
                 case NET_ICMP_MSG_CODE_DEST_HOST_TOS:
                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_ICMP_MSG_TYPE_SRC_QUENCH:
             switch (code) {
                 case NET_ICMP_MSG_CODE_SRC_QUENCH:
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_ICMP_MSG_TYPE_TIME_EXCEED:
             switch (code) {
                 case NET_ICMP_MSG_CODE_TIME_EXCEED_TTL:
                 case NET_ICMP_MSG_CODE_TIME_EXCEED_FRAG_REASM:
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_ICMP_MSG_TYPE_PARAM_PROB:
             switch (code) {
                 case NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR:
                 case NET_ICMP_MSG_CODE_PARAM_PROB_OPT_MISSING:
                                                                /* Validate ICMP Param Prob Msg ptr (see Note #6).      */
#if 0                                                           /* See Note #6b.                                        */
                      if (ptr < NET_ICMP_PTR_IX_BASE) {
                          NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrPtrCtr);
                         *perr = NET_ICMP_ERR_INVALID_PTR;
                      }
#endif
                      ptr_max = pbuf_hdr->IP_HdrLen + ptr_max_protocol;
                      if (ptr >= ptr_max) {
                          NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrPtrCtr);
                         *perr = NET_ICMP_ERR_INVALID_PTR;
                      }
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_ICMP_MSG_TYPE_ECHO_REQ:
        case NET_ICMP_MSG_TYPE_ECHO_REPLY:
        case NET_ICMP_MSG_TYPE_TS_REQ:
        case NET_ICMP_MSG_TYPE_TS_REPLY:
        case NET_ICMP_MSG_TYPE_ADDR_MASK_REPLY:
            *perr = NET_ICMP_ERR_TX_INVALID_ERR_MSG;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */

/*$PAGE*/
#if 0                                                           /* ---------------- UNSUPPORTED TYPES ----------------- */
                                                                /* See Note #5.                                         */
        case NET_ICMP_MSG_TYPE_REDIRECT:
        case NET_ICMP_MSG_TYPE_ROUTE_REQ:
        case NET_ICMP_MSG_TYPE_ROUTE_AD:
        case NET_ICMP_MSG_TYPE_ADDR_MASK_REQ:
                                                                /* 'break' intentionally omitted; do NOT move from the  */
                                                                /* ... following case : 'default'.                      */
#endif

        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrTypeCtr);
            *perr = NET_ICMP_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

#else                                                           /* Prevent compiler warnings.                           */
   (void)&type;
   (void)&code;
   (void)&ptr;
#endif


   *perr = NET_ICMP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetICMP_TxMsgReqValidate()
*
* Description : (1) Validate parameters for ICMP Request Message transmit :
*
*                   (a) Type                                                        See Note #2
*                   (b) Code                                                        See Note #2
*
*
* Argument(s) : type        ICMP Request Message type (see Note #1a) :
*
*                               NET_ICMP_MSG_TYPE_ECHO_REQ
*                               NET_ICMP_MSG_TYPE_TS_REQ
*                               NET_ICMP_MSG_TYPE_ADDR_MASK_REQ
*
*               code        ICMP Request Message code (see Note #1b).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ICMP_ERR_NONE                   Transmit parameters successfully validated 
*                                                                       for ICMP transmit request message.
*                               NET_ICMP_ERR_INVALID_TYPE           Invalid/unknown ICMP message type.
*                               NET_ICMP_ERR_INVALID_CODE           Invalid/unknown ICMP message code.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_TxMsgReq().
*
* Note(s)     : (2) See 'net_icmp.h  ICMP MESSAGE TYPES & CODES' for supported ICMP message types & codes.
*********************************************************************************************************
*/

static  void  NetICMP_TxMsgReqValidate (CPU_INT08U   type,
                                        CPU_INT08U   code,
                                        NET_ERR     *perr)
{

                                                                /* --------- VALIDATE ICMP REQ MSG TYPE/CODE ---------- */
    switch (type) {
        case NET_ICMP_MSG_TYPE_ECHO_REQ:
             switch (code) {
                 case NET_ICMP_MSG_CODE_ECHO_REQ:
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_ICMP_MSG_TYPE_TS_REQ:
             switch (code) {
                 case NET_ICMP_MSG_CODE_TS_REQ:
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        case NET_ICMP_MSG_TYPE_ADDR_MASK_REQ:
             switch (code) {
                 case NET_ICMP_MSG_CODE_ADDR_MASK_REQ:
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrCodeCtr);
                     *perr = NET_ICMP_ERR_INVALID_CODE;
                      return;                                   /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;


        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxHdrTypeCtr);
            *perr = NET_ICMP_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


   *perr = NET_ICMP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetICMP_TxMsgReply()
*
* Description : (1) Transmit ICMP Reply Message in response to received ICMP Request Message :
*
*                   (a) Process ICMP Reply Message :
*
*                       (1) The following ICMP Reply Messages require receive processing :
*
*                           (A) Timestamp Reply Message
*
*                               (1) "The Receive Timestamp is the time the echoer first touched it on receipt"
*                                   (RFC #792, Section 'Timestamp or Timestamp Reply Message : Description').
*
*                   (b) Get buffer for ICMP Reply Message :
*
*                       (1) Copy ICMP Request Message into ICMP Reply Message
*                       (2) Initialize ICMP Reply Message buffer controls
*
*                   (c) Prepare ICMP Reply Message's IP header :
*
*                       (1) RFC #1349, Section 5.1 specifies that "an ICMP reply message is sent with the same
*                           value in the TOS field as was used in the corresponding ICMP request message".
*
*                       (2) RFC #1122, Sections 3.2.2.6 & 3.2.2.8 specify that "if a Record Route and/or Time
*                           Stamp option is received in [an ICMP Request, these options] SHOULD be updated ...
*                           and included in the IP header of the ... Reply message".  Also "if a Source Route
*                           option is received ... the return route MUST be reversed and used as a Source Route
*                           option for the ... Reply message".
*
*                           #### These IP header option requirements for ICMP Reply Messages are NOT yet implemented.
*
*                   (d) Prepare ICMP Reply Message :
*
*                       (1) Echo Reply Message
*
*                           (A) "To form an echo reply message, the source and destination addresses are simply
*                                reversed, the type code changed to [reply], and the checksum recomputed"
*                               (RFC #792, Section 'Echo or Echo Reply Message : Addresses').
*
*                       (2) Timestamp Reply Message
*
*                           (A) "The Transmit Timestamp is the time the echoer last touched the message on sending
*                                it" (RFC #792, Section 'Timestamp or Timestamp Reply Message : Description').
*
*                           (B) "To form a timestamp reply message, the source and destination addresses are 
*                                simply reversed, the type code changed to [reply], and the checksum recomputed"
*                                (RFC #792, Section 'Timestamp or Timestamp Reply Message : Addresses').
*
*                       (3) Some ICMP Reply Message fields are copied directly from the ICMP Request Message.
*
*                           (A) ICMP Reply Message Identification & Sequence Number fields were NOT validated
*                               or converted from network-order to host-order (see 'NetICMP_RxPktValidate()  
*                               Notes #1b2C & #1b2D') & therefore do NOT need to be converted from host-order
*                               to network-order.
*
*                   (e) Transmit ICMP Reply Message
*
*                   (f) Free     ICMP Reply Message buffer
*
*                   (g) Update transmit statistics
*
*$PAGE*
* Argument(s) : pbuf        Pointer to network buffer that received ICMP packet.
*               ----        Argument checked   in NetICMP_Rx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetICMP_Rx().
*
*               picmp_hdr   Pointer to received packet's ICMP header.
*               ---------   Argument validated in NetICMP_Rx()/NetICMP_RxPktValidateBuf().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ICMP_ERR_NONE               ICMP Reply Message successfully transmitted.
*
*                                                               --- RETURNED BY NetICMP_TxPktDiscard() : ---
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
*                                                               --------- RETURNED BY NetIP_Tx() : ---------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_Rx().
*
* Note(s)     : (2) Default case already invalidated in NetICMP_RxPktValidate().  However, the default case
*                   is included as an extra precaution in case 'Type' is incorrectly modified.
*
*               (3) (a) RFC #1122, Section 3.2.2.6 states that "data received in an ICMP Echo Request MUST 
*                       be entirely included in the resulting Echo Reply.  However, if sending the Echo Reply 
*                       requires intentional fragmentation that is not implemented, the datagram MUST be 
*                       truncated to maximum transmission size ... and sent".
*
*                       See also 'net_ip.h  Note #1e'.
*
*                   (b) In case the maximum network buffer size is smaller than the ICMP message data length, 
*                       the ICMP Echo Request Message data should be similarly truncated in order to transmit
*                       the ICMP Echo Reply Message.
*
*               (4) Some buffer controls were previously initialized in NetBuf_Get() when the buffer
*                   was allocated earlier in this function.  These buffer controls do NOT need to be
*                   re-initialized but are shown for completeness.
*
*               (5) (a) ICMP message Check-Sum MUST be calculated AFTER the entire ICMP message has been
*                       prepared.  In addition, ALL multi-octet words are converted from host-order to 
*                       network-order since "the sum of 16-bit integers can be computed in either byte 
*                       order" [RFC #1071, Section 2.(B)].
*
*                   (b) ICMP message Check-Sum field MUST be cleared to '0' BEFORE the ICMP message
*                       Check-Sum is calculated (see RFC #792, Sections 'Echo or Echo Reply Message : 
*                       Checksum' & 'Timestamp or Timestamp Reply Message : Checksum').
*
*                   (c) The ICMP message Check-Sum field is returned in network-order & MUST NOT be re-
*                       converted back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()
*                       Note #3b' & 'net_util.c  NetUtil_16BitOnesCplChkSumDataCalc()  Note #4b').
*
*               (6) Network buffer already freed by lower layer; only increment error counter.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetICMP_TxMsgReply (NET_BUF       *pbuf,
                                  NET_BUF_HDR   *pbuf_hdr,
                                  NET_ICMP_HDR  *picmp_hdr,
                                  NET_ERR       *perr)
{
#if 0                                                           /* DISABLED while NOT yet implemented.                  */
    NET_IP_OPT_CFG_ROUTE_TS   msg_opt_route_ts;
#endif
    NET_ICMP_HDR_ECHO        *picmp_hdr_echo;
    NET_ICMP_HDR_TS          *picmp_hdr_ts;
    NET_BUF                  *pmsg_req;
    NET_BUF                  *pmsg_reply;
    NET_BUF_HDR              *pmsg_req_hdr;
    NET_BUF_HDR              *pmsg_reply_hdr;
    NET_IP_OPT_CFG_ROUTE_TS  *pmsg_opt_cfg_route_ts;
    NET_IP_HDR               *pip_hdr;
    NET_BUF_SIZE              buf_size_max;
    NET_BUF_SIZE              buf_size_max_data;
    NET_MTU                   icmp_mtu;
    CPU_INT16U                msg_size_hdr;
    CPU_INT16U                msg_len;
    CPU_INT16U                msg_len_min;
    CPU_INT16U                msg_len_rem;
    CPU_INT16U                msg_ix;
    CPU_INT16U                msg_reply_ix;
    CPU_INT16U                msg_reply_len;
    CPU_INT16U                msg_chk_sum;
    CPU_INT16U                flags;
    CPU_INT08U               *pmsg_req_data;
    NET_TS                    ts;
    NET_IP_TOS                TOS;
    NET_ERR                   err;



                                                                /* ------------- PROCESS ICMP REPLY MSGs -------------- */
    switch (picmp_hdr->Type) {
        case NET_ICMP_MSG_TYPE_ECHO_REQ:
             msg_size_hdr =  NET_ICMP_HDR_SIZE_ECHO;
             msg_len_min  =  NET_ICMP_MSG_LEN_MIN_ECHO;
             break;


        case NET_ICMP_MSG_TYPE_TS_REQ:
             picmp_hdr_ts = (NET_ICMP_HDR_TS *)picmp_hdr;
             ts           =  NetUtil_TS_Get();                  /* See Note #1a1A1.                                     */
             NET_UTIL_VAL_COPY_SET_NET_32(&picmp_hdr_ts->TS_Rx, &ts);
             msg_size_hdr =  NET_ICMP_HDR_SIZE_TS;
             msg_len_min  =  NET_ICMP_MSG_LEN_MIN_TS;
             break;


        default:                                                /* See Note #2.                                         */
             NetICMP_TxPktDiscard((NET_BUF *)0,
                                  (NET_ERR *)perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }



/*$PAGE*/
                                                                /* -------------- GET ICMP REPLY MSG BUF -------------- */
    msg_len           =  pbuf_hdr->ICMP_MsgLen;                 /* Adj req msg len for reply msg len.                   */
    icmp_mtu          =  NET_IP_MTU;
    msg_ix            =  NET_BUF_DATA_TX_IX;
    buf_size_max      =  NetBuf_GetMaxSize((NET_BUF    *)0,
                                           (NET_BUF_SIZE)msg_ix);

    buf_size_max_data = (NET_BUF_SIZE)DEF_MIN(buf_size_max, icmp_mtu);

    if (msg_len > buf_size_max_data) {                          /* If msg len > max data size, ...                      */
        if (picmp_hdr->Type != NET_ICMP_MSG_TYPE_ECHO_REQ) {    /* ... for Echo Req Msg ONLY,  ...                      */
            NetICMP_TxPktDiscard((NET_BUF *)0,
                                 (NET_ERR *)perr);
            return;
        }
        msg_len = (CPU_INT16U)buf_size_max_data;                /* ... truncate msg len (see Note #3).                  */
    }

    if (msg_len < msg_len_min) {                                /* If msg len < min msg len, rtn err.                   */
        NetICMP_TxPktDiscard((NET_BUF *)0,
                             (NET_ERR *)perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxInvalidLenCtr);
        return;
    }

                                                                /* Get reply msg buf.                                   */
    pmsg_reply = NetBuf_Get((NET_BUF_SIZE) msg_len,
                            (NET_BUF_SIZE) msg_ix,
                            (CPU_INT16U  ) NET_BUF_FLAG_NONE,
                            (NET_ERR    *)&err);
    if (err != NET_BUF_ERR_NONE) {
        NetICMP_TxPktDiscard(pmsg_reply, perr);
        return;
    }


    pmsg_req     = pbuf;
    msg_reply_ix = msg_ix;
    msg_len_rem  = msg_len;

    while ((pmsg_req != (NET_BUF *)0) && (msg_len_rem > 0)) {   /* For ALL ICMP req msg pkt bufs, ...                   */
                                                                /* ... copy rx'd ICMP req msg into ICMP reply tx buf.   */
        pmsg_req_hdr  = &pmsg_req->Hdr;
        pmsg_req_data = &pmsg_req->Data[pmsg_req_hdr->ICMP_MsgIx];
        msg_reply_len =  pmsg_req_hdr->IP_DataLen;              /* Each pkt buf's IP data is ICMP req msg data.         */
        if (msg_reply_len > msg_len_rem) {                      /* If req msg pkt buf data len > rem msg len, ...       */
            msg_reply_len = msg_len_rem;                        /* ... truncate req msg pkt buf data len.               */
        }

        NetBuf_DataWr((NET_BUF    *) pmsg_reply,                
                      (NET_BUF_SIZE) msg_reply_ix,
                      (NET_BUF_SIZE) msg_reply_len,
                      (CPU_INT08U *) pmsg_req_data,
                      (NET_ERR    *)&err);
        if (err != NET_BUF_ERR_NONE) {
            NetICMP_TxPktDiscard(pmsg_reply, perr);
            return;
        }

        msg_reply_ix += msg_reply_len;
        msg_len_rem  -= msg_reply_len;

        pmsg_req      = pmsg_req_hdr->NextBufPtr;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (msg_len_rem > 0) {                                      /* If entire ICMP req msg NOT copied, rtn err.          */
        NetICMP_TxPktDiscard(pmsg_reply, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxInvalidLenCtr);
        return;
    }
#endif

                                                                /* Init reply msg buf ctrls.                            */
    pmsg_reply_hdr                  = &pmsg_reply->Hdr;
    pmsg_reply_hdr->ICMP_MsgIx      = (CPU_INT16U  )msg_ix;
    pmsg_reply_hdr->ICMP_MsgLen     = (CPU_INT16U  )msg_len;
    pmsg_reply_hdr->ICMP_HdrLen     = (CPU_INT16U  )msg_size_hdr;
    pmsg_reply_hdr->TotLen          = (NET_BUF_SIZE)pmsg_reply_hdr->ICMP_MsgLen;
    pmsg_reply_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_ICMP;



/*$PAGE*/
                                                                /* ------------ PREPARE ICMP REPLY IP HDR ------------- */
    pip_hdr               = (NET_IP_HDR *)&pbuf->Data[pbuf_hdr->IP_HdrIx];
    TOS                   =  pip_hdr->TOS;                      /* See Note #1c1.                                       */
    flags                 =  NET_IP_FLAG_NONE;                  /* See Note #1c2.                                       */
    pmsg_opt_cfg_route_ts = (NET_IP_OPT_CFG_ROUTE_TS *)0;



                                                                /* -------------- PREPARE ICMP REPLY MSG -------------- */
    switch (picmp_hdr->Type) {
        case NET_ICMP_MSG_TYPE_ECHO_REQ:                        /* See Note #1d1.                                       */
             picmp_hdr_echo         = (NET_ICMP_HDR_ECHO *)&pmsg_reply->Data[pmsg_reply_hdr->ICMP_MsgIx];
             picmp_hdr_echo->Type   =  NET_ICMP_MSG_TYPE_ECHO_REPLY;
#if 0                                                           /* Copied from ICMP req msg (see Note #1d3).            */
             picmp_hdr_echo->Code   =  NET_ICMP_MSG_CODE_ECHO;
                                                                /* See Note #1d3A.                                      */
            (void)&picmp_hdr_echo->ID;
            (void)&picmp_hdr_echo->SeqNbr;
#endif
                                                                /* Calc ICMP msg data len.                              */
             if (pbuf_hdr->ICMP_MsgLen > msg_len_min) {
                 pmsg_reply_hdr->DataIx  = (CPU_INT16U  )(pmsg_reply_hdr->ICMP_MsgIx  + msg_len_min);
                 pmsg_reply_hdr->DataLen = (NET_BUF_SIZE)(pmsg_reply_hdr->ICMP_MsgLen - msg_len_min);
#if 0                                                           /* Init'd in NetBuf_Get() [see Note #4].                */
             } else {
                 pmsg_reply_hdr->DataIx  = NET_BUF_IX_NONE;
                 pmsg_reply_hdr->DataLen = 0;
#endif
             }
                                                                            /* Calc ICMP msg chk sum (see Note #5).     */
             NET_UTIL_VAL_SET_NET_16(&picmp_hdr_echo->ChkSum, 0x0000);      /* Clr           chk sum (see Note #5b).    */
             msg_chk_sum = NetUtil_16BitOnesCplChkSumDataCalc((void     *) pmsg_reply,
                                                              (void     *) 0,
                                                              (CPU_INT16U) 0,
                                                              (NET_ERR  *)&err);
             NET_UTIL_VAL_COPY_16(&picmp_hdr_echo->ChkSum, &msg_chk_sum);   /* Copy chk sum in net order (see Note #5c).*/
             break;


        case NET_ICMP_MSG_TYPE_TS_REQ:                          /* See Note #1d2.                                       */
             picmp_hdr_ts            = (NET_ICMP_HDR_TS *)&pmsg_reply->Data[pmsg_reply_hdr->ICMP_MsgIx];
             ts                      =  NetUtil_TS_Get();       /* See Note #1d2A.                                      */
             NET_UTIL_VAL_COPY_SET_NET_32(&picmp_hdr_ts->TS_Tx, &ts);

             picmp_hdr_ts->Type      =  NET_ICMP_MSG_TYPE_TS_REPLY;
#if 0                                                           /* Copied from ICMP req msg (see Note #1d3).            */
             picmp_hdr_ts->Code      =  NET_ICMP_MSG_CODE_TS;
                                                                /* See Note #1d3A.                                      */
            (void)&picmp_hdr_ts->ID;
            (void)&picmp_hdr_ts->SeqNbr;
#endif
                                                                /* NULL ICMP msg data len.                              */
#if 0                                                           /* Init'd in NetBuf_Get() [see Note #4].                */
             pmsg_reply_hdr->DataIx  =  NET_BUF_IX_NONE;
             pmsg_reply_hdr->DataLen =  0;
#endif
                                                                            /* Calc ICMP msg chk sum (see Note #5).     */
             NET_UTIL_VAL_SET_NET_16(&picmp_hdr_ts->ChkSum, 0x0000);        /* Clr           chk sum (see Note #5b).    */
             msg_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc((void     *) picmp_hdr_ts,
                                                             (CPU_INT16U) pmsg_reply_hdr->ICMP_MsgLen,
                                                             (NET_ERR  *)&err);
             NET_UTIL_VAL_COPY_16(&picmp_hdr_ts->ChkSum, &msg_chk_sum);     /* Copy chk sum in net order (see Note #5c).*/
             break;


        default:                                                /* See Note #2.                                         */
             NetICMP_TxPktDiscard(pmsg_reply, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

    if (err != NET_UTIL_ERR_NONE) {                             /* Chk err from NetUtil_16BitOnesCplChkSum()'s.         */
        NetICMP_TxPktDiscard(pmsg_reply, perr);
        return;
    }




/*$PAGE*/
                                                                /* ---------------- TX ICMP REPLY MSG ----------------- */
    NetIP_Tx((NET_BUF   *)pmsg_reply,
             (NET_IP_ADDR)pbuf_hdr->IP_AddrDest,
             (NET_IP_ADDR)pbuf_hdr->IP_AddrSrc,
             (NET_IP_TOS )TOS,
             (NET_IP_TTL )NET_IP_TTL_DFLT,
             (CPU_INT16U )flags,
             (void      *)pmsg_opt_cfg_route_ts,
             (NET_ERR   *)perr);



                                                                /* ------ FREE ICMP REPLY MSG / UPDATE TX STATS ------- */
    switch (*perr) {
        case NET_IP_ERR_NONE:
             NetICMP_TxPktFree(pmsg_reply);
             NET_CTR_STAT_INC(Net_StatCtrs.NetICMP_StatTxMsgCtr);
             NET_CTR_STAT_INC(Net_StatCtrs.NetICMP_StatTxMsgReplyCtr);
             break;


        case NET_ERR_TX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrTxPktDiscardedCtr);     /* See Note #6.                             */
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_ERR_INIT_INCOMPLETE:
        default:
             NetICMP_TxPktDiscard(pmsg_reply, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


   *perr = NET_ICMP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetICMP_TxPktFree()
*
* Description : Free network buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_TxMsgErr(),
*               NetICMP_TxMsgReq(),
*               NetICMP_TxMsgReply().
*
* Note(s)     : (1) (a) Although ICMP Transmit initially requests the network buffer for transmit, 
*                       the ICMP layer does NOT maintain a reference to the buffer.
*
*                   (b) Also, since the network interface layer frees ALL unreferenced buffers 
*                       after successful transmission [see 'net_if_pkt.c  NetIF_Pkt_TxPktFree()'
*                       or 'net_if_char.c  NetIF_Char_TxPktFree()'], the ICMP layer MUST not
*                       free the transmit buffer.
*********************************************************************************************************
*/

static  void  NetICMP_TxPktFree (NET_BUF  *pbuf)
{
   (void)&pbuf;                                                 /* Prevent compiler warning (see Note #1).              */
}


/*
*********************************************************************************************************
*                                       NetICMP_TxPktDiscard()
*
* Description : On any ICMP transmit packet errors, discard packet & buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_TxMsgErr(),
*               NetICMP_TxMsgReq(),
*               NetICMP_TxMsgReply().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetICMP_TxPktDiscard (NET_BUF  *pbuf,
                                    NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetICMP_ErrTxPktDiscardedCtr;
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
*                                    NetICMP_HandlerTxSrcQuench()
*
* Description : (1) When network low-resources status is set, transmit periodic ICMP Source Quench Error
*                   Messages to all individual IP source host addresses sending IP packets to this host :
*
*                   (a) Validate IP Source Host Address :
*
*                       (1) ICMP Source Quench Error Messages should be transmitted to individual host 
*                           addresses ONLY.
*
*                       (2) Therefore, ICMP Source Quench Error Messages SHOULD NOT be transmitted to
*                           the following IP source host addresses :
*
*                           (A) This      Host                          RFC #1122, Section 3.2.1.3.(a)
*                           (B) Specified Host                          RFC #1122, Section 3.2.1.3.(b)
*
*                       See also 'net_ip.c  NetIP_RxPktValidate()  Note #9c' for invalid IP source host 
*                       addresses.
*
*                   (b) Search ICMP Transmit Source Quench List for entry with corresponding IP source 
*                       host address (see Notes #2a & #2c).
*
*                   (c) Transmit ICMP Source Quench Error Message to all individual IP source host 
*                       addresses sending IP packets to this host.
*
*                   See also 'NetICMP_HandlerRx()  Note #1a'.
*
*
* Argument(s) : pbuf        Pointer to network buffer that received an IP packet.
*               ----        Argument validated in NetICMP_HandlerRx()
*                                                   by NetIP_Rx().
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_HandlerRx().
*$PAGE*
* Note(s)     : (2) (a) ICMP Transmit Source Quench List totals the number of ICMP Source Quench Error Messages
*                       transmitted to specific IP source host addresses.
*
*                   (b) ICMP transmit source quench entries are linked to form an ICMP Transmit Source Quench
*                       List.  In the diagram below, the horizontal row represents the list of ICMP transmit
*                       source quench entries.
*
*                       (1) 'NetICMP_TxSrcQuenchListHead' points to the head of the list; 
*                           'NetICMP_TxSrcQuenchListTail' points to the tail of the list.
*
*                       (2) ICMP transmit source quench entries' 'PrevPtr' & 'NextPtr' doubly-link each 
*                           entry to form the ICMP Transmit Source Quench List.
*
*                   (c) For any ICMP Transmit Source Quench List lookup, all ICMP transmit source quench
*                       entries are searched in order to find the entry with the appropriate IP address--
*                       i.e. the entry with the corresponding source host address (see Note #2a).
*
*                   (d) New ICMP transmit source quench entries are added at the head of the ICMP Transmit
*                       Source Quench List.  This allows faster lookup for recently added entries.
*
*                   (e) As ICMP transmit source quench entries are added into the list, older entries migrate
*                       to the tail of the list.  If the list is full & a new entry is needed, then the oldest
*                       entry at the tail of the list is removed for allocation.
*
*                   (f) The ICMP Transmit Source Quench List is cleared only when the network low-resources
*                       status clears (see 'NetICMP_HandlerRx()  Note #1b').
*
*
*                                 |                                                             |
*                                 |<------- List of ICMP Transmit Source Quench Entries ------->|
*                                 |                       (see Note #2b)                        |
*
*                            New ICMP entries                                      Oldest entry in ICMP
*                            inserted at head                                   Transmit Source Quench List
*                             (see Note #2d)                                          (see Note #2e)
*
*                                    |                 NextPtr                               |
*                                    |             (see Note #2b2)                           |
*                                    v                    |                                  v
*                                                         |
*             Head of ICMP        -------       -------   v   -------       -------       -------      (see Note #2b1)
*           Transmit Source  ---->|     |------>|     |------>|     |------>|     |------>|     |
*             Quench List         |     |       |     |       |     |       |     |       |     |        Tail of ICMP
*                                 |     |<------|     |<------|     |<------|     |<------|     |<---- Transmit Source
*           (see Note #2b1)       -------       -------   ^   -------       -------       -------        Quench List
*                                                         |
*                                                         |
*                                                      PrevPtr
*                                                  (see Note #2b2)
*
*
*               (3) The following IP header fields MUST be decoded &/or converted from network-order to host-
*                   order BEFORE any ICMP Source Quench List search & error message transmit (see 'net_ip.c
*                   NetIP_RxPktValidate()  Note #3') :
*
*                   (a) Source Address
*
*               (4) (a) See 'net_ip.h  IP ADDRESS DEFINES  Notes #2 & #3' for supported IP addresses.
*
*                   (b) Default case already invalidated in NetIP_RxPktValidate().  However, the default case
*                       is included as an extra precaution in case 'IP_AddrSrc' is incorrectly modified.
*
*               (5) During ICMP transmit source quench entry initialization, some entry values were previously
*                   initialized in NetICMP_HandlerTxSrcQuenchGet() when the entry was allocated from the pool.
*                   These entry values do NOT need to be re-initialized but are shown for completeness.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
static  void  NetICMP_HandlerTxSrcQuench (NET_BUF  *pbuf)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR                   cpu_sr;
#endif
    NET_BUF_HDR             *pbuf_hdr;
    NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench;
    NET_ICMP_TX_SRC_QUENCH  *pentry_next;
    CPU_BOOLEAN              found;
    CPU_BOOLEAN              tx_src_quench;
    CPU_INT16U               th;
    NET_ERR                  err;


    pbuf_hdr = &pbuf->Hdr;

                                                                        /* ----------- VALIDATE IP SRC ADDR ----------- */
                                                                        /* See Notes #1a & #3a.                         */
    if (pbuf_hdr->IP_AddrSrc == NET_IP_ADDR_THIS_HOST) {                /* Chk         'This Host' (see Note #1a2A).    */
        return;
    }

    if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_CLASS_A_MASK) == NET_IP_ADDR_CLASS_A) { 
                                                                        /* Chk Class-A 'This Host' (see Note #1a2B).    */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_CLASS_A_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_A_MASK_HOST)) {
             return;
        }

    } else if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_CLASS_B_MASK) == NET_IP_ADDR_CLASS_B) {
                                                                        /* Chk Class-B 'This Host' (see Note #1a2B).    */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_CLASS_B_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_B_MASK_HOST)) {
             return;
        }

    } else if ((pbuf_hdr->IP_AddrSrc & NET_IP_ADDR_CLASS_C_MASK) == NET_IP_ADDR_CLASS_C) {
                                                                        /* Chk Class-C 'This Host' (see Note #1a2B).    */
        if ((pbuf_hdr->IP_AddrSrc  & NET_IP_ADDR_CLASS_C_MASK_HOST) ==
            (NET_IP_ADDR_THIS_HOST & NET_IP_ADDR_CLASS_C_MASK_HOST)) {
             return;
        }

    } else {                                                            /* Discard invalid addr class (see Note #4).    */
        return;
    }


                                                                        /* ------- SRCH ICMP TX SRC QUENCH LIST ------- */
    ptx_src_quench = NetICMP_TxSrcQuenchListHead;                       /* Start @ ICMP Tx Src Quench List head.        */
    found          = DEF_NO;
    tx_src_quench  = DEF_NO;

    while ((ptx_src_quench != (NET_ICMP_TX_SRC_QUENCH *)0) &&           /* Srch ICMP Tx Src Quench List ...             */
           (found          ==  DEF_NO)) {                               /* ... until entry found.                       */

        pentry_next = (NET_ICMP_TX_SRC_QUENCH *)ptx_src_quench->NextPtr;
        found       = (pbuf_hdr->IP_AddrSrc  == ptx_src_quench->Addr) ? DEF_YES : DEF_NO;
        if (found != DEF_YES) {                                         /* If NOT found, adv to next entry.             */
            ptx_src_quench = pentry_next;

        } else {
            ptx_src_quench->TxCtr++;
            CPU_CRITICAL_ENTER();
            th = NetICMP_TxSrcQuenchTxTh_nbr;
            CPU_CRITICAL_EXIT();
            if (ptx_src_quench->TxCtr >= th) {                          /* If tx ctr > th,                              */
                tx_src_quench = DEF_YES;                                /* .. tx ICMP Src Quench Err Msg.               */
            }
        }
    }

    if (found != DEF_YES) {                                     /* If ICMP tx src quench entry NOT found, ...           */
        ptx_src_quench = NetICMP_HandlerTxSrcQuenchGet(&err);   /* ... get  new entry,                    ...           */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        if (ptx_src_quench == (NET_ICMP_TX_SRC_QUENCH *)0) {
            return;
        }
#endif
                                                                /* ... init new entry,                    ...           */
        ptx_src_quench->Addr    =  pbuf_hdr->IP_AddrSrc;
#if 0                                                           /* Init'd in NetICMP_HandlerTxSrcQuenchGet() ..         */
                                                                /* ... (see Note #5).                                   */
        ptx_src_quench->TxCtr   =  0;
                                                                /* Init'd in NetICMP_HandlerTxSrcQuenchInsert().        */
        ptx_src_quench->PrevPtr = (NET_ICMP_TX_SRC_QUENCH *)0;
        ptx_src_quench->NextPtr = (NET_ICMP_TX_SRC_QUENCH *)0;
#endif

        NetICMP_HandlerTxSrcQuenchInsert(ptx_src_quench);       /* ... insert new entry into list, &      ...           */

        tx_src_quench           =  DEF_YES;                     /* ... tx ICMP Src Quench Err Msg.                      */
    }


/*$PAGE*/
                                                                /* ------------ TX ICMP SRC QUENCH ERR MSG ------------ */
    if (tx_src_quench != DEF_NO) {
        NetICMP_TxMsgErr(pbuf,
                         NET_ICMP_MSG_TYPE_SRC_QUENCH,
                         NET_ICMP_MSG_CODE_SRC_QUENCH,
                         NET_ICMP_MSG_PTR_NONE,
                        &err);
        if (err == NET_ICMP_ERR_NONE) {                         /* If ICMP Src Quench Err Msg successfully tx'd, ...    */
            ptx_src_quench->TxCtr = 0;                          /* ... clr tx ctr.                                      */

        } else if (found != DEF_YES) {                          /* Else if new entry, set tx ctr to th val for retry.   */
            CPU_CRITICAL_ENTER();
            ptx_src_quench->TxCtr = NetICMP_TxSrcQuenchTxTh_nbr;
            CPU_CRITICAL_EXIT();
        }
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetICMP_HandlerTxSrcQuenchClrList()
*
* Description : Clear ICMP Transmit Source Quench List when network low-resources status clears.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_HandlerRx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
static  void  NetICMP_HandlerTxSrcQuenchClrList (void)
{
    NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench;
    NET_ICMP_TX_SRC_QUENCH  *pentry_next;


    ptx_src_quench = NetICMP_TxSrcQuenchListHead;
    while (ptx_src_quench != (NET_ICMP_TX_SRC_QUENCH *)0) {
        pentry_next        = (NET_ICMP_TX_SRC_QUENCH *)ptx_src_quench->NextPtr;
        NetICMP_HandlerTxSrcQuenchFree(ptx_src_quench);
        ptx_src_quench     =  pentry_next;
    }

    NetICMP_TxSrcQuenchListHead = (NET_ICMP_TX_SRC_QUENCH *)0;
    NetICMP_TxSrcQuenchListTail = (NET_ICMP_TX_SRC_QUENCH *)0;
}
#endif


/*
*********************************************************************************************************
*                                   NetICMP_HandlerTxSrcQuenchClr()
*
* Description : Clear ICMP transmit source quench entry controls.
*
* Argument(s) : ptx_src_quench      Pointer to an ICMP transmit source quench entry.
*               --------------      Argument validated in NetICMP_Init(),
*                                                         NetICMP_HandlerTxSrcQuenchGet(),
*                                                         NetICMP_HandlerTxSrcQuenchFree().
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_Init(),
*               NetICMP_HandlerTxSrcQuenchGet(),
*               NetICMP_HandlerTxSrcQuenchFree().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
static  void  NetICMP_HandlerTxSrcQuenchClr (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench)
{
    ptx_src_quench->PrevPtr = (NET_ICMP_TX_SRC_QUENCH *)0;
    ptx_src_quench->NextPtr = (NET_ICMP_TX_SRC_QUENCH *)0;
    ptx_src_quench->Addr    =  NET_IP_ADDR_NONE;
    ptx_src_quench->TxCtr   =  0;
    ptx_src_quench->Flags   =  NET_ICMP_FLAG_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetICMP_HandlerTxSrcQuenchGet()
*
* Description : (1) Allocate & initialize an ICMP transmit source quench entry :
*
*                   (a) Get an     ICMP transmit source quench entry
*                   (b) Validate   ICMP transmit source quench entry
*                   (c) Initialize ICMP transmit source quench entry
*                   (d) Update     ICMP transmit source quench pool statistics
*                   (e) Return pointer to ICMP transmit source quench entry
*                         OR
*                       Null pointer & error code, on failure
*
*               (2) The ICMP transmit source quench pool is implemented as a stack :
*
*                   (a) 'NetICMP_TxSrcQuenchPoolPtr' points to the head of the pool.
*
*                   (b) Entries' 'NextPtr's link each entry to form   the pool stack.
*
*                   (c) Entries are inserted & removed at the head of the pool stack.
*
*
*                            ICMP transmit source
*                               quench entries
*                             inserted & removed
*                                at the head
*                               (see Note #2c)
*
*                                     |                 NextPtr
*                                     |             (see Note #2b)
*                                     v                    |
*                                                          |
*            ICMP Transmit         -------       -------   v   -------       -------       -------
*         Source Quench Pool  ---->|     |------>|     |------>|     |------>|     |------>|     |
*               Pointer            |     |       |     |       |     |       |     |       |     |
*                                  |     |       |     |       |     |       |     |       |     |
*           (see Note #2a)         -------       -------       -------       -------       -------
*
*                                  |                                                             |
*                                  |<---- Pool of Free ICMP Transmit Source Quench Entries ----->|
*                                  |                       (see Note #2)                         |
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ICMP_ERR_NONE                   ICMP transmit source quench entry
*                                                                       successfully allocated & initialized.
*                               NET_ICMP_ERR_NONE_AVAIL             NO available entries to allocate.
*                               NET_ICMP_ERR_INVALID_TYPE           ICMP transmit source quench entry is NOT
*                                                                       a valid entry type.
*
* Return(s)   : Pointer to ICMP transmit source quench entry, if NO errors.
*
*               Pointer to NULL,                              otherwise.
*
* Caller(s)   : NetICMP_HandlerTxSrcQuench().
*
* Note(s)     : (3) (a) ICMP transmit source quench pool is accessed by 'NetICMP_TxSrcQuenchPoolPtr' during
*                       execution of
*
*                       (1) NetICMP_Init()
*                       (2) NetICMP_HandlerTxSrcQuenchGet()
*                       (3) NetICMP_HandlerTxSrcQuenchFree()
*
*                   (b) Since the primary tasks of the network protocol suite are prevented from running 
*                       concurrently (see 'net.h  Note #2'), it is NOT necessary to protect the shared 
*                       resources of the ICMP transmit source quench pool since no asynchronous access
*                       from other network tasks is possible.
*
*               (4) 'No entry available' case NOT possible during correct operation of the ICMP Transmit
*                   Source Quench List.  However, the 'else' case is included as an extra precaution in the
*                   case that the ICMP Transmit Source Quench List is incorrectly modified &/or corrupted.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
static  NET_ICMP_TX_SRC_QUENCH  *NetICMP_HandlerTxSrcQuenchGet (NET_ERR  *perr)
{
    NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench;
    NET_ERR                  stat_err;


                                                                        /* ---------- GET ICMP TX SRC QUENCH ---------- */
    if (NetICMP_TxSrcQuenchPoolPtr != (NET_ICMP_TX_SRC_QUENCH *)0) {    /* If ICMP tx src quench pool NOT empty,        */
                                                                        /* ... get tx src quench entry from pool.       */
        ptx_src_quench              = (NET_ICMP_TX_SRC_QUENCH *)NetICMP_TxSrcQuenchPoolPtr;
        NetICMP_TxSrcQuenchPoolPtr  = (NET_ICMP_TX_SRC_QUENCH *)ptx_src_quench->NextPtr;

                                                                        /* If ICMP Tx Src Quench List NOT empty, ..     */
    } else if (NetICMP_TxSrcQuenchListTail != (NET_ICMP_TX_SRC_QUENCH *)0) {
                                                                        /* ... get tx src quench entry from list tail.  */
        ptx_src_quench              = (NET_ICMP_TX_SRC_QUENCH *)NetICMP_TxSrcQuenchListTail;
        NetICMP_HandlerTxSrcQuenchRemove(ptx_src_quench);               
        ptx_src_quench              = (NET_ICMP_TX_SRC_QUENCH *)NetICMP_TxSrcQuenchPoolPtr;
        NetICMP_TxSrcQuenchPoolPtr  = (NET_ICMP_TX_SRC_QUENCH *)ptx_src_quench->NextPtr;


    } else {                                                            /* Else none avail, rtn err (see Note #4).      */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrNoneAvailCtr);
       *perr =   NET_ICMP_ERR_NONE_AVAIL;
        return ((NET_ICMP_TX_SRC_QUENCH *)0);

    }


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                         /* ------- VALIDATE ICMP TX SRC QUENCH -------- */
    if (ptx_src_quench->Type != NET_ICMP_TYPE_TX_SRC_QUENCH) {
        NetICMP_HandlerTxSrcQuenchDiscard(ptx_src_quench);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrInvalidTypeCtr);
       *perr =   NET_ICMP_ERR_INVALID_TYPE;
        return ((NET_ICMP_TX_SRC_QUENCH *)0);
    }
#endif

                                                                        /* --------- INIT ICMP TX SRC QUENCH ---------- */
    NetICMP_HandlerTxSrcQuenchClr(ptx_src_quench);
    DEF_BIT_SET(ptx_src_quench->Flags, NET_ICMP_FLAG_USED);             /* Set entry as used.                           */

                                                                        /* --- UPDATE ICMP TX SRC QUENCH POOL STATS --- */
    NetStat_PoolEntryUsedInc(&NetICMP_TxSrcQuenchPoolStat, &stat_err);

   *perr =  NET_ICMP_ERR_NONE;

    return (ptx_src_quench);                                            /* ---------- RTN ICMP TX SRC QUENCH ---------- */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetICMP_HandlerTxSrcQuenchFree()
*
* Description : (1) Free an ICMP transmit source quench entry :
*
*                   (a) Clear  ICMP transmit source quench entry controls
*                   (b) Free   ICMP transmit source quench entry back to pool
*                   (c) Update ICMP transmit source quench pool statistics
*
*
* Argument(s) : ptx_src_quench      Pointer to an ICMP transmit source quench entry.
*               --------------      Argument validated in NetICMP_HandlerTxSrcQuenchClrList(),
*                                                         NetICMP_HandlerTxSrcQuenchRemove().
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_HandlerTxSrcQuenchClrList(),
*               NetICMP_HandlerTxSrcQuenchRemove().
*
* Note(s)     : (2) #### To prevent freeing an ICMP transmit source quench entry already freed via
*                   previous   ICMP transmit source quench free, NetICMP_HandlerTxSrcQuenchFree()
*                   checks the ICMP transmit source quench entry's 'USED' flag BEFORE freeing the
*                   ICMP transmit source quench entry.
*
*                   This prevention is only best-effort since any invalid duplicate ICMP transmit
*                   source quench frees MAY be asynchronous to potentially valid ICMP transmit
*                   source quench gets.  Thus the invalid ICMP transmit source quench free(s)
*                   MAY corrupt the ICMP transmit source quench entry's valid operation(s).
*
*                   However, since the primary tasks of the network protocol suite are prevented 
*                   from running concurrently (see 'net.h  Note #2'), it is NOT necessary to protect
*                   ICMP resources from possible corruption since no asynchronous access from other
*                   network tasks is possible.
*********************************************************************************************************
*/

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
static  void  NetICMP_HandlerTxSrcQuenchFree (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN  used;
#endif
    NET_ERR      err;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------- VALIDATE TYPE ------------------ */
    if (ptx_src_quench->Type != NET_ICMP_TYPE_TX_SRC_QUENCH) {
        NetICMP_HandlerTxSrcQuenchDiscard(ptx_src_quench);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrInvalidTypeCtr);
        return;
    }
                                                                /* --------- VALIDATE ICMP TX SRC QUENCH USED --------- */
    used = DEF_BIT_IS_SET(ptx_src_quench->Flags, NET_ICMP_FLAG_USED);
    if (used != DEF_YES) {                                      /* If ICMP tx src quench NOT used, ...                  */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrNotUsedCtr);
        return;                                                 /* ... rtn but do NOT free (see Note #2).               */
    }
#endif

                                                                /* -------------- CLR ICMP TX SRC QUENCH -------------- */
    DEF_BIT_CLR(ptx_src_quench->Flags, NET_ICMP_FLAG_USED);     /* Set ICMP tx src quench entry as NOT used.            */
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    NetICMP_HandlerTxSrcQuenchClr(ptx_src_quench);
#endif    

                                                                /* ------------- FREE ICMP TX SRC QUENCH -------------- */
    ptx_src_quench->NextPtr    = NetICMP_TxSrcQuenchPoolPtr;
    NetICMP_TxSrcQuenchPoolPtr = ptx_src_quench;

                                                                /* ------- UPDATE ICMP TX SRC QUENCH POOL STATS ------- */
    NetStat_PoolEntryUsedDec(&NetICMP_TxSrcQuenchPoolStat, &err);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetICMP_HandlerTxSrcQuenchRemove()
*
* Description : (1) Remove an ICMP transmit source quench entry from the ICMP Transmit Source Quench List :
*
*                   (a) Remove ICMP transmit source quench entry from list
*                   (b) Free   ICMP transmit source quench entry back to pool
*
*
* Argument(s) : ptx_src_quench      Pointer to an ICMP transmit source quench entry.
*               --------------      Argument validated in NetICMP_HandlerTxSrcQuenchGet().
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_HandlerTxSrcQuenchGet().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
static  void  NetICMP_HandlerTxSrcQuenchRemove (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench)
{
                                                                /* ------- REMOVE ICMP TX SRC QUENCH FROM LIST -------- */
    NetICMP_HandlerTxSrcQuenchUnlink(ptx_src_quench);
                                                                /* ------------- FREE ICMP TX SRC QUENCH -------------- */
    NetICMP_HandlerTxSrcQuenchFree(ptx_src_quench);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetICMP_HandlerTxSrcQuenchInsert()
*
* Description : Insert an ICMP transmit source quench entry into the ICMP Transmit Source Quench List at
*               the head of the list.
*
* Argument(s) : ptx_src_quench      Pointer to an ICMP transmit source quench entry.
*               --------------      Argument checked in NetICMP_HandlerTxSrcQuench().
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_HandlerTxSrcQuench().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
static  void  NetICMP_HandlerTxSrcQuenchInsert (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench)
{
    ptx_src_quench->PrevPtr = (NET_ICMP_TX_SRC_QUENCH *)0;
    ptx_src_quench->NextPtr = (NET_ICMP_TX_SRC_QUENCH *)NetICMP_TxSrcQuenchListHead;

    if (NetICMP_TxSrcQuenchListHead != (NET_ICMP_TX_SRC_QUENCH *)0) {   /* If list NOT empty, insert before head.       */
        NetICMP_TxSrcQuenchListHead->PrevPtr = ptx_src_quench;
    } else {                                                            /* Else add first entry to list.                */
        NetICMP_TxSrcQuenchListTail          = ptx_src_quench;
    }
    NetICMP_TxSrcQuenchListHead = ptx_src_quench;                       /* Insert entry @ list head.                    */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetICMP_HandlerTxSrcQuenchUnlink()
*
* Description : Unlink an ICMP transmit source quench entry from the ICMP Transmit Source Quench List.
*
* Argument(s) : ptx_src_quench      Pointer to an ICMP transmit source quench entry.
*               --------------      Argument validated in NetICMP_HandlerTxSrcQuenchRemove().
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_HandlerTxSrcQuenchRemove().
*
* Note(s)     : (1) (a) Since NetICMP_HandlerTxSrcQuenchUnlink() called ONLY to remove & free entries from
*                       list tail, it is NOT necessary to implement unlink to remove entry from any position 
*                       in the list.  However, unlink from any position in the list implemented for correctness
*                       & completeness.
*
*                   (b) For a similar reason, it is NOT necessary to clear the entry's previous & next pointers.
*                       However, pointers cleared to NULL shown for correctness & completeness.
*********************************************************************************************************
*/

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
static  void  NetICMP_HandlerTxSrcQuenchUnlink (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench)
{
    NET_ICMP_TX_SRC_QUENCH  *pentry_prev;
    NET_ICMP_TX_SRC_QUENCH  *pentry_next;

                                                                        /* --- UNLINK ICMP TX SRC QUENCH FROM LIST ---- */
    pentry_prev = ptx_src_quench->PrevPtr;
    pentry_next = ptx_src_quench->NextPtr;
                                                                        /* Point prev entry to next entry.              */
    if (pentry_prev != (NET_ICMP_TX_SRC_QUENCH *)0) {
        pentry_prev->NextPtr        = pentry_next;
    } else {
        NetICMP_TxSrcQuenchListHead = pentry_next;
    }
                                                                        /* Point next entry to prev entry.              */
    if (pentry_next != (NET_ICMP_TX_SRC_QUENCH *)0) {
        pentry_next->PrevPtr        = pentry_prev;
    } else {
        NetICMP_TxSrcQuenchListTail = pentry_prev;
    }

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)                             /* Clr ICMP src quench ptrs (see Note #1b).     */
    ptx_src_quench->PrevPtr = (NET_ICMP_TX_SRC_QUENCH *)0;
    ptx_src_quench->NextPtr = (NET_ICMP_TX_SRC_QUENCH *)0;
#endif
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetICMP_HandlerTxSrcQuenchDiscard()
*
* Description : (1) Discard an invalid/corrupted ICMP transmit source quench entry  :
*
*                   (a) Discard ICMP transmit source quench entry from available pool       See Note #2
*                   (b) Update  ICMP transmit source quench pool statistics
*
*               (2) Assumes ICMP transmit source quench entry is invalid/corrupt & MUST be removed.
*                   ICMP transmit source quench entry removed simply by NOT returning the entry back
*                   to the ICMP transmit source quench pool.
*
*
* Argument(s) : pcache      Pointer to an invalid/corrupted ICMP transmit source quench entry.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_HandlerTxSrcQuenchGet(),
*               NetICMP_HandlerTxSrcQuenchFree().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if ((NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED) && \
     (NET_ERR_CFG_ARG_CHK_DBG_EN    == DEF_ENABLED))
static  void  NetICMP_HandlerTxSrcQuenchDiscard (NET_ICMP_TX_SRC_QUENCH  *ptx_src_quench)
{
    NET_ERR  stat_err;

                                                                /* ------------ DISCARD ICMP TX SRC QUENCH ------------ */
    if (ptx_src_quench == (NET_ICMP_TX_SRC_QUENCH *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetICMP_ErrNullPtrCtr);
        return;
    }

                                                                /* --------------- UPDATE DISCARD STATS --------------- */
    NetStat_PoolEntryLostInc(&NetICMP_TxSrcQuenchPoolStat, &stat_err);
}
#endif

