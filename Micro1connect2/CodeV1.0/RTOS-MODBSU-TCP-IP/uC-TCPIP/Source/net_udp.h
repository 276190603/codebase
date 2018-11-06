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
* Filename      : net_udp.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports User Datagram Protocol as described in RFC #768.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_UDP_MODULE
#define  NET_UDP_EXT
#else
#define  NET_UDP_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                          UDP APPLICATION-PROTOCOL-INTERFACE SELECT DEFINES
*
* Note(s) : (1) The following UDP values MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_cfg.h' so 
*               that the developer can configure UDP for the desired application receive demultiplex
*               selection (see 'net_def.h  UDP LAYER DEFINES  Note #1') :
*
*                   NET_UDP_APP_API_SEL_SOCK                       0
*                   NET_UDP_APP_API_SEL_APP                        1
*                   NET_UDP_APP_API_SEL_SOCK_APP                   2
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                         UDP HEADER DEFINES
*
* Note(s) : (1) The following UDP value MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_buf.h' so that
*               the Network Buffer Module can configure maximum buffer header size (see 'net_def.h  UDP
*               LAYER DEFINES' & 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1') :
*
*               (a) NET_UDP_HDR_SIZE_MAX                   8
*********************************************************************************************************
*/

#define  NET_UDP_HDR_SIZE                                  8    /*  = sizeof(NET_UDP_HDR)                               */
#define  NET_UDP_HDR_SIZE_MIN                            NET_UDP_HDR_SIZE
#if 0                                                           /* See Note #1a.                                        */
#define  NET_UDP_HDR_SIZE_MAX                            NET_UDP_HDR_SIZE
#endif

#define  NET_UDP_HDR_SIZE_TOT_MIN                       (NET_IP_HDR_SIZE_TOT_MIN + NET_UDP_HDR_SIZE_MIN)
#define  NET_UDP_HDR_SIZE_TOT_MAX                       (NET_IP_HDR_SIZE_TOT_MAX + NET_UDP_HDR_SIZE_MAX)


#define  NET_UDP_PSEUDO_HDR_SIZE                          12    /*  = sizeof(NET_UDP_PSEUDO_HDR)                        */


#define  NET_UDP_PORT_NBR_RESERVED                       NET_PORT_NBR_RESERVED
#define  NET_UDP_PORT_NBR_NONE                           NET_UDP_PORT_NBR_RESERVED


#define  NET_UDP_HDR_CHK_SUM_POS_ZERO                 0x0000
#define  NET_UDP_HDR_CHK_SUM_NEG_ZERO                 0xFFFF
#define  NET_UDP_HDR_CHK_SUM_NONE                        NET_UDP_HDR_CHK_SUM_POS_ZERO


/*$PAGE*/
/*
*********************************************************************************************************
*                   UDP DATA/TOTAL LENGTH & MAXIMUM TRANSMISSION UNIT (MTU) DEFINES
*
* Note(s) : (1) (a) UDP total length #define's (NET_UDP_TOT_LEN)  relate to the total size of a complete
*                   UDP packet, including the packet's UDP header.  Note that a complete UDP packet MAY
*                   be fragmented in multiple Internet Protocol packets.
*
*               (b) UDP data  length #define's (NET_UDP_DATA_LEN) relate to the data  size of a complete
*                   UDP packet, equal to the total UDP packet length minus its UDP header size.  Note 
*                   that a complete UDP packet MAY be fragmented in multiple Internet Protocol packets.
*
*               (c) UDP MTU #define's (NET_UDP_MTU) relate to the data size for any single, non-fragmented
*                   UDP packet, equal to a packet's Internet Protocol MTU minus its UDP header size.
*
*           (2) The 'NET_UDP_MTU_NET_RSRC' pre-processor 'else'-conditional code will never be compiled/
*               linked since 'net_buf.h' ensures that at least one of the two configuration constants 
*               (NET_BUF_CFG_NBR_SMALL or NET_BUF_CFG_NBR_LARGE) will be configured with a value greater
*               than zero (see 'net_buf.h  CONFIGURATION ERRORS').  The 'else'-conditional code is
*               included for completeness & as an extra precaution in case 'net_buf.h' is incorrectly
*               modified.
*********************************************************************************************************
*/

                                                                                /* See Notes #1a & #1b.                 */
#define  NET_UDP_DATA_LEN_MIN                              0

#define  NET_UDP_TOT_LEN_MIN                            (NET_UDP_HDR_SIZE    + NET_UDP_DATA_LEN_MIN)
#define  NET_UDP_TOT_LEN_MAX                            (NET_IP_TOT_LEN_MAX  - NET_IP_HDR_SIZE_MIN )

#define  NET_UDP_DATA_LEN_MAX                           (NET_UDP_TOT_LEN_MAX - NET_UDP_HDR_SIZE    )


                                                                                /* See Note #1c.                        */
#define  NET_UDP_MTU_MIN                                (NET_IP_MTU_MIN      - NET_UDP_HDR_SIZE    )
#define  NET_UDP_MTU_MAX                                (NET_IP_MTU_MAX      - NET_UDP_HDR_SIZE    )
#define  NET_UDP_MTU                                     NET_UDP_MTU_MIN


#define  NET_UDP_MTU_NET_RSRC                            NET_UDP_MTU_MAX

#if     (NET_BUF_CFG_NBR_LARGE > 0)
#if    ((NET_BUF_CFG_DATA_SIZE_LARGE - NET_BUF_DATA_SIZE_MIN) < NET_UDP_MTU_NET_RSRC)
#undef   NET_UDP_MTU_NET_RSRC
#define  NET_UDP_MTU_NET_RSRC                           (NET_BUF_CFG_DATA_SIZE_LARGE - NET_BUF_DATA_SIZE_MIN)
#endif

#elif   (NET_BUF_CFG_NBR_SMALL > 0)
#if    ((NET_BUF_CFG_DATA_SIZE_SMALL - NET_BUF_DATA_SIZE_MIN) < NET_UDP_MTU_NET_RSRC)
#undef   NET_UDP_MTU_NET_RSRC
#define  NET_UDP_MTU_NET_RSRC                           (NET_BUF_CFG_DATA_SIZE_SMALL - NET_BUF_DATA_SIZE_MIN)
#endif

#else                                                                           /* See Note #2.                         */
#error  "NET_BUF_CFG_NBR_SMALL  illegally #defined in 'net_buf.h'      "
#error  "NET_BUF_CFG_NBR_LARGE  illegally #defined in 'net_buf.h'      "
#error  "                       [See 'net_buf.h  CONFIGURATION ERRORS']"
#endif


#define  NET_UDP_MTU_ACTUAL                     (DEF_MIN(NET_UDP_MTU, NET_UDP_MTU_NET_RSRC))


/*
*********************************************************************************************************
*                                          UDP FLAG DEFINES
*********************************************************************************************************
*/

                                                                /* ------------------ NET UDP FLAGS ------------------- */
#define  NET_UDP_FLAG_NONE                        DEF_BIT_NONE

                                                                /* ------------------ UDP TX  FLAGS ------------------- */
#define  NET_UDP_FLAG_TX_CHK_SUM_DIS              DEF_BIT_00    /* DISABLE tx        chk sums.                          */
#define  NET_UDP_FLAG_TX_BLOCK                    DEF_BIT_07

                                                                /* ------------------ UDP RX  FLAGS ------------------- */
#define  NET_UDP_FLAG_RX_CHK_SUM_NONE_DISCARD     DEF_BIT_08    /* Discard rx'd NULL chk sum UDP datagrams.             */
#define  NET_UDP_FLAG_RX_DATA_PEEK                DEF_BIT_09
#define  NET_UDP_FLAG_RX_BLOCK                    DEF_BIT_15


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      UDP PORT NUMBER DATA TYPE
*********************************************************************************************************
*/

typedef  NET_PORT_NBR  NET_UDP_PORT_NBR;                        /* Defines UDP port nbr size.                           */


/*$PAGE*/
/*
*********************************************************************************************************
*                                             UDP HEADER
*
* Note(s) : (1) See RFC #768, Section 'Format' for UDP datagram header format.
*********************************************************************************************************
*/

                                                                /* ------------------- NET UDP HDR -------------------- */
typedef  struct  net_udp_hdr {
    NET_UDP_PORT_NBR        PortSrc;                            /* UDP datagram src  port.                              */
    NET_UDP_PORT_NBR        PortDest;                           /* UDP datagram dest port.                              */
    CPU_INT16U              DatagramLen;                        /* UDP datagram msg len.                                */
    NET_CHK_SUM             ChkSum;                             /* UDP datagram chk sum.                                */
} NET_UDP_HDR;


/*
*********************************************************************************************************
*                                          UDP PSEUDO-HEADER
*
* Note(s) : (1) See RFC #768, Section 'Fields : Checksum' for UDP datagram pseudo-header format.
*********************************************************************************************************
*/

                                                                /* ---------------- NET UDP PSEUDO-HDR ---------------- */
typedef  struct  net_udp_pseudo_hdr {
    NET_IP_ADDR             AddrSrc;                            /* UDP datagram src  addr.                              */
    NET_IP_ADDR             AddrDest;                           /* UDP datagram dest addr.                              */
    CPU_INT08U              Zero;                               /* Field MUST be zero'd; i.e. ALL bits clr'd.           */
    CPU_INT08U              Protocol;                           /* UDP datagram protocol.                               */
    CPU_INT16U              DatagramLen;                        /* UDP datagram tot len.                                */
} NET_UDP_PSEUDO_HDR;


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/


void        NetUDP_Init            (void);


                                                                /* --------------------- RX FNCTS --------------------- */
void        NetUDP_Rx              (NET_BUF           *pbuf,
                                    NET_ERR           *perr);

CPU_INT16U  NetUDP_RxAppData       (NET_BUF           *pbuf,
                                    void              *pdata_buf,
                                    CPU_INT16U         data_buf_len,
                                    CPU_INT16U         flags,
                                    void              *pip_opts_buf,
                                    CPU_INT08U         ip_opts_buf_len,
                                    CPU_INT08U        *pip_opts_len,
                                    NET_ERR           *perr);


                                                                /* --------------------- TX FNCTS --------------------- */
CPU_INT16U  NetUDP_TxAppData       (void              *p_data,
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
                                    NET_ERR           *perr);

CPU_INT16U  NetUDP_TxAppDataHandler(void              *p_data,
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
                                    NET_ERR           *perr);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                   DEFINED IN PRODUCT'S  net_bsp.c
*********************************************************************************************************
*/

#if ((NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_APP     ) || \
     (NET_UDP_CFG_APP_API_SEL == NET_UDP_APP_API_SEL_SOCK_APP))
void        NetUDP_RxAppDataHandler(NET_BUF           *pbuf,
                                    NET_IP_ADDR        src_addr,
                                    NET_UDP_PORT_NBR   src_port,
                                    NET_IP_ADDR        dest_addr,
                                    NET_UDP_PORT_NBR   dest_port,
                                    NET_ERR           *perr);
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_UDP_CFG_APP_API_SEL
#error  "NET_UDP_CFG_APP_API_SEL                  not #define'd in 'net_cfg.h'     "
#error  "                                   [MUST be  NET_UDP_APP_API_SEL_SOCK    ]"
#error  "                                   [     ||  NET_UDP_APP_API_SEL_APP     ]"
#error  "                                   [     ||  NET_UDP_APP_API_SEL_SOCK_APP]"
#elif  ((NET_UDP_CFG_APP_API_SEL != NET_UDP_APP_API_SEL_SOCK    ) && \
        (NET_UDP_CFG_APP_API_SEL != NET_UDP_APP_API_SEL_APP     ) && \
        (NET_UDP_CFG_APP_API_SEL != NET_UDP_APP_API_SEL_SOCK_APP))
#error  "NET_UDP_CFG_APP_API_SEL            illegally #define'd in 'net_cfg.h'     "
#error  "                                   [MUST be  NET_UDP_APP_API_SEL_SOCK    ]"
#error  "                                   [     ||  NET_UDP_APP_API_SEL_APP     ]"
#error  "                                   [     ||  NET_UDP_APP_API_SEL_SOCK_APP]"
#endif




#ifndef  NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN
#error  "NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN        not #define'd in 'net_cfg.h'"
#error  "                                   [MUST be  DEF_DISABLED]           "
#error  "                                   [     ||  DEF_ENABLED ]           "
#elif  ((NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN != DEF_DISABLED) && \
        (NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN != DEF_ENABLED ))
#error  "NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN  illegally #define'd in 'net_cfg.h'"
#error  "                                   [MUST be  DEF_DISABLED]           "
#error  "                                   [     ||  DEF_ENABLED ]           "
#endif



#ifndef  NET_UDP_CFG_TX_CHK_SUM_EN
#error  "NET_UDP_CFG_TX_CHK_SUM_EN                not #define'd in 'net_cfg.h'"
#error  "                                   [MUST be  DEF_DISABLED]           "
#error  "                                   [     ||  DEF_ENABLED ]           "
#elif  ((NET_UDP_CFG_TX_CHK_SUM_EN != DEF_DISABLED) && \
        (NET_UDP_CFG_TX_CHK_SUM_EN != DEF_ENABLED ))
#error  "NET_UDP_CFG_TX_CHK_SUM_EN          illegally #define'd in 'net_cfg.h'"
#error  "                                   [MUST be  DEF_DISABLED]           "
#error  "                                   [     ||  DEF_ENABLED ]           "
#endif

