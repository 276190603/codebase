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
*                                           NETWORK DEFINES
*
* Filename      : net_def.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           NETWORK DEFINES
*********************************************************************************************************
*/

#define  NET_FLAG_NONE                                   DEF_BIT_NONE

#define  NET_TS_NONE                                       0L


#define  NET_TX_SUSPEND_TIMEOUT_MIN_MS                     0
#define  NET_TX_SUSPEND_TIMEOUT_MAX_MS                   100


/*
*********************************************************************************************************
*                                    NETWORK PROTOCOL TYPE DEFINES
*
* Note(s) : (1) See 'net.h  Note #1'.
*********************************************************************************************************
*/

                                                                /* ---------------- NET PROTOCOL TYPES ---------------- */
#define  NET_PROTOCOL_TYPE_NONE                            0
#define  NET_PROTOCOL_TYPE_ALL                             1

                                                                /* --------------- LINK LAYER PROTOCOLS --------------- */
#define  NET_PROTOCOL_TYPE_FRAME                          10

#define  NET_PROTOCOL_TYPE_ARP                            20

                                                                /* ---------------- NET LAYER PROTOCOLS --------------- */
#define  NET_PROTOCOL_TYPE_IP_V4                          30
#define  NET_PROTOCOL_TYPE_IP_V4_OPT                      31
#define  NET_PROTOCOL_TYPE_ICMP                           32

                                                                /* ------------- TRANSPORT LAYER PROTOCOLS ------------ */
#define  NET_PROTOCOL_TYPE_UDP                            40
#define  NET_PROTOCOL_TYPE_TCP                            41

                                                                /* ---------------- APP LAYER PROTOCOLS --------------- */
#define  NET_PROTOCOL_TYPE_APP                            50
#define  NET_PROTOCOL_TYPE_SOCK                           51


/*$PAGE*/
/*
*********************************************************************************************************
*                               PARAMETER CONFIGURATION INITIALIZATION
*
* Note(s) : (1) (a) Configure NET_CFG_INIT_CFG_VALS in 'net_cfg.h' with the desired configuration for the
*                   initialization of network protocol suite configurable parameters  :
*
*                       NET_INIT_CFG_VALS_DFLT          Configure network parameters with default values
*                       NET_INIT_CFG_VALS_APP_INIT      Configure network parameters with application-
*                                                           initialized values (recalled from non-volatile 
*                                                           memory &/or hard-coded in product's application)
*
*               (b) (1) When NET_CFG_INIT_CFG_VALS is configured as NET_INIT_CFG_VALS_DFLT, the network
*                       protocol suite's configurable parameters are configured with default values.
*
*                       The application need only call Net_Init() to initialize both the network protocol 
*                       suite & its configurable parameters.
*
*                   (2) When NET_CFG_INIT_CFG_VALS is configured as NET_INIT_CFG_VALS_APP_INIT, the 
*                       application MUST ...
*
*                       (A) Initialize ALL network protocol suite configurable parameters from values
*                           recalled from non-volatile memory &/or hard-coded application values :
*
*                           (1) Call each of the configuration functions in Net_InitDflt() passing the
*                               application-recalled &/or hard-coded values
*                                 OR
*                           (2) Call Net_InitDflt() to initialize ALL network protocol suite configurable
*                               parameters to their default values & then call any of the configuration
*                               functions in Net_InitDflt() passing the recalled &/or hard-coded values
*
*                       (B) Call Net_Init()
*
*               See also 'net.c  Net_InitDflt()  Notes #1 & #2'
*                      & 'net_cfg.h  NETWORK CONFIGURATION  Note #1'.
*********************************************************************************************************
*/

#define  NET_INIT_CFG_VALS_NONE                            0
#define  NET_INIT_CFG_VALS_DFLT                            1
#define  NET_INIT_CFG_VALS_APP_INIT                        2


/*
*********************************************************************************************************
*                                     OPTIMIZATION CONFIGURATION
*
* Note(s) : (1) Configure NET_CFG_OPTIMIZE in 'net_cfg.h' with the desired network performance optimization :
*
*                   NET_OPTIMIZE_SPD            Optimizes network protocol suite for speed performance
*                   NET_OPTIMIZE_SIZE           Optimizes network protocol suite for binary image size
*********************************************************************************************************
*/

#define  NET_OPTIMIZE_NONE                                 0
#define  NET_OPTIMIZE_SPD                                  1
#define  NET_OPTIMIZE_SIZE                                 2


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                NETWORK MODULE & LAYER GLOBAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*
* Note(s) : (1) These module & layer global #define's are required PRIOR to network configuration.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                   NETWORK INTERFACE CARD DEFINES
*********************************************************************************************************
*/

#define  NET_NIC_RD_WR_SEL_NONE                            0
#define  NET_NIC_RD_WR_SEL_FNCT                            1
#define  NET_NIC_RD_WR_SEL_MACRO                           2


/*
*********************************************************************************************************
*                                   NETWORK INTERFACE LAYER DEFINES
*
* Note(s) : (1) ALL supported network interface types MUST be #define'd here so that the developer can
*               select & configure the desired network interface in 'net_cfg.h'.
*
*           (2) See '\<Network Protocol Suite>\IF\Ether\net_if.h  NETWORK INTERFACE HEADER DEFINES  Note #1'.
*********************************************************************************************************
*/
                                                                /* ------------------- NET IF TYPES ------------------- */
#define  NET_IF_TYPE_NONE                                  0

                                                                /* --------------------- ETHERNET --------------------- */
#define  NET_IF_TYPE_ETHER                                 1
#define  NET_IF_HDR_SIZE_ETHER                            14    /* See Note #2.                                         */


/*
*********************************************************************************************************
*                                          ARP LAYER DEFINES
*
* Note(s) : (1) See 'net_arp.h  ARP HARDWARE & PROTOCOL DEFINES  Note #1'.
*********************************************************************************************************
*/
                                                                /* ------------------- ARP HW TYPES ------------------- */
#define  NET_ARP_HW_TYPE_NONE                         0x0000

#define  NET_ARP_HW_TYPE_ETHER                        0x0001
#define  NET_ARP_HW_ADDR_LEN_ETHER                         6

                                                                /* ---------------- ARP PROTOCOL TYPES ---------------- */
#define  NET_ARP_PROTOCOL_TYPE_NONE                   0x0000

#define  NET_ARP_PROTOCOL_TYPE_IP_V4                  0x0800
#define  NET_ARP_PROTOCOL_ADDR_LEN_IP_V4                   4


/*
*********************************************************************************************************
*                                          IP LAYER DEFINES
*********************************************************************************************************
*/

#define  NET_IP_HDR_SIZE_MAX                              60    /* See 'net_ip.h  IP HEADER DEFINES  Note #1'.          */


/*$PAGE*/
/*
*********************************************************************************************************
*                                       TRANSPORT LAYER DEFINES
*
* Note(s) : (1) The following transport layer values are pre-#define'd in 'net_type.h' (see 'net_type.h  
*               NETWORK TRANSPORT LAYER PORT NUMBER DATA TYPE  Note #1') :
*
*               (a) NET_PORT_NBR_MAX                     DEF_INT_16U_MAX_VAL
*********************************************************************************************************
*/

#define  NET_TRANSPORT_LAYER_SEL_NONE                      0
#define  NET_TRANSPORT_LAYER_SEL_UDP                       1
#define  NET_TRANSPORT_LAYER_SEL_UDP_TCP                   2


#define  NET_PORT_NBR_RESERVED                             0
#define  NET_PORT_NBR_NONE                               NET_PORT_NBR_RESERVED

#define  NET_PORT_NBR_MIN                                  1
#if 0                                                           /* See Note #1a.                                        */
#define  NET_PORT_NBR_MAX                                DEF_INT_16U_MAX_VAL
#endif


/*
*********************************************************************************************************
*                                          UDP LAYER DEFINES
*
* Note(s) : (1) See 'net_udp.h  UDP APPLICATION-PROTOCOL-INTERFACE SELECT DEFINES  Note #1'.
*********************************************************************************************************
*/

#define  NET_UDP_HDR_SIZE_MAX                              8    /* See 'net_udp.h  UDP HEADER DEFINES  Note #1'.        */


                                                                /* ----------------- UDP/APP API SEL ------------------ */
                                                                /* See Note #1.                                         */
#define  NET_UDP_APP_API_SEL_NONE                          0
#define  NET_UDP_APP_API_SEL_SOCK                          1
#define  NET_UDP_APP_API_SEL_APP                           2
#define  NET_UDP_APP_API_SEL_SOCK_APP                      3


/*
*********************************************************************************************************
*                                          TCP LAYER DEFINES
*********************************************************************************************************
*/

#define  NET_TCP_HDR_SIZE_MAX                             60    /* See 'net_tcp.h  TCP HEADER DEFINES  Note #1'.        */


/*$PAGE*/
/*
*********************************************************************************************************
*                                BSD 4.x & NETWORK SOCKET LAYER DEFINES
*
* Note(s) : (1) See 'net_sock.h  NETWORK SOCKET FAMILY & PROTOCOL DEFINES  Note #1' &
*                   'net_sock.h  NETWORK SOCKET ADDRESS DEFINES'.
*********************************************************************************************************
*/

                                                                /* ------------ SOCK FAMILY/PROTOCOL TYPES ------------ */
#define  NET_SOCK_FAMILY_NONE                              0
#define  NET_SOCK_PROTOCOL_FAMILY_NONE                   NET_SOCK_FAMILY_NONE
#define  NET_SOCK_ADDR_FAMILY_NONE                       NET_SOCK_FAMILY_NONE

                                                                /* ------------------- TCP/IP SOCKS ------------------- */
#ifdef   AF_INET
#undef   AF_INET
#endif
#define  AF_INET                                           2

#ifdef   PF_INET
#undef   PF_INET
#endif
#define  PF_INET                                     AF_INET


#define  NET_SOCK_PROTOCOL_FAMILY_IP_V4              PF_INET    /* TCP/IP sock protocol family type.                    */
#define  NET_SOCK_ADDR_FAMILY_IP_V4                  AF_INET    /* TCP/IP sock addr     family type.                    */
#define  NET_SOCK_FAMILY_IP_V4                           NET_SOCK_PROTOCOL_FAMILY_IP_V4

#define  NET_SOCK_ADDR_LEN_IP_V4                           6    /* TCP/IP sock addr len =                               */
                                                                /*             2-octet TCP/UDP port value               */
                                                                /*             4-octet IP      addr value               */

#define  NET_SOCK_PROTOCOL_MAX_MIN                         1

#define  NET_SOCK_PROTOCOL_MAX_IP_V4_UDP                   1
#define  NET_SOCK_PROTOCOL_MAX_IP_V4_UDP_TCP               2
#define  NET_SOCK_PROTOCOL_MAX_IP_V4                     NET_SOCK_PROTOCOL_MAX_IP_V4_UDP_TCP



                                                                /* ------------------ SOCK BLOCK SEL ------------------ */
#define  NET_SOCK_BLOCK_SEL_NONE                           0
#define  NET_SOCK_BLOCK_SEL_DFLT                           1    /* Block behavior determined by run-time sock opts.     */
#define  NET_SOCK_BLOCK_SEL_BLOCK                          2
#define  NET_SOCK_BLOCK_SEL_NO_BLOCK                       3


/*
*********************************************************************************************************
*                                NETWORK CONNECTION MANAGEMENT DEFINES
*********************************************************************************************************
*/

                                                                /* ---------------- CONN FAMILY TYPES ----------------- */
#define  NET_CONN_FAMILY_NONE                              0
                                                                /* Net Socket Cfg.                                      */
#define  NET_CONN_FAMILY_IP_V4_SOCK                      NET_SOCK_FAMILY_IP_V4
#define  NET_CONN_ADDR_LEN_IP_V4_SOCK                    NET_SOCK_ADDR_LEN_IP_V4
#define  NET_CONN_PROTOCOL_MAX_IP_V4_SOCK                NET_SOCK_PROTOCOL_MAX_IP_V4


#define  NET_CONN_TYPE_CONN_NONE                           0
#define  NET_CONN_TYPE_CONN_HALF                           1
#define  NET_CONN_TYPE_CONN_FULL                           2

