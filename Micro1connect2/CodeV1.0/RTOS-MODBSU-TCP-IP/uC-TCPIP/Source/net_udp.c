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
*                                          NETWORK UDP LAYER
*                                      (USER DATAGRAM PROTOCOL)
*
* Filename      : net_udp.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports User Datagram Protocol as described in RFC #768.
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

#define    NET_UDP_MODULE
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

                                                                            /* --------------- RX FNCTS --------------- */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetUDP_RxPktValidateBuf  (NET_BUF_HDR       *pbuf_hdr,
                                        NET_ERR           *perr);
#endif

static  void  NetUDP_RxPktValidate     (NET_BUF           *pbuf,
                                        NET_BUF_HDR       *pbuf_hdr,
                                        NET_UDP_HDR       *pudp_hdr,
                                        NET_ERR           *perr);


static  void  NetUDP_RxPktDemuxDatagram(NET_BUF           *pbuf,
                                        NET_ERR           *perr);

#if ((NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_APP     ) || \
     (NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_SOCK_APP))
static  void  NetUDP_RxPktDemuxAppData (NET_BUF           *pbuf,
                                        NET_ERR           *perr);
#endif


static  void  NetUDP_RxPktFree         (NET_BUF           *pbuf);

static  void  NetUDP_RxPktDiscard      (NET_BUF           *pbuf,
                                        NET_ERR           *perr);


                                                                            /* --------------- TX FNCTS --------------- */

static  void  NetUDP_Tx                (NET_BUF           *pbuf,
                                        NET_IP_ADDR        src_addr,
                                        NET_UDP_PORT_NBR   src_port,
                                        NET_IP_ADDR        dest_addr,
                                        NET_UDP_PORT_NBR   dest_port,
                                        NET_IP_TOS         TOS,
                                        NET_IP_TTL         TTL,
                                        CPU_INT16U         flags_udp,
                                        CPU_INT16U         flags_ip,
                                        void              *popts_ip,
                                        NET_ERR           *perr);

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetUDP_TxPktValidate     (NET_BUF_HDR       *pbuf_hdr,
                                        NET_UDP_PORT_NBR   src_port,
                                        NET_UDP_PORT_NBR   dest_port,
                                        CPU_INT16U         flags_udp,
                                        NET_ERR           *perr);
#endif

static  void  NetUDP_TxPktPrepareHdr   (NET_BUF           *pbuf,
                                        NET_BUF_HDR       *pbuf_hdr,
                                        NET_IP_ADDR        src_addr,
                                        NET_UDP_PORT_NBR   src_port,
                                        NET_IP_ADDR        dest_addr,
                                        NET_UDP_PORT_NBR   dest_port,
                                        CPU_INT16U         flags_udp,
                                        NET_ERR           *perr);

static  void  NetUDP_TxPktFree         (NET_BUF           *pbuf);

static  void  NetUDP_TxPktDiscard      (NET_BUF           *pbuf);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetUDP_Init()
*
* Description : (1) Initialize User Datagram Protocol Layer :
*
*                   (a) 
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

void  NetUDP_Init (void)
{
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             NetUDP_Rx()
*
* Description : (1) Process received datagrams & forward to socket or application layer :
*
*                   (a) Validate UDP packet
*                   (b) Demultiplex datagram to socket/application connection
*                   (c) Update receive statistics
*
*               (2) Although UDP data units are typically referred to as 'datagrams' (see RFC #768, Section
*                   'Introduction'), the term 'UDP packet' (see RFC #1983, 'packet') is used for UDP Receive
*                   until the packet is validated as a UDP datagram.
*
*
* Argument(s) : pbuf        Pointer to network buffer that received UDP packet.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UDP_ERR_NONE                UDP datagram successfully received & processed.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               ----- RETURNED BY NetUDP_RxPktDiscard() : -----
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_RxPktDemuxDatagram().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (3) NetUDP_Rx() blocked until network initialization completes.
*
*               (4) Network buffer already freed by higher layer; only increment error counter.
*
*               (5) RFC #792, Section 'Destination Unreachable Message : Description' states that 
*                   "if, in the destination host, the IP module cannot deliver the datagram because 
*                   the indicated ... process port is not active, the destination host may send a 
*                   destination unreachable message to the source host".
*********************************************************************************************************
*/
/*$PAGE*/
void  NetUDP_Rx (NET_BUF  *pbuf,
                 NET_ERR  *perr)
{
    NET_BUF_HDR  *pbuf_hdr;
    NET_UDP_HDR  *pudp_hdr;
    NET_ERR       msg_err;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit rx (see Note #3).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        NetUDP_RxPktDiscard(pbuf, perr);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrNullPtrCtr);
        return;
    }
#endif


    NET_CTR_STAT_INC(Net_StatCtrs.NetUDP_StatRxPktCtr);


                                                                /* ----------------- VALIDATE UDP PKT ----------------- */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NetUDP_RxPktValidateBuf(pbuf_hdr, perr);                    /* Validate rx'd buf.                                   */
    switch (*perr) {
        case NET_UDP_ERR_NONE:
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        default:
             NetUDP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif
    pudp_hdr = (NET_UDP_HDR *)&pbuf->Data[pbuf_hdr->TCP_UDP_HdrDataIx];
    NetUDP_RxPktValidate(pbuf, pbuf_hdr, pudp_hdr, perr);       /* Validate rx'd pkt.                                   */


                                                                /* ------------------ DEMUX DATAGRAM ------------------ */
    switch (*perr) {
        case NET_UDP_ERR_NONE:
             NetUDP_RxPktDemuxDatagram(pbuf, perr);
             break;


        case NET_UDP_ERR_INVALID_PORT_NBR:
        case NET_UDP_ERR_INVALID_LEN:
        case NET_UDP_ERR_INVALID_LEN_DATA:
        case NET_UDP_ERR_INVALID_CHK_SUM:
        default:
             NetUDP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


/*$PAGE*/
                                                                /* ----------------- UPDATE RX STATS ------------------ */
    switch (*perr) {                                            /* Chk err from NetUDP_RxPktDemuxDatagram().            */
        case NET_APP_ERR_NONE:
        case NET_SOCK_ERR_NONE:
             NET_CTR_STAT_INC(Net_StatCtrs.NetUDP_StatRxDatagramProcessedCtr);
            *perr = NET_UDP_ERR_NONE;
             break;


        case NET_ERR_RX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxPktDiscardedCtr);  /* See Note #4.                                 */
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_ERR_RX_DEST:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxDestCtr);
             NetICMP_TxMsgErr(pbuf,                             /* Tx ICMP port unreach (see Note #5).                  */
                              NET_ICMP_MSG_TYPE_DEST_UNREACH,
                              NET_ICMP_MSG_CODE_DEST_PORT,
                              NET_ICMP_MSG_PTR_NONE,
                             &msg_err);
             NetUDP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_ERR_INIT_INCOMPLETE:
        default:
             NetUDP_RxPktDiscard(pbuf, perr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetUDP_RxAppData()
*
* Description : (1) Deframe application data from received UDP packet buffer(s) :
*
*                   (a) Validate receive packet buffer(s)
*                   (b) Validate receive data buffer                                        See Note #4
*                   (c) Validate receive flags                                              See Note #5
*                   (d) Get any received IP options                                         See Note #6
*                   (e) Deframe application data from UDP packet buffer(s)
*                   (f) Free UDP packet buffer(s)
*
*
* Argument(s) : pbuf                Pointer to network buffer that received UDP datagram.
*
*               pdata_buf           Pointer to application buffer to receive application data.
*
*               data_buf_len        Size    of application receive buffer (in octets) [see Note #4].
*
*               flags               Flags to select receive options (see Note #5); bit-field flags logically OR'd :
*
*                                       NET_UDP_FLAG_NONE               No      UDP receive flags selected.
*                                       NET_UDP_FLAG_RX_DATA_PEEK       Receive UDP application data without consuming
*                                                                           the data; i.e. do NOT free any UDP receive
*                                                                           packet buffer(s).
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
*                               NET_UDP_ERR_NONE                UDP application data successfully deframed; check 
*                                                                   return value for number of data octets received.
*                               NET_UDP_ERR_INVALID_DATA_SIZE   UDP data receive buffer insufficient size; some, 
*                                                                   but not all, UDP application data deframed
*                                                                   into receive buffer (see Note #4b).
*
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_UDP_ERR_NULL_PTR            Argument 'pbuf'/'pdata_buf' passed a NULL pointer.
*                               NET_UDP_ERR_INVALID_FLAG        Invalid UDP flags.
*                                                               
*                                                               ------- RETURNED BY NetUDP_RxPktDiscard() : --------
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : Total application data octets deframed into receive buffer, if NO errors.
*
*               0,                                                          otherwise.
*
* Caller(s)   : NetUDP_RxAppDataHandler(),
*               NetSock_RxDataHandlerDatagram().
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #2].
*$PAGE*
* Note(s)     : (2) NetUDP_RxAppData() MUST be called with the global network lock already acquired.
*
*                   See also 'NetUDP_RxPktDemuxAppData()  Note #1a1A1b'.
*
*               (3) NetUDP_RxAppData() blocked until network initialization completes.
*
*               (4) (a) Application data receive buffer should be large enough to receive either ...
*
*                       (1) The maximum UDP datagram size (i.e. 65,507 octets)
*                             OR
*                       (2) The application's expected maximum UDP datagram size
*
*                   (b) If the application receive buffer size is NOT large enough for the received UDP datagram,
*                       the remaining application data octets are discarded & NET_UDP_ERR_INVALID_DATA_SIZE error 
*                       is returned.
*
*               (5) If UDP receive flag options that are NOT implemented are requested, NetUDP_RxAppData() aborts
*                   & returns appropriate error codes so that requested flag options are NOT silently ignored.
*
*               (6) (a) If ...
*
*                       (1) NO IP options were received with the UDP datagram
*                             OR
*                       (2) NO IP options receive buffer is provided by the application
*                             OR
*                       (3) IP options receive buffer NOT large enough for the received IP options
*
*                       ... then NO IP options are returned & any received IP options are silently discarded.
*
*                   (b) The IP options receive buffer size SHOULD be large enough to receive the maximum 
*                       IP options size, NET_IP_HDR_OPT_SIZE_MAX.
*
*                   (c) IP options are received from the first packet buffer.  In other words, if multiple
*                       packet buffers are received for a fragmented datagram, IP options are received from
*                       the first fragment of the datagram.
*
*                   (d) (1) (A) RFC #1122, Section 3.2.1.8 states that "all IP options ... received in datagrams 
*                               MUST be passed to the transport layer ... [which] MUST ... interpret those IP
*                               options that they understand and silently ignore the others".
*
*                           (B) RFC #1122, Section 4.1.3.2 adds that "UDP MUST pass any IP option that it receives
*                               from the IP layer transparently to the application layer".
*
*                       (2) #### Received IP options should be provided/decoded via appropriate IP layer API.
*
*                       #### NOT currently implemented.
*
*               (7) Pointers to variables that return values MUST be initialized to return PRIOR to all
*                   other validation or function handling in case of any error(s).
*
*               (8) Since pointer arithmetic is based on the specific pointer data type & inherent pointer
*                   data type size, pointer arithmetic operands :
*
*                   (a) MUST be in terms of the specific pointer data type & data type size; ...
*                   (b) SHOULD NOT & in some cases MUST NOT be cast to other data types or data type sizes.
*
*               (9) (a) On any internal receive     errors, UDP receive packets are     discarded.
*
*                   (b) On any external application errors, UDP receive packets are NOT discarded;
*                       the application MAY continue to attempt to receive the application data
*                       via NetUDP_RxAppData().
*
*              (10) #### IP options arguments may NOT be necessary.
*********************************************************************************************************
*/
/*$PAGE*/
CPU_INT16U  NetUDP_RxAppData (NET_BUF     *pbuf,
                              void        *pdata_buf,
                              CPU_INT16U   data_buf_len,
                              CPU_INT16U   flags,
                              void        *pip_opts_buf,
                              CPU_INT08U   ip_opts_buf_len,
                              CPU_INT08U  *pip_opts_len,
                              NET_ERR     *perr)
{
#if ((NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED))
    CPU_INT16U     flag_mask;
    CPU_BOOLEAN    used;
#endif
    CPU_BOOLEAN    peek;
    NET_BUF       *pbuf_head;
    NET_BUF       *pbuf_next;
    NET_BUF_HDR   *pbuf_head_hdr;
    NET_BUF_HDR   *pbuf_hdr;
    NET_BUF_SIZE   data_len_pkt;
    CPU_INT16U     data_len_buf_rem;
    CPU_INT16U     data_len_tot;
    CPU_INT08U    *p_data;
    CPU_INT08U    *pip_opts;
    CPU_INT08U     ip_opt_len;
    NET_ERR        err;
    NET_ERR        err_rtn;

                                                                
    if (pip_opts_len != (CPU_INT08U *)0) {                      /* Init len for err (see Note #7).                      */
       *pip_opts_len  =  0;
    }


#if ((NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED))
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit rx (see Notes #3 & #9b).  */
       *perr =  NET_ERR_INIT_INCOMPLETE;
        return (0);
    }

                                                                /* --------------- VALIDATE RX PKT BUFS --------------- */
    if (pbuf == (NET_BUF *)0) {
       *perr =  NET_UDP_ERR_NULL_PTR;                           /* See Note #9b.                                        */
        return (0);
    }

    used = NetBuf_IsUsed(pbuf);
    if (used != DEF_YES) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxPktDiscardedCtr);
       *perr =  NET_ERR_RX;                                     /* See Note #9b.                                        */
        return (0);
    }

                                                                /* --------------- VALIDATE RX DATA BUF --------------- */
    if (pdata_buf == (CPU_INT08U *)0) {
       *perr =  NET_UDP_ERR_NULL_PTR;                           /* See Note #9b.                                        */
        return (0);
    }
    if (data_buf_len < 1) {
       *perr =  NET_UDP_ERR_INVALID_DATA_SIZE;                  /* See Note #9b.                                        */
        return (0);
    }

                                                                /* ---------------- VALIDATE RX FLAGS ----------------- */
    flag_mask = NET_UDP_FLAG_NONE        |
                NET_UDP_FLAG_RX_DATA_PEEK; 

    if ((flags & ~flag_mask) != NET_UDP_FLAG_NONE) {            /* If any invalid flags req'd, rtn err (see Note #5).   */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrInvalidFlagsCtr);
       *perr =  NET_UDP_ERR_INVALID_FLAG;                       /* See Note #9b.                                        */
        return (0);
    }
#endif


/*$PAGE*/
                                                                /* ----------------- GET RX'D IP OPTS ----------------- */
                                                                /* See Note #6.                                         */
    pbuf_hdr = &pbuf->Hdr;
    if (pbuf_hdr->IP_OptPtr != (NET_BUF *)0) {                  /* If IP opts rx'd,                 & ...               */
        if (pip_opts_buf    != (void    *)0) {                  /* .. IP opts rx buf avail,         & ...               */
            if (ip_opts_buf_len >= pbuf_hdr->IP_HdrLen) {       /* .. IP opts rx buf size sufficient, ...               */
                pip_opts   = &pbuf->Data[pbuf_hdr->IP_HdrIx];
                ip_opt_len = (CPU_INT08U)pbuf_hdr->IP_HdrLen;
                Mem_Copy((void     *)pip_opts_buf,              /* .. copy IP opts into rx buf.                         */     
                         (void     *)pip_opts,
                         (CPU_SIZE_T) ip_opt_len);

               *pip_opts_len = ip_opt_len;
            }
        }
    }


                                                                /* ------------- DEFRAME UDP APP RX DATA -------------- */
    pbuf_head        = (NET_BUF     *) pbuf;
    pbuf_head_hdr    = (NET_BUF_HDR *)&pbuf_head->Hdr;
    p_data           = (CPU_INT08U  *) pdata_buf;
    data_len_buf_rem = (CPU_INT16U   ) data_buf_len;
    data_len_tot     = (CPU_INT16U   ) 0;
    err_rtn          =  NET_UDP_ERR_NONE;

    while ((pbuf != (NET_BUF *)0) &&                            /* Copy app rx data from avail pkt buf(s).              */
           (data_len_buf_rem > 0)) {

        pbuf_hdr  = &pbuf->Hdr;
        pbuf_next =  pbuf_hdr->NextBufPtr;

        if (data_len_buf_rem >= pbuf_hdr->DataLen) {            /* If rem data buf len >= pkt buf data len, ...         */
            data_len_pkt = (NET_BUF_SIZE)pbuf_hdr->DataLen;     /* ...      copy all      pkt buf data len.             */
        } else {
            data_len_pkt = (NET_BUF_SIZE)data_len_buf_rem;      /* Else lim copy to rem data buf len ...                */
            err_rtn      =  NET_UDP_ERR_INVALID_DATA_SIZE;      /* ... & rtn data size err code (see Note #4b).         */
        }

        NetBuf_DataRd((NET_BUF    *) pbuf,
                      (NET_BUF_SIZE) pbuf_hdr->DataIx,
                      (NET_BUF_SIZE) data_len_pkt,
                      (CPU_INT08U *) p_data,
                      (NET_ERR    *)&err);
        if (err != NET_BUF_ERR_NONE) {                          /* See Note #9a.                                        */
            NetUDP_RxPktDiscard(pbuf_head, perr);
            return (0);
        }
                                                                /* Update data ptr & lens.                              */
        p_data           +=             data_len_pkt;           /* MUST NOT cast ptr operand (see Note #8b).            */
        data_len_tot     += (CPU_INT16U)data_len_pkt;
        data_len_buf_rem -= (CPU_INT16U)data_len_pkt;

        pbuf              =  pbuf_next;
    }


                                                                /* ----------------- FREE UDP RX PKTS ----------------- */
    peek = DEF_BIT_IS_SET(flags, NET_UDP_FLAG_RX_DATA_PEEK);
    if (peek != DEF_YES) {                                      /* If peek opt NOT req'd, pkt buf(s) consumed : ...     */
        pbuf_head_hdr->NextPrimListPtr = (NET_BUF *)0;          /* ... unlink from any other pkt bufs/chains    ...     */
        NetUDP_RxPktFree(pbuf_head);                            /* ... & free pkt buf(s).                               */
    }


   *perr =  err_rtn;

    return (data_len_tot);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetUDP_TxAppData()
*
* Description : (1) Transmit data from Application layer(s) via UDP layer :
*
*                   (a) Acquire  network lock
*                   (b) Transmit application data via UDP Transmit
*                   (c) Release  network lock
*
*
* Argument(s) : p_data      Pointer to application data.
*
*               data_len    Length  of application data (in octets) [see Note #5].
*
*               src_addr    Source      IP  address.
*
*               src_port    Source      UDP port.
*
*               dest_addr   Destination IP  address.
*
*               dest_port   Destination UDP port.
*
*               TOS         Specific TOS to transmit UDP/IP packet
*                               (see 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
*
*               TTL         Specific TTL to transmit UDP/IP packet
*                               (see 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES') :
*
*                               NET_IP_HDR_TTL_MIN        1     minimum TTL transmit value
*                               NET_IP_HDR_TTL_MAX      255     maximum TTL transmit value
*                               NET_IP_HDR_TTL_DFLT             default TTL transmit value
*                               NET_IP_HDR_TTL_NONE       0     replace with default TTL
*
*               flags_udp   Flags to select UDP transmit options (see Note #4); bit-field flags logically OR'd :
*
*                               NET_UDP_FLAG_NONE               No UDP  transmit flags selected.
*                               NET_UDP_FLAG_TX_CHK_SUM_DIS     DISABLE transmit check-sums.
*                               NET_UDP_FLAG_TX_BLOCK           Transmit UDP application data with blocking,
*                                                                   if flag set; without blocking, if clear
*                                                                   (see Note #4a).
*
*               flags_ip    Flags to select IP  transmit options; bit-field flags logically OR'd :
*
*                               NET_IP_FLAG_NONE                No  IP transmit flags selected.
*                               NET_IP_FLAG_TX_DONT_FRAG        Set IP 'Don't Frag' flag.
*
*               popts_ip    Pointer to one or more IP options configuration data structures
*                               (see 'net_ip.h  IP HEADER OPTION CONFIGURATION DATA TYPES') :
*
*                               NULL                            NO IP transmit options configuration.
*                               NET_IP_OPT_CFG_ROUTE_TS         Route &/or Internet Timestamp options configuration.
*                               NET_IP_OPT_CFG_SECURITY         Security options configuration
*                                                                   (see 'net_ip.c  Note #1f').
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               ---- RETURNED BY NetUDP_TxAppDataHandler() : -----
*                               NET_UDP_ERR_NONE                Application data successfully prepared & 
*                                                                   transmitted via UDP layer.
*
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_ERR_INVALID_PROTOCOL        Invalid/unknown protocol type.
*
*                               NET_UDP_ERR_NULL_PTR            Argument 'p_data'   passed a NULL pointer.
*                               NET_UDP_ERR_INVALID_DATA_SIZE   Argument 'data_len' passed an invalid size
*                                                                   (see Notes #5b & #5a2B).
*                               NET_UDP_ERR_INVALID_LEN_DATA    Invalid protocol/data length.
*                               NET_UDP_ERR_INVALID_PORT_NBR    Invalid UDP port number.
*                               NET_UDP_ERR_INVALID_FLAG        Invalid UDP flag(s).
*
*                               NET_BUF_ERR_NULL_PTR            Argument(s) passed a NULL pointer.
*                               NET_BUF_ERR_INVALID_TYPE        Invalid buffer type.
*                               NET_BUF_ERR_NONE_AVAIL          NO available buffers to allocate.
*                               NET_BUF_ERR_INVALID_SIZE        Requested size is greater then the maximum buffer 
*                                                                   size available.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*                               NET_BUF_ERR_INVALID_LEN         Requested size & start index calculation overflows 
*                                                                   buffer's DATA area.
*
*                               NET_UTIL_ERR_NULL_PTR           Check-sum passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Check-sum passed a zero size.
*                               NET_UTIL_ERR_INVALID_PROTOCOL   Invalid data packet protocol.
*
*                               NET_ERR_TX                      Transmit error; packet(s) discarded.
*
*                                                               ----------- RETURNED BY NetOS_Lock() : -----------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*$PAGE*
* Return(s)   : Number of data octets transmitted, if NO errors.
*
*               0,                                 otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #2].
*
* Note(s)     : (2) NetUDP_TxAppData() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global 
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access 
*                   is asynchronous to other network protocol tasks.
*
*                   See also 'NetUDP_TxAppDataHandler()  Note #2'.
*
*               (3) NetUDP_TxAppData() blocked until network initialization completes.
*
*                   See      'NetUDP_TxAppDataHandler()  Note #3'.
*
*               (4) #### Some UDP transmit flag options NOT yet implemented :
*
*                   (a) NET_UDP_FLAG_TX_BLOCK
*
*                   See also 'NetUDP_TxPktValidate()  Note #2b'.
*
*               (5) (a) (1) Datagram transmission & reception MUST be atomic -- i.e. every single, complete
*                           datagram transmitted SHOULD be received as a single, complete datagram.  Thus, 
*                           each call to transmit data MUST be transmitted in a single, complete datagram.
*
*                       (2) (A) IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states that
*                               "if the message is too long to pass through the underlying protocol, send() 
*                               shall fail and no data shall be transmitted".
*
*                           (B) Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h 
*                               Note #1e'), if the requested datagram transmit data length is greater than 
*                               the UDP MTU, then NO data is transmitted & NET_UDP_ERR_INVALID_DATA_SIZE
*                               error is returned.
*
*                   (b) 'data_len' of 0 octets NOT allowed.
*********************************************************************************************************
*/

CPU_INT16U  NetUDP_TxAppData (void              *p_data,
                              CPU_INT16U         data_len,
                              NET_IP_ADDR        src_addr,
                              NET_UDP_PORT_NBR   src_port,
                              NET_IP_ADDR        dest_addr,
                              NET_UDP_PORT_NBR   dest_port,
                              NET_IP_TOS         TOS,
                              NET_IP_TTL         TTL,
                              CPU_INT16U         flags_udp,
                              CPU_INT16U         flags_ip,
                              void              *popts_ip,
                              NET_ERR           *perr)
{
    CPU_INT16U  data_len_tot;
    NET_ERR     err;


    NetOS_Lock(&err);                                           /* Acquire net lock (see Note #2b).                     */
    if (err !=  NET_OS_ERR_NONE) {
        return (0);
    }
                                                                /* Tx UDP app data.                                     */
    data_len_tot = NetUDP_TxAppDataHandler(p_data,
                                           data_len,
                                           src_addr,
                                           src_port,
                                           dest_addr,
                                           dest_port,
                                           TOS,
                                           TTL,
                                           flags_udp,
                                           flags_ip,
                                           popts_ip,
                                           perr);

    NetOS_Unlock();                                             /* Release net lock.                                    */

    return (data_len_tot);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetUDP_TxAppDataHandler()
*
* Description : (1) Prepare & transmit data from Application layer(s) via UDP layer :
*
*                   (a) Validate application data
*                   (b) Transmit application data via UDP Transmit :
*                       (1) Calculate/validate application data buffer size
*                       (2) Get buffer(s) for application data
*                       (3) Copy application data into UDP packet buffer(s)
*                       (4) Initialize UDP packet buffer controls
*                       (5) Free UDP packet buffer(s)
*
*
* Argument(s) : p_data      Pointer to application data.
*
*               data_len    Length  of application data (in octets) [see Note #5].
*
*               src_addr    Source      IP  address.
*
*               src_port    Source      UDP port.
*
*               dest_addr   Destination IP  address.
*
*               dest_port   Destination UDP port.
*
*               TOS         Specific TOS to transmit UDP/IP packet
*                               (see 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
*
*               TTL         Specific TTL to transmit UDP/IP packet
*                               (see 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES') :
*
*                               NET_IP_HDR_TTL_MIN        1     minimum TTL transmit value
*                               NET_IP_HDR_TTL_MAX      255     maximum TTL transmit value
*                               NET_IP_HDR_TTL_DFLT             default TTL transmit value
*                               NET_IP_HDR_TTL_NONE       0     replace with default TTL
*
*               flags_udp   Flags to select UDP transmit options (see Note #4); bit-field flags logically OR'd :
*
*                               NET_UDP_FLAG_NONE               No UDP  transmit flags selected.
*                               NET_UDP_FLAG_TX_CHK_SUM_DIS     DISABLE transmit check-sums.
*                               NET_UDP_FLAG_TX_BLOCK           Transmit UDP application data with blocking,
*                                                                   if flag set; without blocking, if clear
*                                                                   (see Note #4a).
*
*               flags_ip    Flags to select IP  transmit options; bit-field flags logically OR'd :
*
*                               NET_IP_FLAG_NONE                No  IP transmit flags selected.
*                               NET_IP_FLAG_TX_DONT_FRAG        Set IP 'Don't Frag' flag.
*
*               popts_ip    Pointer to one or more IP options configuration data structures
*                               (see 'net_ip.h  IP HEADER OPTION CONFIGURATION DATA TYPES') :
*
*                               NULL                            NO IP transmit options configuration.
*                               NET_IP_OPT_CFG_ROUTE_TS         Route &/or Internet Timestamp options configuration.
*                               NET_IP_OPT_CFG_SECURITY         Security options configuration
*                                                                   (see 'net_ip.c  Note #1f').
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UDP_ERR_NONE                Application data successfully prepared & 
*                                                                   transmitted via UDP layer.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_UDP_ERR_NULL_PTR            Argument 'p_data'   passed a NULL pointer.
*                               NET_UDP_ERR_INVALID_DATA_SIZE   Argument 'data_len' passed an invalid size
*                                                                   (see Notes #5b & #5a2B).
*
*                                                               ----------- RETURNED BY NetBuf_Get() : -----------
*                               NET_BUF_ERR_INVALID_SIZE        Requested size is greater then the maximum buffer 
*                                                                   size available.
*                               NET_BUF_ERR_INVALID_LEN         Requested size & start index calculation overflows 
*                                                                   buffer's DATA area.
*                               NET_BUF_ERR_INVALID_TYPE        Invalid buffer type.
*                               NET_BUF_ERR_NONE_AVAIL          NO available buffers to allocate.
*
*                                                               --------- RETURNED BY NetBuf_DataWr() : ----------
*                               NET_BUF_ERR_NULL_PTR            Argument(s) passed a NULL pointer.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
*                                                               ----------- RETURNED BY NetUDP_Tx() : ------------
*                               NET_ERR_INVALID_PROTOCOL        Invalid/unknown protocol type.
*                               NET_UDP_ERR_INVALID_LEN_DATA    Invalid protocol/data length.
*                               NET_UDP_ERR_INVALID_PORT_NBR    Invalid UDP port number.
*                               NET_UDP_ERR_INVALID_FLAG        Invalid UDP flag(s).
*                               NET_UTIL_ERR_NULL_PTR           Check-sum passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Check-sum passed a zero size.
*                               NET_UTIL_ERR_INVALID_PROTOCOL   Invalid data packet protocol.
*                               NET_ERR_TX                      Transmit error; packet(s) discarded.
*$PAGE*
* Return(s)   : Number of data octets transmitted, if NO errors.
*
*               0,                                 otherwise.
*
* Caller(s)   : NetSock_TxDataHandlerDatagram().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (2) NetUDP_TxAppDataHandler() is called by network protocol suite function(s) & MUST 
*                   be called with the global network lock already acquired.
*
*                   See also 'NetUDP_TxAppData()  Note #2'.
*
*               (3) NetUDP_TxAppDataHandler() blocked until network initialization completes.
*
*               (4) #### Some UDP transmit flag options NOT yet implemented :
*
*                   (a) NET_UDP_FLAG_TX_BLOCK
*
*                   See also 'NetUDP_TxPktValidate()  Note #2b'.
*
*               (5) (a) (1) Datagram transmission & reception MUST be atomic -- i.e. every single, complete
*                           datagram transmitted SHOULD be received as a single, complete datagram.  Thus, 
*                           each call to transmit data MUST be transmitted in a single, complete datagram.
*
*                       (2) (A) IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states that
*                               "if the message is too long to pass through the underlying protocol, send() 
*                               shall fail and no data shall be transmitted".
*
*                           (B) Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h 
*                               Note #1e'), if the requested datagram transmit data length is greater than 
*                               the UDP MTU, then NO data is transmitted & NET_UDP_ERR_INVALID_DATA_SIZE
*                               error is returned.
*
*                   (b) 'data_len' of 0 octets NOT allowed.
*
*               (6) On ANY transmit error, any remaining application data transmit is immediately aborted.
*********************************************************************************************************
*/

CPU_INT16U  NetUDP_TxAppDataHandler (void              *p_data,
                                     CPU_INT16U         data_len,
                                     NET_IP_ADDR        src_addr,
                                     NET_UDP_PORT_NBR   src_port,
                                     NET_IP_ADDR        dest_addr,
                                     NET_UDP_PORT_NBR   dest_port,
                                     NET_IP_TOS         TOS,
                                     NET_IP_TTL         TTL,
                                     CPU_INT16U         flags_udp,
                                     CPU_INT16U         flags_ip,
                                     void              *popts_ip,
                                     NET_ERR           *perr)
{
    NET_BUF       *pbuf;
    NET_BUF_HDR   *pbuf_hdr;
    NET_MTU        udp_mtu;
    NET_BUF_SIZE   buf_size_max;
    NET_BUF_SIZE   buf_size_max_data;
    NET_BUF_SIZE   data_ix_pkt;
    NET_BUF_SIZE   data_len_pkt;
    CPU_INT16U     data_len_tot;
    CPU_INT08U    *p_data_pkt;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit tx (see Note #3).         */
       *perr =  NET_ERR_INIT_INCOMPLETE;
        return (0);
    }
#endif


#if ((NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED) || \
     (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED))
                                                                /* ---------------- VALIDATE APP DATA ----------------- */
    if (p_data == (void *)0) {
       *perr =  NET_UDP_ERR_NULL_PTR;
        return (0);
    }
    if (data_len < 1) {                                         /* Validate data len (see Note #5b).                    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrTxInvalidSizeCtr);
       *perr =  NET_UDP_ERR_INVALID_DATA_SIZE;
        return (0);
    }
#endif

/*$PAGE*/
                                                                /* ------------------- TX APP DATA -------------------- */
                                                                /* Calc buf max data size.                              */
    udp_mtu           =  NET_UDP_MTU;
    data_ix_pkt       =  NET_BUF_DATA_TX_IX;
    buf_size_max      =  NetBuf_GetMaxSize((NET_BUF    *)0,
                                           (NET_BUF_SIZE)data_ix_pkt);

    buf_size_max_data = (NET_BUF_SIZE)DEF_MIN(buf_size_max, udp_mtu);

    if (data_len > buf_size_max_data) {                         /* If data len > max data size, abort tx ...            */
       *perr =  NET_UDP_ERR_INVALID_DATA_SIZE;                  /* ... & rtn size err (see Note #5a2B).                 */
        return (0);

    } else {                                                    /* Else lim pkt data len to data len.                   */
        data_len_pkt = (NET_BUF_SIZE)data_len;
    }

    data_len_tot =  0;
    p_data_pkt   = (CPU_INT08U *)p_data;
                                                                /* Get app data tx buf.                                 */
    pbuf = NetBuf_Get((NET_BUF_SIZE)data_len_pkt,
                      (NET_BUF_SIZE)data_ix_pkt,
                      (CPU_INT16U  )NET_BUF_FLAG_NONE,
                      (NET_ERR    *)perr);
    if (*perr != NET_BUF_ERR_NONE) {
         NetUDP_TxPktDiscard(pbuf);
         return (data_len_tot);
    }

    NetBuf_DataWr((NET_BUF    *)pbuf,                           /* Wr app data into app data tx buf.                    */
                  (NET_BUF_SIZE)data_ix_pkt,
                  (NET_BUF_SIZE)data_len_pkt,
                  (CPU_INT08U *)p_data_pkt,
                  (NET_ERR    *)perr);
    if (*perr != NET_BUF_ERR_NONE) {
         NetUDP_TxPktDiscard(pbuf);
         return (data_len_tot);
    }

                                                                /* Init app data tx buf ctrls.                          */
    pbuf_hdr                  = &pbuf->Hdr;
    pbuf_hdr->DataIx          = (CPU_INT16U  )data_ix_pkt;
    pbuf_hdr->DataLen         = (NET_BUF_SIZE)data_len_pkt;
    pbuf_hdr->TotLen          = (NET_BUF_SIZE)pbuf_hdr->DataLen;
    pbuf_hdr->ProtocolHdrType =  NET_PROTOCOL_TYPE_APP;

    NetUDP_Tx(pbuf,                                             /* Tx app data buf via UDP tx.                          */
              src_addr,
              src_port,
              dest_addr,
              dest_port,
              TOS,
              TTL,
              flags_udp,
              flags_ip,
              popts_ip,
              perr);
    if (*perr != NET_UDP_ERR_NONE) {
         return (data_len_tot);
    }

    NetUDP_TxPktFree(pbuf);                                     /* Free app data tx buf.                                */


    data_len_tot += data_len_pkt;                               /* Calc tot app data len tx'd.                          */



   *perr =  NET_UDP_ERR_NONE;

    return (data_len_tot);
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
*                                      NetUDP_RxPktValidateBuf()
*
* Description : Validate received buffer header as UDP protocol.
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header that received UDP packet.
*               --------    Argument validated in NetUDP_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UDP_ERR_NONE                Received buffer's UDP header validated.
*                               NET_ERR_INVALID_PROTOCOL        Buffer's protocol type is NOT UDP.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_Rx().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetUDP_RxPktValidateBuf (NET_BUF_HDR  *pbuf_hdr,
                                       NET_ERR      *perr)
{
                                                                /* --------------- VALIDATE UDP BUF HDR --------------- */
    if (pbuf_hdr->ProtocolHdrType != NET_PROTOCOL_TYPE_UDP) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxInvalidProtocolCtr);
       *perr = NET_ERR_INVALID_PROTOCOL;
        return;
    }

    if (pbuf_hdr->TCP_UDP_HdrDataIx == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

   *perr = NET_UDP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetUDP_RxPktValidate()
*
* Description : (1) Validate received UDP packet :
*
*                   (a) Validate the received packet's following UDP header fields :
*
*                       (1) Source      Port
*                       (2) Destination Port
*                       (3) Datagram Length                                 See Note #3
*                       (4) Check-Sum                                       See Note #4
*
*                   (b) Convert the following UDP header fields from network-order to host-order :
*
*                       (1) Source      Port                                See Notes #1bB1
*                       (2) Destination Port                                See Notes #1bB2
*                       (3) Datagram Length                                 See Notes #1bB3
*                       (4) Check-Sum                                       See Note  #4d
*
*                           (A) These fields are NOT converted directly in the received packet buffer's 
*                               data area but are converted in local or network buffer variables ONLY.
*
*                           (B) The following UDP header fields are converted & stored in network buffer
*                               variables :
*
*                               (1) Source      Port
*                               (2) Destination Port
*                               (3) Datagram Length
*
*                   (c) Update network buffer's protocol controls
*
*
* Argument(s) : pbuf        Pointer to network buffer that received UDP packet.
*               ----        Argument checked   in NetUDP_Rx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetUDP_Rx().
*
*               pudp_hdr    Pointer to received packet's UDP header.
*               --------    Argument validated in NetUDP_Rx()/NetUDP_RxPktValidateBuf().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UDP_ERR_NONE                Received packet validated.
*                               NET_UDP_ERR_INVALID_PORT_NBR    Invalid UDP port number.
*                               NET_UDP_ERR_INVALID_LEN         Invalid UDP datagram      length.
*                               NET_UDP_ERR_INVALID_LEN_DATA    Invalid UDP datagram data length.
*                               NET_UDP_ERR_INVALID_CHK_SUM     Invalid UDP check-sum.
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_Rx().
*$PAGE*
* Note(s)     : (2) See 'net_udp.h  UDP HEADER' for UDP header format.
*
*               (3) In addition to validating that the UDP Datagram Length is greater than or equal to the 
*                   minimum UDP header length,     the UDP Datagram Length is compared to the remaining IP 
*                   Datagram Length which should be identical.
*
*               (4) (a) UDP header Check-Sum field MUST be validated BEFORE (or AFTER) any multi-octet words
*                       are converted from network-order to host-order since "the sum of 16-bit integers can
*                       be computed in either byte order" [RFC #1071, Section 2.(B)].
*
*                       In other words, the UDP Datagram Check-Sum CANNOT be validated AFTER SOME but NOT ALL 
*                       multi-octet words have been converted from network-order to host-order.
*
*                   (b) However, ALL received packets' multi-octet words are converted in local or network 
*                       buffer variables ONLY (see Note #1bA).  Therefore, UDP Datagram Check-Sum may be 
*                       validated at any point.
*
*                   (c) The UDP Datagram Check-Sum MUST be validated AFTER the datagram length field has been
*                       validated so that the total UDP Datagram Length (in octets) will already be calculated
*                       for the UDP Check-Sum calculation.
*
*                       For efficiency, the UDP Datagram Check-Sum is validated AFTER all other UDP header 
*                       fields have been validated.  Thus, the iteration-intensive UDP Datagram Check-Sum is 
*                       calculated only after all other UDP header fields have been quickly validated.
*
*                   (d) (1) Before the UDP Datagram Check-Sum is validated, it is necessary to convert the 
*                           Check-Sum from network-order to host-order to verify whether the received UDP 
*                           datagram's Check-Sum is valid -- i.e. whether the UDP datagram was transmitted 
*                           with or without a computed Check-Sum (see RFC #768, Section 'Fields : Checksum').
*
*                       (2) Since the value that indicates no check-sum was computed for the received UDP
*                           datagram is one's-complement positive zero -- all check-sum bits equal to zero,
*                           a value that is endian-order independent -- it is NOT absolutely necessary to 
*                           convert the UDP Datagram Check-Sum from network-order to host-order.
*
*                           However, network data value macro's inherently convert data words from network
*                           word order to CPU word order.
*
*                           See also 'net_util.h  NETWORK DATA VALUE MACRO'S  Note #1a1'.
*
*                       (3) (A) Any UDP datagram received with NO computed check-sum is flagged so that "an
*                               application MAY optionally ... discard ... UDP datagrams without checksums" 
*                               (see RFC #1122, Section 4.1.3.4).
*
*                               #### API to handle/discard UDP datagrams without checksums NOT yet implemented.
*
*                           (B) UDP buffer flag value to clear was previously initialized in NetBuf_Get() when 
*                               the buffer was allocated.  This buffer flag value does NOT need to be re-cleared
*                               but is shown for completeness.
*
*                   (e) (1) In addition to the UDP datagram header & data, the UDP Check-Sum calculation
*                           includes "a pseudo header of information from the IP header ... conceptually
*                           prefixed to the UDP header [which] contains the source address, the destination
*                           address, the protocol, and the UDP length" (see RFC #768, Section 'Fields :
*                           Checksum').
*
*                       (2) Since network check-sum functions REQUIRE that 16-bit one's-complement check-
*                           sum calculations be performed on headers & data arranged in network-order (see 
*                           'net_util.c  NetUtil_16BitOnesCplChkSumDataVerify()  Note #4'), UDP pseudo-header 
*                           values MUST be set or converted to network-order.
*
*                   (f) RFC #768, Section 'Fields : Checksum' specifies that "the data [is] padded with zero
*                       octets at the end (if necessary) to make a multiple of two octets".
*
*                       See also 'net_util.c  NetUtil_16BitSumDataCalc()  Note #8'.
*
*               (5) (a) Since the minimum network buffer size MUST be configured such that the entire UDP 
*                       header MUST be received in a single packet (see 'net_buf.h  NETWORK BUFFER INDEX &
*                       SIZE DEFINES  Note #1c'), after the UDP header size is decremented from the first 
*                       packet buffer's remaining number of data octets, any remaining octets MUST be user 
*                       &/or application data octets.
*
*                       (1) Note that the 'Data' index is updated regardless of a null-size data length.
*
*                   (b) If additional packet buffers exist, the remaining IP datagram 'Data' MUST be user
*                       &/or application data.  Therefore, the 'Data' length does NOT need to be adjusted 
*                       but the 'Data' index MUST be updated.
*
*                   (c) #### Total UDP Datagram Length & Data Length is duplicated in ALL fragmented packet 
*                       buffers (may NOT be necessary; remove if unnecessary).
*$PAGE*
*               (6) RFC #1122, Sections 3.2.1 & 3.2.2 require that IP & ICMP packets with certain invalid
*                   header fields be "silently discarded".  However, NO RFC specifies how UDP should handle 
*                   received datagrams with invalid header fields.
*
*                   In addition, UDP is a "transaction oriented" protocol that does NOT guarantee "delivery
*                   and duplicate protection" of UDP datagrams (see RFC #768, Section 'Introduction').
*
*                   Therefore, it is assumed that ALL UDP datagrams with ANY invalid header fields SHOULD
*                   be silently discarded.
*
*               (7) (a) RFC #1122, Section 3.2.1.8 states that "all IP options ... received in datagrams 
*                       MUST be passed to the transport layer ... [which] MUST ... interpret those IP
*                       options that they understand and silently ignore the others".
*
*                   (b) RFC #1122, Section 4.1.3.2 adds that "UDP MUST pass any IP option that it receives
*                       from the IP layer transparently to the application layer".
*
*                   See also 'NetUDP_RxAppData()  Note #6d'.
*********************************************************************************************************
*/

static  void  NetUDP_RxPktValidate (NET_BUF      *pbuf,
                                    NET_BUF_HDR  *pbuf_hdr,
                                    NET_UDP_HDR  *pudp_hdr,
                                    NET_ERR      *perr)
{
    CPU_INT16U           udp_tot_len;
    CPU_INT16U           udp_data_len;
    NET_CHK_SUM          udp_chk_sum;
    CPU_BOOLEAN          udp_chk_sum_valid;
    NET_UDP_PSEUDO_HDR   udp_pseudo_hdr;
    NET_BUF             *pbuf_next;
    NET_BUF_HDR         *pbuf_next_hdr;



                                                                /* ---------------- VALIDATE UDP PORTS ---------------- */
    NET_UTIL_VAL_COPY_GET_NET_16(&pbuf_hdr->TCP_UDP_PortSrc,  &pudp_hdr->PortSrc);
    if (pbuf_hdr->TCP_UDP_PortSrc  == NET_UDP_PORT_NBR_RESERVED) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxHdrPortSrcCtr);
       *perr = NET_UDP_ERR_INVALID_PORT_NBR;
        return;
    }

    NET_UTIL_VAL_COPY_GET_NET_16(&pbuf_hdr->TCP_UDP_PortDest, &pudp_hdr->PortDest);
    if (pbuf_hdr->TCP_UDP_PortDest == NET_UDP_PORT_NBR_RESERVED) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxHdrPortDestCtr);
       *perr = NET_UDP_ERR_INVALID_PORT_NBR;
        return;
    }


                                                                /* ------------ VALIDATE UDP DATAGRAM LEN ------------- */
                                                                /* See Note #3.                                         */
    NET_UTIL_VAL_COPY_GET_NET_16(&udp_tot_len, &pudp_hdr->DatagramLen);
    pbuf_hdr->TCP_UDP_TotLen = udp_tot_len;
    if (pbuf_hdr->TCP_UDP_TotLen < NET_UDP_TOT_LEN_MIN) {       /* If datagram len <  min tot     len, rtn err.         */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxHdrDatagramLenCtr);
       *perr = NET_UDP_ERR_INVALID_LEN;
        return;
    }
    if (pbuf_hdr->TCP_UDP_TotLen > NET_UDP_TOT_LEN_MAX) {       /* If datagram len >  max tot     len, rtn err.         */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxHdrDatagramLenCtr);
       *perr = NET_UDP_ERR_INVALID_LEN;
        return;
    }

    if (pbuf_hdr->TCP_UDP_TotLen != pbuf_hdr->IP_DatagramLen) { /* If datagram len != IP datagram len, rtn err.         */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxHdrDatagramLenCtr);
       *perr = NET_UDP_ERR_INVALID_LEN;
        return;
    }


/*$PAGE*/
                                                                /* --------------- VALIDATE UDP CHK SUM --------------- */
                                                                /* See Note #4.                                         */
    NET_UTIL_VAL_COPY_GET_NET_16(&udp_chk_sum, &pudp_hdr->ChkSum);
    if (udp_chk_sum != NET_UDP_HDR_CHK_SUM_NONE) {              /* If chk sum rx'd, verify chk sum (see Note #4d).      */
                                                                /* Prepare UDP chk sum pseudo-hdr  (see Note #4e).      */
        udp_pseudo_hdr.AddrSrc     = (NET_IP_ADDR)NET_UTIL_HOST_TO_NET_32(pbuf_hdr->IP_AddrSrc);
        udp_pseudo_hdr.AddrDest    = (NET_IP_ADDR)NET_UTIL_HOST_TO_NET_32(pbuf_hdr->IP_AddrDest);
        udp_pseudo_hdr.Zero        = (CPU_INT08U )0x00;
        udp_pseudo_hdr.Protocol    = (CPU_INT08U )NET_IP_HDR_PROTOCOL_UDP;
        udp_pseudo_hdr.DatagramLen = (CPU_INT16U )NET_UTIL_HOST_TO_NET_16(pbuf_hdr->TCP_UDP_TotLen);

        udp_chk_sum_valid          =  NetUtil_16BitOnesCplChkSumDataVerify((void     *) pbuf,
                                                                           (void     *)&udp_pseudo_hdr,
                                                                           (CPU_INT16U) NET_UDP_PSEUDO_HDR_SIZE,
                                                                           (NET_ERR  *) perr);
        if (udp_chk_sum_valid != DEF_OK) {
            NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxHdrChkSumCtr);
           *perr = NET_UDP_ERR_INVALID_CHK_SUM;
            return;
        }

        DEF_BIT_SET(pbuf_hdr->Flags, NET_BUF_FLAG_RX_UDP_CHK_SUM_VALID);

    } else {                                                    /* Else discard or flag NO rx'd chk sum (see Note #4d3).*/
#if (NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN != DEF_DISABLED)
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxHdrChkSumCtr);
       *perr = NET_UDP_ERR_INVALID_CHK_SUM;
        return;
#endif
#if 0                                                           /* Clr'd in NetBuf_Get() [see Note #4d3B].              */
        DEF_BIT_CLR(pbuf_hdr->Flags, NET_BUF_FLAG_RX_UDP_CHK_SUM_VALID);
#endif
    }


                                                                /* ----------------- UPDATE BUF CTRLS ----------------- */
                                                                /* Calc UDP data len/ix (see Note #5a).                 */
    pbuf_hdr->TCP_UDP_HdrLen = NET_UDP_HDR_SIZE;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (pbuf_hdr->TCP_UDP_HdrLen > udp_tot_len) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxHdrDataLenCtr);
       *perr = NET_UDP_ERR_INVALID_LEN_DATA;
        return;
    }
    if (pbuf_hdr->TCP_UDP_HdrLen > pbuf_hdr->DataLen) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrRxHdrDataLenCtr);
       *perr = NET_UDP_ERR_INVALID_LEN_DATA;
        return;
    }
#endif
    udp_data_len               =  udp_tot_len - pbuf_hdr->TCP_UDP_HdrLen;
    pbuf_hdr->TCP_UDP_DataLen  =  udp_data_len;

    pbuf_hdr->DataLen         -= (NET_BUF_SIZE) pbuf_hdr->TCP_UDP_HdrLen;
    pbuf_hdr->DataIx           = (CPU_INT16U  )(pbuf_hdr->TCP_UDP_HdrDataIx + pbuf_hdr->TCP_UDP_HdrLen);
    pbuf_hdr->ProtocolHdrType  =  NET_PROTOCOL_TYPE_APP;

    pbuf_next = pbuf_hdr->NextBufPtr;
    while (pbuf_next != (NET_BUF *)0) {                         /* Calc ALL pkt bufs' data len/ix    (see Note #5b).    */
        pbuf_next_hdr                  = &pbuf_next->Hdr;
        pbuf_next_hdr->DataIx          =  pbuf_next_hdr->TCP_UDP_HdrDataIx;
        pbuf_next_hdr->TCP_UDP_HdrLen  =  0;                    /* NULL UDP hdr  len in each pkt buf.                   */
        pbuf_next_hdr->TCP_UDP_TotLen  =  udp_tot_len;          /* Dup  UDP tot  len & ...                              */
        pbuf_next_hdr->TCP_UDP_DataLen =  udp_data_len;         /* ...      data len in each pkt buf (see Note #5c).    */
        pbuf_hdr->ProtocolHdrType      =  NET_PROTOCOL_TYPE_APP;
        pbuf_next                      =  pbuf_next_hdr->NextBufPtr;
    }


   *perr = NET_UDP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetUDP_RxPktDemuxDatagram()
*
* Description : Demultiplex UDP datagram to appropriate socket or application connection.
*
* Argument(s) : pbuf        Pointer to network buffer that received UDP datagram.
*               ----        Argument checked in NetUDP_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_ERR_RX                      Receive error; packet discarded.
*                               NET_ERR_RX_DEST                 Invalid destination; no connection available
*                                                                   for received packet.
*
*                                                               -------- RETURNED BY NetSock_Rx() : --------
*                               NET_SOCK_ERR_NONE               UDP datagram successfully received to
*                                                                   socket      connection.
*
*                                                               - RETURNED BY NetUDP_RxPktDemuxAppData() : -
*                               NET_APP_ERR_NONE                UDP datagram successfully received to
*                                                                   application connection.
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_Rx().
*
* Note(s)     : (1) (a) Attempt demultiplex of received UDP datagram to socket connections first, if enabled.
*
*                   (b) On any error, attempt demultiplex to application connections, if enabled.
*
*               (2) When network buffer is demultiplexed to socket or application receive, the buffer's reference 
*                   counter is NOT incremented since the UDP layer does NOT maintain a reference to the buffer.
*********************************************************************************************************
*/

static  void  NetUDP_RxPktDemuxDatagram (NET_BUF  *pbuf,
                                         NET_ERR  *perr)
{
#if ((NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_APP     ) || \
     (NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_SOCK_APP))
    CPU_BOOLEAN  rx_demux_app;
#endif


#if ((NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_SOCK    ) || \
     (NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_SOCK_APP))     /* If socks en'd, ...                               */
    NetSock_Rx(pbuf, perr);                                         /* ... attempt sock demux (see Note #1a).           */
#endif

#if   (NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_APP)
    rx_demux_app = DEF_YES;
#elif (NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_SOCK_APP)
    switch (*perr) {
        case NET_SOCK_ERR_NONE:
             rx_demux_app = DEF_NO;
             break;


        case NET_ERR_RX_DEST:
             rx_demux_app = DEF_YES;
             break;


        case NET_ERR_INIT_INCOMPLETE:
        case NET_ERR_RX:
        default:
             return;                                                /* Prevent 'break NOT reachable' compiler warning.  */
    }
#endif

#if ((NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_APP     ) || \
     (NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_SOCK_APP))     /* If socks dis'd OR sock demux fails, ...          */
    if (rx_demux_app == DEF_YES) {
        NetUDP_RxPktDemuxAppData(pbuf, perr);                       /* ... attempt app  demux (see Note #1b).           */
    }
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetUDP_RxPktDemuxAppData()
*
* Description : Prepare UDP packet for application-specific UDP connection handler function.
*
* Argument(s) : pbuf        Pointer to network buffer that received UDP datagram.
*               ----        Argument checked in NetUDP_Rx().
*
*               perr        Pointer to variable that will receive the return error code from this function
*                               (see Note #1b) :
*
*                                                               -- RETURNED BY NetUDP_RxAppDataHandler() : ---
*                               NET_APP_ERR_NONE                UDP datagram successfully received to
*                                                                   application connection.
*                               NET_ERR_RX                      Receive error; packet discarded.
*                               NET_ERR_RX_DEST                 Invalid destination; no application connection
*                                                                   available for received packet.
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_RxPktDemuxDatagram().
*
* Note(s)     : (1) Application-specific UDP connection handler function is fully responsible for properly 
*                   receiving, demultiplexing, & handling all UDP receive packets to the appropriate 
*                   application connections.
*
*                   (a) (1) (A) (1) If the application-specific UDP connection handler function successfully
*                                   demultiplexes UDP receive packets to an application connection, the handler 
*                                   function MUST at some point call NetUDP_RxAppData() to deframe the application
*                                   data from the packet buffer(s) into an application array as well as copy any
*                                   received IP options into an application IP options buffer.
*
*                                   (a) The application-specific connection handler function may service the
*                                       application data immediately within the handler function, in which case
*                                       the application data SHOULD be serviced as quickly as possible since the
*                                       network's global lock remains acquired for the full duration of the
*                                       network receive.  Thus, no other network receives or transmits can occur
*                                       while the application-specific handler function executes.
*
*                                   (b) The application-specific connection handler function may delay servicing
*                                       the application data by some other application-specific data servicing
*                                       function(s), in which case the servicing function(s) MUST :
*
*                                       (1) Acquire the network's global lock PRIOR to calling NetUDP_RxAppData()
*                                       (2) Release the network's global lock AFTER    calling NetUDP_RxAppData()
*
*                                       See 'NetUDP_RxAppData()  Note #2'.
*
*                               (2) (a) If application-specific handler function calls NetUDP_RxAppData() for a
*                                       specific UDP receive packet, the handler function MUST NOT free the UDP 
*                                       packet's network buffer(s), since NetUDP_RxAppData() frees the network
*                                       buffer(s) [see 'NetUDP_RxAppData()  Note #1f'].
*
*                                   (b) If application-specific handler function does NOT call NetUDP_RxAppData() 
*                                       for a specific UDP receive packet, the handler function SHOULD call 
*                                       NetUDP_RxPktDiscard() to free the packet network buffer(s).
*
*                               (3) FAILURE to call NetUDP_RxAppData() or NetUDP_RxPktDiscard() MAY result in 
*                                   lost network buffer(s).
*
*                           (B) Application-specific handler function MUST return 'NET_APP_ERR_NONE' error code
*                               following the successful demultiplexing of UDP receive packet(s) to application 
*                               connection(s), regardless of whether the handler function serviced the application
*                               data.
*
*                       (2) If the application-specific UDP connection handler function fails to demultiplex or
*                           handle UDP receive packets, the handler function MUST return one of the following 
*                           error codes :
*
*                           (A) 'NET_ERR_RX_DEST' for any received UDP packets NOT destined to an available
*                                   application connection.
*
*                           (B) 'NET_ERR_RX'      for any other receive error.
*
*                   (b) Application-specific UDP connection handler function MUST return one of the following
*                       error codes ONLY [see 'Argument(s) : perr'] :
*
*                           NET_APP_ERR_NONE                                        See Note #1a1B
*                           NET_ERR_RX                                              See Note #1a2B
*                           NET_ERR_RX_DEST                                         See Note #1a2A
*********************************************************************************************************
*/
/*$PAGE*/
#if ((NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_APP     ) || \
     (NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_SOCK_APP))
static  void  NetUDP_RxPktDemuxAppData (NET_BUF  *pbuf,
                                        NET_ERR  *perr)
{
    NET_BUF_HDR       *pbuf_hdr;
    NET_IP_ADDR        src_addr;
    NET_IP_ADDR        dest_addr;
    NET_UDP_PORT_NBR   src_port;
    NET_UDP_PORT_NBR   dest_port;


    pbuf_hdr  = &pbuf->Hdr;
    src_addr  = (NET_IP_ADDR     )pbuf_hdr->IP_AddrSrc;
    src_port  = (NET_UDP_PORT_NBR)pbuf_hdr->TCP_UDP_PortSrc;
    dest_addr = (NET_IP_ADDR     )pbuf_hdr->IP_AddrDest;
    dest_port = (NET_UDP_PORT_NBR)pbuf_hdr->TCP_UDP_PortDest;

    NetUDP_RxAppDataHandler(pbuf, src_addr, src_port, dest_addr, dest_port, perr);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetUDP_RxPktFree()
*
* Description : Free network buffer(s).
*
* Argument(s) : pbuf        Pointer to network buffer.
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_RxAppData().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetUDP_RxPktFree (NET_BUF  *pbuf)
{
    NetBuf_FreeBufList((NET_BUF *)pbuf,
                       (NET_CTR *)0);
}


/*
*********************************************************************************************************
*                                        NetUDP_RxPktDiscard()
*
* Description : On any UDP receive errors, discard UDP packet(s) & buffer(s).
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_RX                      Receive error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_Rx(),
*               NetUDP_RxAppData().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetUDP_RxPktDiscard (NET_BUF  *pbuf,
                                   NET_ERR  *perr)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetUDP_ErrRxPktDiscardedCtr;
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
*                                             NetUDP_Tx()
*
* Description : (1) Prepare & transmit packets from application layer(s) to IP layer :
*
*                   (a) Validate transmit packet
*                   (b) Prepare  UDP datagram header
*                   (c) Transmit UDP packet
*                   (d) Update   transmit statistics
*
*
* Argument(s) : pbuf        Pointer to network buffer to transmit UDP packet.
*               ----        Argument validated in NetUDP_TxAppDataHandler().
*
*               src_addr    Source      IP  address.
*
*               src_port    Source      UDP port.
*
*               dest_addr   Destination IP  address.
*
*               dest_port   Destination UDP port.
*
*               TOS         Specific TOS to transmit UDP/IP packet
*                               (see 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
*
*               TTL         Specific TTL to transmit UDP/IP packet
*                               (see 'net_ip.h  IP HEADER TIME-TO-LIVE (TTL) DEFINES') :
*
*                               NET_IP_HDR_TTL_MIN        1     minimum TTL transmit value
*                               NET_IP_HDR_TTL_MAX      255     maximum TTL transmit value
*                               NET_IP_HDR_TTL_DFLT             default TTL transmit value
*                               NET_IP_HDR_TTL_NONE       0     replace with default TTL
*
*               flags_udp   Flags to select UDP transmit options (see Note #2); bit-field flags logically OR'd :
*
*                               NET_UDP_FLAG_NONE               No UDP  transmit flags selected.
*                               NET_UDP_FLAG_TX_CHK_SUM_DIS     DISABLE transmit check-sums.
*                               NET_UDP_FLAG_TX_BLOCK           Transmit UDP application data with blocking,
*                                                                   if flag set; without blocking, if clear
*                                                                   (see Note #2a).
*
*               flags_ip    Flags to select IP  transmit options; bit-field flags logically OR'd :
*
*                               NET_IP_FLAG_NONE                No  IP transmit flags selected.
*                               NET_IP_FLAG_TX_DONT_FRAG        Set IP 'Don't Frag' flag.
*
*               popts_ip    Pointer to one or more IP options configuration data structures
*                               (see 'net_ip.h  IP HEADER OPTION CONFIGURATION DATA TYPES') :
*
*                               NULL                            NO IP transmit options configuration.
*                               NET_IP_OPT_CFG_ROUTE_TS         Route &/or Internet Timestamp options configuration.
*                               NET_IP_OPT_CFG_SECURITY         Security options configuration
*                                                                   (see 'net_ip.c  Note #1f').
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UDP_ERR_NONE                UDP datagram(s) successfully prepared & 
*                                                                   transmitted to IP layer.
*
*                                                               -- RETURNED BY NetUDP_TxPktValidate() : --
*                               NET_ERR_INVALID_PROTOCOL        Invalid/unknown protocol type.
*                               NET_BUF_ERR_INVALID_IX          Invalid or insufficient buffer index.
*                               NET_UDP_ERR_INVALID_LEN_DATA    Invalid protocol/data length.
*                               NET_UDP_ERR_INVALID_PORT_NBR    Invalid UDP port number.
*                               NET_UDP_ERR_INVALID_FLAG        Invalid UDP flag(s).
*
*                                                               - RETURNED BY NetUDP_TxPktPrepareHdr() : -
*                               NET_UTIL_ERR_NULL_PTR           Check-sum passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Check-sum passed a zero size.
*                               NET_UTIL_ERR_INVALID_PROTOCOL   Invalid data packet protocol.
*
*                                                               -------- RETURNED BY NetIP_Tx() : --------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_ERR_TX                      Transmit error; packet(s) discarded.
* Return(s)   : none.
*
* Caller(s)   : NetUDP_TxAppDataHandler().
*
* Note(s)     : (2) #### Some UDP transmit flag options NOT yet implemented :
*
*                   (a) NET_UDP_FLAG_TX_BLOCK
*
*                   See also 'NetUDP_TxPktValidate()  Note #2b'.
*
*               (3) Network buffer already freed by lower layer; only increment error counter.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetUDP_Tx (NET_BUF           *pbuf,
                         NET_IP_ADDR        src_addr,
                         NET_UDP_PORT_NBR   src_port,
                         NET_IP_ADDR        dest_addr,
                         NET_UDP_PORT_NBR   dest_port,
                         NET_IP_TOS         TOS,
                         NET_IP_TTL         TTL,
                         CPU_INT16U         flags_udp,
                         CPU_INT16U         flags_ip,
                         void              *popts_ip,
                         NET_ERR           *perr)
{
    NET_BUF_HDR  *pbuf_hdr;


                                                                /* --------------- VALIDATE TX UDP PKT ---------------- */
    pbuf_hdr = &pbuf->Hdr;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NetUDP_TxPktValidate(pbuf_hdr,
                         src_port,
                         dest_port,
                         flags_udp,
                         perr);
    switch (*perr) {
        case NET_UDP_ERR_NONE:
             break;


        case NET_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        case NET_UDP_ERR_INVALID_LEN_DATA:
        case NET_UDP_ERR_INVALID_PORT_NBR:
        case NET_UDP_ERR_INVALID_FLAG:
        default:
             NetUDP_TxPktDiscard(pbuf);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif


/*$PAGE*/
                                                                /* ------------------ PREPARE UDP HDR ----------------- */
    NetUDP_TxPktPrepareHdr(pbuf,
                           pbuf_hdr,
                           src_addr,
                           src_port,
                           dest_addr,
                           dest_port,
                           flags_udp,
                           perr);


                                                                /* -------------------- TX UDP PKT -------------------- */
    switch (*perr) {
        case NET_UDP_ERR_NONE:
             NetIP_Tx(pbuf,
                      src_addr,
                      dest_addr,
                      TOS,
                      TTL,
                      flags_ip,
                      popts_ip,
                      perr);
             break;


        case NET_UTIL_ERR_NULL_PTR:
        case NET_UTIL_ERR_NULL_SIZE:
        case NET_UTIL_ERR_INVALID_PROTOCOL:
        case NET_BUF_ERR_INVALID_IX:
        default:
             NetUDP_TxPktDiscard(pbuf);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }


                                                                /* ----------------- UPDATE TX STATS ------------------ */
    switch (*perr) {                                            /* Chk err from NetIP_Tx().                             */
        case NET_IP_ERR_NONE:
             NET_CTR_STAT_INC(Net_StatCtrs.NetUDP_StatTxDatagramCtr);
            *perr = NET_UDP_ERR_NONE;
             break;


        case NET_ERR_TX:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrTxPktDiscardedCtr);  /* See Note #3.                                 */
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */


        case NET_ERR_INIT_INCOMPLETE:                           
        default:
             NetUDP_TxPktDiscard(pbuf);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetUDP_TxPktValidate()
*
* Description : (1) Validate UDP transmit packet parameters & options :
*
*                   (a) Validate the following transmit packet parameters :
*
*                       (1) Supported protocols :
*                           (A) Application
*                           (B) BSD Sockets
*
*                       (2) Buffer protocol index
*                       (3) Data Length
*                       (4) Source      Port
*                       (5) Destination Port
*                       (6) Flags
*
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetUDP_Tx().
*
*               src_port    Source      UDP port.
*
*               dest_port   Destination UDP port.
*
*               flags_udp   Flags to select UDP transmit options (see Note #2); bit-field flags logically OR'd :
*
*                               NET_UDP_FLAG_NONE               No UDP  transmit flags selected.
*                               NET_UDP_FLAG_TX_CHK_SUM_DIS     DISABLE transmit check-sums.
*                               NET_UDP_FLAG_TX_BLOCK           Transmit UDP application data with blocking,
*                                                                   if flag set; without blocking, if clear
*                                                                   (see Note #2b1).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UDP_ERR_NONE                Transmit packet validated.
*                               NET_ERR_INVALID_PROTOCOL        Invalid/unknown protocol type.
*                               NET_BUF_ERR_INVALID_IX          Invalid or insufficient buffer index.
*                               NET_UDP_ERR_INVALID_LEN_DATA    Invalid protocol/data length.
*                               NET_UDP_ERR_INVALID_PORT_NBR    Invalid UDP port number.
*                               NET_UDP_ERR_INVALID_FLAG        Invalid UDP flag(s).
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_Tx().
*
* Note(s)     : (2) (a) #### Only some UDP transmit flag options are implemented.  If other flag options 
*                       are requested, NetUDP_Tx() handler function(s) abort & return appropriate error 
*                       codes so that requested flag options are NOT silently ignored.
*
*                   (b) #### Some UDP transmit flag options NOT yet implemented :
*
*                       (1) NET_UDP_FLAG_TX_BLOCK
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetUDP_TxPktValidate (NET_BUF_HDR       *pbuf_hdr,
                                    NET_UDP_PORT_NBR   src_port,
                                    NET_UDP_PORT_NBR   dest_port,
                                    CPU_INT16U         flags_udp,
                                    NET_ERR           *perr)
{
    CPU_INT16U  ix;
    CPU_INT16U  len;
    CPU_INT16U  flag_mask;


                                                                /* ----------------- VALIDATE PROTOCOL ---------------- */
    switch (pbuf_hdr->ProtocolHdrType) {
        case NET_PROTOCOL_TYPE_APP:
        case NET_PROTOCOL_TYPE_SOCK:
        case NET_PROTOCOL_TYPE_UDP:
             ix  = (CPU_INT16U)pbuf_hdr->DataIx;
             len = (CPU_INT16U)pbuf_hdr->DataLen;
             break;


        case NET_PROTOCOL_TYPE_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrTxInvalidProtocolCtr);
            *perr = NET_ERR_INVALID_PROTOCOL;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

    if (ix == NET_BUF_IX_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrTxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

    if (ix < NET_UDP_HDR_SIZE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrTxInvalidBufIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }



                                                                /* -------------- VALIDATE TOT DATA LEN --------------- */
    if (len != pbuf_hdr->TotLen) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrTxHdrDataLenCtr);
       *perr = NET_UDP_ERR_INVALID_LEN_DATA;
        return;
    }



                                                                /* ---------------- VALIDATE UDP PORTS ---------------- */
    if (src_port  == NET_UDP_PORT_NBR_RESERVED) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrTxHdrPortSrcCtr);
       *perr = NET_UDP_ERR_INVALID_PORT_NBR;
        return;
    }

    if (dest_port == NET_UDP_PORT_NBR_RESERVED) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrTxHdrPortDestCtr);
       *perr = NET_UDP_ERR_INVALID_PORT_NBR;
        return;
    }



                                                                /* ---------------- VALIDATE UDP FLAGS ---------------- */
    flag_mask = NET_UDP_FLAG_NONE           |
                NET_UDP_FLAG_TX_CHK_SUM_DIS |
                NET_UDP_FLAG_TX_BLOCK;                          /* See Note #2b1.                                       */

    if ((flags_udp & ~flag_mask) != NET_UDP_FLAG_NONE) {        /* If any invalid flags req'd, rtn err (see Note #2a).  */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetUDP_ErrTxHdrFlagsCtr);
       *perr = NET_UDP_ERR_INVALID_FLAG;
        return;
    }


   *perr = NET_UDP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetUDP_TxPktPrepareHdr()
*
* Description : (1) Prepare UDP header :
*
*                   (a) Update network buffer's protocol index & length controls
*
*                   (b) Prepare the transmit packet's following UDP header fields :
*
*                       (1) Source      Port
*                       (2) Destination Port
*                       (3) Datagram Length
*                       (4) Check-Sum                                   See Note #3
*
*                   (c) Convert the following UDP header fields from host-order to network-order :
*
*                       (1) Source      Port
*                       (2) Destination Port
*                       (3) Datagram Length
*                       (4) Check-Sum                                   See Note #3g
*
*
* Argument(s) : pbuf        Pointer to network buffer to transmit UDP packet.
*               ----        Argument checked   in NetUDP_Tx().
*
*               pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetUDP_Tx().
*
*               src_addr    Source      IP  address.
*
*               src_port    Source      UDP port.
*               --------    Argument validated in NetUDP_TxPktValidate().
*
*               dest_addr   Destination IP  address.
*
*               dest_port   Destination UDP port.
*               ---------   Argument validated in NetUDP_TxPktValidate().
*
*               flags_udp   Flags to select UDP transmit options (see Note #2); bit-field flags logically OR'd :
*               ---------   
*                               NET_UDP_FLAG_NONE               No UDP  transmit flags selected.
*                               NET_UDP_FLAG_TX_CHK_SUM_DIS     DISABLE transmit check-sums.
*                               NET_UDP_FLAG_TX_BLOCK           Transmit UDP application data with blocking,
*                                                                   if flag set; without blocking, if clear
*                                                                   (see Note #2a).
*
*                           Argument checked    in NetUDP_TxPktValidate().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UDP_ERR_NONE                UDP header successfully prepared.
*
*                                                               - RETURNED BY NetUtil_16BitOnesCplChkSumDataCalc() : -
*                               NET_UTIL_ERR_NULL_PTR           Check-sum passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Check-sum passed a zero size.
*                               NET_UTIL_ERR_INVALID_PROTOCOL   Invalid data packet protocol.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_Tx().
*$PAGE*
* Note(s)     : (2) #### Some UDP transmit flag options NOT yet implemented :
*
*                   (a) NET_UDP_FLAG_TX_BLOCK
*
*                   See also 'NetUDP_TxPktValidate()  Note #2b'.
*
*               (3) (a) UDP header Check-Sum MUST be calculated AFTER the entire UDP header has been prepared.
*                       In addition, ALL multi-octet words are converted from host-order to network-order 
*                       since "the sum of 16-bit integers can be computed in either byte order" [RFC #1071, 
*                       Section 2.(B)].
*
*                   (b) RFC #1122, Section 4.1.3.4 states that "an application MAY optionally be able to 
*                       control whether a UDP checksum will be generated".
*
*                   (c) Although neither RFC #768 nor RFC #1122, Sections 4.1 expressly specifies, it is
*                       assumed that that the UDP header Check-Sum field MUST be cleared to '0' BEFORE the
*                       UDP header Check-Sum is calculated.
*
*                       See also 'net_ip.c    NetIP_TxPktPrepareHdr()   Note #6b',
*                                'net_icmp.c  NetICMP_TxMsgErr()        Note #7b',
*                                'net_icmp.c  NetICMP_TxMsgReq()        Note #7b',
*                                'net_icmp.c  NetICMP_TxMsgReply()      Note #5b',
*                                'net_tcp.c   NetTCP_TxPktPrepareHdr()  Note #3b'.
*
*                   (d) (1) In addition to the UDP datagram header & data, the UDP Check-Sum calculation
*                           includes "a pseudo header of information from the IP header ... conceptually
*                           prefixed to the UDP header [which] contains the source address, the destination
*                           address, the protocol, and the UDP length" (see RFC #768, Section 'Fields :
*                           Checksum').
*
*                       (2) Since network check-sum functions REQUIRE that 16-bit one's-complement check-
*                           sum calculations be performed on headers & data arranged in network-order (see 
*                           'net_util.c  NetUtil_16BitOnesCplChkSumDataCalc()  Note #3'), UDP pseudo-header 
*                           values MUST be set or converted to network-order.
*
*                   (e) RFC #768, Section 'Fields : Checksum' specifies that "the data [is] padded with zero
*                       octets at the end (if necessary) to make a multiple of two octets".
*
*                       See also 'net_util.c  NetUtil_16BitSumDataCalc()  Note #8'.
*
*                   (f) "If the computed checksum is zero" (i.e. one's-complement positive zero -- all 
*                        bits equal to zero), then "it is transmitted as all ones (the equivalent in 
*                        one's complement arithmetic" (i.e. one's-complement negative zero -- all bits 
*                        equal to one) [RFC #768, Section 'Fields : Checksum'].
*
*                   (g) The UDP header Check-Sum field is returned in network-order & MUST NOT be re-
*                       converted back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumDataCalc()
*                       Note #4').
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetUDP_TxPktPrepareHdr (NET_BUF           *pbuf,
                                      NET_BUF_HDR       *pbuf_hdr,
                                      NET_IP_ADDR        src_addr,
                                      NET_UDP_PORT_NBR   src_port,
                                      NET_IP_ADDR        dest_addr,
                                      NET_UDP_PORT_NBR   dest_port,
                                      CPU_INT16U         flags_udp,
                                      NET_ERR           *perr)
{
    NET_UDP_HDR         *pudp_hdr;
    NET_UDP_PSEUDO_HDR   udp_pseudo_hdr;
    NET_CHK_SUM          udp_chk_sum;
    CPU_BOOLEAN          tx_chk_sum;


                                                                /* ----------------- UPDATE BUF CTRLS ----------------- */
    pbuf_hdr->TCP_UDP_HdrLen     =  NET_UDP_HDR_SIZE;
    pbuf_hdr->TCP_UDP_HdrDataIx  =  pbuf_hdr->DataIx - pbuf_hdr->TCP_UDP_HdrLen;

    pbuf_hdr->TotLen            += (NET_BUF_SIZE)pbuf_hdr->TCP_UDP_HdrLen;
    pbuf_hdr->TCP_UDP_TotLen     = (CPU_INT16U  )pbuf_hdr->TotLen;
    pbuf_hdr->TCP_UDP_DataLen    = (CPU_INT16U  )pbuf_hdr->DataLen;

    pbuf_hdr->ProtocolHdrType    =  NET_PROTOCOL_TYPE_UDP;



                                                                /* ----------------- PREPARE UDP HDR ------------------ */
    pudp_hdr = (NET_UDP_HDR *)&pbuf->Data[pbuf_hdr->TCP_UDP_HdrDataIx];



                                                                /* ---------------- PREPARE UDP PORTS ----------------- */
    NET_UTIL_VAL_COPY_SET_NET_16(&pudp_hdr->PortSrc,  &src_port);
    NET_UTIL_VAL_COPY_SET_NET_16(&pudp_hdr->PortDest, &dest_port);



                                                                /* ------------- PREPARE UDP DATAGRAM LEN ------------- */
    NET_UTIL_VAL_COPY_SET_NET_16(&pudp_hdr->DatagramLen, &pbuf_hdr->TCP_UDP_TotLen);



                                                                /* --------------- PREPARE UDP CHK SUM ---------------- */
#if (NET_UDP_CFG_TX_CHK_SUM_EN == DEF_ENABLED)
    tx_chk_sum = DEF_BIT_IS_CLR(flags_udp, NET_UDP_FLAG_TX_CHK_SUM_DIS);
#else
    tx_chk_sum = DEF_NO;
#endif

    if (tx_chk_sum == DEF_YES) {                                /* If en'd (see Note #3b), prepare UDP tx chk sum.      */
        NET_UTIL_VAL_SET_NET_16(&pudp_hdr->ChkSum, 0x0000);     /* Clr UDP chk sum            (see Note #3c).           */
                                                                /* Cfg UDP chk sum pseudo-hdr (see Note #3d).           */
        udp_pseudo_hdr.AddrSrc     = (NET_IP_ADDR)NET_UTIL_HOST_TO_NET_32(src_addr);
        udp_pseudo_hdr.AddrDest    = (NET_IP_ADDR)NET_UTIL_HOST_TO_NET_32(dest_addr);
        udp_pseudo_hdr.Zero        = (CPU_INT08U )0x00;
        udp_pseudo_hdr.Protocol    = (CPU_INT08U )NET_IP_HDR_PROTOCOL_UDP;
        udp_pseudo_hdr.DatagramLen = (CPU_INT16U )NET_UTIL_HOST_TO_NET_16(pbuf_hdr->TCP_UDP_TotLen);
                                                                /* Calc UDP chk sum.                                    */
        udp_chk_sum                =  NetUtil_16BitOnesCplChkSumDataCalc((void     *) pbuf,
                                                                         (void     *)&udp_pseudo_hdr,
                                                                         (CPU_INT16U) NET_UDP_PSEUDO_HDR_SIZE,
                                                                         (NET_ERR  *) perr);
        if (*perr != NET_UTIL_ERR_NONE) {
             return;
        }

        if (udp_chk_sum == NET_UDP_HDR_CHK_SUM_POS_ZERO) {      /* If equal to one's-cpl pos zero, ...                  */
            udp_chk_sum  = NET_UDP_HDR_CHK_SUM_NEG_ZERO;        /* ...  set to one's-cpl neg zero (see Note #3f).       */
        }

    } else {                                                    /* Else tx NO chk sum.                                  */
        udp_chk_sum = NET_UTIL_HOST_TO_NET_16(NET_UDP_HDR_CHK_SUM_NONE);
    }

    NET_UTIL_VAL_COPY_16(&pudp_hdr->ChkSum, &udp_chk_sum);      /* Copy UDP chk sum in net order  (see Note #3g).       */



   *perr = NET_UDP_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetUDP_TxPktFree()
*
* Description : Free network buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_Tx(),
*               NetUDP_TxAppDataHandler().
*
* Note(s)     : (1) (a) Although UDP Transmit initially requests the network buffer for transmit, 
*                       the UDP layer does NOT maintain a reference to the buffer.
*
*                   (b) Also, since the network interface layer frees ALL unreferenced buffers 
*                       after successful transmission [see 'net_if_pkt.c  NetIF_Pkt_TxPktFree()'
*                       or 'net_if_char.c  NetIF_Char_TxPktFree()'], the UDP layer MUST not
*                       free the transmit buffer.
*********************************************************************************************************
*/

static  void  NetUDP_TxPktFree (NET_BUF  *pbuf)
{
   (void)&pbuf;                                                 /* Prevent compiler warning (see Note #1).              */
}


/*
*********************************************************************************************************
*                                        NetUDP_TxPktDiscard()
*
* Description : On any UDP transmit packet errors, discard packet & buffer.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
* Return(s)   : none.
*
* Caller(s)   : NetUDP_Tx(),
*               NetUDP_TxAppDataHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetUDP_TxPktDiscard (NET_BUF  *pbuf)
{
    NET_CTR  *pctr;


#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
    pctr = (NET_CTR *)&Net_ErrCtrs.NetUDP_ErrTxPktDiscardedCtr;
#else
    pctr = (NET_CTR *) 0;
#endif
    NetBuf_FreeBuf((NET_BUF *)pbuf,
                   (NET_CTR *)pctr);
}

