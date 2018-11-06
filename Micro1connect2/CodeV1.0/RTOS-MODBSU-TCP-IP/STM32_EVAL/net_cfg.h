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
*                                     NETWORK CONFIGURATION FILE
*
*                                              TEMPLATE
*
* Filename      : net_cfg.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NETWORK CONFIGURATION
*
* Note(s) : (1) (a) Configure NET_CFG_INIT_CFG_VALS with the desired configuration for the initialization
*                   of network protocol suite configurable parameters :
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
*                               to initialize some of the network protocol suite configurable parameters
*
*                       (B) Call Net_Init()
*
*               See also 'net.c  Net_InitDflt()  Notes #1 & #2'
*                      & 'net_def.h  PARAMETER CONFIGURATION INITIALIZATION  Note #1'.
*
*
*           (2) Configure NET_CFG_OPTIMIZE with the desired network performance optimization :
*
*                   NET_OPTIMIZE_SPD            Optimizes network protocol suite for speed performance
*                   NET_OPTIMIZE_SIZE           Optimizes network protocol suite for binary image size
*********************************************************************************************************
*/
                                                                /* Configure network protocol suite's configurable ...  */
                                                                /* ... parameters' initial values (see Note #1) :       */
#define  NET_CFG_INIT_CFG_VALS                  NET_INIT_CFG_VALS_DFLT
                                                                /*   NET_INIT_CFG_VALS_DFLT         Default     values  */
                                                                /*   NET_INIT_CFG_VALS_APP_INIT     Application values  */


                                                                /* Configure network protocol suite's performance ...   */
                                                                /* ... optimization (see Note #2) :                     */
#define  NET_CFG_OPTIMIZE                       NET_OPTIMIZE_SPD
                                                                /*   NET_OPTIMIZE_SPD               Optimize for Speed  */
                                                                /*   NET_OPTIMIZE_SIZE              Optimize for Size   */


                                                                /* Configure network transmit suspend timeout in ...    */
#define  NET_CFG_TX_SUSPEND_TIMEOUT_MS                     1    /* ... integer milliseconds.                            */


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NETWORK DEBUG CONFIGURATION
*
* Note(s) : (1) Configure NET_DBG_CFG_INFO_EN to enable/disable network protocol suite debug information :
*
*               (a) Internal constants                         assigned to global variables
*               (b) Internal variable  data sizes calculated & assigned to global variables
*
*           (2) Configure NET_DBG_CFG_STATUS_EN to enable/disable network protocol suite debug status
*               functions.
*
*           (3) Configure NET_DBG_CFG_MEM_CLR_EN to enable/disable the network protocol suite from clearing
*               internal data structure memory buffers; a convenient feature while debugging.
*
*           (4) Configure NET_DBG_CFG_TEST_EN to enable/disable the network protocol suite test features :
*
*               (a) Internal data structure test &/or control flags
*               (b) Internal data structure read functionality
*********************************************************************************************************
*/

                                                                /* Configure debug information feature (see Note #1) :  */
#define  NET_DBG_CFG_INFO_EN                    DEF_DISABLED
                                                                /*   DEF_DISABLED  Debug information DISABLED           */
                                                                /*   DEF_ENABLED   Debug information ENABLED            */

                                                                /* Configure debug status functions (see Note #2) :     */
#define  NET_DBG_CFG_STATUS_EN                  DEF_DISABLED
                                                                /*   DEF_DISABLED  Debug status functions DISABLED      */
                                                                /*   DEF_ENABLED   Debug status functions ENABLED       */

                                                                /* Configure memory clear feature  (see Note #3) :      */
#define  NET_DBG_CFG_MEM_CLR_EN                 DEF_DISABLED
                                                                /*   DEF_DISABLED  Data structure clears DISABLED       */
                                                                /*   DEF_ENABLED   Data structure clears ENABLED        */

                                                                /* Configure test/debug feature(s) (see Note #4) :      */
#define  NET_DBG_CFG_TEST_EN                    DEF_DISABLED
                                                                /*   DEF_DISABLED  Test/Debug features DISABLED         */
                                                                /*   DEF_ENABLED   Test/Debug features ENABLED          */


/*
*********************************************************************************************************
*                                NETWORK ARGUMENT CHECK CONFIGURATION
*
* Note(s) : (1) Configure NET_ERR_CFG_ARG_CHK_EXT_EN to enable/disable the network protocol suite external
*               argument check feature :
*
*               (a) When ENABLED,  ALL arguments received from any port interface provided by the developer
*                   or application are checked/validated.
*
*               (b) When DISABLED, NO  arguments received from any port interface provided by the developer
*                   or application are checked/validated.
*
*           (2) Configure NET_ERR_CFG_ARG_CHK_DBG_EN to enable/disable the network protocol suite internal,
*               debug argument check feature :
*
*               (a) When ENABLED,     internal arguments are checked/validated to debug the network protocol
*                   suite.
*
*               (b) When DISABLED, NO internal arguments are checked/validated to debug the network protocol
*                   suite.
*********************************************************************************************************
*/
                                                                /* Configure external argument check feature ...        */
                                                                /* ... (see Note #1) :                                  */
#define  NET_ERR_CFG_ARG_CHK_EXT_EN             DEF_ENABLED
                                                                /*   DEF_DISABLED     Argument check DISABLED           */
                                                                /*   DEF_ENABLED      Argument check ENABLED            */

                                                                /* Configure internal argument check feature ...        */
                                                                /* ... (see Note #2) :                                  */
#define  NET_ERR_CFG_ARG_CHK_DBG_EN             DEF_DISABLED
                                                                /*   DEF_DISABLED     Argument check DISABLED           */
                                                                /*   DEF_ENABLED      Argument check ENABLED            */


/*$PAGE*/
/*
*********************************************************************************************************
*                               NETWORK COUNTER MANAGEMENT CONFIGURATION
*
* Note(s) : (1) Configure NET_CTR_CFG_STAT_EN to enable/disable network protocol suite statistics counters.
*
*           (2) Configure NET_CTR_CFG_ERR_EN  to enable/disable network protocol suite error      counters.
*********************************************************************************************************
*/

                                                                /* Configure statistics counter feature (see Note #1) : */
#define  NET_CTR_CFG_STAT_EN                    DEF_ENABLED
                                                                /*   DEF_DISABLED     Stat  counters DISABLED           */
                                                                /*   DEF_ENABLED      Stat  counters ENABLED            */

                                                                /* Configure error      counter feature (see Note #2) : */
#define  NET_CTR_CFG_ERR_EN                     DEF_ENABLED
                                                                /*   DEF_DISABLED     Error counters DISABLED           */
                                                                /*   DEF_ENABLED      Error counters ENABLED            */


/*
*********************************************************************************************************
*                             NETWORK STATISTIC MANAGEMENT CONFIGURATION
*********************************************************************************************************
*/



/*$PAGE*/
/*
*********************************************************************************************************
*                               NETWORK TIMER MANAGEMENT CONFIGURATION
*
* Note(s) : (1) Configure NET_TMR_CFG_NBR_TMR with the desired number of network TIMER objects.
*
*               Timers are required for :
*
*               (a) ARP  cache entries
*               (b) IP   fragment reassembly
*               (c) ICMP low-network-resources monitor task
*               (d) TCP  state machine connections
*
*           (2) Configure NET_TMR_CFG_TASK_FREQ to schedule the execution frequency of the network timer
*               task -- how often NetTmr_TaskHandler() is scheduled to run per second as implemented in
*               NetOS_Tmr_Task().
*
*               (a) NET_TMR_CFG_TASK_FREQ  MUST NOT be configured as a floating-point frequency.
*
*               See also 'net_tmr.h  NETWORK TIMER TASK TIME DEFINES  Notes #1 & #2'
*                      & 'net_os.c   NetOS_Tmr_Task()  Notes #1 & #2'.
*********************************************************************************************************
*/

#define  NET_TMR_CFG_NBR_TMR                              30    /* Configure total number of TIMERs (see Note #1).      */

#define  NET_TMR_CFG_TASK_FREQ                            10    /* Configure Timer Task frequency   (see Note #2).      */


/*
*********************************************************************************************************
*                               NETWORK BUFFER MANAGEMENT CONFIGURATION
*
* Note(s) : (1) (a) Configure NET_BUF_CFG_NBR_SMALL with the desired number of SMALL network BUFFER objects.
*
*               (b) Configure NET_BUF_CFG_NBR_LARGE with the desired number of SMALL network BUFFER objects.
*
*               (c) Buffers are required to :
*
*                   (1) Receive  packets from the network
*                   (2) Transmit packets to   the network
*
*           (2) (a) Configure NET_BUF_CFG_DATA_SIZE_SMALL with the desired data buffer size for SMALL network BUFFERs.
*
*               (b) Configure NET_BUF_CFG_DATA_SIZE_LARGE with the desired data buffer size for LARGE network BUFFERs.
*
*               (c) NET_BUF_CFG_DATA_SIZE_SMALL & NET_BUF_CFG_DATA_SIZE_LARGE  MUST be declared with values ...
*
*                   (1) that are greater than or equal to NET_BUF_DATA_SIZE_MIN (see 'net_buf.h  NETWORK BUFFER 
*                       INDEX & SIZE DEFINES')
*
*                   (2) such that NET_BUF_CFG_DATA_SIZE_LARGE has a value greater than NET_BUF_CFG_DATA_SIZE_SMALL
*
*                   ... and SHOULD be declared with values
*
*                   (3) that are integer multiples of 'CPU_ALIGN' word size (to force word-alignment of buffer arrays)
*
*               See also 'net_buf.h  NETWORK BUFFER DATA TYPE  Note #3'.
*********************************************************************************************************
*/
                                                                /* Configure total number of       BUFFERs ...          */
                                                                /* ... (see Note #1) :                                  */
#define  NET_BUF_CFG_NBR_SMALL                            5     /* Configure total number of SMALL BUFFERs.             */
#define  NET_BUF_CFG_NBR_LARGE                            5     /* Configure total number of LARGE BUFFERs.             */

                                                                /* Configure size of       BUFFER DATA areas ...        */
                                                                /* ... (see Note #2) :                                  */
#define  NET_BUF_CFG_DATA_SIZE_SMALL                     256    /* Configure size of SMALL BUFFER DATA area.            */
#define  NET_BUF_CFG_DATA_SIZE_LARGE                    1596    /* Configure size of LARGE BUFFER DATA area.            */


/*$PAGE*/
/*
*********************************************************************************************************
*                                NETWORK INTERFACE CARD CONFIGURATION
*
* Note(s) : (1) Configure NIC-specific parameters.
*
*               Consult the specific NIC's documentation to determine the NIC's required configuration
*               parameters; see also the specific NIC's source documentation in the following network
*               directory :
*
*                   \<Network Protocol Suite>\NIC\<if>\<nic>\
*
*                       where
*                               <Network Protocol Suite>        directory path for network protocol suite
*                               <nic>                           directory name for specific network interface card (NIC)
*
*           (2) Configure NET_NIC_CFG_INT_CTRL_EN to enable/disable any software to handle the presence
*               of a NIC interrupt controller.
*
*           (3) Configure NET_NIC_CFG_RD_WR_SEL   to select whether to implement the NIC's read/write
*               functionality with functions or macro's.
*
*               (a) When NET_NIC_RD_WR_SEL_FNCT,  NIC read/write functionality SHOULD be implemented
*                   as functions in the application-specific 'net_bsp.c' (or some other application-
*                   specific file) & MUST be prototyped in the NIC's 'net_nic.h'.
*
*               (b) When NET_NIC_RD_WR_SEL_MACRO, NIC read/write functionality SHOULD be implemented
*                   as macro's   in the application-specific 'net_bsp.h'.
*
*           (4) (a) Configure NET_NIC_CFG_TX_RDY_INIT_VAL to initialize the NIC-transmit-ready OS object with
*                   one of the following values :
*
*                   (1) 0, for NIC driver's that implement the NIC's transmit-ready status using a
*                              Transmit Empty    interrupt.
*
*                   (2) 1, for NIC driver's that implement the NIC's transmit-ready status using a
*                              Transmit Complete interrupt.
*
*               (b) This configuration currently applicable for single-transmit-packet-buffer NIC drivers ONLY.
*
*               See also 'net_os.c  NetOS_NIC_Init()  Note #1a'.
*
*           (5) Configure NET_NIC_CFG_TX_PKT_PREPARE_EN to enable/disable the early preparation of NIC
*               transmit packet(s) by the network interface :
*
*               (a) When ENABLED,  NIC transmit packet(s) are     prepared BEFORE waiting on the NIC's 
*                   transmit ready signal.  This is useful for NICs that can simultaneously prepare
*                   transmit packet(s) while also transmitting previously-prepared packet(s).
*
*               (a) When DISABLED, NIC transmit packet(s) are NOT prepared BEFORE waiting on the NIC's 
*                   transmit ready signal.
*********************************************************************************************************
*/
                                                                /* Configure presence of interrupt controller ...       */
                                                                /* ... (see Note #2) :                                  */
#define  NET_NIC_CFG_INT_CTRL_EN                DEF_ENABLED
                                                                /*   DEF_DISABLED  Interrupt controller NOT present     */
                                                                /*   DEF_ENABLED   Interrupt controller     present     */

                                                                /* Configure NIC Read/Write Selection (see Note #3) :   */
#define  NET_NIC_CFG_RD_WR_SEL                  NET_NIC_RD_WR_SEL_FNCT
                                                                /*   NET_NIC_RD_WR_SEL_FNCT   NIC Read/Write ...        */
                                                                /*   ... functionality implemented with functions       */
                                                                /*   NET_NIC_RD_WR_SEL_MACRO  NIC Read/Write ...        */
                                                                /*   ... functionality implemented with macro's         */

#define  NET_NIC_CFG_TX_RDY_INIT_VAL                       1    /* Configure transmit ready initial value (see Note #4).*/

                                                                /* Configure transmit preparation feature (see Note #5):*/
#define  NET_NIC_CFG_TX_PKT_PREPARE_EN          DEF_DISABLED
                                                                /*   DEF_DISABLED   Transmit packets NOT pre-prepared   */
                                                                /*   DEF_ENABLED    Transmit packets     pre-prepared   */


/*$PAGE*/
/*
*********************************************************************************************************
*                                NETWORK INTERFACE LAYER CONFIGURATION
*
* Note(s) : (1) See 'net_def.h  NETWORK INTERFACE LAYER DEFINES' for supported network interface types.
*********************************************************************************************************
*/
                                                                /* Configure interface type (see Note #1).              */
#define  NET_IF_CFG_TYPE                        NET_IF_TYPE_ETHER

                                                                /* Configure network interface address filter feature : */
#define  NET_IF_CFG_ADDR_FLTR_EN                DEF_ENABLED
                                                                /*   DEF_DISABLED        Addresses NOT filtered         */
                                                                /*   DEF_ENABLED         Addresses     filtered         */


/*
*********************************************************************************************************
*                           ADDRESS RESOLUTION PROTOCOL LAYER CONFIGURATION
*
* Note(s) : (1) Address Resolution Protocol ONLY required for some network interfaces (see 'net_arp.h
*               Note #1').
*
*           (2) See 'net_arp.h  ARP HARDWARE & PROTOCOL DEFINES  Note #1' for supported ARP hardware &
*               protocol types.
*
*           (3) Configure NET_ARP_CFG_ADDR_FLTR_EN to enable/disable the ARP address filtering feature
*               to selectively filter & discard ALL misdirected or incorrectly received ARP messages.
*********************************************************************************************************
*/
                                                                /* Configure ARP types (see Note #2) :                  */
#define  NET_ARP_CFG_HW_TYPE                    NET_ARP_HW_TYPE_ETHER
#define  NET_ARP_CFG_PROTOCOL_TYPE              NET_ARP_PROTOCOL_TYPE_IP_V4

#define  NET_ARP_CFG_NBR_CACHE                            10    /* Configure ARP cache size.                            */

                                                                /* Configure ARP address filter feature (see Note #3) : */
#define  NET_ARP_CFG_ADDR_FLTR_EN               DEF_ENABLED
                                                                /*   DEF_DISABLED    ARP addresses NOT filtered         */
                                                                /*   DEF_ENABLED     ARP addresses     filtered         */

/*$PAGE*/
/*
*********************************************************************************************************
*                                INTERNET PROTOCOL LAYER CONFIGURATION
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                        INTERNET CONTROL MESSAGE PROTOCOL LAYER CONFIGURATION
*********************************************************************************************************
*/
                                                                /* Configure ICMP Transmit Source Quench feature :      */
#define  NET_ICMP_CFG_TX_SRC_QUENCH_EN          DEF_DISABLED
                                                                /*   DEF_DISABLED  ICMP Transmit Source Quench DISABLED */
                                                                /*   DEF_ENABLED   ICMP Transmit Source Quench ENABLED  */

#define  NET_ICMP_CFG_TX_SRC_QUENCH_SIZE                  10    /* Configure ICMP transmit source quench list size.     */


/*$PAGE*/
/*
*********************************************************************************************************
*                                    TRANSPORT LAYER CONFIGURATION
*********************************************************************************************************
*/
                                                                /* Configure Transport Layer Selection :                */
#define  NET_CFG_TRANSPORT_LAYER_SEL            NET_TRANSPORT_LAYER_SEL_UDP_TCP
                                                                /*   NET_TRANSPORT_LAYER_SEL_UDP       UDP     ONLY     */
                                                                /*   NET_TRANSPORT_LAYER_SEL_UDP_TCP   UDP/TCP          */


/*
*********************************************************************************************************
*                             USER DATAGRAM PROTOCOL LAYER CONFIGURATION
*
* Note(s) : (1) Configure NET_UDP_CFG_APP_API_SEL with the desired configuration for demultiplexing
*               UDP datagrams to application connections :
*
*                   NET_UDP_APP_API_SEL_SOCK        Demultiplex UDP datagrams to BSD sockets ONLY.
*                   NET_UDP_APP_API_SEL_APP         Demultiplex UDP datagrams to application-specific
*                                                       connections ONLY.
*                   NET_UDP_APP_API_SEL_SOCK_APP    Demultiplex UDP datagrams to BSD sockets first;
*                                                       if NO socket connection found to demultiplex
*                                                       a UDP datagram, demultiplex to application-
*                                                       specific connection.
*
*               See also 'net_udp.c  NetUDP_RxPktDemuxDatagram()  Note #1'
*                      & 'net_udp.c  NetUDP_RxPktDemuxAppData()   Note #1'.
*
*           (2) (a) RFC #1122, Section 4.1.3.4 states that "an application MAY optionally ... discard 
*                   ... [or allow] ... received ... UDP datagrams without checksums".
*
*               (b) Configure NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN to enable/disable discarding of UDP
*                   datagrams received with NO computed check-sum :
*
*                   (1) When ENABLED,  ALL UDP datagrams received without a check-sum are discarded.
*
*                   (2) When DISABLED, ALL UDP datagrams received without a check-sum are flagged so
*                       that application(s) may handle &/or discard.
*
*               See also 'net_udp.c  NetUDP_RxPktValidate()  Note #4d3A'.
*
*           (3) (a) RFC #1122, Section 4.1.3.4 states that "an application MAY optionally be able to 
*                   control whether a UDP checksum will be generated".
*
*               (b) Configure NET_UDP_CFG_TX_CHK_SUM_EN to enable/disable transmitting UDP datagrams 
*                   with check-sums :
*
*                   (1) When ENABLED,  ALL UDP datagrams are transmitted with    a computed check-sum.
*
*                   (2) When DISABLED, ALL UDP datagrams are transmitted without a computed check-sum.
*
*               See also 'net_udp.c  NetUDP_TxPktPrepareHdr()  Note #3b'.
*********************************************************************************************************
*/
                                                                /* Configure UDP Receive Demultiplex Selection ...      */
                                                                /* ... (see Note #1) :                                  */
#define  NET_UDP_CFG_APP_API_SEL                NET_UDP_APP_API_SEL_SOCK
                                                                /*                       Demultiplex & Receive via ...  */
                                                                /*   NET_UDP_APP_API_SEL_SOCK      Sockets     ONLY     */
                                                                /*   NET_UDP_APP_API_SEL_APP       Application ONLY     */
                                                                /*   NET_UDP_APP_API_SEL_SOCK_APP  First by Socket,     */
                                                                /*                                 Next  by Application */

                                                                /* Configure UDP Receive Check-Sum Discard feature ...  */
                                                                /* ... (see Note #2b)                                   */
#define  NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN      DEF_DISABLED
                                                                /*   DEF_DISABLED  UDP Datagrams  Received without ...  */
                                                                /*                     Check-Sums Validated             */
                                                                /*   DEF_ENABLED   UDP Datagrams  Received without ...  */
                                                                /*                     Check-Sums Discarded             */

                                                                /* Configure UDP Transmit Check-Sum feature ...         */
                                                                /* ... (see Note #3b) :                                 */
#define  NET_UDP_CFG_TX_CHK_SUM_EN              DEF_ENABLED
                                                                /*   DEF_DISABLED  Transmit Check-Sums  DISABLED        */
                                                                /*   DEF_ENABLED   Transmit Check-Sums   ENABLED        */


/*$PAGE*/
/*
*********************************************************************************************************
*                          TRANSMISSION CONTROL PROTOCOL LAYER CONFIGURATION
*
* Note(s) : (1) (a) Configure NET_TCP_CFG_NBR_CONN with the desired number of TCP connection objects.
*
*               (b) Since TCP requires network sockets (see 'net_tcp.h  Note #2'), the configured number
*                   of sockets MUST be greater than or equal to the configured number of TCP connections.
*
*               See also 'net_sock.h  MODULE  Note #1',
*                        'net_tcp.h   MODULE  Note #1',
*                      & 'net_cfg_net.h  NETWORK SOCKET LAYER CONFIGURATION  Note #4'.
*
*           (2) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 20.4, Page 282 states that
*               "4.2BSD defaulted the send and receive buffer" (i.e. socket buffer/TCP window) "to 2048
*               bytes each.  With 4.3BSD both were increased to 4096 bytes ... Other systems, such as
*               ... 4.4BSD ... use larger default buffer sizes, such as 8192 or 16384 bytes.
*
*               The common default of 4096 bytes ... is not optimal for Ethernet.  An approximate 40%
*               increase in throughput is seen by just increasing both buffers to 16384 bytes."
*
*           (3) RFC #793, Section 3.3 'Sequence Numbers : Knowing When to Keep Quiet' states that 
*               "the Maximum Segment Lifetime (MSL) is ... to be 2 minutes ... [but] may be changed
*               if experience indicates it is desirable to do so".
*
*               See also 'net_tcp.h  TCP CONNECTION TIMEOUT DEFINES  Note #1'.
*
*           (4) RFC #2581, Section 4.2 states that "an ACK ... MUST be generated within 500 ms of the
*               arrival of the first unacknowledged packet".
*               
*               See also 'net_tcp.h  TCP CONGESTION CONTROL DEFINES  Note #6b'
*                      & 'net_tcp.c  NetTCP_TxConnAck()  Note #6a2'.
*
*           (5) Configure timeout values in integer number of milliseconds.  Timeout values may also 
*               be configured with network time constant, NET_TMR_TIME_INFINITE, to never timeout.
*********************************************************************************************************
*/

#define  NET_TCP_CFG_NBR_CONN                             10    /* Configure total number of TCP connections.           */


                                                                /* Configure TCP connections' window sizes ...          */
                                                                /* ... in integer number of octets (see Note #2) :      */

#define  NET_TCP_CFG_RX_WIN_SIZE_OCTET                  4096    /* Configure TCP connection receive  window size.       */
#define  NET_TCP_CFG_TX_WIN_SIZE_OCTET                  4096    /* Configure TCP connection transmit window size.       */


                                                                /* Configure TCP connections' default maximum ...       */
                                                                /* ... segment lifetime timeout (MSL) value,  ...       */
#define  NET_TCP_CFG_TIMEOUT_CONN_MAX_SEG_SEC              3    /* ... in integer seconds (see Note #3).                */


                                                                /* Configure TCP acknowledgement delay ...              */
#define  NET_TCP_CFG_TIMEOUT_CONN_ACK_DLY_MS             500    /* ... in integer milliseconds (see Note #4).           */


                                                                /* Configure TCP timeouts (see Note #5) :               */
                                                                /* Configure TCP connection receive  queue timeout.     */
#define  NET_TCP_CFG_TIMEOUT_CONN_RX_Q_MS       NET_TMR_TIME_INFINITE
                                                                /* Configure TCP connection transmit queue timeout.     */
#define  NET_TCP_CFG_TIMEOUT_CONN_TX_Q_MS       NET_TMR_TIME_INFINITE


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NETWORK SOCKET LAYER CONFIGURATION
*
* Note(s) : (1) Network Socket Layer NOT required for all application interfaces (see 'net_sock.h
*               MODULE  Note #1').
*
*           (2) See 'net_sock.h  NETWORK SOCKET FAMILY & PROTOCOL DEFINES  Note #1' for supported 
*               Network Socket Family types.
*
*           (3) Configure socket select maximum number of socket events/operations to wait on.
*
*               See 'net_sock.c  NetSock_Sel()         Note #4b'
*                 & 'net_os.c    NetOS_Sock_SelWait()  Note #2b2'.
*
*           (4) Configure timeout values in integer number of milliseconds.  Timeout values may also 
*               be configured with network time constant, NET_TMR_TIME_INFINITE, to never timeout.
*********************************************************************************************************
*/
                                                                /* Configure socket family type (see Note #2) :         */
#define  NET_SOCK_CFG_FAMILY                    NET_SOCK_FAMILY_IP_V4

#define  NET_SOCK_CFG_NBR_SOCK                            10    /* Configure total number of sockets.                   */


                                                                /* Configure socket blocking/non-blocking behavior :    */
#define  NET_SOCK_CFG_BLOCK_SEL                 NET_SOCK_BLOCK_SEL_BLOCK
                                                                /*   NET_SOCK_BLOCK_SEL_DFLT       Default blocking;    */
                                                                /*                                 ... run-time options */
                                                                /*                                 ... can override     */
                                                                /*   NET_SOCK_BLOCK_SEL_BLOCK      Always  block        */
                                                                /*   NET_SOCK_BLOCK_SEL_NO_BLOCK   Never   block        */


                                                                /* Configure socket select functionality :              */
#define  NET_SOCK_CFG_SEL_EN                    DEF_ENABLED
                                                                /*   DEF_DISABLED  Socket select  DISABLED              */
                                                                /*   DEF_ENABLED   Socket select   ENABLED              */

                                                                /* Configure maximum number of socket select ...        */
#define  NET_SOCK_CFG_SEL_NBR_EVENTS_MAX                  10    /* ... operations/events to wait on (see Note #3).      */


                                                                /* Configure stream-type sockets' accept queue ...      */
#define  NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX               5    /* ... maximum size.                                    */

#define  NET_SOCK_CFG_PORT_NBR_RANDOM_BASE             65000    /* Configure random ports' starting port number.        */


                                                                /* Configure socket timeout values (see Note #4) :      */
                                                                /* Configure socket receive    queue   timeout.         */
#define  NET_SOCK_CFG_TIMEOUT_RX_Q_MS           NET_TMR_TIME_INFINITE
                                                                /* Configure socket connection request timeout.         */
#define  NET_SOCK_CFG_TIMEOUT_CONN_REQ_MS       NET_TMR_TIME_INFINITE
                                                                /* Configure socket connection accept  timeout.         */
#define  NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_MS    NET_TMR_TIME_INFINITE
                                                                /* Configure socket connection close   timeout.         */
#define  NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_MS            10000


/*
*********************************************************************************************************
*                                     BSD 4.x LAYER CONFIGURATION
*
* Note(s) : (1) BSD 4.x Layer API NOT required for all applications (see 'net_bsd.h  MODULE  Note #1b').
*********************************************************************************************************
*/
                                                                /* Configure BSD 4.x Layer API (see Note #1) :          */
#define  NET_BSD_CFG_API_EN                     DEF_ENABLED
                                                                /*   DEF_DISABLED  BSD 4.x Layer API DISABLED           */
                                                                /*   DEF_ENABLED   BSD 4.x Layer API ENABLED            */


/*$PAGE*/
/*
*********************************************************************************************************
*                             NETWORK CONNECTION MANAGEMENT CONFIGURATION
*
* Note(s) : (1) Network Connection Management NOT required for all application interfaces (see 'net_conn.h
*               MODULE  Note #1').
*
*           (2) See 'net_def.h  NETWORK CONNECTION MANAGEMENT DEFINES' for supported Family types.
*
*           (3) The configured number of connections MUST be greater than the configured/required/expected
*               number of application connections & transport layer connections.
*
*               See also 'net_cfg_net.h  NETWORK CONNECTION MANAGEMENT CONFIGURATION  Note #4'.
*********************************************************************************************************
*/
                                                                /* Configure connection family type      (see Note #2). */
#define  NET_CONN_CFG_FAMILY                    NET_CONN_FAMILY_IP_V4_SOCK

#define  NET_CONN_CFG_NBR_CONN                            20    /* Configure total number of connections (see Note #3). */

