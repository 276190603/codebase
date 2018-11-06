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
* Filename      : net_if_pkt.h
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
*                                               MODULE
*
* Note(s) : (1) Packet-based Network Interface Layer module is required for some network interfaces (see
*               'net_if_pkt.h  Note #1).
*
*           (2) The following packet-based-network-interface-module-present configuration value MUST be
*               pre-#define'd in 'net_cfg_net.h' PRIOR to all other network modules that require Packet-
*               Based Network Interface Layer Configuration (see  'net_cfg_net.h  NETWORK INTERFACE LAYER
*               CONFIGURATION  Note #3') :
*
*                   NET_IF_PKT_MODULE_PRESENT
*********************************************************************************************************
*/

#ifdef   NET_IF_PKT_MODULE_PRESENT                              /* See Note #2.                                         */


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_IF_PKT_MODULE
#define  NET_IF_PKT_EXT
#else
#define  NET_IF_PKT_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*
* Note(s) : (1) Network Interface Receive Task Handler functions named 'NetIF_' ONLY to allow NIC & OS port
*               interfaces to be generic for either packet-based or character-based network interface.
*********************************************************************************************************
*/

void         NetIF_Pkt_Init         (NET_ERR  *perr);


                                                                /* --------------------- RX FNCTS --------------------- */
void         NetIF_RxTaskHandlerInit(void);

void         NetIF_RxTaskHandler    (void);


                                                                /* --------------------- TX FNCTS --------------------- */
void         NetIF_Pkt_Tx           (NET_BUF  *pbuf,
                                     NET_ERR  *perr);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                      DEFINED IN OS'S  net_os.c
*
* Note(s) : (1) OS interface functions named 'IF' ONLY since OS port MUST be generic for either packet-
*               based or character-based network interface.
*********************************************************************************************************
*/

void         NetOS_IF_Init          (NET_ERR  *perr);          /* Create pkt IF objs & start rx task.                  */


void         NetOS_IF_RxTaskWait    (NET_ERR  *perr);          /* Wait   for pkt IF rx signal from NIC Rx ISR.         */

void         NetOS_IF_RxTaskSignal  (NET_ERR  *perr);          /* Signal     pkt IF rx rdy    from NIC Rx ISR.         */


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_if_pkt.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of pkt-based net IF module include (see Note #1).*/

