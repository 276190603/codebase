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
*                                    NETWORK ERROR CODE MANAGEMENT
*
* Filename      : net_err.h
* Version       : V1.92
* Programmer(s) : ITJ
*                 SR
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/


#ifdef   NET_ERR_MODULE
#define  NET_ERR_EXT
#else
#define  NET_ERR_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         NETWORK ERROR CODES
*
* Note(s) : (1) All generic network error codes are #define'd in 'net_err.h';
*               Any port-specific   error codes are #define'd in port-specific header files.
*
*           (2) Network error codes are grouped in series of 1000 per module/layer.
*********************************************************************************************************
*/

#define  NET_ERR_NONE                                      0

#define  NET_ERR_INIT_INCOMPLETE                          10    /* Net init NOT completed.                              */
#define  NET_ERR_INVALID_PROTOCOL                         20    /* Invalid/unknown/unsupported net protocol.            */


#define  NET_ERR_RX                                      400    /* Rx err.                                              */
#define  NET_ERR_RX_DEST                                 450    /* Invalid rx dest.                                     */

#define  NET_ERR_TX                                      500    /* Tx err.                                              */


/*
*********************************************************************************************************
*                                    NETWORK-OS LAYER ERROR CODES
*********************************************************************************************************
*/

#define  NET_OS_ERR_NONE                                1000

#define  NET_OS_ERR_LOCK                                1010
#define  NET_OS_ERR_TASK                                1020

#define  NET_OS_ERR_INVALID_TIME                        1050    /* Invalid time/tick val.                               */

#define  NET_OS_ERR_TIME_DLY_FAULT                      1060    /* Time dly fault.                                      */
#define  NET_OS_ERR_TIME_DLY_MAX                        1061    /* Time dly max'd.                                      */


#define  NET_OS_ERR_INIT                                1100    /* Net  init   signal          NOT rx'd by net task(s). */

#define  NET_OS_ERR_INIT_SIGNAL                         1110    /* Net  init   signal          NOT successfully init'd. */
#define  NET_OS_ERR_INIT_SIGNAL_NAME                    1111    /* Net  init   signal  name    NOT successfully cfg'd.  */
#define  NET_OS_ERR_INIT_SIGNALD                        1112    /* Net  init                   NOT successfully signl'd.*/
#define  NET_OS_ERR_INIT_LOCK                           1120    /* Net  lock   signal          NOT successfully init'd. */
#define  NET_OS_ERR_INIT_LOCK_NAME                      1121    /* Net  lock   signal  name    NOT successfully cfg'd.  */
#define  NET_OS_ERR_INIT_TX_SUSPEND                     1130    /* Net  tx     suspend         NOT successfully init'd. */
#define  NET_OS_ERR_INIT_TX_SUSPEND_NAME                1131    /* Net  tx     suspend name    NOT successfully cfg'd.  */
#define  NET_OS_ERR_INIT_TX_SUSPEND_TIMEOUT             1132    /* Net  tx     suspend timeout NOT successfully cfg'd.  */


#define  NET_OS_ERR_INIT_TMR_TASK                       1200    /* Net  tmr    task            NOT successfully init'd. */
#define  NET_OS_ERR_INIT_TMR_TASK_NAME                  1201    /* Net  tmr    task    name    NOT successfully cfg'd.  */


#define  NET_OS_ERR_INIT_NIC_TX_RDY                     1300    /* NIC  tx rdy signal          NOT successfully init'd. */
#define  NET_OS_ERR_INIT_NIC_TX_RDY_NAME                1301    /* NIC  tx rdy signal  name    NOT successfully cfg'd.  */

#define  NET_OS_ERR_INIT_IF_RX_Q                        1350    /* IF   rx Q   signal          NOT successfully init'd. */
#define  NET_OS_ERR_INIT_IF_RX_Q_NAME                   1351    /* IF   rx Q   signal  name    NOT successfully cfg'd.  */
#define  NET_OS_ERR_INIT_IF_RX_TASK                     1352    /* IF   rx     task            NOT successfully init'd. */
#define  NET_OS_ERR_INIT_IF_RX_TASK_NAME                1353    /* IF   rx     task    name    NOT successfully cfg'd.  */


#define  NET_OS_ERR_INIT_TCP_RX_Q                       1500    /* TCP  rx Q's                 NOT successfully init'd. */
#define  NET_OS_ERR_INIT_TCP_RX_Q_TIMEOUT               1501    /* TCP  rx Q's         timeout NOT successfully init'd. */
#define  NET_OS_ERR_INIT_TCP_TX_Q                       1502    /* TCP  tx Q's                 NOT successfully init'd. */
#define  NET_OS_ERR_INIT_TCP_TX_Q_TIMEOUT               1503    /* TCP  tx Q's         timeout NOT successfully init'd. */


#define  NET_OS_ERR_INIT_SOCK_RX_Q                      1600    /* Sock rx Q's                 NOT successfully init'd. */
#define  NET_OS_ERR_INIT_SOCK_RX_Q_TIMEOUT              1601    /* Sock rx Q's         timeout NOT successfully init'd. */
#define  NET_OS_ERR_INIT_SOCK_CONN                      1602    /* Sock req    signal          NOT successfully init'd. */
#define  NET_OS_ERR_INIT_SOCK_CONN_TIMEOUT              1603    /* Sock req    signal  timeout NOT successfully init'd. */
#define  NET_OS_ERR_INIT_SOCK_ACCEPT                    1604    /* Sock accept signal          NOT successfully init'd. */
#define  NET_OS_ERR_INIT_SOCK_ACCEPT_TIMEOUT            1605    /* Sock accept signal  timeout NOT successfully init'd. */
#define  NET_OS_ERR_INIT_SOCK_CLOSE                     1606    /* Sock close  signal          NOT successfully init'd. */
#define  NET_OS_ERR_INIT_SOCK_CLOSE_TIMEOUT             1607    /* Sock close  signal  timeout NOT successfully init'd. */


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NETWORK UTILITY LIBRARY ERROR CODES
*********************************************************************************************************
*/

#define  NET_UTIL_ERR_NONE                              2000
#define  NET_UTIL_ERR_NULL_PTR                          2020    /* Ptr arg(s) passed NULL ptr(s).                       */
#define  NET_UTIL_ERR_NULL_SIZE                         2021
#define  NET_UTIL_ERR_INVALID_PROTOCOL                  2030    /* Invalid/unknown/unsupported net protocol.            */


/*
*********************************************************************************************************
*                                      ASCII LIBRARY ERROR CODES
*********************************************************************************************************
*/

#define  NET_ASCII_ERR_NONE                             3000
#define  NET_ASCII_ERR_NULL_PTR                         3020    /* Ptr arg(s) passed NULL ptr(s).                       */

#define  NET_ASCII_ERR_INVALID_LEN                      3030    /* Invalid ASCII str  len.                              */
#define  NET_ASCII_ERR_INVALID_CHAR                     3031    /* Invalid ASCII char.                                  */
#define  NET_ASCII_ERR_INVALID_CHAR_LEN                 3032    /* Invalid ASCII char len.                              */
#define  NET_ASCII_ERR_INVALID_CHAR_VAL                 3033    /* Invalid ASCII char val.                              */
#define  NET_ASCII_ERR_INVALID_CHAR_SEQ                 3034    /* Invalid ASCII char seq.                              */


/*
*********************************************************************************************************
*                               NETWORK STATISTIC MANAGEMENT ERROR CODES
*********************************************************************************************************
*/

#define  NET_STAT_ERR_NONE                              4000
#define  NET_STAT_ERR_NULL_PTR                          4020    /* Ptr arg(s) passed NULL ptr(s).                       */

#define  NET_STAT_ERR_POOL_INVALID_TYPE                 4030    /* Type specified invalid or unknown.                   */
#define  NET_STAT_ERR_POOL_NONE_AVAIL                   4031    /* NO stat pool entries avail.                          */
#define  NET_STAT_ERR_POOL_NONE_USED                    4032    /* NO stat pool entries used.                           */
#define  NET_STAT_ERR_POOL_NONE_REM                     4033    /* NO stat pool entries remain.                         */


/*
*********************************************************************************************************
*                                NETWORK TIMER MANAGEMENT ERROR CODES
*********************************************************************************************************
*/

#define  NET_TMR_ERR_NONE                               5000
#define  NET_TMR_ERR_NONE_AVAIL                         5010    /* NO net tmrs avail.                                   */
#define  NET_TMR_ERR_NULL_PTR                           5020    /* Ptr arg(s) passed NULL ptr(s).                       */
#define  NET_TMR_ERR_NULL_OBJ                           5021    /* Arg 'obj'  is a NULL ptr.                            */
#define  NET_TMR_ERR_NULL_FNCT                          5022    /* Arg 'fnct' is a NULL ptr.                            */
#define  NET_TMR_ERR_INVALID_TYPE                       5030    /* Type specified invalid or unknown.                   */


/*$PAGE*/
/*
*********************************************************************************************************
*                                NETWORK BUFFER MANAGEMENT ERROR CODES
*********************************************************************************************************
*/

#define  NET_BUF_ERR_NONE                               6000
#define  NET_BUF_ERR_NONE_AVAIL                         6010    /* NO net bufs of req'd size avail.                     */
#define  NET_BUF_ERR_NOT_USED                           6011    /* Net buf NOT used.                                    */
#define  NET_BUF_ERR_NULL_PTR                           6020    /* Ptr arg(s) passed NULL ptr(s).                       */

#define  NET_BUF_ERR_INVALID_TYPE                       6030    /* Type    specified invalid or unknown.                */
#define  NET_BUF_ERR_INVALID_SIZE                       6031    /* Size    specified invalid or 0.                      */
#define  NET_BUF_ERR_INVALID_IX                         6032    /* Ix      specified outside DATA area.                 */
#define  NET_BUF_ERR_INVALID_LEN                        6033    /* Len     specified outside DATA area.                 */


/*
*********************************************************************************************************
*                              NETWORK CONNECTION MANAGEMENT ERROR CODES
*********************************************************************************************************
*/

#define  NET_CONN_ERR_NONE                              7000
#define  NET_CONN_ERR_NONE_AVAIL                        7010    /* NO net conns avail.                                  */
#define  NET_CONN_ERR_NOT_USED                          7011    /* Net conn NOT used.                                   */
#define  NET_CONN_ERR_NULL_PTR                          7020    /* Ptr arg(s) passed NULL ptr(s).                       */

#define  NET_CONN_ERR_INVALID_TYPE                      7050    /* Invalid conn type.                                   */
#define  NET_CONN_ERR_INVALID_CONN                      7051    /* Invalid conn id.                                     */
#define  NET_CONN_ERR_INVALID_FAMILY                    7052    /* Invalid conn list family.                            */
#define  NET_CONN_ERR_INVALID_PROTOCOL_IX               7053    /* Invalid conn list protocol ix.                       */
#define  NET_CONN_ERR_INVALID_ADDR                      7054    /* Invalid conn addr.                                   */
#define  NET_CONN_ERR_INVALID_ADDR_LEN                  7055    /* Invalid conn addr len.                               */

#define  NET_CONN_ERR_ADDR_NOT_USED                     7100    /* Conn addr NOT    used.                               */
#define  NET_CONN_ERR_ADDR_IN_USE                       7101    /* Conn addr cur in use.                                */

#define  NET_CONN_ERR_CONN_NONE                         7110    /* NO   conn.                                           */
#define  NET_CONN_ERR_CONN_HALF                         7111    /* Half conn (local                   addr valid).      */
#define  NET_CONN_ERR_CONN_HALF_WILDCARD                7112    /* Half conn (local wildcard          addr valid).      */
#define  NET_CONN_ERR_CONN_FULL                         7113    /* Full conn (local          & remote addr valid).      */
#define  NET_CONN_ERR_CONN_FULL_WILDCARD                7114    /* Full conn (local wildcard & remote addr valid).      */


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NETWORK INTERFACE CARD ERROR CODES
*
* Note(s) : (1) Some NIC error codes #define'd in 'net_nic.h' since they are NIC-dependent.
*********************************************************************************************************
*/

#define  NET_NIC_ERR_NONE                              10000
#define  NET_NIC_ERR_NULL_PTR                          10020    /* Ptr arg(s) passed NULL ptr(s).                       */
#define  NET_NIC_ERR_NIC_OFF                           10030    /* NIC status is OFF (a.k.a. DOWN).                     */


#define  NET_NIC_ERR_INVALID_SIZE                      10100    /* Invalid size                                         */


#define  NET_NIC_ERR_TX_RDY_SIGNAL                     10500    /* Tx rdy signal failed.                                */
#define  NET_NIC_ERR_TX_RDY_SIGNAL_TIMEOUT             10501    /* Tx rdy signal timeout; NO signal rx'd from NIC.      */
#define  NET_NIC_ERR_TX_RDY_SIGNAL_FAULT               10502    /* Tx rdy signal fault.                                 */


/*
*********************************************************************************************************
*                                 NETWORK INTERFACE LAYER ERROR CODES
*
* Note(s) : (1) Some IF error codes #define'd in 'net_if.h' since they are IF-dependent.
*********************************************************************************************************
*/

#define  NET_IF_ERR_NONE                               11000
#define  NET_IF_ERR_NULL_PTR                           11020    /* Ptr arg(s) passed NULL ptr(s).                       */


#define  NET_IF_ERR_INVALID_MTU                        11200    /* Invalid MTU.                                         */
#define  NET_IF_ERR_INVALID_LEN_DATA                   11201    /* Invalid data  len.                                   */
#define  NET_IF_ERR_INVALID_LEN_FRAME                  11202    /* Invalid frame len.                                   */

#define  NET_IF_ERR_INVALID_ADDR                       11210    /* Invalid addr.                                        */
#define  NET_IF_ERR_INVALID_ADDR_LEN                   11211    /* Invalid addr len.                                    */
#define  NET_IF_ERR_INVALID_ADDR_DEST                  11212    /* Invalid addr dest.                                   */
#define  NET_IF_ERR_INVALID_ADDR_SRC                   11213    /* Invalid addr sec.                                    */


#define  NET_IF_ERR_RX_Q_EMPTY                         11400    /* Rx Q empty; i.e.       NO rx'd pkt signals in Q.     */
#define  NET_IF_ERR_RX_Q_FULL                          11401    /* Rx Q full;  i.e. too many rx'd pkt signals in Q.     */
#define  NET_IF_ERR_RX_Q_SIGNAL                        11405    /* Rx Q signal failed.                                  */
#define  NET_IF_ERR_RX_Q_SIGNAL_TIMEOUT                11406    /* Rx Q signal timeout; NO pkt(s) rx'd from NIC.        */
#define  NET_IF_ERR_RX_Q_SIGNAL_FAULT                  11407    /* Rx Q signal fault.                                   */
#define  NET_IF_ERR_RX_Q_POST_FAIL                       NET_IF_ERR_RX_Q_SIGNAL /* Req'd for backwards-compatibility.   */


#define  NET_IF_ERR_TX_RDY                             11500    /* Tx to dev rdy.                                       */
#define  NET_IF_ERR_TX_BROADCAST                       11501    /* Tx broadcast on local net.                           */
#define  NET_IF_ERR_TX_ADDR_REQ                        11510    /* Tx req  for hw addr.                                 */
#define  NET_IF_ERR_TX_ADDR_PEND                       11511    /* Tx pend on  hw addr.                                 */


/*
*********************************************************************************************************
*                                        ARP LAYER ERROR CODES
*********************************************************************************************************
*/

#define  NET_ARP_ERR_NONE                              15000
#define  NET_ARP_ERR_NULL_PTR                          15020    /* Ptr arg(s) passed NULL ptr(s).                       */


#define  NET_ARP_ERR_INVALID_HW_TYPE                   15100    /* Invalid ARP hw       type.                           */
#define  NET_ARP_ERR_INVALID_HW_LEN                    15101    /* Invalid ARP hw       addr len.                       */
#define  NET_ARP_ERR_INVALID_HW_ADDR                   15102    /* Invalid ARP hw       addr.                           */
#define  NET_ARP_ERR_INVALID_PROTOCOL_TYPE             15103    /* Invalid ARP protocol type.                           */
#define  NET_ARP_ERR_INVALID_PROTOCOL_LEN              15104    /* Invalid ARP protocol addr len.                       */
#define  NET_ARP_ERR_INVALID_PROTOCOL_ADDR             15105    /* Invalid ARP protocol addr.                           */
#define  NET_ARP_ERR_INVALID_OP_CODE                   15106    /* Invalid ARP op code.                                 */
#define  NET_ARP_ERR_INVALID_OP_ADDR                   15107    /* Invalid ARP op code  addr.                           */
#define  NET_ARP_ERR_INVALID_LEN_MSG                   15108    /* Invalid ARP msg len.                                 */


#define  NET_ARP_ERR_CACHE_NONE_AVAIL                  15150    /* NO ARP caches avail.                                 */
#define  NET_ARP_ERR_CACHE_INVALID_TYPE                15151    /* ARP cache type invalid or unknown.                   */
#define  NET_ARP_ERR_CACHE_NOT_FOUND                   15155    /* ARP cache NOT found.                                 */
#define  NET_ARP_ERR_CACHE_PEND                        15156    /* ARP cache hw addr pending.                           */
#define  NET_ARP_ERR_CACHE_RESOLVED                    15157    /* ARP cache hw addr resolved.                          */


#define  NET_ARP_ERR_RX_TARGET_THIS_HOST               15400    /* Rx ARP msg     for this host.                        */
#define  NET_ARP_ERR_RX_TARGET_NOT_THIS_HOST           15401    /* Rx ARP msg NOT for this host.                        */
#define  NET_ARP_ERR_RX_TARGET_REPLY                   15402    /* Rx ARP Reply   for this host NOT in ARP cache.       */

#define  NET_ARP_ERR_RX_REQ_TX_REPLY                   15450    /* Rx'd ARP Req;   tx ARP Reply.                        */
#define  NET_ARP_ERR_RX_REPLY_TX_PKTS                  15451    /* Rx'd ARP Reply; tx ARP cache pkts.                   */


/*$PAGE*/
/*
*********************************************************************************************************
*                                        IP LAYER ERROR CODES
*********************************************************************************************************
*/

#define  NET_IP_ERR_NONE                               21000
#define  NET_IP_ERR_NULL_PTR                           21020    /* Ptr arg(s) passed NULL ptr(s).                       */


#define  NET_IP_ERR_INVALID_VER                        21100    /* Invalid IP ver.                                      */
#define  NET_IP_ERR_INVALID_LEN_HDR                    21101    /* Invalid IP hdr  len.                                 */
#define  NET_IP_ERR_INVALID_TOS                        21102    /* Invalid IP TOS.                                      */
#define  NET_IP_ERR_INVALID_LEN_TOT                    21103    /* Invalid IP tot  len.                                 */
#define  NET_IP_ERR_INVALID_LEN_DATA                   21104    /* Invalid IP data len.                                 */
#define  NET_IP_ERR_INVALID_FLAG                       21105    /* Invalid IP flags.                                    */
#define  NET_IP_ERR_INVALID_FRAG                       21106    /* Invalid IP fragmentation.                            */
#define  NET_IP_ERR_INVALID_PROTOCOL                   21107    /* Invalid/unknown protocol type.                       */
#define  NET_IP_ERR_INVALID_CHK_SUM                    21108    /* Invalid IP chk sum.                                  */
#define  NET_IP_ERR_INVALID_ADDR_SRC                   21110    /* Invalid IP src       addr.                           */
#define  NET_IP_ERR_INVALID_ADDR_DEST                  21111    /* Invalid IP dest      addr.                           */
#define  NET_IP_ERR_INVALID_ADDR_BROADCAST             21112    /* Invalid IP broadcast addr.                           */
#define  NET_IP_ERR_INVALID_ADDR_HOST                  21115    /* Invalid IP host      addr.                           */
#define  NET_IP_ERR_INVALID_ADDR_NET                   21116    /* Invalid IP net       addr.                           */
#define  NET_IP_ERR_INVALID_ADDR_GATEWAY               21117    /* Invalid IP gateway   addr.                           */
#define  NET_IP_ERR_INVALID_OPT                        21120    /* Invalid IP opt.                                      */
#define  NET_IP_ERR_INVALID_OPT_PTR                    21121    /* Invalid IP opt ptr.                                  */
#define  NET_IP_ERR_INVALID_OPT_LEN                    21122    /* Invalid IP opt len.                                  */
#define  NET_IP_ERR_INVALID_OPT_TYPE                   21123    /* Invalid IP opt type.                                 */
#define  NET_IP_ERR_INVALID_OPT_NBR                    21124    /* Invalid IP opt nbr same opt.                         */
#define  NET_IP_ERR_INVALID_OPT_CFG                    21125    /* Invalid IP opt cfg.                                  */
#define  NET_IP_ERR_INVALID_OPT_FLAG                   21126    /* Invalid IP opt flag.                                 */
#define  NET_IP_ERR_INVALID_OPT_ROUTE                  21127    /* Invalid IP opt route.                                */
#define  NET_IP_ERR_INVALID_OPT_END                    21128    /* Invalid IP opt list ending.                          */


#define  NET_IP_ERR_RX_FRAG_NONE                       21400    /* Rx'd datagram NOT frag'd.                            */
#define  NET_IP_ERR_RX_FRAG_OFFSET                     21401    /* Invalid frag offset.                                 */
#define  NET_IP_ERR_RX_FRAG_SIZE                       21402    /* Invalid frag     size.                               */
#define  NET_IP_ERR_RX_FRAG_SIZE_TOT                   21403    /* Invalid frag tot size.                               */
#define  NET_IP_ERR_RX_FRAG_LEN_TOT                    21404    /* Invalid frag tot len.                                */
#define  NET_IP_ERR_RX_FRAG_DISCARD                    21405    /* Invalid frag(s) discarded.                           */
#define  NET_IP_ERR_RX_FRAG_REASM                      21410    /* Frag'd datagram reasm in progress.                   */
#define  NET_IP_ERR_RX_FRAG_COMPLETE                   21411    /* Frag'd datagram reasm'd.                             */

#define  NET_IP_ERR_RX_OPT_BUF_NONE_AVAIL              21420    /* No bufs avail for IP opts.                           */
#define  NET_IP_ERR_RX_OPT_BUF_WR                      21421    /* IP opt buf wr err.                                   */


#define  NET_IP_ERR_TX_DEST_NONE                       21500    /* NO      tx dest.                                     */
#define  NET_IP_ERR_TX_DEST_INVALID                    21501    /* Invalid tx dest.                                     */
#define  NET_IP_ERR_TX_DEST_LOCAL_HOST                 21502    /* Tx to local host   addr.                             */
#define  NET_IP_ERR_TX_DEST_BROADCAST                  21503    /* Tx to local net broadcast.                           */
#define  NET_IP_ERR_TX_DEST_HOST_THIS_NET              21504    /* Tx to local net host.                                */
#define  NET_IP_ERR_TX_DEST_DFLT_GATEWAY               21505    /* Tx to local net dflt gateway.                        */


/*
*********************************************************************************************************
*                                       ICMP LAYER ERROR CODES
*********************************************************************************************************
*/

#define  NET_ICMP_ERR_NONE                             22000
#define  NET_ICMP_ERR_NONE_AVAIL                       22001    /* NO ICMP data type avail.                             */


#define  NET_ICMP_ERR_INVALID_TYPE                     22100    /* Invalid ICMP msg  type / ICMP data type.             */
#define  NET_ICMP_ERR_INVALID_CODE                     22101    /* Invalid ICMP msg  code.                              */
#define  NET_ICMP_ERR_INVALID_PTR                      22102    /* Invalid ICMP msg  ptr.                               */
#define  NET_ICMP_ERR_INVALID_LEN                      22103    /* Invalid ICMP msg  len.                               */
#define  NET_ICMP_ERR_INVALID_LEN_DATA                 22104    /* Invalid ICMP data len.                               */
#define  NET_ICMP_ERR_INVALID_CHK_SUM                  22105    /* Invalid ICMP chk  sum.                               */

#define  NET_ICMP_ERR_MSG_TYPE_ERR                     22150    /* ICMP err   msg type.                                 */
#define  NET_ICMP_ERR_MSG_TYPE_REQ                     22151    /* ICMP req   msg type.                                 */
#define  NET_ICMP_ERR_MSG_TYPE_REPLY                   22152    /* ICMP reply msg type.                                 */


#define  NET_ICMP_ERR_RX_BROADCAST                     22400    /* ICMP rx invalid broadcast.                           */


#define  NET_ICMP_ERR_TX_INVALID_BROADCAST             22500    /* ICMP tx invalid broadcast.                           */
#define  NET_ICMP_ERR_TX_INVALID_FRAG                  22501    /* ICMP tx invalid frag.                                */
#define  NET_ICMP_ERR_TX_INVALID_ADDR_SRC              22502    /* ICMP tx invalid addr src.                            */
#define  NET_ICMP_ERR_TX_INVALID_ERR_MSG               22503    /* ICMP tx invalid err msg.                             */  


/*$PAGE*/
/*
*********************************************************************************************************
*                                        UDP LAYER ERROR CODES
*********************************************************************************************************
*/

#define  NET_UDP_ERR_NONE                              30000
#define  NET_UDP_ERR_NULL_PTR                          30020    /* Ptr arg(s) passed NULL ptr(s).                       */

#define  NET_UDP_ERR_INVALID_DATA_SIZE                 30040    /* Invalid UDP data size.                               */

#define  NET_UDP_ERR_INVALID_LEN                       30100    /* Invalid UDP datagram len.                            */
#define  NET_UDP_ERR_INVALID_LEN_DATA                  30101    /* Invalid UDP data     len.                            */
#define  NET_UDP_ERR_INVALID_PORT_NBR                  30102    /* Invalid UDP port nbr.                                */
#define  NET_UDP_ERR_INVALID_CHK_SUM                   30103    /* Invalid UDP chk  sum.                                */
#define  NET_UDP_ERR_INVALID_FLAG                      30104    /* Invalid UDP flags.                                   */


/*
*********************************************************************************************************
*                                        TCP LAYER ERROR CODES
*********************************************************************************************************
*/

#define  NET_TCP_ERR_NONE                              31000
#define  NET_TCP_ERR_NONE_AVAIL                        31010    /* NO TCP conns avail.                                  */
#define  NET_TCP_ERR_NULL_PTR                          31020    /* Ptr arg(s) passed NULL ptr(s).                       */

#define  NET_TCP_ERR_INVALID_DATA_SIZE                 31040    /* Invalid TCP data size.                               */


#define  NET_TCP_ERR_INVALID_LEN_HDR                   31100    /* Invalid TCP hdr  len.                                */
#define  NET_TCP_ERR_INVALID_LEN_SEG                   31101    /* Invalid TCP seg  len.                                */
#define  NET_TCP_ERR_INVALID_LEN_DATA                  31102    /* Invalid TCP data len.                                */
#define  NET_TCP_ERR_INVALID_PORT_NBR                  31103    /* Invalid TCP port nbr.                                */
#define  NET_TCP_ERR_INVALID_FLAG                      31104    /* Invalid TCP flags.                                   */
#define  NET_TCP_ERR_INVALID_CHK_SUM                   31105    /* Invalid TCP chk sum.                                 */
#define  NET_TCP_ERR_INVALID_OPT                       31110    /* Invalid TCP opt.                                     */
#define  NET_TCP_ERR_INVALID_OPT_TYPE                  31111    /* Invalid TCP opt type.                                */
#define  NET_TCP_ERR_INVALID_OPT_NBR                   31112    /* Invalid TCP opt nbr same opt.                        */
#define  NET_TCP_ERR_INVALID_OPT_LEN                   31113    /* Invalid TCP opt len.                                 */
#define  NET_TCP_ERR_INVALID_OPT_CFG                   31114    /* Invalid TCP opt cfg.                                 */
#define  NET_TCP_ERR_INVALID_OPT_END                   31115    /* Invalid TCP opt list ending.                         */

/*$PAGE*/
#define  NET_TCP_ERR_INVALID_CONN_TYPE                 31200    /* Invalid   TCP conn type.                             */
#define  NET_TCP_ERR_INVALID_CONN                      31201    /* Invalid   TCP conn/id.                               */
#define  NET_TCP_ERR_INVALID_CONN_ID                   31202    /* Invalid   TCP conn's conn id.                        */
#define  NET_TCP_ERR_INVALID_CONN_OP                   31203    /* Invalid   TCP conn op.                               */
#define  NET_TCP_ERR_INVALID_CONN_STATE                31204    /* Invalid   TCP conn state.                            */

#define  NET_TCP_ERR_CONN_NONE                         31210    /* NO        TCP conn.                                  */
#define  NET_TCP_ERR_CONN_NOT_USED                     31211    /*           TCP conn NOT used.                         */
#define  NET_TCP_ERR_CONN_CLOSED                       31220    /*           TCP conn successfully closed.              */
#define  NET_TCP_ERR_CONN_CLOSE                        31221    /*           TCP conn abort        closed.              */
#define  NET_TCP_ERR_CONN_FAULT                        31222    /*           TCP conn fault        closed.              */
#define  NET_TCP_ERR_CONN_FAIL                         31225    /*           TCP conn op failed.                        */
#define  NET_TCP_ERR_CONN_LISTEN_Q_MAX                 31230    /*           TCP conn listen Q max lim.                 */

#define  NET_TCP_ERR_CONN_SEQ_NONE                     31250    /* NO        TCP conn seq.                              */
#define  NET_TCP_ERR_CONN_SEQ_SYNC                     31251    /*   Valid   TCP conn sync.                             */
#define  NET_TCP_ERR_CONN_SEQ_SYNC_INVALID             31252    /* Invalid   TCP conn sync.                             */
#define  NET_TCP_ERR_CONN_SEQ_VALID                    31253    /*   Valid   TCP conn seq.                              */
#define  NET_TCP_ERR_CONN_SEQ_INVALID                  31254    /* Invalid   TCP conn seq.                              */
#define  NET_TCP_ERR_CONN_SEQ_FIN_VALID                31255    /*   Valid   TCP conn fin.                              */
#define  NET_TCP_ERR_CONN_SEQ_FIN_INVALID              31256    /* Invalid   TCP conn fin.                              */

#define  NET_TCP_ERR_CONN_ACK_NONE                     31260    /* NO        TCP conn ack.                              */
#define  NET_TCP_ERR_CONN_ACK_VALID                    31261    /*   Valid   TCP conn ack.                              */
#define  NET_TCP_ERR_CONN_ACK_INVALID                  31262    /* Invalid   TCP conn ack.                              */
#define  NET_TCP_ERR_CONN_ACK_DUP                      31263    /* Duplicate TCP conn ack.                              */
#define  NET_TCP_ERR_CONN_ACK_DLYD                     31265    /* Dly'd     TCP conn ack.                              */
#define  NET_TCP_ERR_CONN_ACK_PREVLY_TXD               31266    /*           TCP conn ack prev'ly tx'd.                 */

#define  NET_TCP_ERR_CONN_RESET_NONE                   31270    /* NO        TCP conn reset.                            */
#define  NET_TCP_ERR_CONN_RESET_VALID                  31271    /*   Valid   TCP conn reset.                            */
#define  NET_TCP_ERR_CONN_RESET_INVALID                31272    /* Invalid   TCP conn reset.                            */

#define  NET_TCP_ERR_CONN_PROBE_INVALID                31282    /* Invalid   TCP conn probe.                            */

#define  NET_TCP_ERR_CONN_DATA_NONE                    31290    /* NO        TCP conn data.                             */
#define  NET_TCP_ERR_CONN_DATA_VALID                   31291    /*   Valid   TCP conn data.                             */
#define  NET_TCP_ERR_CONN_DATA_INVALID                 31292    /* Invalid   TCP conn data.                             */
#define  NET_TCP_ERR_CONN_DATA_DUP                     31293    /* Duplicate TCP conn data.                             */


#define  NET_TCP_ERR_RX                                31400    /*    Rx err.                                           */
#define  NET_TCP_ERR_RX_Q_CLOSED                       31410    /*    Rx Q closed; i.e.   do NOT rx   pkt(s) to Q.      */
#define  NET_TCP_ERR_RX_Q_EMPTY                        31411    /*    Rx Q empty;  i.e.      NO  rx'd pkt(s) in Q.      */
#define  NET_TCP_ERR_RX_Q_FULL                         31412    /*    Rx Q full;   i.e. too many rx'd pkt(s) in Q.      */
#define  NET_TCP_ERR_RX_Q_ABORT                        31415    /*    Rx Q abort      failed.                           */
#define  NET_TCP_ERR_RX_Q_SIGNAL                       31420    /*    Rx Q signal     failed.                           */
#define  NET_TCP_ERR_RX_Q_SIGNAL_CLR                   31421    /*    Rx Q signal clr failed.                           */
#define  NET_TCP_ERR_RX_Q_SIGNAL_TIMEOUT               31422    /*    Rx Q signal timeout.                              */
#define  NET_TCP_ERR_RX_Q_SIGNAL_ABORT                 31423    /*    Rx Q signal aborted.                              */
#define  NET_TCP_ERR_RX_Q_SIGNAL_FAULT                 31424    /*    Rx Q signal fault.                                */

#define  NET_TCP_ERR_TX                                31500    /*    Tx err.                                           */
#define  NET_TCP_ERR_TX_Q_CLOSED                       31510    /*    Tx Q closed; i.e.   do NOT Q tx   pkt(s) to Q.    */
#define  NET_TCP_ERR_TX_Q_EMPTY                        31511    /*    Tx Q empty;  i.e.      NO    tx   pkt(s) in Q.    */
#define  NET_TCP_ERR_TX_Q_FULL                         31512    /*    Tx Q full;   i.e. too many   tx'd pkt(s) in Q.    */
#define  NET_TCP_ERR_TX_Q_ABORT                        31515    /*    Tx Q abort      failed.                           */
#define  NET_TCP_ERR_TX_Q_SUSPEND                      31516    /*    Tx Q suspended.                                   */
#define  NET_TCP_ERR_TX_Q_SIGNAL                       31520    /*    Tx Q signal     failed.                           */
#define  NET_TCP_ERR_TX_Q_SIGNAL_CLR                   31521    /*    Tx Q signal clr failed.                           */
#define  NET_TCP_ERR_TX_Q_SIGNAL_TIMEOUT               31522    /*    Tx Q signal timeout.                              */
#define  NET_TCP_ERR_TX_Q_SIGNAL_ABORT                 31523    /*    Tx Q signal aborted.                              */
#define  NET_TCP_ERR_TX_Q_SIGNAL_FAULT                 31524    /*    Tx Q signal fault.                                */

#define  NET_TCP_ERR_RE_TX_SEG_TH                      31550    /* Re-tx Q seg(s) re-tx'd > th.                         */


/*$PAGE*/
/*
*********************************************************************************************************
*                                    APPLICATION LAYER ERROR CODES
*********************************************************************************************************
*/

#define  NET_APP_ERR_NONE                              40000
#define  NET_APP_ERR_NULL_PTR                          40020    /* Ptr  arg(s) passed NULL ptr(s).                      */


/*
*********************************************************************************************************
*                                  NETWORK SOCKET LAYER ERROR CODES
*********************************************************************************************************
*/

#define  NET_SOCK_ERR_NONE                             41000
#define  NET_SOCK_ERR_NONE_AVAIL                       41010    /* NO socks avail.                                      */
#define  NET_SOCK_ERR_NOT_USED                         41011    /* Sock NOT used.                                       */
#define  NET_SOCK_ERR_CLOSED                           41020    /* Sock       closed.                                   */
#define  NET_SOCK_ERR_FAULT                            41021    /* Sock fault closed.                                   */
#define  NET_SOCK_ERR_TIMEOUT                          41022    /* Sock op(s) timeout.                                  */
#define  NET_SOCK_ERR_NULL_PTR                         41030    /* Ptr  arg(s) passed NULL ptr(s).                      */
#define  NET_SOCK_ERR_NULL_SIZE                        41031    /* Size arg(s) passed NULL size(s).                     */

#define  NET_SOCK_ERR_INVALID_DATA_SIZE                41040    /* Invalid sock data size.                              */


#define  NET_SOCK_ERR_INVALID_FAMILY                   41050    /* Invalid sock protocol family.                        */
#define  NET_SOCK_ERR_INVALID_PROTOCOL                 41051    /* Invalid sock protocol.                               */
#define  NET_SOCK_ERR_INVALID_TYPE                     41052    /* Invalid sock type.                                   */
#define  NET_SOCK_ERR_INVALID_SOCK                     41053    /* Invalid sock      id.                                */
#define  NET_SOCK_ERR_INVALID_DESC                     41054    /* Invalid sock desc id(s).                             */
#define  NET_SOCK_ERR_INVALID_CONN                     41055    /* Invalid sock conn/id.                               */
#define  NET_SOCK_ERR_INVALID_STATE                    41056    /* Invalid sock state.                                  */
#define  NET_SOCK_ERR_INVALID_OP                       41057    /* Invalid sock op.                                     */
#define  NET_SOCK_ERR_INVALID_FLAG                     41058    /* Invalid sock flag.                                   */
#define  NET_SOCK_ERR_INVALID_TIMEOUT                  41059    /* Invalid sock timeout val.                            */

#define  NET_SOCK_ERR_INVALID_ADDR                     41070    /* Invalid sock addr.                                   */
#define  NET_SOCK_ERR_INVALID_ADDR_LEN                 41071    /* Invalid sock addr len.                               */
#define  NET_SOCK_ERR_ADDR_IN_USE                      41072    /* Sock addr cur in use.                                */

#define  NET_SOCK_ERR_INVALID_PORT_NBR                 41080    /* Invalid port nbr.                                    */
#define  NET_SOCK_ERR_INVALID_PORT_Q_NBR_USED          41081    /* Invalid nbr Q entries used.                          */
#define  NET_SOCK_ERR_PORT_NBR_NONE_AVAIL              41085    /* Port nbr(s) NOT avail.                               */
#define  NET_SOCK_ERR_PORT_NBR_IN_Q                    41086    /* Port nbr cur in Q.                                   */

#define  NET_SOCK_ERR_INVALID_EVENT                    41090    /* Invalid sock event.                                  */
#define  NET_SOCK_ERR_EVENTS_NBR_MAX                   41091    /* Sock nbr events max'd.                               */


#define  NET_SOCK_ERR_CONN_IN_USE                      41100    /* Sock conn cur in use.                                */
#define  NET_SOCK_ERR_CONN_IN_PROGRESS                 41101    /* Sock conn        NOT complete.                       */
#define  NET_SOCK_ERR_CONN_CLOSED                      41105    /* Sock conn closed.                                    */
#define  NET_SOCK_ERR_CONN_CLOSE_IN_PROGRESS           41106    /* Sock conn close  NOT complete.                       */
#define  NET_SOCK_ERR_CONN_FAIL                        41110    /* Sock conn op         failed.                         */
#define  NET_SOCK_ERR_CONN_ABORT                       41112    /* Sock conn abort      failed.                         */
#define  NET_SOCK_ERR_CONN_SIGNAL                      41120    /* Sock conn signal     failed.                         */
#define  NET_SOCK_ERR_CONN_SIGNAL_CLR                  41121    /* Sock conn signal clr failed.                         */
#define  NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT              41122    /* Sock conn signal timeout.                            */
#define  NET_SOCK_ERR_CONN_SIGNAL_ABORT                41123    /* Sock conn signal aborted.                            */
#define  NET_SOCK_ERR_CONN_SIGNAL_FAULT                41124    /* Sock conn signal fault.                              */
#define  NET_SOCK_ERR_CONN_ACCEPT_Q_NONE_AVAIL         41130    /* Sock conn accept Q conn id's NOT avail.              */
#define  NET_SOCK_ERR_CONN_ACCEPT_Q_MAX                41132    /* Sock conn accept Q max  lim.                         */
#define  NET_SOCK_ERR_CONN_ACCEPT_Q_DUP                41133    /* Sock conn accept Q conn id dup.                      */


#define  NET_SOCK_ERR_RX_Q_CLOSED                      41400    /* Rx Q closed; i.e.   do NOT rx   pkt(s) to Q.         */
#define  NET_SOCK_ERR_RX_Q_EMPTY                       41401    /* Rx Q empty;  i.e.      NO  rx'd pkt(s) in Q.         */
#define  NET_SOCK_ERR_RX_Q_FULL                        41402    /* Rx Q full;   i.e. too many rx'd pkt(s) in Q.         */
#define  NET_SOCK_ERR_RX_Q_ABORT                       41405    /* Rx Q abort      failed.                              */
#define  NET_SOCK_ERR_RX_Q_SIGNAL                      41410    /* Rx Q signal     failed.                              */
#define  NET_SOCK_ERR_RX_Q_SIGNAL_CLR                  41411    /* Rx Q signal clr failed.                              */
#define  NET_SOCK_ERR_RX_Q_SIGNAL_TIMEOUT              41412    /* Rx Q signal timeout.                                 */
#define  NET_SOCK_ERR_RX_Q_SIGNAL_ABORT                41413    /* Rx Q signal aborted.                                 */
#define  NET_SOCK_ERR_RX_Q_SIGNAL_FAULT                41414    /* Rx Q signal fault.                                   */

#define  NET_SOCK_ERR_TX_Q_CLOSED                      41500    /* Tx Q closed; i.e. do NOT Q pkt(s) to tx.             */


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    NETWORK ERROR CODES DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_ERR;


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                               NETWORK ARGUMENT CHECK FAILURE MACRO'S
*
* Description : Call application-specific handler function(s) to monitor, log, & handle network error(s).
*
* Argument(s) : id      Specific network error code ID.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               These macro's are INTERNAL network protocol suite macro's & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) (a) When NET_ERR_CFG_ARG_CHK_EXT_EN is ENABLED, ALL arguments received from any port interface
*                       provided by the developer or application are checked/validated.
*
*                       To provide feedback to an application on any port interface argument check failures, the 
*                       developer MUST implement the failure hook function, NetErr_FailArgChkExt(), in
*               
*                           \<Your Product Application>\net_bsp.c
*
*                               where
*                                       <Your Product Application>    directory path for Your Product's Application
*
*
*                   (b) When NET_ERR_CFG_ARG_CHK_DBG_EN is ENABLED, internal arguments are checked/validated to
*                       debug the network protocol suite.
*
*                       To provide feedback to an application on any internal network argument check failures, the 
*                       developer MUST implement the failure hook function, NetErr_FailArgChkDbg(), in
*               
*                           \<Your Product Application>\net_bsp.c
*
*                               where
*                                       <Your Product Application>    directory path for Your Product's Application
*
*
*               (2) When argument check failure macro's are used in conjunction with network counter macro's
*                   (see 'net_ctr.h  NETWORK COUNTER MACRO'S'), argument check failure macro MUST be called
*                   AFTER network counter macro.  This ensures that the network counter is adjusted BEFORE
*                   the failure hook function is called (see Notes #1 & #2).
*
*               (3) Argument check identification numbers (id's) are grouped in series of 1000 per module/layer.
*
*               (4) (a) #### Argument check failure macro's     NOT yet implemented.
*
*                   (b) #### Argument check failure macro's may NOT be necessary (remove if unnecessary).
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
#define  NET_ERR_FAIL_ARG_CHK_EXT(id)               { NetErr_FailArgChkExt((CPU_INT16U)id); }
#else
#define  NET_ERR_FAIL_ARG_CHK_EXT(id)
#endif

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
#define  NET_ERR_FAIL_ARG_CHK_DBG(id)               { NetErr_FailArgChkDbg((CPU_INT16U)id); }
#else
#define  NET_ERR_FAIL_ARG_CHK_DBG(id)
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if (NET_DBG_CFG_TEST_EN == DEF_ENABLED) 
CPU_CHAR  *NetErr_GetStr  (NET_ERR     err_code);
#endif


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                   DEFINED IN PRODUCT'S  net_bsp.c
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
void  NetErr_FailArgChkExt(CPU_INT16U  id);
#endif

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
void  NetErr_FailArgChkDbg(CPU_INT16U  id);
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_ERR_CFG_ARG_CHK_EXT_EN
#error  "NET_ERR_CFG_ARG_CHK_EXT_EN        not #define'd in 'net_cfg.h'"
#error  "                            [MUST be  DEF_DISABLED]           "
#error  "                            [     ||  DEF_ENABLED ]           "
#elif  ((NET_ERR_CFG_ARG_CHK_EXT_EN != DEF_DISABLED) && \
        (NET_ERR_CFG_ARG_CHK_EXT_EN != DEF_ENABLED ))
#error  "NET_ERR_CFG_ARG_CHK_EXT_EN  illegally #define'd in 'net_cfg.h'"
#error  "                            [MUST be  DEF_DISABLED]           "
#error  "                            [     ||  DEF_ENABLED ]           "
#endif



#ifndef  NET_ERR_CFG_ARG_CHK_DBG_EN
#error  "NET_ERR_CFG_ARG_CHK_DBG_EN        not #define'd in 'net_cfg.h'"
#error  "                            [MUST be  DEF_DISABLED]           "
#error  "                            [     ||  DEF_ENABLED ]           "
#elif  ((NET_ERR_CFG_ARG_CHK_DBG_EN != DEF_DISABLED) && \
        (NET_ERR_CFG_ARG_CHK_DBG_EN != DEF_ENABLED ))
#error  "NET_ERR_CFG_ARG_CHK_DBG_EN  illegally #define'd in 'net_cfg.h'"
#error  "                            [MUST be  DEF_DISABLED]           "
#error  "                            [     ||  DEF_ENABLED ]           "
#endif

