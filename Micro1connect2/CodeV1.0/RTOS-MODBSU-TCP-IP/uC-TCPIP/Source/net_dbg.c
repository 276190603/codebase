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
*                                       NETWORK DEBUG MANAGEMENT
*
* Filename      : net_dbg.c
* Version       : V1.92
* Programmer(s) : ITJ
*                 SR
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_DBG_MODULE
#include  <net.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                          GLOBAL NETWORK MODULE DEBUG INFORMATION CONSTANTS
*
* Note(s) : (1) Many data-size constants ideally defined with network data-type sizes; defined as 
*               'CPU_INT32U' because many network data-type sizes NOT defined at time of declaration
*               (see 'net.h  NETWORK INCLUDE FILES').
*********************************************************************************************************
*/

const  CPU_INT16U  Net_Version                        =  NET_VERSION;


#if (NET_DBG_CFG_INFO_EN              == DEF_ENABLED)

                                                                                    /* ------ NET MISC MODULE(S) ------ */
const  CPU_INT08U  Net_CfgInitCfgVals                 =  NET_CFG_INIT_CFG_VALS;
const  CPU_INT08U  Net_CfgOptimize                    =  NET_CFG_OPTIMIZE;

const  CPU_INT32U  Net_CfgTxSuspendTimeout_ms         =  NET_CFG_TX_SUSPEND_TIMEOUT_MS;

                                                                                    /* ------- NET ERR  MODULE -------- */
const  CPU_INT08U  NetErr_CfgArgChkExtEn              =  NET_ERR_CFG_ARG_CHK_EXT_EN;
const  CPU_INT08U  NetErr_CfgArgChkDbgEn              =  NET_ERR_CFG_ARG_CHK_DBG_EN;

                                                                                    /* ------- NET DBG  MODULE -------- */
const  CPU_INT08U  NetDbg_CfgInfoEn                   =  NET_DBG_CFG_INFO_EN;
const  CPU_INT08U  NetDbg_CfgStatusEn                 =  NET_DBG_CFG_STATUS_EN;
const  CPU_INT08U  NetDbg_CfgStatusRsrcLostEn         =  NET_DBG_CFG_STATUS_RSRC_LOST_EN;
const  CPU_INT08U  NetDbg_CfgStatusRsrcLoEn           =  NET_DBG_CFG_STATUS_RSRC_LO_EN;
const  CPU_INT08U  NetDbg_CfgMemClrEn                 =  NET_DBG_CFG_MEM_CLR_EN;
const  CPU_INT08U  NetDbg_CfgTestEn                   =  NET_DBG_CFG_TEST_EN;
const  CPU_INT08U  NetDbg_CfgMonTaskEn                =  NET_DBG_CFG_MON_TASK_EN;

                                                                                    /* ------- NET CTR  MODULE -------- */
const  CPU_INT08U  NetCtr_CfgStatEn                   =  NET_CTR_CFG_STAT_EN;
const  CPU_INT08U  NetCtr_CfgErrEn                    =  NET_CTR_CFG_ERR_EN;

                                                                                    /* ------- NET TMR  MODULE -------- */
const  CPU_INT16U  NetTmr_CfgNbrTmr                   =  NET_TMR_CFG_NBR_TMR;
const  CPU_INT16U  NetTmr_CfgTaskFreq                 =  NET_TMR_CFG_TASK_FREQ;


                                                                                    /* ------- NET BUF  MODULE -------- */
const  CPU_INT16U  NetBuf_CfgNbrSmall                 =  NET_BUF_CFG_NBR_SMALL;
#if (NET_BUF_CFG_NBR_SMALL            >  0)
const  CPU_INT32U  NetBuf_CfgDataSizeSmall            =  NET_BUF_CFG_DATA_SIZE_SMALL;
#endif

const  CPU_INT16U  NetBuf_CfgNbrLarge                 =  NET_BUF_CFG_NBR_LARGE;
#if (NET_BUF_CFG_NBR_LARGE            >  0)
const  CPU_INT32U  NetBuf_CfgDataSizeLarge            =  NET_BUF_CFG_DATA_SIZE_LARGE;
#endif


#ifdef  NET_CONN_MODULE_PRESENT
                                                                                    /* ------- NET CONN MODULE -------- */
const  CPU_INT16U  NetConn_CfgNbrConn                 =  NET_CONN_CFG_NBR_CONN;
#endif


/*$PAGE*/
#ifdef  NET_ARP_MODULE_PRESENT
                                                                                    /* ------- NET ARP  MODULE -------- */
const  CPU_INT16U  NetARP_CfgNbrCache                 =  NET_ARP_CFG_NBR_CACHE;

const  CPU_INT08U  NetARP_CfgAddrFilterEn             =  NET_ARP_CFG_ADDR_FLTR_EN;
#endif


                                                                                    /* ------- NET ICMP MODULE -------- */
const  CPU_INT08U  NetICMP_CfgTxSrcQuenchEn           =  NET_ICMP_CFG_TX_SRC_QUENCH_EN;
#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN    == DEF_ENABLED)
const  CPU_INT16U  NetICMP_CfgTxSrcQuenchSize         =  NET_ICMP_CFG_TX_SRC_QUENCH_SIZE;
#endif


                                                                                    /* ------- NET UDP  MODULE -------- */
const  CPU_INT08U  NetUDP_CfgAppAPI_Sel               =  NET_UDP_CFG_APP_API_SEL;
const  CPU_INT08U  NetUDP_CfgRxChkSumDiscardEn        =  NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN;
const  CPU_INT08U  NetUDP_CfgTxChkSumEn               =  NET_UDP_CFG_TX_CHK_SUM_EN;



#ifdef  NET_TCP_MODULE_PRESENT
                                                                                    /* ------- NET TCP  MODULE -------- */
const  CPU_INT16U  NetTCP_CfgNbrConn                  =  NET_TCP_CFG_NBR_CONN;

const  CPU_INT16U  NetTCP_CfgRxWinSize_octet          =  NET_TCP_CFG_RX_WIN_SIZE_OCTET;
const  CPU_INT16U  NetTCP_CfgTxWinSize_octet          =  NET_TCP_CFG_TX_WIN_SIZE_OCTET;

const  CPU_INT32U  NetTCP_CfgTimeoutConnMaxSeg_sec    =  NET_TCP_CFG_TIMEOUT_CONN_MAX_SEG_SEC;
const  CPU_INT32U  NetTCP_CfgTimeoutConnAckDly_ms     =  NET_TCP_CFG_TIMEOUT_CONN_ACK_DLY_MS;
const  CPU_INT32U  NetTCP_CfgTimeoutConnRxQ_ms        =  NET_TCP_CFG_TIMEOUT_CONN_RX_Q_MS;
const  CPU_INT32U  NetTCP_CfgTimeoutConnTxQ_ms        =  NET_TCP_CFG_TIMEOUT_CONN_TX_Q_MS;
#endif



#ifdef  NET_SOCK_MODULE_PRESENT
                                                                                    /* ------- NET SOCK MODULE -------- */
const  CPU_INT16U  NetSock_CfgNbrSock                 =  NET_SOCK_CFG_NBR_SOCK;

const  CPU_INT08U  NetSock_CfgBlockSel                =  NET_SOCK_CFG_BLOCK_SEL;

const  CPU_INT08U  NetSock_CfgSelEn                   =  NET_SOCK_CFG_SEL_EN;

const  CPU_INT16U  NetSock_CfgRandomPortNbrBase       =  NET_SOCK_CFG_PORT_NBR_RANDOM_BASE;
const  CPU_INT16U  NetSock_NbrRandomPortNbr           =  NET_SOCK_PORT_NBR_RANDOM_NBR;

const  CPU_INT32U  NetSock_CfgTimeoutRxQ_ms           =  NET_SOCK_CFG_TIMEOUT_RX_Q_MS;

const  CPU_INT08U  NetSock_CfgStreamTypeEn            =  NET_SOCK_CFG_TYPE_STREAM_EN;
#if (NET_SOCK_CFG_TYPE_STREAM_EN      == DEF_ENABLED)
const  CPU_INT16U  NetSock_CfgConnAcceptQ_MaxSize     =  NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX;
const  CPU_INT32U  NetSock_CfgTimeoutConnReq_ms       =  NET_SOCK_CFG_TIMEOUT_CONN_REQ_MS;
const  CPU_INT32U  NetSock_CfgTimeoutConnAccept_ms    =  NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_MS;
const  CPU_INT32U  NetSock_CfgTimeoutConnClose_ms     =  NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_MS;
#endif
#endif


                                                                                    /* ------- NET BSD  MODULE -------- */
const  CPU_INT08U  NetBSD_API_En                      =  NET_BSD_CFG_API_EN;



                                                                                    /* ------- NET OS   MODULE -------- */
const  CPU_INT32U  NetOS_TimeoutMin_tick              =  NET_OS_TIMEOUT_MIN_TICK;
const  CPU_INT32U  NetOS_TimeoutMax_tick              =  NET_OS_TIMEOUT_MAX_TICK;
const  CPU_INT32U  NetOS_TimeoutMin_sec               =  NET_OS_TIMEOUT_MIN_SEC;
const  CPU_INT32U  NetOS_TimeoutMax_sec               =  NET_OS_TIMEOUT_MAX_SEC;
const  CPU_INT32U  NetOS_TimeoutMin_ms                =  NET_OS_TIMEOUT_MIN_mS;
const  CPU_INT32U  NetOS_TimeoutMax_ms                =  NET_OS_TIMEOUT_MAX_mS;
const  CPU_INT32U  NetOS_TimeoutMin_us                =  NET_OS_TIMEOUT_MIN_uS;
const  CPU_INT32U  NetOS_TimeoutMax_us                =  NET_OS_TIMEOUT_MAX_uS;


#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                              GLOBAL NETWORK MODULE DATA SIZE CONSTANTS
*
* Note(s) : (1) Many data-size constants ideally defined with network data-type sizes; defined as 
*               'CPU_INT32U' because many network data-type sizes NOT defined at time of declaration
*               (see 'net.h  NETWORK INCLUDE FILES').
*
*           (2) Network protocol suite total data size does NOT include data sizes from the following 
*               network files :
*
*                   (a) Custom Library Files            See 'net.h  NETWORK INCLUDE FILES  Note #3a'
*                   (b) BSP Port Files                  See 'net.h  NETWORK INCLUDE FILES  Note #1a4'
*                   (c) CPU Port Files                  See 'net.h  NETWORK INCLUDE FILES  Note #1c'
*                   (d) OS  Port Files                  See 'net.h  NETWORK INCLUDE FILES  Note #1d'
*                   (e) IF  Port Files                  See 'net.h  NETWORK INCLUDE FILES  Note #1e'
*                   (f) NIC Port Files                  See 'net.h  NETWORK INCLUDE FILES  Note #1f'
*********************************************************************************************************
*/

#if (NET_DBG_CFG_INFO_EN              == DEF_ENABLED)

                                                                                    /* ---------- NET MODULE ---------- */
const  CPU_INT32U  Net_DataSize                       =  0

                                                      +  sizeof(Net_InitDone)

#if (NET_CFG_LOAD_BAL_EN              == DEF_ENABLED)
                                                      +  sizeof(Net_RxPktCtr)
                                                      +  sizeof(Net_TxSuspendCtr)
#endif
                                                      ;


                                                                                    /* ------ NET MISC MODULE(S) ------ */
const  CPU_INT32U  NetTS_Size                         =  sizeof(NET_TS);


                                                                                    /* ------- NET ERR  MODULE -------- */
const  CPU_INT32U  NetErr_Size                        =  sizeof(NET_ERR);



                                                                                    /* ------- NET DBG  MODULE -------- */
const  CPU_INT32U  NetDbg_StatusSize                  =  sizeof(NET_DBG_STATUS);

const  CPU_INT32U  NetDbg_DataSize                    =  0

#if    (NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED)
#if    (NET_BUF_CFG_NBR_SMALL         >  0)        
                                                      +  sizeof(NetDbg_RsrcBufSmallThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcBufSmallThLoHyst_nbr)
#endif
#if    (NET_BUF_CFG_NBR_LARGE         >  0)        
                                                      +  sizeof(NetDbg_RsrcBufLargeThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcBufLargeThLoHyst_nbr)
#endif
                                                      +  sizeof(NetDbg_RsrcTmrThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcTmrThLoHyst_nbr)
#ifdef  NET_CONN_MODULE_PRESENT
                                                      +  sizeof(NetDbg_RsrcConnThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcConnThLoHyst_nbr)
#endif
#ifdef  NET_ARP_MODULE_PRESENT
                                                      +  sizeof(NetDbg_RsrcARP_CacheThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcARP_CacheThLoHyst_nbr)
#endif
#ifdef  NET_TCP_MODULE_PRESENT
                                                      +  sizeof(NetDbg_RsrcTCP_ConnThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcTCP_ConnThLoHyst_nbr)
#endif
#ifdef  NET_SOCK_MODULE_PRESENT
                                                      +  sizeof(NetDbg_RsrcSockThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcSockThLoHyst_nbr)
#endif
#endif

#if (NET_DBG_CFG_MON_TASK_EN          == DEF_ENABLED)
                                                      +  sizeof(NetDbg_MonTaskTmr)
                                                      +  sizeof(NetDbg_MonTaskTime_sec)
                                                      +  sizeof(NetDbg_MonTaskTime_tick)

                                                      +  sizeof(NetDbg_MonTaskStatusRsrcLost)
                                                      +  sizeof(NetDbg_MonTaskStatusRsrcLo)
#endif
                                                      ;



/*$PAGE*/
                                                                                    /* ------- NET CTR  MODULE -------- */
const  CPU_INT32U  NetCtr_Size                        =  sizeof(NET_CTR);

#if (NET_CTR_CFG_STAT_EN              == DEF_ENABLED)
const  CPU_INT32U  NetCtr_StatCtrsSize                =  sizeof(Net_StatCtrs);
#endif
#if (NET_CTR_CFG_ERR_EN               == DEF_ENABLED)
const  CPU_INT32U  NetCtr_ErrCtrsSize                 =  sizeof(Net_ErrCtrs);
#endif


const  CPU_INT32U  NetCtr_DataSize                    =  0
#if (NET_CTR_CFG_STAT_EN              == DEF_ENABLED)
                                                      +  sizeof(Net_StatCtrs)
#endif
#if (NET_CTR_CFG_ERR_EN               == DEF_ENABLED)
                                                      +  sizeof(Net_ErrCtrs)
#endif
                                                      ;



                                                                                    /* ------- NET STAT MODULE -------- */
const  CPU_INT32U  NetStat_PoolSize                   =  sizeof(NET_STAT_POOL);


const  CPU_INT32U  NetStat_DataSize                   =  0;



                                                                                    /* ------- NET TMR  MODULE -------- */
const  CPU_INT32U  NetTmr_TickSize                    =  sizeof(NET_TMR_TICK);

const  CPU_INT32U  NetTmr_Size                        =  sizeof(NET_TMR);
const  CPU_INT32U  NetTmr_TblSize                     =  sizeof(NetTmr_Tbl);


const  CPU_INT32U  NetTmr_DataSize                    =  0

                                                      +  sizeof(NetTmr_Tbl)
                                                      +  sizeof(NetTmr_PoolPtr)
                                                      +  sizeof(NetTmr_PoolStat)

                                                      +  sizeof(NetTmr_TaskListHead)
                                                      +  sizeof(NetTmr_TaskListPtr)

                                                      ;


/*$PAGE*/
                                                                                    /* ------- NET BUF  MODULE -------- */
const  CPU_INT32U  NetBuf_HdrSize                     =  sizeof(NET_BUF_HDR);

#if (NET_BUF_CFG_NBR_SMALL            >  0)
const  CPU_INT32U  NetBuf_SmallSize                   =  sizeof(NET_BUF_SMALL);
const  CPU_INT32U  NetBuf_SmallTblSize                =  sizeof(NetBuf_SmallTbl);
#endif

#if (NET_BUF_CFG_NBR_LARGE            >  0)
const  CPU_INT32U  NetBuf_LargeSize                   =  sizeof(NET_BUF_LARGE);
const  CPU_INT32U  NetBuf_LargeTblSize                =  sizeof(NetBuf_LargeTbl);
#endif


const  CPU_INT32U  NetBuf_DataSize                    =  0

#if (NET_BUF_CFG_NBR_SMALL            >  0)
                                                      +  sizeof(NetBuf_SmallTbl)
                                                      +  sizeof(NetBuf_SmallPoolPtr)
                                                      +  sizeof(NetBuf_SmallPoolStat)
#endif

#if (NET_BUF_CFG_NBR_LARGE            >  0)
                                                      +  sizeof(NetBuf_LargeTbl)
                                                      +  sizeof(NetBuf_LargePoolPtr)
                                                      +  sizeof(NetBuf_LargePoolStat)
#endif
                                                      ;



#ifdef  NET_CONN_MODULE_PRESENT
                                                                                    /* ------- NET CONN MODULE -------- */
const  CPU_INT32U  NetConn_ID_Size                    =  sizeof(NET_CONN_ID);
const  CPU_INT32U  NetConn_AddrLenSize                =  sizeof(NET_CONN_ADDR_LEN);

const  CPU_INT32U  NetConn_Size                       =  sizeof(NET_CONN);
const  CPU_INT32U  NetConn_TblSize                    =  sizeof(NetConn_Tbl);

const  CPU_INT32U  NetConn_ConnListTblSize            =  sizeof(NetConn_ConnListHead);

const  CPU_INT32U  NetConn_AddrWildCardSize           =  sizeof(NetConn_AddrWildCard);


const  CPU_INT32U  NetConn_DataSize                   =  0

                                                      +  sizeof(NetConn_Tbl)
                                                      +  sizeof(NetConn_PoolPtr)
                                                      +  sizeof(NetConn_PoolStat)

                                                      +  sizeof(NetConn_ConnListHead)

                                                      +  sizeof(NetConn_ConnListChainPtr)
                                                      +  sizeof(NetConn_ConnListConnPtr)
                                                      +  sizeof(NetConn_ConnListNextChainPtr)
                                                      +  sizeof(NetConn_ConnListNextConnPtr)

                                                      +  sizeof(NetConn_AccessedTh_nbr)

                                                      +  sizeof(NetConn_AddrWildCardAvail)
                                                      +  sizeof(NetConn_AddrWildCard)

                                                      ;
#endif



/*$PAGE*/
#ifdef  NET_ARP_MODULE_PRESENT
                                                                                    /* ------- NET ARP  MODULE -------- */
const  CPU_INT32U  NetARP_HdrSize                     =  sizeof(NET_ARP_HDR);

const  CPU_INT32U  NetARP_CacheSize                   =  sizeof(NET_ARP_CACHE);
const  CPU_INT32U  NetARP_CacheTblSize                =  sizeof(NetARP_CacheTbl);


const  CPU_INT32U  NetARP_DataSize                    =  0

                                                      +  sizeof(NetARP_HostAddrPtrHW)
                                                      +  sizeof(NetARP_HostAddrPtrProtocol)
                                                      +  sizeof(NetARP_HostAddrProtocolConflict)

                                                      +  sizeof(NetARP_CacheTbl)
                                                      +  sizeof(NetARP_CachePoolPtr)
                                                      +  sizeof(NetARP_CachePoolStat)

#if (NET_CTR_CFG_STAT_EN              == DEF_ENABLED)
                                                      +  sizeof(NetARP_CacheSrchCtr_lo)
                                                      +  sizeof(NetARP_CacheSrchCtr_hi)
                                                      +  sizeof(NetARP_CacheFoundCtr_lo)
                                                      +  sizeof(NetARP_CacheFoundCtr_hi)
                                                      +  sizeof(NetARP_CacheFoundPct)
#endif

                                                      +  sizeof(NetARP_CacheListHead)
                                                      +  sizeof(NetARP_CacheListTail)
                                                      +  sizeof(NetARP_CacheTimeout_sec)
                                                      +  sizeof(NetARP_CacheTimeout_tick)
                                                      +  sizeof(NetARP_CacheAccessedTh_nbr)

                                                      +  sizeof(NetARP_ReqMaxAttempts_nbr)
                                                      +  sizeof(NetARP_ReqTimeout_sec)
                                                      +  sizeof(NetARP_ReqTimeout_tick)

                                                      ;
#endif



/*$PAGE*/
                                                                                    /* ------- NET IP   MODULE -------- */
const  CPU_INT32U  NetIP_HdrSize                      =  sizeof(NET_IP_HDR);

const  CPU_INT32U  NetIP_TOS_Size                     =  sizeof(NET_IP_TOS);
const  CPU_INT32U  NetIP_TTL_Size                     =  sizeof(NET_IP_TTL);
const  CPU_INT32U  NetIP_AddrSize                     =  sizeof(NET_IP_ADDR);

const  CPU_INT32U  NetIP_OptSize                      =  sizeof(NET_IP_OPT_SIZE);
const  CPU_INT32U  NetIP_OptSrcRouteSize              =  sizeof(NET_IP_OPT_SRC_ROUTE);
const  CPU_INT32U  NetIP_OptTS_RouteSize              =  sizeof(NET_IP_OPT_TS_ROUTE);
const  CPU_INT32U  NetIP_OptTS_Size                   =  sizeof(NET_IP_OPT_TS);


const  CPU_INT32U  NetIP_DataSize                     =  0

                                                      +  sizeof(NetIP_AddrThisHost)
                                                      +  sizeof(NetIP_AddrThisHostNetOrder)
                                                      +  sizeof(NetIP_AddrThisHostSubnetMask)
                                                      +  sizeof(NetIP_AddrThisHostSubnetMaskHost)
                                                      +  sizeof(NetIP_AddrThisHostSubnetNet)
                                                      +  sizeof(NetIP_AddrDfltGateway)

                                                      +  sizeof(NetIP_FragReasmListsHead)
                                                      +  sizeof(NetIP_FragReasmListsTail)
                                                      +  sizeof(NetIP_FragReasmTimeout_sec)
                                                      +  sizeof(NetIP_FragReasmTimeout_tick)

                                                      +  sizeof(NetIP_TxID_Ctr)

                                                      ;



                                                                                    /* ------- NET ICMP MODULE -------- */
const  CPU_INT32U  NetICMP_HdrSize                    =  sizeof(NET_ICMP_HDR);
const  CPU_INT32U  NetICMP_HdrErrSize                 =  sizeof(NET_ICMP_HDR_ERR);
const  CPU_INT32U  NetICMP_HdrParamProbSize           =  sizeof(NET_ICMP_HDR_PARAM_PROB);
const  CPU_INT32U  NetICMP_HdrEchoSize                =  sizeof(NET_ICMP_HDR_ECHO);
const  CPU_INT32U  NetICMP_HdrTS_Size                 =  sizeof(NET_ICMP_HDR_TS);
const  CPU_INT32U  NetICMP_HdrAddrMaskSize            =  sizeof(NET_ICMP_HDR_ADDR_MASK);

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN    == DEF_ENABLED)
const  CPU_INT32U  NetICMP_TxSrcQuenchSize            =  sizeof(NET_ICMP_TX_SRC_QUENCH);
const  CPU_INT32U  NetICMP_TxSrcQuenchTblSize         =  sizeof(NetICMP_TxSrcQuenchTbl);
#endif


const  CPU_INT32U  NetICMP_DataSize                   =  0

#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN    == DEF_ENABLED)
                                                      +  sizeof(NetICMP_TxSrcQuenchTbl)
                                                      +  sizeof(NetICMP_TxSrcQuenchPoolStat)
                                                      +  sizeof(NetICMP_TxSrcQuenchPoolPtr)
                                                      +  sizeof(NetICMP_TxSrcQuenchListHead)
                                                      +  sizeof(NetICMP_TxSrcQuenchListTail)
                                                      +  sizeof(NetICMP_TxSrcQuenchTxTh_nbr)
#endif

                                                      +  sizeof(NetICMP_TxSeqNbrCtr)

                                                      ;



/*$PAGE*/
                                                                                    /* ------- NET UDP  MODULE -------- */
const  CPU_INT32U  NetUDP_HdrSize                     =  sizeof(NET_UDP_HDR);
const  CPU_INT32U  NetUDP_PseudoHdrSize               =  sizeof(NET_UDP_PSEUDO_HDR);

const  CPU_INT32U  NetUDP_PortNbrSize                 =  sizeof(NET_UDP_PORT_NBR);


const  CPU_INT32U  NetUDP_DataSize                    =  0;



#ifdef  NET_TCP_MODULE_PRESENT
                                                                                    /* ------- NET TCP  MODULE -------- */
const  CPU_INT32U  NetTCP_HdrSize                     =  sizeof(NET_TCP_HDR);
const  CPU_INT32U  NetTCP_PseudoHdrSize               =  sizeof(NET_TCP_PSEUDO_HDR);

const  CPU_INT32U  NetTCP_PortNbrSize                 =  sizeof(NET_TCP_PORT_NBR);
const  CPU_INT32U  NetTCP_SeqNbrSize                  =  sizeof(NET_TCP_SEQ_NBR);
const  CPU_INT32U  NetTCP_SegLenSize                  =  sizeof(NET_TCP_SEG_SIZE);
const  CPU_INT32U  NetTCP_WinSize                     =  sizeof(NET_TCP_WIN_SIZE);
const  CPU_INT32U  NetTCP_OptSize                     =  sizeof(NET_TCP_OPT_SIZE);

const  CPU_INT32U  NetTCP_ConnID_Size                 =  sizeof(NET_TCP_CONN_ID);
const  CPU_INT32U  NetTCP_ConnSize                    =  sizeof(NET_TCP_CONN);
const  CPU_INT32U  NetTCP_ConnTblSize                 =  sizeof(NetTCP_ConnTbl);


const  CPU_INT32U  NetTCP_DataSize                    =  0

                                                      +  sizeof(NetTCP_ConnTbl)
                                                      +  sizeof(NetTCP_ConnPoolPtr)
                                                      +  sizeof(NetTCP_ConnPoolStat)

                                                      +  sizeof(NetTCP_TxSeqNbrCtr)

                                                      ;
#endif




/*$PAGE*/
#ifdef  NET_SOCK_MODULE_PRESENT
                                                                                    /* ------- NET SOCK MODULE -------- */
const  CPU_INT32U  NetSock_ID_Size                    =  sizeof(NET_SOCK_ID);
const  CPU_INT32U  NetSock_RtnCodeSize                =  sizeof(NET_SOCK_RTN_CODE);

const  CPU_INT32U  NetSock_AddrLenSize                =  sizeof(NET_SOCK_ADDR_LEN);
const  CPU_INT32U  NetSock_SockAddrSize               =  sizeof(NET_SOCK_ADDR);

const  CPU_INT32U  NetSock_Size                       =  sizeof(NET_SOCK);
const  CPU_INT32U  NetSock_TblSize                    =  sizeof(NetSock_Tbl);

const  CPU_INT32U  NetSock_Q_LenSize                  =  sizeof(NET_SOCK_Q_SIZE);
const  CPU_INT32U  NetSock_RandomPortNbrQ_Size        =  sizeof(NetSock_RandomPortNbrQ);


const  CPU_INT32U  NetSock_DataSize                   =  0

                                                      +  sizeof(NetSock_Tbl)
                                                      +  sizeof(NetSock_PoolPtr)
                                                      +  sizeof(NetSock_PoolStat)

                                                      +  sizeof(NetSock_RandomPortNbrQ)
                                                      +  sizeof(NetSock_RandomPortNbrQ_HeadIx)
                                                      +  sizeof(NetSock_RandomPortNbrQ_TailIx)
                                                      +  sizeof(NetSock_RandomPortNbrQ_NbrUsed)

                                                      ;
#endif



                                                                                    /* ------- NET BSD  MODULE -------- */
#if    (NET_BSD_CFG_API_EN            == DEF_ENABLED)
#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_FAMILY           == NET_SOCK_FAMILY_IP_V4)
const  CPU_INT32U  NetBSD_IP_to_Str_ArraySize         =  sizeof(NetBSD_IP_to_Str_Array);
#endif
#endif
#endif


const  CPU_INT32U  NetBSD_DataSize                    =  0

#if    (NET_BSD_CFG_API_EN            == DEF_ENABLED)
#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_FAMILY           == NET_SOCK_FAMILY_IP_V4)
                                                      +  sizeof(NetBSD_IP_to_Str_Array)
#endif
#endif
#endif
                                                      ;



/*$PAGE*/
                                                                                    /* ------ NET TOTAL DATA SIZE ----- */
                                                                                    /* Calc total data size ...         */
const  CPU_INT32U  Net_DataSizeTot                    =  0                          /* ... (see Note #2).               */


                                                                                    /* ---------- NET MODULE ---------- */
                                                      +  sizeof(Net_InitDone)

#if (NET_CFG_LOAD_BAL_EN              == DEF_ENABLED)
                                                      +  sizeof(Net_RxPktCtr)
                                                      +  sizeof(Net_TxSuspendCtr)
#endif



                                                                                    /* ------- NET DBG  MODULE -------- */
#if    (NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED)
#if    (NET_BUF_CFG_NBR_SMALL         >  0)     
                                                      +  sizeof(NetDbg_RsrcBufSmallThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcBufSmallThLoHyst_nbr)
#endif
#if    (NET_BUF_CFG_NBR_LARGE         >  0)     
                                                      +  sizeof(NetDbg_RsrcBufLargeThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcBufLargeThLoHyst_nbr)
#endif
                                                      +  sizeof(NetDbg_RsrcTmrThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcTmrThLoHyst_nbr)
#ifdef  NET_CONN_MODULE_PRESENT
                                                      +  sizeof(NetDbg_RsrcConnThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcConnThLoHyst_nbr)
#endif
#ifdef  NET_ARP_MODULE_PRESENT
                                                      +  sizeof(NetDbg_RsrcARP_CacheThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcARP_CacheThLoHyst_nbr)
#endif
#ifdef  NET_TCP_MODULE_PRESENT
                                                      +  sizeof(NetDbg_RsrcTCP_ConnThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcTCP_ConnThLoHyst_nbr)
#endif
#ifdef  NET_SOCK_MODULE_PRESENT
                                                      +  sizeof(NetDbg_RsrcSockThLo_nbr)
                                                      +  sizeof(NetDbg_RsrcSockThLoHyst_nbr)
#endif
#endif

#if (NET_DBG_CFG_MON_TASK_EN          == DEF_ENABLED)
                                                      +  sizeof(NetDbg_MonTaskTmr)
                                                      +  sizeof(NetDbg_MonTaskTime_sec)
                                                      +  sizeof(NetDbg_MonTaskTime_tick)

                                                      +  sizeof(NetDbg_MonTaskStatusRsrcLost)
                                                      +  sizeof(NetDbg_MonTaskStatusRsrcLo)
#endif



/*$PAGE*/
                                                                                    /* ------- NET CTR  MODULE -------- */
#if (NET_CTR_CFG_STAT_EN              == DEF_ENABLED)
                                                      +  sizeof(Net_StatCtrs)
#endif
#if (NET_CTR_CFG_ERR_EN               == DEF_ENABLED)
                                                      +  sizeof(Net_ErrCtrs)
#endif



                                                                                    /* ------- NET STAT MODULE -------- */



                                                                                    /* ------- NET TMR  MODULE -------- */
                                                      +  sizeof(NetTmr_Tbl)
                                                      +  sizeof(NetTmr_PoolPtr)
                                                      +  sizeof(NetTmr_PoolStat)

                                                      +  sizeof(NetTmr_TaskListHead)
                                                      +  sizeof(NetTmr_TaskListPtr)



                                                                                    /* ------- NET BUF  MODULE -------- */
#if (NET_BUF_CFG_NBR_SMALL            >  0)
                                                      +  sizeof(NetBuf_SmallTbl)
                                                      +  sizeof(NetBuf_SmallPoolPtr)
                                                      +  sizeof(NetBuf_SmallPoolStat)
#endif

#if (NET_BUF_CFG_NBR_LARGE            >  0)
                                                      +  sizeof(NetBuf_LargeTbl)
                                                      +  sizeof(NetBuf_LargePoolPtr)
                                                      +  sizeof(NetBuf_LargePoolStat)
#endif




#ifdef  NET_CONN_MODULE_PRESENT
                                                                                    /* ------- NET CONN MODULE -------- */
                                                      +  sizeof(NetConn_Tbl)
                                                      +  sizeof(NetConn_PoolPtr)
                                                      +  sizeof(NetConn_PoolStat)

                                                      +  sizeof(NetConn_ConnListHead)

                                                      +  sizeof(NetConn_ConnListChainPtr)
                                                      +  sizeof(NetConn_ConnListConnPtr)
                                                      +  sizeof(NetConn_ConnListNextChainPtr)
                                                      +  sizeof(NetConn_ConnListNextConnPtr)

                                                      +  sizeof(NetConn_AccessedTh_nbr)

                                                      +  sizeof(NetConn_AddrWildCardAvail)
                                                      +  sizeof(NetConn_AddrWildCard)
#endif



/*$PAGE*/
#ifdef  NET_ARP_MODULE_PRESENT
                                                                                    /* ------- NET ARP  MODULE -------- */
                                                      +  sizeof(NetARP_HostAddrPtrHW)
                                                      +  sizeof(NetARP_HostAddrPtrProtocol)
                                                      +  sizeof(NetARP_HostAddrProtocolConflict)

                                                      +  sizeof(NetARP_CacheTbl)
                                                      +  sizeof(NetARP_CachePoolPtr)
                                                      +  sizeof(NetARP_CachePoolStat)

#if (NET_CTR_CFG_STAT_EN              == DEF_ENABLED)
                                                      +  sizeof(NetARP_CacheSrchCtr_lo)
                                                      +  sizeof(NetARP_CacheSrchCtr_hi)
                                                      +  sizeof(NetARP_CacheFoundCtr_lo)
                                                      +  sizeof(NetARP_CacheFoundCtr_hi)
                                                      +  sizeof(NetARP_CacheFoundPct)
#endif

                                                      +  sizeof(NetARP_CacheListHead)
                                                      +  sizeof(NetARP_CacheListTail)
                                                      +  sizeof(NetARP_CacheTimeout_sec)
                                                      +  sizeof(NetARP_CacheTimeout_tick)
                                                      +  sizeof(NetARP_CacheAccessedTh_nbr)

                                                      +  sizeof(NetARP_ReqMaxAttempts_nbr)
                                                      +  sizeof(NetARP_ReqTimeout_sec)
                                                      +  sizeof(NetARP_ReqTimeout_tick)
#endif



                                                                                    /* ------- NET IP   MODULE -------- */
                                                      +  sizeof(NetIP_AddrThisHost)
                                                      +  sizeof(NetIP_AddrThisHostNetOrder)
                                                      +  sizeof(NetIP_AddrThisHostSubnetMask)
                                                      +  sizeof(NetIP_AddrThisHostSubnetMaskHost)
                                                      +  sizeof(NetIP_AddrThisHostSubnetNet)
                                                      +  sizeof(NetIP_AddrDfltGateway)

                                                      +  sizeof(NetIP_FragReasmListsHead)
                                                      +  sizeof(NetIP_FragReasmListsTail)
                                                      +  sizeof(NetIP_FragReasmTimeout_sec)
                                                      +  sizeof(NetIP_FragReasmTimeout_tick)

                                                      +  sizeof(NetIP_TxID_Ctr)



                                                                                    /* ------- NET ICMP MODULE -------- */
#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN    == DEF_ENABLED)
                                                      +  sizeof(NetICMP_TxSrcQuenchTbl)
                                                      +  sizeof(NetICMP_TxSrcQuenchPoolStat)
                                                      +  sizeof(NetICMP_TxSrcQuenchPoolPtr)
                                                      +  sizeof(NetICMP_TxSrcQuenchListHead)
                                                      +  sizeof(NetICMP_TxSrcQuenchListTail)
                                                      +  sizeof(NetICMP_TxSrcQuenchTxTh_nbr)
#endif

                                                      +  sizeof(NetICMP_TxSeqNbrCtr)




/*$PAGE*/
                                                                                    /* ------- NET UDP  MODULE -------- */



#ifdef  NET_TCP_MODULE_PRESENT
                                                                                    /* ------- NET TCP  MODULE -------- */
                                                      +  sizeof(NetTCP_ConnTbl)
                                                      +  sizeof(NetTCP_ConnPoolPtr)
                                                      +  sizeof(NetTCP_ConnPoolStat)

                                                      +  sizeof(NetTCP_TxSeqNbrCtr)
#endif



#ifdef  NET_SOCK_MODULE_PRESENT
                                                                                    /* ------- NET SOCK MODULE -------- */
                                                      +  sizeof(NetSock_Tbl)
                                                      +  sizeof(NetSock_PoolPtr)
                                                      +  sizeof(NetSock_PoolStat)

                                                      +  sizeof(NetSock_RandomPortNbrQ)
                                                      +  sizeof(NetSock_RandomPortNbrQ_HeadIx)
                                                      +  sizeof(NetSock_RandomPortNbrQ_TailIx)
                                                      +  sizeof(NetSock_RandomPortNbrQ_NbrUsed)
#endif



                                                                                    /* ------- NET BSD  MODULE -------- */
#if    (NET_BSD_CFG_API_EN            == DEF_ENABLED)
#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_FAMILY           == NET_SOCK_FAMILY_IP_V4)
                                                      +  sizeof(NetBSD_IP_to_Str_Array)
#endif
#endif
#endif



                                                      ;                             /* End of total data size calc.     */

#endif


/*$PAGE*/
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


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void            NetDbg_InitInfoConst            (void);


#if ((NET_DBG_CFG_STATUS_EN == DEF_ENABLED) && \
     (defined(NET_TCP_MODULE_PRESENT)))
static  NET_DBG_STATUS  NetDbg_ChkStatusHandlerTCP_Q    (NET_TCP_CONN  *pconn_chk,
                                                         NET_BUF       *pbuf_chk_q);

static  NET_DBG_STATUS  NetDbg_ChkStatusHandlerTCP_Q_Dup(NET_BUF       *pbuf_chk,
                                                         NET_BUF       *pbuf_q);
#endif


#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)
static  void            NetDbg_MonTask                  (void          *ptask);
#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetDbg_Init()
*
* Description : (1) Initialize Network Debug Management Module :
*
*                   (a) Initialize debug information constants
*                   (b) Initialize debug status & monitor task handlers
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
* Note(s)     : (2) NetDbg_Init()  MUST follow :
*
*                   (a) NetTmr_Init() to successfully get a network timer during network initialization
*                       (see 'net.c  Net_Init()  Note #3c').
*
*                   (b) NetDbg_CfgMonTaskTime()  via Net_InitDflt()  to successfully configure the
*                       Debug Monitor Task time.
*
*               (3) Do NOT need to verify success of Timer Get since every status get will check & get
*                   a Debug Monitor Task Handler timer as needed.
*
*                   See also 'NetDbg_MonTask()  Note #4'.
*********************************************************************************************************
*/

void  NetDbg_Init (void)
{
#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR        cpu_sr;
#endif
    NET_TMR_TICK  timeout_tick;
    NET_ERR       err;
#endif

                                                                        /* ----------- INIT DBG INFO CONSTS ----------- */
    NetDbg_InitInfoConst();


#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)                            /* ----- INIT DBG STATUS TASK / HANDLERS ------ */
    CPU_CRITICAL_ENTER();
    NetDbg_MonTaskStatusRsrcLost = NET_DBG_STATUS_OK;
    NetDbg_MonTaskStatusRsrcLo   = NET_DBG_STATUS_OK;

    timeout_tick                 = NetDbg_MonTaskTime_tick;
    CPU_CRITICAL_EXIT();
                                                                        /* See Note #3.                                 */
    NetDbg_MonTaskTmr            = NetTmr_Get((void       *) NetDbg_MonTask,
                                              (CPU_FNCT_PTR) NetDbg_MonTask,
                                              (NET_TMR_TICK) timeout_tick,
                                              (CPU_INT16U  ) NET_TMR_FLAG_NONE,
                                              (NET_ERR    *)&err);
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetDbg_CfgRsrcBufSmallThLo()
*
* Description : (1) Configure SMALL network buffer monitor threshold & hysteresis :
*
*                   (a) Validate  desired threshold  value
*                   (b) Validate  desired hysteresis value
*                   (c) Configure desired threshold & hysteresis values
*
*
* Argument(s) : th          Desired number of SMALL network buffers concurrently in use to trip  low resources.
*
*               hyst        Desired number of SMALL network buffers freed               to clear low resources.
*
* Return(s)   : DEF_OK, SMALL network buffer monitor threshold & hysteresis configured.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : (2) Hysteresis maximum validation MUST occur BEFORE minimum validation since hysteresis
*                   maximum value MAY be zero (e.g. for extremely few configured objects).  This ensures
*                   that the monitor hysteresis value is greater than or equal to the hysteresis minimum
*                   value.
*
*               (3) 'NetDbg_RsrcBufSmallThLo' variables MUST ALWAYS be accessed exclusively in critical 
*                    sections.
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED) && \
     (NET_BUF_CFG_NBR_SMALL         >            0))
CPU_BOOLEAN  NetDbg_CfgRsrcBufSmallThLo (NET_DBG_RSRC_QTY  th,
                                         NET_DBG_RSRC_QTY  hyst)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR            cpu_sr;
#endif
    NET_DBG_RSRC_QTY  th_max;
    NET_DBG_RSRC_QTY  th_min;
    NET_DBG_RSRC_QTY  hyst_max;

                                                                        /* -------- VALIDATE SMALL BUF MON TH --------- */
    th_max = (NET_DBG_RSRC_TH_LO_MAX_NUMER * NET_BUF_CFG_NBR_SMALL) / NET_DBG_RSRC_TH_LO_MAX_DENOM;
    if (th > th_max) {
        th = th_max;
    }

    th_min = (NET_DBG_RSRC_TH_LO_MIN_NUMER * NET_BUF_CFG_NBR_SMALL) / NET_DBG_RSRC_TH_LO_MIN_DENOM;
    if (th < th_min) {
        th = th_min;
    }
    if (th < NET_DBG_RSRC_TH_LO_MIN) {
        th = NET_DBG_RSRC_TH_LO_MIN;
    }

                                                                        /* ------- VALIDATE SMALL BUF MON HYST -------- */
                                                                        /* See Note #2.                                 */ 
    hyst_max = (NET_DBG_RSRC_TH_LO_HYST_MAX_NUMER * NET_BUF_CFG_NBR_SMALL) / NET_DBG_RSRC_TH_LO_HYST_MAX_DENOM;
    if (hyst > hyst_max) {
        hyst = hyst_max;
    }

    if (hyst < NET_DBG_RSRC_TH_LO_HYST_MIN) {
        hyst = NET_DBG_RSRC_TH_LO_HYST_MIN;
    }


                                                                        /* ------- CFG SMALL BUF MON TH & HYST -------- */
    CPU_CRITICAL_ENTER();
    NetDbg_RsrcBufSmallThLo_nbr     = th;
    NetDbg_RsrcBufSmallThLoHyst_nbr = hyst;
    CPU_CRITICAL_EXIT();


    return (DEF_OK);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetDbg_CfgRsrcBufLargeThLo()
*
* Description : (1) Configure LARGE network buffer monitor threshold & hysteresis :
*
*                   (a) Validate  desired threshold  value
*                   (b) Validate  desired hysteresis value
*                   (c) Configure desired threshold & hysteresis values
*
*
* Argument(s) : th          Desired number of LARGE network buffers concurrently in use to trip  low resources.
*
*               hyst        Desired number of LARGE network buffers freed               to clear low resources.
*
* Return(s)   : DEF_OK, LARGE network buffer monitor threshold & hysteresis configured.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : (2) Hysteresis maximum validation MUST occur BEFORE minimum validation since hysteresis
*                   maximum value MAY be zero (e.g. for extremely few configured objects).  This ensures
*                   that the monitor hysteresis value is greater than or equal to the hysteresis minimum
*                   value.
*
*               (3) 'NetDbg_RsrcBufLargeThLo' variables MUST ALWAYS be accessed exclusively in critical 
*                    sections.
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED) && \
     (NET_BUF_CFG_NBR_LARGE         >            0))
CPU_BOOLEAN  NetDbg_CfgRsrcBufLargeThLo (NET_DBG_RSRC_QTY  th,
                                         NET_DBG_RSRC_QTY  hyst)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR            cpu_sr;
#endif
    NET_DBG_RSRC_QTY  th_max;
    NET_DBG_RSRC_QTY  th_min;
    NET_DBG_RSRC_QTY  hyst_max;

                                                                        /* -------- VALIDATE LARGE BUF MON TH --------- */
    th_max = (NET_DBG_RSRC_TH_LO_MAX_NUMER * NET_BUF_CFG_NBR_LARGE) / NET_DBG_RSRC_TH_LO_MAX_DENOM;
    if (th > th_max) {
        th = th_max;
    }

    th_min = (NET_DBG_RSRC_TH_LO_MIN_NUMER * NET_BUF_CFG_NBR_LARGE) / NET_DBG_RSRC_TH_LO_MIN_DENOM;
    if (th < th_min) {
        th = th_min;
    }
    if (th < NET_DBG_RSRC_TH_LO_MIN) {
        th = NET_DBG_RSRC_TH_LO_MIN;
    }

                                                                        /* ------- VALIDATE LARGE BUF MON HYST -------- */
                                                                        /* See Note #2.                                 */ 
    hyst_max = (NET_DBG_RSRC_TH_LO_HYST_MAX_NUMER * NET_BUF_CFG_NBR_LARGE) / NET_DBG_RSRC_TH_LO_HYST_MAX_DENOM;
    if (hyst > hyst_max) {
        hyst = hyst_max;
    }

    if (hyst < NET_DBG_RSRC_TH_LO_HYST_MIN) {
        hyst = NET_DBG_RSRC_TH_LO_HYST_MIN;
    }


                                                                        /* ------- CFG LARGE BUF MON TH & HYST -------- */
    CPU_CRITICAL_ENTER();
    NetDbg_RsrcBufLargeThLo_nbr     = th;
    NetDbg_RsrcBufLargeThLoHyst_nbr = hyst;
    CPU_CRITICAL_EXIT();


    return (DEF_OK);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetDbg_CfgRsrcTmrThLo()
*
* Description : (1) Configure network timer monitor threshold & hysteresis :
*
*                   (a) Validate  desired threshold  value
*                   (b) Validate  desired hysteresis value
*                   (c) Configure desired threshold & hysteresis values
*
*
* Argument(s) : th          Desired number of network timers concurrently in use to trip  low resources.
*
*               hyst        Desired number of network timers freed               to clear low resources.
*
* Return(s)   : DEF_OK, network timer monitor threshold & hysteresis configured.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : (2) Hysteresis maximum validation MUST occur BEFORE minimum validation since hysteresis
*                   maximum value MAY be zero (e.g. for extremely few configured objects).  This ensures
*                   that the monitor hysteresis value is greater than or equal to the hysteresis minimum
*                   value.
*
*               (3) 'NetDbg_RsrcTmrThLo' variables MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED)
CPU_BOOLEAN  NetDbg_CfgRsrcTmrThLo (NET_DBG_RSRC_QTY  th,
                                    NET_DBG_RSRC_QTY  hyst)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR            cpu_sr;
#endif
    NET_DBG_RSRC_QTY  th_max;
    NET_DBG_RSRC_QTY  th_min;
    NET_DBG_RSRC_QTY  hyst_max;

                                                                        /* ----------- VALIDATE TMR MON TH ------------ */
    th_max = (NET_DBG_RSRC_TH_LO_MAX_NUMER * NET_TMR_CFG_NBR_TMR) / NET_DBG_RSRC_TH_LO_MAX_DENOM;
    if (th > th_max) {
        th = th_max;
    }

    th_min = (NET_DBG_RSRC_TH_LO_MIN_NUMER * NET_TMR_CFG_NBR_TMR) / NET_DBG_RSRC_TH_LO_MIN_DENOM;
    if (th < th_min) {
        th = th_min;
    }
    if (th < NET_DBG_RSRC_TH_LO_MIN) {
        th = NET_DBG_RSRC_TH_LO_MIN;
    }

                                                                        /* ---------- VALIDATE TMR MON HYST ----------- */
                                                                        /* See Note #2.                                 */ 
    hyst_max = (NET_DBG_RSRC_TH_LO_HYST_MAX_NUMER * NET_TMR_CFG_NBR_TMR) / NET_DBG_RSRC_TH_LO_HYST_MAX_DENOM;
    if (hyst > hyst_max) {
        hyst = hyst_max;
    }

    if (hyst < NET_DBG_RSRC_TH_LO_HYST_MIN) {
        hyst = NET_DBG_RSRC_TH_LO_HYST_MIN;
    }


                                                                        /* ------------ CFG TMR TH & HYST ------------- */
    CPU_CRITICAL_ENTER();
    NetDbg_RsrcTmrThLo_nbr     = th;
    NetDbg_RsrcTmrThLoHyst_nbr = hyst;
    CPU_CRITICAL_EXIT();


    return (DEF_OK);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetDbg_CfgRsrcConnThLo()
*
* Description : (1) Configure network connection monitor threshold & hysteresis :
*
*                   (a) Validate  desired threshold  value
*                   (b) Validate  desired hysteresis value
*                   (c) Configure desired threshold & hysteresis values
*
*
* Argument(s) : th          Desired number of network connections concurrently in use to trip  low resources.
*
*               hyst        Desired number of network connections freed               to clear low resources.
*
* Return(s)   : DEF_OK, network connection monitor threshold & hysteresis configured.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : (2) Hysteresis maximum validation MUST occur BEFORE minimum validation since hysteresis
*                   maximum value MAY be zero (e.g. for extremely few configured objects).  This ensures
*                   that the monitor hysteresis value is greater than or equal to the hysteresis minimum
*                   value.
*
*               (3) 'NetDbg_RsrcConnThLo' variables MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED) && \
     (defined(NET_CONN_MODULE_PRESENT)))
CPU_BOOLEAN  NetDbg_CfgRsrcConnThLo (NET_DBG_RSRC_QTY  th,
                                     NET_DBG_RSRC_QTY  hyst)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR            cpu_sr;
#endif
    NET_DBG_RSRC_QTY  th_max;
    NET_DBG_RSRC_QTY  th_min;
    NET_DBG_RSRC_QTY  hyst_max;

                                                                        /* --------- VALIDATE NET CONN MON TH --------- */
    th_max = (NET_DBG_RSRC_TH_LO_MAX_NUMER * NET_CONN_CFG_NBR_CONN) / NET_DBG_RSRC_TH_LO_MAX_DENOM;
    if (th > th_max) {
        th = th_max;
    }

    th_min = (NET_DBG_RSRC_TH_LO_MIN_NUMER * NET_CONN_CFG_NBR_CONN) / NET_DBG_RSRC_TH_LO_MIN_DENOM;
    if (th < th_min) {
        th = th_min;
    }
    if (th < NET_DBG_RSRC_TH_LO_MIN) {
        th = NET_DBG_RSRC_TH_LO_MIN;
    }

                                                                        /* -------- VALIDATE NET CONN MON HYST -------- */
                                                                        /* See Note #2.                                 */ 
    hyst_max = (NET_DBG_RSRC_TH_LO_HYST_MAX_NUMER * NET_CONN_CFG_NBR_CONN) / NET_DBG_RSRC_TH_LO_HYST_MAX_DENOM;
    if (hyst > hyst_max) {
        hyst = hyst_max;
    }

    if (hyst < NET_DBG_RSRC_TH_LO_HYST_MIN) {
        hyst = NET_DBG_RSRC_TH_LO_HYST_MIN;
    }


                                                                        /* ---------- CFG NET CONN TH & HYST ---------- */
    CPU_CRITICAL_ENTER();
    NetDbg_RsrcConnThLo_nbr     = th;
    NetDbg_RsrcConnThLoHyst_nbr = hyst;
    CPU_CRITICAL_EXIT();


    return (DEF_OK);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetDbg_CfgRsrcARP_CacheThLo()
*
* Description : (1) Configure ARP cache monitor threshold & hysteresis :
*
*                   (a) Validate  desired threshold  value
*                   (b) Validate  desired hysteresis value
*                   (c) Configure desired threshold & hysteresis values
*
*
* Argument(s) : th          Desired number of ARP cache concurrently in use to trip  low resources.
*
*               hyst        Desired number of ARP cache freed               to clear low resources.
*
* Return(s)   : DEF_OK, ARP cache monitor threshold & hysteresis configured.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : (2) Hysteresis maximum validation MUST occur BEFORE minimum validation since hysteresis
*                   maximum value MAY be zero (e.g. for extremely few configured objects).  This ensures
*                   that the monitor hysteresis value is greater than or equal to the hysteresis minimum
*                   value.
*
*               (3) 'NetDbg_RsrcARP_CacheThLo' variables MUST ALWAYS be accessed exclusively in critical 
*                    sections.
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED) && \
     (defined(NET_ARP_MODULE_PRESENT)))
CPU_BOOLEAN  NetDbg_CfgRsrcARP_CacheThLo (NET_DBG_RSRC_QTY  th,
                                          NET_DBG_RSRC_QTY  hyst)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR            cpu_sr;
#endif
    NET_DBG_RSRC_QTY  th_max;
    NET_DBG_RSRC_QTY  th_min;
    NET_DBG_RSRC_QTY  hyst_max;

                                                                        /* -------- VALIDATE ARP CACHE MON TH --------- */
    th_max = (NET_DBG_RSRC_TH_LO_MAX_NUMER * NET_ARP_CFG_NBR_CACHE) / NET_DBG_RSRC_TH_LO_MAX_DENOM;
    if (th > th_max) {
        th = th_max;
    }

    th_min = (NET_DBG_RSRC_TH_LO_MIN_NUMER * NET_ARP_CFG_NBR_CACHE) / NET_DBG_RSRC_TH_LO_MIN_DENOM;
    if (th < th_min) {
        th = th_min;
    }
    if (th < NET_DBG_RSRC_TH_LO_MIN) {
        th = NET_DBG_RSRC_TH_LO_MIN;
    }

                                                                        /* ------- VALIDATE ARP CACHE MON HYST -------- */
                                                                        /* See Note #2.                                 */ 
    hyst_max = (NET_DBG_RSRC_TH_LO_HYST_MAX_NUMER * NET_ARP_CFG_NBR_CACHE) / NET_DBG_RSRC_TH_LO_HYST_MAX_DENOM;
    if (hyst > hyst_max) {
        hyst = hyst_max;
    }

    if (hyst < NET_DBG_RSRC_TH_LO_HYST_MIN) {
        hyst = NET_DBG_RSRC_TH_LO_HYST_MIN;
    }


                                                                        /* --------- CFG ARP CACHE TH & HYST ---------- */
    CPU_CRITICAL_ENTER();
    NetDbg_RsrcARP_CacheThLo_nbr     = th;
    NetDbg_RsrcARP_CacheThLoHyst_nbr = hyst;
    CPU_CRITICAL_EXIT();


    return (DEF_OK);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetDbg_CfgRsrcTCP_ConnThLo()
*
* Description : (1) Configure TCP connection monitor threshold & hysteresis :
*
*                   (a) Validate  desired threshold  value
*                   (b) Validate  desired hysteresis value
*                   (c) Configure desired threshold & hysteresis values
*
*
* Argument(s) : th          Desired number of TCP connections concurrently in use to trip  low resources.
*
*               hyst        Desired number of TCP connections freed               to clear low resources.
*
* Return(s)   : DEF_OK, TCP connection monitor threshold & hysteresis configured.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : (2) Hysteresis maximum validation MUST occur BEFORE minimum validation since hysteresis
*                   maximum value MAY be zero (e.g. for extremely few configured objects).  This ensures
*                   that the monitor hysteresis value is greater than or equal to the hysteresis minimum
*                   value.
*
*               (3) 'NetDbg_RsrcTCP_ConnThLo' variables MUST ALWAYS be accessed exclusively in critical 
*                    sections.
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED) && \
     (defined(NET_TCP_MODULE_PRESENT)))
CPU_BOOLEAN  NetDbg_CfgRsrcTCP_ConnThLo (NET_DBG_RSRC_QTY  th,
                                         NET_DBG_RSRC_QTY  hyst)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR            cpu_sr;
#endif
    NET_DBG_RSRC_QTY  th_max;
    NET_DBG_RSRC_QTY  th_min;
    NET_DBG_RSRC_QTY  hyst_max;

                                                                        /* --------- VALIDATE TCP CONN MON TH --------- */
    th_max = (NET_DBG_RSRC_TH_LO_MAX_NUMER * NET_TCP_CFG_NBR_CONN) / NET_DBG_RSRC_TH_LO_MAX_DENOM;
    if (th > th_max) {
        th = th_max;
    }

    th_min = (NET_DBG_RSRC_TH_LO_MIN_NUMER * NET_TCP_CFG_NBR_CONN) / NET_DBG_RSRC_TH_LO_MIN_DENOM;
    if (th < th_min) {
        th = th_min;
    }
    if (th < NET_DBG_RSRC_TH_LO_MIN) {
        th = NET_DBG_RSRC_TH_LO_MIN;
    }

                                                                        /* -------- VALIDATE TCP CONN MON HYST -------- */
                                                                        /* See Note #2.                                 */ 
    hyst_max = (NET_DBG_RSRC_TH_LO_HYST_MAX_NUMER * NET_TCP_CFG_NBR_CONN) / NET_DBG_RSRC_TH_LO_HYST_MAX_DENOM;
    if (hyst > hyst_max) {
        hyst = hyst_max;
    }

    if (hyst < NET_DBG_RSRC_TH_LO_HYST_MIN) {
        hyst = NET_DBG_RSRC_TH_LO_HYST_MIN;
    }


                                                                        /* ---------- CFG TCP CONN TH & HYST ---------- */
    CPU_CRITICAL_ENTER();
    NetDbg_RsrcTCP_ConnThLo_nbr     = th;
    NetDbg_RsrcTCP_ConnThLoHyst_nbr = hyst;
    CPU_CRITICAL_EXIT();


    return (DEF_OK);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetDbg_CfgRsrcSockThLo()
*
* Description : (1) Configure socket monitor threshold & hysteresis :
*
*                   (a) Validate  desired threshold  value
*                   (b) Validate  desired hysteresis value
*                   (c) Configure desired threshold & hysteresis values
*
*
* Argument(s) : th          Desired number of sockets concurrently in use to trip  low resources.
*
*               hyst        Desired number of sockets freed               to clear low resources.
*
* Return(s)   : DEF_OK, socket monitor threshold & hysteresis configured.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : (2) Hysteresis maximum validation MUST occur BEFORE minimum validation since hysteresis
*                   maximum value MAY be zero (e.g. for extremely few configured objects).  This ensures
*                   that the monitor hysteresis value is greater than or equal to the hysteresis minimum
*                   value.
*
*               (3) 'NetDbg_RsrcSockThLo' variables MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED) && \
     (defined(NET_SOCK_MODULE_PRESENT)))
CPU_BOOLEAN  NetDbg_CfgRsrcSockThLo (NET_DBG_RSRC_QTY  th,
                                     NET_DBG_RSRC_QTY  hyst)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR            cpu_sr;
#endif
    NET_DBG_RSRC_QTY  th_max;
    NET_DBG_RSRC_QTY  th_min;
    NET_DBG_RSRC_QTY  hyst_max;

                                                                        /* ----------- VALIDATE SOCK MON TH ----------- */
    th_max = (NET_DBG_RSRC_TH_LO_MAX_NUMER * NET_SOCK_CFG_NBR_SOCK) / NET_DBG_RSRC_TH_LO_MAX_DENOM;
    if (th > th_max) {
        th = th_max;
    }

    th_min = (NET_DBG_RSRC_TH_LO_MIN_NUMER * NET_SOCK_CFG_NBR_SOCK) / NET_DBG_RSRC_TH_LO_MIN_DENOM;
    if (th < th_min) {
        th = th_min;
    }
    if (th < NET_DBG_RSRC_TH_LO_MIN) {
        th = NET_DBG_RSRC_TH_LO_MIN;
    }

                                                                        /* ---------- VALIDATE SOCK MON HYST ---------- */
                                                                        /* See Note #2.                                 */ 
    hyst_max = (NET_DBG_RSRC_TH_LO_HYST_MAX_NUMER * NET_SOCK_CFG_NBR_SOCK) / NET_DBG_RSRC_TH_LO_HYST_MAX_DENOM;
    if (hyst > hyst_max) {
        hyst = hyst_max;
    }

    if (hyst < NET_DBG_RSRC_TH_LO_HYST_MIN) {
        hyst = NET_DBG_RSRC_TH_LO_HYST_MIN;
    }


                                                                        /* ------------ CFG SOCK TH & HYST ------------ */
    CPU_CRITICAL_ENTER();
    NetDbg_RsrcSockThLo_nbr     = th;
    NetDbg_RsrcSockThLoHyst_nbr = hyst;
    CPU_CRITICAL_EXIT();


    return (DEF_OK);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetDbg_CfgMonTaskTime()
*
* Description : (1) Configure Debug Monitor Task time (i.e. task scheduling period) :
*
*                   (a) Validate  desired monitor task time
*                   (b) Configure desired monitor task time
*
*
* Argument(s) : time_sec    Desired value for Debug Monitor Task time (in seconds).
*
* Return(s)   : DEF_OK,   Debug Monitor Task time configured.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Net_InitDflt(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : (2) Time in seconds converted to 'NET_TMR_TICK' ticks in order to pre-compute initial 
*                   timeout value in 'NET_TMR_TICK' ticks.
*
*               (3) 'NetDbg_MonTaskTime' variables MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)
CPU_BOOLEAN  NetDbg_CfgMonTaskTime (CPU_INT16U  time_sec)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR        cpu_sr;
#endif
    NET_TMR_TICK  timeout_tick;

                                                                        /* ---------- VALIDATE MON TASK TIME ---------- */
    if (time_sec < NET_DBG_MON_TASK_TIME_MIN_SEC) {
        return (DEF_FAIL);
    }
    if (time_sec > NET_DBG_MON_TASK_TIME_MAX_SEC) {
        return (DEF_FAIL);
    }

                                                                        /* ------------ CFG MON TASK TIME ------------- */
    timeout_tick            = (NET_TMR_TICK)time_sec * NET_TMR_TIME_TICK_PER_SEC;
    CPU_CRITICAL_ENTER();
    NetDbg_MonTaskTime_sec  =  time_sec;
    NetDbg_MonTaskTime_tick =  timeout_tick;
    CPU_CRITICAL_EXIT();

    return (DEF_OK);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetDbg_ChkStatus()
*
* Description : Check network protocol suite status.
*
*               See 'NetDbg_ChkStatusHandler()  Note #1'.
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if network status OK; i.e. no warnings, faults, or errors.
*
*               Otherwise,         return summary network status codes logically OR'd :
*
*                   NET_DBG_STATUS_FAULT_LOCK                   Network lock NOT acquired (see Note #1).
*
*                                                               -- RETURNED BY NetDbg_ChkStatusHandler() : --
*                   NET_DBG_STATUS_FAULT                        Some network status fault(s).
*
*                   NET_DBG_STATUS_FAULT_RSRC_LOST              Some network resource(s) lost.
*                   NET_DBG_STATUS_FAULT_RSRC_LO                Some network resource(s) low.
*
*                   NET_DBG_STATUS_FAULT_BUF                    Some network buffer     management fault(s).
*                   NET_DBG_STATUS_FAULT_TMR                    Some network timer      management fault(s).
*                   NET_DBG_STATUS_FAULT_CONN                   Some network connection management fault(s).
*
*                   NET_DBG_STATUS_FAULT_TCP                    Some TCP layer                     fault(s).
*
*               See also 'NetDbg_ChkStatusHandler()  Note #3'.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetDbg_ChkStatus() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetDbg_ChkStatusHandler()  Note #2'.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_STATUS_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_ChkStatus (void)
{
    NET_DBG_STATUS  status;
    NET_ERR         err;


    NetOS_Lock(&err);                                                   /* Acquire net lock (see Note #1b).             */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_DBG_STATUS_FAULT_LOCK);
    }

    status = NetDbg_ChkStatusHandler();                                 /* Chk net status.                              */
                                                                        
    NetOS_Unlock();                                                     /* Release net lock.                            */

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetDbg_ChkStatusBufs()
*
* Description : Check network buffer management status.
*
*               See 'NetDbg_ChkStatusHandlerBufs()  Note #1'.
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO network buffer faults.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_LOCK                             Network lock NOT acquired (see Note #1).
*
*                                                               -- RETURNED BY NetDbg_ChkStatusHandlerBufs() : ---
*                   NET_DBG_SF_BUF                              Some network buffer management fault(s).
*
*                   NET_DBG_SF_BUF_SMALL_TYPE                   Small buffer invalid type.
*                   NET_DBG_SF_BUF_SMALL_ID                     Small buffer invalid ID.
*                   NET_DBG_SF_BUF_SMALL_LINK_TYPE              Small buffer invalid link type.
*                   NET_DBG_SF_BUF_SMALL_LINK_UNUSED            Small buffer         link unused.
*                   NET_DBG_SF_BUF_SMALL_LINK_BACK_TO_BUF       Small buffer invalid link     back to same buffer.
*                   NET_DBG_SF_BUF_SMALL_LINK_NOT_TO_BUF        Small buffer invalid link NOT back to same buffer.
*                   NET_DBG_SF_BUF_SMALL_LINK_TO_BUF            Small buffer invalid link     back to      buffer.
*                   NET_DBG_SF_BUF_SMALL_POOL_TYPE              Small buffer invalid pool buffer type.
*                   NET_DBG_SF_BUF_SMALL_POOL_ID                Small buffer invalid pool buffer ID.
*                   NET_DBG_SF_BUF_SMALL_POOL_DUP               Small buffer pool contains duplicate buffer(s).
*                   NET_DBG_SF_BUF_SMALL_POOL_NBR_MAX           Small buffer pool number of buffers greater than
*                                                                   maximum number of small buffers.
*                   NET_DBG_SF_BUF_SMALL_USED_IN_POOL           Small buffer   used but     in pool.
*                   NET_DBG_SF_BUF_SMALL_UNUSED_NOT_IN_POOL     Small buffer unused but NOT in pool.
*
*                   NET_DBG_SF_BUF_LARGE_TYPE                   Large buffer invalid type.
*                   NET_DBG_SF_BUF_LARGE_ID                     Large buffer invalid ID.
*                   NET_DBG_SF_BUF_LARGE_LINK_TYPE              Large buffer invalid link type.
*                   NET_DBG_SF_BUF_LARGE_LINK_UNUSED            Large buffer         link unused.
*                   NET_DBG_SF_BUF_LARGE_LINK_BACK_TO_BUF       Large buffer invalid link     back to same buffer.
*                   NET_DBG_SF_BUF_LARGE_LINK_NOT_TO_BUF        Large buffer invalid link NOT back to same buffer.
*                   NET_DBG_SF_BUF_LARGE_LINK_TO_BUF            Large buffer invalid link     back to      buffer.
*                   NET_DBG_SF_BUF_LARGE_POOL_TYPE              Large buffer invalid pool buffer type.
*                   NET_DBG_SF_BUF_LARGE_POOL_ID                Large buffer invalid pool buffer ID.
*                   NET_DBG_SF_BUF_LARGE_POOL_DUP               Large buffer pool contains duplicate buffer(s).
*                   NET_DBG_SF_BUF_LARGE_POOL_NBR_MAX           Large buffer pool number of buffers greater than
*                                                                   maximum number of large buffers.
*                   NET_DBG_SF_BUF_LARGE_USED_IN_POOL           Large buffer   used but     in pool.
*                   NET_DBG_SF_BUF_LARGE_UNUSED_NOT_IN_POOL     Large buffer unused but NOT in pool.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetDbg_ChkStatusBufs() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetDbg_ChkStatusHandlerBufs()  Note #2'.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_STATUS_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_ChkStatusBufs (void)
{
    NET_DBG_STATUS  status;
    NET_ERR         err;


    NetOS_Lock(&err);                                                   /* Acquire net lock (see Note #1b).             */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_DBG_SF_LOCK);
    }

    status = NetDbg_ChkStatusHandlerBufs();                             /* Chk net buf status.                          */
                                                                        
    NetOS_Unlock();                                                     /* Release net lock.                            */

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetDbg_ChkStatusTmrs()
*
* Description : Check network timer management status.
*
*               See 'NetDbg_ChkStatusHandlerTmrs()  Note #1'.
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO network timer faults.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_LOCK                             Network lock NOT acquired (see Note #1).
*
*                                                               --- RETURNED BY NetDbg_ChkStatusHandlerTmrs() : ----
*                   NET_DBG_SF_TMR                              Some network timer management fault(s).
*
*                   NET_DBG_SF_TMR_TYPE                         Network timer invalid type.
*                   NET_DBG_SF_TMR_ID                           Network timer invalid ID.
*                   NET_DBG_SF_TMR_LINK_TYPE                    Network timer invalid link type.
*                   NET_DBG_SF_TMR_LINK_UNUSED                  Network timer         link unused.
*                   NET_DBG_SF_TMR_LINK_BACK_TO_TMR             Network timer invalid link back to same timer.
*                   NET_DBG_SF_TMR_LINK_TO_TMR                  Network timer invalid link back to      timer.
*
*                   NET_DBG_SF_TMR_POOL_TYPE                    Network timer invalid pool type.
*                   NET_DBG_SF_TMR_POOL_ID                      Network timer invalid pool ID.
*                   NET_DBG_SF_TMR_POOL_DUP                     Network timer pool contains duplicate timer(s).
*                   NET_DBG_SF_TMR_POOL_NBR_MAX                 Network timer pool number of timers greater
*                                                                   than maximum   number of timers.
*
*                   NET_DBG_SF_TMR_LIST_TYPE                    Network timer task list invalid type.
*                   NET_DBG_SF_TMR_LIST_ID                      Network timer task list invalid ID.
*                   NET_DBG_SF_TMR_LIST_DUP                     Network timer task list contains duplicate timer(s).
*                   NET_DBG_SF_TMR_LIST_NBR_MAX                 Network timer task list number of timers greater
*                                                                   than maximum number of timers.
*                   NET_DBG_SF_TMR_LIST_NBR_USED                Network timer task list number of timers NOT equal
*                                                                   to number of used timers.
*
*                   NET_DBG_SF_TMR_USED_IN_POOL                 Network timer   used but     in pool.
*                   NET_DBG_SF_TMR_UNUSED_NOT_IN_POOL           Network timer unused but NOT in pool.
*                   NET_DBG_SF_TMR_UNUSED_IN_LIST               Network timer unused but     in timer task list.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetDbg_ChkStatusTmrs() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetDbg_ChkStatusHandlerTmrs()  Note #2'.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_STATUS_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_ChkStatusTmrs (void)
{
    NET_DBG_STATUS  status;
    NET_ERR         err;


    NetOS_Lock(&err);                                                   /* Acquire net lock (see Note #1b).             */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_DBG_SF_LOCK);
    }

    status = NetDbg_ChkStatusHandlerTmrs();                             /* Chk net tmr status.                          */
                                                                        
    NetOS_Unlock();                                                     /* Release net lock.                            */

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetDbg_ChkStatusConns()
*
* Description : Check network connection management status.
*
*               See 'NetDbg_ChkStatusHandlerConns()  Note #1'.
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO network connection faults.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_LOCK                             Network lock NOT acquired (see Note #1).
*
*                                                               - RETURNED BY NetDbg_ChkStatusHandlerConns() : -
*                   NET_DBG_SF_CONN                             Some network connection management fault(s).
*
*                   NET_DBG_SF_CONN_TYPE                        Network connection invalid type.
*                   NET_DBG_SF_CONN_FAMILY                      Network connection invalid family.
*                   NET_DBG_SF_CONN_PROTOCOL_IX_NBR_MAX         Network connection invalid list index number.
*                   NET_DBG_SF_CONN_ID                          Network connection invalid ID.
*                   NET_DBG_SF_CONN_ID_NONE                     Network connection with NO connection IDs.
*                   NET_DBG_SF_CONN_ID_UNUSED                   Network connection linked to unused connection.
*                   NET_DBG_SF_CONN_LINK_TYPE                   Network connection invalid link type.
*                   NET_DBG_SF_CONN_LINK_UNUSED                 Network connection         link unused.
*                   NET_DBG_SF_CONN_LINK_BACK_TO_CONN           Network connection invalid link     back to
*                                                                   same connection.
*                   NET_DBG_SF_CONN_LINK_NOT_TO_CONN            Network connection invalid link NOT back to
*                                                                   same connection.
*                   NET_DBG_SF_CONN_LINK_NOT_IN_LIST            Network connection NOT in appropriate
*                                                                   connection list.
*
*                   NET_DBG_SF_CONN_POOL_TYPE                   Network connection invalid pool type.
*                   NET_DBG_SF_CONN_POOL_ID                     Network connection invalid pool ID.
*                   NET_DBG_SF_CONN_POOL_DUP                    Network connection pool contains duplicate
*                                                                   connection(s).
*                   NET_DBG_SF_CONN_POOL_NBR_MAX                Network connection pool  number of connections
*                                                                   greater than maximum number of connections.
*
*                   NET_DBG_SF_CONN_LIST_NBR_NOT_SOLITARY       Network connection lists number of connections
*                                                                   NOT equal to solitary connection.
*
*                   NET_DBG_SF_CONN_USED_IN_POOL                Network connection   used but     in pool.
*                   NET_DBG_SF_CONN_USED_NOT_IN_LIST            Network connection   used but NOT in list.
*                   NET_DBG_SF_CONN_UNUSED_IN_LIST              Network connection unused but     in list.
*                   NET_DBG_SF_CONN_UNUSED_NOT_IN_POOL          Network connection unused but NOT in pool.
*                   NET_DBG_SF_CONN_IN_LIST_IN_POOL             Network connection     in list &     in pool.
*                   NET_DBG_SF_CONN_NOT_IN_LIST_NOT_IN_POOL     Network connection NOT in list & NOT in pool.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetDbg_ChkStatusConns() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetDbg_ChkStatusHandlerConns()  Note #2'.
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_EN == DEF_ENABLED) && \
     (defined(NET_CONN_MODULE_PRESENT)))
NET_DBG_STATUS  NetDbg_ChkStatusConns (void)
{
    NET_DBG_STATUS  status;
    NET_ERR         err;


    NetOS_Lock(&err);                                                   /* Acquire net lock (see Note #1b).             */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_DBG_SF_LOCK);
    }

    status = NetDbg_ChkStatusHandlerConns();                            /* Chk net conn status.                         */
                                                                        
    NetOS_Unlock();                                                     /* Release net lock.                            */

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetDbg_ChkStatusTCP()
*
* Description : Check TCP layer status.
*
*               See 'NetDbg_ChkStatusHandlerTCP()  Note #1'.
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO TCP layer faults.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_LOCK                             Network lock NOT acquired (see Note #1).
*
*                                                               ---- RETURNED BY NetDbg_ChkStatusHandlerTCP() : -----
*                   NET_DBG_SF_TCP                              Some TCP layer fault(s).
*
*                   NET_DBG_SF_TCP_CONN_TYPE                    TCP connection invalid type.
*                   NET_DBG_SF_TCP_CONN_ID                      TCP connection invalid ID.
*                   NET_DBG_SF_TCP_CONN_LINK_TYPE               TCP connection invalid link type.
*                   NET_DBG_SF_TCP_CONN_LINK_UNUSED             TCP connection         link unused.
*
*                   NET_DBG_SF_TCP_CONN_POOL_TYPE               TCP connection invalid pool type.
*                   NET_DBG_SF_TCP_CONN_POOL_ID                 TCP connection invalid pool ID.
*                   NET_DBG_SF_TCP_CONN_POOL_DUP                TCP connection pool contains duplicate connection(s).
*                   NET_DBG_SF_TCP_CONN_POOL_NBR_MAX            TCP connection pool number of connections greater
*                                                                   than maximum    number of connections.
*
*                   NET_DBG_SF_TCP_CONN_USED_IN_POOL            TCP connection   used but     in pool.
*                   NET_DBG_SF_TCP_CONN_UNUSED_NOT_IN_POOL      TCP connection unused but NOT in pool.
*
*                   NET_DBG_SF_TCP_CONN_Q                       Some TCP connection queue fault(s).
*                   NET_DBG_SF_TCP_CONN_Q_BUF_TYPE              TCP connection queue buffer invalid type.     
*                   NET_DBG_SF_TCP_CONN_Q_BUF_UNUSED            TCP connection queue buffer         unused.     
*                   NET_DBG_SF_TCP_CONN_Q_LINK_TYPE             TCP connection queue buffer invalid link type.
*                   NET_DBG_SF_TCP_CONN_Q_LINK_UNUSED           TCP connection queue buffer         link unused.
*                   NET_DBG_SF_TCP_CONN_Q_BUF_DUP               TCP connection queue contains duplicate buffer(s).
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetDbg_ChkStatusTCP() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetDbg_ChkStatusHandlerTCP()  Note #2'.
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_EN == DEF_ENABLED) && \
     (defined(NET_TCP_MODULE_PRESENT)))
NET_DBG_STATUS  NetDbg_ChkStatusTCP (void)
{
    NET_DBG_STATUS  status;
    NET_ERR         err;


    NetOS_Lock(&err);                                                   /* Acquire net lock (see Note #1b).             */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_DBG_SF_LOCK);
    }

    status = NetDbg_ChkStatusHandlerTCP();                              /* Chk TCP layer status.                        */
                                                                        
    NetOS_Unlock();                                                     /* Release net lock.                            */

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetDbg_ChkStatusRsrcLost()
*
* Description : Check network protocol suite lost resources status.
*
*               See 'NetDbg_ChkStatusHandlerRsrcLost()  Note #1'.
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO network resources lost.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_LOCK                             Network lock NOT acquired (see Note #1).
*
*                                                               - RETURNED BY NetDbg_ChkStatusHandlerRsrcLost() : -
*                   NET_DBG_SF_RSRC_LOST                        Some network                resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_BUF_SMALL              Some network SMALL buffer   resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_BUF_LARGE              Some network LARGE buffer   resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_TMR                    Some network timer          resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_CONN                   Some network     connection resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_ARP_CACHE              Some network ARP cache      resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_TCP_CONN               Some network TCP connection resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_SOCK                   Some network socket         resource(s) lost.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetDbg_ChkStatusRsrcLost() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetDbg_ChkStatusHandlerRsrcLost()  Note #2'.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_STATUS_RSRC_LOST_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_ChkStatusRsrcLost (void)
{
    NET_DBG_STATUS  status;
    NET_ERR         err;


    NetOS_Lock(&err);                                                   /* Acquire net lock (see Note #1b).             */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_DBG_SF_LOCK);
    }

    status = NetDbg_ChkStatusHandlerRsrcLost();                         /* Chk net rsrc lost status.                    */
                                                                        
    NetOS_Unlock();                                                     /* Release net lock.                            */

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetDbg_ChkStatusRsrcLo()
*
* Description : Check network protocol suite low resources status.
*
*               See 'NetDbg_ChkStatusHandlerRsrcLo()  Note #1'.
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO network resources low.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_LOCK                             Network lock NOT acquired (see Note #1).
*
*                                                               - RETURNED BY NetDbg_ChkStatusHandlerRsrcLo() : -
*                   NET_DBG_SF_RSRC_LO                          Some network                resource(s) low.
*                   NET_DBG_SF_RSRC_LO_BUF_SMALL                     Network SMALL buffer   resources   low.
*                   NET_DBG_SF_RSRC_LO_BUF_LARGE                     Network LARGE buffer   resources   low.
*                   NET_DBG_SF_RSRC_LO_TMR                           Network timer          resources   low.
*                   NET_DBG_SF_RSRC_LO_CONN                          Network     connection resources   low.
*                   NET_DBG_SF_RSRC_LO_ARP_CACHE                     Network ARP cache      resources   low.
*                   NET_DBG_SF_RSRC_LO_TCP_CONN                      Network TCP connection resources   low.
*                   NET_DBG_SF_RSRC_LO_SOCK                          Network socket         resources   low.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetDbg_ChkStatusRsrcLo() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetDbg_ChkStatusHandlerRsrcLo()  Note #2'.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_ChkStatusRsrcLo (void)
{
    NET_DBG_STATUS  status;
    NET_ERR         err;


    NetOS_Lock(&err);                                                   /* Acquire net lock (see Note #1b).             */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_DBG_SF_LOCK);
    }

    status = NetDbg_ChkStatusHandlerRsrcLo();                           /* Chk net rsrc lo status.                      */
                                                                        
    NetOS_Unlock();                                                     /* Release net lock.                            */

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetDbg_MonTaskStatusGetRsrcLost()
*
* Description : Get current Debug Monitor Task lost resource status.
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_SF_LOCK, if network lock NOT acquired (see Note #1).
*
*               Otherwise,       return current Debug Monitor Task lost resource status.
*
*                                See also 'NetDbg_ChkStatusHandlerRsrcLost()  Return(s)'.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetDbg_MonTaskStatusGetRsrcLost() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetDbg_MonTaskStatGetHandlerLost()  Note #2'.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_MonTaskStatusGetRsrcLost (void)
{
    NET_DBG_STATUS  status;
    NET_ERR         err;


    NetOS_Lock(&err);                                                   /* Acquire net lock (see Note #1b).             */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_DBG_SF_LOCK);
    }

    status = NetDbg_MonTaskStatGetHandlerLost();                        /* Get mon task rsrc lost status.               */
                                                                        
    NetOS_Unlock();                                                     /* Release net lock.                            */

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetDbg_MonTaskStatusGetRsrcLo()
*
* Description : Get current Debug Monitor Task low resource status.
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_SF_LOCK, if network lock NOT acquired (see Note #1).
*
*               Otherwise,       return current Debug Monitor Task low resource status.
*
*                                See also 'NetDbg_ChkStatusHandlerRsrcLo()  Return(s)'.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetDbg_MonTaskStatusGetRsrcLo() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetDbg_MonTaskStatGetHandlerLo()  Note #2'.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_MonTaskStatusGetRsrcLo (void)
{
    NET_DBG_STATUS  status;
    NET_ERR         err;


    NetOS_Lock(&err);                                                   /* Acquire net lock (see Note #1b).             */
    if (err !=  NET_OS_ERR_NONE) {
        return (NET_DBG_SF_LOCK);
    }

    status = NetDbg_MonTaskStatGetHandlerLo();                          /* Get mon task rsrc lo status.                 */
                                                                        
    NetOS_Unlock();                                                     /* Release net lock.                            */

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetDbg_GetInfoConn()
*
* Description : Get information on a network connection.
*
*               See 'NetDbg_GetInfoHandlerConn()  Note #1'.
*
* Argument(s) : conn_id             Handle identifier of network connection.
*
*               pconn_family        Pointer to variable that will receive the network connection family type.
*
*                                       Connection family type,     if network connection valid & used.
*                                       NET_CONN_FAMILY_NONE,       otherwise.
*
*               pconn_protocol_ix   Pointer to variable that will receive the network connection protocol index :
*
*                                       Connection chain index,     if network connection valid & used.
*                                       NET_CONN_PROTOCOL_IX_NONE,  otherwise.
*
*               pconn_ix_chain      Pointer to variable that will receive the network connection chain index 
*                                       in its connection list (see 'NetDbg_GetInfoHandlerConn()  Note #2a') :
*
*                                       Connection chain index,     if network connection found in its 
*                                                                      network connection list.
*                                       NET_CONN_LIST_IX_NONE,      otherwise.
*
*               pconn_ix            Pointer to variable that will receive the network connection       index 
*                                       in its connection list (see 'NetDbg_GetInfoHandlerConn()  Note #2b') :
*
*                                       Connection       index,     if network connection found in its 
*                                                                      network connection list.
*                                       NET_CONN_LIST_IX_NONE,      otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               - RETURNED BY NetDbg_GetInfoHandlerConn() : -
*                               NET_CONN_ERR_NONE               Network connection information successfully 
*                                                                   obtained; check return value(s).
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
*                                                               -------- RETURNED BY NetOS_Lock() : ---------
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s) [see also Note #1].
*
* Note(s)     : (1) NetDbg_GetInfoConn() is called by application function(s) & ... :
*
*                   (a) MUST NOT be called with the global network lock already acquired; ...
*                   (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
*                       network lock (see 'net.h  Note #2').
*
*                   This is required since an application's network protocol suite API function access is 
*                   asynchronous to other network protocol tasks.
*
*                   See also 'NetDbg_GetInfoHandlerConn()  Note #3'.
*********************************************************************************************************
*/
/*$PAGE*/
#if ((NET_DBG_CFG_STATUS_EN == DEF_ENABLED) && \
     (defined(NET_CONN_MODULE_PRESENT)))
void  NetDbg_GetInfoConn (NET_CONN_ID            conn_id,
                          NET_CONN_FAMILY       *pconn_family,
                          NET_CONN_PROTOCOL_IX  *pconn_protocol_ix,
                          NET_CONN_LIST_IX      *pconn_ix_chain,
                          NET_CONN_LIST_IX      *pconn_ix,
                          NET_ERR               *perr)
{
    NetOS_Lock(perr);                                                   /* Acquire net lock (see Note #1b).             */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }
                                                                        /* Get net conn info status.                    */
    NetDbg_GetInfoHandlerConn(conn_id,
                              pconn_family,
                              pconn_protocol_ix,
                              pconn_ix_chain,
                              pconn_ix,
                              perr);

    NetOS_Unlock();                                                     /* Release net lock.                            */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetDbg_ChkStatusHandler()
*
* Description : (1) Check network protocol suite status :
*
*                   (a) Lost network resource(s)
*                   (b) Low  network resource(s)
*                   (c) Network buffer(s)
*                   (d) Network timer(s)
*                   (e) Network connection(s)
*                   (f) TCP layer
*
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if network status OK; i.e. no warnings, faults, or errors.
*
*               Otherwise,         return summary network status codes logically OR'd (see Note #3) :
*
*                   NET_DBG_STATUS_FAULT                        Some network status fault(s).
*
*                   NET_DBG_STATUS_FAULT_RSRC_LOST              Some network resource(s) lost.
*                   NET_DBG_STATUS_FAULT_RSRC_LO                Some network resource(s) low.
*
*                   NET_DBG_STATUS_FAULT_BUF                    Some network buffer     management fault(s).
*                   NET_DBG_STATUS_FAULT_TMR                    Some network timer      management fault(s).
*                   NET_DBG_STATUS_FAULT_CONN                   Some network connection management fault(s).
*
*                   NET_DBG_STATUS_FAULT_TCP                    Some TCP layer fault(s).
*
* Caller(s)   : NetDbg_ChkStatus().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (2) NetDbg_ChkStatusHandler() is called by network protocol suite function(s) & MUST be
*                   called with the global network lock already acquired.
*
*                   See also 'NetDbg_ChkStatus()  Note #1'.
*
*               (3) If any module returns a network status fault, check the module's network status return 
*                   value for more specific network status fault(s).
*
*                   See also 'net_dbg.h  NETWORK DEBUG STATUS DEFINES  Note #2'.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_DBG_CFG_STATUS_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_ChkStatusHandler (void)
{
#if (NET_DBG_CFG_STATUS_RSRC_LOST_EN == DEF_ENABLED)
    NET_DBG_STATUS  status_rsrc_lost;
#endif
#if (NET_DBG_CFG_STATUS_RSRC_LO_EN   == DEF_ENABLED)
    NET_DBG_STATUS  status_rsrc_lo;
#endif
#ifdef  NET_CONN_MODULE_PRESENT
    NET_DBG_STATUS  status_conn;
#endif
#ifdef  NET_TCP_MODULE_PRESENT
    NET_DBG_STATUS  status_tcp;
#endif
    NET_DBG_STATUS  status;
    NET_DBG_STATUS  status_buf;
    NET_DBG_STATUS  status_tmr;


    status = NET_DBG_STATUS_OK;



#if (NET_DBG_CFG_STATUS_RSRC_LOST_EN == DEF_ENABLED)                    /* ------------- CHK LOST RSRC's -------------- */
    status_rsrc_lost = NetDbg_ChkStatusHandlerRsrcLost();
    if (status_rsrc_lost != NET_DBG_STATUS_OK) {
        DEF_BIT_SET(status, NET_DBG_STATUS_FAULT_RSRC_LOST);
    }
#endif

#if (NET_DBG_CFG_STATUS_RSRC_LO_EN   == DEF_ENABLED)                    /* ------------- CHK LO   RSRC's -------------- */
    status_rsrc_lo   = NetDbg_ChkStatusHandlerRsrcLo();
    if (status_rsrc_lo   != NET_DBG_STATUS_OK) {
        DEF_BIT_SET(status, NET_DBG_STATUS_FAULT_RSRC_LO);
    }
#endif


                                                                        /* -------------- CHK NET BUF's --------------- */
    status_buf = NetDbg_ChkStatusHandlerBufs();
    if (status_buf != NET_DBG_STATUS_OK) {
        DEF_BIT_SET(status, NET_DBG_STATUS_FAULT_BUF);
    }

                                                                        /* -------------- CHK NET TMR's --------------- */
    status_tmr = NetDbg_ChkStatusHandlerTmrs();
    if (status_tmr != NET_DBG_STATUS_OK) {
        DEF_BIT_SET(status, NET_DBG_STATUS_FAULT_TMR);
    }


#ifdef  NET_CONN_MODULE_PRESENT                                         /* -------------- CHK NET CONN's -------------- */
    status_conn = NetDbg_ChkStatusHandlerConns();
    if (status_conn != NET_DBG_STATUS_OK) {
        DEF_BIT_SET(status, NET_DBG_STATUS_FAULT_CONN);
    }
#endif

#ifdef  NET_TCP_MODULE_PRESENT                                          /* -------------- CHK TCP LAYER --------------- */
    status_tcp = NetDbg_ChkStatusHandlerTCP();
    if (status_tcp != NET_DBG_STATUS_OK) {
        DEF_BIT_SET(status, NET_DBG_STATUS_FAULT_TCP);
    }
#endif



    if (status != NET_DBG_STATUS_OK) {                                  /* For ANY fault status, ...                    */
        DEF_BIT_SET(status, NET_DBG_STATUS_FAULT);                      /* ... set breakpoint.                          */
    }


    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetDbg_ChkStatusHandlerBufs()
*
* Description : (1) Check network buffer management status :
*
*                   (a) Corrupted buffer pools
*                   (b) Corrupted buffers
*
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO network buffer faults.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_BUF                              Some network buffer management fault(s).
*
*                   NET_DBG_SF_BUF_SMALL_TYPE                   Small buffer invalid type.
*                   NET_DBG_SF_BUF_SMALL_ID                     Small buffer invalid ID.
*                   NET_DBG_SF_BUF_SMALL_LINK_TYPE              Small buffer invalid link type.
*                   NET_DBG_SF_BUF_SMALL_LINK_UNUSED            Small buffer         link unused.
*                   NET_DBG_SF_BUF_SMALL_LINK_BACK_TO_BUF       Small buffer invalid link     back to same buffer.
*                   NET_DBG_SF_BUF_SMALL_LINK_NOT_TO_BUF        Small buffer invalid link NOT back to same buffer.
*                   NET_DBG_SF_BUF_SMALL_LINK_TO_BUF            Small buffer invalid link     back to      buffer.
*                   NET_DBG_SF_BUF_SMALL_POOL_TYPE              Small buffer invalid pool buffer type.
*                   NET_DBG_SF_BUF_SMALL_POOL_ID                Small buffer invalid pool buffer ID.
*                   NET_DBG_SF_BUF_SMALL_POOL_DUP               Small buffer pool contains duplicate buffer(s).
*                   NET_DBG_SF_BUF_SMALL_POOL_NBR_MAX           Small buffer pool number of buffers greater than
*                                                                   maximum number of small buffers.
*                   NET_DBG_SF_BUF_SMALL_USED_IN_POOL           Small buffer   used but     in pool.
*                   NET_DBG_SF_BUF_SMALL_UNUSED_NOT_IN_POOL     Small buffer unused but NOT in pool.
*
*                   NET_DBG_SF_BUF_LARGE_TYPE                   Large buffer invalid type.
*                   NET_DBG_SF_BUF_LARGE_ID                     Large buffer invalid ID.
*                   NET_DBG_SF_BUF_LARGE_LINK_TYPE              Large buffer invalid link type.
*                   NET_DBG_SF_BUF_LARGE_LINK_UNUSED            Large buffer         link unused.
*                   NET_DBG_SF_BUF_LARGE_LINK_BACK_TO_BUF       Large buffer invalid link     back to same buffer.
*                   NET_DBG_SF_BUF_LARGE_LINK_NOT_TO_BUF        Large buffer invalid link NOT back to same buffer.
*                   NET_DBG_SF_BUF_LARGE_LINK_TO_BUF            Large buffer invalid link     back to      buffer.
*                   NET_DBG_SF_BUF_LARGE_POOL_TYPE              Large buffer invalid pool buffer type.
*                   NET_DBG_SF_BUF_LARGE_POOL_ID                Large buffer invalid pool buffer ID.
*                   NET_DBG_SF_BUF_LARGE_POOL_DUP               Large buffer pool contains duplicate buffer(s).
*                   NET_DBG_SF_BUF_LARGE_POOL_NBR_MAX           Large buffer pool number of buffers greater than
*                                                                   maximum number of large buffers.
*                   NET_DBG_SF_BUF_LARGE_USED_IN_POOL           Large buffer   used but     in pool.
*                   NET_DBG_SF_BUF_LARGE_UNUSED_NOT_IN_POOL     Large buffer unused but NOT in pool.
*
* Caller(s)   : NetDbg_ChkStatusBufs(),
*               NetDbg_ChkStatusHandler().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (2) NetDbg_ChkStatusHandlerBufs() is called by network protocol suite function(s) & MUST
*                   be called with the global network lock already acquired.
*
*                   See also 'NetDbg_ChkStatusBufs()  Note #1'.
*
*               (3) #### Link pointers NOT yet validated to be aligned on CPU pointer-aligned addresses
*                   which MAY lead to unaligned pointer access & generate CPU pointer-alignment exceptions.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_DBG_CFG_STATUS_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_ChkStatusHandlerBufs (void)
{
#if (NET_BUF_CFG_NBR_SMALL > 0)
    NET_BUF_QTY      buf_in_buf_pool_small[NET_BUF_CFG_NBR_SMALL];
    NET_BUF_SMALL   *pbuf_chk_small;
#endif
#if (NET_BUF_CFG_NBR_LARGE > 0)
    NET_BUF_QTY      buf_in_buf_pool_large[NET_BUF_CFG_NBR_LARGE];
    NET_BUF_LARGE   *pbuf_chk_large;
#endif
#if ((NET_BUF_CFG_NBR_SMALL > 0) || \
     (NET_BUF_CFG_NBR_LARGE > 0))
    CPU_BOOLEAN      err_pool;
    CPU_BOOLEAN      err_buf;
    CPU_BOOLEAN      used;
    NET_BUF_QTY      i;
    NET_BUF_QTY      id;
    NET_BUF         *pbuf_link;
#endif
    NET_DBG_STATUS   status;


    status = NET_DBG_STATUS_OK;


#if (NET_BUF_CFG_NBR_SMALL > 0)
                                                                        /* ------------ CHK SMALL BUF POOL ------------ */
    for (i = 0; i < NET_BUF_CFG_NBR_SMALL; i++) {
        buf_in_buf_pool_small[i] = DEF_NO;
    }

    pbuf_chk_small = NetBuf_SmallPoolPtr;
    i              = 0;
    err_pool       = DEF_NO;
    while ((pbuf_chk_small != (NET_BUF_SMALL *)0) &&                    /* Chk SMALL buf pool for used entries.         */
           (i < NET_BUF_CFG_NBR_SMALL)) {

        if (pbuf_chk_small->Hdr.Type != NET_BUF_TYPE_SMALL) {           /* If  SMALL buf type invalid,    set err.      */
            DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_POOL_TYPE);
            err_pool = DEF_YES;

        } else {
            id = pbuf_chk_small->Hdr.ID;
            if (id >= NET_BUF_CFG_NBR_SMALL) {                          /* If  SMALL buf ID   invalid,    set err.      */
                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_POOL_ID);
                err_pool = DEF_YES;

            } else {
                if (buf_in_buf_pool_small[id] == DEF_YES) {             /* If  SMALL buf already in pool, set err.      */
                    DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_POOL_DUP);
                    err_pool = DEF_YES;
                }
                buf_in_buf_pool_small[id] = DEF_YES;
            
                used = DEF_BIT_IS_SET(pbuf_chk_small->Hdr.Flags, NET_BUF_FLAG_USED);
                if (used != DEF_NO) {                                   /* If  SMALL buf used    in pool, set err.      */
                    DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_USED_IN_POOL);
                }
            }
        }

        if ((err_pool == DEF_NO) &&                                                 /* If NO buf pool err &          .. */
            (pbuf_chk_small != (NET_BUF_SMALL *)pbuf_chk_small->Hdr.NextBufPtr)) {  /* .. buf does NOT point to itself, */
             pbuf_chk_small  = (NET_BUF_SMALL *)pbuf_chk_small->Hdr.NextBufPtr;     /* .. adv to next buf.              */
             i++;

        } else {                                                                    /* Else abort buf pool chk.         */
             pbuf_chk_small  = (NET_BUF_SMALL *)0;
        }
    }

    if (pbuf_chk_small != (NET_BUF_SMALL *)0) {                         /* If ptr non-NULL, then             ...        */
                                                                        /* ... pool nbr bufs > max nbr bufs; ...        */
        DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_POOL_NBR_MAX);         /* ... set err.                                 */
    }


                                                                        /* -------------- CHK SMALL BUFS -------------- */
    pbuf_chk_small = &NetBuf_SmallTbl[0];
    for (i = 0; i < NET_BUF_CFG_NBR_SMALL; i++) {
        if (pbuf_chk_small->Hdr.Type != NET_BUF_TYPE_SMALL) {           /* If SMALL buf type invalid, set err.          */
            DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_TYPE);

        } else {
            if (pbuf_chk_small->Hdr.ID != i) {                          /* If SMALL buf ID   invalid, set err.          */
                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_ID);
            }
/*$PAGE*/
            used = DEF_BIT_IS_SET(pbuf_chk_small->Hdr.Flags, NET_BUF_FLAG_USED);
            if (used != DEF_YES) {                                      /* If SMALL buf unused ...                      */
                if (buf_in_buf_pool_small[i] != DEF_YES) {              /* ... but NOT in pool, set err.                */
                    DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_UNUSED_NOT_IN_POOL);
                }
            }
                                                                        /* If ANY SMALL buf points to itself, set err.  */
            if (((NET_BUF_SMALL *)(pbuf_chk_small->Hdr.PrevPrimListPtr) == pbuf_chk_small) ||
                ((NET_BUF_SMALL *)(pbuf_chk_small->Hdr.NextPrimListPtr) == pbuf_chk_small) ||
                ((NET_BUF_SMALL *)(pbuf_chk_small->Hdr.PrevSecListPtr ) == pbuf_chk_small) ||
                ((NET_BUF_SMALL *)(pbuf_chk_small->Hdr.NextSecListPtr ) == pbuf_chk_small) ||
                ((NET_BUF_SMALL *)(pbuf_chk_small->Hdr.PrevBufPtr     ) == pbuf_chk_small) ||
                ((NET_BUF_SMALL *)(pbuf_chk_small->Hdr.NextBufPtr     ) == pbuf_chk_small)) {
                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_BACK_TO_BUF);
            }

            if (used == DEF_YES) {
                                                                        /* Chk SMALL buf's prev prim list buf.          */
                pbuf_link = pbuf_chk_small->Hdr.PrevPrimListPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to SMALL buf, set err.  */
                            if  ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextPrimListPtr) != pbuf_chk_small) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to SMALL buf, set err.  */
                            if (((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevPrimListPtr) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevBufPtr     ) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextBufPtr     ) == pbuf_chk_small)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TO_BUF);
                            }
                        }
                    }
                }

                                                                        /* Chk SMALL buf's next prim list buf.          */
                pbuf_link = pbuf_chk_small->Hdr.NextPrimListPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to SMALL buf, set err.  */
                            if  ((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevPrimListPtr) != pbuf_chk_small) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to SMALL buf, set err.  */
                            if (((NET_BUF_SMALL *)(pbuf_link->Hdr.NextPrimListPtr) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevBufPtr     ) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextBufPtr     ) == pbuf_chk_small)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TO_BUF);
                            }
                        }
                    }
                }
/*$PAGE*/
                                                                        /* Chk SMALL buf's prev sec  list buf.          */
                pbuf_link = pbuf_chk_small->Hdr.PrevSecListPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to SMALL buf, set err.  */
                            if  ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextSecListPtr) != pbuf_chk_small) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to SMALL buf, set err.  */
                            if (((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevBufPtr    ) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextBufPtr    ) == pbuf_chk_small)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TO_BUF);
                            }
                        }
                    }
                }

                                                                        /* Chk SMALL buf's next sec  list buf.          */
                pbuf_link = pbuf_chk_small->Hdr.NextSecListPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to SMALL buf, set err.  */
                            if  ((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevSecListPtr) != pbuf_chk_small) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to SMALL buf, set err.  */
                            if (((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevBufPtr    ) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextBufPtr    ) == pbuf_chk_small)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TO_BUF);
                            }
                        }
                    }
                }
/*$PAGE*/
                                                                        /* Chk SMALL buf's prev           buf.          */
                pbuf_link = pbuf_chk_small->Hdr.PrevBufPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to SMALL buf, set err.  */
                            if  ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextBufPtr     ) != pbuf_chk_small) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to SMALL buf, set err.  */
                            if (((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevPrimListPtr) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextPrimListPtr) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevSecListPtr ) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextSecListPtr ) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevBufPtr     ) == pbuf_chk_small)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TO_BUF);
                            }
                        }
                    }
                }

                                                                        /* Chk SMALL buf's next           buf.          */
                pbuf_link = pbuf_chk_small->Hdr.NextBufPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to SMALL buf, set err.  */
                            if  ((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevBufPtr     ) != pbuf_chk_small) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to SMALL buf, set err.  */
                            if (((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevPrimListPtr) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextPrimListPtr) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.PrevSecListPtr ) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextSecListPtr ) == pbuf_chk_small) ||
                                ((NET_BUF_SMALL *)(pbuf_link->Hdr.NextBufPtr     ) == pbuf_chk_small)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_SMALL_LINK_TO_BUF);
                            }
                        }
                    }
                }
            }
        }

        pbuf_chk_small++;
    }
#endif



/*$PAGE*/
#if (NET_BUF_CFG_NBR_LARGE > 0)
                                                                        /* ------------ CHK LARGE BUF POOL ------------ */
    for (i = 0; i < NET_BUF_CFG_NBR_LARGE; i++) {
        buf_in_buf_pool_large[i] = DEF_NO;
    }

    pbuf_chk_large = NetBuf_LargePoolPtr;
    i              = 0;
    err_pool       = DEF_NO;
    while ((pbuf_chk_large != (NET_BUF_LARGE *)0) &&                    /* Chk LARGE buf pool for used entries.         */
           (i < NET_BUF_CFG_NBR_LARGE)) {

        if (pbuf_chk_large->Hdr.Type != NET_BUF_TYPE_LARGE) {           /* If  LARGE buf type invalid,    set err.      */
            DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_POOL_TYPE);
            err_pool = DEF_YES;

        } else {
            id = pbuf_chk_large->Hdr.ID;
            if (id >= NET_BUF_CFG_NBR_LARGE) {                          /* If  LARGE buf ID   invalid,    set err.      */
                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_POOL_ID);
                err_pool = DEF_YES;

            } else {
                if (buf_in_buf_pool_large[id] == DEF_YES) {             /* If  LARGE buf already in pool, set err.      */
                    DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_POOL_DUP);
                    err_pool = DEF_YES;
                }
                buf_in_buf_pool_large[id] = DEF_YES;
            
                used = DEF_BIT_IS_SET(pbuf_chk_large->Hdr.Flags, NET_BUF_FLAG_USED);
                if (used != DEF_NO) {                                   /* If  LARGE buf used    in pool, set err.      */
                    DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_USED_IN_POOL);
                }
            }
        }

        if ((err_pool == DEF_NO) &&                                                 /* If NO buf pool err &          .. */
            (pbuf_chk_large != (NET_BUF_LARGE *)pbuf_chk_large->Hdr.NextBufPtr)) {  /* .. buf does NOT point to itself, */
             pbuf_chk_large  = (NET_BUF_LARGE *)pbuf_chk_large->Hdr.NextBufPtr;     /* .. adv to next buf.              */
             i++;

        } else {                                                                    /* Else abort buf pool chk.         */
             pbuf_chk_large  = (NET_BUF_LARGE *)0;
        }
    }

    if (pbuf_chk_large != (NET_BUF_LARGE *)0) {                         /* If ptr non-NULL, then             ...        */
                                                                        /* ... pool nbr bufs > max nbr bufs; ...        */
        DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_POOL_NBR_MAX);         /* ... set err.                                 */
    }


                                                                        /* -------------- CHK LARGE BUFS -------------- */
    pbuf_chk_large = &NetBuf_LargeTbl[0];
    for (i = 0; i < NET_BUF_CFG_NBR_LARGE; i++) {
        if (pbuf_chk_large->Hdr.Type != NET_BUF_TYPE_LARGE) {           /* If LARGE buf type invalid, set err.          */
            DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_TYPE);

        } else {
            if (pbuf_chk_large->Hdr.ID != i) {                          /* If LARGE buf ID   invalid, set err.          */
                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_ID);
            }
/*$PAGE*/
            used = DEF_BIT_IS_SET(pbuf_chk_large->Hdr.Flags, NET_BUF_FLAG_USED);
            if (used != DEF_YES) {                                      /* If LARGE buf unused ...                      */
                if (buf_in_buf_pool_large[i] != DEF_YES) {              /* ... but NOT in pool, set err.                */
                    DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_UNUSED_NOT_IN_POOL);
                }
            }
                                                                        /* If ANY LARGE buf points to itself, set err.  */
            if (((NET_BUF_LARGE *)(pbuf_chk_large->Hdr.PrevPrimListPtr) == pbuf_chk_large) ||
                ((NET_BUF_LARGE *)(pbuf_chk_large->Hdr.NextPrimListPtr) == pbuf_chk_large) ||
                ((NET_BUF_LARGE *)(pbuf_chk_large->Hdr.PrevSecListPtr ) == pbuf_chk_large) ||
                ((NET_BUF_LARGE *)(pbuf_chk_large->Hdr.NextSecListPtr ) == pbuf_chk_large) ||
                ((NET_BUF_LARGE *)(pbuf_chk_large->Hdr.PrevBufPtr     ) == pbuf_chk_large) ||
                ((NET_BUF_LARGE *)(pbuf_chk_large->Hdr.NextBufPtr     ) == pbuf_chk_large)) {
                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_BACK_TO_BUF);
            }

            if (used == DEF_YES) {
                                                                        /* Chk LARGE buf's prev prim list buf.          */
                pbuf_link = pbuf_chk_large->Hdr.PrevPrimListPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to LARGE buf, set err.  */
                            if  ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextPrimListPtr) != pbuf_chk_large) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to LARGE buf, set err.  */
                            if (((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevPrimListPtr) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevBufPtr     ) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextBufPtr     ) == pbuf_chk_large)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TO_BUF);
                            }
                        }
                    }
                }

                                                                        /* Chk LARGE buf's next prim list buf.          */
                pbuf_link = pbuf_chk_large->Hdr.NextPrimListPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to LARGE buf, set err.  */
                            if  ((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevPrimListPtr) != pbuf_chk_large) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to LARGE buf, set err.  */
                            if (((NET_BUF_LARGE *)(pbuf_link->Hdr.NextPrimListPtr) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevBufPtr     ) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextBufPtr     ) == pbuf_chk_large)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TO_BUF);
                            }
                        }
                    }
                }
/*$PAGE*/
                                                                        /* Chk LARGE buf's prev sec  list buf.          */
                pbuf_link = pbuf_chk_large->Hdr.PrevSecListPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to LARGE buf, set err.  */
                            if  ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextSecListPtr) != pbuf_chk_large) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to LARGE buf, set err.  */
                            if (((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevBufPtr    ) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextBufPtr    ) == pbuf_chk_large)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TO_BUF);
                            }
                        }
                    }
                }

                                                                        /* Chk LARGE buf's next sec  list buf.          */
                pbuf_link = pbuf_chk_large->Hdr.NextSecListPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to LARGE buf, set err.  */
                            if  ((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevSecListPtr) != pbuf_chk_large) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to LARGE buf, set err.  */
                            if (((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevBufPtr    ) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextBufPtr    ) == pbuf_chk_large)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TO_BUF);
                            }
                        }
                    }
                }
/*$PAGE*/
                                                                        /* Chk LARGE buf's prev           buf.          */
                pbuf_link = pbuf_chk_large->Hdr.PrevBufPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to LARGE buf, set err.  */
                            if  ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextBufPtr     ) != pbuf_chk_large) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to LARGE buf, set err.  */
                            if (((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevPrimListPtr) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextPrimListPtr) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevSecListPtr ) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextSecListPtr ) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevBufPtr     ) == pbuf_chk_large)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TO_BUF);
                            }
                        }
                    }
                }

                                                                        /* Chk LARGE buf's next           buf.          */
                pbuf_link = pbuf_chk_large->Hdr.NextBufPtr;
                if (pbuf_link != (NET_BUF *)0) {
                    switch (pbuf_link->Hdr.Type) {
                        case NET_BUF_TYPE_SMALL:
                        case NET_BUF_TYPE_LARGE:
                             err_buf = DEF_NO;
                             break;


                        case NET_BUF_TYPE_NONE:                         /* If buf type invalid, set err.                */
                        default:
                             DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TYPE);
                             err_buf = DEF_YES;
                             break;
                    }

                    if (err_buf == DEF_NO) {
                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                       set err.  */
                            DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_UNUSED);

                        } else {
                                                                        /* If NOT pointing back to LARGE buf, set err.  */
                            if  ((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevBufPtr     ) != pbuf_chk_large) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_NOT_TO_BUF);
                            }
                                                                        /* If other links point to LARGE buf, set err.  */
                            if (((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevPrimListPtr) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextPrimListPtr) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.PrevSecListPtr ) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextSecListPtr ) == pbuf_chk_large) ||
                                ((NET_BUF_LARGE *)(pbuf_link->Hdr.NextBufPtr     ) == pbuf_chk_large)) {
                                DEF_BIT_SET(status, NET_DBG_SF_BUF_LARGE_LINK_TO_BUF);
                            }
                        }
                    }
                }
            }
        }

        pbuf_chk_large++;
    }
#endif



/*$PAGE*/
    if (status != NET_DBG_STATUS_OK) {                                  /* For ANY fault status, ...                    */
        DEF_BIT_SET(status, NET_DBG_SF_BUF);                            /* ... set breakpoint.                          */
    }


    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetDbg_ChkStatusHandlerTmrs()
*
* Description : (1) Check network timer management status :
*
*                   (a) Corrupted timer pool
*                   (b) Corrupted timers
*                   (c) Corrupted timer task list
*
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO network timer faults.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_TMR                              Some network timer management fault(s).
*
*                   NET_DBG_SF_TMR_TYPE                         Network timer invalid type.
*                   NET_DBG_SF_TMR_ID                           Network timer invalid ID.
*                   NET_DBG_SF_TMR_LINK_TYPE                    Network timer invalid link type.
*                   NET_DBG_SF_TMR_LINK_UNUSED                  Network timer         link unused.
*                   NET_DBG_SF_TMR_LINK_BACK_TO_TMR             Network timer invalid link back to same timer.
*                   NET_DBG_SF_TMR_LINK_TO_TMR                  Network timer invalid link back to      timer.
*
*                   NET_DBG_SF_TMR_POOL_TYPE                    Network timer invalid pool type.
*                   NET_DBG_SF_TMR_POOL_ID                      Network timer invalid pool ID.
*                   NET_DBG_SF_TMR_POOL_DUP                     Network timer pool contains duplicate timer(s).
*                   NET_DBG_SF_TMR_POOL_NBR_MAX                 Network timer pool number of timers greater
*                                                                   than maximum   number of timers.
*
*                   NET_DBG_SF_TMR_LIST_TYPE                    Network timer task list invalid type.
*                   NET_DBG_SF_TMR_LIST_ID                      Network timer task list invalid ID.
*                   NET_DBG_SF_TMR_LIST_DUP                     Network timer task list contains duplicate timer(s).
*                   NET_DBG_SF_TMR_LIST_NBR_MAX                 Network timer task list number of timers greater
*                                                                   than maximum number of timers.
*                   NET_DBG_SF_TMR_LIST_NBR_USED                Network timer task list number of timers NOT equal
*                                                                   to number of used timers.
*
*                   NET_DBG_SF_TMR_USED_IN_POOL                 Network timer   used but     in pool.
*                   NET_DBG_SF_TMR_UNUSED_NOT_IN_POOL           Network timer unused but NOT in pool.
*                   NET_DBG_SF_TMR_UNUSED_IN_LIST               Network timer unused but     in timer task list.
*
* Caller(s)   : NetDbg_ChkStatusTmrs(),
*               NetDbg_ChkStatusHandler().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (2) NetDbg_ChkStatusHandlerTmrs() is called by network protocol suite function(s) & MUST
*                   be called with the global network lock already acquired.
*
*                   See also 'NetDbg_ChkStatusTmrs()  Note #1'.
*
*               (3) #### Link pointers NOT yet validated to be aligned on CPU pointer-aligned addresses
*                   which MAY lead to unaligned pointer access & generate CPU pointer-alignment exceptions.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_DBG_CFG_STATUS_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_ChkStatusHandlerTmrs (void)
{
    NET_DBG_STATUS   status;
    CPU_BOOLEAN      err_pool;
    CPU_BOOLEAN      err_list;
    CPU_BOOLEAN      used;
    NET_TMR_QTY      i;
    NET_TMR_QTY      tmr_nbr_used;
    NET_TMR_QTY      tmr_nbr_in_list;
    NET_TMR_QTY      tmr_in_pool[NET_TMR_CFG_NBR_TMR];
    NET_TMR_QTY      tmr_in_list[NET_TMR_CFG_NBR_TMR];
    NET_TMR         *ptmr_chk;
    NET_TMR         *ptmr_link;
 

    status = NET_DBG_STATUS_OK;


                                                                        /* --------------- CHK TMR POOL --------------- */
    for (i = 0; i < NET_TMR_CFG_NBR_TMR; i++) {
        tmr_in_pool[i] = DEF_NO;
    }

    ptmr_chk = NetTmr_PoolPtr;
    i        = 0;
    err_pool = DEF_NO;
    while ((ptmr_chk != (NET_TMR *)0) &&                                /* Chk tmr pool for used entries.               */
           (i < NET_TMR_CFG_NBR_TMR)) {

        if (ptmr_chk->Type != NET_TMR_TYPE_TMR) {                       /* If  tmr type invalid,    set err.            */
            DEF_BIT_SET(status, NET_DBG_SF_TMR_POOL_TYPE);
            err_pool = DEF_YES;

        } else if (ptmr_chk->ID >= NET_TMR_CFG_NBR_TMR) {               /* If  tmr ID   invalid,    set err.            */
            DEF_BIT_SET(status, NET_DBG_SF_TMR_POOL_ID);
            err_pool = DEF_YES;

        } else {
            if (tmr_in_pool[ptmr_chk->ID] == DEF_YES) {                 /* If  tmr already in pool, set err.            */
                DEF_BIT_SET(status, NET_DBG_SF_TMR_POOL_DUP);
                err_pool = DEF_YES;
            }
            tmr_in_pool[ptmr_chk->ID] = DEF_YES;
            
            used = DEF_BIT_IS_SET(ptmr_chk->Flags, NET_TMR_FLAG_USED);
            if (used != DEF_NO) {                                       /* If  tmr used    in pool, set err.            */
                DEF_BIT_SET(status, NET_DBG_SF_TMR_USED_IN_POOL);
            }
        }

        if ((err_pool ==  DEF_NO) &&                                    /* If NO tmr pool err                  ...      */
            (ptmr_chk != (NET_TMR *)ptmr_chk->NextPtr)) {               /* ... & tmr does NOT point to itself, ...      */
             ptmr_chk  = (NET_TMR *)ptmr_chk->NextPtr;                  /* ... adv to next tmr.                         */
             i++;

        } else {                                                        /* Else abort tmr pool chk.                     */
             ptmr_chk  = (NET_TMR *)0;
        }
    }

    if (ptmr_chk != (NET_TMR *)0) {                                     /* If ptr non-NULL, then             ...        */
                                                                        /* ... pool nbr tmrs > max nbr tmrs; ...        */
        DEF_BIT_SET(status, NET_DBG_SF_TMR_POOL_NBR_MAX);               /* ... set err.                                 */
    }


/*$PAGE*/
                                                                        /* ----------------- CHK TMRS ----------------- */
    ptmr_chk     = &NetTmr_Tbl[0];
    tmr_nbr_used =  0;
    for (i = 0; i < NET_TMR_CFG_NBR_TMR; i++) {
        if (ptmr_chk->Type != NET_TMR_TYPE_TMR) {                       /* If tmr type invalid, set err.                */
            DEF_BIT_SET(status, NET_DBG_SF_TMR_TYPE);

        } else {
            if (ptmr_chk->ID != i) {                                    /* If tmr ID   invalid, set err.                */
                DEF_BIT_SET(status, NET_DBG_SF_TMR_ID);
            }

            used = DEF_BIT_IS_SET(ptmr_chk->Flags, NET_TMR_FLAG_USED);
            if (used != DEF_YES) {                                      /* If tmr unused ...                            */
                if (tmr_in_pool[i] != DEF_YES) {                        /* ... but NOT in pool, set err.                */
                    DEF_BIT_SET(status, NET_DBG_SF_TMR_UNUSED_NOT_IN_POOL);
                }
            }

            if (ptmr_chk->NextPtr == ptmr_chk) {                        /* If ANY tmr points to itself, set err.        */
                DEF_BIT_SET(status, NET_DBG_SF_TMR_LINK_BACK_TO_TMR);
            }

            if (used == DEF_YES) {
                tmr_nbr_used++;
                                                                        /* Chk tmr's next tmr.                          */
                ptmr_link = ptmr_chk->NextPtr;
                if (ptmr_link != (NET_TMR *)0) {
                    if (ptmr_link->Type != NET_TMR_TYPE_TMR) {          /* If  tmr type invalid,        set err.        */
                        DEF_BIT_SET(status, NET_DBG_SF_TMR_LINK_TYPE);

                    } else {
                        used = DEF_BIT_IS_SET(ptmr_link->Flags, NET_TMR_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used,                 set err.        */
                            DEF_BIT_SET(status, NET_DBG_SF_TMR_LINK_UNUSED);

                        } else {
                                                                        /* If pointing back to tmr,     set err.        */
                            if (ptmr_link->NextPtr == ptmr_chk) {
                                DEF_BIT_SET(status, NET_DBG_SF_TMR_LINK_TO_TMR);
                            }
                        }
                    }
                }
            }
        }

        ptmr_chk++;
    }


/*$PAGE*/
                                                                        /* ------------ CHK TMR TASK LIST ------------- */
    for (i = 0; i < NET_TMR_CFG_NBR_TMR; i++) {
        tmr_in_list[i] = DEF_NO;
    }

    ptmr_chk        = NetTmr_TaskListHead;
    i               = 0;
    tmr_nbr_in_list = 0;
    err_list        = DEF_NO;
    while ((ptmr_chk != (NET_TMR *)0) &&                                /* Chk corrupted tmr task list.                 */
           (i < NET_TMR_CFG_NBR_TMR)) {

        if (ptmr_chk->Type != NET_TMR_TYPE_TMR) {                       /* If   tmr type invalid, set err.              */
            DEF_BIT_SET(status, NET_DBG_SF_TMR_LIST_TYPE);
            err_list = DEF_YES;

        } else if (ptmr_chk->ID >= NET_TMR_CFG_NBR_TMR) {               /* If  tmr ID   invalid,  set err.              */
            DEF_BIT_SET(status, NET_DBG_SF_TMR_LIST_ID);
            err_list = DEF_YES;

        } else {
            if (tmr_in_list[i] == DEF_YES) {                            /* If   tmr already in tmr task list, set err.  */
                DEF_BIT_SET(status, NET_DBG_SF_TMR_LIST_DUP);
                err_list = DEF_YES;
            }
            tmr_in_list[i] = DEF_YES;                                   /* Mark tmr as      in tmr task list.           */
            tmr_nbr_in_list++;

            used = DEF_BIT_IS_SET(ptmr_chk->Flags, NET_TMR_FLAG_USED);
            if (used != DEF_YES) {                                      /* If   tmr in list unused, set err.            */
                DEF_BIT_SET(status, NET_DBG_SF_TMR_UNUSED_IN_LIST);
            }
        }

        if ((err_list ==  DEF_NO) &&                                    /* If NO tmr task list err             ...      */
            (ptmr_chk != (NET_TMR *)ptmr_chk->NextPtr)) {               /* ... & tmr does NOT point to itself, ...      */
             ptmr_chk  = (NET_TMR *)ptmr_chk->NextPtr;                  /* ... adv to next tmr.                         */
             i++;

        } else {                                                        /* Else abort tmr task list chk.                */
             ptmr_chk  = (NET_TMR *)0;
        }
    }

    if (ptmr_chk != (NET_TMR *)0) {                                     /* If ptr non-NULL, then                     .. */
                                                                        /* .. tmr task list nbr tmrs  > max  nbr tmrs;  */
        DEF_BIT_SET(status, NET_DBG_SF_TMR_LIST_NBR_MAX);               /* ... set err.                                 */
    }

    if (tmr_nbr_in_list != tmr_nbr_used) {                              /* If tmr task list nbr tmrs != used nbr tmrs,  */
        DEF_BIT_SET(status, NET_DBG_SF_TMR_LIST_NBR_USED);              /* ... set err.                                 */
    }



    if (status != NET_DBG_STATUS_OK) {                                  /* For ANY fault status, ...                    */
        DEF_BIT_SET(status, NET_DBG_SF_TMR);                            /* ... set breakpoint.                          */
    }


    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetDbg_ChkStatusHandlerConns()
*
* Description : (1) Check network connection management status :
*
*                   (a) Corrupted connection pool
*                   (b) Corrupted connections
*                   (c) Corrupted connection lists
*
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO network connection faults.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_CONN                             Some network connection management fault(s).
*
*                   NET_DBG_SF_CONN_TYPE                        Network connection invalid type.
*                   NET_DBG_SF_CONN_FAMILY                      Network connection invalid family.
*                   NET_DBG_SF_CONN_PROTOCOL_IX_NBR_MAX         Network connection invalid list index number.
*                   NET_DBG_SF_CONN_ID                          Network connection invalid ID.
*                   NET_DBG_SF_CONN_ID_NONE                     Network connection with NO connection IDs.
*                   NET_DBG_SF_CONN_ID_UNUSED                   Network connection linked to unused connection.
*                   NET_DBG_SF_CONN_LINK_TYPE                   Network connection invalid link type.
*                   NET_DBG_SF_CONN_LINK_UNUSED                 Network connection         link unused.
*                   NET_DBG_SF_CONN_LINK_BACK_TO_CONN           Network connection invalid link     back to
*                                                                   same connection.
*                   NET_DBG_SF_CONN_LINK_NOT_TO_CONN            Network connection invalid link NOT back to
*                                                                   same connection.
*                   NET_DBG_SF_CONN_LINK_NOT_IN_LIST            Network connection NOT in appropriate
*                                                                   connection list.
*
*                   NET_DBG_SF_CONN_POOL_TYPE                   Network connection invalid pool type.
*                   NET_DBG_SF_CONN_POOL_ID                     Network connection invalid pool ID.
*                   NET_DBG_SF_CONN_POOL_DUP                    Network connection pool contains duplicate
*                                                                   connection(s).
*                   NET_DBG_SF_CONN_POOL_NBR_MAX                Network connection pool  number of connections
*                                                                   greater than maximum number of connections.
*
*                   NET_DBG_SF_CONN_LIST_NBR_NOT_SOLITARY       Network connection lists number of connections
*                                                                   NOT equal to solitary connection.
*
*                   NET_DBG_SF_CONN_USED_IN_POOL                Network connection   used but     in pool.
*                   NET_DBG_SF_CONN_USED_NOT_IN_LIST            Network connection   used but NOT in list.
*                   NET_DBG_SF_CONN_UNUSED_IN_LIST              Network connection unused but     in list.
*                   NET_DBG_SF_CONN_UNUSED_NOT_IN_POOL          Network connection unused but NOT in pool.
*                   NET_DBG_SF_CONN_IN_LIST_IN_POOL             Network connection     in list &     in pool.
*                   NET_DBG_SF_CONN_NOT_IN_LIST_NOT_IN_POOL     Network connection NOT in list & NOT in pool.
*
* Caller(s)   : NetDbg_ChkStatusConns(),
*               NetDbg_ChkStatusHandler().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (2) NetDbg_ChkStatusHandlerConns() is called by network protocol suite function(s) & MUST
*                   be called with the global network lock already acquired.
*
*                   See also 'NetDbg_ChkStatusConns()  Note #1'.
*
*               (3) #### Link pointers NOT yet validated to be aligned on CPU pointer-aligned addresses
*                   which MAY lead to unaligned pointer access & generate CPU pointer-alignment exceptions.
*
*               (4) #### Network connection lists are NOT yet checked to verify that the correct number
*                   of network connections are included in the appropriate network connection lists.
*********************************************************************************************************
*/
/*$PAGE*/
#if ((NET_DBG_CFG_STATUS_EN == DEF_ENABLED) && \
     (defined(NET_CONN_MODULE_PRESENT)))
NET_DBG_STATUS  NetDbg_ChkStatusHandlerConns (void)
{
    NET_DBG_STATUS      status;
    NET_ERR             err;
    CPU_BOOLEAN         err_pool;
    CPU_BOOLEAN         used;
    NET_CONN_LIST_QTY   j;
    NET_CONN_QTY        i;
    NET_CONN_QTY        conn_list_nbr_conns[NET_CONN_CFG_PROTOCOL_MAX];
    NET_CONN_QTY        conn_in_conn_pool[NET_CONN_CFG_NBR_CONN];
    NET_CONN           *pconn_chk;
    NET_CONN           *pconn_link;
    NET_CONN_ID         conn_id;


    status = NET_DBG_STATUS_OK;


                                                                        /* ------------ CHK NET CONN POOL ------------- */
    for (i = 0; i < NET_CONN_CFG_NBR_CONN; i++) {
        conn_in_conn_pool[i] = DEF_NO;
    }

    pconn_chk = NetConn_PoolPtr;
    i         = 0;
    err_pool  = DEF_NO;
    while ((pconn_chk != (NET_CONN *)0) &&                              /* Chk conn pool for used entries.              */
           (i < NET_CONN_CFG_NBR_CONN)) {

        if (pconn_chk->Type != NET_CONN_TYPE_CONN) {                    /* If  conn type invalid, set err.              */
            DEF_BIT_SET(status, NET_DBG_SF_CONN_POOL_TYPE);
            err_pool = DEF_YES;

        } else if (pconn_chk->ID >= NET_CONN_CFG_NBR_CONN) {            /* If  conn ID   invalid, set err.              */
            DEF_BIT_SET(status, NET_DBG_SF_CONN_POOL_ID);
            err_pool = DEF_YES;

        } else {
            if (conn_in_conn_pool[pconn_chk->ID] == DEF_YES) {          /* If  conn already in pool, set err.           */
                DEF_BIT_SET(status, NET_DBG_SF_CONN_POOL_DUP);
                err_pool = DEF_YES;
            }
            conn_in_conn_pool[pconn_chk->ID] = DEF_YES;
            
            used = DEF_BIT_IS_SET(pconn_chk->Flags, NET_CONN_FLAG_USED);
            if (used != DEF_NO) {                                       /* If  conn used    in pool, set err.           */
                DEF_BIT_SET(status, NET_DBG_SF_CONN_USED_IN_POOL);
            }
        }

        if ((err_pool  ==  DEF_NO) &&                                   /* If NO conn pool err                  ...     */
            (pconn_chk != (NET_CONN *)pconn_chk->NextConnPtr)) {        /* ... & conn does NOT point to itself, ...     */
             pconn_chk  = (NET_CONN *)pconn_chk->NextConnPtr;           /* ... adv to next conn.                        */
             i++;

        } else {                                                        /* Else abort conn pool chk.                    */
             pconn_chk  = (NET_CONN *)0;
        }
    }

    if (pconn_chk != (NET_CONN *)0) {                                   /* If ptr non-NULL, then               ...      */
                                                                        /* ... pool nbr conns > max nbr conns; ...      */
        DEF_BIT_SET(status, NET_DBG_SF_CONN_POOL_NBR_MAX);              /* ... set err.                                 */
    }



/*$PAGE*/
                                                                        /* -------- CHK NET CONNS / CONN LISTS -------- */
    for (j = 0; j < NET_CONN_CFG_PROTOCOL_MAX; j++) {
        conn_list_nbr_conns[j] = 0;
    }

    pconn_chk = &NetConn_Tbl[0];
    for (i = 0; i < NET_CONN_CFG_NBR_CONN; i++) {                       /* Chk corrupted conns &/or conn lists.         */
        if (pconn_chk->Type != NET_CONN_TYPE_CONN) {                    /* If conn type invalid, set err.               */
            DEF_BIT_SET(status, NET_DBG_SF_CONN_TYPE);

        } else {
            if (pconn_chk->ID != i) {                                   /* If conn ID   invalid, set err.               */
                DEF_BIT_SET(status, NET_DBG_SF_CONN_ID);
            }

            used = DEF_BIT_IS_SET(pconn_chk->Flags, NET_CONN_FLAG_USED);
            if (used != DEF_YES) {                                      /* If conn unused ...                           */
                if (conn_in_conn_pool[i] != DEF_YES) {                  /* ... but NOT in pool, set err.                */
                    DEF_BIT_SET(status, NET_DBG_SF_CONN_UNUSED_NOT_IN_POOL);
                }
            }
                                                                        /* If ANY conn points to itself, set err.       */
            if ((pconn_chk->PrevChainPtr == pconn_chk) ||
                (pconn_chk->NextChainPtr == pconn_chk) ||
                (pconn_chk->PrevConnPtr  == pconn_chk) ||
                (pconn_chk->NextConnPtr  == pconn_chk)) {
                DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_BACK_TO_CONN);
            }

            if (pconn_chk->ConnList != (NET_CONN **)0) {                /* If conn     in conn list,     ...            */
                if (used != DEF_YES) {                                  /* ... but NOT used,    set err; ...            */
                    DEF_BIT_SET(status, NET_DBG_SF_CONN_UNUSED_IN_LIST);
                }
                if (conn_in_conn_pool[i] != DEF_NO) {                   /* ... but     in pool, set err.                */
                    DEF_BIT_SET(status, NET_DBG_SF_CONN_IN_LIST_IN_POOL);
                }
            } else {                                                    /* If conn NOT in conn list,     ...            */
                if (used != DEF_NO) {                                   /* ... but     used,    set err; ...            */
                    DEF_BIT_SET(status, NET_DBG_SF_CONN_USED_NOT_IN_LIST);
                }
                if (conn_in_conn_pool[i] != DEF_YES) {                  /* ... but NOT in pool, set err.                */
                    DEF_BIT_SET(status, NET_DBG_SF_CONN_NOT_IN_LIST_NOT_IN_POOL);
                }
            }

            if (used == DEF_YES) {
                switch (pconn_chk->Family) {                            /* Chk valid conn family.                       */
                    case NET_CONN_FAMILY_IP_V4_SOCK:
                         break;


                    case NET_CONN_FAMILY_NONE:
                    default:
                         DEF_BIT_SET(status, NET_DBG_SF_CONN_FAMILY);
                         break;
                }


                if ((pconn_chk->ID_App       == NET_CONN_ID_NONE) &&    /* Chk conn IDs.                                */
                    (pconn_chk->ID_Transport == NET_CONN_ID_NONE)) {
                     DEF_BIT_SET(status, NET_DBG_SF_CONN_ID_NONE);
                }
                                                                        /* Chk conn(s) used.                            */
                conn_id = pconn_chk->ID_App;
                if (conn_id != NET_CONN_ID_NONE) {
                    used = DEF_NO;
#if    (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
                    used = NetSock_IsUsed((NET_SOCK_ID) conn_id,
                                          (NET_ERR   *)&err);
#endif
                    if (used != DEF_YES) {                              /* If app      conn unused, set err.            */
                        DEF_BIT_SET(status, NET_DBG_SF_CONN_ID_UNUSED);
                    }
                }
                                                                        /* Chk conn(s) used.                            */
                conn_id = pconn_chk->ID_Transport;
                if (conn_id != NET_CONN_ID_NONE) {
                    used = DEF_NO;
#if    (NET_CONN_CFG_FAMILY == NET_CONN_FAMILY_IP_V4_SOCK)
#ifdef  NET_TCP_MODULE_PRESENT
                    used = NetTCP_ConnIsUsed((NET_TCP_CONN_ID) conn_id,
                                             (NET_ERR       *)&err);
#endif
#endif
                    if (used != DEF_YES) {                              /* If transport conn unused, set err.           */
                        DEF_BIT_SET(status, NET_DBG_SF_CONN_ID_UNUSED);
                    }
                }

/*$PAGE*/
                if (pconn_chk->ConnList != (NET_CONN **)0) {
                                                                        /* If protocol ix > max, set err.               */
                    if (pconn_chk->ProtocolIx >= NET_CONN_CFG_PROTOCOL_MAX) {
                        DEF_BIT_SET(status, NET_DBG_SF_CONN_PROTOCOL_IX_NBR_MAX);

                    } else {                                            /* If conn in valid conn list, ...              */
                                                                        /* ... inc nbr conn list conns (see Note #4).   */
                        conn_list_nbr_conns[pconn_chk->ProtocolIx]++;

                                                                        /* If conn points to NO other conn(s),   ...    */
                        if ((pconn_chk->PrevChainPtr == (NET_CONN *)0) &&
                            (pconn_chk->NextChainPtr == (NET_CONN *)0) &&
                            (pconn_chk->PrevConnPtr  == (NET_CONN *)0) &&
                            (pconn_chk->NextConnPtr  == (NET_CONN *)0)) {
                                                                        /* ... & NOT solitary conn in conn list, ...    */
                            if (conn_list_nbr_conns[pconn_chk->ProtocolIx] > 1) {
                                                                        /* ... set err.                                 */
                                DEF_BIT_SET(status, NET_DBG_SF_CONN_LIST_NBR_NOT_SOLITARY);
                            }

                        } else {
                                                                        /* Chk conn's prev chain conn.                  */
                            pconn_link = pconn_chk->PrevChainPtr;
                            if (pconn_link != (NET_CONN *)0) {
                                                                        /* If conn type invalid, set err.               */
                                if (pconn_link->Type != NET_CONN_TYPE_CONN) {
                                    DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_TYPE);

                                } else {
                                    used = DEF_BIT_IS_SET(pconn_link->Flags, NET_CONN_FLAG_USED);
                                    if (used != DEF_YES) {              /* If NOT used,                  set err.       */
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_UNUSED);
                                                                        /* If NOT in conn's conn list,   set err.       */
                                    } else if (pconn_link->ConnList     != pconn_chk->ConnList) {
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_NOT_IN_LIST);
                                                                        /* If NOT pointing back to conn, set err.       */
                                    } else if (pconn_link->NextChainPtr != pconn_chk) {
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_NOT_TO_CONN);
                                    }
                                }
                            }

                                                                        /* Chk conn's next chain conn.                  */
                            pconn_link = pconn_chk->NextChainPtr;
                            if (pconn_link != (NET_CONN *)0) {
                                                                        /* If conn type invalid, set err.               */
                                if (pconn_link->Type != NET_CONN_TYPE_CONN) {
                                    DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_TYPE);

                                } else {
                                    used = DEF_BIT_IS_SET(pconn_link->Flags, NET_CONN_FLAG_USED);
                                    if (used != DEF_YES) {              /* If NOT used,                  set err.       */
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_UNUSED);
                                                                        /* If NOT in conn's conn list,   set err.       */
                                    } else if (pconn_link->ConnList     != pconn_chk->ConnList) {
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_NOT_IN_LIST);
                                                                        /* If NOT pointing back to conn, set err.       */
                                    } else if (pconn_link->PrevChainPtr != pconn_chk) {
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_NOT_TO_CONN);
                                    }
                                }
                            }
/*$PAGE*/
                                                                        /* Chk conn's prev conn.                        */
                            pconn_link = pconn_chk->PrevConnPtr;
                            if (pconn_link != (NET_CONN *)0) {
                                                                        /* If conn type invalid, set err.               */
                                if (pconn_link->Type != NET_CONN_TYPE_CONN) {
                                    DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_TYPE);

                                } else {
                                    used = DEF_BIT_IS_SET(pconn_link->Flags, NET_CONN_FLAG_USED);
                                    if (used != DEF_YES) {              /* If NOT used,                  set err.       */
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_UNUSED);
                                                                        /* If NOT in conn's conn list,   set err.       */
                                    } else if (pconn_link->ConnList    != pconn_chk->ConnList) {
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_NOT_IN_LIST);
                                                                        /* If NOT pointing back to conn, set err.       */
                                    } else if (pconn_link->NextConnPtr != pconn_chk) {
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_NOT_TO_CONN);
                                    }
                                }
                            }

                                                                        /* Chk conn's next conn.                        */
                            pconn_link = pconn_chk->NextConnPtr;
                            if (pconn_link != (NET_CONN *)0) {
                                                                        /* If conn type invalid, set err.               */
                                if (pconn_link->Type != NET_CONN_TYPE_CONN) {
                                    DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_TYPE);

                                } else {
                                    used = DEF_BIT_IS_SET(pconn_link->Flags, NET_CONN_FLAG_USED);
                                    if (used != DEF_YES) {              /* If NOT used,                  set err.       */
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_UNUSED);
                                                                        /* If NOT in conn's conn list,   set err.       */
                                    } else if (pconn_link->ConnList    != pconn_chk->ConnList) {
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_NOT_IN_LIST);
                                                                        /* If NOT pointing back to conn, set err.       */
                                    } else if (pconn_link->PrevConnPtr != pconn_chk) {
                                        DEF_BIT_SET(status, NET_DBG_SF_CONN_LINK_NOT_TO_CONN);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        pconn_chk++;
    }


   (void)&err;                                                          /* Prevent possible compiler warning.           */


    if (status != NET_DBG_STATUS_OK) {                                  /* For ANY fault status, ...                    */
        DEF_BIT_SET(status, NET_DBG_SF_CONN);                           /* ... set breakpoint.                          */
    }


    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetDbg_ChkStatusHandlerTCP()
*
* Description : (1) Check TCP layer status :
*
*                   (a) Corrupted TCP connection pool
*                   (b) Corrupted TCP connections
*
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO TCP layer faults.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_TCP                              Some TCP layer fault(s).
*
*                   NET_DBG_SF_TCP_CONN_TYPE                    TCP connection invalid type.
*                   NET_DBG_SF_TCP_CONN_ID                      TCP connection invalid ID.
*                   NET_DBG_SF_TCP_CONN_LINK_TYPE               TCP connection invalid link type.
*                   NET_DBG_SF_TCP_CONN_LINK_UNUSED             TCP connection         link unused.
*
*                   NET_DBG_SF_TCP_CONN_POOL_TYPE               TCP connection invalid pool type.
*                   NET_DBG_SF_TCP_CONN_POOL_ID                 TCP connection invalid pool ID.
*                   NET_DBG_SF_TCP_CONN_POOL_DUP                TCP connection pool contains duplicate connection(s).
*                   NET_DBG_SF_TCP_CONN_POOL_NBR_MAX            TCP connection pool number of connections greater
*                                                                   than maximum    number of connections.
*
*                   NET_DBG_SF_TCP_CONN_USED_IN_POOL            TCP connection   used but     in pool.
*                   NET_DBG_SF_TCP_CONN_UNUSED_NOT_IN_POOL      TCP connection unused but NOT in pool.
*
*                                                               --- RETURNED BY NetDbg_ChkStatusHandlerTCP_Q() : ----
*                   NET_DBG_SF_TCP_CONN_Q                       Some TCP connection queue fault(s).
*                   NET_DBG_SF_TCP_CONN_Q_BUF_TYPE              TCP connection queue buffer invalid type.     
*                   NET_DBG_SF_TCP_CONN_Q_BUF_UNUSED            TCP connection queue buffer         unused.     
*                   NET_DBG_SF_TCP_CONN_Q_LINK_TYPE             TCP connection queue buffer invalid link type.
*                   NET_DBG_SF_TCP_CONN_Q_LINK_UNUSED           TCP connection queue buffer         link unused.
*                   NET_DBG_SF_TCP_CONN_Q_BUF_DUP               TCP connection queue contains duplicate buffer(s).
*
* Caller(s)   : NetDbg_ChkStatusTCP(),
*               NetDbg_ChkStatusHandler().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (2) NetDbg_ChkStatusHandlerTCP() is called by network protocol suite function(s) & MUST
*                   be called with the global network lock already acquired.
*
*                   See also 'NetDbg_ChkStatusTCP()  Note #1'.
*
*               (3) #### Link pointers NOT yet validated to be aligned on CPU pointer-aligned addresses
*                   which MAY lead to unaligned pointer access & generate CPU pointer-alignment exceptions.
*********************************************************************************************************
*/
/*$PAGE*/
#if ((NET_DBG_CFG_STATUS_EN == DEF_ENABLED) && \
     (defined(NET_TCP_MODULE_PRESENT)))
NET_DBG_STATUS  NetDbg_ChkStatusHandlerTCP (void)
{
    NET_DBG_STATUS     status;
    NET_DBG_STATUS     status_q;
    CPU_BOOLEAN        err_pool;
    CPU_BOOLEAN        used;
    NET_TCP_CONN_QTY   i;
    NET_TCP_CONN_QTY   conn_in_conn_pool[NET_TCP_CFG_NBR_CONN];
    NET_TCP_CONN      *pconn_chk;


    status = NET_DBG_STATUS_OK;


                                                                        /* ------------ CHK TCP CONN POOL ------------- */
    for (i = 0; i < NET_TCP_CFG_NBR_CONN; i++) {
        conn_in_conn_pool[i] = DEF_NO;
    }

    pconn_chk = NetTCP_ConnPoolPtr;
    i         = 0;
    err_pool  = DEF_NO;
    while ((pconn_chk != (NET_TCP_CONN *)0) &&                          /* Chk TCP conn pool for used entries.          */
           (i < NET_TCP_CFG_NBR_CONN)) {

        if (pconn_chk->Type != NET_TCP_TYPE_CONN) {                     /* If  TCP conn type invalid, set err.          */
            DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_POOL_TYPE);
            err_pool = DEF_YES;

        } else if (pconn_chk->ID >= NET_TCP_CFG_NBR_CONN) {             /* If  TCP conn ID   invalid, set err.          */
            DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_POOL_ID);
            err_pool = DEF_YES;

        } else {
            if (conn_in_conn_pool[pconn_chk->ID] == DEF_YES) {          /* If  TCP conn already in pool, set err.       */
                DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_POOL_DUP);
                err_pool = DEF_YES;
            }
            conn_in_conn_pool[pconn_chk->ID] = DEF_YES;
            
            used = DEF_BIT_IS_SET(pconn_chk->Flags, NET_TCP_FLAG_USED);
            if (used != DEF_NO) {                                       /* If  TCP conn used    in pool, set err.       */
                DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_USED_IN_POOL);
            }
        }

        if ((err_pool  ==  DEF_NO) &&                                   /* If NO TCP conn pool err                  ... */
            (pconn_chk != (NET_TCP_CONN *)pconn_chk->NextPtr)) {        /* ... & TCP conn does NOT point to itself, ... */
             pconn_chk  = (NET_TCP_CONN *)pconn_chk->NextPtr;           /* ... adv to next TCP conn.                    */
             i++;

        } else {                                                        /* Else abort TCP conn pool chk.                */
             pconn_chk  = (NET_TCP_CONN *)0;
        }
    }

    if (pconn_chk != (NET_TCP_CONN *)0) {                               /* If ptr non-NULL, then                      ..*/
                                                                        /* .. pool nbr TCP conns > max nbr TCP conns; ..*/
        DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_POOL_NBR_MAX);          /* .. set err.                                  */
    }


/*$PAGE*/
                                                                        /* -------------- CHK TCP CONNS --------------- */
    pconn_chk = &NetTCP_ConnTbl[0];
    for (i = 0; i < NET_TCP_CFG_NBR_CONN; i++) {
        if (pconn_chk->Type != NET_TCP_TYPE_CONN) {                     /* If TCP conn type invalid, set err.           */
            DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_TYPE);

        } else {
            if (pconn_chk->ID != i) {                                   /* If TCP conn ID   invalid, set err.           */
                DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_ID);
            }

            used = DEF_BIT_IS_SET(pconn_chk->Flags, NET_TCP_FLAG_USED);
            if (used != DEF_YES) {                                      /* If TCP conn unused ...                       */
                if (conn_in_conn_pool[i] != DEF_YES) {                  /* ... but NOT in pool, set err.                */
                    DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_UNUSED_NOT_IN_POOL);
                }

            } else {                                                    /* Chk TCP conn Q's.                            */
                                                                        /* Chk TCP conn transport rx Q.                 */
                status_q = NetDbg_ChkStatusHandlerTCP_Q(pconn_chk, pconn_chk->RxQ_Transport_Head);
                DEF_BIT_SET(status, status_q);
                                                                        /* Chk TCP conn app       rx Q.                 */
                status_q = NetDbg_ChkStatusHandlerTCP_Q(pconn_chk, pconn_chk->RxQ_App_Head);
                DEF_BIT_SET(status, status_q);
                                                                        /* Chk TCP conn    tx Q.                        */
                status_q = NetDbg_ChkStatusHandlerTCP_Q(pconn_chk, pconn_chk->TxQ_Head);
                DEF_BIT_SET(status, status_q);
                                                                        /* Chk TCP conn re-tx Q.                        */
                status_q = NetDbg_ChkStatusHandlerTCP_Q(pconn_chk, pconn_chk->ReTxQ_Head);
                DEF_BIT_SET(status, status_q);
            }
        }

        pconn_chk++;
    }



    if (status != NET_DBG_STATUS_OK) {                                  /* For ANY fault status, ...                    */
        DEF_BIT_SET(status, NET_DBG_SF_TCP);                            /* ... set breakpoint.                          */
    }


    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetDbg_ChkStatusHandlerRsrcLost()
*
* Description : (1) Check network protocol suite lost resources status :
*
*                   (a) Network buffer(s)
*                   (b) Network timer(s)
*                   (c) Network connection(s)
*                   (d) ARP    layer
*                   (e) TCP    layer
*                   (f) Socket layer
*
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO network resources lost.
*
*               Otherwise, the return network status codes logically OR'd :
*
*                   NET_DBG_SF_RSRC_LOST                        Some network                resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_BUF_SMALL              Some network SMALL buffer   resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_BUF_LARGE              Some network LARGE buffer   resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_TMR                    Some network timer          resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_CONN                   Some network     connection resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_ARP_CACHE              Some network ARP cache      resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_TCP_CONN               Some network TCP connection resource(s) lost.
*                   NET_DBG_SF_RSRC_LOST_SOCK                   Some network socket         resource(s) lost.
*
* Caller(s)   : NetDbg_ChkStatusRsrcLost(),
*               NetDbg_ChkStatusHandler(),
*               NetDbg_MonTask().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (2) NetDbg_ChkStatusHandlerRsrcLost() is called by network protocol suite function(s) &
*                   MUST be called with the global network lock already acquired.
*
*                   See also 'NetDbg_ChkStatusRsrcLost()  Note #1'.
*********************************************************************************************************
*/
/*$PAGE*/
#if (NET_DBG_CFG_STATUS_RSRC_LOST_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_ChkStatusHandlerRsrcLost (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR            cpu_sr;
#endif
    NET_DBG_STATUS    status;
    NET_DBG_RSRC_QTY  nbr_lost;


    status = NET_DBG_STATUS_OK;

                                                                        /* ------------- CHK LOST RSRC's -------------- */
                                                                        /* --------- CHK NET BUF  LOST RSRC's --------- */
#if (NET_BUF_CFG_NBR_SMALL > 0)
    CPU_CRITICAL_ENTER();
    nbr_lost = (NET_DBG_RSRC_QTY)NetBuf_SmallPoolStat.EntriesLostCur;
    CPU_CRITICAL_EXIT();
    if (nbr_lost > 0) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LOST_BUF_SMALL);
    }
#endif

#if (NET_BUF_CFG_NBR_LARGE > 0)
    CPU_CRITICAL_ENTER();
    nbr_lost = (NET_DBG_RSRC_QTY)NetBuf_LargePoolStat.EntriesLostCur;
    CPU_CRITICAL_EXIT();
    if (nbr_lost > 0) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LOST_BUF_LARGE);
    }
#endif

                                                                        /* --------- CHK NET TMR  LOST RSRC's --------- */
    CPU_CRITICAL_ENTER();
    nbr_lost = (NET_DBG_RSRC_QTY)NetTmr_PoolStat.EntriesLostCur;
    CPU_CRITICAL_EXIT();
    if (nbr_lost > 0) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LOST_TMR);
    }


#ifdef  NET_CONN_MODULE_PRESENT                                         /* --------- CHK NET CONN LOST RSRC's --------- */
    CPU_CRITICAL_ENTER();
    nbr_lost = (NET_DBG_RSRC_QTY)NetConn_PoolStat.EntriesLostCur;
    CPU_CRITICAL_EXIT();
    if (nbr_lost > 0) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LOST_CONN);
    }
#endif


#ifdef  NET_ARP_MODULE_PRESENT                                          /* -------- CHK ARP  LAYER LOST RSRC's -------- */
    CPU_CRITICAL_ENTER();
    nbr_lost = (NET_DBG_RSRC_QTY)NetARP_CachePoolStat.EntriesLostCur;
    CPU_CRITICAL_EXIT();
    if (nbr_lost > 0) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LOST_ARP_CACHE);
    }
#endif


#ifdef  NET_TCP_MODULE_PRESENT                                          /* -------- CHK TCP  LAYER LOST RSRC's -------- */
    CPU_CRITICAL_ENTER();
    nbr_lost = (NET_DBG_RSRC_QTY)NetTCP_ConnPoolStat.EntriesLostCur;
    CPU_CRITICAL_EXIT();
    if (nbr_lost > 0) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LOST_TCP_CONN);
    }
#endif


#ifdef  NET_SOCK_MODULE_PRESENT                                         /* -------- CHK SOCK LAYER LOST RSRC's -------- */
    CPU_CRITICAL_ENTER();
    nbr_lost = (NET_DBG_RSRC_QTY)NetSock_PoolStat.EntriesLostCur;
    CPU_CRITICAL_EXIT();
    if (nbr_lost > 0) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LOST_SOCK);
    }
#endif



    if (status != NET_DBG_STATUS_OK) {                                  /* For ANY fault status, ...                    */
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LOST);                      /* ... set breakpoint.                          */
    }


    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetDbg_ChkStatusHandlerRsrcLo()
*
* Description : (1) Check network protocol suite low resources status :
*
*                   (a) Network buffer(s)
*                   (b) Network timer(s)
*                   (c) Network connection(s)
*                   (d) ARP    layer
*                   (e) TCP    layer
*                   (f) Socket layer
*
*
* Argument(s) : none.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO network resources low.
*
*               Otherwise, the return network status codes logically OR'd :
*
*                   NET_DBG_SF_RSRC_LO                          Some network                resource(s) low.
*                   NET_DBG_SF_RSRC_LO_BUF_SMALL                     Network SMALL buffer   resources   low.
*                   NET_DBG_SF_RSRC_LO_BUF_LARGE                     Network LARGE buffer   resources   low.
*                   NET_DBG_SF_RSRC_LO_TMR                           Network timer          resources   low.
*                   NET_DBG_SF_RSRC_LO_CONN                          Network     connection resources   low.
*                   NET_DBG_SF_RSRC_LO_ARP_CACHE                     Network ARP cache      resources   low.
*                   NET_DBG_SF_RSRC_LO_TCP_CONN                      Network TCP connection resources   low.
*                   NET_DBG_SF_RSRC_LO_SOCK                          Network socket         resources   low.
*
* Caller(s)   : NetDbg_ChkStatusRsrcLo(),
*               NetDbg_ChkStatusHandler(),
*               NetDbg_MonTask().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (2) NetDbg_ChkStatusHandlerRsrcLo() is called by network protocol suite function(s) &
*                   MUST be called with the global network lock already acquired.
*
*                   See also 'NetDbg_ChkStatusRsrcLo()  Note #1'.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_STATUS_RSRC_LO_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_ChkStatusHandlerRsrcLo (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR            cpu_sr;
#endif
    NET_DBG_STATUS    status_cur;
    NET_DBG_STATUS    status;
    NET_DBG_RSRC_QTY  nbr_used;
    NET_DBG_RSRC_QTY  rsrc_th;
    CPU_BOOLEAN       rsrc_lo;


                                                                        /* Get cur net lo rsrc status.                  */
#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)
    CPU_CRITICAL_ENTER();
    status_cur = NetDbg_MonTaskStatusRsrcLo;
    CPU_CRITICAL_EXIT();
#else
    status_cur = NET_DBG_STATUS_OK;
#endif
    status     = NET_DBG_STATUS_OK;


/*$PAGE*/
                                                                        /* -------------- CHK LO RSRC's --------------- */
                                                                        /* ---------- CHK NET BUF  LO RSRC's ---------- */
#if (NET_BUF_CFG_NBR_SMALL > 0)
    CPU_CRITICAL_ENTER();
    nbr_used = (NET_DBG_RSRC_QTY)NetBuf_SmallPoolStat.EntriesUsed;
    rsrc_lo  =  DEF_BIT_IS_SET(status_cur, NET_DBG_SF_RSRC_LO_BUF_SMALL);
    if (rsrc_lo == DEF_NO) {                                            /* If cur'ly NO lo rsrc, chk lo rsrc th.        */
        rsrc_th  = NetDbg_RsrcBufSmallThLo_nbr;

    } else {                                                            /* Else for     lo rsrc, chk lo rsrc th clr'd.  */
        if (NetDbg_RsrcBufSmallThLo_nbr > NetDbg_RsrcBufSmallThLoHyst_nbr) {
            rsrc_th = NetDbg_RsrcBufSmallThLo_nbr - NetDbg_RsrcBufSmallThLoHyst_nbr;
        } else {
            rsrc_th = NET_DBG_RSRC_TH_LO_MIN;
        }
    }
    CPU_CRITICAL_EXIT();

    if (nbr_used >= rsrc_th) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LO_BUF_SMALL);
    }
#endif

#if (NET_BUF_CFG_NBR_LARGE > 0)
    CPU_CRITICAL_ENTER();
    nbr_used = (NET_DBG_RSRC_QTY)NetBuf_LargePoolStat.EntriesUsed;
    rsrc_lo  =  DEF_BIT_IS_SET(status_cur, NET_DBG_SF_RSRC_LO_BUF_LARGE);
    if (rsrc_lo == DEF_NO) {                                            /* If cur'ly NO lo rsrc, chk lo rsrc th.        */
        rsrc_th  = NetDbg_RsrcBufLargeThLo_nbr;

    } else {                                                            /* Else for     lo rsrc, chk lo rsrc th clr'd.  */
        if (NetDbg_RsrcBufLargeThLo_nbr > NetDbg_RsrcBufLargeThLoHyst_nbr) {
            rsrc_th = NetDbg_RsrcBufLargeThLo_nbr - NetDbg_RsrcBufLargeThLoHyst_nbr;
        } else {
            rsrc_th = NET_DBG_RSRC_TH_LO_MIN;
        }
    }
    CPU_CRITICAL_EXIT();

    if (nbr_used >= rsrc_th) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LO_BUF_LARGE);
    }
#endif


                                                                        /* ---------- CHK NET TMR  LO RSRC's ---------- */
    CPU_CRITICAL_ENTER();
    nbr_used = (NET_DBG_RSRC_QTY)NetTmr_PoolStat.EntriesUsed;
    rsrc_lo  =  DEF_BIT_IS_SET(status_cur, NET_DBG_SF_RSRC_LO_TMR);
    if (rsrc_lo == DEF_NO) {                                            /* If cur'ly NO lo rsrc, chk lo rsrc th.        */
        rsrc_th  = NetDbg_RsrcTmrThLo_nbr;

    } else {                                                            /* Else for     lo rsrc, chk lo rsrc th clr'd.  */
        if (NetDbg_RsrcTmrThLo_nbr > NetDbg_RsrcTmrThLoHyst_nbr) {
            rsrc_th = NetDbg_RsrcTmrThLo_nbr - NetDbg_RsrcTmrThLoHyst_nbr;
        } else {
            rsrc_th = NET_DBG_RSRC_TH_LO_MIN;
        }
    }
    CPU_CRITICAL_EXIT();

    if (nbr_used >= rsrc_th) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LO_TMR);
    }



#ifdef  NET_CONN_MODULE_PRESENT                                         /* ---------- CHK NET CONN LO RSRC's ---------- */
    CPU_CRITICAL_ENTER();
    nbr_used = (NET_DBG_RSRC_QTY)NetConn_PoolStat.EntriesUsed;
    rsrc_lo  =  DEF_BIT_IS_SET(status_cur, NET_DBG_SF_RSRC_LO_CONN);
    if (rsrc_lo == DEF_NO) {                                            /* If cur'ly NO lo rsrc, chk lo rsrc th.        */
        rsrc_th  = NetDbg_RsrcConnThLo_nbr;

    } else {                                                            /* Else for     lo rsrc, chk lo rsrc th clr'd.  */
        if (NetDbg_RsrcConnThLo_nbr > NetDbg_RsrcConnThLoHyst_nbr) {
            rsrc_th = NetDbg_RsrcConnThLo_nbr - NetDbg_RsrcConnThLoHyst_nbr;
        } else {
            rsrc_th = NET_DBG_RSRC_TH_LO_MIN;
        }
    }
    CPU_CRITICAL_EXIT();

    if (nbr_used >= rsrc_th) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LO_CONN);
    }
#endif



/*$PAGE*/
#ifdef  NET_ARP_MODULE_PRESENT                                          /* --------- CHK ARP  LAYER LO RSRC's --------- */
    CPU_CRITICAL_ENTER();
    nbr_used = (NET_DBG_RSRC_QTY)NetARP_CachePoolStat.EntriesUsed;
    rsrc_lo  =  DEF_BIT_IS_SET(status_cur, NET_DBG_SF_RSRC_LO_ARP_CACHE);
    if (rsrc_lo == DEF_NO) {                                            /* If cur'ly NO lo rsrc, chk lo rsrc th.        */
        rsrc_th  = NetDbg_RsrcARP_CacheThLo_nbr;

    } else {                                                            /* Else for     lo rsrc, chk lo rsrc th clr'd.  */
        if (NetDbg_RsrcARP_CacheThLo_nbr > NetDbg_RsrcARP_CacheThLoHyst_nbr) {
            rsrc_th = NetDbg_RsrcARP_CacheThLo_nbr - NetDbg_RsrcARP_CacheThLoHyst_nbr;
        } else {
            rsrc_th = NET_DBG_RSRC_TH_LO_MIN;
        }
    }
    CPU_CRITICAL_EXIT();

    if (nbr_used >= rsrc_th) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LO_ARP_CACHE);
    }
#endif



#ifdef  NET_TCP_MODULE_PRESENT                                          /* --------- CHK TCP  LAYER LO RSRC's --------- */
    CPU_CRITICAL_ENTER();
    nbr_used = (NET_DBG_RSRC_QTY)NetTCP_ConnPoolStat.EntriesUsed;
    rsrc_lo  =  DEF_BIT_IS_SET(status_cur, NET_DBG_SF_RSRC_LO_TCP_CONN);
    if (rsrc_lo == DEF_NO) {                                            /* If cur'ly NO lo rsrc, chk lo rsrc th.        */
        rsrc_th  = NetDbg_RsrcTCP_ConnThLo_nbr;

    } else {                                                            /* Else for     lo rsrc, chk lo rsrc th clr'd.  */
        if (NetDbg_RsrcTCP_ConnThLo_nbr > NetDbg_RsrcTCP_ConnThLoHyst_nbr) {
            rsrc_th = NetDbg_RsrcTCP_ConnThLo_nbr - NetDbg_RsrcTCP_ConnThLoHyst_nbr;
        } else {
            rsrc_th = NET_DBG_RSRC_TH_LO_MIN;
        }
    }
    CPU_CRITICAL_EXIT();

    if (nbr_used >= rsrc_th) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LO_TCP_CONN);
    }
#endif



#ifdef  NET_SOCK_MODULE_PRESENT                                         /* --------- CHK SOCK LAYER LO RSRC's --------- */
    CPU_CRITICAL_ENTER();
    nbr_used = (NET_DBG_RSRC_QTY)NetSock_PoolStat.EntriesUsed;
    rsrc_lo  =  DEF_BIT_IS_SET(status_cur, NET_DBG_SF_RSRC_LO_SOCK);
    if (rsrc_lo == DEF_NO) {                                            /* If cur'ly NO lo rsrc, chk lo rsrc th.        */
        rsrc_th  = NetDbg_RsrcSockThLo_nbr;

    } else {                                                            /* Else for     lo rsrc, chk lo rsrc th clr'd.  */
        if (NetDbg_RsrcSockThLo_nbr > NetDbg_RsrcSockThLoHyst_nbr) {
            rsrc_th = NetDbg_RsrcSockThLo_nbr - NetDbg_RsrcSockThLoHyst_nbr;
        } else {
            rsrc_th = NET_DBG_RSRC_TH_LO_MIN;
        }
    }
    CPU_CRITICAL_EXIT();

    if (nbr_used >= rsrc_th) {
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LO_SOCK);
    }
#endif



    if (status != NET_DBG_STATUS_OK) {                                  /* For ANY fault status, ...                    */
        DEF_BIT_SET(status, NET_DBG_SF_RSRC_LO);                        /* ... set breakpoint.                          */
    }


    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetDbg_MonTaskStatGetHandlerLost()
*
* Description : Get current Debug Monitor Task lost resource status.
*
* Argument(s) : none.
*
* Return(s)   : Current Debug Monitor Task lost resource status.
*
*               See also 'NetDbg_ChkStatusHandlerRsrcLost()  Return(s)'.
*
* Caller(s)   : NetDbg_MonTaskStatusGetRsrcLost().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (1) NetDbg_MonTaskStatusGetHandlerRsrcLost() abbreviated to NetDbg_MonTaskStatGetHandlerLost()
*                   to enforce ANSI-compliance of 31-character symbol length uniqueness.
*
*                   See also 'net_dbg.h  NETWORK DEBUG STATUS FUNCTION PROTOTYPES  Note #2'.
*
*               (2) NetDbg_MonTaskStatGetHandlerLost() is called by network protocol suite function(s) &
*                   MUST be called with the global network lock already acquired.
*
*                   See also 'NetDbg_MonTaskStatusGetRsrcLost()  Note #1'.
*
*               (3) (a) On every status get, the Debug Monitor Task Handler timer is checked in case 
*                       no timer was available to reschedule the Debug Monitor Task Handler.
*
*                   (b) Do NOT need to verify success of Timer Get since every status get will check
*                       & get a Debug Monitor Task Handler timer as needed.
*
*                   See also 'NetDbg_MonTask()  Note #4'.
*
*               (4) 'NetDbg_MonTaskStatusRsrcLost' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_MonTaskStatGetHandlerLost (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR          cpu_sr;
#endif
    NET_DBG_STATUS  status;
    NET_TMR_TICK    timeout_tick;
    NET_ERR         err;


    CPU_CRITICAL_ENTER();
    status = NetDbg_MonTaskStatusRsrcLost;                              /* Get mon task rsrc lost status.               */
    CPU_CRITICAL_EXIT();

    if (NetDbg_MonTaskTmr == (NET_TMR *)0) {                            /* If mon task tmr NULL, get tmr (see Note #3a).*/
        CPU_CRITICAL_ENTER();
        timeout_tick       =  NetDbg_MonTaskTime_tick;
        CPU_CRITICAL_EXIT();
        NetDbg_MonTaskTmr  =  NetTmr_Get((void       *) NetDbg_MonTask,
                                         (CPU_FNCT_PTR) NetDbg_MonTask,
                                         (NET_TMR_TICK) timeout_tick,
                                         (CPU_INT16U  ) NET_TMR_FLAG_NONE,
                                         (NET_ERR    *)&err);           /* See Note #3b.                                */
    }

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetDbg_MonTaskStatGetHandlerLo()
*
* Description : Get current Debug Monitor Task low resource status.
*
* Argument(s) : none.
*
* Return(s)   : Current Debug Monitor Task low resource status.
*
*               See also 'NetDbg_ChkStatusHandlerRsrcLo()  Return(s)'.
*
* Caller(s)   : NetDbg_MonTaskStatusGetRsrcLo(),
*               NetICMP_HandlerRx().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s) [see also Note #2].
*
* Note(s)     : (1) NetDbg_MonTaskStatusGetHandlerRsrcLo() abbreviated to NetDbg_MonTaskStatGetHandlerLo()
*                   to enforce ANSI-compliance of 31-character symbol length uniqueness.
*
*                   See also 'net_dbg.h  NETWORK DEBUG STATUS FUNCTION PROTOTYPES  Note #2'.
*
*               (2) NetDbg_MonTaskStatGetHandlerLost() is called by network protocol suite function(s) &
*                   MUST be called with the global network lock already acquired.
*
*                   See also 'NetDbg_MonTaskStatusGetRsrcLo()  Note #1'.
*
*               (3) (a) On every status get, the Debug Monitor Task Handler timer is checked in case 
*                       no timer was available to reschedule the Debug Monitor Task Handler.
*
*                   (b) Do NOT need to verify success of Timer Get since every status get will check
*                       & get a Debug Monitor Task Handler timer as needed.
*
*                   See also 'NetDbg_MonTask()  Note #4'.
*
*               (4) 'NetDbg_MonTaskStatusRsrcLo' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)
NET_DBG_STATUS  NetDbg_MonTaskStatGetHandlerLo (void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR          cpu_sr;
#endif
    NET_DBG_STATUS  status;
    NET_TMR_TICK    timeout_tick;
    NET_ERR         err;


    CPU_CRITICAL_ENTER();
    status = NetDbg_MonTaskStatusRsrcLo;                                /* Get mon task rsrc lo status.                 */
    CPU_CRITICAL_EXIT();

    if (NetDbg_MonTaskTmr == (NET_TMR *)0) {                            /* If mon task tmr NULL, get tmr (see Note #3a).*/
        CPU_CRITICAL_ENTER();
        timeout_tick       =  NetDbg_MonTaskTime_tick;
        CPU_CRITICAL_EXIT();
        NetDbg_MonTaskTmr  =  NetTmr_Get((void       *) NetDbg_MonTask,
                                         (CPU_FNCT_PTR) NetDbg_MonTask,
                                         (NET_TMR_TICK) timeout_tick,
                                         (CPU_INT16U  ) NET_TMR_FLAG_NONE,
                                         (NET_ERR    *)&err);           /* See Note #3b.                                */
    }

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetDbg_GetInfoHandlerConn()
*
* Description : (1) Get information on a network connection :
*
*                   (a) Network connection's configured family type
*                   (b) Network connection's configured protocol index
*                   (c) Network connection's current connection chain index in a connection list
*                   (d) Network connection's current connection       index in a connection list
*
*               (2) In the diagram below, ...
*
*                   (a) The top horizontal row represents the connection list's list of connection chains,
*                       where a network connection's current connection chain index in the connection list
*                       is determined by its relative position in the top row :
*
*                       (1) The left -most connection at the head of the list is index '0' & ...
*                       (2) The right-most connection at the tail of the list is index 'N-1'.
*
*                   (b) Each vertical column represents the connections in each connection chain, where a
*                       network connection's current connection index in the connection chain is determined
*                       by its relative position in the connection chain :
*
*                       (1) The connection at the top    of the chain is index '0' & ...
*                       (2) The connection at the bottom of the chain is index 'N-1'.
*
*
*                           Head of Connection List                                 Tail of Connection List
*                                 @ Index '0'                                            @ Index 'N-1'
*                               (see Note #2a1)                                         (see Note #2a2)
*                                      |                                                       |
*                                      |                                                       |
*                                      v                                                       v
*
*           Connection @  Top       -------       -------       -------       -------       -------
*          of Connection Chain ---> |     |------>|     |------>|     |------>|     |------>|     |
*              @ Index '0'          |     |       |     |       |     |       |     |       |     |
*            (see Note #2b1)        |     |<------|     |<------|     |<------|     |<------|     |
*                                   -------       -------       -------       -------       -------
*                                     | ^                                       | ^
*                                     | |                                       | |
*                                     v |                                       v |
*                                   -------                                   -------
*                                   |     |                                   |     |
*                                   |     |                                   |     |
*                                   |     |                                   |     |
*                                   -------                                   -------
*                                     | ^                                       | ^
*                                     | |                                       | |
*                                     v |                                       v |
*                                   -------                                   -------
*                                   |     |                                   |     |
*                                   |     |                                   |     |
*                                   |     |                                   |     |
*                                   -------                                   -------
*                                     | ^
*                                     | |
*                                     v |
*          Connection @ Bottom      -------
*          of Connection Chain ---> |     |
*             @ Index 'N-1'         |     |
*            (see Note #2b2)        |     |
*                                   -------
*
*$PAGE*
* Argument(s) : conn_id             Handle identifier of network connection.
*
*               pconn_family        Pointer to variable that will receive the network connection family type.
*
*                                       Connection family type,     if network connection valid & used.
*                                       NET_CONN_FAMILY_NONE,       otherwise.
*
*               pconn_protocol_ix   Pointer to variable that will receive the network connection protocol index :
*
*                                       Connection chain index,     if network connection valid & used.
*                                       NET_CONN_PROTOCOL_IX_NONE,  otherwise.
*
*               pconn_ix_chain      Pointer to variable that will receive the network connection chain index 
*                                       in its connection list (see Note #2a) :
*
*                                       Connection chain index,     if network connection found in its 
*                                                                      network connection list.
*                                       NET_CONN_LIST_IX_NONE,      otherwise.
*
*               pconn_ix            Pointer to variable that will receive the network connection       index 
*                                       in its connection list (see Note #2b) :
*
*                                       Connection       index,     if network connection found in its 
*                                                                      network connection list.
*                                       NET_CONN_LIST_IX_NONE,      otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_CONN_ERR_NONE               Network connection information successfully 
*                                                                   obtained; check return value(s) 
*
*                                                               ----- RETURNED BY NetConn_IsUsed() : ------
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_CONN_ERR_INVALID_CONN       Invalid network connection number.
*                               NET_CONN_ERR_NOT_USED           Network connection NOT currently used.
*
* Return(s)   : none.
*
* Caller(s)   : NetDbg_GetInfoConn().
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s) [see also Note #3].
*
* Note(s)     : (3) NetDbg_GetInfoHandlerConn() is called by network protocol suite function(s) & MUST 
*                   be called with the global network lock already acquired.
*
*                   See also 'NetDbg_GetInfoConn()  Note #1'.
*
*               (4) Pointers to variables that return values MUST be initialized to return PRIOR to all
*                   other validation or function handling in case of any error(s).
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_EN == DEF_ENABLED) && \
     (defined(NET_CONN_MODULE_PRESENT)))
void  NetDbg_GetInfoHandlerConn (NET_CONN_ID            conn_id,
                                 NET_CONN_FAMILY       *pconn_family,
                                 NET_CONN_PROTOCOL_IX  *pconn_protocol_ix,
                                 NET_CONN_LIST_IX      *pconn_ix_chain,
                                 NET_CONN_LIST_IX      *pconn_ix,
                                 NET_ERR               *perr)
{
    NET_CONN          *pconn;
    NET_CONN          *pconn_cur;
    NET_CONN          *pconn_next;
    NET_CONN          *pconn_chain_cur;
    NET_CONN          *pconn_chain_next;
    NET_CONN_LIST_IX   ix_chain;
    NET_CONN_LIST_IX   ix_conn;
    CPU_BOOLEAN        found;


/*$PAGE*/
                                                                        /* Init rtn vals for err (see Note #4).         */
    if (pconn_family      != (NET_CONN_FAMILY *)0) {
       *pconn_family       =  NET_CONN_FAMILY_NONE;
    }
    if (pconn_protocol_ix != (NET_CONN_PROTOCOL_IX *)0) {
       *pconn_protocol_ix  =  NET_CONN_PROTOCOL_IX_NONE;
    }

    if (pconn_ix_chain != (NET_CONN_LIST_IX *)0) {
       *pconn_ix_chain  =  NET_CONN_LIST_IX_NONE;
    }
    if (pconn_ix       != (NET_CONN_LIST_IX *)0) {
       *pconn_ix        =  NET_CONN_LIST_IX_NONE;
    }


                                                                        /* ---------- VALIDATE NET CONN USED ---------- */
   (void)NetConn_IsUsed(conn_id, perr);
    if (*perr != NET_CONN_ERR_NONE) {
         return;
    }


                                                                        /* ------------ SRCH NET CONN LIST ------------ */
    pconn           = &NetConn_Tbl[conn_id];
    pconn_chain_cur = *pconn->ConnList;
    found           =  DEF_NO;
    ix_chain        =  0;
    ix_conn         =  0;

    while ((pconn_chain_cur != (NET_CONN *)0) &&                        /* Srch ALL net conn chains ..                  */
           (found           ==  DEF_NO)) {                              /* .. until net conn found.                     */

        pconn_chain_next = pconn_chain_cur->NextChainPtr;
        pconn_cur        = pconn_chain_cur;
        ix_conn          = 0;

        while ((pconn_cur != (NET_CONN *)0) &&                          /* Srch ALL net conns in chain ..               */
               (found     ==  DEF_NO)) {                                /* .. until net conn found.                     */

            pconn_next = pconn_cur->NextConnPtr;

            if (pconn_cur == pconn) {
                found = DEF_YES;
            } else {                                                    /* If NOT found, ...                            */                                      
                pconn_cur = pconn_next;                                 /* ... adv to next conn.                        */
                ix_conn++;
            }
        }

        if (found != DEF_YES) {                                         /* If NOT found, ...                            */
            pconn_chain_cur = pconn_chain_next;                         /* ... adv to next conn chain.                  */
            ix_chain++;
        }
    }

                                                                        /* Rtn net conn's family & protocol ix.         */
    if (pconn_family      != (NET_CONN_FAMILY *)0) {
       *pconn_family       =  pconn->Family;
    }
    if (pconn_protocol_ix != (NET_CONN_PROTOCOL_IX *)0) {
       *pconn_protocol_ix  =  pconn->ProtocolIx;
    }

    if (found == DEF_YES) {                                             /* If net conn found in conn list, ...          */
                                                                        /* ... rtn net conn's list ix's (see Note #2).  */
        if (pconn_ix_chain != (NET_CONN_LIST_IX *)0) {
           *pconn_ix_chain  =  ix_chain;
        }
        if (pconn_ix       != (NET_CONN_LIST_IX *)0) {
           *pconn_ix        =  ix_conn;
        }
    }


   *perr = NET_CONN_ERR_NONE;
}
#endif


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
*                                       NetDbg_InitInfoConst()
*
* Description : (1) Initialize debug information constants :
*
*                   (a) Prevent possible compiler optimization removal of debug information constants.
*
*                       (1) Some compilers ALWAYS optimize & remove unused local debug information constants.
*
*                       (2) Some compilers MAY    optimize & remove unused local debug information constants
*                           if compiler optimization settings are maximally configured or configured greater
*                           than some   optimization setting threshold.
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetDbg_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetDbg_InitInfoConst (void)
{
    volatile  void  *pconst;                                                        /* See Note #1a.                    */


    pconst = (void *)&Net_Version;


#if (NET_DBG_CFG_INFO_EN == DEF_ENABLED)
                                                                                    /* ---------- NET MODULE ---------- */
    pconst = (void *)&Net_DataSize;



                                                                                    /* ------ NET MISC MODULE(S) ------ */
    pconst = (void *)&Net_CfgInitCfgVals;
    pconst = (void *)&Net_CfgOptimize;
    pconst = (void *)&Net_CfgTxSuspendTimeout_ms;

    pconst = (void *)&NetTS_Size;



                                                                                    /* ------- NET ERR  MODULE -------- */
    pconst = (void *)&NetErr_CfgArgChkExtEn;
    pconst = (void *)&NetErr_CfgArgChkDbgEn;

    pconst = (void *)&NetErr_Size;



                                                                                    /* ------- NET DBG  MODULE -------- */
    pconst = (void *)&NetDbg_CfgInfoEn;
    pconst = (void *)&NetDbg_CfgStatusEn;
    pconst = (void *)&NetDbg_CfgStatusRsrcLostEn;
    pconst = (void *)&NetDbg_CfgStatusRsrcLoEn;
    pconst = (void *)&NetDbg_CfgMemClrEn;
    pconst = (void *)&NetDbg_CfgTestEn;
    pconst = (void *)&NetDbg_CfgMonTaskEn;

    pconst = (void *)&NetDbg_StatusSize;

    pconst = (void *)&NetDbg_DataSize;



/*$PAGE*/
                                                                                    /* ------- NET CTR  MODULE -------- */
    pconst = (void *)&NetCtr_CfgStatEn;
    pconst = (void *)&NetCtr_CfgErrEn;

    pconst = (void *)&NetCtr_Size;

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
    pconst = (void *)&NetCtr_StatCtrsSize;
#endif
#if (NET_CTR_CFG_ERR_EN  == DEF_ENABLED)
    pconst = (void *)&NetCtr_ErrCtrsSize;
#endif

    pconst = (void *)&NetCtr_DataSize;



                                                                                    /* ------- NET STAT MODULE -------- */
    pconst = (void *)&NetStat_PoolSize;

    pconst = (void *)&NetStat_DataSize;



                                                                                    /* ------- NET TMR  MODULE -------- */
    pconst = (void *)&NetTmr_CfgNbrTmr;
    pconst = (void *)&NetTmr_CfgTaskFreq;

    pconst = (void *)&NetTmr_TickSize;
    pconst = (void *)&NetTmr_Size;
    pconst = (void *)&NetTmr_TblSize;

    pconst = (void *)&NetTmr_DataSize;



                                                                                    /* ------- NET BUF  MODULE -------- */
    pconst = (void *)&NetBuf_HdrSize;

    pconst = (void *)&NetBuf_CfgNbrSmall;
#if (NET_BUF_CFG_NBR_SMALL > 0)
    pconst = (void *)&NetBuf_CfgDataSizeSmall;

    pconst = (void *)&NetBuf_SmallSize;
    pconst = (void *)&NetBuf_SmallTblSize;
#endif

    pconst = (void *)&NetBuf_CfgNbrLarge;
#if (NET_BUF_CFG_NBR_LARGE > 0)
    pconst = (void *)&NetBuf_CfgDataSizeLarge;

    pconst = (void *)&NetBuf_LargeSize;
    pconst = (void *)&NetBuf_LargeTblSize;
#endif

    pconst = (void *)&NetBuf_DataSize;




#ifdef  NET_CONN_MODULE_PRESENT
                                                                                    /* ------- NET CONN MODULE -------- */
    pconst = (void *)&NetConn_CfgNbrConn;

    pconst = (void *)&NetConn_ID_Size;
    pconst = (void *)&NetConn_AddrLenSize;
    pconst = (void *)&NetConn_Size;
    pconst = (void *)&NetConn_TblSize;
    pconst = (void *)&NetConn_ConnListTblSize;
    pconst = (void *)&NetConn_AddrWildCardSize;

    pconst = (void *)&NetConn_DataSize;
#endif




/*$PAGE*/
#ifdef  NET_ARP_MODULE_PRESENT
                                                                                    /* ------- NET ARP  MODULE -------- */
    pconst = (void *)&NetARP_CfgNbrCache;
    pconst = (void *)&NetARP_CfgAddrFilterEn;

    pconst = (void *)&NetARP_HdrSize;
    pconst = (void *)&NetARP_CacheSize;
    pconst = (void *)&NetARP_CacheTblSize;

    pconst = (void *)&NetARP_DataSize;
#endif



                                                                                    /* ------- NET IP   MODULE -------- */
    pconst = (void *)&NetIP_HdrSize;
    pconst = (void *)&NetIP_TOS_Size;
    pconst = (void *)&NetIP_TTL_Size;
    pconst = (void *)&NetIP_AddrSize;
    pconst = (void *)&NetIP_OptSize;
    pconst = (void *)&NetIP_OptSrcRouteSize;
    pconst = (void *)&NetIP_OptTS_RouteSize;
    pconst = (void *)&NetIP_OptTS_Size;

    pconst = (void *)&NetIP_DataSize;



                                                                                    /* ------- NET ICMP MODULE -------- */
    pconst = (void *)&NetICMP_HdrSize;
    pconst = (void *)&NetICMP_HdrErrSize;
    pconst = (void *)&NetICMP_HdrParamProbSize;
    pconst = (void *)&NetICMP_HdrEchoSize;
    pconst = (void *)&NetICMP_HdrTS_Size;
    pconst = (void *)&NetICMP_HdrAddrMaskSize;

    pconst = (void *)&NetICMP_CfgTxSrcQuenchEn;
#if (NET_ICMP_CFG_TX_SRC_QUENCH_EN == DEF_ENABLED)
    pconst = (void *)&NetICMP_TxSrcQuenchSize;
    pconst = (void *)&NetICMP_TxSrcQuenchTblSize;

    pconst = (void *)&NetICMP_CfgTxSrcQuenchSize;
#endif

    pconst = (void *)&NetICMP_DataSize;



                                                                                    /* ------- NET UDP  MODULE -------- */
    pconst = (void *)&NetUDP_CfgAppAPI_Sel;
    pconst = (void *)&NetUDP_CfgRxChkSumDiscardEn;
    pconst = (void *)&NetUDP_CfgTxChkSumEn;

    pconst = (void *)&NetUDP_HdrSize;
    pconst = (void *)&NetUDP_PseudoHdrSize;
    pconst = (void *)&NetUDP_PortNbrSize;

    pconst = (void *)&NetUDP_DataSize;



#ifdef  NET_TCP_MODULE_PRESENT
                                                                                    /* ------- NET TCP  MODULE -------- */
    pconst = (void *)&NetTCP_CfgNbrConn;
    pconst = (void *)&NetTCP_CfgRxWinSize_octet;
    pconst = (void *)&NetTCP_CfgTxWinSize_octet;
    pconst = (void *)&NetTCP_CfgTimeoutConnMaxSeg_sec;
    pconst = (void *)&NetTCP_CfgTimeoutConnAckDly_ms;
    pconst = (void *)&NetTCP_CfgTimeoutConnRxQ_ms;
    pconst = (void *)&NetTCP_CfgTimeoutConnTxQ_ms;

    pconst = (void *)&NetTCP_HdrSize;
    pconst = (void *)&NetTCP_PseudoHdrSize;
    pconst = (void *)&NetTCP_PortNbrSize;
    pconst = (void *)&NetTCP_SeqNbrSize;
    pconst = (void *)&NetTCP_SegLenSize;
    pconst = (void *)&NetTCP_WinSize;
    pconst = (void *)&NetTCP_OptSize;
    pconst = (void *)&NetTCP_ConnID_Size;
    pconst = (void *)&NetTCP_ConnSize;
    pconst = (void *)&NetTCP_ConnTblSize;

    pconst = (void *)&NetTCP_DataSize;
#endif



/*$PAGE*/
#ifdef  NET_SOCK_MODULE_PRESENT
                                                                                    /* ------- NET SOCK MODULE -------- */
    pconst = (void *)&NetSock_CfgNbrSock;
    pconst = (void *)&NetSock_CfgBlockSel;
    pconst = (void *)&NetSock_CfgRandomPortNbrBase;
    pconst = (void *)&NetSock_NbrRandomPortNbr;
    pconst = (void *)&NetSock_CfgTimeoutRxQ_ms;

    pconst = (void *)&NetSock_CfgStreamTypeEn;
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
    pconst = (void *)&NetSock_CfgConnAcceptQ_MaxSize;
    pconst = (void *)&NetSock_CfgTimeoutConnReq_ms;
    pconst = (void *)&NetSock_CfgTimeoutConnAccept_ms;
    pconst = (void *)&NetSock_CfgTimeoutConnClose_ms;
#endif

    pconst = (void *)&NetSock_ID_Size;
    pconst = (void *)&NetSock_RtnCodeSize;
    pconst = (void *)&NetSock_AddrLenSize;
    pconst = (void *)&NetSock_SockAddrSize;
    pconst = (void *)&NetSock_Size;
    pconst = (void *)&NetSock_TblSize;
    pconst = (void *)&NetSock_Q_LenSize;
    pconst = (void *)&NetSock_RandomPortNbrQ_Size;

    pconst = (void *)&NetSock_DataSize;
#endif



                                                                                    /* ------- NET BSD  MODULE -------- */
#if    (NET_BSD_CFG_API_EN  == DEF_ENABLED)
#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
    pconst = (void *)&NetBSD_IP_to_Str_ArraySize;
#endif
#endif
#endif

    pconst = (void *)&NetBSD_DataSize;



                                                                                    /* ------- NET OS   MODULE -------- */
    pconst = (void *)&NetOS_TimeoutMin_tick;
    pconst = (void *)&NetOS_TimeoutMax_tick;
    pconst = (void *)&NetOS_TimeoutMin_sec;
    pconst = (void *)&NetOS_TimeoutMax_sec;
    pconst = (void *)&NetOS_TimeoutMin_ms;
    pconst = (void *)&NetOS_TimeoutMax_ms;



                                                                                    /* ----- NET TOTAL DATA SIZE ------ */
    pconst = (void *)&Net_DataSizeTot;

#endif



   (void)&pconst;                                                                   /* Prevent compiler warning.        */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetDbg_ChkStatusHandlerTCP_Q()
*
* Description : (1) Check TCP connection queue for faults :
*
*                   (a) TCP connection queue buffer links to unused buffers
*                   (b) TCP connection queue duplicate buffer references
*
*
* Argument(s) : pconn_chk       Pointer to a TCP connection.
*
*               pbuf_chk_q      Pointer to a TCP connection queue buffer.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO TCP connection queue faults.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_TCP_CONN_Q                       Some TCP connection queue fault(s).
*
*                   NET_DBG_SF_TCP_CONN_Q_BUF_TYPE              TCP connection queue buffer invalid type.     
*                   NET_DBG_SF_TCP_CONN_Q_BUF_UNUSED            TCP connection queue buffer         unused.     
*                   NET_DBG_SF_TCP_CONN_Q_LINK_TYPE             TCP connection queue buffer invalid link type.
*                   NET_DBG_SF_TCP_CONN_Q_LINK_UNUSED           TCP connection queue buffer         link unused.
*
*                                                               - RETURNED BY NetDbg_ChkStatusHandlerTCP_Q_Dup() : -
*                   NET_DBG_SF_TCP_CONN_Q_BUF_DUP               TCP connection queue contains duplicate buffer(s).
*
* Caller(s)   : NetDbg_ChkStatusHandlerTCP().
*
* Note(s)     : (2) #### Link pointers NOT yet validated to be aligned on CPU pointer-aligned addresses
*                   which MAY lead to unaligned pointer access & generate CPU pointer-alignment exceptions.
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_EN == DEF_ENABLED) && \
     (defined(NET_TCP_MODULE_PRESENT)))
static  NET_DBG_STATUS  NetDbg_ChkStatusHandlerTCP_Q (NET_TCP_CONN  *pconn_chk,
                                                      NET_BUF       *pbuf_chk_q)
{
    NET_DBG_STATUS   status;
    NET_DBG_STATUS   status_dup;
    CPU_BOOLEAN      err_buf;
    CPU_BOOLEAN      err_buf_chain;
    CPU_BOOLEAN      used;
    NET_BUF         *pbuf_chk;
    NET_BUF         *pbuf_chk_chain;
    NET_BUF         *pbuf_link;


    status = NET_DBG_STATUS_OK;


/*$PAGE*/
    pbuf_chk_chain = pbuf_chk_q;                                        /* -------------- CHK TCP CONN Q -------------- */
    while (pbuf_chk_chain != (NET_BUF *)0) {
        pbuf_chk = pbuf_chk_chain;
        while (pbuf_chk   != (NET_BUF *)0) {                            /* Chk every buf in every buf chain.            */
            switch (pbuf_chk->Hdr.Type) {
                case NET_BUF_TYPE_SMALL:
                case NET_BUF_TYPE_LARGE:
                     err_buf = DEF_NO;
                     break;


                case NET_BUF_TYPE_NONE:                                 /* If buf type invalid, set err.                */
                default:
                     DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_BUF_TYPE);
                     err_buf = DEF_YES;
                     break;
            }

            if (err_buf == DEF_NO) {
                used = DEF_BIT_IS_SET(pbuf_chk->Hdr.Flags, NET_BUF_FLAG_USED);
                if (used != DEF_YES) {                                  /* If TCP conn Q buf unused, set err.           */
                     DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_BUF_UNUSED);
                     err_buf = DEF_YES;

                } else {
                                                                        /* --------- CHK TCP CONN Q BUF LINKS --------- */
                                                                        /* Chk Q buf's prev prim list buf.              */
                    pbuf_link = pbuf_chk->Hdr.PrevPrimListPtr;
                    if (pbuf_link != (NET_BUF *)0) {
                        switch (pbuf_link->Hdr.Type) {
                            case NET_BUF_TYPE_SMALL:
                            case NET_BUF_TYPE_LARGE:
                                 break;


                            case NET_BUF_TYPE_NONE:                     /* If buf type invalid, set err.                */
                            default:
                                 DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_TYPE);
                                 break;
                        }

                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used, set err.                        */
                            DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_UNUSED);
                        }
                    }

                                                                        /* Chk Q buf's next prim list buf.              */
                    pbuf_link = pbuf_chk->Hdr.NextPrimListPtr;
                    if (pbuf_link != (NET_BUF *)0) {
                        switch (pbuf_link->Hdr.Type) {
                            case NET_BUF_TYPE_SMALL:
                            case NET_BUF_TYPE_LARGE:
                                 break;


                            case NET_BUF_TYPE_NONE:                     /* If buf type invalid, set err.                */
                            default:
                                 DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_TYPE);
                                 break;
                        }

                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used, set err.                        */
                            DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_UNUSED);
                        }
                    }
/*$PAGE*/
                                                                        /* Chk Q buf's prev sec  list buf.              */
                    pbuf_link = pbuf_chk->Hdr.PrevSecListPtr;
                    if (pbuf_link != (NET_BUF *)0) {
                        switch (pbuf_link->Hdr.Type) {
                            case NET_BUF_TYPE_SMALL:
                            case NET_BUF_TYPE_LARGE:
                                 break;


                            case NET_BUF_TYPE_NONE:                     /* If buf type invalid, set err.                */
                            default:
                                 DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_TYPE);
                                 break;
                        }

                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used, set err.                        */
                            DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_UNUSED);
                        }
                    }

                                                                        /* Chk Q buf's next sec  list buf.              */
                    pbuf_link = pbuf_chk->Hdr.NextSecListPtr;
                    if (pbuf_link != (NET_BUF *)0) {
                        switch (pbuf_link->Hdr.Type) {
                            case NET_BUF_TYPE_SMALL:
                            case NET_BUF_TYPE_LARGE:
                                 break;


                            case NET_BUF_TYPE_NONE:                     /* If buf type invalid, set err.                */
                            default:
                                 DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_TYPE);
                                 break;
                        }

                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used, set err.                        */
                            DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_UNUSED);
                        }
                    }

                                                                        /* Chk Q buf's prev           buf.              */
                    pbuf_link = pbuf_chk->Hdr.PrevBufPtr;
                    if (pbuf_link != (NET_BUF *)0) {
                        switch (pbuf_link->Hdr.Type) {
                            case NET_BUF_TYPE_SMALL:
                            case NET_BUF_TYPE_LARGE:
                                 break;


                            case NET_BUF_TYPE_NONE:                     /* If buf type invalid, set err.                */
                            default:
                                 DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_TYPE);
                                 break;
                        }

                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used, set err.                        */
                            DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_UNUSED);
                        }
                    }

                                                                        /* Chk Q buf's next           buf.              */
                    pbuf_link = pbuf_chk->Hdr.NextBufPtr;
                    if (pbuf_link != (NET_BUF *)0) {
                        switch (pbuf_link->Hdr.Type) {
                            case NET_BUF_TYPE_SMALL:
                            case NET_BUF_TYPE_LARGE:
                                 break;


                            case NET_BUF_TYPE_NONE:                     /* If buf type invalid, set err.                */
                            default:
                                 DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_TYPE);
                                 break;
                        }

                        used = DEF_BIT_IS_SET(pbuf_link->Hdr.Flags, NET_BUF_FLAG_USED);
                        if (used != DEF_YES) {                          /* If NOT used, set err.                        */
                            DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_LINK_UNUSED);
                        }
                    }
/*$PAGE*/
                                                                        /* --------- CHK TCP CONN Q DUP BUF's --------- */
                                                                        /* Chk dup buf(s) in TCP conn transport rx Q.   */
                    if (pbuf_chk_q != pconn_chk->RxQ_Transport_Head) {
                        status_dup  = NetDbg_ChkStatusHandlerTCP_Q_Dup(pbuf_chk, pconn_chk->RxQ_Transport_Head);
                        DEF_BIT_SET(status, status_dup);
                    }
                                                                        /* Chk dup buf(s) in TCP conn app       rx Q.   */
                    if (pbuf_chk_q != pconn_chk->RxQ_App_Head) {
                        status_dup  = NetDbg_ChkStatusHandlerTCP_Q_Dup(pbuf_chk, pconn_chk->RxQ_App_Head);
                        DEF_BIT_SET(status, status_dup);
                    }
                                                                        /* Chk dup buf(s) in TCP conn    tx Q.          */
                    if (pbuf_chk_q != pconn_chk->TxQ_Head) {
                        status_dup  = NetDbg_ChkStatusHandlerTCP_Q_Dup(pbuf_chk, pconn_chk->TxQ_Head);
                        DEF_BIT_SET(status, status_dup);
                    }
                                                                        /* Chk dup buf(s) in TCP conn re-tx Q.          */
                    if (pbuf_chk_q != pconn_chk->ReTxQ_Head) {
                        status_dup  = NetDbg_ChkStatusHandlerTCP_Q_Dup(pbuf_chk, pconn_chk->ReTxQ_Head);
                        DEF_BIT_SET(status, status_dup);
                    }
                }
            }


            if ((err_buf  ==  DEF_NO) &&                                            /* If NO buf err &               .. */
                (pbuf_chk != (NET_BUF *)pbuf_chk->Hdr.NextBufPtr)) {                /* .. buf does NOT point to itself, */
                 pbuf_chk  = (NET_BUF *)pbuf_chk->Hdr.NextBufPtr;                   /* .. adv to next buf.              */
            } else {                                                                /* Else exit chain.                 */
                 pbuf_chk  = (NET_BUF *)0;
            }
        }


        err_buf_chain = ((err_buf != DEF_NO) && (pbuf_chk == pbuf_chk_chain)) ? DEF_YES : DEF_NO;

        if ((err_buf_chain  ==  DEF_NO) &&                                          /* If NO buf chain err &         .. */
            (pbuf_chk_chain != (NET_BUF *)pbuf_chk_chain->Hdr.NextPrimListPtr)) {   /* .. buf does NOT point to itself, */
             pbuf_chk_chain  = (NET_BUF *)pbuf_chk_chain->Hdr.NextPrimListPtr;      /* .. adv to next chain.            */
        } else {                                                                    /* Else exit q.                     */
             pbuf_chk_chain  = (NET_BUF *)0;
        }
    }



    if (status != NET_DBG_STATUS_OK) {                                  /* For ANY fault status, ...                    */
        DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q);                     /* ... set breakpoint.                          */
    }


    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetDbg_ChkStatusHandlerTCP_Q_Dup()
*
* Description : Check TCP connection queue for duplicate buffer reference(s).
*
* Argument(s) : pbuf_chk    Pointer to a TCP connection queue buffer.
*
*               pbuf_q      Pointer to a TCP connection queue to check for duplicate reference(s) to the
*                               buffer.
*
* Return(s)   : NET_DBG_STATUS_OK, if NO duplicate buffer reference(s) in the TCP connection queue.
*
*               Otherwise,         return network status codes logically OR'd :
*
*                   NET_DBG_SF_TCP_CONN_Q_BUF_DUP               TCP connection queue contains duplicate buffer(s).
*
* Caller(s)   : NetDbg_ChkStatusHandlerTCP_Q().
*
* Note(s)     : (1) Assumes TCP connection queues' network buffer types have already been validated.
*
*               (2) #### Link pointers are NOT yet validated to be aligned on CPU pointer-aligned addresses.
*********************************************************************************************************
*/

#if ((NET_DBG_CFG_STATUS_EN == DEF_ENABLED) && \
     (defined(NET_TCP_MODULE_PRESENT)))
static  NET_DBG_STATUS  NetDbg_ChkStatusHandlerTCP_Q_Dup (NET_BUF  *pbuf_chk,
                                                          NET_BUF  *pbuf_q)
{
    NET_DBG_STATUS   status;
    NET_BUF         *pbuf_chain;
    NET_BUF         *pbuf;


    status = NET_DBG_STATUS_OK;


    pbuf_chain = pbuf_q;
    while (pbuf_chain != (NET_BUF *)0) {
        pbuf = pbuf_chain;
        while (pbuf != (NET_BUF *)0) {                                  /* Chk every buf in every buf chain.            */
            if (pbuf_chk == pbuf) {                                     /* If dup buf found, set err.                   */
                DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_BUF_DUP);
            }

            if (pbuf != (NET_BUF *)pbuf->Hdr.NextBufPtr) {              /* If buf does NOT point to itself, ...         */
                pbuf  = (NET_BUF *)pbuf->Hdr.NextBufPtr;                /* ... adv to next buf.                         */
            } else {                                                    /* Else exit chain.                             */
                pbuf  = (NET_BUF *)0;
            }
        }

        if (pbuf_chain != (NET_BUF *)pbuf_chain->Hdr.NextPrimListPtr) { /* If buf does NOT point to itself, ...         */
            pbuf_chain  = (NET_BUF *)pbuf_chain->Hdr.NextPrimListPtr;   /* ... adv to next chain.                       */
        } else {                                                        /* Else exit q.                                 */
            pbuf_chain  = (NET_BUF *)0;
        }
    }


    if (status != NET_DBG_STATUS_OK) {                                  /* For ANY fault status, ...                    */
        DEF_BIT_SET(status, NET_DBG_SF_TCP_CONN_Q_BUF_DUP);             /* ... set breakpoint.                          */
    }

    return (status);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetDbg_MonTask()
*
* Description : (1) Monitor network resources' status :
*
*                   (a) Check & update network resources' usage status :
*                       (1) Check lost resources
*                       (2) Check low  resources
*
*                   (b) Get Debug Monitor Task Handler timer                     See Note #4
*
*
* Argument(s) : ptask       Pointer to this Debug Monitor Task Handler function (see Note #2).
*
* Return(s)   : none.
*
* Caller(s)   : Referenced in NetDbg_Init(),
*                             NetDbg_MonTask(),
*                             NetDbg_MonTaskStatGetHandlerLost(),
*                             NetDbg_MonTaskStatGetHandlerLo().
*
* Note(s)     : (2) Network timer module requires a pointer to an object when allocating a timer.
*                   Since Debug Monitor Task does NOT require a pointer to an object in order to
*                   execute, a pointer to this task handler function is passed instead.
*
*                   See also 'net_tmr.c  NetTmr_Get()  Note #3'.
*
*               (3) This function is a network timer callback function :
*
*                   (a) Clear the timer pointer :
*                       (1) Reset by NetTmr_Get().                              See Note #4
*
*                   (b) but do NOT re-free the timer.
*
*               (4) Do NOT need to verify success of Timer Get since every status get will check
*                   & get a Debug Monitor Task Handler timer as needed.
*
*                   See also 'NetDbg_MonTaskStatGetHandlerLost()  Note #3'
*                          & 'NetDbg_MonTaskStatGetHandlerLo()    Note #3'.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_MON_TASK_EN == DEF_ENABLED)
static  void  NetDbg_MonTask (void  *ptask)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR          cpu_sr;
#endif
    NET_DBG_STATUS  status;
    NET_TMR_TICK    timeout_tick;
    NET_ERR         err;


   (void)&ptask;                                                        /* Prevent compiler warning (see Note #2).      */

                                                                        /* -------------- MON NET RSRC's -------------- */
    status = NetDbg_ChkStatusHandlerRsrcLost();
    CPU_CRITICAL_ENTER();
    NetDbg_MonTaskStatusRsrcLost = status;
    CPU_CRITICAL_EXIT();

    status = NetDbg_ChkStatusHandlerRsrcLo();
    CPU_CRITICAL_ENTER();
    NetDbg_MonTaskStatusRsrcLo   = status;
    CPU_CRITICAL_EXIT();


                                                                        /* ------------- GET MON TASK TMR ------------- */
    CPU_CRITICAL_ENTER();
    timeout_tick      = NetDbg_MonTaskTime_tick;
    CPU_CRITICAL_EXIT();
    NetDbg_MonTaskTmr = NetTmr_Get((void       *) NetDbg_MonTask,       /* See Note #2.                                 */
                                   (CPU_FNCT_PTR) NetDbg_MonTask,
                                   (NET_TMR_TICK) timeout_tick,
                                   (CPU_INT16U  ) NET_TMR_FLAG_NONE,
                                   (NET_ERR    *)&err);                 /* See Note #4.                                 */
}
#endif

