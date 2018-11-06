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
* Filename      : net_icmp.h
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
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_ICMP_MODULE
#define  NET_ICMP_EXT
#else
#define  NET_ICMP_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                 ICMP TRANSMIT SOURCE QUENCH DEFINES
*********************************************************************************************************
*/

#define  NET_ICMP_TX_SRC_QUENCH_TH_MIN                     1
#define  NET_ICMP_TX_SRC_QUENCH_TH_MAX                   100
#define  NET_ICMP_TX_SRC_QUENCH_TH_DFLT                    5


/*
*********************************************************************************************************
*                                          ICMP FLAG DEFINES
*********************************************************************************************************
*/

                                                                /* ------------------ NET ICMP FLAGS ------------------ */
#define  NET_ICMP_FLAG_NONE                       DEF_BIT_NONE
#define  NET_ICMP_FLAG_USED                       DEF_BIT_00    /* ICMP entry cur used; i.e. NOT in free pool.          */


/*
*********************************************************************************************************
*                                          ICMP TYPE DEFINES
*
* Note(s) : (1) NET_ICMP_TYPE_??? #define values specifically chosen as ASCII representations of the ICMP
*               types.  Memory displays of ICMP types will display with their chosen ASCII names.
*********************************************************************************************************
*/

                                                                /* ------------------ NET ICMP TYPES ------------------ */
#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  NET_ICMP_TYPE_NONE                       0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_ICMP_TYPE_TX_SRC_QUENCH              0x54585351    /* "TXSQ" in ASCII.                                     */

#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)
#define  NET_ICMP_TYPE_NONE                       0x454E4F4E    /* "NONE" in ASCII.                                     */
#define  NET_ICMP_TYPE_TX_SRC_QUENCH              0x51535854    /* "TXSQ" in ASCII.                                     */

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  NET_ICMP_TYPE_NONE                       0x4F4E454E    /* "NONE" in ASCII.                                     */
#define  NET_ICMP_TYPE_TX_SRC_QUENCH              0x58545153    /* "TXSQ" in ASCII.                                     */

#else                                                           /* Dflt CPU_WORD_SIZE_08.                               */
#define  NET_ICMP_TYPE_NONE                       0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_ICMP_TYPE_TX_SRC_QUENCH              0x54585351    /* "TXSQ" in ASCII.                                     */

#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     ICMP MESSAGE TYPES & CODES
*
* Note(s) : (1) 'DEST_UNREACH' abbreviated to 'DEST' for ICMP 'Destination Unreachable' message 
*                error codes to enforce ANSI-compliance of 31-character symbol length uniqueness.
*
*           (2) ICMP 'Redirect' & 'Router' messages are NOT supported (see 'net_icmp.h  Note #1').
*
*           (3) ICMP 'Address Mask Request' messages received by this host are NOT supported (see
*               'net_icmp.h  Note #3').
*********************************************************************************************************
*/

                                                                /* ------------------ ICMP MSG TYPES ------------------ */
#define  NET_ICMP_MSG_TYPE_NONE                          DEF_INT_08U_MAX_VAL

#define  NET_ICMP_MSG_TYPE_ECHO_REPLY                      0
#define  NET_ICMP_MSG_TYPE_DEST_UNREACH                    3
#define  NET_ICMP_MSG_TYPE_SRC_QUENCH                      4
#define  NET_ICMP_MSG_TYPE_REDIRECT                        5    /* See Note #2.                                         */
#define  NET_ICMP_MSG_TYPE_ECHO_REQ                        8
#define  NET_ICMP_MSG_TYPE_ROUTE_AD                        9    /* See Note #2.                                         */
#define  NET_ICMP_MSG_TYPE_ROUTE_REQ                      10    /* See Note #2.                                         */
#define  NET_ICMP_MSG_TYPE_TIME_EXCEED                    11
#define  NET_ICMP_MSG_TYPE_PARAM_PROB                     12
#define  NET_ICMP_MSG_TYPE_TS_REQ                         13
#define  NET_ICMP_MSG_TYPE_TS_REPLY                       14
#define  NET_ICMP_MSG_TYPE_ADDR_MASK_REQ                  17
#define  NET_ICMP_MSG_TYPE_ADDR_MASK_REPLY                18


                                                                /* ------------------ ICMP MSG CODES ------------------ */
#define  NET_ICMP_MSG_CODE_NONE                          DEF_INT_08U_MAX_VAL

#define  NET_ICMP_MSG_CODE_ECHO                            0
#define  NET_ICMP_MSG_CODE_ECHO_REQ                        0
#define  NET_ICMP_MSG_CODE_ECHO_REPLY                      0

#define  NET_ICMP_MSG_CODE_DEST_NET                        0    /* See Note #1.                                         */
#define  NET_ICMP_MSG_CODE_DEST_HOST                       1
#define  NET_ICMP_MSG_CODE_DEST_PROTOCOL                   2
#define  NET_ICMP_MSG_CODE_DEST_PORT                       3
#define  NET_ICMP_MSG_CODE_DEST_FRAG_NEEDED                4
#define  NET_ICMP_MSG_CODE_DEST_ROUTE_FAIL                 5
#define  NET_ICMP_MSG_CODE_DEST_NET_UNKNOWN                6
#define  NET_ICMP_MSG_CODE_DEST_HOST_UNKNOWN               7
#define  NET_ICMP_MSG_CODE_DEST_HOST_ISOLATED              8
#define  NET_ICMP_MSG_CODE_DEST_NET_TOS                   11
#define  NET_ICMP_MSG_CODE_DEST_HOST_TOS                  12

#define  NET_ICMP_MSG_CODE_SRC_QUENCH                      0

#define  NET_ICMP_MSG_CODE_TIME_EXCEED_TTL                 0
#define  NET_ICMP_MSG_CODE_TIME_EXCEED_FRAG_REASM          1

#define  NET_ICMP_MSG_CODE_PARAM_PROB_IP_HDR               0
#define  NET_ICMP_MSG_CODE_PARAM_PROB_OPT_MISSING          1

#define  NET_ICMP_MSG_CODE_TS                              0
#define  NET_ICMP_MSG_CODE_TS_REQ                          0
#define  NET_ICMP_MSG_CODE_TS_REPLY                        0

#define  NET_ICMP_MSG_CODE_ADDR_MASK                       0
#define  NET_ICMP_MSG_CODE_ADDR_MASK_REQ                   0
#define  NET_ICMP_MSG_CODE_ADDR_MASK_REPLY                 0


/*$PAGE*/
/*
*********************************************************************************************************
*                                        ICMP MESSAGE DEFINES
*********************************************************************************************************
*/

#define  NET_ICMP_HDR_SIZE_DFLT                            8

#define  NET_ICMP_HDR_SIZE_DEST_UNREACH                  NET_ICMP_HDR_SIZE_DFLT
#define  NET_ICMP_HDR_SIZE_SRC_QUENCH                    NET_ICMP_HDR_SIZE_DFLT
#define  NET_ICMP_HDR_SIZE_TIME_EXCEED                   NET_ICMP_HDR_SIZE_DFLT
#define  NET_ICMP_HDR_SIZE_PARAM_PROB                    NET_ICMP_HDR_SIZE_DFLT
#define  NET_ICMP_HDR_SIZE_ECHO                          NET_ICMP_HDR_SIZE_DFLT
#define  NET_ICMP_HDR_SIZE_TS                             20
#define  NET_ICMP_HDR_SIZE_ADDR_MASK                      12


#define  NET_ICMP_HDR_NBR_OCTETS_UNUSED                    4
#define  NET_ICMP_HDR_NBR_OCTETS_UNUSED_PARAM_PROB         3


#define  NET_ICMP_MSG_ERR_HDR_SIZE_MIN                   NET_IP_HDR_SIZE_MIN
#define  NET_ICMP_MSG_ERR_HDR_SIZE_MAX                   NET_IP_HDR_SIZE_MAX

#define  NET_ICMP_MSG_ERR_DATA_SIZE_MIN_BITS              64                    /* See RFC #1122, Section 3.2.2.        */
#define  NET_ICMP_MSG_ERR_DATA_SIZE_MIN_OCTETS        (((NET_ICMP_MSG_ERR_DATA_SIZE_MIN_BITS - 1) / DEF_OCTET_NBR_BITS) + 1)

#define  NET_ICMP_MSG_ERR_LEN_MIN                       (NET_ICMP_MSG_ERR_HDR_SIZE_MIN  + NET_ICMP_MSG_ERR_DATA_SIZE_MIN_OCTETS)
#define  NET_ICMP_MSG_ERR_LEN_MAX                       (NET_ICMP_MSG_ERR_HDR_SIZE_MAX  + NET_ICMP_MSG_ERR_DATA_SIZE_MIN_OCTETS)


#define  NET_ICMP_MSG_LEN_MIN_DFLT                       NET_ICMP_HDR_SIZE_DFLT

#define  NET_ICMP_MSG_LEN_MIN_DEST_UNREACH              (NET_ICMP_HDR_SIZE_DEST_UNREACH + NET_ICMP_MSG_ERR_LEN_MIN)
#define  NET_ICMP_MSG_LEN_MIN_SRC_QUENCH                (NET_ICMP_HDR_SIZE_SRC_QUENCH   + NET_ICMP_MSG_ERR_LEN_MIN)
#define  NET_ICMP_MSG_LEN_MIN_TIME_EXCEED               (NET_ICMP_HDR_SIZE_TIME_EXCEED  + NET_ICMP_MSG_ERR_LEN_MIN)
#define  NET_ICMP_MSG_LEN_MIN_PARAM_PROB                (NET_ICMP_HDR_SIZE_PARAM_PROB   + NET_ICMP_MSG_ERR_LEN_MIN)
#define  NET_ICMP_MSG_LEN_MIN_ECHO                       NET_ICMP_HDR_SIZE_ECHO
#define  NET_ICMP_MSG_LEN_MIN_TS                         NET_ICMP_HDR_SIZE_TS
#define  NET_ICMP_MSG_LEN_MIN_ADDR_MASK                  NET_ICMP_HDR_SIZE_ADDR_MASK


#define  NET_ICMP_MSG_LEN_MAX_NONE                    0xFFFF

#define  NET_ICMP_MSG_LEN_MAX_DEST_UNREACH               NET_ICMP_MSG_LEN_MAX_NONE
#define  NET_ICMP_MSG_LEN_MAX_SRC_QUENCH                 NET_ICMP_MSG_LEN_MAX_NONE
#define  NET_ICMP_MSG_LEN_MAX_TIME_EXCEED                NET_ICMP_MSG_LEN_MAX_NONE
#define  NET_ICMP_MSG_LEN_MAX_PARAM_PROB                 NET_ICMP_MSG_LEN_MAX_NONE
#define  NET_ICMP_MSG_LEN_MAX_ECHO                       NET_ICMP_MSG_LEN_MAX_NONE
#define  NET_ICMP_MSG_LEN_MAX_TS                         NET_ICMP_HDR_SIZE_TS
#define  NET_ICMP_MSG_LEN_MAX_ADDR_MASK                  NET_ICMP_HDR_SIZE_ADDR_MASK


#define  NET_ICMP_MSG_PTR_NONE                           DEF_INT_08U_MAX_VAL

#define  NET_ICMP_MSG_PTR_MIN_PARAM_PROB                 NET_ICMP_MSG_LEN_MIN_DFLT


/*$PAGE*/
/*
*********************************************************************************************************
*                                        ICMP POINTER DEFINES
*
* Note(s) : (1) RFC #791 & RFC #792 define a pointer (PTR) as an index (IX) into an option or message :
*
*               (a) RFC #791, Section 3.1 'Options : Loose/Strict Source & Record Route'
*               (b) RFC #791, Section 3.1 'Options : Internet Timestamp'
*               (c) RFC #792,             'Parameter Problem Message'
*
*           (2) ICMP Parameter Problem Message pointer validation currently ONLY supports the following
*               protocols :
*
*               (a) IP
*********************************************************************************************************
*/

#define  NET_ICMP_PTR_IX_BASE                              0

                                                                                /* ---------- IP HDR PTR IXs ---------- */
#define  NET_ICMP_PTR_IX_IP_HDR_BASE                     NET_ICMP_PTR_IX_BASE

#define  NET_ICMP_PTR_IX_IP_HDR_VER                     (NET_ICMP_PTR_IX_IP_HDR_BASE +  0)
#define  NET_ICMP_PTR_IX_IP_HDR_HDR_LEN                 (NET_ICMP_PTR_IX_IP_HDR_BASE +  0)
#define  NET_ICMP_PTR_IX_IP_HDR_TOS                     (NET_ICMP_PTR_IX_IP_HDR_BASE +  1)
#define  NET_ICMP_PTR_IX_IP_HDR_TOT_LEN                 (NET_ICMP_PTR_IX_IP_HDR_BASE +  2)
#define  NET_ICMP_PTR_IX_IP_HDR_ID                      (NET_ICMP_PTR_IX_IP_HDR_BASE +  4)
#define  NET_ICMP_PTR_IX_IP_HDR_FLAGS                   (NET_ICMP_PTR_IX_IP_HDR_BASE +  6)
#define  NET_ICMP_PTR_IX_IP_HDR_FRAG_OFFSET             (NET_ICMP_PTR_IX_IP_HDR_BASE +  6)
#define  NET_ICMP_PTR_IX_IP_HDR_TTL                     (NET_ICMP_PTR_IX_IP_HDR_BASE +  8)
#define  NET_ICMP_PTR_IX_IP_HDR_PROTOCOL                (NET_ICMP_PTR_IX_IP_HDR_BASE +  9)
#define  NET_ICMP_PTR_IX_IP_HDR_CHK_SUM                 (NET_ICMP_PTR_IX_IP_HDR_BASE + 10)
#define  NET_ICMP_PTR_IX_IP_HDR_ADDR_SRC                (NET_ICMP_PTR_IX_IP_HDR_BASE + 12)
#define  NET_ICMP_PTR_IX_IP_HDR_ADDR_DEST               (NET_ICMP_PTR_IX_IP_HDR_BASE + 16)
#define  NET_ICMP_PTR_IX_IP_HDR_OPTS                    (NET_ICMP_PTR_IX_IP_HDR_BASE + 20)

                                                                                /* --------- ICMP MSG PTR IXs --------- */
#define  NET_ICMP_PTR_IX_ICMP_MSG_BASE                     0

#define  NET_ICMP_PTR_IX_ICMP_MSG_TYPE                     0
#define  NET_ICMP_PTR_IX_ICMP_MSG_CODE                     1
#define  NET_ICMP_PTR_IX_ICMP_MSG_CHK_SUM                  2

#define  NET_ICMP_PTR_IX_ICMP_MSG_PTR                      4
#define  NET_ICMP_PTR_IX_ICMP_MSG_UNUSED                   4
#define  NET_ICMP_PTR_IX_ICMP_MSG_UNUSED_PARAM_PROB        5


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                            ICMP TRANSMIT SOURCE QUENCH QUANTITY DATA TYPE
*
* Note(s) : (1) NET_ICMP_SRC_QUENCH_NBR_MAX  SHOULD be #define'd based on 'NET_ICMP_SRC_QUENCH_QTY' 
*               data type declared.
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_ICMP_SRC_QUENCH_QTY;                   /* Defines max qty of src quench entries to support.    */

#define  NET_ICMP_SRC_QUENCH_NBR_MIN                       1
#define  NET_ICMP_SRC_QUENCH_NBR_MAX     DEF_INT_16U_MAX_VAL    /* See Note #1.                                         */


/*$PAGE*/
/*
*********************************************************************************************************
*                                             ICMP HEADER
*
* Note(s) : (1) See RFC #792 for ICMP message header formats.
*
*           (2) ICMP 'Redirect' & 'Router' messages are NOT supported (see 'net_icmp.h  Note #1').
*********************************************************************************************************
*/

                                                                        /* --------------- NET ICMP HDR --------------- */
typedef  struct  net_icmp_hdr {
    CPU_INT08U      Type;                                               /* ICMP msg type.                               */
    CPU_INT08U      Code;                                               /* ICMP msg code.                               */
    CPU_INT16U      ChkSum;                                             /* ICMP msg chk sum.                            */
} NET_ICMP_HDR;


/*
*********************************************************************************************************
*                                          ICMP ERROR HEADER
*
* Note(s) : (1) See RFC #792, Sections 'Destination Unreachable Message', 'Source Quench Message', 'Time
*               Exceeded Message' for ICMP 'Error Message' header format.
*
*           (2) 'Unused' field MUST be cleared (i.e. ALL 'Unused' field octets MUST be set to 0x00).
*********************************************************************************************************
*/

                                                                        /* ------------- NET ICMP ERR HDR ------------- */
typedef  struct  net_icmp_hdr_err {
    CPU_INT08U      Type;                                               /* ICMP msg type.                               */
    CPU_INT08U      Code;                                               /* ICMP msg code.                               */
    CPU_INT16U      ChkSum;                                             /* ICMP msg chk sum.                            */

    CPU_INT08U      Unused[NET_ICMP_HDR_NBR_OCTETS_UNUSED];             /* See Note #2.                                 */

    CPU_INT08U      Data[NET_ICMP_MSG_ERR_LEN_MAX];
} NET_ICMP_HDR_ERR;


/*
*********************************************************************************************************
*                                    ICMP PARAMETER PROBLEM HEADER
*
* Note(s) : (1) See RFC #792, Section 'Parameter Problem Message' for ICMP 'Parameter Problem Message'
*               header format.
*
*           (2) 'Unused' field MUST be cleared (i.e. ALL 'Unused' field octets MUST be set to 0x00).
*********************************************************************************************************
*/

                                                                        /* ---------- NET ICMP PARAM PROB HDR --------- */
typedef  struct  net_icmp_hdr_param_prob {
    CPU_INT08U      Type;                                               /* ICMP msg type.                               */
    CPU_INT08U      Code;                                               /* ICMP msg code.                               */
    CPU_INT16U      ChkSum;                                             /* ICMP msg chk sum.                            */

    CPU_INT08U      Ptr;                                                /* Ptr into ICMP err msg.                       */
    CPU_INT08U      Unused[NET_ICMP_HDR_NBR_OCTETS_UNUSED_PARAM_PROB];  /* See Note #2.                                 */

    CPU_INT08U      Data[NET_ICMP_MSG_ERR_LEN_MAX];
} NET_ICMP_HDR_PARAM_PROB;


/*$PAGE*/
/*
*********************************************************************************************************
*                                   ICMP ECHO REQUEST/REPLY HEADER
*
* Note(s) : (1) See RFC #792, Section 'Echo or Echo Reply Message' for ICMP 'Echo Request/Reply Message'
*               header format.
*
*           (2) 'Data' declared with 1 entry; prevents removal by compiler optimization.
*********************************************************************************************************
*/

                                                                        /* ------- NET ICMP ECHO REQ/REPLY HDR -------- */
typedef  struct  net_icmp_hdr_echo {
    CPU_INT08U      Type;                                               /* ICMP msg type.                               */
    CPU_INT08U      Code;                                               /* ICMP msg code.                               */
    CPU_INT16U      ChkSum;                                             /* ICMP msg chk sum.                            */

    CPU_INT16U      ID;                                                 /* ICMP msg ID.                                 */
    CPU_INT16U      SeqNbr;                                             /* ICMP seq nbr.                                */

    CPU_INT08U      Data[1];                                            /* ICMP msg data (see Note #2).                 */
} NET_ICMP_HDR_ECHO;


/*
*********************************************************************************************************
*                                   ICMP INTERNET TIMESTAMP HEADER
*
* Note(s) : (1) See RFC #792, Section 'Timestamp or Timestamp Reply Message' for ICMP 'Internet Timestamp
*               Message' header format.
*********************************************************************************************************
*/

                                                                        /* -------- NET ICMP TS REQ/REPLY HDR --------- */
typedef  struct  net_icmp_hdr_ts {
    CPU_INT08U      Type;                                               /* ICMP msg type.                               */
    CPU_INT08U      Code;                                               /* ICMP msg code.                               */
    CPU_INT16U      ChkSum;                                             /* ICMP msg chk sum.                            */

    CPU_INT16U      ID;                                                 /* ICMP msg ID.                                 */
    CPU_INT16U      SeqNbr;                                             /* ICMP seq nbr.                                */

    NET_TS          TS_Originate;                                       /* TS @ req tx.                                 */
    NET_TS          TS_Rx;                                              /* TS @ target rx.                              */
    NET_TS          TS_Tx;                                              /* TS @ target tx.                              */
} NET_ICMP_HDR_TS;


/*
*********************************************************************************************************
*                               ICMP ADDRESS MASK REQUEST/REPLY HEADER
*
* Note(s) : (1) See RFC #950, Appendix I 'Address Mask ICMP' for ICMP 'Address Mask Request/Reply Message'
*               header format.
*********************************************************************************************************
*/

                                                                        /* ----- NET ICMP ADDR MASK REQ/REPLY HDR ----- */
typedef  struct  net_icmp_hdr_addr_mask {
    CPU_INT08U      Type;                                               /* ICMP msg type.                               */
    CPU_INT08U      Code;                                               /* ICMP msg code.                               */
    CPU_INT16U      ChkSum;                                             /* ICMP msg chk sum.                            */

    CPU_INT16U      ID;                                                 /* ICMP msg ID.                                 */
    CPU_INT16U      SeqNbr;                                             /* ICMP seq nbr.                                */

    NET_IP_ADDR     AddrMask;                                           /* Addr mask.                                   */
} NET_ICMP_HDR_ADDR_MASK;


/*$PAGE*/
/*
*********************************************************************************************************
*                   ICMP REQUEST MESSAGE IDENTIFICATION & SEQUENCE NUMBER DATA TYPE
*********************************************************************************************************
*/

                                                                        /* ------- NET ICMP REQ MSG ID/SEQ NBR -------- */
typedef  struct  net_icmp_req_id_seq {
    CPU_INT16U      ID;                                                 /* ICMP Req Msg ID.                             */
    CPU_INT16U      SeqNbr;                                             /* ICMP Req Msg Seq Nbr.                        */
} NET_ICMP_REQ_ID_SEQ;


/*
*********************************************************************************************************
*                             ICMP TRANSMIT SOURCE QUENCH LIST DATA TYPE
*
*                                       NET_ICMP_TX_SRC_QUENCH
*                                          |-------------|
*                                          |  ICMP Type  |
*                              Previous    |-------------|
*                                ICMP  <----------O      |
*                                entry     |-------------|     Next
*                                          |      O----------> ICMP
*                                          |-------------|     entry
*                                          |  IP Address |
*                                          |-------------|
*                                          |   Transmit  |
*                                          |   Counter   |
*                                          |-------------|
*
*
* Note(s) : (1) Forced word-alignment at start of ICMP entry NOT required since first data member 'Type'
*               is declared as 'CPU_INT32U'.
*********************************************************************************************************
*/

                                                                    /* ------------ NET ICMP TX SRC QUENCH ------------ */
struct  net_icmp_tx_src_quench {
    NET_TYPE                    Type;                               /* Type cfg'd @ init : NET_ICMP_TYPE_TX_SRC_QUENCH. */

    NET_ICMP_TX_SRC_QUENCH     *PrevPtr;                            /* Ptr to PREV ICMP tx src quench entry.            */
    NET_ICMP_TX_SRC_QUENCH     *NextPtr;                            /* Ptr to NEXT ICMP tx src quench entry.            */

    NET_IP_ADDR                 Addr;                               /* IP addr to tx src quench err msg.                */
    CPU_INT16U                  TxCtr;                              /* Ctr lim's  tx src quench err msgs.               */

    CPU_INT16U                  Flags;
};


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)                  /* -------------- ICMP TX SRC QUENCH -------------- */

NET_ICMP_EXT  NET_ICMP_TX_SRC_QUENCH   NetICMP_TxSrcQuenchTbl[NET_ICMP_CFG_TX_SRC_QUENCH_SIZE];
NET_ICMP_EXT  NET_STAT_POOL            NetICMP_TxSrcQuenchPoolStat;

NET_ICMP_EXT  NET_ICMP_TX_SRC_QUENCH  *NetICMP_TxSrcQuenchPoolPtr;  /* Ptr to pool of free ICMP Tx Src Quench entries.  */
NET_ICMP_EXT  NET_ICMP_TX_SRC_QUENCH  *NetICMP_TxSrcQuenchListHead; /* Ptr to head of      ICMP Tx Src Quench List.     */
NET_ICMP_EXT  NET_ICMP_TX_SRC_QUENCH  *NetICMP_TxSrcQuenchListTail; /* Ptr to tail of      ICMP Tx Src Quench List.     */

NET_ICMP_EXT  CPU_INT16U               NetICMP_TxSrcQuenchTxTh_nbr; /* Nbr of rx'd IP pkts to tx ICMP Src Quench Msg.   */
#endif


NET_ICMP_EXT  CPU_INT16U               NetICMP_TxSeqNbrCtr;         /* Global tx seq nbr field ctr.                     */


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      NET_ICMP_TX_GET_SEQ_NBR()
*
* Description : Get next ICMP transmit message sequence number.
*
* Argument(s) : seq_nbr     Variable that will receive the returned ICMP transmit message sequence number.
*
* Return(s)   : none.
*
* Caller(s)   : NetICMP_TxMsgReq().
*
*               This macro is an INTERNAL network protocol suite macro & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Return ICMP sequence number is NOT converted from host-order to network-order.
*********************************************************************************************************
*/

#define  NET_ICMP_TX_GET_SEQ_NBR(seq_nbr)           { NET_UTIL_VAL_COPY_16(&(seq_nbr), &NetICMP_TxSeqNbrCtr); \
                                                      NetICMP_TxSeqNbrCtr++;                                  }


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*
* Note(s) : (1) See 'net_os.c    NetOS_ICMP_TxMsgReq()  Note #1' &
*                   'net_icmp.c  NetICMP_TxMsgReq()     Note #3'.
*********************************************************************************************************
*/

void                 NetICMP_Init            (void);


#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
CPU_BOOLEAN          NetICMP_CfgTxSrcQuenchTh(CPU_INT16U    th);
#endif



void                 NetICMP_Rx              (NET_BUF      *pbuf,
                                              NET_ERR      *perr);



void                 NetICMP_TxMsgErr        (NET_BUF      *pbuf,
                                              CPU_INT08U    type,
                                              CPU_INT08U    code,
                                              CPU_INT08U    ptr,
                                              NET_ERR      *perr);

NET_ICMP_REQ_ID_SEQ  NetICMP_TxMsgReq        (CPU_INT08U    type,           /* See Note #1.                             */
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
                                              NET_ERR      *perr);



void                 NetICMP_HandlerRx       (NET_BUF      *pbuf);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                      DEFINED IN OS'S  net_os.c
*
* Note(s) : (1) See 'net_os.c    NetOS_ICMP_TxMsgReq()  Note #1' &
*                   'net_icmp.c  NetICMP_TxMsgReq()     Note #3'.
*********************************************************************************************************
*/

NET_ICMP_REQ_ID_SEQ  NetOS_ICMP_TxMsgReq     (CPU_INT08U    type,           /* See Note #1.                             */
                                              CPU_INT08U    code,
                                              NET_IP_ADDR   addr_src,
                                              NET_IP_ADDR   addr_dest,
                                              NET_IP_TOS    TOS,
                                              NET_IP_TTL    TTL,
                                              CPU_INT16U    flags,
                                              void         *popts,
                                              void         *p_data,
                                              CPU_INT16U    data_len,
                                              NET_ERR      *perr);


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_ICMP_CFG_TX_SRC_QUENCH_EN
#error  "NET_ICMP_CFG_TX_SRC_QUENCH_EN          not #define'd in 'net_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((NET_ICMP_CFG_TX_SRC_QUENCH_EN != DEF_DISABLED) && \
        (NET_ICMP_CFG_TX_SRC_QUENCH_EN != DEF_ENABLED ))
#error  "NET_ICMP_CFG_TX_SRC_QUENCH_EN    illegally #define'd in 'net_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif   (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)


#ifndef  NET_ICMP_CFG_TX_SRC_QUENCH_SIZE
#error  "NET_ICMP_CFG_TX_SRC_QUENCH_SIZE        not #define'd in 'net_cfg.h'       "
#error  "                                 [MUST be  >= NET_ICMP_SRC_QUENCH_NBR_MIN]"
#error  "                                 [     &&  <= NET_ICMP_SRC_QUENCH_NBR_MAX]"
#elif  ((NET_ICMP_CFG_TX_SRC_QUENCH_SIZE < NET_ICMP_SRC_QUENCH_NBR_MIN) || \
        (NET_ICMP_CFG_TX_SRC_QUENCH_SIZE > NET_ICMP_SRC_QUENCH_NBR_MAX))
#error  "NET_ICMP_CFG_TX_SRC_QUENCH_SIZE  illegally #define'd in 'net_cfg.h'       "
#error  "                                 [MUST be  >= NET_ICMP_SRC_QUENCH_NBR_MIN]"
#error  "                                 [     &&  <= NET_ICMP_SRC_QUENCH_NBR_MAX]"
#endif


#endif

