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
*                                         NETWORK HEADER FILE
*
* Filename      : net.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) (a) The following network protocols are supported/implemented :
*
*                                                                             ---- LINK LAYER PROTOCOLS -----
*                         (1) (A) ARP   Address Resolution Protocol
*                                                                             --- NETWORK LAYER PROTOCOLS ---
*                         (2) (A) IP    Internet Protocol
*                             (B) ICMP  Internet Control Message Protocol
*                                                                             -- TRANSPORT LAYER PROTOCOLS --
*                         (3) (A) UDP   User Datagram Protocol
*                             (B) TCP   Transmission Control Protocol
*
*                     (b) The following network protocols are intentionally NOT supported/implemented :
*
*                                                                             ---- LINK LAYER PROTOCOLS -----
*                         (1) (A) RARP  Reverse Address Resolution Protocol
*                                                                             --- NETWORK LAYER PROTOCOLS ---
*                         (2) (A) IGMP  Internet Group Management  Protocol
*
*
*                 (2) To protect the validity & prevent the corruption of shared network protocol resources,
*                     the primary tasks of the network protocol suite are prevented from running concurrently
*                     through the use of a global network lock implementing protection by mutual exclusion.
*
*                     (a) The mechanism of protected mutual exclusion is irrelevant but MUST be implemented 
*                         in the following two functions :
*
*                             NetOS_Lock()                          acquire access to network protocol suite
*                             NetOS_Unlock()                        release access to network protocol suite
*
*                         implemented in 
*
*                             \<Network Protocol Suite>\OS\<os>\net_os.*
*
*                             where
*                                     <Network Protocol Suite>      directory path for network protocol suite
*                                     <os>                          directory name for specific OS
*
*                     (b) Since this global lock implements mutual exclusion at the network protocol suite
*                         task level, critical sections are NOT required to prevent task-level concurrency
*                         in the network protocol suite.
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) This main network protocol suite header file is protected from multiple pre-processor
*               inclusion through use of the network module present pre-processor macro definition.
*
*               See also 'NETWORK INCLUDE FILES  Note #5'.
*********************************************************************************************************
*/

#ifndef  NET_MODULE_PRESENT                                     /* See Note #1.                                         */
#define  NET_MODULE_PRESENT


/*
*********************************************************************************************************
*                                       NETWORK VERSION NUMBER
*
* Note(s) : (1) (a) The network protocol suite software version is denoted as follows :
*
*                       Vx.yy
*
*                           where
*                                   V               denotes 'Version' label
*                                   x               denotes major software version revision number
*                                   yy              denotes minor software version revision number
*
*               (b) The software version label #define is formatted as follows :
*
*                       ver = x.yy * 100
*
*                           where
*                                   ver             denotes software version number scaled as an integer value
*                                   x.yy            denotes software version number
*********************************************************************************************************
*/

#define  NET_VERSION                                     192u   /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_MODULE
#define  NET_EXT
#else
#define  NET_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NETWORK INCLUDE FILES
*
* Note(s) : (1) The network protocol suite files are located in the following directories :
*
*               (a) (1) \<Your Product Application>\app_cfg.h
*                   (2)                            \net_cfg.h
*                   (3)                            \net_bsp.*
*                   (4)                            \net_isr*.*
*
*               (b) \<Network Protocol Suite>\Source\net.h
*                                                   \net_*.*
*
*               (c) \<Network Protocol Suite>\Ports\<cpu>\<compiler>\net_*_a.*
*
*               (d) \<Network Protocol Suite>\OS\<os>\net_os.*
*
*               (e) (1) \<Network Protocol Suite>\IF\net_if_*.*
*                   (2) \<Network Protocol Suite>\IF\<if>\net_if.*
*
*               (f) \<Network Protocol Suite>\NIC\<if>\<nic>\net_nic.*
*
*                       where
*                               <Your Product Application>      directory path for Your Product's Application
*                               <Network Protocol Suite>        directory path for network protocol suite
*                               <cpu>                           directory name for specific processor              (CPU)
*                               <compiler>                      directory name for specific compiler
*                               <os>                            directory name for specific operating system       (OS)
*                               <if>                            directory name for specific network interface      (IF)
*                               <nic>                           directory name for specific network interface card (NIC)
*
*           (2) CPU-configuration software files are located in the following directories :
*
*               (a) \<CPU-Compiler Directory>\cpu_def.h
*
*               (b) \<CPU-Compiler Directory>\<cpu>\<compiler>\cpu*.*
*
*                       where
*                               <CPU-Compiler Directory>        directory path for common   CPU-compiler software
*                               <cpu>                           directory name for specific processor (CPU)
*                               <compiler>                      directory name for specific compiler
*
*           (3) NO compiler-supplied standard library functions are used by the network protocol suite.
*
*               (a) Standard library functions are implemented in the custom library module(s) :
*
*                       \<Custom Library Directory>\lib*.*
*
*                           where
*                                   <Custom Library Directory>      directory path for custom library software
*
*               (b) Network-specific library functions are implemented in the Network Utility module,
*                   'net_util.*' (see 'net_util.h  Note #1').
*
*           (4) Compiler MUST be configured to include as additional include path directories :
*
*               (a) '\<Your Product Application>\' directory                                See Note #1a
*
*               (b) '\<Network Protocol Suite>\'   directory                                See Note #1b
*
*               (c) '\<Custom Library Directory>\' directory                                See Note #3a
*
*               (d) Specific port directories :
*
*                   (1) (A) '\<CPU-Compiler Directory>\'                    directory       See Note #2
*                       (B) '\<CPU-Compiler Directory>\<cpu>\<compiler>'    directory
*
*                   (2) '\<Network Protocol Suite>\Ports\<cpu>\<compiler>\' directory       See Note #1c
*
*                   (3) '\<Network Protocol Suite>\OS\<os>\'                directory       See Note #1d
*
*                   (4) (A) '\<Network Protocol Suite>\IF\'                 directory       See Note #1e
*                       (B) '\<Network Protocol Suite>\IF\<if>\'            directory
*
*                   (5) '\<Network Protocol Suite>\NIC\<if>\<nic>\'         directory       See Note #1f
*
*                       where
*                               <Your Product Application>      directory path for Your Product's Application
*                               <Network Protocol Suite>        directory path for network protocol suite
*                               <Custom Library Directory>      directory path for custom  library software
*                               <CPU-Compiler Directory>        directory path for common  CPU-compiler software
*                               <cpu>                           directory name for specific processor        (CPU)
*                               <compiler>                      directory name for specific compiler
*                               <os>                            directory name for specific operating system (OS)
*                               <if>                            directory name for specific network interface      (IF)
*                               <nic>                           directory name for specific network interface card (NIC)
*
*           (5) An application MUST pre-processor include ONLY this main network protocol suite
*               header file, 'net.h'.  All other network protocol suite files are included via
*               this main network header file.
*
*               See also 'MODULE  Note #1'.
*********************************************************************************************************
*/
/*$PAGE*/
#include  <cpu.h>                                       /* CPU Configuration       (see Note #2)                        */

#include  <lib_def.h>                                   /* Standard        Defines (see Note #3a)                       */
#include  <lib_mem.h>                                   /* Standard Memory Library (see Note #3a)                       */



#include  <net_def.h>                                   /* Network      Defines                                         */
#include  <net_nic_def.h>                               /* Network Card Defines                                         */
#include  <net_type.h>                                  /* Network Data Types                                           */

#include  <app_cfg.h>                                   /* Application  Configuration File (see Note #1a1)              */
#include  <net_cfg.h>                                   /* Network      Configuration File (see Note #1a2)              */
#include  <net_cfg_net.h>                               /* Network      Configuration (based on 'net_cfg.h' settings)   */



#include  <net_err.h>                                   /* Network Error Code Management                                */
#include  <net_ctr.h>                                   /* Network Counter    Management                                */

#include  <net_stat.h>                                  /* Network Statistics Management                                */
#include  <net_tmr.h>                                   /* Network Timer      Management                                */
#include  <net_buf.h>                                   /* Network Buffer     Management                                */
#include  <net_conn.h>                                  /* Network Connection Management                                */
#include  <net_dbg.h>                                   /* Network Debug      Management                                */

#include  <net_ascii.h>                                 /* Network ASCII   Library                                      */
#include  <net_util.h>                                  /* Network Utility Library (see Note #3b)                       */



#include  <net_bsp.h>                                   /* Network BSP Interface   (see Note #1a4)                      */
#include  <net_os.h>                                    /* Network-OS  Interface   (see Note #1d)                       */


#include  <net_if.h>                                    /* Network-Link Interface  (see Note #1e)                       */
#include  <net_nic.h>                                   /* Network Card Interface  (see Note #1f)                       */


#include  <net_ip.h>                                    /* Network IP     Layer                                         */
#include  <net_icmp.h>                                  /* Network ICMP   Layer                                         */

#include  <net_udp.h>                                   /* Network UDP    Layer                                         */
#include  <net_tcp.h>                                   /* Network TCP    Layer                                         */


#include  <net_bsd.h>                                   /* Network BSD    Layer                                         */
#include  <net_sock.h>                                  /* Network Socket Layer                                         */


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  NET_TASK_NBR                                      2


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

NET_EXT  CPU_BOOLEAN        Net_InitDone;               /* Indicates when network initialization is complete.           */

                                                        /* ------------------------- NET CTRS ------------------------- */
#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
NET_EXT  NET_CTR            Net_RxPktCtr;               /* Indicates number of queued received packets NOT yet handled. */
NET_EXT  NET_CTR            Net_TxSuspendCtr;           /* Indicates number of network transmits currently suspended.   */
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    NETWORK RECEIVE PACKET MACRO'S
*
* Description : Monitor number of network receive packet(s) queued &/or available.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_RxHandlerLoadBal(),
*               NetOS_IF_RxTaskSignal().
*
*               These macro's are network protocol suite to network interface (IF) macro's & SHOULD be 
*               called only by appropriate network interface function(s).
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
*                   See also 'net.c      Net_RxPktIsAvail()     Note #1',
*                            'net.c      Net_TxSuspend()        Note #1',
*                            'net.c      Net_TxSuspendSignal()  Note #1',
*                          & 'net_tcp.c  NetTCP_TxConnTxQ()     Note #10'.
*
*               (2) Network receive packet functionality implemented with inline macro's to :
*
*                   (a) Reduce network receive packet handling overhead
*                   (b) Reduce network receive packet code size when disabled
*
*               (3) (a) 'Net_RxPktCtr' MUST ALWAYS be accessed exclusively in critical sections.
*
*                   (b) ALL functions which call network receive packet macro's MUST declare local
*                       variable 'cpu_sr' if CPU critical section method is configured as
*                       CPU_CRITICAL_METHOD_STATUS_LOCAL :
*
*                           #if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
*                                CPU_SR  cpu_sr;
*                           #endif
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)

#define  NET_RX_PKT_INC()                               { CPU_CRITICAL_ENTER();             \
                                                          if (Net_RxPktCtr < NET_CTR_MAX) { \
                                                              Net_RxPktCtr++;               \
                                                          }                                 \
                                                          CPU_CRITICAL_EXIT();              }

#define  NET_RX_PKT_DEC()                               { CPU_CRITICAL_ENTER();             \
                                                          if (Net_RxPktCtr > 0) {           \
                                                              Net_RxPktCtr--;               \
                                                          }                                 \
                                                          CPU_CRITICAL_EXIT();              }

#else

#define  NET_RX_PKT_INC()
#define  NET_RX_PKT_DEC()

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

NET_ERR      Net_Init           (void);                     /* Network startup function.                                */

void         Net_InitDflt       (void);                     /* Initialize default values for configurable parameters.   */


CPU_INT16U   Net_VersionGet     (void);                     /* Get network protocol suite software version.             */


#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
                                                            /* Check if any network receive packet(s) are available.    */
CPU_BOOLEAN  Net_RxPktIsAvail   (NET_CTR  rx_chk_nbr);


void         Net_TxSuspend      (void);                     /* Suspend network transmit.                                */

void         Net_TxSuspendSignal(void);                     /* Signal  network transmit suspend.                        */
#endif


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                      DEFINED IN OS'S  net_os.c
*********************************************************************************************************
*/

void         NetOS_Init                  (NET_ERR     *perr);       /* Create network objects.                          */


void         NetOS_InitWait              (NET_ERR     *perr);       /* Wait  until network initialization is complete.  */

void         NetOS_InitSignal            (NET_ERR     *perr);       /* Signal that network initialization is complete.  */


void         NetOS_Lock                  (NET_ERR     *perr);       /* Acquire access to network protocol suite.        */

void         NetOS_Unlock                (void);                    /* Release access to network protocol suite.        */


#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
void         NetOS_TxSuspendWait         (void);                    /* Wait until network transmit suspend is signaled. */

void         NetOS_TxSuspendSignal       (void);                    /* Signal     network transmit suspend.             */


void         NetOS_TxSuspendTimeoutSet   (CPU_INT32U   timeout_ms,  /* Set        network transmit suspend timeout.     */
                                          NET_ERR     *perr);
                                                               
CPU_INT32U   NetOS_TxSuspendTimeoutGet_ms(NET_ERR     *perr);       /* Get        network transmit suspend timeout.     */
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_CFG_INIT_CFG_VALS
#error  "NET_CFG_INIT_CFG_VALS                not #define'd in 'net_cfg.h'   "
#error  "                               [MUST be  NET_INIT_CFG_VALS_DFLT    ]"
#error  "                               [     ||  NET_INIT_CFG_VALS_APP_INIT]"

#elif  ((NET_CFG_INIT_CFG_VALS != NET_INIT_CFG_VALS_DFLT    ) && \
        (NET_CFG_INIT_CFG_VALS != NET_INIT_CFG_VALS_APP_INIT))
#error  "NET_CFG_INIT_CFG_VALS          illegally #define'd in 'net_cfg.h'   "
#error  "                               [MUST be  NET_INIT_CFG_VALS_DFLT    ]"
#error  "                               [     ||  NET_INIT_CFG_VALS_APP_INIT]"
#endif




#ifndef  NET_CFG_OPTIMIZE
#error  "NET_CFG_OPTIMIZE                     not #define'd in 'net_cfg.h'"
#error  "                               [MUST be  NET_OPTIMIZE_SPD ]      "
#error  "                               [     ||  NET_OPTIMIZE_SIZE]      "

#elif  ((NET_CFG_OPTIMIZE != NET_OPTIMIZE_SPD ) && \
        (NET_CFG_OPTIMIZE != NET_OPTIMIZE_SIZE))
#error  "NET_CFG_OPTIMIZE               illegally #define'd in 'net_cfg.h'"
#error  "                               [MUST be  NET_OPTIMIZE_SPD ]      "
#error  "                               [     ||  NET_OPTIMIZE_SIZE]      "
#endif




#if     (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)

#ifndef  NET_CFG_TX_SUSPEND_TIMEOUT_MS
#error  "NET_CFG_TX_SUSPEND_TIMEOUT_MS        not #define'd in 'net_cfg.h'         "
#error  "                               [MUST be  >= NET_TX_SUSPEND_TIMEOUT_MIN_MS]"
#error  "                               [     &&  <= NET_TX_SUSPEND_TIMEOUT_MAX_MS]"

#elif  ((NET_CFG_TX_SUSPEND_TIMEOUT_MS < NET_TX_SUSPEND_TIMEOUT_MIN_MS) || \
        (NET_CFG_TX_SUSPEND_TIMEOUT_MS > NET_TX_SUSPEND_TIMEOUT_MAX_MS))
#error  "NET_CFG_TX_SUSPEND_TIMEOUT_MS  illegally #define'd in 'net_cfg.h'         "
#error  "                               [MUST be  >= NET_TX_SUSPEND_TIMEOUT_MIN_MS]"
#error  "                               [     &&  <= NET_TX_SUSPEND_TIMEOUT_MAX_MS]"
#endif

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of net module include.                           */

