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
*                                         NETWORK DATA TYPES
*
* Filename      : net_type.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            NETWORK TYPE
*
* Note(s) : (1) 'NET_TYPE' declared as 'CPU_INT32U' & all 'NET_TYPE's #define'd with large, non-trivial 
*               values to trap & discard invalid/corrupted network objects based on 'NET_TYPE'.
*********************************************************************************************************
*/

typedef  CPU_INT32U  NET_TYPE;


/*
*********************************************************************************************************
*                          NETWORK MAXIMUM TRANSMISSION UNIT (MTU) DATA TYPE
*
* Note(s) : (1) NET_MTU_MIN_VAL & NET_MTU_MAX_VAL  SHOULD be #define'd based on 'NET_MTU' data type declared.
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_MTU;
                                                                /* See Note #1.                                         */
#define  NET_MTU_MIN_VAL                DEF_INT_16U_MIN_VAL
#define  NET_MTU_MAX_VAL                DEF_INT_16U_MAX_VAL


/*
*********************************************************************************************************
*                                     NETWORK CHECK-SUM DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_CHK_SUM;


/*
*********************************************************************************************************
*                                     NETWORK TIMESTAMP DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT32U  NET_TS;


/*
*********************************************************************************************************
*                                       NETWORK TIMER DATA TYPES
*********************************************************************************************************
*/

typedef  struct  net_tmr   NET_TMR;


/*
*********************************************************************************************************
*                                      NETWORK BUFFER DATA TYPES
*********************************************************************************************************
*/

typedef  struct  net_buf   NET_BUF;


/*
*********************************************************************************************************
*                                    NETWORK CONNECTION DATA TYPES
*********************************************************************************************************
*/

typedef  CPU_INT16S        NET_CONN_ID;

typedef  struct  net_conn  NET_CONN;


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NETWORK PROTOCOL DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_PROTOCOL_TYPE;


/*
*********************************************************************************************************
*                            NETWORK TRANSPORT LAYER PORT NUMBER DATA TYPE
*
* Note(s) : (1) NET_PORT_NBR_MAX  SHOULD be #define'd based on 'NET_PORT_NBR' data type declared.
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_PORT_NBR;

#define  NET_PORT_NBR_MAX               DEF_INT_16U_MAX_VAL     /* See Note #1.                                         */


/*
*********************************************************************************************************
*                        NETWORK TRANSPORT LAYER PORT NUMBER QUANTITY DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_PORT_NBR_QTY;                          /* Defines max qty of port nbrs to support.             */


/*
*********************************************************************************************************
*                                    NETWORK ARP LAYER DATA TYPES
*********************************************************************************************************
*/

typedef  struct  net_arp_cache  NET_ARP_CACHE;


/*
*********************************************************************************************************
*                                     NETWORK IP LAYER DATA TYPES
*********************************************************************************************************
*/

typedef  CPU_INT32U  NET_IP_ADDR;                               /* Defines IPv4 IP addr size.                           */


/*
*********************************************************************************************************
*                                    NETWORK ICMP LAYER DATA TYPES
*********************************************************************************************************
*/

typedef  struct  net_icmp_tx_src_quench  NET_ICMP_TX_SRC_QUENCH;


/*
*********************************************************************************************************
*                                    NETWORK TCP LAYER DATA TYPES
*********************************************************************************************************
*/

typedef  CPU_INT32U            NET_TCP_SEQ_NBR;
typedef  CPU_INT16U            NET_TCP_SEG_SIZE;
typedef  CPU_INT16U            NET_TCP_WIN_SIZE;

typedef  CPU_INT32U            NET_TCP_TX_RTT_MS;
typedef  NET_TCP_TX_RTT_MS     NET_TCP_TX_RTT_TS_MS;

typedef  struct  net_tcp_conn  NET_TCP_CONN;


/*
*********************************************************************************************************
*                                   NETWORK SOCKET LAYER DATA TYPES
*********************************************************************************************************
*/

typedef  struct  net_sock      NET_SOCK;

