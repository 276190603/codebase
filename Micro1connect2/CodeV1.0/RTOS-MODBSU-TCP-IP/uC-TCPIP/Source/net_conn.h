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
*                                    NETWORK CONNECTION MANAGEMENT
*
* Filename      : net_conn.h
* Version       : V1.92
* Programmer(s) : ITJ
*                 SR
*********************************************************************************************************
* Note(s)       : (1) Supports network connections for local & remote addresses of the following :
*
*                     (a) Families :
*                         (1) IPv4 Connections
*                             (A) BSD 4.x Sockets
*
*                     (b) Connection types :
*                         (1) Datagram
*                         (2) Stream
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) Network Connection module is required for :
*
*               (a) IPv4 Connections
*                   (1) BSD 4.x Sockets
*
*               See also 'net_conn.h  Note #1'.
*
*           (2) The following connection-module-present configuration value MUST be pre-#define'd in 
*               'net_cfg_net.h' PRIOR to all other network modules that require Network Connection 
*               Management Module configuration (see 'net_cfg_net.h  NETWORK CONNECTION MANAGEMENT 
*               CONFIGURATION  Note #2b') :
*
*                   NET_CONN_MODULE_PRESENT
*********************************************************************************************************
*/

#ifdef   NET_CONN_MODULE_PRESENT                                /* See Note #2.                                         */


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_CONN_MODULE
#define  NET_CONN_EXT
#else
#define  NET_CONN_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  NET_CONN_ACCESSED_TH_MIN                         10u
#define  NET_CONN_ACCESSED_TH_MAX                      65000u
#define  NET_CONN_ACCESSED_TH_DFLT                       100u


#define  NET_CONN_CLOSE_FULL                               0
#define  NET_CONN_CLOSE_HALF                               1    /* Conn closed to tx's; open to rx's.                   */


/*
*********************************************************************************************************
*                              NETWORK CONNECTION PROTOCOL INDEX DEFINES
*********************************************************************************************************
*/

#if     (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
#define  NET_CONN_PROTOCOL_IX_IP_V4_UDP                    0
#define  NET_CONN_PROTOCOL_IX_IP_V4_TCP                    1
#endif


/*
*********************************************************************************************************
*                                 NETWORK CONNECTION ADDRESS DEFINES
*********************************************************************************************************
*/

#if     (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)

#define  NET_CONN_ADDR_IP_LEN_PORT                       NET_SOCK_ADDR_IP_LEN_PORT
#define  NET_CONN_ADDR_IP_LEN_ADDR                       NET_SOCK_ADDR_IP_LEN_ADDR

#define  NET_CONN_ADDR_IP_IX_BASE                        NET_SOCK_ADDR_IP_IX_BASE
#define  NET_CONN_ADDR_IP_IX_PORT                        NET_SOCK_ADDR_IP_IX_PORT
#define  NET_CONN_ADDR_IP_IX_ADDR                        NET_SOCK_ADDR_IP_IX_ADDR


#define  NET_CONN_ADDR_IP_WILD_CARD                      NET_SOCK_ADDR_IP_WILD_CARD
#define  NET_CONN_ADDR_IP_BROADCAST                      NET_SOCK_ADDR_IP_BROADCAST

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NETWORK CONNECTION FLAG DEFINES
*********************************************************************************************************
*/

                                                                /* ------------------ NET CONN FLAGS ------------------ */
#define  NET_CONN_FLAG_NONE                       DEF_BIT_NONE
#define  NET_CONN_FLAG_USED                       DEF_BIT_00    /* Conn cur used; i.e. NOT in free conn pool.           */


/*
*********************************************************************************************************
*                                   NETWORK CONNECTION TYPE DEFINES
*
* Note(s) : (1) NET_CONN_TYPE_??? #define values specifically chosen as ASCII representations of the
*               network connection types.  Memory displays of network connections will display the 
*               network connection TYPEs with their chosen ASCII names.
*********************************************************************************************************
*/

                                                                /* ------------------ NET CONN TYPES ------------------ */
#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  NET_CONN_TYPE_NONE                       0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_CONN_TYPE_CONN                       0x434F4E4E    /* "CONN" in ASCII.                                     */

#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)
#define  NET_CONN_TYPE_NONE                       0x454E4F4E    /* "NONE" in ASCII.                                     */
#define  NET_CONN_TYPE_CONN                       0x4E4E4F43    /* "CONN" in ASCII.                                     */

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  NET_CONN_TYPE_NONE                       0x4F4E454E    /* "NONE" in ASCII.                                     */
#define  NET_CONN_TYPE_CONN                       0x4F434E4E    /* "CONN" in ASCII.                                     */

#else                                                           /* Dflt CPU_WORD_SIZE_08.                               */
#define  NET_CONN_TYPE_NONE                       0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_CONN_TYPE_CONN                       0x434F4E4E    /* "CONN" in ASCII.                                     */

#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                 NETWORK CONNECTION FAMILY DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT08U  NET_CONN_FAMILY;


/*
*********************************************************************************************************
*                               NETWORK CONNECTION LIST INDEX DATA TYPE
*
* Note(s) : (1) NET_CONN_PROTOCOL_IX_NONE  SHOULD be #define'd based on 'NET_CONN_PROTOCOL_IX' data type
*               declared.
*********************************************************************************************************
*/

typedef  CPU_INT08U  NET_CONN_PROTOCOL_IX;

#define  NET_CONN_PROTOCOL_IX_NONE      DEF_INT_08U_MAX_VAL     /* Define as max unsigned val (see Note #1).            */


/*
*********************************************************************************************************
*                             NETWORK CONNECTION ADDRESS LENGTH DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_CONN_ADDR_LEN;


/*
*********************************************************************************************************
*                             NETWORK CONNECTION LIST QUANTITY DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT08U  NET_CONN_LIST_QTY;                         /* Defines max qty of conn lists to support.            */


/*$PAGE*/
/*
*********************************************************************************************************
*                                NETWORK CONNECTION QUANTITY DATA TYPE
*
* Note(s) : (1) See also 'NETWORK CONNECTION IDENTIFICATION DATA TYPE  Note #2b'.
*********************************************************************************************************
*/

typedef  CPU_INT16S  NET_CONN_QTY;                              /* Defines max qty of conns      to support.            */


/*
*********************************************************************************************************
*                             NETWORK CONNECTION IDENTIFICATION DATA TYPE
*
* Note(s) : (1) The following network connection data types are pre-defined in 'net_type.h' PRIOR to all 
*               other network modules that require network connection data types :
*
*               (a) NET_CONN_ID
*
*           (2) (a) The following network connection values are pre-#define'd in 'net_cfg_net.h' PRIOR 
*                   to 'net_conn.h' based on other modules' configuration values :
*
*                   (1) NET_CONN_NBR_MIN
*
*               (b) (1) NET_CONN_NBR_MAX  SHOULD be #define'd based on 'NET_CONN_QTY' data type declared.
*
*                   (2) However, since network connection handle identifiers are data-typed as 16-bit 
*                       signed integers; the maximum number of valid network connection identifiers, & 
*                       therefore the maximum number of valid network connections, is the total number 
*                       of non-negative values that 16-bit signed integers support.
*********************************************************************************************************
*/

#if 0                                                           /* See Note #1a.                                        */
typedef  CPU_INT16S  NET_CONN_ID;
#endif

#if 0                                                           /* See Note #2a1.                                       */
#define  NET_CONN_NBR_MIN                                  1
#endif
#define  NET_CONN_NBR_MAX                DEF_INT_16S_MAX_VAL    /* See Note #2b.                                        */

#define  NET_CONN_ID_NONE                                 -1
#define  NET_CONN_ID_MIN                                   0
#define  NET_CONN_ID_MAX          (NET_CONN_CFG_NBR_CONN - 1)


/*
*********************************************************************************************************
*                               NETWORK CONNECTION LIST INDEX DATA TYPE
*********************************************************************************************************
*/

typedef  NET_CONN_QTY  NET_CONN_LIST_IX;

#define  NET_CONN_LIST_IX_NONE                            -1
#define  NET_CONN_LIST_IX_MIN                              0
#define  NET_CONN_LIST_IX_MAX     (NET_CONN_CFG_NBR_CONN - 1)


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NETWORK CONNECTION DATA TYPE
*
*                                             NET_CONN
*                                          |-------------|
*                                          |  Conn Type  |
*                            Previous      |-------------|
*                           Connection <----------O      |
*                           Chain Lists    |-------------|        Next
*                                          |      O----------> Connection
*                            Previous      |-------------|     Chain Lists
*                           Connection <----------O      |
*                                          |-------------|        Next
*                                          |      O----------> Connection
*                                          |-------------|            
*                                          |      O----------> Connection
*                                          |-------------|        List
*                                          |  Conn IDs   |
*                                          |-------------|
*                                          | Connection  |
*                                          |   Family    |
*                                          |  Protocol   |
*                                          |-------------|
*                                          |    Local    |
*                                          |   Address   |
*                                          |-------------|
*                                          |   Remote    |
*                                          |   Address   |
*                                          |-------------|
*                                          |  Accessed   |
*                                          |  Counters   |
*                                          |-------------|
*                                          |    Flags    |
*                                          |-------------|
*
*
* Note(s) : (1) Forced word-alignment at start of network connection NOT required since first data member
*               'Type' is declared as 'CPU_INT32U'.
*********************************************************************************************************
*/

                                                                /* --------------------- NET CONN --------------------- */
struct  net_conn {
    NET_TYPE                Type;                               /* Type cfg'd @ init : NET_CONN_TYPE_CONN.              */

    NET_CONN               *PrevChainPtr;                       /* Ptr to PREV conn chain list.                         */
    NET_CONN               *NextChainPtr;                       /* Ptr to NEXT conn chain list.                         */

    NET_CONN               *PrevConnPtr;                        /* Ptr to PREV conn.                                    */
    NET_CONN               *NextConnPtr;                        /* Ptr to NEXT conn.                                    */

    NET_CONN              **ConnList;                           /* Conn list.                                           */


    NET_CONN_ID             ID;                                 /* Net             conn id.                             */
    NET_CONN_ID             ID_App;                             /* App       layer conn id.                             */
    NET_CONN_ID             ID_AppClone;                        /* App       layer conn id clone.                       */
    NET_CONN_ID             ID_Transport;                       /* Transport layer conn id.                             */


    NET_CONN_FAMILY         Family;                             /* Conn family.                                         */
    NET_CONN_PROTOCOL_IX    ProtocolIx;                         /* Conn protocol ix.                                    */


    CPU_INT08U              AddrLocal[NET_CONN_CFG_ADDR_LEN];   /* Conn local  addr.                                    */
    CPU_BOOLEAN             AddrLocalValid;                     /* Conn local  addr valid flag.                         */

    CPU_INT08U              AddrRemote[NET_CONN_CFG_ADDR_LEN];  /* Conn remote addr.                                    */
    CPU_BOOLEAN             AddrRemoteValid;                    /* Conn remote addr valid flag.                         */


    CPU_INT16U              ConnChainAccessedCtr;               /* Nbr conn chain head accesses.                        */
    CPU_INT16U              ConnAccessedCtr;                    /* Nbr conn            accesses (non-chain head).       */

    
    CPU_INT16U              Flags;                              /* Conn flags.                                          */
};


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

NET_CONN_EXT  NET_CONN        NetConn_Tbl[NET_CONN_CFG_NBR_CONN];
NET_CONN_EXT  NET_CONN       *NetConn_PoolPtr;                          /* Ptr to pool of free net conns.               */
NET_CONN_EXT  NET_STAT_POOL   NetConn_PoolStat;

                                                                        /* Array of ptrs to net conn lists.             */
NET_CONN_EXT  NET_CONN       *NetConn_ConnListHead[NET_CONN_CFG_PROTOCOL_MAX];

NET_CONN_EXT  NET_CONN       *NetConn_ConnListChainPtr;                 /* Ptr to cur  conn list chain to update.       */
NET_CONN_EXT  NET_CONN       *NetConn_ConnListConnPtr;                  /* Ptr to cur  conn list conn  to update.       */
NET_CONN_EXT  NET_CONN       *NetConn_ConnListNextChainPtr;             /* Ptr to next conn list chain to update.       */
NET_CONN_EXT  NET_CONN       *NetConn_ConnListNextConnPtr;              /* Ptr to next conn list conn  to update.       */


NET_CONN_EXT  CPU_INT16U      NetConn_AccessedTh_nbr;                   /* Nbr successful srch's to promote net conns.  */

                                                                        /* Conn addr wildcard cfg.                      */
NET_CONN_EXT  CPU_BOOLEAN     NetConn_AddrWildCardAvail;
NET_CONN_EXT  CPU_INT08U      NetConn_AddrWildCard[NET_CONN_CFG_ADDR_LEN];


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void                NetConn_Init                (void);



                                                                                /* ------------ CFG FNCTS ------------- */
CPU_BOOLEAN         NetConn_CfgAccessedTh       (CPU_INT16U             nbr_access);



                                                                                /* ---------- CONN API FNCTS ---------- */
NET_CONN_ID         NetConn_Get                 (NET_CONN_FAMILY        family,
                                                 NET_CONN_PROTOCOL_IX   protocol_ix,
                                                 NET_ERR               *perr);

void                NetConn_Free                (NET_CONN_ID            conn_id);



void                NetConn_CloseFromApp        (NET_CONN_ID            conn_id,
                                                 CPU_BOOLEAN            close_conn_transport);

void                NetConn_CloseFromTransport  (NET_CONN_ID            conn_id,
                                                 CPU_BOOLEAN            close_conn_app);


void                NetConn_CloseAllConns       (void);

void                NetConn_CloseAllConnsHandler(void);




NET_CONN_ID         NetConn_ID_AppGet           (NET_CONN_ID            conn_id,
                                                 NET_ERR               *perr);

void                NetConn_ID_AppSet           (NET_CONN_ID            conn_id,
                                                 NET_CONN_ID            conn_id_app,
                                                 NET_ERR               *perr);


NET_CONN_ID         NetConn_ID_AppCloneGet      (NET_CONN_ID            conn_id,
                                                 NET_ERR               *perr);

void                NetConn_ID_AppCloneSet      (NET_CONN_ID            conn_id,
                                                 NET_CONN_ID            conn_id_app,
                                                 NET_ERR               *perr);


NET_CONN_ID         NetConn_ID_TransportGet     (NET_CONN_ID            conn_id,
                                                 NET_ERR               *perr);

void                NetConn_ID_TransportSet     (NET_CONN_ID            conn_id,
                                                 NET_CONN_ID            conn_id_transport,
                                                 NET_ERR               *perr);



void                NetConn_AddrLocalGet        (NET_CONN_ID            conn_id,
                                                 CPU_INT08U            *paddr_local,
                                                 NET_CONN_ADDR_LEN     *paddr_len,
                                                 NET_ERR               *perr);

void                NetConn_AddrLocalSet        (NET_CONN_ID            conn_id,
                                                 CPU_INT08U            *paddr_local,
                                                 NET_CONN_ADDR_LEN      addr_len,
                                                 CPU_BOOLEAN            addr_over_wr,
                                                 NET_ERR               *perr);


void                NetConn_AddrRemoteGet       (NET_CONN_ID            conn_id,
                                                 CPU_INT08U            *paddr_remote,
                                                 NET_CONN_ADDR_LEN     *paddr_len,
                                                 NET_ERR               *perr);

void                NetConn_AddrRemoteSet       (NET_CONN_ID            conn_id,
                                                 CPU_INT08U            *paddr_remote,
                                                 NET_CONN_ADDR_LEN      addr_len,
                                                 CPU_BOOLEAN            addr_over_wr,
                                                 NET_ERR               *perr);

CPU_BOOLEAN         NetConn_AddrRemoteCmp       (NET_CONN_ID            conn_id,
                                                 CPU_INT08U            *paddr_remote,
                                                 NET_CONN_ADDR_LEN      addr_len,
                                                 NET_ERR               *perr);



/*$PAGE*/
                                                                                /* -------- CONN STATUS FNCTS --------- */
CPU_BOOLEAN         NetConn_IsUsed              (NET_CONN_ID            conn_id,
                                                 NET_ERR               *perr);

void                NetConn_IsConn              (NET_CONN_ID            conn_id,
                                                 NET_ERR               *perr);


NET_STAT_POOL       NetConn_PoolStatGet         (void);

void                NetConn_PoolStatResetMaxUsed(void);



                                                                                /* --------- CONN LIST FNCTS ---------- */
NET_CONN_ID         NetConn_Srch                (NET_CONN_FAMILY        family,
                                                 NET_CONN_PROTOCOL_IX   protocol_ix,
                                                 CPU_INT08U            *paddr_local,
                                                 CPU_INT08U            *paddr_remote,
                                                 NET_CONN_ADDR_LEN      addr_len,
                                                 NET_CONN_ID           *pconn_id_transport,
                                                 NET_CONN_ID           *pconn_id_app,
                                                 NET_ERR               *perr);


void                NetConn_ListAdd             (NET_CONN_ID            conn_id,
                                                 NET_ERR               *perr);

void                NetConn_ListUnlink          (NET_CONN_ID            conn_id,
                                                 NET_ERR               *perr);


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_CONN_CFG_FAMILY
#error  "NET_CONN_CFG_FAMILY              not #define'd in 'net_cfg.h'   "
#error  "                           [MUST be  NET_CONN_FAMILY_IP_V4_SOCK]"



#elif   (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
                                                            /* Correctly configured in 'net_cfg_net.h'; DO NOT MODIFY.  */
#ifndef  NET_CONN_CFG_PROTOCOL_MAX
#error  "NET_CONN_CFG_PROTOCOL_MAX        not #define'd in 'net_cfg_net.h'        "
#error  "                           [MUST be  >= NET_SOCK_PROTOCOL_MAX_MIN       ]"
#error  "                           [     &&  <= NET_CONN_PROTOCOL_MAX_IP_V4_SOCK]"
#elif  ((NET_CONN_CFG_PROTOCOL_MAX < NET_SOCK_PROTOCOL_MAX_MIN       ) || \
        (NET_CONN_CFG_PROTOCOL_MAX > NET_CONN_PROTOCOL_MAX_IP_V4_SOCK))
#error  "NET_CONN_CFG_PROTOCOL_MAX  illegally #define'd in 'net_cfg_net.h'        "
#error  "                           [MUST be  >= NET_SOCK_PROTOCOL_MAX_MIN       ]"
#error  "                           [     &&  <= NET_CONN_PROTOCOL_MAX_IP_V4_SOCK]"
#endif


#ifndef  NET_CONN_CFG_ADDR_LEN
#error  "NET_CONN_CFG_ADDR_LEN            not #define'd in 'net_cfg_net.h' "
#error  "                           [MUST be  NET_CONN_ADDR_LEN_IP_V4_SOCK]"
#elif   (NET_CONN_CFG_ADDR_LEN != NET_CONN_ADDR_LEN_IP_V4_SOCK)
#error  "NET_CONN_CFG_ADDR_LEN      illegally #define'd in 'net_cfg_net.h' "
#error  "                           [MUST be  NET_CONN_ADDR_LEN_IP_V4_SOCK]"
#endif



#else
#error  "NET_CONN_CFG_FAMILY        illegally #define'd in 'net_cfg.h'   "
#error  "                           [MUST be  NET_CONN_FAMILY_IP_V4_SOCK]"

#endif




#ifndef  NET_CONN_CFG_NBR_CONN
#error  "NET_CONN_CFG_NBR_CONN            not #define'd in 'net_cfg.h'"
#error  "                           [MUST be  >= NET_CONN_NBR_MIN]    "
#error  "                           [     &&  <= NET_CONN_NBR_MAX]    "
#elif  ((NET_CONN_CFG_NBR_CONN < NET_CONN_NBR_MIN) || \
        (NET_CONN_CFG_NBR_CONN > NET_CONN_NBR_MAX))
#error  "NET_CONN_CFG_NBR_CONN      illegally #define'd in 'net_cfg.h'"
#error  "                           [MUST be  >= NET_CONN_NBR_MIN]    "
#error  "                           [     &&  <= NET_CONN_NBR_MAX]    "
#endif


/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_conn.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of net conn module include (see Note #1).        */

