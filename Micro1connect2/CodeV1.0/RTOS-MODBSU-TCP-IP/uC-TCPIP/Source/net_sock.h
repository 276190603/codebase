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
*                                         NETWORK SOCKET LAYER
*
* Filename      : net_sock.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports BSD 4.x Socket Layer with the following restrictions/constraints :
*
*                     (a) ONLY supports a single address family from the following families :
*                         (1) IPv4 (AF_INET)
*
*                     (b) ONLY supports the following socket types :
*                         (1) Datagram (SOCK_DGRAM)
*                         (2) Stream   (SOCK_STREAM)
*
*                     (c) ONLY supports a single protocol family from the following families :
*                         (1) IPv4 (PF_INET)
*                             (A) ONLY supports the following protocols :
*                                 (1) UDP (IPPROTO_UDP)
*                                 (2) TCP (IPPROTO_TCP)
*
*                     (d) ONLY supports the following socket options :
*                         (1) Global options
*                             #### ( )
*                         (2) Socket options
*                             #### ( )
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


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) Network Socket Layer module is required for :
*
*               (a) IPv4 Sockets
*                   (1) UDP Datagram Sockets
*                   (2) TCP Stream   Sockets
*
*               See also 'net_sock.h  Note #1c',
*                        'net_cfg.h   TRANSPORT LAYER CONFIGURATION',
*                      & 'net_cfg.h   USER DATAGRAM PROTOCOL LAYER CONFIGURATION'.
*
*           (2) The following socket-module-present configuration value MUST be pre-#define'd in 
*               'net_cfg_net.h' PRIOR to all other network modules that require Network Socket Layer
*               configuration (see 'net_cfg_net.h  NETWORK SOCKET LAYER CONFIGURATION  Note #2b') :
*
*                   NET_SOCK_MODULE_PRESENT
*********************************************************************************************************
*/

#ifdef   NET_SOCK_MODULE_PRESENT                                /* See Note #2.                                         */


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_SOCK_MODULE
#define  NET_SOCK_EXT
#else
#define  NET_SOCK_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                  NETWORK SOCKET PORT NUMBER DEFINES
*
* Note(s) : (1) Socket port numbers defined in host-order.
*
*               See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*********************************************************************************************************
*/

#define  NET_SOCK_PORT_NBR_RESERVED                      NET_PORT_NBR_RESERVED
#define  NET_SOCK_PORT_NBR_NONE                          NET_SOCK_PORT_NBR_RESERVED
#define  NET_SOCK_PORT_NBR_RANDOM                        NET_SOCK_PORT_NBR_RESERVED


#define  NET_SOCK_PORT_NBR_RANDOM_NBR_OFFSET              10

#define  NET_SOCK_PORT_NBR_RANDOM_NBR                   (NET_SOCK_CFG_NBR_SOCK + NET_SOCK_PORT_NBR_RANDOM_NBR_OFFSET)

#if     (NET_SOCK_PORT_NBR_RANDOM_NBR                  > NET_PORT_NBR_MAX)
#undef   NET_SOCK_PORT_NBR_RANDOM_NBR
#define  NET_SOCK_PORT_NBR_RANDOM_NBR                    NET_PORT_NBR_MAX
#endif

#define  NET_SOCK_PORT_NBR_RANDOM_MIN                    NET_SOCK_CFG_PORT_NBR_RANDOM_BASE
#define  NET_SOCK_PORT_NBR_RANDOM_MAX                   (NET_SOCK_CFG_PORT_NBR_RANDOM_BASE + NET_SOCK_PORT_NBR_RANDOM_NBR - 1)


/*
*********************************************************************************************************
*                                        NETWORK SOCKET STATES
*********************************************************************************************************
*/

#define  NET_SOCK_STATE_NONE                               0
#define  NET_SOCK_STATE_FREE                               1
#define  NET_SOCK_STATE_DISCARD                            2
#define  NET_SOCK_STATE_CLOSED                            10
#define  NET_SOCK_STATE_CLOSED_FAULT                      11
#define  NET_SOCK_STATE_CLOSE_IN_PROGRESS                 15
#define  NET_SOCK_STATE_CLOSING_DATA_AVAIL                16
#define  NET_SOCK_STATE_BOUND                             20
#define  NET_SOCK_STATE_LISTEN                            30
#define  NET_SOCK_STATE_CONN                              40
#define  NET_SOCK_STATE_CONN_IN_PROGRESS                  41
#define  NET_SOCK_STATE_CONN_DONE                         42


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NETWORK SOCKET FLAG DEFINES
*********************************************************************************************************
*/

                                                                /* ------------------ NET SOCK FLAGS ------------------ */
#define  NET_SOCK_FLAG_NONE                       DEF_BIT_NONE
#define  NET_SOCK_FLAG_USED                       DEF_BIT_00    /* Sock cur used; i.e. NOT in free sock pool.           */
#define  NET_SOCK_FLAG_BLOCK                      DEF_BIT_01    /* Sock blocking ENABLED.                               */


/*
*********************************************************************************************************
*                                     NETWORK SOCKET TYPE DEFINES
*
* Note(s) : (1) NET_SOCK_TYPE_??? #define values specifically chosen as ASCII representations of the socket
*               types.  Memory displays of sockets will display the socket TYPEs with their chosen ASCII 
*               names.
*********************************************************************************************************
*/

                                                                /* ------------------ NET SOCK TYPES ------------------ */
#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  NET_SOCK_TYPE_SOCK_NONE                  0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_SOCK_TYPE_SOCK                       0x534F434B    /* "SOCK" in ASCII.                                     */

#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)
#define  NET_SOCK_TYPE_SOCK_NONE                  0x454E4F4E    /* "NONE" in ASCII.                                     */
#define  NET_SOCK_TYPE_SOCK                       0x4B434F53    /* "SOCK" in ASCII.                                     */

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  NET_SOCK_TYPE_SOCK_NONE                  0x4F4E454E    /* "NONE" in ASCII.                                     */
#define  NET_SOCK_TYPE_SOCK                       0x4F534B43    /* "SOCK" in ASCII.                                     */

#else                                                           /* Dflt CPU_WORD_SIZE_08.                               */
#define  NET_SOCK_TYPE_SOCK_NONE                  0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_SOCK_TYPE_SOCK                       0x534F434B    /* "SOCK" in ASCII.                                     */

#endif
#endif


/*
*********************************************************************************************************
*                                  NETWORK SOCKET EVENT TYPE DEFINES
*
* Note(s) : (1) 'EVENT_TYPE' abbreviated to 'EVENT' for event type constants to enforce ANSI-compliance
*                of 31-character symbol length uniqueness.
*********************************************************************************************************
*/

#define  NET_SOCK_EVENT_NONE                               0
#define  NET_SOCK_EVENT_ERR                                1


#define  NET_SOCK_EVENT_SOCK_RX                           10
#define  NET_SOCK_EVENT_SOCK_TX                           11
#define  NET_SOCK_EVENT_SOCK_ACCEPT                       12
#define  NET_SOCK_EVENT_SOCK_CONN                         13
#define  NET_SOCK_EVENT_SOCK_CLOSE                        14

#define  NET_SOCK_EVENT_SOCK_ERR_RX                       20
#define  NET_SOCK_EVENT_SOCK_ERR_TX                       21
#define  NET_SOCK_EVENT_SOCK_ERR_ACCEPT                   22
#define  NET_SOCK_EVENT_SOCK_ERR_CONN                     23
#define  NET_SOCK_EVENT_SOCK_ERR_CLOSE                    24


#define  NET_SOCK_EVENT_TRANSPORT_RX                      30
#define  NET_SOCK_EVENT_TRANSPORT_TX                      31

#define  NET_SOCK_EVENT_TRANSPORT_ERR_RX                  40
#define  NET_SOCK_EVENT_TRANSPORT_ERR_TX                  41


/*$PAGE*/
/*
*********************************************************************************************************
*                              NETWORK SOCKET FAMILY & PROTOCOL DEFINES
*
* Note(s) : (1) The following socket values MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_cfg.h'
*               so that the developer can configure sockets for the correct socket family values (see
*               'net_def.h  BSD 4.x & NETWORK SOCKET LAYER DEFINES  Note #1' & 'net_cfg_net.h  NETWORK 
*               SOCKET LAYER CONFIGURATION') :
*
*               (a) (1) NET_SOCK_ADDR_FAMILY_IP_V4
*                   (2) NET_SOCK_PROTOCOL_FAMILY_IP_V4
*                   (3) NET_SOCK_FAMILY_IP_V4
*
*               (b) (1) NET_SOCK_ADDR_LEN_IP_V4
*                   (2) NET_SOCK_PROTOCOL_MAX_IP_V4
*********************************************************************************************************
*/

#if 0                                                           /* See Note #1.                                         */
                                                                /* ------------ SOCK FAMILY/PROTOCOL TYPES ------------ */
#define  NET_SOCK_PROTOCOL_FAMILY_IP_V4              PF_INET    /* TCP/IP sock protocol family type.                    */
#define  NET_SOCK_ADDR_FAMILY_IP_V4                  AF_INET    /* TCP/IP sock addr     family type.                    */
#define  NET_SOCK_FAMILY_IP_V4                           NET_SOCK_PROTOCOL_FAMILY_IP_V4

#define  NET_SOCK_ADDR_LEN_IP_V4                           6    /* TCP/IP sock addr len =                               */
                                                                /*             2-octet TCP/UDP port value               */
                                                                /*             4-octet IP      addr value               */

#define  NET_SOCK_PROTOCOL_MAX_IP_V4                     NET_SOCK_PROTOCOL_MAX_IP_V4_UDP_TCP

#endif


                                                                /* -------------------- SOCK TYPES -------------------- */
#define  NET_SOCK_TYPE_NONE                                0
#define  NET_SOCK_TYPE_FAULT                              -1

#define  NET_SOCK_TYPE_DATAGRAM                         SOCK_DGRAM
#define  NET_SOCK_TYPE_STREAM                           SOCK_STREAM


                                                                /* -------------- TCP/IP SOCK PROTOCOLS --------------- */
#define  NET_SOCK_PROTOCOL_DFLT                            0
#define  NET_SOCK_PROTOCOL_NONE                         NET_SOCK_PROTOCOL_DFLT

#define  NET_SOCK_PROTOCOL_UDP                          IPPROTO_UDP
#define  NET_SOCK_PROTOCOL_TCP                          IPPROTO_TCP


/*
*********************************************************************************************************
*                                   NETWORK SOCKET ADDRESS DEFINES
*********************************************************************************************************
*/

                                                                /* ------------------ SOCK ADDR CFG ------------------- */
#if     (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)

#define  NET_SOCK_ADDR_IP_LEN_PORT               (sizeof(NET_PORT_NBR))
#define  NET_SOCK_ADDR_IP_LEN_ADDR               (sizeof(NET_IP_ADDR))

#define  NET_SOCK_ADDR_IP_IX_BASE                          0
#define  NET_SOCK_ADDR_IP_IX_PORT                        NET_SOCK_ADDR_IP_IX_BASE
#define  NET_SOCK_ADDR_IP_IX_ADDR                       (NET_SOCK_ADDR_IP_IX_PORT + NET_SOCK_ADDR_IP_LEN_PORT) 


#define  NET_SOCK_ADDR_IP_WILD_CARD                      INADDR_ANY
#define  NET_SOCK_ADDR_IP_BROADCAST                      INADDR_BROADCAST

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NETWORK SOCKET API DEFINES
*********************************************************************************************************
*/

#define  NET_SOCK_BSD_ERR_NONE                           NET_BSD_ERR_NONE
#define  NET_SOCK_BSD_ERR_DFLT                           NET_BSD_ERR_DFLT

#define  NET_SOCK_BSD_ERR_OPEN                           NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_CLOSE                          NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_BIND                           NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_CONN                           NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_LISTEN                         NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_ACCEPT                         NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_RX                             NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_TX                             NET_SOCK_BSD_ERR_DFLT
#define  NET_SOCK_BSD_ERR_SEL                            NET_SOCK_BSD_ERR_DFLT


#define  NET_SOCK_BSD_RTN_CODE_OK                        NET_BSD_RTN_CODE_OK
#define  NET_SOCK_BSD_RTN_CODE_TIMEOUT                   NET_BSD_RTN_CODE_TIMEOUT
#define  NET_SOCK_BSD_RTN_CODE_CONN_CLOSED               NET_BSD_RTN_CODE_CONN_CLOSED


/*
*********************************************************************************************************
*                                         SOCKET FLAG DEFINES
*********************************************************************************************************
*/

#define  NET_SOCK_FLAG_RX_DATA_PEEK                      MSG_PEEK

#define  NET_SOCK_FLAG_NO_BLOCK                          MSG_DONTWAIT
#define  NET_SOCK_FLAG_RX_NO_BLOCK                       NET_SOCK_FLAG_NO_BLOCK
#define  NET_SOCK_FLAG_TX_NO_BLOCK                       NET_SOCK_FLAG_NO_BLOCK


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                               NETWORK SOCKET ADDRESS FAMILY DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_SOCK_ADDR_FAMILY;


/*
*********************************************************************************************************
*                               NETWORK SOCKET ADDRESS LENGTH DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16S  NET_SOCK_ADDR_LEN;


/*
*********************************************************************************************************
*                              NETWORK SOCKET PROTOCOL FAMILY DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16S  NET_SOCK_PROTOCOL_FAMILY;


/*
*********************************************************************************************************
*                                  NETWORK SOCKET PROTOCOL DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16S  NET_SOCK_PROTOCOL;


/*
*********************************************************************************************************
*                                    NETWORK SOCKET TYPE DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16S  NET_SOCK_TYPE;


/*
*********************************************************************************************************
*                            NETWORK SOCKET (ERROR) RETURN CODE DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16S  NET_SOCK_RTN_CODE;


/*
*********************************************************************************************************
*                                   NETWORK SOCKET STATE DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT08U  NET_SOCK_STATE;


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NETWORK SOCKET QUANTITY DATA TYPE
*
* Note(s) : (1) See also 'NETWORK SOCKET IDENTIFICATION DATA TYPE  Note #1'.
*********************************************************************************************************
*/

typedef  CPU_INT16S  NET_SOCK_QTY;                              /* Defines max qty of socks to support.                 */


/*
*********************************************************************************************************
*                               NETWORK SOCKET IDENTIFICATION DATA TYPE
*
* Note(s) : (1) (a) NET_SOCK_NBR_MAX  SHOULD be #define'd based on 'NET_SOCK_QTY' data type declared.
*
*               (b) However, since socket handle identifiers are data-typed as 16-bit signed integers; 
*                   the maximum unique number of valid socket handle identifiers, & therefore the 
*                   maximum number of valid sockets, is the total number of non-negative values that 
*                   16-bit signed integers support.
*********************************************************************************************************
*/

typedef  CPU_INT16S  NET_SOCK_ID;

#define  NET_SOCK_NBR_MIN                                  1
#define  NET_SOCK_NBR_MAX                DEF_INT_16S_MAX_VAL    /* See Note #1.                                         */

#define  NET_SOCK_ID_NONE                                 -1
#define  NET_SOCK_ID_MIN                                   0
#define  NET_SOCK_ID_MAX          (NET_SOCK_CFG_NBR_SOCK - 1)


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NETWORK SOCKET QUEUE SIZE DATA TYPE
*
* Note(s) : (1) (a) NET_SOCK_Q_SIZE #define's SHOULD be #define'd based on 'NET_SOCK_Q_SIZE'
*                   data type declared.
*
*               (b) However, since socket/connection handle identifiers are data-typed as 16-bit 
*                   signed integers; the maximum unique number of valid socket/connection handle 
*                   identifiers, & therefore the maximum number of valid sockets/connections, is 
*                   the total number of non-negative values that 16-bit signed integers support.
*
*                   See also             'NETWORK SOCKET     IDENTIFICATION DATA TYPE  Note #1b'
*                          & 'net_conn.h  NETWORK CONNECTION IDENTIFICATION DATA TYPE  Note #2b'.
*
*           (2) (a) NET_SOCK_Q_IX   #define's SHOULD be #define'd based on 'NET_SOCK_Q_SIZE'
*                   data type declared.
*
*               (b) Since socket queue size is data typed as a 16-bit unsigned integer but the
*                   maximum queue sizes are #define'd as 16-bit signed integer values ... :
*
*                   (1) Valid socket queue indices are #define'd within the range of     16-bit 
*                         signed integer values, ...
*                   (2) but   socket queue indice exception values may be #define'd with 16-bit
*                       unsigned integer values.
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_SOCK_Q_SIZE;

                                                                /* See Note #1.                                         */
#define  NET_SOCK_Q_SIZE_NONE          0
#define  NET_SOCK_Q_SIZE_MIN         NET_SOCK_NBR_MIN
#define  NET_SOCK_Q_SIZE_MAX         NET_SOCK_NBR_MAX           /* See Note #1b.                                        */

                                                                /* See Note #2.                                         */
#define  NET_SOCK_Q_IX_NONE          DEF_INT_16U_MAX_VAL        /* See Note #2b.                                        */
#define  NET_SOCK_Q_IX_MIN                                 0
#define  NET_SOCK_Q_IX_MAX          (NET_SOCK_Q_SIZE_MAX - 1)


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NETWORK SOCKET ADDRESS DATA TYPES
*
* Note(s) : (1) See 'net_sock.h  Note #1a' for supported socket address families.
*
*           (2) (a) Socket address structure 'AddrFamily' member MUST be configured in host-order & MUST 
*                   NOT be converted to/from network-order.
*
*               (b) Socket address structure addresses MUST be configured/converted from host-order to
*                   network-order.
*
*               See also 'net_bsd.h  BSD 4.x SOCKET DATA TYPES  Note #2b'.
*********************************************************************************************************
*/

                                                                        /* -------------- NET SOCK ADDR --------------- */
#define  NET_SOCK_BSD_ADDR_LEN_MAX                  NET_BSD_ADDR_LEN_MAX

typedef  struct  net_sock_addr {
    NET_SOCK_ADDR_FAMILY    AddrFamily;                                 /* Sock addr family type (see Note #2a).        */
    CPU_INT08U              Addr[NET_SOCK_BSD_ADDR_LEN_MAX];            /* Sock addr             (see Note #2b).        */
} NET_SOCK_ADDR;

#define  NET_SOCK_ADDR_SIZE                 (sizeof(NET_SOCK_ADDR))



#if (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
                                                                        /* ------------- NET SOCK ADDR IP ------------- */
#define  NET_SOCK_ADDR_IP_NBR_OCTETS_UNUSED         NET_BSD_ADDR_IP_NBR_OCTETS_UNUSED

typedef  struct  net_sock_addr_ip {
    NET_SOCK_ADDR_FAMILY    AddrFamily;                                 /* Sock addr family type (see Note #2a).        */
    NET_PORT_NBR            Port;                                       /* UDP/TCP port nbr      (see Note #2b).        */
    NET_IP_ADDR             Addr;                                       /* IP addr               (see Note #2b).        */
    CPU_INT08U              Unused[NET_SOCK_ADDR_IP_NBR_OCTETS_UNUSED]; /* Unused (MUST be zero).                       */
} NET_SOCK_ADDR_IP;

#define  NET_SOCK_ADDR_IP_SIZE              (sizeof(NET_SOCK_ADDR_IP))

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NETWORK SOCKET DATA TYPE
*
*                             NET_SOCK
*                          |-------------|
*                          |  Sock Type  |
*                          |-------------|      Next
*                          |      O----------> Socket     Buffer Queue
*                          |-------------|                    Heads      -------
*                          |      O------------------------------------> |     |
*                          |-------------|                               |     |
*                          |      O----------------------                -------
*                          |-------------|              |                  | ^
*                          |  Conn IDs   |              |                  v |
*                          |-------------|              |                -------
*                          |    Sock     |              |                |     |
*                          |   Family/   |              |                |     |
*                          |  Protocol   |              |                -------
*                          |-------------|              |                  | ^
*                          | Conn Ctrls  |              | Buffer Queue     v |
*                          |-------------|              |     Tails      -------
*                          |    Flags    |              ---------------> |     |
*                          |-------------|                               |     |
*                          |    State    |                               -------
*                          |-------------|
*
*
* Note(s) : (1) Forced word-alignment at start of socket NOT required since first data member 'Type'
*               is declared as 'CPU_INT32U'.
*
*           (2) #### 'TxQ_Head'/'TxQ_Tail' may NOT be necessary (remove if unnecessary).
*********************************************************************************************************
*/

                                                                        /* ----------------- NET SOCK ----------------- */
struct  net_sock {
    NET_TYPE                    Type;                                   /* Type cfg'd @ init : NET_SOCK_TYPE_SOCK.      */

    NET_SOCK                   *NextPtr;                                /* Ptr to NEXT sock.                            */


    NET_BUF                    *RxQ_Head;                               /* Ptr to head of sock's rx buf Q.              */
    NET_BUF                    *RxQ_Tail;                               /* Ptr to tail of sock's rx buf Q.              */

#if 0                                                                   /* #### See Note #2.                            */
    NET_BUF                    *TxQ_Head;                               /* Ptr to head of sock's tx buf Q.              */
    NET_BUF                    *TxQ_Tail;                               /* Ptr to tail of sock's tx buf Q.              */
#endif


    NET_SOCK_ID                 ID;                                     /* Sock id.                                     */
    NET_CONN_ID                 ID_Conn;                                /* Conn id.                                     */


    NET_SOCK_PROTOCOL_FAMILY    ProtocolFamily;                         /* Sock protocol family.                        */
    NET_SOCK_PROTOCOL           Protocol;                               /* Sock protocol.                               */
    NET_SOCK_TYPE               SockType;                               /* Sock type.                                   */


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
    NET_SOCK_Q_SIZE             ConnAcceptQ_SizeMax;                    /* Max Q size to accept rx'd conn reqs.         */
    NET_SOCK_Q_SIZE             ConnAcceptQ_SizeCur;                    /* Cur Q size to accept rx'd conn reqs.         */
                                                                        /* Conn accept Q (conn id's q'd into array).    */
    NET_CONN_ID                 ConnAcceptQ[NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX];
    NET_SOCK_Q_SIZE             ConnAcceptQ_HeadIx;                     /* Conn accept Q head ix.                       */
    NET_SOCK_Q_SIZE             ConnAcceptQ_TailIx;                     /* Conn accept Q tail ix.                       */
#endif


    NET_SOCK_STATE              State;                                  /* Sock state.                                  */

    CPU_INT16U                  Flags;                                  /* Sock flags.                                  */
};


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NETWORK SOCKET TIMEOUT DATA TYPE
*
* Note(s) : (1) (a) IEEE Std 1003.1, 2004 Edition, Section 'sys/select.h : DESCRIPTION' states that "the 
*                   timeval structure ... includes at least the following members" :
*
*                   (1) time_t         tv_sec     Seconds
*                   (2) suseconds_t    tv_usec    Microseconds
*
*               (b) Ideally, the Network Socket Layer's 'NET_SOCK_TIMEOUT' data type would be based on 
*                   the BSD 4.x Layer's 'timeval' data type definition.  However, since BSD 4.x Layer 
*                   application interface (API) is NOT guaranteed to be present in the project build 
*                   (see 'net_bsd.h  MODULE  Note #1bA'); the Network Socket Layer's 'NET_SOCK_TIMEOUT' 
*                   data type MUST be independently defined.
*
*                   However, for correct interoperability between the BSD 4.x Layer 'timeval' data type
*                   & the Network Socket Layer's 'NET_SOCK_TIMEOUT' data type; ANY modification to either 
*                   of these data types MUST be appropriately synchronized.
*
*               See also 'net_bsd.h  BSD 4.x SOCKET DATA TYPES  Note #4'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)

typedef  struct  net_sock_timeout {                                     /* See Note #1a.                                */
    CPU_INT32S      timeout_sec;
    CPU_INT32S      timeout_us;
} NET_SOCK_TIMEOUT;

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                      NETWORK SOCKET (IDENTIFICATION) DESCRIPTOR SET DATA TYPE
*
* Note(s) : (1) (a) (1) IEEE Std 1003.1, 2004 Edition, Section 'sys/select.h : DESCRIPTION' states 
*                       that the "'fd_set' type ... shall [be] define[d] ... as a structure".
*
*                   (2) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                       6th Printing, Section 6.3, Pages 162-163 states that "descriptor sets [are] 
*                       typically an array of integers, with each bit in each integer corresponding 
*                       to a descriptor".
*
*               (b) Ideally, the Network Socket Layer's 'NET_SOCK_DESC' data type would be based on
*                   the BSD 4.x Layer's 'fd_set' data type definition.  However, since BSD 4.x Layer
*                   application interface (API) is NOT guaranteed to be present in the project build
*                   (see 'net_bsd.h  MODULE  Note #1bA'); the Network Socket Layer's 'NET_SOCK_DESC' 
*                   data type MUST be independently defined.
*
*                   However, for correct interoperability between the BSD 4.x Layer 'fd_set' data type
*                   & the Network Socket Layer's 'NET_SOCK_DESC' data type; ANY modification to either 
*                   of these data types MUST be appropriately synchronized.
*
*               See also 'net_bsd.h  BSD 4.x SOCKET DATA TYPES  Note #5'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)

#define  NET_SOCK_DESC_NBR_MIN_DESC                               0
#define  NET_SOCK_DESC_NBR_MAX_DESC           NET_SOCK_CFG_NBR_SOCK

#define  NET_SOCK_DESC_NBR_MIN                                             0
#define  NET_SOCK_DESC_NBR_MAX               (NET_SOCK_DESC_NBR_MAX_DESC - 1)

#define  NET_SOCK_DESC_ARRAY_SIZE          (((NET_SOCK_DESC_NBR_MAX_DESC - 1) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)) + 1)

typedef  struct  net_sock_desc {                                                /* See Note #1a.                        */
    CPU_DATA        SockID_DescNbrSet[NET_SOCK_DESC_ARRAY_SIZE];
} NET_SOCK_DESC;

#endif


/*
*********************************************************************************************************
*                                   NETWORK SOCKET EVENT DATA TYPES
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)

typedef  CPU_INT32U  NET_SOCK_EVENT_QTY;                                /* Defines max qty of sock events to support.   */
typedef  CPU_INT08U  NET_SOCK_EVENT_TYPE;


typedef  struct  net_sock_event_tbl {
    NET_SOCK_EVENT_TYPE     EventType;                                  /* Sock event type.                             */
    NET_SOCK_ID             SockID;                                     /* Sock event sock id.                          */
    NET_CONN_ID             ConnID;                                     /* Sock event conn id.                          */
} NET_SOCK_EVENT_TBL;

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

NET_SOCK_EXT  NET_SOCK           NetSock_Tbl[NET_SOCK_CFG_NBR_SOCK];
NET_SOCK_EXT  NET_SOCK          *NetSock_PoolPtr;                       /* Ptr to pool of free socks.                   */
NET_SOCK_EXT  NET_STAT_POOL      NetSock_PoolStat;


NET_SOCK_EXT  NET_PORT_NBR       NetSock_RandomPortNbrQ[NET_SOCK_PORT_NBR_RANDOM_NBR];
NET_SOCK_EXT  NET_PORT_NBR       NetSock_RandomPortNbrQ_HeadIx;         /* Random port FIFO Q head ix.                  */
NET_SOCK_EXT  NET_PORT_NBR       NetSock_RandomPortNbrQ_TailIx;         /* Random port FIFO Q tail ix.                  */
NET_SOCK_EXT  NET_PORT_NBR_QTY   NetSock_RandomPortNbrQ_NbrUsed;        /* Random port FIFO Q nbr used.                 */


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                  NETWORK SOCKET DESCRIPTOR MACRO'S
*
* Description : Initialize, modify, & check network socket descriptor sets for multiplexed I/O functions.
*
* Argument(s) : desc_nbr    Socket descriptor number to initialize, modify, or check; when applicable.
*
*               pdesc_set   Pointer to a descriptor set.
*
* Return(s)   : Return values macro-dependent :
*
*                   none, for    network socket descriptor initialization & modification macro's.
*
*                   1,    if any network socket descriptor condition(s) satisfied.
*
*                   0,    otherwise.
*
*               See also 'net_bsd.h  BSD 4.x FILE DESCRIPTOR MACRO'S  Note #2a2'.
*
* Caller(s)   : Application.
*
*               These macro's are network protocol suite application interface (API) macro's & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) Ideally, network socket descriptor macro's ('NET_SOCK_DESC_???()') would be based
*                   on the BSD 4.x Layer's file descriptor macro ('FD_???()') definitions.  However, 
*                   since  BSD 4.x Layer application interface (API) is NOT guaranteed to be present 
*                   in the project build (see 'net_bsd.h  MODULE  Note #1bA'); the network socket 
*                   descriptor macro's MUST be independently defined.
*
*                   However, for correct interoperability between network socket descriptor macro's
*                   & BSD 4.x Layer file descriptor macro's; ANY modification to any of these macro 
*                   definitions MUST be appropriately synchronized.
*
*                   See also 'net_bsd.h  BSD 4.x FILE DESCRIPTOR MACRO'S  Note #3'.
*********************************************************************************************************
*/

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)

#define  NET_SOCK_DESC_COPY(pdesc_set_dest, pdesc_set_src)                                                                        \
                                  { if ((((NET_SOCK_DESC *)(pdesc_set_dest)) != (NET_SOCK_DESC *)0) &&                            \
                                        (((NET_SOCK_DESC *)(pdesc_set_src )) != (NET_SOCK_DESC *)0)) {                            \
                                        Mem_Copy((void     *)     (&(((NET_SOCK_DESC *)(pdesc_set_dest))->SockID_DescNbrSet[0])), \
                                                 (void     *)     (&(((NET_SOCK_DESC *)(pdesc_set_src ))->SockID_DescNbrSet[0])), \
                                                 (CPU_SIZE_T)(sizeof(((NET_SOCK_DESC *)(pdesc_set_dest))->SockID_DescNbrSet)));   \
                                    }                                                                                             }

#define  NET_SOCK_DESC_INIT(pdesc_set)                                                                                       \
                                  { if (((NET_SOCK_DESC *)(pdesc_set)) != (NET_SOCK_DESC *)0) {                              \
                                        Mem_Clr ((void     *)     (&(((NET_SOCK_DESC *)(pdesc_set))->SockID_DescNbrSet[0])), \
                                                 (CPU_SIZE_T)(sizeof(((NET_SOCK_DESC *)(pdesc_set))->SockID_DescNbrSet)));   \
                                    }                                                                                        }


#define  NET_SOCK_DESC_CLR(desc_nbr, pdesc_set)                                                                                                                                \
                                              { if (((desc_nbr) >= NET_SOCK_DESC_NBR_MIN) &&                                                                                   \
                                                    ((desc_nbr) <= NET_SOCK_DESC_NBR_MAX) &&                                                                                   \
                                                                    (((NET_SOCK_DESC *)(pdesc_set)) != (NET_SOCK_DESC *)0)) {                                                  \
                                                     DEF_BIT_CLR   ((((NET_SOCK_DESC *)(pdesc_set))->SockID_DescNbrSet[(desc_nbr) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)]), \
                                                     DEF_BIT                                                          ((desc_nbr) % (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS))); \
                                                }                                                                                                                              }

#define  NET_SOCK_DESC_SET(desc_nbr, pdesc_set)                                                                                                                                \
                                              { if (((desc_nbr) >= NET_SOCK_DESC_NBR_MIN) &&                                                                                   \
                                                    ((desc_nbr) <= NET_SOCK_DESC_NBR_MAX) &&                                                                                   \
                                                                    (((NET_SOCK_DESC *)(pdesc_set)) != (NET_SOCK_DESC *)0)) {                                                  \
                                                     DEF_BIT_SET   ((((NET_SOCK_DESC *)(pdesc_set))->SockID_DescNbrSet[(desc_nbr) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)]), \
                                                     DEF_BIT                                                          ((desc_nbr) % (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS))); \
                                                }                                                                                                                              }

#define  NET_SOCK_DESC_IS_SET(desc_nbr, pdesc_set)                                                                                                                             \
                                                  ((((desc_nbr) >= NET_SOCK_DESC_NBR_MIN) &&                                                                                   \
                                                    ((desc_nbr) <= NET_SOCK_DESC_NBR_MAX) &&                                                                                   \
                                                                    (((NET_SOCK_DESC *)(pdesc_set)) != (NET_SOCK_DESC *)0)) ?                                                  \
                                                  (((DEF_BIT_IS_SET((((NET_SOCK_DESC *)(pdesc_set))->SockID_DescNbrSet[(desc_nbr) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)]), \
                                                     DEF_BIT                                                          ((desc_nbr) % (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)))) \
                                                  == DEF_YES) ? 1 : 0)                                                                                                         \
                                                                  : 0)

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void                NetSock_Init            (NET_ERR                   *perr);


                                                                                    /* ----------- RX FNCTS ----------- */
void                NetSock_Rx              (NET_BUF                   *pbuf,
                                             NET_ERR                   *perr);


                                                                                    /* ----------- TX FNCTS ----------- */




                                                                                    /* -------- SOCK API FNCTS -------- */
NET_SOCK_ID         NetSock_Open            (NET_SOCK_PROTOCOL_FAMILY   protocol_family,
                                             NET_SOCK_TYPE              sock_type,
                                             NET_SOCK_PROTOCOL          protocol,
                                             NET_ERR                   *perr);


NET_SOCK_RTN_CODE   NetSock_Close           (NET_SOCK_ID                sock_id,
                                             NET_ERR                   *perr);

void                NetSock_CloseFromConn   (NET_SOCK_ID                sock_id);


void                NetSock_FreeConnFromSock(NET_SOCK_ID                sock_id,
                                             NET_CONN_ID                conn_id);



NET_SOCK_RTN_CODE   NetSock_Bind            (NET_SOCK_ID                sock_id,
                                             NET_SOCK_ADDR             *paddr_local,
                                             NET_SOCK_ADDR_LEN          addr_len,
                                             NET_ERR                   *perr);


NET_SOCK_RTN_CODE   NetSock_Conn            (NET_SOCK_ID                sock_id,
                                             NET_SOCK_ADDR             *paddr_remote,
                                             NET_SOCK_ADDR_LEN          addr_len,
                                             NET_ERR                   *perr);

#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void                NetSock_ConnSignalReq   (NET_SOCK_ID                sock_id,
                                             NET_ERR                   *perr);

void                NetSock_ConnSignalAccept(NET_SOCK_ID                sock_id,
                                             NET_CONN_ID                conn_id,
                                             NET_ERR                   *perr);

void                NetSock_ConnSignalClose (NET_SOCK_ID                sock_id,
                                             CPU_BOOLEAN                data_avail,
                                             NET_ERR                   *perr);


NET_SOCK_RTN_CODE   NetSock_Listen          (NET_SOCK_ID                sock_id,
                                             NET_SOCK_Q_SIZE            sock_q_size,
                                             NET_ERR                   *perr);

CPU_BOOLEAN         NetSock_ListenQ_IsAvail (NET_SOCK_ID                sock_id,
                                             NET_ERR                   *perr);


NET_SOCK_ID         NetSock_Accept          (NET_SOCK_ID                sock_id,
                                             NET_SOCK_ADDR             *paddr_remote,
                                             NET_SOCK_ADDR_LEN         *paddr_len,
                                             NET_ERR                   *perr);
#endif

/*$PAGE*/
NET_SOCK_RTN_CODE   NetSock_RxDataFrom      (NET_SOCK_ID                sock_id,
                                             void                      *pdata_buf,
                                             CPU_INT16U                 data_buf_len,
                                             CPU_INT16S                 flags,
                                             NET_SOCK_ADDR             *paddr_remote,
                                             NET_SOCK_ADDR_LEN         *paddr_len,
                                             void                      *pip_opts_buf,
                                             CPU_INT08U                 ip_opts_buf_len,
                                             CPU_INT08U                *pip_opts_len,
                                             NET_ERR                   *perr);

NET_SOCK_RTN_CODE   NetSock_RxData          (NET_SOCK_ID                sock_id,
                                             void                      *pdata_buf,
                                             CPU_INT16U                 data_buf_len,
                                             CPU_INT16S                 flags,
                                             NET_ERR                   *perr);


NET_SOCK_RTN_CODE   NetSock_TxDataTo        (NET_SOCK_ID                sock_id,
                                             void                      *p_data,
                                             CPU_INT16U                 data_len,
                                             CPU_INT16S                 flags,
                                             NET_SOCK_ADDR             *paddr_remote,
                                             NET_SOCK_ADDR_LEN          addr_len,
                                             NET_ERR                   *perr);

NET_SOCK_RTN_CODE   NetSock_TxData          (NET_SOCK_ID                sock_id,
                                             void                      *p_data,
                                             CPU_INT16U                 data_len,
                                             CPU_INT16S                 flags,
                                             NET_ERR                   *perr);



#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
NET_SOCK_RTN_CODE   NetSock_Sel             (NET_SOCK_QTY               sock_nbr_max,
                                             NET_SOCK_DESC             *psock_desc_rd,
                                             NET_SOCK_DESC             *psock_desc_wr,
                                             NET_SOCK_DESC             *psock_desc_err,
                                             NET_SOCK_TIMEOUT          *ptimeout,
                                             NET_ERR                   *perr);
#endif


/*$PAGE*/
                                                                                    /* -------- SOCK CFG FNCTS -------- */
                                                                                    /* Cfg dflt sock rx Q   timeout.    */
void                NetSock_CfgTimeoutRxQ_Dflt        (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);
                                                                                    /* Cfg/set  sock rx Q   timeout.    */
void                NetSock_CfgTimeoutRxQ_Set         (NET_SOCK_ID   sock_id,
                                                       CPU_INT32U    timeout_ms,
                                                       NET_ERR      *perr);
                                                                                    /* Get      sock rx Q   timeout.    */
CPU_INT32U          NetSock_CfgTimeoutRxQ_Get_ms      (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);


                                                                                    /* Cfg dflt sock tx Q   timeout.    */
void                NetSock_CfgTimeoutTxQ_Dflt        (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);
                                                                                    /* Cfg/set  sock tx Q   timeout.    */
void                NetSock_CfgTimeoutTxQ_Set         (NET_SOCK_ID   sock_id,
                                                       CPU_INT32U    timeout_ms,
                                                       NET_ERR      *perr);
                                                                                    /* Get      sock tx Q   timeout.    */
CPU_INT32U          NetSock_CfgTimeoutTxQ_Get_ms      (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);


#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                                                                                    /* Cfg dflt sock conn   timeout.    */
void                NetSock_CfgTimeoutConnReqDflt     (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);
                                                                                    /* Cfg/set  sock conn   timeout.    */
void                NetSock_CfgTimeoutConnReqSet      (NET_SOCK_ID   sock_id,
                                                       CPU_INT32U    timeout_ms,
                                                       NET_ERR      *perr);
                                                                                    /* Get      sock conn   timeout.    */
CPU_INT32U          NetSock_CfgTimeoutConnReqGet_ms   (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);


                                                                                    /* Cfg dflt sock accept timeout.    */
void                NetSock_CfgTimeoutConnAcceptDflt  (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);
                                                                                    /* Cfg/set  sock accept timeout.    */
void                NetSock_CfgTimeoutConnAcceptSet   (NET_SOCK_ID   sock_id,
                                                       CPU_INT32U    timeout_ms,
                                                       NET_ERR      *perr);
                                                                                    /* Get      sock accept timeout.    */
CPU_INT32U          NetSock_CfgTimeoutConnAcceptGet_ms(NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);


                                                                                    /* Cfg dflt sock close  timeout.    */
void                NetSock_CfgTimeoutConnCloseDflt   (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);
                                                                                    /* Cfg/set  sock close  timeout.    */
void                NetSock_CfgTimeoutConnCloseSet    (NET_SOCK_ID   sock_id,
                                                       CPU_INT32U    timeout_ms,
                                                       NET_ERR      *perr);
                                                                                    /* Get      sock close  timeout.    */
CPU_INT32U          NetSock_CfgTimeoutConnCloseGet_ms (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);
#endif



                                                                                    /* ------ SOCK STATUS FNCTS ------- */
CPU_BOOLEAN         NetSock_IsUsed                    (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);

CPU_BOOLEAN         NetSock_IsConn                    (NET_SOCK_ID   sock_id,
                                                       NET_ERR      *perr);


NET_STAT_POOL       NetSock_PoolStatGet               (void);

void                NetSock_PoolStatResetMaxUsed      (void);


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                      DEFINED IN OS'S  net_os.c
*********************************************************************************************************
*/

                                                                                    /* Create sock objs.                */
void                NetOS_Sock_Init                     (NET_ERR      *perr);



                                                                                    /* Clr      sock rx Q signal.       */
void                NetOS_Sock_RxQ_Clr                  (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Wait for sock rx Q signal.       */
void                NetOS_Sock_RxQ_Wait                 (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Signal   sock rx Q.              */
void                NetOS_Sock_RxQ_Signal               (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Abort    sock rx Q.              */
void                NetOS_Sock_RxQ_Abort                (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);

                                                                                    /* Set dflt sock rx Q timeout.      */
void                NetOS_Sock_RxQ_TimeoutDflt          (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Set      sock rx Q timeout.      */
void                NetOS_Sock_RxQ_TimeoutSet           (NET_SOCK_ID   sock_id,
                                                         CPU_INT32U    timeout_ms,
                                                         NET_ERR      *perr);
                                                                                    /* Get      sock rx Q timeout.      */
CPU_INT32U          NetOS_Sock_RxQ_TimeoutGet_ms        (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);



#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                                                                                    /* Clr      sock conn signal.       */
void                NetOS_Sock_ConnReqClr               (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Wait for sock conn signal.       */
void                NetOS_Sock_ConnReqWait              (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Signal   sock conn.              */
void                NetOS_Sock_ConnReqSignal            (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Abort    sock conn.              */
void                NetOS_Sock_ConnReqAbort             (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);

                                                                                    /* Set dflt sock conn timeout.      */
void                NetOS_Sock_ConnReqTimeoutDflt       (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Set      sock conn timeout.      */
void                NetOS_Sock_ConnReqTimeoutSet        (NET_SOCK_ID   sock_id,
                                                         CPU_INT32U    timeout_ms,
                                                         NET_ERR      *perr);
                                                                                    /* Get      sock conn timeout.      */
CPU_INT32U          NetOS_Sock_ConnReqTimeoutGet_ms     (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);


                                                                                    /* Clr      sock accept Q signal.   */
void                NetOS_Sock_ConnAcceptQ_Clr          (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Wait for sock accept Q signal.   */
void                NetOS_Sock_ConnAcceptQ_Wait         (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Signal   sock accept Q.          */
void                NetOS_Sock_ConnAcceptQ_Signal       (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Abort    sock accept Q wait.     */
void                NetOS_Sock_ConnAcceptQ_Abort        (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);

                                                                                    /* Set dflt sock accept Q timeout.  */
void                NetOS_Sock_ConnAcceptQ_TimeoutDflt  (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Set      sock accept Q timeout.  */
void                NetOS_Sock_ConnAcceptQ_TimeoutSet   (NET_SOCK_ID   sock_id,
                                                         CPU_INT32U    timeout_ms,
                                                         NET_ERR      *perr);
                                                                                    /* Get      sock accept Q timeout.  */
CPU_INT32U          NetOS_Sock_ConnAcceptQ_TimeoutGet_ms(NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);


/*$PAGE*/
                                                                                    /* Clr      sock close signal.      */
void                NetOS_Sock_ConnCloseClr             (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Wait for sock close signal.      */
void                NetOS_Sock_ConnCloseWait            (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Signal   sock close.             */
void                NetOS_Sock_ConnCloseSignal          (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Abort    sock close.             */
void                NetOS_Sock_ConnCloseAbort           (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);

                                                                                    /* Set dflt sock close timeout.     */
void                NetOS_Sock_ConnCloseTimeoutDflt     (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
                                                                                    /* Set      sock close timeout.     */
void                NetOS_Sock_ConnCloseTimeoutSet      (NET_SOCK_ID   sock_id,
                                                         CPU_INT32U    timeout_ms,
                                                         NET_ERR      *perr);
                                                                                    /* Get      sock close timeout.     */
CPU_INT32U          NetOS_Sock_ConnCloseTimeoutGet_ms   (NET_SOCK_ID   sock_id,
                                                         NET_ERR      *perr);
#endif



#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
                                                                                    /* Wait for rdy sock sel events.    */
NET_SOCK_EVENT_QTY  NetOS_Sock_SelWait(NET_SOCK_EVENT_QTY   sock_events_nbr_cfgd,
                                       NET_SOCK_EVENT_TBL  *psock_events_cfg,
                                       NET_SOCK_EVENT_TBL  *psock_events_rtn,
                                       CPU_INT32U           timeout_sec,
                                       CPU_INT32U           timeout_us,
                                       NET_ERR             *perr);
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_SOCK_CFG_FAMILY
#error  "NET_SOCK_CFG_FAMILY                        not #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  NET_SOCK_FAMILY_IP_V4]  "

#elif   (NET_SOCK_CFG_FAMILY != NET_SOCK_FAMILY_IP_V4)
#error  "NET_SOCK_CFG_FAMILY                  illegally #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  NET_SOCK_FAMILY_IP_V4]  "


#else

                                                            /* Correctly configured in 'net_cfg_net.h'; DO NOT MODIFY.  */
#ifndef  NET_SOCK_CFG_ADDR_LEN
#error  "NET_SOCK_CFG_ADDR_LEN                      not #define'd in 'net_cfg_net.h'"
#error  "                                     [MUST be  NET_SOCK_ADDR_LEN_IP_V4]    "

#elif   (NET_SOCK_CFG_ADDR_LEN != NET_SOCK_ADDR_LEN_IP_V4)
#error  "NET_SOCK_CFG_ADDR_LEN                illegally #define'd in 'net_cfg_net.h'"
#error  "                                     [MUST be  NET_SOCK_ADDR_LEN_IP_V4]    "
#endif


#ifndef  NET_SOCK_CFG_PROTOCOL_MAX
#error  "NET_SOCK_CFG_PROTOCOL_MAX                  not #define'd in 'net_cfg_net.h'   "
#error  "                                     [MUST be  >  0                          ]"
#error  "                                     [     &&  <= NET_SOCK_PROTOCOL_MAX_IP_V4]"

#elif  ((NET_SOCK_CFG_PROTOCOL_MAX < 1                          ) || \
        (NET_SOCK_CFG_PROTOCOL_MAX > NET_SOCK_PROTOCOL_MAX_IP_V4))
#error  "NET_SOCK_CFG_PROTOCOL_MAX            illegally #define'd in 'net_cfg_net.h'   "
#error  "                                     [MUST be  >  0                          ]"
#error  "                                     [     &&  <= NET_SOCK_PROTOCOL_MAX_IP_V4]"
#endif


#endif




#ifndef  NET_SOCK_CFG_NBR_SOCK
#error  "NET_SOCK_CFG_NBR_SOCK                      not #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  >= NET_SOCK_NBR_MIN    ]"
#error  "                                     [     &&  <= NET_SOCK_NBR_MAX    ]"

#elif  ((NET_SOCK_CFG_NBR_SOCK < NET_SOCK_NBR_MIN) || \
        (NET_SOCK_CFG_NBR_SOCK > NET_SOCK_NBR_MAX))
#error  "NET_SOCK_CFG_NBR_SOCK                illegally #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  >= NET_SOCK_NBR_MIN    ]"
#error  "                                     [     &&  <= NET_SOCK_NBR_MAX    ]"
#endif




#ifndef  NET_SOCK_CFG_BLOCK_SEL
#error  "NET_SOCK_CFG_BLOCK_SEL                     not #define'd in 'net_cfg.h'    "
#error  "                                     [MUST be  NET_SOCK_BLOCK_SEL_DFLT    ]"
#error  "                                     [     ||  NET_SOCK_BLOCK_SEL_BLOCK   ]"
#error  "                                     [     ||  NET_SOCK_BLOCK_SEL_NO_BLOCK]"

#elif  ((NET_SOCK_CFG_BLOCK_SEL != NET_SOCK_BLOCK_SEL_DFLT    ) && \
        (NET_SOCK_CFG_BLOCK_SEL != NET_SOCK_BLOCK_SEL_BLOCK   ) && \
        (NET_SOCK_CFG_BLOCK_SEL != NET_SOCK_BLOCK_SEL_NO_BLOCK))
#error  "NET_SOCK_CFG_BLOCK_SEL               illegally #define'd in 'net_cfg.h'    "
#error  "                                     [MUST be  NET_SOCK_BLOCK_SEL_DFLT    ]"
#error  "                                     [     ||  NET_SOCK_BLOCK_SEL_BLOCK   ]"
#error  "                                     [     ||  NET_SOCK_BLOCK_SEL_NO_BLOCK]"
#endif




/*$PAGE*/
#ifndef  NET_SOCK_CFG_SEL_EN
#error  "NET_SOCK_CFG_SEL_EN                        not #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  DEF_DISABLED]           "
#error  "                                     [     ||  DEF_ENABLED ]           "

#elif  ((NET_SOCK_CFG_SEL_EN != DEF_DISABLED) && \
        (NET_SOCK_CFG_SEL_EN != DEF_ENABLED ))
#error  "NET_SOCK_CFG_SEL_EN                  illegally #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  DEF_DISABLED]           "
#error  "                                     [     ||  DEF_ENABLED ]           "


#elif   (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)

#ifndef  NET_SOCK_CFG_SEL_NBR_EVENTS_MAX
#error  "NET_SOCK_CFG_SEL_NBR_EVENTS_MAX            not #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_OS_NBR_EVENTS_MIN]"
#error  "                                     [     &&  <= NET_OS_NBR_EVENTS_MAX]"

#elif  ((NET_SOCK_CFG_SEL_NBR_EVENTS_MAX < NET_OS_NBR_EVENTS_MIN) || \
        (NET_SOCK_CFG_SEL_NBR_EVENTS_MAX > NET_OS_NBR_EVENTS_MAX))
#error  "NET_SOCK_CFG_SEL_NBR_EVENTS_MAX      illegally #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_OS_NBR_EVENTS_MIN]"
#error  "                                     [     &&  <= NET_OS_NBR_EVENTS_MAX]"
#endif

#endif



                                                                /* Correctly configured in 'net_sock.h'; DO NOT MODIFY. */
#ifndef  NET_SOCK_CFG_TYPE_STREAM_EN
#error  "NET_SOCK_CFG_TYPE_STREAM_EN                not #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  DEF_DISABLED]           "
#error  "                                     [     ||  DEF_ENABLED ]           "

#elif  ((NET_SOCK_CFG_TYPE_STREAM_EN != DEF_DISABLED) && \
        (NET_SOCK_CFG_TYPE_STREAM_EN != DEF_ENABLED ))
#error  "NET_SOCK_CFG_TYPE_STREAM_EN          illegally #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  DEF_DISABLED]           "
#error  "                                     [     ||  DEF_ENABLED ]           "


#elif   (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)

#ifndef  NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX
#error  "NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX        not #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  >= NET_SOCK_Q_SIZE_MIN] "
#error  "                                     [     &&  <= NET_SOCK_Q_SIZE_MAX] "

#elif  ((NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX < NET_SOCK_Q_SIZE_MIN) || \
        (NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX > NET_SOCK_Q_SIZE_MAX))
#error  "NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX  illegally #define'd in 'net_cfg.h'"
#error  "                                     [MUST be  >= NET_SOCK_Q_SIZE_MIN] "
#error  "                                     [     &&  <= NET_SOCK_Q_SIZE_MAX] "
#endif

#endif




#ifndef  NET_SOCK_CFG_PORT_NBR_RANDOM_BASE
#error  "NET_SOCK_CFG_PORT_NBR_RANDOM_BASE          not #define'd in 'net_cfg.h'                                                                    "
#error  "                                     [MUST be  >=   NET_PORT_NBR_MIN                                                                      ]"
#error  "                                     [     &&  <= ((NET_PORT_NBR_MAX + 1) - (NET_SOCK_CFG_NBR_SOCK + NET_SOCK_PORT_NBR_RANDOM_NBR_OFFSET))]"

#elif  ((NET_SOCK_CFG_PORT_NBR_RANDOM_BASE < NET_PORT_NBR_MIN) || \
        (NET_SOCK_PORT_NBR_RANDOM_MAX      > NET_PORT_NBR_MAX))
#error  "NET_SOCK_CFG_PORT_NBR_RANDOM_BASE    illegally #define'd in 'net_cfg.h'                                                                    "
#error  "                                     [MUST be  >=   NET_PORT_NBR_MIN                                                                      ]"
#error  "                                     [     &&  <= ((NET_PORT_NBR_MAX + 1) - (NET_SOCK_CFG_NBR_SOCK + NET_SOCK_PORT_NBR_RANDOM_NBR_OFFSET))]"
#endif


                                                                /* Correctly configured in 'net_sock.h'; DO NOT MODIFY. */
#ifndef  NET_SOCK_PORT_NBR_RANDOM_NBR
#error  "NET_SOCK_PORT_NBR_RANDOM_NBR               not #define'd in 'net_sock.h'"
#error  "                                     [MUST be  >= NET_PORT_NBR_MIN]     "
#error  "                                     [     &&  <= NET_PORT_NBR_MAX]     "

#elif  ((NET_SOCK_PORT_NBR_RANDOM_NBR < NET_PORT_NBR_MIN) || \
        (NET_SOCK_PORT_NBR_RANDOM_NBR > NET_PORT_NBR_MAX))
#error  "NET_SOCK_PORT_NBR_RANDOM_NBR         illegally #define'd in 'net_sock.h'"
#error  "                                     [MUST be  >= NET_PORT_NBR_MIN]     "
#error  "                                     [     &&  <= NET_PORT_NBR_MAX]     "
#endif




/*$PAGE*/
#ifndef  NET_SOCK_CFG_TIMEOUT_RX_Q_MS
#error  "NET_SOCK_CFG_TIMEOUT_RX_Q_MS               not #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                     [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"

#elif (((NET_SOCK_CFG_TIMEOUT_RX_Q_MS <  NET_OS_TIMEOUT_MIN_mS)  || \
        (NET_SOCK_CFG_TIMEOUT_RX_Q_MS >  NET_OS_TIMEOUT_MAX_mS)) && \
        (NET_SOCK_CFG_TIMEOUT_RX_Q_MS != NET_TMR_TIME_INFINITE))
#error  "NET_SOCK_CFG_TIMEOUT_RX_Q_MS         illegally #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                     [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"
#endif



#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)

#ifndef  NET_SOCK_CFG_TIMEOUT_CONN_REQ_MS
#error  "NET_SOCK_CFG_TIMEOUT_CONN_REQ_MS           not #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                     [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"

#elif (((NET_SOCK_CFG_TIMEOUT_CONN_REQ_MS <  NET_OS_TIMEOUT_MIN_mS)  || \
        (NET_SOCK_CFG_TIMEOUT_CONN_REQ_MS >  NET_OS_TIMEOUT_MAX_mS)) && \
        (NET_SOCK_CFG_TIMEOUT_CONN_REQ_MS != NET_TMR_TIME_INFINITE))
#error  "NET_SOCK_CFG_TIMEOUT_CONN_REQ_MS     illegally #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                     [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"
#endif



#ifndef  NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_MS
#error  "NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_MS        not #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                     [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"

#elif (((NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_MS <  NET_OS_TIMEOUT_MIN_mS)  || \
        (NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_MS >  NET_OS_TIMEOUT_MAX_mS)) && \
        (NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_MS != NET_TMR_TIME_INFINITE))
#error  "NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_MS  illegally #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                     [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"
#endif



#ifndef  NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_MS
#error  "NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_MS         not #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                     [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"

#elif (((NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_MS <  NET_OS_TIMEOUT_MIN_mS)  || \
        (NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_MS >  NET_OS_TIMEOUT_MAX_mS)) && \
        (NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_MS != NET_TMR_TIME_INFINITE))
#error  "NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_MS   illegally #define'd in 'net_cfg.h' "
#error  "                                     [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                     [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                     [     ||  == NET_TMR_TIME_INFINITE]"
#endif

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_sock.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of net sock module include (see Note #1).        */

