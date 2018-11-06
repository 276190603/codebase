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
*                                     NETWORK STATISTICS MANAGEMENT
*
* Filename      : net_stat.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_STAT_MODULE
#define  NET_STAT_EXT
#else
#define  NET_STAT_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                NETWORK STATISTICS POOL TYPE DEFINES
*
* Note(s) : (1) NET_STAT_TYPE_??? #define values specifically chosen as ASCII representations of the network
*               statistics pool types.  Memory displays of statistics pools will display the statistics pool 
*               TYPEs with their chosen ASCII names.
*********************************************************************************************************
*/

                                                                /* ---------------- NET STAT POOL TYPES --------------- */
#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  NET_STAT_TYPE_NONE                       0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_STAT_TYPE_POOL                       0x504F4F4C    /* "POOL" in ASCII.                                     */

#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)
#define  NET_STAT_TYPE_NONE                       0x454E4F4E    /* "NONE" in ASCII.                                     */
#define  NET_STAT_TYPE_POOL                       0x4C4F4F50    /* "POOL" in ASCII.                                     */

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  NET_STAT_TYPE_NONE                       0x4F4E454E    /* "NONE" in ASCII.                                     */
#define  NET_STAT_TYPE_POOL                       0x4F504C4F    /* "POOL" in ASCII.                                     */

#else                                                           /* Dflt CPU_WORD_SIZE_08.                               */
#define  NET_STAT_TYPE_NONE                       0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_STAT_TYPE_POOL                       0x504F4F4C    /* "POOL" in ASCII.                                     */

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
*                              NETWORK STATISTICS POOL QUANTITY DATA TYPE
*
* Note(s) : (1) Statistics pool quantity data type MUST be configured with an appropriate-sized network 
*               data type large enough to perform calculations on the following data types :
*
*               (a) NET_BUF_QTY
*               (b) NET_TMR_QTY
*               (c) NET_CONN_QTY
*               (d) NET_CONN_LIST_QTY
*               (e) NET_ARP_CACHE_QTY
*               (f) NET_ICMP_SRC_QUENCH_QTY
*               (g) NET_TCP_CONN_QTY
*               (h) NET_SOCK_QTY
*
*           (2) NET_STAT_POOL_NBR_MAX  SHOULD be #define'd based on 'NET_STAT_POOL_QTY' data type declared.
*********************************************************************************************************
*/

typedef  CPU_INT16U  NET_STAT_POOL_QTY;                         /* Defines max qty of stat pool entries to support.     */

#define  NET_STAT_POOL_NBR_MIN                             1
#define  NET_STAT_POOL_NBR_MAX           DEF_INT_16U_MAX_VAL    /* See Note #2.                                         */

#define  NET_STAT_POOL_MIN_VAL                             0
#define  NET_STAT_POOL_MAX_VAL    (NET_STAT_POOL_NBR_MAX - 1)


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NETWORK STATISTICS POOL DATA TYPE
*
* Note(s) : (1) Forced word-alignment at start of statistics pool NOT required since first data member
*               'Type' is declared as 'CPU_INT32U'.
*
*           (2) Pool statistic entries MUST ALWAYS be accessed exclusively in critical sections.
*
*           (3) (a) 'EntriesInit'/'EntriesTot' indicate the initial/current total number of entries in 
*                    the statistics pool.
*
*               (b) 'EntriesAvail'/'EntriesUsed' track the current number of entries available/used for 
*                    the statistics pool.
*
*               (c) 'EntriesLostCur'/'EntriesLostTot' track the current/total number of unrecoverable 
*                    invalid/corrupted entries lost from the statistics pool.  Lost entries MUST be 
*                    determined by the owner of the statistics pool.
*
*               (d) 'EntriesAllocatedCtr'/'EntriesDeallocatedCtr' tracks the current number of allocated/
*                    deallocated entries in the statistics pool.
*
*           (4) Assuming statistics pool are always accessed in critical sections (see Note #2), the 
*               following equations for statistics pools are true at all times :
*
*                       EntriesInit  = EntriesTot   + EntriesLostTot
*
*                       EntriesTot   = EntriesAvail + EntriesUsed
*
*                       EntriesUsed  = EntriesAllocatedCtr - EntriesDeallocatedCtr - EntriesLostCur
*********************************************************************************************************
*/

                                                                /* ------------------- NET POOL STAT ------------------ */
typedef  struct  net_stat_pool {
    NET_TYPE            Type;                                   /* Type cfg'd @ init : NET_STAT_TYPE_POOL.              */

    NET_STAT_POOL_QTY   EntriesInit;                            /* Init  nbr entries      in   pool.                    */
    NET_STAT_POOL_QTY   EntriesTot;                             /* Tot   nbr entries      in   pool.                    */
    NET_STAT_POOL_QTY   EntriesAvail;                           /* Avail nbr entries      in   pool.                    */
    NET_STAT_POOL_QTY   EntriesUsed;                            /* Used  nbr entries      from pool.                    */
    NET_STAT_POOL_QTY   EntriesUsedMax;                         /* Max   nbr entries used from pool.                    */
    NET_STAT_POOL_QTY   EntriesLostCur;                         /* Cur   nbr entries lost from pool.                    */
    NET_STAT_POOL_QTY   EntriesLostTot;                         /* Tot   nbr entries lost from pool.                    */

    CPU_INT32U          EntriesAllocatedCtr;                    /* Tot   nbr entries successfully  allocated.           */
    CPU_INT32U          EntriesDeallocatedCtr;                  /* Tot   nbr entries successfully deallocated.          */
} NET_STAT_POOL;


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  NetStat_Init            (void);


                                                                            /* --------- STAT POOL API FNCTS ---------- */
void  NetStat_PoolInit        (NET_STAT_POOL      *pstat_pool,
                               NET_STAT_POOL_QTY   nbr_avail,
                               NET_ERR            *perr);

void  NetStat_PoolClr         (NET_STAT_POOL      *pstat_pool,
                               NET_ERR            *perr);


void  NetStat_PoolReset       (NET_STAT_POOL      *pstat_pool,
                               NET_ERR            *perr);

void  NetStat_PoolResetUsedMax(NET_STAT_POOL      *pstat_pool,
                               NET_ERR            *perr);


void  NetStat_PoolEntryUsedInc(NET_STAT_POOL      *pstat_pool,
                               NET_ERR            *perr);

void  NetStat_PoolEntryUsedDec(NET_STAT_POOL      *pstat_pool,
                               NET_ERR            *perr);

void  NetStat_PoolEntryLostInc(NET_STAT_POOL      *pstat_pool,
                               NET_ERR            *perr);


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

