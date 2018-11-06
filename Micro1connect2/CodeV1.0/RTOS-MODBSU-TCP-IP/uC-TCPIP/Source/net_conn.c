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
* Filename      : net_conn.c
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
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_CONN_MODULE
#include  <net.h>


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) See 'net_conn.h  MODULE'.
*********************************************************************************************************
*/

#ifdef  NET_CONN_MODULE_PRESENT


/*$PAGE*/
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  NET_CONN  *NetConn_ListSrch      (NET_CONN_FAMILY    family,
                                          NET_CONN         **pconn_list,
                                          CPU_INT08U        *paddr_local); 


static  NET_CONN  *NetConn_ChainSrch     (NET_CONN         **pconn_list,
                                          NET_CONN          *pconn_chain,
                                          CPU_INT08U        *paddr_local,
                                          CPU_INT08U        *paddr_wildcard,
                                          CPU_INT08U        *paddr_remote,
                                          NET_ERR           *perr);


static  void       NetConn_ChainInsert   (NET_CONN         **pconn_list,
                                          NET_CONN          *pconn_chain);

static  void       NetConn_ChainUnlink   (NET_CONN         **pconn_list,
                                          NET_CONN          *pconn_chain);


static  void       NetConn_Add           (NET_CONN         **pconn_list,
                                          NET_CONN          *pconn_chain,
                                          NET_CONN          *pconn);

static  void       NetConn_Unlink        (NET_CONN          *pconn);



static  void       NetConn_Close         (NET_CONN          *pconn);

static  void       NetConn_CloseApp      (NET_CONN          *pconn);

static  void       NetConn_CloseTransport(NET_CONN          *pconn);



static  void       NetConn_FreeHandler   (NET_CONN          *pconn);


static  void       NetConn_Clr           (NET_CONN          *pconn);

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void       NetConn_Discard       (NET_CONN          *pconn);
#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetConn_Init()
*
* Description : (1) Initialize Network Connection Management Module :
*
*                   (a) Initialize network connection pool
*                   (b) Initialize network connection table
*                   (c) Initialize network connection lists
*                   (d) Initialize network connection wildcard address(s)
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) Network connection pool MUST be initialized PRIOR to initializing the pool with 
*                   pointers to network connections.
*
*               (3) Network connection  addresses maintained in network-order.  Therefore, network 
*                   connection wildcard address(s) MUST be configured in network-order.
*
*               (4) Ideally, network connection address configuration comparison(s) would be calculated 
*                   at compile-time.  However, some compilers do NOT allow pre-processor directives to 
*                   include run-time macro's -- e.g. 'sizeof()'.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetConn_Init (void)
{
    NET_CONN            *pconn;
    NET_CONN           **pconn_list;
    NET_CONN_QTY         i;
    NET_CONN_LIST_QTY    j;
    CPU_INT16U           k;
    CPU_INT16U           ix;
    CPU_INT16U           val_len;
    CPU_INT08U           val;
    NET_ERR              stat_err;


                                                                /* ------------- INIT NET CONN POOL/STATS ------------- */
    NetConn_PoolPtr = (NET_CONN *)0;                            /* Init-clr net conn pool (see Note #2).                */

    NetStat_PoolInit((NET_STAT_POOL   *)&NetConn_PoolStat,
                     (NET_STAT_POOL_QTY) NET_CONN_CFG_NBR_CONN,
                     (NET_ERR         *)&stat_err);


                                                                /* ---------------- INIT NET CONN TBL ----------------- */
    pconn = &NetConn_Tbl[0];
    for (i = 0; i < NET_CONN_CFG_NBR_CONN; i++) {
        pconn->Type  =  NET_CONN_TYPE_CONN;                     /* Init each net conn type/id--NEVER modify.            */
        pconn->ID    = (NET_CONN_ID)i;

        pconn->Flags =  NET_CONN_FLAG_NONE;                     /* Init each net conn as NOT used.                      */

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
        NetConn_Clr(pconn);
#endif

        pconn->NextConnPtr = NetConn_PoolPtr;                   /* Free each net conn to net conn pool (see Note #2).   */
        NetConn_PoolPtr    = pconn;

        pconn++;
    }


                                                                /* --------------- INIT NET CONN LISTS ---------------- */
                                                                /* Init net conn lists.                                 */
    pconn_list = &NetConn_ConnListHead[0];
    for (j = 0; j < NET_CONN_CFG_PROTOCOL_MAX; j++) {
       *pconn_list = (NET_CONN *)0;
        pconn_list++;
    }

                                                                /* Init net conn list ptrs.                             */
    NetConn_ConnListChainPtr     = (NET_CONN *)0;
    NetConn_ConnListConnPtr      = (NET_CONN *)0;
    NetConn_ConnListNextChainPtr = (NET_CONN *)0;
    NetConn_ConnListNextConnPtr  = (NET_CONN *)0;


                                                                /* ----------- INIT NET CONN WILDCARD ADDRS ----------- */
                                                                /* See Note #3.                                         */
    Mem_Clr((void     *)&NetConn_AddrWildCard[0],
            (CPU_SIZE_T) NET_CONN_CFG_ADDR_LEN);

#if (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
    if (NET_CONN_ADDR_IP_LEN_ADDR <= NET_CONN_CFG_ADDR_LEN) {   /* See Note #4.                                         */
        NetConn_AddrWildCardAvail  = DEF_YES;

        val_len = NET_CONN_ADDR_IP_LEN_ADDR;
        for (k = 0; k < val_len; k++) {
            val                      = (NET_CONN_ADDR_IP_WILD_CARD >> (k * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
            ix                       =  val_len - k - 1;
            NetConn_AddrWildCard[ix] =  val;
        }

    } else {
        NetConn_AddrWildCardAvail = DEF_NO;
    }

#else
    NetConn_AddrWildCardAvail = DEF_NO;
   (void)&k;                                                    /* Prevent compiler warnings.                           */
   (void)&ix;
   (void)&val;
   (void)&val_len;
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetConn_CfgAccessedTh()
*
* Description : Configure network connection access promotion threshold.
*
* Argument(s) : nbr_access      Desired number of accesses before network connection is promoted.
*
* Return(s)   : DEF_OK,   network connection access promotion threshold configured.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) 'NetConn_AccessedTh_nbr' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetConn_CfgAccessedTh (CPU_INT16U  nbr_access)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif


#if (NET_CONN_ACCESSED_TH_MIN > DEF_INT_16U_MIN_VAL)
    if (nbr_access < NET_CONN_ACCESSED_TH_MIN) {
        return (DEF_FAIL);
    }
#endif
#if (NET_CONN_ACCESSED_TH_MAX < DEF_INT_16U_MAX_VAL)
    if (nbr_access > NET_CONN_ACCESSED_TH_MAX) {
        return (DEF_FAIL);
    }
#endif

    CPU_CRITICAL_ENTER();
    NetConn_AccessedTh_nbr = nbr_access;
    CPU_CRITICAL_EXIT();

    return (DEF_OK);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetConn_Get()
*
* Description : (1) Allocate & initialize a network connection :
*
*                   (a) Get      a network connection
*                   (b) Validate   network connection
*                   (c) Initialize network connection
*                   (d) Update     network connection pool statistics
*                   (e) Return network connection handle identifier
*                         OR
*                       Null identifier & error code, on failure
*
*               (2) The network connection pool is implemented as a stack :
*
*                   (a) 'NetConn_PoolPtr' points to the head of the network connection pool.
*
*                   (b) Connections' 'NextConnPtr's link each connection to form the connection pool stack.
*
*                   (c) Connections are inserted & removed at the head of        the connection pool stack.
*
*
*                                     Connections are
*                                    inserted & removed
*                                        at the head
*                                      (see Note #2c)
*
*                                             |               NextConnPtr
*                                             |             (see Note #2b)
*                                             v                    |
*                                                                  |
*                                          -------       -------   v   -------       -------
*                     Connection Pool ---->|     |------>|     |------>|     |------>|     |
*                         Pointer          |     |       |     |       |     |       |     |
*                                          |     |       |     |       |     |       |     |
*                      (see Note #2a)      -------       -------       -------       -------
*
*                                          |                                               |
*                                          |<--------- Pool of Free Connections ---------->|
*                                          |                (see Note #2)                  |
*
*
* Argument(s) : family          Network connection family type.
*
*               protocol_ix     Network connection protocol index.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE                   Network connection successfully allocated &
*                                                                       initialized.
*                               NET_CONN_ERR_NONE_AVAIL             NO available network connections to allocate.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_TYPE           Invalid network connection type.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*
* Return(s)   : Connection handle identifier, if NO errors.
*
*               NET_CONN_ID_NONE,             otherwise.
*
* Caller(s)   : NetSock_BindHandler(),
*               NetTCP_RxPktConnHandlerListen().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (3) (a) Network connection pool is accessed by 'NetConn_PoolPtr' during execution of
*
*                       (1) NetConn_Init()
*                       (2) NetConn_Get()
*                       (3) NetConn_FreeHandler()
*
*                   (b) Since the primary tasks of the network protocol suite are prevented from running 
*                       concurrently (see 'net.h  Note #2'), it is NOT necessary to protect the shared 
*                       resources of the network connection pool since no asynchronous access from other 
*                       network tasks is possible.
*********************************************************************************************************
*/
/*$PAGE*/
NET_CONN_ID  NetConn_Get (NET_CONN_FAMILY        family,
                          NET_CONN_PROTOCOL_IX   protocol_ix,
                          NET_ERR               *perr)
{
    NET_CONN     *pconn;
    NET_CONN_ID   conn_id;
    NET_ERR       stat_err;


                                                                /* -------------- VALIDATE NET CONN ARGS -------------- */
    switch (family) {
#if (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
        case NET_CONN_FAMILY_IP_V4_SOCK:
             switch (protocol_ix) {
                 case NET_CONN_PROTOCOL_IX_IP_V4_UDP:
#ifdef  NET_TCP_MODULE_PRESENT
                 case NET_CONN_PROTOCOL_IX_IP_V4_TCP:
#endif
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidProtocolIxCtr);
                     *perr =  NET_CONN_ERR_INVALID_PROTOCOL_IX;
                      return (NET_CONN_ID_NONE);                /* Prevent 'break NOT reachable' compiler warning.      */
             }
             break;
#endif

        case NET_CONN_FAMILY_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidFamilyCtr);
            *perr =  NET_CONN_ERR_INVALID_FAMILY;
             return (NET_CONN_ID_NONE);                         /* Prevent 'break NOT reachable' compiler warning.      */
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (protocol_ix >= NET_CONN_CFG_PROTOCOL_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidProtocolIxCtr);
       *perr =  NET_CONN_ERR_INVALID_PROTOCOL_IX;
        return (NET_CONN_ID_NONE);
    }
#endif


                                                                /* ------------------- GET NET CONN ------------------- */
    if (NetConn_PoolPtr != (NET_CONN *)0) {                     /* If net conn pool NOT empty, get net conn from pool.  */
        pconn            = (NET_CONN *)NetConn_PoolPtr;
        NetConn_PoolPtr  = (NET_CONN *)pconn->NextConnPtr;

    } else {                                                    /* If none avail, rtn err.                              */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrNoneAvailCtr);
       *perr =  NET_CONN_ERR_NONE_AVAIL;
        return (NET_CONN_ID_NONE);
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE NET CONN ----------------- */
    if (pconn->Type != NET_CONN_TYPE_CONN) {
        NetConn_Discard(pconn);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidTypeCtr);
       *perr =  NET_CONN_ERR_INVALID_TYPE;
        return (NET_CONN_ID_NONE);
    }
#endif

                                                                /* ------------------ INIT NET CONN ------------------- */
    NetConn_Clr(pconn);
    DEF_BIT_SET(pconn->Flags, NET_CONN_FLAG_USED);              /* Set net conn as used.                                */
    pconn->Family     = family;
    pconn->ProtocolIx = protocol_ix;

                                                                /* ------------ UPDATE NET CONN POOL STATS ------------ */
    NetStat_PoolEntryUsedInc(&NetConn_PoolStat, &stat_err);

                                                                /* ----------------- RTN NET CONN ID ------------------ */
    conn_id = pconn->ID;
   *perr    = NET_CONN_ERR_NONE;

    return (conn_id);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetConn_Free()
*
* Description : Free a network connection.
*
*               (1) Network connection free ONLY frees but does NOT close any connections.
*
*
* Argument(s) : conn_id     Handle identifier of network connection to free.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) #### To prevent freeing a network connection already freed via previous network
*                   connection free, NetConn_Free() checks if the network connection is used BEFORE 
*                   freeing the network connection.
*
*                   This prevention is only best-effort since any invalid duplicate network connection 
*                   frees MAY be asynchronous to potentially valid network connection gets.  Thus the 
*                   invalid network connection free(s) MAY corrupt the network connection's valid
*                   operation(s).
*
*                   However, since the primary tasks of the network protocol suite are prevented from
*                   running concurrently (see 'net.h  Note #2'), it is NOT necessary to protect network
*                   connection resources from possible corruption since no asynchronous access from
*                   other network tasks is possible.
*********************************************************************************************************
*/

void  NetConn_Free (NET_CONN_ID  conn_id)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NET_ERR    err;
#endif
    NET_CONN  *pconn;

                                                                /* ---------------- VALIDATE NET CONN ----------------- */
    if (conn_id == NET_CONN_ID_NONE) {
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, &err);
    if (err != NET_CONN_ERR_NONE) {                             /* If net conn NOT used, ...                            */
        return;                                                 /* ... rtn but do NOT free (see Note #2).               */
    }
#endif

                                                                /* ------------------ FREE NET CONN ------------------- */
    pconn = &NetConn_Tbl[conn_id];
    NetConn_FreeHandler(pconn);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetConn_CloseFromApp()
*
* Description : (1) Close a network connection from application layer :
*
*                   (a) Close transport connection, if requested                            See Note #3c
*                   (b) Clear network   connection's reference to application connection    See Note #3c
*                   (c) Free  network   connection, if necessary
*
*
* Argument(s) : conn_id                 Handle identifier of network connection to close.
*
*               close_conn_transport    Indicate whether to close transport connection :
*
*                                           DEF_YES                    Close transport connection.
*                                           DEF_NO              Do NOT close transport connection.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CloseConn(),
*               NetSock_CloseSockHandler(),
*               NetSock_ConnAcceptQ_Clr().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) #### To prevent closing a network connection already closed via previous network
*                   connection close, NetConn_CloseFromApp() checks if the network connection is used 
*                   BEFORE closing the network connection.
*
*                   This prevention is only best-effort since any invalid duplicate network connection 
*                   closes MAY be asynchronous to potentially valid network connection gets.  Thus the 
*                   invalid network connection close(s) MAY corrupt the network connection's valid
*                   operation(s).
*
*                   However, since the primary tasks of the network protocol suite are prevented from
*                   running concurrently (see 'net.h  Note #2'), it is NOT necessary to protect network
*                   connection resources from possible corruption since no asynchronous access from
*                   other network tasks is possible.
*
*                   (a) Network connection(s) MAY already be closed AFTER other network connection 
*                       close operations & MUST be validated as used BEFORE any subsequent network 
*                       connection close operation(s).
*
*               (3) (a) Network connections are considered connected if any of the following network 
*                       connections are valid :
*
*                       (1) Application layer connection
*                       (2) Transport   layer connection
*
*                               (A) Network connections which ONLY reference application layer clone 
*                                   connection(s) are NOT considered connected since the actual non-
*                                   cloned application connection MAY or MAY NOT reference the cloned
*                                   network connection.
*
*                   (b) Since NetConn_CloseFromApp() actively closes the application layer connection,
*                       network connections need only validate the remaining transport layer connection
*                       as connected.
*
*                   (c) Since network connection(s) connection validation determines, in part, when to
*                       close the network connection (see Note #3a), & since NetConn_CloseFromTransport() 
*                       may indirectly call NetConn_CloseFromApp(); clearing the network connection's 
*                       application layer connection handle identifier MUST follow the closing of the 
*                       transport   layer connection to prevent re-closing the network connection.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetConn_CloseFromApp (NET_CONN_ID  conn_id,
                            CPU_BOOLEAN  close_conn_transport)
{
    CPU_BOOLEAN   used;
    CPU_BOOLEAN   connd;
    CPU_BOOLEAN   free;
    NET_CONN     *pconn;
    NET_ERR       err;

                                                                    /* -------------- VALIDATE NET CONN --------------- */
    if (conn_id == NET_CONN_ID_NONE) {
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                     /* ------------ VALIDATE NET CONN USED ------------ */
   (void)NetConn_IsUsed(conn_id, &err);
    if (err != NET_CONN_ERR_NONE) {                                 /* If net conn NOT used, ...                        */
        return;                                                     /* ... rtn but do NOT close (see Note #2).          */
    }
#endif


    pconn = &NetConn_Tbl[conn_id];

                                                                    /* ------------- CLOSE TRANSPORT CONN ------------- */
    if (close_conn_transport == DEF_YES) {                          
        NetConn_CloseTransport(pconn);                              /* Close transport conn, if req'd.                  */
        used  =  DEF_BIT_IS_SET(pconn->Flags, NET_CONN_FLAG_USED);  /* If still used (see Note #2a), free net conn ...  */
        free  = (used == DEF_YES) ? DEF_YES : DEF_NO;               /* ... since app & transport conns closed.          */

    } else {                                                        /* Else chk net conn conn'd (see Note #3b).         */
        connd = (pconn->ID_Transport != NET_CONN_ID_NONE) ? DEF_YES : DEF_NO;
        free  =  (connd != DEF_YES) ? DEF_YES : DEF_NO;             /* Free net conn, if NOT conn'd.                    */
    }

                                                                    /* ---------------- CLOSE APP CONN ---------------- */
    NetConn_ID_AppSet(conn_id, NET_CONN_ID_NONE, &err);             /* Clr net conn's app conn id (see Note #3c).       */

                                                                    /* ---------------- FREE NET CONN ----------------- */
    if (free == DEF_YES) {
        NetConn_FreeHandler(pconn);                                 /* Free net conn, if req'd.                         */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetConn_CloseFromTransport()
*
* Description : (1) Close a network connection from transport layer :
*
*                   (a) Close application connection, if requested                          See Note #3c
*                   (b) Clear network     connection's reference to transport connection    See Note #3c
*                   (c) Free  network     connection, if necessary
*
*
* Argument(s) : conn_id             Handle identifier of network connection to close.
*
*               close_conn_app      Indicate whether to close application connection :
*
*                                       DEF_YES                        Close application connection.
*                                       DEF_NO                  Do NOT close application connection.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_ConnClose().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) #### To prevent closing a network connection already closed via previous network
*                   connection close, NetConn_CloseFromTransport() checks if the network connection 
*                   is used BEFORE closing the network connection.
*
*                   This prevention is only best-effort since any invalid duplicate network connection 
*                   closes MAY be asynchronous to potentially valid network connection gets.  Thus the 
*                   invalid network connection close(s) MAY corrupt the network connection's valid
*                   operation(s).
*
*                   However, since the primary tasks of the network protocol suite are prevented from
*                   running concurrently (see 'net.h  Note #2'), it is NOT necessary to protect network
*                   connection resources from possible corruption since no asynchronous access from
*                   other network tasks is possible.
*
*                   (a) Network connection(s) MAY already be closed AFTER other network connection 
*                       close operations & MUST be validated as used BEFORE any subsequent network 
*                       connection close operation(s).
*
*               (3) (a) Network connections are considered connected if any of the following network 
*                       connections are valid :
*
*                       (1) Application layer connection
*                       (2) Transport   layer connection
*
*                               (A) Network connections which ONLY reference application layer clone 
*                                   connection(s) are NOT considered connected since the actual non-
*                                   cloned application connection MAY or MAY NOT reference the cloned
*                                   network connection.
*
*                   (b) Since NetConn_CloseFromTransport() actively closes the transport layer connection,
*                       network connections need only validate the remaining application layer connection
*                       as connected.
*
*                   (c) Since network connection(s) connection validation determines, in part, when to
*                       close the network connection (see Note #3a), & since NetConn_CloseFromApp() may
*                       indirectly call NetConn_CloseFromTransport(); clearing the network connection's 
*                       transport   layer connection handle identifier MUST follow the closing of the 
*                       application layer connection to prevent re-closing the network connection.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetConn_CloseFromTransport (NET_CONN_ID  conn_id,
                                  CPU_BOOLEAN  close_conn_app)
{
    CPU_BOOLEAN   used;
    CPU_BOOLEAN   connd;
    CPU_BOOLEAN   free;
    NET_CONN     *pconn;
    NET_ERR       err;

                                                                    /* -------------- VALIDATE NET CONN --------------- */
    if (conn_id == NET_CONN_ID_NONE) {
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                     /* ------------ VALIDATE NET CONN USED ------------ */
   (void)NetConn_IsUsed(conn_id, &err);
    if (err != NET_CONN_ERR_NONE) {                                 /* If net conn NOT used, ...                        */
        return;                                                     /* ... rtn but do NOT close (see Note #2).          */
    }
#endif


    pconn = &NetConn_Tbl[conn_id];

                                                                    /* ---------------- CLOSE APP CONN ---------------- */
    if (close_conn_app == DEF_YES) {                          
        NetConn_CloseApp(pconn);                                    /* Close app conn, if req'd.                        */
        used  =  DEF_BIT_IS_SET(pconn->Flags, NET_CONN_FLAG_USED);  /* If still used (see Note #2a), free net conn ...  */
        free  = (used == DEF_YES) ? DEF_YES : DEF_NO;               /* ... since app & transport conns closed.          */

    } else {                                                        /* Else chk net conn conn'd (see Note #3b).         */
        connd = (pconn->ID_App != NET_CONN_ID_NONE) ? DEF_YES : DEF_NO;
        free  =  (connd != DEF_YES) ? DEF_YES : DEF_NO;             /* Free net conn, if NOT conn'd.                    */
    }

                                                                    /* ------------- CLOSE TRANSPORT CONN ------------- */
    NetConn_ID_TransportSet(conn_id, NET_CONN_ID_NONE, &err);       /* Clr net conn's transport conn id (see Note #3c). */

                                                                    /* ---------------- FREE NET CONN ----------------- */
    if (free == DEF_YES) {
        NetConn_FreeHandler(pconn);                                 /* Free net conn, if req'd.                         */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetConn_CloseAllConns()
*
* Description : Close ALL network connections.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY
*               be called by application function(s) [see also Note #2].
*
* Note(s)     : (1) (a) Certain circumstances may require that :
*
*                       (1) ALL network protocol suite connections close; ...
*                       (2) All pending network &/or application connection function(s) SHOULD :
*                           (A) Abort, immediately if possible; ...
*                           (B) Return appropriate closed error code(s).
*
*                   (b) The following example(s) list when to close all network connections :
*
*                       (1) RFC #2131, Section 4.4.5 states that "if the [DHCP] client is given a
*                           new network address, it MUST NOT continue using the previous network
*                           address and SHOULD notify the local users of the problem".
*
*                           Therefore, ALL network connections based on a previous network address
*                           MUST be closed.
*
*               (2) NetConn_CloseAllConns() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetConn_CloseAllConnsHandler()  Note #1'.
*********************************************************************************************************
*/

void  NetConn_CloseAllConns (void)
{
    NET_ERR  err;


    NetOS_Lock(&err);                                           /* Acquire net lock (see Note #2b).                     */
    if (err != NET_OS_ERR_NONE) {
        return;
    }

    NetConn_CloseAllConnsHandler();                             /* Close all net conns.                                 */

    NetOS_Unlock();                                             /* Release net lock.                                    */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetConn_CloseAllConnsHandler()
*
* Description : Close ALL network connections in ALL network connection lists.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_CloseAllConns().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s) [see also Note #1].
*
* Note(s)     : (1) NetConn_CloseAllConnsHandler() is called by network protocol suite function(s) &
*                   MUST be called with the global network lock already acquired.
*
*                   See also 'NetConn_CloseAllConns()  Note #2'.
*
*               (2) Invalid network connection(s) in the connection list MAY already be closed in other
*                   validation functions.
*
*               (3) Since network connection close handlers execute asynchronously to NetConn_FreeHandler(),
*                   network connection list pointers ('NetConn_ConnListChainPtr', 'NetConn_ConnListConnPtr',
*                   etc.) MUST be coordinated with NetConn_FreeHandler() to avoid network connection list
*                   corruption :
*
*                   (a) (1) Network connection list pointers typically advance to the next network connection
*                           in a network connection list.
*
*                       (2) However, whenever a network connection list pointer connection is freed by an
*                           asynchronous network connection close, the network connection list pointer(s)
*                           MUST be advanced to the next valid & available network connection in the
*                           network connection list.
*
*                           See also 'NetConn_FreeHandler()  Note #2a'.
*
*                   (b) Network connection list pointers MUST be cleared after handling the network
*                       connection list.
*
*                       (1) However, network connection list pointers are implicitly cleared after 
*                           handling the network connection list.
*********************************************************************************************************
*/

void  NetConn_CloseAllConnsHandler (void)
{
    NET_CONN           **pconn_list;
    NET_CONN_LIST_QTY    i;

                                                                        /* ----- CLOSE ALL CONN LISTS' NET CONNS ------ */
    pconn_list = &NetConn_ConnListHead[0];
    for (i = 0; i < NET_CONN_CFG_PROTOCOL_MAX; i++) {
        NetConn_ConnListChainPtr = *pconn_list;                         /* Start close @ conn list head.                */
        while (NetConn_ConnListChainPtr != (NET_CONN *)0) {             /* Close ALL net conn chains ...                */
            NetConn_ConnListNextChainPtr = (NET_CONN *)NetConn_ConnListChainPtr->NextChainPtr;
            NetConn_ConnListConnPtr      = (NET_CONN *)NetConn_ConnListChainPtr;

            while (NetConn_ConnListConnPtr != (NET_CONN *)0) {          /* ... & ALL net conns.                         */
                NetConn_ConnListNextConnPtr = (NET_CONN *)NetConn_ConnListConnPtr->NextConnPtr;

                NetConn_Close(NetConn_ConnListConnPtr);                 /* Close net conn.                              */

                NetConn_ConnListConnPtr = NetConn_ConnListNextConnPtr;  /* Adv to next net conn       (see Note #3a1).  */
            }

            NetConn_ConnListChainPtr    = NetConn_ConnListNextChainPtr; /* Adv to next net conn chain (see Note #3a1).  */
        }

#if 0                                                                   /* Clr net conn list ptrs (see Note #3b1).      */
        NetConn_ConnListChainPtr     = (NET_CONN *)0;
        NetConn_ConnListConnPtr      = (NET_CONN *)0;
        NetConn_ConnListNextChainPtr = (NET_CONN *)0;
        NetConn_ConnListNextConnPtr  = (NET_CONN *)0;
#endif

       *pconn_list = (NET_CONN *)0;
        pconn_list++;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetConn_ID_AppGet()
*
* Description : Get a network connection's application layer handle identifier.
*
* Argument(s) : conn_id     Handle identifier of network connection to get application layer handle identifier.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE               Network connection's handle identifier 
*                                                                   successfully returned.
*
*                                                               --- RETURNED BY NetConn_IsUsed() : ---
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : Network connection's application layer handle identifier, if NO errors.
*
*               NET_CONN_ID_NONE,                                         otherwise.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

NET_CONN_ID  NetConn_ID_AppGet (NET_CONN_ID   conn_id,
                                NET_ERR      *perr)
{
    NET_CONN     *pconn;
    NET_CONN_ID   conn_id_app;

                                                                /* ---------------- VALIDATE NET CONN ----------------- */
    if (conn_id == NET_CONN_ID_NONE) {
       *perr =  NET_CONN_ERR_NONE;
        return (NET_CONN_ID_NONE);
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return (NET_CONN_ID_NONE);
    }
#endif


    pconn       = &NetConn_Tbl[conn_id];
    conn_id_app =  pconn->ID_App;                               /* Get net conn's app id.                               */


   *perr =  NET_CONN_ERR_NONE;

    return (conn_id_app);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetConn_ID_AppSet()
*
* Description : Set a network connection's application layer handle identifier.
*
* Argument(s) : conn_id         Handle identifier of network connection to set application layer handle
*                                   identifier.
*
*               conn_id_app     Connection's application layer handle identifier.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE               Network connection's handle identifier
*                                                                   successfully set.
*
*                                                               --- RETURNED BY NetConn_IsUsed() : ---
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetConn_ID_AppSet (NET_CONN_ID   conn_id,
                         NET_CONN_ID   conn_id_app,
                         NET_ERR      *perr)
{
    NET_CONN  *pconn;

                                                                /* ---------------- VALIDATE NET CONN ----------------- */
    if (conn_id == NET_CONN_ID_NONE) {
       *perr = NET_CONN_ERR_NONE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }
#endif

                                                                /* --------------- VALIDATE APP CONN ID --------------- */
    if (conn_id_app < NET_CONN_ID_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnCtr);
       *perr = NET_CONN_ERR_INVALID_CONN;
        return;
    }

    pconn         = &NetConn_Tbl[conn_id];
    pconn->ID_App =  conn_id_app;                               /* Set net conn's app id.                               */


   *perr = NET_CONN_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetConn_ID_AppCloneGet()
*
* Description : Get a network connection's application layer clone handle identifier.
*
* Argument(s) : conn_id     Handle identifier of network connection to get application layer clone handle
*                               identifier.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE               Network connection's handle identifier 
*                                                                   successfully returned.
*
*                                                               --- RETURNED BY NetConn_IsUsed() : ---
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : Network connection's application layer clone handle identifier, if NO errors.
*
*               NET_CONN_ID_NONE,                                               otherwise.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

NET_CONN_ID  NetConn_ID_AppCloneGet (NET_CONN_ID   conn_id,
                                     NET_ERR      *perr)
{
    NET_CONN     *pconn;
    NET_CONN_ID   conn_id_app_clone;

                                                                /* ---------------- VALIDATE NET CONN ----------------- */
    if (conn_id == NET_CONN_ID_NONE) {
       *perr =  NET_CONN_ERR_NONE;
        return (NET_CONN_ID_NONE);
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return (NET_CONN_ID_NONE);
    }
#endif


    pconn             = &NetConn_Tbl[conn_id];
    conn_id_app_clone =  pconn->ID_AppClone;                    /* Get net conn's app clone id.                         */


   *perr =  NET_CONN_ERR_NONE;

    return (conn_id_app_clone);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetConn_ID_AppCloneSet()
*
* Description : Set a network connection's application layer clone handle identifier.
*
* Argument(s) : conn_id         Handle identifier of network connection to set application layer clone 
*                                   handle identifier.
*
*               conn_id_app     Connection's application layer handle identifier.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE               Network connection's handle identifier
*                                                                   successfully set.
*
*                                                               --- RETURNED BY NetConn_IsUsed() : ---
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetConn_ID_AppCloneSet (NET_CONN_ID   conn_id,
                              NET_CONN_ID   conn_id_app,
                              NET_ERR      *perr)
{
    NET_CONN  *pconn;

                                                                /* ---------------- VALIDATE NET CONN ----------------- */
    if (conn_id == NET_CONN_ID_NONE) {
       *perr = NET_CONN_ERR_NONE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }
#endif

                                                                /* --------------- VALIDATE APP CONN ID --------------- */
    if (conn_id_app < NET_CONN_ID_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnCtr);
       *perr = NET_CONN_ERR_INVALID_CONN;
        return;
    }

    pconn              = &NetConn_Tbl[conn_id];
    pconn->ID_AppClone =  conn_id_app;                          /* Set net conn's app clone id.                         */


   *perr = NET_CONN_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetConn_ID_TransportGet()
*
* Description : Get a network connection's transport layer handle identifier.
*
* Argument(s) : conn_id     Handle identifier of network connection to get transport layer handle identifier.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE               Network connection's handle identifier 
*                                                                   successfully returned.
*
*                                                               --- RETURNED BY NetConn_IsUsed() : ---
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : Network connection's transport layer handle identifier, if NO errors.
*
*               NET_CONN_ID_NONE,                                       otherwise.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

NET_CONN_ID  NetConn_ID_TransportGet (NET_CONN_ID   conn_id,
                                      NET_ERR      *perr)
{
    NET_CONN     *pconn;
    NET_CONN_ID   conn_id_transport;

                                                                /* ---------------- VALIDATE NET CONN ----------------- */
    if (conn_id == NET_CONN_ID_NONE) {
       *perr =  NET_CONN_ERR_NONE;
        return (NET_CONN_ID_NONE);
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return (NET_CONN_ID_NONE);
    }
#endif


    pconn             = &NetConn_Tbl[conn_id];
    conn_id_transport =  pconn->ID_Transport;                   /* Get net conn's transport id.                         */


   *perr =  NET_CONN_ERR_NONE;

    return (conn_id_transport);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetConn_ID_TransportSet()
*
* Description : Set a network connection's transport layer handle identifier.
*
* Argument(s) : conn_id             Handle identifier of network connection to set transport layer handle
*                                       identifier.
*
*               conn_id_transport   Connection's transport layer handle identifier.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE               Network connection's handle identifier
*                                                                   successfully set.
*
*                                                               --- RETURNED BY NetConn_IsUsed() : ---
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetConn_ID_TransportSet (NET_CONN_ID   conn_id,
                               NET_CONN_ID   conn_id_transport,
                               NET_ERR      *perr)
{
    NET_CONN  *pconn;

                                                                /* ---------------- VALIDATE NET CONN ----------------- */
    if (conn_id == NET_CONN_ID_NONE) {
       *perr = NET_CONN_ERR_NONE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }
#endif

                                                                /* ------------ VALIDATE TRANSPORT CONN ID ------------ */
    if (conn_id_transport < NET_CONN_ID_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnCtr);
       *perr = NET_CONN_ERR_INVALID_CONN;
        return;
    }

    pconn               = &NetConn_Tbl[conn_id];
    pconn->ID_Transport =  conn_id_transport;                   /* Set net conn's transport id.                         */


   *perr = NET_CONN_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetConn_AddrLocalGet()
*
* Description : Get a network connection's local address.
*
* Argument(s) : conn_id         Handle identifier of network connection to get local address.
*
*               paddr_local     Pointer to variable that will receive the return  local address (see Note #1),
*                                   if NO errors.
*
*               paddr_len       Pointer to a variable to ... :
*                              
*                                   (a) Pass the size of the address buffer pointed to by 'paddr_local'.
*                                   (b) (1) Return the actual local address length, if NO errors;
*                                       (2) Return 0,                               otherwise.
*
*                               See also Note #2.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE                   Network connection's address successfully
*                                                                       returned.
*                               NET_CONN_ERR_NULL_PTR               Argument 'paddr_local'/'paddr_len'
*                                                                       passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
*                                                                   ----- RETURNED BY NetConn_IsUsed() : -----
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnHandlerStream(),
*               NetSock_ConnHandlerAddrRemoteValidate(),
*               NetSock_TxDataHandlerDatagram(),
*               NetSock_FreeAddr(),
*               NetTCP_TxConnPrepareSegAddrs().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network connection addresses maintained in network-order.
*
*               (2) Since 'paddr_len' parameter is both an input & output parameter (see 'Argument(s) :
*                   paddr_len'), ... :
*
*                   (a) Its input value SHOULD be validated prior to use.
*                       (1) In the case that the 'paddr_len' parameter is passed a null pointer,
*                           NO input value is validated or used.
*
*                   (b) While its output value MUST be initially configured to return a default
*                       value in case of any function exit due to error/fault conditions.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetConn_AddrLocalGet (NET_CONN_ID         conn_id,
                            CPU_INT08U         *paddr_local,
                            NET_CONN_ADDR_LEN  *paddr_len,
                            NET_ERR            *perr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NET_CONN_ADDR_LEN   addr_len;
#endif
    NET_CONN           *pconn;

                                                                /* ------------------ VALIDATE ADDR ------------------- */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (paddr_len == (NET_CONN_ADDR_LEN *)0) {                  /* See Note #2a1.                                       */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrNullPtrCtr);
       *perr = NET_CONN_ERR_NULL_PTR;
        return;
    }

     addr_len = *paddr_len;
#endif
   *paddr_len =  0;                                             /* Cfg dflt addr len for err (see Note #2b).            */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (addr_len < NET_CONN_CFG_ADDR_LEN) {                     /* Validate initial addr len (see Note #2a).            */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnAddrLenCtr);
       *perr = NET_CONN_ERR_INVALID_ADDR_LEN;
        return;
    }
    if (paddr_local == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrNullPtrCtr);
       *perr = NET_CONN_ERR_NULL_PTR;
        return;
    }

                                                                /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }
#endif


    pconn = &NetConn_Tbl[conn_id];

                                                                /* -------------- VALIDATE NET CONN ADDR -------------- */
    if (pconn->AddrLocalValid != DEF_YES) {                     /* If net conn local addr NOT avail, rtn err.           */
       *perr = NET_CONN_ERR_ADDR_NOT_USED;
        return;
    }


    Mem_Copy((void     *) paddr_local,                          /* Copy & rtn net conn local addr.                      */
             (void     *)&pconn->AddrLocal[0],
             (CPU_SIZE_T) NET_CONN_CFG_ADDR_LEN);

   *paddr_len = NET_CONN_CFG_ADDR_LEN;


   *perr = NET_CONN_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetConn_AddrLocalSet()
*
* Description : Set a network connection's local address.
*
* Argument(s) : conn_id         Handle identifier of network connection to set local address.
*
*               paddr_local     Pointer to local address (see Note #1).
*
*               addr_len        Length  of local address (in octets).
*
*               addr_over_wr    Allow      local address overwrite :
*
*                                   DEF_NO                          Do NOT overwrite local address.
*                                   DEF_YES                                Overwrite local address.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE                   Network connection's address successfully set.
*                               NET_CONN_ERR_NULL_PTR               Argument 'paddr_local' passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
*                                                                   ------- RETURNED BY NetConn_IsUsed() : -------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_BindHandler(),
*               NetTCP_RxPktConnHandlerListen().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network connection addresses maintained in network-order.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetConn_AddrLocalSet (NET_CONN_ID         conn_id,
                            CPU_INT08U         *paddr_local,
                            NET_CONN_ADDR_LEN   addr_len,
                            CPU_BOOLEAN         addr_over_wr,
                            NET_ERR            *perr)
{
    NET_CONN  *pconn;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }
#endif

    pconn = &NetConn_Tbl[conn_id];

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------ VALIDATE ADDR ------------------- */
    if (paddr_local == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrNullPtrCtr);
       *perr = NET_CONN_ERR_NULL_PTR;
        return;
    }
    if (addr_len != NET_CONN_CFG_ADDR_LEN) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnAddrLenCtr);
       *perr = NET_CONN_ERR_INVALID_ADDR_LEN;
        return;
    }
    if (addr_over_wr != DEF_YES) {                              /* If addr over-wr NOT req'd ...                        */
        if (pconn->AddrLocalValid != DEF_NO) {                  /* ... & local addr valid,   ...                        */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnAddrInUseCtr);
           *perr = NET_CONN_ERR_ADDR_IN_USE;                    /* ... rtn err.                                         */
            return;
        }
    }

#else                                                           /* Prevent compiler warnings.                           */
   (void)&addr_len;
   (void)&addr_over_wr;
#endif


    Mem_Copy((void     *)&pconn->AddrLocal[0],                  /* Copy local addr to net conn addr.                    */
             (void     *) paddr_local,
             (CPU_SIZE_T) NET_CONN_CFG_ADDR_LEN);

    pconn->AddrLocalValid = DEF_YES;


   *perr = NET_CONN_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetConn_AddrRemoteGet()
*
* Description : Get a network connection's remote address.
*
* Argument(s) : conn_id         Handle identifier of network connection to get remote address.
*
*               paddr_remote    Pointer to variable that will receive the return  remote address (see Note #1),
*                                   if NO errors.
*
*               paddr_len       Pointer to a variable to ... :
*                              
*                                   (a) Pass the size of the address buffer pointed to by 'paddr_remote'.
*                                   (b) (1) Return the actual local address length, if NO errors;
*                                       (2) Return 0,                               otherwise.
*
*                               See also Note #2.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE                   Network connection's address successfully
*                                                                       returned.
*                               NET_CONN_ERR_NULL_PTR               Argument 'paddr_local'/'paddr_len'
*                                                                       passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
*                                                                   ----- RETURNED BY NetConn_IsUsed() : -----
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Accept(),
*               NetSock_BindHandler(),
*               NetSock_RxDataHandlerStream(),
*               NetSock_TxDataHandlerDatagram(),
*               NetTCP_TxConnPrepareSegAddrs().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network connection addresses maintained in network-order.
*
*               (2) Since 'paddr_len' parameter is both an input & output parameter (see 'Argument(s) :
*                   paddr_len'), ... :
*
*                   (a) Its input value SHOULD be validated prior to use.
*                       (1) In the case that the 'paddr_len' parameter is passed a null pointer,
*                           NO input value is validated or used.
*
*                   (b) While its output value MUST be initially configured to return a default
*                       value in case of any function exit due to error/fault conditions.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetConn_AddrRemoteGet (NET_CONN_ID         conn_id,
                             CPU_INT08U         *paddr_remote,
                             NET_CONN_ADDR_LEN  *paddr_len,
                             NET_ERR            *perr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NET_CONN_ADDR_LEN   addr_len;
#endif
    NET_CONN           *pconn;

                                                                /* ------------------ VALIDATE ADDR ------------------- */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (paddr_len == (NET_CONN_ADDR_LEN *)0) {                  /* See Note #2a1.                                       */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrNullPtrCtr);
       *perr = NET_CONN_ERR_NULL_PTR;
        return;
    }

     addr_len = *paddr_len;
#endif
   *paddr_len =  0;                                             /* Cfg dflt addr len for err (see Note #2b).            */

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (addr_len < NET_CONN_CFG_ADDR_LEN) {                     /* Validate initial addr len (see Note #2a).            */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnAddrLenCtr);
       *perr = NET_CONN_ERR_INVALID_ADDR_LEN;
        return;
    }
    if (paddr_remote == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrNullPtrCtr);
       *perr = NET_CONN_ERR_NULL_PTR;
        return;
    }

                                                                /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }
#endif


    pconn = &NetConn_Tbl[conn_id];

                                                                /* -------------- VALIDATE NET CONN ADDR -------------- */
    if (pconn->AddrRemoteValid != DEF_YES) {                    /* If net conn remote addr NOT avail, rtn err.          */
       *perr = NET_CONN_ERR_ADDR_NOT_USED;
        return;
    }


    Mem_Copy((void     *) paddr_remote,                         /* Copy & rtn net conn remote addr.                     */
             (void     *)&pconn->AddrRemote[0],
             (CPU_SIZE_T) NET_CONN_CFG_ADDR_LEN);

   *paddr_len = NET_CONN_CFG_ADDR_LEN;


   *perr = NET_CONN_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetConn_AddrRemoteSet()
*
* Description : Set a network connection's remote address.
*
* Argument(s) : conn_id         Handle identifier of network connection to set remote address.
*
*               paddr_remote    Pointer to remote address (see Note #1).
*
*               addr_len        Length  of remote address (in octets).
*
*               addr_over_wr    Allow      remote address overwrite :
*
*                                   DEF_NO                          Do NOT overwrite remote address.
*                                   DEF_YES                                Overwrite remote address.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE                   Network connection's address successfully set.
*                               NET_CONN_ERR_NULL_PTR               Argument 'paddr_local' passed a NULL pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_IN_USE            Network connection address already in use.
*
*                                                                   ------- RETURNED BY NetConn_IsUsed() : -------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnHandlerAddrRemoteSet(),
*               NetTCP_RxPktConnHandlerListen().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network connection addresses maintained in network-order.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetConn_AddrRemoteSet (NET_CONN_ID         conn_id,
                             CPU_INT08U         *paddr_remote,
                             NET_CONN_ADDR_LEN   addr_len,
                             CPU_BOOLEAN         addr_over_wr,
                             NET_ERR            *perr)
{
    NET_CONN  *pconn;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }
#endif

    pconn = &NetConn_Tbl[conn_id];

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------ VALIDATE ADDR ------------------- */
    if (paddr_remote == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrNullPtrCtr);
       *perr = NET_CONN_ERR_NULL_PTR;
        return;
    }
    if (addr_len != NET_CONN_CFG_ADDR_LEN) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnAddrLenCtr);
       *perr = NET_CONN_ERR_INVALID_ADDR_LEN;
        return;
    }
    if (addr_over_wr != DEF_YES) {                              /* If addr over-wr NOT req'd ...                        */
        if (pconn->AddrRemoteValid != DEF_NO) {                 /* ... & remote addr valid,  ...                        */
            NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnAddrInUseCtr);
           *perr = NET_CONN_ERR_ADDR_IN_USE;                    /* ... rtn err.                                         */
            return;
        }
    }

#else                                                           /* Prevent compiler warnings.                           */
   (void)&addr_len;
   (void)&addr_over_wr;
#endif


    Mem_Copy((void     *)&pconn->AddrRemote[0],                 /* Copy remote addr to net conn addr.                   */
             (void     *) paddr_remote,
             (CPU_SIZE_T) NET_CONN_CFG_ADDR_LEN);

    pconn->AddrRemoteValid = DEF_YES;


   *perr = NET_CONN_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetConn_AddrRemoteCmp()
*
* Description : Compare an address to a network connection's remote address.
*
* Argument(s) : conn_id         Handle identifier of connection to compare.
*
*               paddr_remote    Pointer to remote address to compare.
*
*               addr_len        Length  of remote address (in octets).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE                   Remote address successfully compared to 
*                                                                       network connection's remote address;
*                                                                       check return value.
*                               NET_CONN_ERR_NULL_PTR               Argument 'paddr_remote' passed a NULL
*                                                                       pointer.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*                               NET_CONN_ERR_ADDR_NOT_USED          Network connection address NOT in use.
*
*                                                                   ----- RETURNED BY NetConn_IsUsed() : -----
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*
* Return(s)   : DEF_YES, if addresses successfully compare.
*
*               DEF_NO,  otherwise.
*
* Caller(s)   : NetSock_IsValidAddrRemote().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
CPU_BOOLEAN  NetConn_AddrRemoteCmp (NET_CONN_ID         conn_id,
                                    CPU_INT08U         *paddr_remote,
                                    NET_CONN_ADDR_LEN   addr_len,
                                    NET_ERR            *perr)
{
    NET_CONN     *pconn;
    CPU_BOOLEAN   cmp;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return (DEF_NO);
    }

                                                                /* ------------------ VALIDATE ADDR ------------------- */
    if (paddr_remote == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrNullPtrCtr);
       *perr =  NET_CONN_ERR_NULL_PTR;
        return (DEF_NO);
    }
    if (addr_len != NET_CONN_CFG_ADDR_LEN) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnAddrLenCtr);
       *perr =  NET_CONN_ERR_INVALID_ADDR_LEN;
        return (DEF_NO);
    }

#else                                                           /* Prevent compiler warning.                            */
   (void)&addr_len;
#endif


    pconn = &NetConn_Tbl[conn_id];

                                                                /* -------------- VALIDATE NET CONN ADDR -------------- */
    if (pconn->AddrRemoteValid != DEF_YES) {                    /* If conn local addr NOT avail, rtn err.               */
       *perr =  NET_CONN_ERR_ADDR_NOT_USED;
        return (DEF_NO);
    }

                                                                
    cmp = Mem_Cmp((void     *) paddr_remote,                    /* Cmp remote addr to conn addr.                        */
                  (void     *)&pconn->AddrRemote[0],
                  (CPU_SIZE_T) NET_CONN_CFG_ADDR_LEN);

   *perr =  NET_CONN_ERR_NONE;

    return (cmp);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetConn_IsUsed()
*
* Description : Validate network connection in use.
*
* Argument(s) : conn_id     Handle identifier of network connection to validate.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE               Network connection successfully validated
*                                                                   as in use.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : DEF_YES, network connection   valid &      in use.
*
*               DEF_NO,  network connection invalid or NOT in use.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) NetConn_IsUsed() blocked until network initialization completes.
*
*               (2) NetConn_IsUsed() MUST be called with the global network lock already acquired.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetConn_IsUsed (NET_CONN_ID   conn_id,
                             NET_ERR      *perr)
{
    NET_CONN     *pconn;
    CPU_BOOLEAN   used;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit (see Note #1).            */
       *perr =  NET_ERR_INIT_INCOMPLETE;
        return (DEF_NO);
    }
#endif

                                                                /* --------------- VALIDATE NET CONN ID --------------- */
    if (conn_id < NET_CONN_ID_MIN) {
       *perr =  NET_CONN_ERR_INVALID_CONN;
        return (DEF_NO);
    }
    if (conn_id > NET_CONN_ID_MAX) {
       *perr =  NET_CONN_ERR_INVALID_CONN;
        return (DEF_NO);
    }

                                                                /* -------------- VALIDATE NET CONN USED -------------- */
    pconn = &NetConn_Tbl[conn_id];
    used  =  DEF_BIT_IS_SET(pconn->Flags, NET_CONN_FLAG_USED);
    if (used != DEF_YES) {
       *perr =  NET_CONN_ERR_NOT_USED;
        return (DEF_NO);
    }


   *perr =  NET_CONN_ERR_NONE;

    return (DEF_YES);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetConn_IsConn()
*
* Description : Determine network connection status.
*
* Argument(s) : conn_id     Handle identifier of network connection to check.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_CONN_NONE          NO   network connection.
*                               NET_CONN_ERR_CONN_HALF          Half network connection --
*                                                                   local            address valid.
*                               NET_CONN_ERR_CONN_FULL          Full network connection --
*                                                                   local AND remote address valid.
*
*                                                               --- RETURNED BY NetConn_IsUsed() : ---
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) NetConn_IsConn() MUST be called with the global network lock already acquired.
*********************************************************************************************************
*/

void  NetConn_IsConn (NET_CONN_ID   conn_id,
                      NET_ERR      *perr)
{
    NET_CONN  *pconn;

                                                                /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }

                                                                /* -------------- DETERMINE CONN STATUS --------------- */
    pconn = &NetConn_Tbl[conn_id];
    if (pconn->AddrLocalValid == DEF_YES) {
        if (pconn->AddrRemoteValid == DEF_YES) {
           *perr = NET_CONN_ERR_CONN_FULL;
        } else {
           *perr = NET_CONN_ERR_CONN_HALF;
        }
    } else {
       *perr = NET_CONN_ERR_CONN_NONE;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetConn_PoolStatGet()
*
* Description : Get network connection statistics pool.
*
* Argument(s) : none.
*
* Return(s)   : Network connection statistics pool, if NO errors.
*
*               NULL               statistics pool, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) NetConn_PoolStatGet() blocked until network initialization completes; return NULL
*                   statistics pool.
*
*               (2) 'NetConn_PoolStat' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

NET_STAT_POOL  NetConn_PoolStatGet (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR         cpu_sr;
#endif
#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    NET_ERR        err;
#endif
    NET_STAT_POOL  stat_pool;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, ...                            */
        NetStat_PoolClr(&stat_pool, &err);
        return (stat_pool);                                     /* ... rtn NULL stat pool (see Note #1).                */
    }
#endif


    CPU_CRITICAL_ENTER();
    stat_pool = NetConn_PoolStat;
    CPU_CRITICAL_EXIT();

    return (stat_pool);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetConn_PoolStatResetMaxUsed()
*
* Description : Reset network connection statistics pool's maximum number of entries used.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetConn_PoolStatResetMaxUsed (void)
{
    NET_ERR  err;


    NetStat_PoolResetUsedMax(&NetConn_PoolStat, &err);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetConn_Srch()
*
* Description : (1) Search connection list for network connection with specific local &/or remote addresses :
*
*                   (a) Get    network connection list  head pointer
*                   (b) Search network connection list  for best-match  network connection chain
*                   (c) Search network connection chain for best-match  network connection
*                   (d) Return network connection handle identifier, if network connection     found
*                         OR
*                       Null identifier,                             if network connection NOT found
*
*               (2) Network connection are organized into connection lists & chains to expedite connection 
*                   searches.
*
*                   (a) (1) For each connection family & protocol type, one connection list is maintained.
*                       
*                       (2) (A) Each connection list is maintained as a list of connection lists.  Each 
*                               connection list is uniquely organized by the local port number.
*
*                               In other words, a connection list's top-level connections are each the heads 
*                               of their own connection list & each have a local port number that is unique 
*                               from all the other connection lists.
*
*                           (B) Each connection list maintained within a connection list is also referred to 
*                               as a connection chain.
*
*                       (3) Each connection chain is an organization of connections that all share the same
*                           local port number but different local address &/or remote address.
*
*                           In other words, connections in a connection chain share the same local port number;
*                           however, no two connections in a connection chain share the same local AND remote
*                           connection address.
*
*                       (4) In the diagram below, ...
*
*                           (A) The top horizontal row  represents the connection list's list of connection chains.
*
*                           (B) Each    vertical column represents the connections in unique     connection chains.
*
*                           (C) Each connection list is a pointer that points to the head of the connection list.
*
*                           (D) Connections' 'PrevChainPtr' & 'NextChainPtr' doubly-link each connection chain to 
*                               form the list of connection chains.
*
*                           (E) Connections' 'PrevConnPtr'  & 'NextConnPtr'  doubly-link each connection to form 
*                               a connection chain.
*
*                   (b) (1) For any connection search, the specific family/protocol connection list is searched 
*                           primarily by the local port number to find the appropriate connection chain.  If a 
*                           connection chain with the local port number is found, then each connection in the 
*                           connection chain is searched in order to find the connection with the best match.
*
*                       (2) Network connection searches are resolved in order :
*
*                           (A) From greatest number of identical connection address fields ...
*                           (B)   to least    number of identical connection address fields.
*
*                       (3) To expedite faster connection searches for recently added network connections :
*
*                           (A) New network connection chains are added at the head of a network connection list.
*                           (B) New network connections       are added at the head of a network connection chain.
*
*                           See also 'NetConn_Add()  Note #1'.
*$PAGE*
*
*                                            |                                                             |
*                                            |<---------------- List of Connection Chains ---------------->|
*                                            |                      (see Note #2a4A)                       |
*
*                             New connection chains inserted at head
*                              of connection list (see Note #2b3A);
*                              new connections inserted at head of
*                                connection chain (see Note #2b3B)
*
*                                               |              NextChainPtr
*                                               |            (see Note #2a4D)
*                                               v                    |
*                                                                    |
*                           Head of          -------       -------   v   -------       -------       -------
*                       Connection List ---->|     |------>|     |------>|     |------>|     |------>|     |
*                                            |     |       |     |       |     |       |     |       |     |
*                      (see Note #2a4C)      |     |<------|     |<------|     |<------|     |<------|     |
*                                            -------       -------   ^   -------       -------       -------
*                                              | ^                   |                   | ^
*                                              | |                   |                   | |
*                                              v |             PrevChainPtr              v |
*               ---                          -------         (see Note #2a4D)          -------
*                ^                           |     |                                   |     |
*                |                           |     |                                   |     |
*                |                           |     |                                   |     |
*                |                           -------                                   -------
*                |                             | ^                                       | ^
*                |           NextConnPtr ----> | |                                       | | <----  PrevConnPtr
*                |        (see Note #2a4E)     v |                                       v |     (see Note #2a4E)
*                                            -------                                   -------
*      Connections organized                 |     |                                   |     |
*     into a connection chain                |     |                                   |     |
*        (see Note #2a4B)                    |     |                                   |     |
*                                            -------                                   -------
*                |                             | ^
*                |                             | |
*                |                             v |
*                |                           -------
*                |                           |     |
*                |                           |     |
*                v                           |     |
*               ---                          -------
*
*
* Argument(s) : family                  Network connection family type.
*
*               protocol_ix             Network connection protocol index.
*
*               paddr_local             Pointer to local  address (see Note #3).
*
*               paddr_remote            Pointer to remote address (see Note #3).
*
*               addr_len                Length  of search addresses (in octets).
*
*               pconn_id_transport      Pointer to variable that will receive the returned network
*                                           connection's transport   handle identifier.
*
*               pconn_id_app            Pointer to variable that will receive the returned network
*                                           connection's application handle identifier.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_CONN_NONE              NO   network connection found.
*
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection list family.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*                               NET_CONN_ERR_INVALID_ADDR           Invalid network connection address.
*                               NET_CONN_ERR_INVALID_ADDR_LEN       Invalid network connection address length.
*
*                                                                   ------ RETURNED BY NetConn_ChainSrch() : ------
*                               NET_CONN_ERR_CONN_HALF              Half network connection found --
*                                                                       local            addresses match.
*                               NET_CONN_ERR_CONN_HALF_WILDCARD     Half network connection found --
*                                                                       local & wildcard addresses match.
*                               NET_CONN_ERR_CONN_FULL              Full network connection found --
*                                                                       local & remote   addresses match.
*                               NET_CONN_ERR_CONN_FULL_WILDCARD     Full network connection found --
*                                                                       local & wildcard addresses match
*                                                                             & remote   addresses match.
*$PAGE*
* Return(s)   : Handle identifier of network connection with specific local &/or remote address, if found.
*
*               NET_CONN_ID_NONE,                                                                otherwise.
*
* Caller(s)   : NetSock_RxPktDemux(),
*               NetSock_BindHandler(),
*               NetSock_ConnHandlerAddrRemoteValidate(),
*               NetTCP_RxPktDemuxSeg().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (3) Network connection addresses maintained in network-order.
*
*               (4) Pointers to variables that return values MUST be initialized to return PRIOR to all
*                   other validation or function handling in case of any error(s).
*********************************************************************************************************
*/

NET_CONN_ID  NetConn_Srch (NET_CONN_FAMILY        family,
                           NET_CONN_PROTOCOL_IX   protocol_ix,
                           CPU_INT08U            *paddr_local,
                           CPU_INT08U            *paddr_remote,
                           NET_CONN_ADDR_LEN      addr_len,
                           NET_CONN_ID           *pconn_id_transport,
                           NET_CONN_ID           *pconn_id_app,
                           NET_ERR               *perr)
{
    NET_CONN     **pconn_list;
    NET_CONN      *pconn_chain;
    NET_CONN      *pconn;
    NET_CONN_ID    conn_id;
    CPU_INT08U     addr_wildcard[NET_CONN_CFG_ADDR_LEN];
    CPU_INT08U    *paddr_wildcard;


                                                                /* Init conn id's for err or failed srch (see Note #4). */
    if (pconn_id_transport != (NET_CONN_ID *)0) {
       *pconn_id_transport  =  NET_CONN_ID_NONE;
    }
    if (pconn_id_app       != (NET_CONN_ID *)0) {
       *pconn_id_app        =  NET_CONN_ID_NONE;
    }


                                                                /* --------------- VALIDATE LOCAL ADDR ---------------- */
    if (paddr_local == (CPU_INT08U *)0) {
       *perr =  NET_CONN_ERR_INVALID_ADDR;
        return (NET_CONN_ID_NONE);
    }


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ---------------- VALIDATE ADDR LEN ----------------- */
    if (addr_len != NET_CONN_CFG_ADDR_LEN) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidConnAddrLenCtr);
       *perr =  NET_CONN_ERR_INVALID_ADDR_LEN;
        return (NET_CONN_ID_NONE);
    }
#else
   (void)&addr_len;                                             /* Prevent compiler warning.                            */
#endif


/*$PAGE*/
                                                                /* ------ VALIDATE/CFG CONN LIST FAMILY/PROTOCOL ------ */
    paddr_wildcard = (CPU_INT08U *)0;                           /* Clr wildcard addr.                                   */

    switch (family) {
#if (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
        case NET_CONN_FAMILY_IP_V4_SOCK:
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
             switch (protocol_ix) {
                 case NET_CONN_PROTOCOL_IX_IP_V4_UDP:
#ifdef  NET_TCP_MODULE_PRESENT
                 case NET_CONN_PROTOCOL_IX_IP_V4_TCP:
#endif
                      break;


                 default:
                      NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidProtocolIxCtr);
                     *perr = NET_CONN_ERR_INVALID_PROTOCOL_IX;
                      return (NET_CONN_ID_NONE);                /* Prevent 'break NOT reachable' compiler warning.      */
             }
#endif

             if (NetConn_AddrWildCardAvail == DEF_YES) {        /* Cfg wildcard addr.                                   */
                 Mem_Copy((void     *)&addr_wildcard[0],
                          (void     *)paddr_local,
                          (CPU_SIZE_T) NET_CONN_CFG_ADDR_LEN);

                 Mem_Copy((void     *)&addr_wildcard[NET_CONN_ADDR_IP_IX_ADDR],
                          (void     *)&NetConn_AddrWildCard[0],
                          (CPU_SIZE_T) NET_CONN_ADDR_IP_LEN_ADDR);

                 paddr_wildcard = &addr_wildcard[0];
             }
             break;
#endif

        case NET_CONN_FAMILY_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidFamilyCtr);
            *perr = NET_CONN_ERR_INVALID_FAMILY;
             return (NET_CONN_ID_NONE);                         /* Prevent 'break NOT reachable' compiler warning.      */
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (protocol_ix >= NET_CONN_CFG_PROTOCOL_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidProtocolIxCtr);
       *perr =  NET_CONN_ERR_INVALID_PROTOCOL_IX;
        return (NET_CONN_ID_NONE);
    }
#endif


                                                                /* ---------------- SRCH NET CONN LIST ---------------- */
    pconn_list  = &NetConn_ConnListHead[protocol_ix];
    pconn_chain =  NetConn_ListSrch((NET_CONN_FAMILY)family,
                                    (NET_CONN     **)pconn_list,
                                    (CPU_INT08U    *)paddr_local);
    if (pconn_chain == (NET_CONN *)0) {
       *perr =  NET_CONN_ERR_CONN_NONE;
        return (NET_CONN_ID_NONE);                              /* NO net conn chain found.                             */
    }


                                                                /* ---------------- SRCH NET CONN CHAIN --------------- */
    pconn = NetConn_ChainSrch((NET_CONN  **)pconn_list,
                              (NET_CONN   *)pconn_chain,
                              (CPU_INT08U *)paddr_local,
                              (CPU_INT08U *)paddr_wildcard,
                              (CPU_INT08U *)paddr_remote,
                              (NET_ERR    *)perr);
    if (pconn == (NET_CONN *)0) {                               /* NO net conn       found.                             */
                                                                /* Rtn err from NetConn_ChainSrch().                    */
        return (NET_CONN_ID_NONE);
    }


                                                                /* If net conn       found, rtn conn id's.              */
    if (pconn_id_transport != (NET_CONN_ID *)0) {
       *pconn_id_transport  =  pconn->ID_Transport;
    }
    if (pconn_id_app       != (NET_CONN_ID *)0) {
       *pconn_id_app        =  pconn->ID_App;
    }

    conn_id = pconn->ID;


    return (conn_id);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetConn_ListAdd()
*
* Description : (1) Add a network connection into a connection list :
*
*                   (a) Get network connection's appropriate connection list
*                   (b) Get network connection's appropriate connection chain
*                   (c) Add network connection into          connection list
*
*
* Argument(s) : conn_id     Handle identifier of network connection to add.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE                   Network connection successfully
*                                                                       added to connection list.
*                               NET_CONN_ERR_INVALID_FAMILY         Invalid network connection family.
*                               NET_CONN_ERR_INVALID_PROTOCOL_IX    Invalid network connection list protocol index.
*                               NET_CONN_ERR_INVALID_ADDR           Invalid network connection address.
*
*                                                                   ------- RETURNED BY NetConn_IsUsed() : --------
*                               NET_ERR_INIT_INCOMPLETE             Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN           Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED               Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_BindHandler(),
*               NetSock_ConnHandlerDatagram(),
*               NetSock_ConnHandlerStream(),
*               NetTCP_RxPktConnHandlerListen().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (2) Network connection addresses maintained in network-order.
*
*               (3) Local address MUST be configured prior to network connection list add.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetConn_ListAdd (NET_CONN_ID   conn_id,
                       NET_ERR      *perr)
{
    NET_CONN               *pconn;
    NET_CONN               *pconn_chain;
    NET_CONN              **pconn_list;
    NET_CONN_PROTOCOL_IX    protocol_ix;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }
#endif

    pconn = &NetConn_Tbl[conn_id];
                                                                /* ------ VALIDATE/CFG CONN LIST FAMILY/PROTOCOL ------ */
    switch (pconn->Family) {
#if (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
        case NET_CONN_FAMILY_IP_V4_SOCK:
             break;
#endif

        case NET_CONN_FAMILY_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidFamilyCtr);
            *perr = NET_CONN_ERR_INVALID_FAMILY;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

                                                                /* ----------- VALIDATE NET CONN LOCAL ADDR ----------- */
    if (pconn->AddrLocalValid != DEF_YES) {
       *perr = NET_CONN_ERR_INVALID_ADDR;
        return;
    }


                                                                /* ---------------- GET NET CONN LIST ----------------- */
    protocol_ix = pconn->ProtocolIx;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (protocol_ix >= NET_CONN_CFG_PROTOCOL_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidProtocolIxCtr);
       *perr = NET_CONN_ERR_INVALID_PROTOCOL_IX;
        return;
    }
#endif

                                                                /* ---------- GET NET CONN'S CONN LIST CHAIN ---------- */
    pconn_list  = &NetConn_ConnListHead[protocol_ix];
    pconn_chain =  NetConn_ListSrch((NET_CONN_FAMILY) pconn->Family,
                                    (NET_CONN     **) pconn_list,
                                    (CPU_INT08U    *)&pconn->AddrLocal[0]);

                                                                /* --------- ADD NET CONN INTO NET CONN LIST ---------- */
    NetConn_Add(pconn_list, pconn_chain, pconn);


   *perr = NET_CONN_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetConn_ListUnlink()
*
* Description : Unlink a network connection from a connection list.
*
* Argument(s) : conn_id     Handle identifier of network connection to unlink.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE               Network connection successfully unlinked
*                                                                   from connection list.
*
*                                                               ---- RETURNED BY NetConn_IsUsed() : ----
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_BindHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetConn_ListUnlink (NET_CONN_ID   conn_id,
                          NET_ERR      *perr)
{
    NET_CONN  *pconn;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* -------------- VALIDATE NET CONN USED -------------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }
#endif

                                                                /* -------- UNLINK NET CONN FROM NET CONN LIST -------- */
    pconn = &NetConn_Tbl[conn_id];
    NetConn_Unlink(pconn);


   *perr = NET_CONN_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         NetConn_ListSrch()
*
* Description : Search a network connection list for network connection chain with specific local port.
*
* Argument(s) : family          Network connection family type.
*               ------          Argument checked   in NetConn_Srch()
*                                                     NetConn_ListAdd().
*
*               pconn_list      Pointer to a connection list.
*               ----------      Argument validated in NetConn_Srch()
*                                                     NetConn_ListAdd().
*
*               paddr_local     Pointer to local address.
*               -----------     Argument checked   in NetConn_Srch()
*                                                     NetConn_ListAdd().
*
* Return(s)   : Pointer to connection chain with specific local port, if found.
*
*               Pointer to NULL,                                      otherwise.
*
* Caller(s)   : NetConn_Srch(),
*               NetConn_ListAdd().
*
* Note(s)     : (1) Network connection addresses maintained in network-order.
*
*               (2) (a) Assumes ALL connection lists' network connections' local addresses are valid.
*
*                   (b) Any connections whose local addresses are NOT valid are closed & unlinked 
*                       from their respective connection list.  Such connections may be accessed by 
*                       the application layer but will NOT correctly receive demultiplexed packets 
*                       due to invalid/incomplete connection address(s).
*
*               (3) #### Multiple network connection families NOT yet supported.
*********************************************************************************************************
*/
/*$PAGE*/
static  NET_CONN  *NetConn_ListSrch (NET_CONN_FAMILY    family,
                                     NET_CONN         **pconn_list,
                                     CPU_INT08U        *paddr_local)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR              cpu_sr;
#endif
#if (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
    CPU_INT08U         *pport_nbr_addr_local;
    CPU_INT08U         *pport_nbr_conn_chain;
    NET_CONN_ADDR_LEN   port_nbr_len;
#endif
    NET_CONN           *pconn_chain;
    NET_CONN           *pconn_chain_next;
    NET_CONN           *pconn_next;
    CPU_BOOLEAN         found;
    CPU_INT16U          th;


    switch (family) {                                                       /* Get local port nbr to srch.              */
#if (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
        case NET_CONN_FAMILY_IP_V4_SOCK:
             pport_nbr_addr_local = paddr_local + NET_CONN_ADDR_IP_IX_PORT;
              port_nbr_len        =               NET_CONN_ADDR_IP_LEN_PORT;
             break;
#endif

        case NET_CONN_FAMILY_NONE:                                          /* See Note #3.                             */
        default:
             return ((NET_CONN *)0);                                        /* Prevent 'break NOT reachable' warning.   */
    }


                                                                            /* ---------- SRCH NET CONN LIST ---------- */
    pconn_chain = (NET_CONN *)*pconn_list;                                  /* Start @ list head.                       */
    found       =  DEF_NO;

    while ((pconn_chain != (NET_CONN *)0) &&                                /* Srch ALL net conn chains ..              */
           (found       ==  DEF_NO)) {                                      /* .. until net conn chain found.           */

        pconn_chain_next = (NET_CONN *)pconn_chain->NextChainPtr;

        if (pconn_chain->AddrLocalValid == DEF_YES) {                       /* If conn chain's local addr valid, ...    */
            switch (pconn_chain->Family) {                                  /* ... cmp local port nbrs.                 */
#if (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
                case NET_CONN_FAMILY_IP_V4_SOCK:
                     pport_nbr_conn_chain = &pconn_chain->AddrLocal[0] + NET_CONN_ADDR_IP_IX_PORT;
                     found                =  Mem_Cmp((void     *)pport_nbr_addr_local,
                                                     (void     *)pport_nbr_conn_chain,
                                                     (CPU_SIZE_T) port_nbr_len);
                     break;
#endif

                case NET_CONN_FAMILY_NONE:                                  /* See Note #3.                             */
                default:
                     break;
            }

            if (found != DEF_YES) {                                         /* If NOT found, ...                        */
                pconn_chain = pconn_chain_next;                             /* ... adv to next conn chain.              */
            }

        } else {                                                            /* If conn chain's local addr NOT valid, ...*/
            pconn_next =  pconn_chain->PrevConnPtr;
            NetConn_Close(pconn_chain);                                     /* ... close conn (see Note #2b).           */

            if (pconn_next != (NET_CONN *)0) {                              /* If any conns rem in conn chain,     ...  */
                pconn_chain =  pconn_next;                                  /* ...      adv to next conn in chain; ...  */
            } else {
                pconn_chain =  pconn_chain_next;                            /* ... else adv to next conn chain.         */
            }
        }
    }


    if (found == DEF_YES) {                                                 /* If net conn chain found, ..              */
        pconn_chain->ConnChainAccessedCtr++;                                /* .. inc conn chain access ctr.            */
        CPU_CRITICAL_ENTER();
        th = NetConn_AccessedTh_nbr;
        CPU_CRITICAL_EXIT();
        if (pconn_chain->ConnChainAccessedCtr > th) {                       /* If conn chain accessed > th           .. */
            pconn_chain->ConnChainAccessedCtr = 0;
            if (pconn_chain != *pconn_list) {                               /* .. & conn chain NOT    @ conn list head, */
                                                                            /* .. promote conn chain to conn list head. */
                NetConn_ChainUnlink(pconn_list, pconn_chain);
                NetConn_ChainInsert(pconn_list, pconn_chain);                        
            }
        }
    }


    return (pconn_chain);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetConn_ChainSrch()
*
* Description : Search a network connection chain for network connection with specific local & remote 
*               addresses.
*
* Argument(s) : pconn_list          Pointer to a connection list.
*               ----------          Argument validated in NetConn_Srch().
*
*               pconn_chain         Pointer to a connection chain.
*               -----------         Argument validated in NetConn_Srch().
*
*               paddr_local         Pointer to local          address.
*               -----------         Argument checked   in NetConn_Srch().
*
*               paddr_wildcard      Pointer to local wildcard address.
*               --------------      Argument validated in NetConn_Srch().
*
*               paddr_remote        Pointer to remote         address.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_CONN_NONE              NO   network connection found.
*                               NET_CONN_ERR_CONN_HALF              Half network connection found --
*                                                                       local            addresses match.
*                               NET_CONN_ERR_CONN_HALF_WILDCARD     Half network connection found --
*                                                                       local & wildcard addresses match.
*                               NET_CONN_ERR_CONN_FULL              Full network connection found --
*                                                                       local & remote   addresses match.
*                               NET_CONN_ERR_CONN_FULL_WILDCARD     Full network connection found --
*                                                                       local & wildcard addresses match
*                                                                             & remote   addresses match.
*
* Return(s)   : Pointer to connection with specific local & remote address, if found.
*
*               Pointer to NULL,                                            otherwise.
*
* Caller(s)   : NetConn_Srch().
*
* Note(s)     : (1) Network connection addresses maintained in network-order.
*
*               (2) (a) Assumes ALL connection lists' network connections' local addresses are valid.
*
*                   (b) Any connections whose local addresses are NOT valid are closed & unlinked 
*                       from their respective connection list.  Such connections may be accessed by 
*                       the application layer but will NOT correctly receive demultiplexed packets 
*                       due to invalid/incomplete connection address(s).
*********************************************************************************************************
*/

static  NET_CONN  *NetConn_ChainSrch (NET_CONN    **pconn_list,
                                      NET_CONN     *pconn_chain,
                                      CPU_INT08U   *paddr_local,
                                      CPU_INT08U   *paddr_wildcard,
                                      CPU_INT08U   *paddr_remote,
                                      NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR        cpu_sr;
#endif
    NET_CONN     *pconn;
    NET_CONN     *pconn_next;
    NET_CONN     *pconn_half;
    NET_CONN     *pconn_half_wildcard;
    NET_CONN     *pconn_full_wildcard;
    CPU_INT08U   *pconn_addr_local;
    CPU_INT08U   *pconn_addr_remote;
    CPU_INT16U    th;
    CPU_BOOLEAN   found;
    CPU_BOOLEAN   found_local;
    CPU_BOOLEAN   found_local_wildcard;
    CPU_BOOLEAN   found_remote;
    CPU_BOOLEAN   addr_local_wildcard;


/*$PAGE*/
                                                                            /* --------- SRCH NET CONN CHAIN ---------- */
    pconn               = (NET_CONN *)pconn_chain;                          /* Start @ chain head.                      */
    pconn_half          = (NET_CONN *)0;
    pconn_half_wildcard = (NET_CONN *)0;
    pconn_full_wildcard = (NET_CONN *)0;
    found               =  DEF_NO;
    addr_local_wildcard =  Mem_Cmp((void     *)paddr_local,
                                   (void     *)paddr_wildcard,
                                   (CPU_SIZE_T)NET_CONN_CFG_ADDR_LEN);

    while ((pconn != (NET_CONN *)0) &&                                      /* Srch ALL net conns in chain ..           */
           (found ==  DEF_NO)) {                                            /* .. until net conn found.                 */

        pconn_next = (NET_CONN *)pconn->NextConnPtr;

        if (pconn->AddrLocalValid == DEF_YES) {                             /* If conn's local addr valid, ...          */
                                                                            /* ... cmp to conn addrs.                   */
            pconn_addr_local  = (CPU_INT08U *)&pconn->AddrLocal[0];
            pconn_addr_remote = (CPU_INT08U *)&pconn->AddrRemote[0];

            found_local       =  Mem_Cmp((void     *)paddr_local,
                                         (void     *)pconn_addr_local,
                                         (CPU_SIZE_T)NET_CONN_CFG_ADDR_LEN);
            found_remote      =  Mem_Cmp((void     *)paddr_remote,
                                         (void     *)pconn_addr_remote,
                                         (CPU_SIZE_T)NET_CONN_CFG_ADDR_LEN);

            if (found_local == DEF_YES) {                                   /* If local addrs match;                ... */
                if ((paddr_remote           != (CPU_INT08U *)0) &&          /* ... &   remote addrs avail,          ... */
                    (pconn->AddrRemoteValid ==  DEF_YES)) {

                    found = found_remote;                                   /* ... cmp remote addrs;                ... */

                } else if (pconn->AddrRemoteValid == DEF_NO) {              /* ... else if conn remote addr NOT avail,  */
                    pconn_half = pconn;                                     /* ... save half conn.                      */
                }
                                                                            /* Else if local addrs do NOT match,    ... */
            } else if ((paddr_wildcard      != (CPU_INT08U *)0) &&          /* ... & wildcard addr      avail,      ... */
                       (addr_local_wildcard ==  DEF_NO)) {                  /* ... &   local  addr !=   wildcard addr;  */
                                                                            /* ... cmp local  addr with wildcard addr.  */
                found_local_wildcard = Mem_Cmp((void     *)paddr_wildcard,
                                               (void     *)pconn_addr_local,
                                               (CPU_SIZE_T)NET_CONN_CFG_ADDR_LEN);

                if (found_local_wildcard == DEF_YES) {                      /* If local wildcard addrs match;       ... */
                    if ((paddr_remote           != (CPU_INT08U *)0) &&
                        (pconn->AddrRemoteValid ==  DEF_YES)) {

                        if (found_remote == DEF_YES) {                      /* ... & remote addrs avail & match,    ... */
                            pconn_full_wildcard = pconn;                    /* ... save full-wildcard conn;         ... */
                        }

                    } else if (pconn->AddrRemoteValid == DEF_NO) {          /* ... else if conn remote addr NOT avail,  */
                        pconn_half_wildcard = pconn;                        /* ... save half-wildcard conn.             */
                    }
                }
            }
        
        } else {                                                            /* If conn's local addr NOT valid, ...      */
            NetConn_Close(pconn);                                           /* ... close conn (see Note #2b).           */
        }
        
        if (found != DEF_YES) {                                             /* If NOT found, adv to next conn.          */
            pconn  = pconn_next;
        }
    }


    if (found == DEF_YES) {                                                 /* Full conn found.                         */
       *perr   = NET_CONN_ERR_CONN_FULL;

    } else if (pconn_full_wildcard != (NET_CONN *)0) {                      /* Full conn found with wildcard addr.      */
       *perr   = NET_CONN_ERR_CONN_FULL_WILDCARD;
        pconn  = pconn_full_wildcard;

    } else if (pconn_half          != (NET_CONN *)0) {                      /* Half conn found.                         */
       *perr   = NET_CONN_ERR_CONN_HALF;
        pconn  = pconn_half;

    } else if (pconn_half_wildcard != (NET_CONN *)0) {                      /* Half conn found with wildcard addr.      */
       *perr   = NET_CONN_ERR_CONN_HALF_WILDCARD;
        pconn  = pconn_half_wildcard;
    
    } else {                                                                /* NO   conn found.                         */
       *perr   = NET_CONN_ERR_CONN_NONE;
    }


/*$PAGE*/
    if (pconn != (NET_CONN *)0) {                                           /* If net conn found, ..                    */
        pconn->ConnAccessedCtr++;                                           /* .. inc conn access ctr.                  */
        CPU_CRITICAL_ENTER();
        th = NetConn_AccessedTh_nbr;
        CPU_CRITICAL_EXIT();
        if (pconn->ConnAccessedCtr > th) {                                  /* If conn accessed > th               ..   */
            pconn->ConnAccessedCtr = 0;
            if (pconn != pconn_chain) {                                     /* .. & conn  NOT   @ conn chain head, ..   */
                                                                            /* .. promote conn to conn chain head.      */
                NetConn_Unlink(pconn);
                NetConn_Add(pconn_list, pconn_chain, pconn);
            }
        }
    }


    return (pconn);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetConn_ChainInsert()
*
* Description : (1) Insert a connection chain into a connection list :
*
*                   (a) Insert connection chain at the head of the connection list
*                   (b) Set each chain connection's connection list
*
*
* Argument(s) : pconn_list      Pointer to a connection list.
*               ----------      Argument validated in NetConn_ListSrch().
*
*               pconn_chain     Pointer to a connection chain.
*               -----------     Argument validated in NetConn_ListSrch().
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_ListSrch().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetConn_ChainInsert (NET_CONN  **pconn_list,
                                   NET_CONN   *pconn_chain)
{
    NET_CONN  *pconn;
    NET_CONN  *pconn_next;

                                                                /* ------- INSERT CONN CHAIN AT CONN LIST HEAD -------- */
    pconn_chain->PrevChainPtr = (NET_CONN *)0;
    pconn_chain->NextChainPtr = (NET_CONN *)(*pconn_list);

    if (*pconn_list != (NET_CONN *)0) {                         /* If conn list NOT empty,                         ...  */
       (*pconn_list)->PrevChainPtr = pconn_chain;               /* ... insert conn chain before cur conn list head ...  */
    }
   *pconn_list = pconn_chain;                                   /*     Set    conn chain as     new conn list head.     */

                                                                /* ------------ SET CHAIN CONNS' CONN LIST ------------ */
    pconn = pconn_chain;
    while (pconn  != (NET_CONN *)0) {
        pconn->ConnList = pconn_list;
        pconn_next      = pconn->NextConnPtr;
        pconn           = pconn_next;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetConn_ChainUnlink()
*
* Description : (1) Unlink a connection chain from a connection list :
*
*                   (a) Unlink connection chain
*                   (b) Clear each chain connection's connection list
*
*
* Argument(s) : pconn_list      Pointer to a connection list.
*               ----------      Argument validated in NetConn_ListSrch().
*
*               pconn_chain     Pointer to a connection chain.
*               -----------     Argument validated in NetConn_ListSrch().
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_ListSrch().
*
* Note(s)     : (2) Since NetConn_ChainUnlink() called ONLY to remove & then re-link connection chains,
*                   it is NOT necessary to clear the entry's previous & next chain pointers.  However,
*                   pointers cleared to NULL shown for correctness & completeness.
*********************************************************************************************************
*/

static  void  NetConn_ChainUnlink (NET_CONN  **pconn_list,
                                   NET_CONN   *pconn_chain)
{
    NET_CONN  *pconn_chain_prev;
    NET_CONN  *pconn_chain_next;
    NET_CONN  *pconn;
    NET_CONN  *pconn_next;

                                                                /* --------- UNLINK CONN CHAIN FROM CONN LIST --------- */
    pconn_chain_prev = pconn_chain->PrevChainPtr;
    pconn_chain_next = pconn_chain->NextChainPtr;
                                                                /* Point prev conn chain to next conn chain.            */
    if (pconn_chain_prev != (NET_CONN *)0) {
        pconn_chain_prev->NextChainPtr = pconn_chain_next;
    } else {
       *pconn_list                     = pconn_chain_next;
    }
                                                                /* Point next conn chain to prev conn chain.            */
    if (pconn_chain_next != (NET_CONN *)0) {
        pconn_chain_next->PrevChainPtr = pconn_chain_prev;
    }

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)                     /* Clr conn chain's chain ptrs (see Note #2).           */
    pconn_chain->PrevChainPtr = (NET_CONN *)0;
    pconn_chain->NextChainPtr = (NET_CONN *)0;
#endif

                                                                /* ------------ CLR CHAIN CONNS' CONN LIST ------------ */
    pconn = pconn_chain;
    while (pconn  != (NET_CONN *)0) {
        pconn->ConnList = (NET_CONN **)0;
        pconn_next      = (NET_CONN  *)pconn->NextConnPtr;
        pconn           = (NET_CONN  *)pconn_next;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetConn_Add()
*
* Description : (1) Add a network connection into a network connection list :
*
*                   (a) New network connection chains are added at the head of a network connection list.
*                   (b) New network connections       are added at the head of a network connection chain.
*
*
* Argument(s) : pconn_list      Pointer to a network connection list.
*               ----------      Argument validated in NetConn_ListAdd(),
*                                                     NetConn_ListSrch().
*
*               pconn_chain     Pointer to a network connection chain.
*               -----------     Argument validated in NetConn_ListAdd(),
*                                                     NetConn_ListSrch().
*
*               pconn           Pointer to a network connection.
*               -----           Argument validated in NetConn_ListAdd(),
*                                                     NetConn_ListSrch().
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_ListAdd(),
*               NetConn_ChainSrch().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetConn_Add (NET_CONN  **pconn_list,
                           NET_CONN   *pconn_chain,
                           NET_CONN   *pconn)
{
    NET_CONN  *pconn_chain_prev;
    NET_CONN  *pconn_chain_next;


    if (pconn_chain == (NET_CONN *)0) {                             /* If conn chain empty, ...                         */
                                                                    /* ... insert conn as new conn chain (see Note #1a).*/
        pconn->PrevChainPtr = (NET_CONN *)0;
        pconn->NextChainPtr = (NET_CONN *)(*pconn_list);
        pconn->PrevConnPtr  = (NET_CONN *)0;
        pconn->NextConnPtr  = (NET_CONN *)0;

        if (*pconn_list != (NET_CONN *)0) {                         /* If conn list NOT empty,                   ...    */
           (*pconn_list)->PrevChainPtr = pconn;                     /* ... insert conn before cur conn list head ...    */
        }
       *pconn_list = pconn;                                         /*     Set    conn as     new conn list head.       */

    } else {                                                        /* Else add net conn into existing conn chain ...   */
                                                                    /* ... as new conn chain head (see Note #1b).       */
        pconn_chain_prev          = (NET_CONN *)pconn_chain->PrevChainPtr;
        pconn_chain_next          = (NET_CONN *)pconn_chain->NextChainPtr;
        pconn_chain->PrevChainPtr = (NET_CONN *)0;
        pconn_chain->NextChainPtr = (NET_CONN *)0;
        pconn_chain->PrevConnPtr  = (NET_CONN *)pconn;

        pconn->PrevConnPtr        = (NET_CONN *)0;
        pconn->NextConnPtr        = (NET_CONN *)pconn_chain;

        pconn->PrevChainPtr       = (NET_CONN *)pconn_chain_prev;
        if (pconn_chain_prev != (NET_CONN *)0) {
            pconn_chain_prev->NextChainPtr = pconn;
        } else {
           *pconn_list                     = pconn;
        }

        pconn->NextChainPtr   = (NET_CONN *)pconn_chain_next;
        if (pconn_chain_next != (NET_CONN *)0) {
            pconn_chain_next->PrevChainPtr = pconn;
        }
                                                                    /* Inherit conn chain accessed ctr.                 */
        pconn->ConnChainAccessedCtr        = pconn_chain->ConnChainAccessedCtr;
        pconn_chain->ConnChainAccessedCtr  = 0;
    }

    pconn->ConnList = pconn_list;                                   /* Mark conn's conn list ownership.                 */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetConn_Unlink()
*
* Description : Unlink a network connection from its network connection list.
*
* Argument(s) : pconn       Pointer to a network connection.
*               -----       Argument validated in NetConn_ListUnlink(),
*                                                 NetConn_ListSrch(),
*                                                 NetConn_FreeHandler().
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_ListUnlink(),
*               NetConn_ChainSrch(),
*               NetConn_FreeHandler().
*
* Note(s)     : (1) Since NetConn_Unlink() called ONLY to remove & then re-link or free network
*                   connections, it is NOT necessary to clear the entry's previous & next chain/
*                   connection pointers.  However, pointers cleared to NULL shown for correctness
*                   & completeness.
*********************************************************************************************************
*/
/*$PAGE*/
static  void  NetConn_Unlink (NET_CONN  *pconn)
{
    NET_CONN  **pconn_list;
    NET_CONN   *pconn_prev;
    NET_CONN   *pconn_next;
    NET_CONN   *pconn_chain_prev;
    NET_CONN   *pconn_chain_next;


    pconn_list = pconn->ConnList;
    if (pconn_list == (NET_CONN **)0) {                                 /* If net conn NOT in conn list, ...            */
        return;                                                         /* ... exit unlink.                             */
    }

                                                                        /* ----- UNLINK NET CONN FROM CONN CHAIN ------ */
    pconn_prev = pconn->PrevConnPtr;
    pconn_next = pconn->NextConnPtr;
                                                                
    if (pconn_prev != (NET_CONN *)0) {                                  /* If prev net conn non-NULL, ...               */
                                                                        /* ... unlink conn from middle of conn chain.   */
        pconn_prev->NextConnPtr = pconn_next;                           /* Point prev conn to next conn.                */

        if (pconn_next != (NET_CONN *)0) {                              /* If next net conn non-NULL, ...               */
            pconn_next->PrevConnPtr = pconn_prev;                       /* ... point next conn to prev conn.            */
        }

    } else {                                                            /* Else unlink conn chain head.                 */
        pconn_chain_prev = (NET_CONN *)pconn->PrevChainPtr;
        pconn_chain_next = (NET_CONN *)pconn->NextChainPtr;

        if (pconn_next  != (NET_CONN *)0) {                             /* If next conn in conn chain non-NULL, ...     */
                                                                        /* ... promote next conn to conn chain head.    */
            pconn_next->PrevChainPtr = pconn_chain_prev;
            if (pconn_chain_prev != (NET_CONN *)0) {
                pconn_chain_prev->NextChainPtr = pconn_next;
            } else {
               *pconn_list                     = pconn_next;
            }

            pconn_next->NextChainPtr = pconn_chain_next;
            if (pconn_chain_next != (NET_CONN *)0) {
                pconn_chain_next->PrevChainPtr = pconn_next;
            }

            pconn_next->PrevConnPtr          = (NET_CONN *)0;
                                                                        /* Inherit conn chain accessed ctr.             */
            pconn_next->ConnChainAccessedCtr =  pconn->ConnChainAccessedCtr;
            pconn->ConnChainAccessedCtr      =  0;

        } else {                                                        /* Else remove conn list entirely.              */
            if (pconn_chain_prev != (NET_CONN *)0) {
                pconn_chain_prev->NextChainPtr = pconn_chain_next;
            } else {
               *pconn_list                     = pconn_chain_next;
            }

            if (pconn_chain_next != (NET_CONN *)0) {
                pconn_chain_next->PrevChainPtr = pconn_chain_prev;
            }
        }
    }

    pconn->ConnList     = (NET_CONN **)0;                               /* Clr net conn's conn list ownership.          */

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)                             /* Clr net conn's chain/conn ptrs (see Note #1).*/
    pconn->PrevChainPtr = (NET_CONN  *)0;
    pconn->NextChainPtr = (NET_CONN  *)0;
    pconn->PrevConnPtr  = (NET_CONN  *)0;
    pconn->NextConnPtr  = (NET_CONN  *)0;
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetConn_Close()
*
* Description : (1) Close a network connection :
*
*                   (a) Update network connection close statistic(s)
*
*                   (b) Close  network connection(s) :
*                       (1) Close application connection
*                       (2) Close transport   connection
*
*                   (c) Free   network connection, if necessary
*
*
* Argument(s) : pconn       Pointer to a network connection.
*               -----       Argument checked in NetConn_CloseAllConnsHandler(),
*                                               NetConn_ListSrch(),
*                                               NetConn_ChainSrch().
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_CloseAllConnsHandler(),
*               NetConn_ListSrch(),
*               NetConn_ChainSrch().
*
* Note(s)     : (2) Network connection's handle identifier MUST be obtained PRIOR to any network 
*                   connection validation &/or close operations.
*
*               (3) (a) Network connection(s) MAY  already be closed BEFORE any  network connection 
*                       close operations & MUST be validated as used BEFORE each network connection 
*                       close operation.
*
*                   (b) Network connection SHOULD already be closed AFTER application & transport 
*                       layer connections have both closed.
*
*                       See also 'NetConn_CloseFromApp()        Note #1c'
*                              & 'NetConn_CloseFromTransport()  Note #1c'.
*********************************************************************************************************
*/

static  void  NetConn_Close (NET_CONN  *pconn)
{
    NET_CONN_ID  conn_id;
    NET_ERR      err;
    CPU_BOOLEAN  used;

                                                                /* --------------- UPDATE CLOSE STATS ----------------- */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrCloseCtr);

                                                                /* ------------------ CLOSE CONN(S) ------------------- */
    conn_id = pconn->ID;                                        /* Get net conn id (see Note #2).                       */

    used = NetConn_IsUsed(conn_id, &err);
    if (used == DEF_YES) {                                      /* If net conn used (see Note #3a), ...                 */
        NetConn_CloseApp(pconn);                                /* ... close app conn.                                  */
    }

    used = NetConn_IsUsed(conn_id, &err);
    if (used == DEF_YES) {                                      /* If net conn used (see Note #3a), ...                 */
        NetConn_CloseTransport(pconn);                          /* ... close transport conn.                            */
    }


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------ FREE NET CONN ------------------- */
    used = NetConn_IsUsed(conn_id, &err);
    if (used == DEF_YES) {                                      /* If net conn used (see Note #3b), ...                 */
        NetConn_FreeHandler(pconn);                             /* ... free net conn.                                   */
    }
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetConn_CloseApp()
*
* Description : (1) Close a network connection's application connection :
*
*                   (a) Free  network     connection from application clone connection, if necessary
*                   (b) Close application connection
*
*
* Argument(s) : pconn       Pointer to a network connection.
*               -----       Argument validated in NetConn_Close(),
*                                                 NetConn_CloseFromTransport().
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_Close(),
*               NetConn_CloseFromTransport().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetConn_CloseApp (NET_CONN  *pconn)
{
                                                                /* ---------------- CLOSE APP CONN(S) ----------------- */
#if (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)         
    NetSock_FreeConnFromSock((NET_SOCK_ID)pconn->ID_AppClone,   /* Free net conn from clone sock conn (see Note #1a).   */
                             (NET_CONN_ID)pconn->ID);    

    NetSock_CloseFromConn((NET_SOCK_ID)pconn->ID_App);          /* Close sock app conn (see Note #1b).                  */
#endif

   (void)&pconn;                                                /* Prevent possible compiler warning.                   */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetConn_CloseTransport()
*
* Description : Close a network connection's transport connection.
*
* Argument(s) : pconn       Pointer to a network connection.
*               -----       Argument validated in NetConn_Close(),
*                                                 NetConn_CloseFromApp().
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_Close(),
*               NetConn_CloseFromApp().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetConn_CloseTransport (NET_CONN  *pconn)
{
                                                                /* --------------- CLOSE TRANSPORT CONN -------------- */
#if    (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
#ifdef  NET_TCP_MODULE_PRESENT
    NetTCP_ConnCloseFromConn((NET_TCP_CONN_ID)pconn->ID_Transport);
#endif
#endif

   (void)&pconn;                                                /* Prevent possible compiler warning.                   */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetConn_FreeHandler()
*
* Description : (1) Free a network connection :
*
*                   (a) Remove network connection from a   network connection list :
*                       (1) Update network connection list pointer(s)                       See Note #2
*                       (2) Unlink network connection from network connection list
*
*                   (b) Clear  network connection values
*                   (c) Free   network connection back to network connection pool
*                   (d) Update network connection pool statistics
*
*
* Argument(s) : pconn       Pointer to a network connection.
*               -----       Argument validated in NetConn_Free(),
*                                                 NetConn_Close(),
*                                                 NetConn_CloseFromApp(),
*                                                 NetConn_CloseFromTransport().
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_Free(),
*               NetConn_Close(),
*               NetConn_CloseFromApp(),
*               NetConn_CloseFromTransport(),
*
* Note(s)     : (2) Since network connection close handlers execute asynchronously to NetConn_FreeHandler(),
*                   network connection list pointers ('NetConn_ConnListChainPtr', 'NetConn_ConnListConnPtr',
*                   etc.) MUST be coordinated with NetConn_FreeHandler() to avoid network connection list
*                   corruption :
*
*                   (a) Whenever a network connection list pointer connection is freed, network connection
*                       list pointers MUST be advanced to the next valid & available network connection in
*                       the appropriate network connection list.
*
*                   See also 'NetConn_CloseAllConnsHandler()  Note #3'.
*********************************************************************************************************
*/

static  void  NetConn_FreeHandler (NET_CONN  *pconn)
{
    NET_CONN  *pnext;
    NET_ERR    err;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------ VALIDATE TYPE ------------------- */
    if (pconn->Type != NET_CONN_TYPE_CONN) {
        NetConn_Discard(pconn);
        NET_CTR_ERR_INC(Net_ErrCtrs.NetConn_ErrInvalidTypeCtr);
        return;
    }
#endif
                                                                /* -------- REMOVE NET CONN FROM NET CONN LIST -------- */
                                                                /* If net conn is next conn list conn to update, ...    */
                                                                /* ... adv to skip this net conn (see Note #2a).        */
    if (pconn == NetConn_ConnListNextChainPtr) {
        pnext                        = NetConn_ConnListNextChainPtr->NextChainPtr;
        NetConn_ConnListNextChainPtr = pnext;
    }
    if (pconn == NetConn_ConnListNextConnPtr) {
        pnext                        = NetConn_ConnListNextConnPtr->NextConnPtr;
        NetConn_ConnListNextConnPtr  = pnext;
    }

    NetConn_Unlink(pconn);                                      /* Unlink net conn from net conn list.                  */


                                                                /* ------------------- CLR NET CONN ------------------- */
    DEF_BIT_CLR(pconn->Flags, NET_CONN_FLAG_USED);              /* Set net conn as NOT used.                            */
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    NetConn_Clr(pconn);
#endif

                                                                /* ------------------ FREE NET CONN ------------------- */
    pconn->NextConnPtr = NetConn_PoolPtr;
    NetConn_PoolPtr    = pconn;

                                                                /* ------------ UPDATE NET CONN POOL STATS ------------ */
    NetStat_PoolEntryUsedDec(&NetConn_PoolStat, &err);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetConn_Clr()
*
* Description : Clear network connection controls.
*
* Argument(s) : pconn       Pointer to a network connection.
*               -----       Argument validated in NetConn_Init(),
*                                                 NetConn_Get(),
*                                                 NetConn_FreeHandler().
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_Init(),
*               NetConn_Get(),
*               NetConn_FreeHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetConn_Clr (NET_CONN  *pconn)
{
    pconn->PrevChainPtr         = (NET_CONN  *)0;
    pconn->NextChainPtr         = (NET_CONN  *)0;
    pconn->PrevConnPtr          = (NET_CONN  *)0;
    pconn->NextConnPtr          = (NET_CONN  *)0;
    pconn->ConnList             = (NET_CONN **)0;
    pconn->ID_App               =  NET_CONN_ID_NONE;
    pconn->ID_AppClone          =  NET_CONN_ID_NONE;
    pconn->ID_Transport         =  NET_CONN_ID_NONE;
    pconn->Family               =  NET_CONN_FAMILY_NONE;
    pconn->ProtocolIx           =  NET_CONN_PROTOCOL_IX_NONE;
    pconn->AddrLocalValid       =  DEF_NO;
    pconn->AddrRemoteValid      =  DEF_NO;
    pconn->ConnChainAccessedCtr =  0;
    pconn->ConnAccessedCtr      =  0;
    pconn->Flags                =  NET_CONN_FLAG_NONE;

    Mem_Clr((void     *)&pconn->AddrLocal[0],
            (CPU_SIZE_T) NET_CONN_CFG_ADDR_LEN);
    Mem_Clr((void     *)&pconn->AddrRemote[0],
            (CPU_SIZE_T) NET_CONN_CFG_ADDR_LEN);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetConn_Discard()
*
* Description : (1) Discard an invalid/corrupted network  connection :
*
*                   (a) Discard network connection from available network connection pool       See Note #2
*                   (b) Update  network connection pool statistics
*
*               (2) Assumes network connection is invalid/corrupt & MUST be removed.  Network 
*                   connection removed simply by NOT returning the network connection back to
*                   the network connection pool.
*
*
* Argument(s) : pconn       Pointer to an invalid/corrupted network connection.
*
* Return(s)   : none.
*
* Caller(s)   : NetConn_Get(),
*               NetConn_FreeHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
static  void  NetConn_Discard (NET_CONN  *pconn)
{
    NET_ERR  stat_err;

                                                                /* ----------------- DISCARD NET CONN ----------------- */
   (void)&pconn;                                                /* Prevent compiler warning (see Note #2).              */

                                                                /* --------------- UPDATE DISCARD STATS --------------- */
    NetStat_PoolEntryLostInc(&NetConn_PoolStat, &stat_err);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_conn.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of net conn module include (see Note #1).        */

