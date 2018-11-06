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
*                                      NETWORK BUFFER MANAGEMENT
*
* Filename      : net_buf.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_BUF_MODULE
#include  <net.h>


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

#if (NET_BUF_CFG_NBR_SMALL > 0)
static  void  NetBuf_InitSmall(void);
#endif

#if (NET_BUF_CFG_NBR_LARGE > 0)
static  void  NetBuf_InitLarge(void);
#endif


static  void  NetBuf_ClrHdr   (NET_BUF_HDR  *pbuf_hdr);


static  void  NetBuf_Discard  (NET_BUF      *pbuf,
                               NET_TYPE      type);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetBuf_Init()
*
* Description : (1) Initialize Network Buffer Management Module :
*
*                   (a) Initialize network buffers
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
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetBuf_Init (void)
{
                                                                /* -------------------- INIT BUFs --------------------- */
#if (NET_BUF_CFG_NBR_SMALL > 0)
    NetBuf_InitSmall();                                         /* Init SMALL bufs.                                     */
#endif

#if (NET_BUF_CFG_NBR_LARGE > 0)
    NetBuf_InitLarge();                                         /* Init LARGE bufs.                                     */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetBuf_GetMaxSize()
*
* Description : Get maximum possible buffer allocation size starting at a specific buffer index.
*
* Argument(s) : pbuf        Pointer to a network buffer.
*
*               ix_start    Requested buffer index to store buffer data.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) The 'LARGE BUF MAX SIZE' pre-processor conditional does NOT need to verify SMALL
*                   buffer configuration since 'net_buf.h' ensures that NET_BUF_CFG_DATA_SIZE_LARGE
*                   MUST be configured with a value greater than NET_BUF_CFG_DATA_SIZE_SMALL (see
*                   'net_buf.h  CONFIGURATION ERRORS').
*
*               (2) The 'NO BUFS' pre-processor 'else'-conditional code will never be compiled/linked 
*                   since 'net_buf.h' ensures that at least one of the two configuration constants 
*                   (NET_BUF_CFG_NBR_SMALL or NET_BUF_CFG_NBR_LARGE) will be configured with a value 
*                   greater than zero (see 'net_buf.h  CONFIGURATION ERRORS').  The 'else'-conditional 
*                   code is included for completeness & as an extra precaution in case 'net_buf.h' is
*                   incorrectly modified.
*
*               (3) Although network buffers' 'Data' buffer declared with an additional CPU word size
*                   (see 'net_buf.h  NETWORK BUFFER DATA TYPE  Note #4b'), this additional CPU word 
*                   size does NOT increase the overall useable network buffer 'Data' size.
*********************************************************************************************************
*/
/*$PAGE*/
NET_BUF_SIZE  NetBuf_GetMaxSize (NET_BUF       *pbuf,
                                 NET_BUF_SIZE   ix_start)
{
    NET_BUF_HDR   *pbuf_hdr;
    NET_BUF_SIZE   max_size;


    if (pbuf != (NET_BUF *)0) {                                         /* Chk pbuf's max size.                         */
        pbuf_hdr = &pbuf->Hdr;

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        switch (pbuf_hdr->Type) {
#if (NET_BUF_CFG_NBR_SMALL > 0)
            case NET_BUF_TYPE_SMALL:
#endif
#if (NET_BUF_CFG_NBR_LARGE > 0)
            case NET_BUF_TYPE_LARGE:
#endif
                 break;


            case NET_BUF_TYPE_NONE:
            default:
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrInvalidTypeCtr);
                 return (0);                                            /* Prevent 'break NOT reachable' warning.       */
        }
#endif

        if (ix_start < pbuf_hdr->Size) {
            max_size = pbuf_hdr->Size - ix_start;
        } else {
            max_size = 0;
        }


    } else {                                                            /* Else chk buf cfg sizes.                      */

#if   (NET_BUF_CFG_NBR_LARGE > 0)                                       /* ------------ LARGE BUF MAX SIZE ------------ */
        if (ix_start < NET_BUF_CFG_DATA_SIZE_LARGE) {
            max_size = NET_BUF_CFG_DATA_SIZE_LARGE - ix_start;
        } else {
            max_size = 0;
        }

#elif (NET_BUF_CFG_NBR_SMALL > 0)                                       /* ------------ SMALL BUF MAX SIZE ------------ */
        if (ix_start < NET_BUF_CFG_DATA_SIZE_SMALL) {
            max_size = NET_BUF_CFG_DATA_SIZE_SMALL - ix_start;
        } else {
            max_size = 0;
        }

#else                                                                   /* ----------------- NO BUFS ------------------ */
                                                                        /* See Note #2.                                 */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrSizeCtr);
        max_size = 0;
#endif
    }


    return (max_size);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetBuf_Get()
*
* Description : (1) Allocate & initialize a network buffer of sufficient size :
*
*                   (a) Get buffer of sufficient size
*                   (b) Validate   buffer
*                   (c) Initialize buffer
*                   (d) Update buffer pool statistics
*                   (e) Return pointer to buffer
*                         OR
*                       Null pointer & error code, on failure
*
*               (2) The buffer pools are implemented as stacks :
*
*                   (a) 'NetBuf_SmallPoolPtr'/'NetBuf_LargePoolPtr' point to the heads of the buffer pools.
*
*                   (b) Buffers' 'NextBufPtr's link each buffer to form the buffer pool stacks.
*
*                   (c) Buffers are inserted & removed at the heads of  the buffer pool stacks.
*
*
*                                       Buffers are
*                                    inserted & removed
*                                        at the head
*                                      (see Note #2c)
*
*                                             |               NextBufPtr
*                                             |             (see Note #2b)
*                                             v                    |
*                                                                  |
*                                          -------       -------   v   -------       -------
*                        Buffer Pool  ---->|     |------>|     |------>|     |------>|     |
*                          Pointer         |     |       |     |       |     |       |     |
*                                          |     |       |     |       |     |       |     |
*                      (see Note #2a)      -------       -------       -------       -------
*
*                                          |                                               |
*                                          |<------------ Pool of Free Buffers ----------->|
*                                          |                 (see Note #2)                 |
*
*
* Argument(s) : size        Requested buffer size  to store buffer data.
*
*               ix_start    Requested buffer index to store buffer data.
*
*               flags       Flags to select buffer options; bit-field flags logically OR'd :
*
*                               NET_BUF_FLAG_NONE               NO buffer flags selected.
*                               NET_BUF_FLAG_CLR_MEM            Clear buffer memory (i.e. set each buffer 
*                                                                   data octet to 0x00).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_BUF_ERR_NONE                Network buffer successfully allocated & initialized.
*                               NET_BUF_ERR_INVALID_SIZE        Requested size is greater then the maximum buffer 
*                                                                   size available.
*                               NET_BUF_ERR_INVALID_LEN         Requested size & start index calculation overflows 
*                                                                   buffer's DATA area.
*                               NET_BUF_ERR_INVALID_TYPE        Network buffer is NOT a valid buffer type.
*                               NET_BUF_ERR_NONE_AVAIL          NO available buffers to allocate.
*
* Return(s)   : Pointer to network buffer, if NO errors.
*
*               Pointer to NULL,           otherwise.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*$PAGE*
* Note(s)     : (3) (a) 'size' & 'ix_start' argument check NOT required unless 'NET_BUF_SIZE's native data 
*                       type 'CPU_INT16U' is incorrectly configured as a signed integer in 'cpu.h'.
*
*                   (b) 'size' of 0 octets allowed.
*
*               (4) The 'GET BUF : NO BUFS' pre-processor 'else'-conditional code will never be compiled/
*                   linked since 'net_buf.h' ensures that at least one of the two configuration constants 
*                   (NET_BUF_CFG_NBR_SMALL or NET_BUF_CFG_NBR_LARGE) will be configured with a value greater 
*                   than zero (see 'net_buf.h  CONFIGURATION ERRORS').  The 'else'-conditional code is included
*                   for completeness & as an extra precaution in case 'net_buf.h' is incorrectly modified.
*
*               (5) Buffer memory cleared in NetBuf_Get() instead of in NetBuf_Free() so that the data in
*                   any freed buffer in a buffer pool may be inspected until that buffer is next allocated.
*********************************************************************************************************
*/

NET_BUF  *NetBuf_Get (NET_BUF_SIZE   size,
                      NET_BUF_SIZE   ix_start,
                      CPU_INT16U     flags,
                      NET_ERR       *perr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NET_TYPE         buf_type;
#endif
    NET_BUF_SIZE     size_len;
    NET_STAT_POOL   *pbuf_pool_stat;
    NET_BUF        **pbuf_pool;
    NET_BUF         *pbuf;
    NET_BUF_HDR     *pbuf_hdr;
    CPU_BOOLEAN      buf_clr_mem;
    NET_ERR          stat_err;


#if 0                                                           /* See Note #3.                                         */
                                                                /* ------------------ VALIDATE SIZE ------------------- */
    if (size < 0) {                                             /* If neg size or ix req'd, rtn err (see Note #3a).     */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrSizeCtr);
       *perr =   NET_BUF_ERR_INVALID_SIZE;
        return ((NET_BUF *)0);
    }

    if (ix_start < 0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrIxCtr);
       *perr =   NET_BUF_ERR_INVALID_IX;
        return ((NET_BUF *)0);
    }
#endif


/*$PAGE*/
                                                                        /* ----------------- GET BUF ------------------ */
    size_len = size + ix_start;                                         /* Calc tot req'd size from start ix.           */
                                                                        /* Discard possible size len ovf's.             */
    if (size_len < size) {                                              
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrLenCtr);
       *perr =   NET_BUF_ERR_INVALID_LEN;
        return ((NET_BUF *)0);
    }
    if (size_len < ix_start) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrLenCtr);
       *perr =   NET_BUF_ERR_INVALID_LEN;
        return ((NET_BUF *)0);
    }


#if ((NET_BUF_CFG_NBR_SMALL > 0) && (NET_BUF_CFG_NBR_LARGE > 0))        /* ------------ SMALL & LARGE BUFS ------------ */
    if (size_len <= NET_BUF_CFG_DATA_SIZE_SMALL) {                      /* If size <= SMALL buf, ...                    */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        buf_type       =  NET_BUF_TYPE_SMALL;
#endif
        pbuf_pool      = (NET_BUF      **)&NetBuf_SmallPoolPtr;         /* ... get a  SMALL buf.                        */
        pbuf_pool_stat = (NET_STAT_POOL *)&NetBuf_SmallPoolStat;

    } else if (size_len <= NET_BUF_CFG_DATA_SIZE_LARGE) {               /* If size <= LARGE buf, ...                    */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        buf_type       =  NET_BUF_TYPE_LARGE;
#endif
        pbuf_pool      = (NET_BUF      **)&NetBuf_LargePoolPtr;         /* ... get a  LARGE buf.                        */
        pbuf_pool_stat = (NET_STAT_POOL *)&NetBuf_LargePoolStat;

    } else {                                                            /* Else rtn err.                                */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrSizeCtr);
       *perr =   NET_BUF_ERR_INVALID_SIZE;
        return ((NET_BUF *)0);
    }


#elif ((NET_BUF_CFG_NBR_SMALL > 0) && (NET_BUF_CFG_NBR_LARGE == 0))     /* ------------- SMALL BUFS ONLY -------------- */
    if (size_len <= NET_BUF_CFG_DATA_SIZE_SMALL) {                      /* If size <= SMALL buf, ...                    */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        buf_type       =  NET_BUF_TYPE_SMALL;
#endif
        pbuf_pool      = (NET_BUF      **)&NetBuf_SmallPoolPtr;         /* ... get a  SMALL buf.                        */
        pbuf_pool_stat = (NET_STAT_POOL *)&NetBuf_SmallPoolStat;

    } else {                                                            /* Else rtn err.                                */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrSizeCtr);
       *perr =   NET_BUF_ERR_INVALID_SIZE;
        return ((NET_BUF *)0);
    }


#elif ((NET_BUF_CFG_NBR_SMALL == 0) && (NET_BUF_CFG_NBR_LARGE > 0))     /* ------------- LARGE BUFS ONLY -------------- */
    if (size_len <= NET_BUF_CFG_DATA_SIZE_LARGE) {                      /* If size <= LARGE buf, ...                    */
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        buf_type       =  NET_BUF_TYPE_LARGE;
#endif
        pbuf_pool      = (NET_BUF      **)&NetBuf_LargePoolPtr;         /* ... get a  LARGE buf.                        */
        pbuf_pool_stat = (NET_STAT_POOL *)&NetBuf_LargePoolStat;

    } else {                                                            /* Else none avail, rtn err.                    */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrSizeCtr);
       *perr =   NET_BUF_ERR_INVALID_SIZE;
        return ((NET_BUF *)0);
    }


#else                                                                   /* ----------------- NO BUFS ------------------ */
                                                                        /* See Note #4.                                 */
    NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrSizeCtr);
   *perr =   NET_BUF_ERR_INVALID_SIZE;
    return ((NET_BUF *)0);
#endif


/*$PAGE*/
    if (*pbuf_pool != (NET_BUF *)0) {                           /* If buf pool NOT empty, get buf from pool.            */
         pbuf       = *pbuf_pool;
         pbuf_hdr   = &pbuf->Hdr;
        *pbuf_pool  =  pbuf_hdr->NextBufPtr;
    } else {                                                    /* If NO buf avail, rtn err.                            */
         NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrNoneAvailCtr);
        *perr =   NET_BUF_ERR_NONE_AVAIL;
         return ((NET_BUF *)0);
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ------------------- VALIDATE BUF ------------------- */
    switch (pbuf_hdr->Type) {                                   /* Validate buf Type/Size.                              */
        case NET_BUF_TYPE_SMALL:
        case NET_BUF_TYPE_LARGE:
             if (pbuf_hdr->Size < NET_BUF_DATA_SIZE_MIN) {      /* If  buf   size < NET_BUF_DATA_SIZE_MIN, ...          */
                 NetBuf_Discard(pbuf, buf_type);                /* ... buf   size invalid; discard buf.                 */
                 NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrSizeCtr);
                *perr =   NET_BUF_ERR_INVALID_SIZE;
                 return ((NET_BUF *)0);
             }
             break;


        case NET_BUF_TYPE_NONE:                                 /* If invalid buf type, discard buf.                    */
        default:
             NetBuf_Discard(pbuf, NET_BUF_TYPE_NONE);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrInvalidTypeCtr);
            *perr =   NET_BUF_ERR_INVALID_TYPE;
             return ((NET_BUF *)0);                             /* Prevent 'break NOT reachable' compiler warning.      */
    }
#endif


                                                                /* --------------------- INIT BUF --------------------- */
    NetBuf_ClrHdr(pbuf_hdr);
    DEF_BIT_SET(pbuf_hdr->Flags, NET_BUF_FLAG_USED);            /* Set buf as used.                                     */
    pbuf_hdr->RefCtr = 1;                                       /* Set ref ctr to 1; NetBuf_Get() caller is first ref.  */

                                                                /* Clr ALL buf data octets to 0x00 (see Note #5).       */
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
   (void)&flags;                                                /* Prevent compiler warning.                            */
    buf_clr_mem = DEF_YES;
#else
    buf_clr_mem = DEF_BIT_IS_SET(flags, NET_BUF_FLAG_CLR_MEM);
#endif
    if (buf_clr_mem == DEF_YES) {
        DEF_BIT_SET(pbuf_hdr->Flags, NET_BUF_FLAG_CLR_MEM);
        Mem_Clr((void     *)&pbuf->Data[0],
                (CPU_SIZE_T) pbuf_hdr->Size);
    }


                                                                /* --------------- UPDATE BUF POOL STATS -------------- */
    NetStat_PoolEntryUsedInc(pbuf_pool_stat, &stat_err);

   *perr =  NET_BUF_ERR_NONE;

    return (pbuf);                                              /* --------------------- RTN BUF ---------------------- */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetBuf_Free()
*
* Description : (1) Free a network buffer :
*
*                   (a) Configure buffer free by buffer type
*                   (b) Unlink    buffer from network layer(s)                          See Note #3
*                   (c) Clear     buffer controls
*                   (d) Free      buffer back to buffer pool
*                   (e) Update    buffer pool statistics
*                   (f) Free IP option buffer                                           See Note #4
*
*
* Argument(s) : pbuf        Pointer to a network buffer.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) #### To prevent freeing a buffer already freed via auxiliary pointer(s), NetBuf_Free()
*                   checks the buffer's 'USED' flag BEFORE freeing the buffer.
*
*                   This prevention is only best-effort since any invalid duplicate  buffer frees MAY be
*                   asynchronous to potentially valid buffer gets.  Thus the invalid buffer free(s) MAY
*                   corrupt the buffer's valid operation(s).
*
*                   However, since the primary tasks of the network protocol suite are prevented from
*                   running concurrently (see 'net.h  Note #2'), it is NOT necessary to protect network
*                   buffer resources from possible corruption since no asynchronous access from other
*                   network tasks is possible.
*
*               (3) If a network buffer's unlink function is available, it is assumed that the function
*                   correctly unlinks the network buffer from any other network layer(s).
*
*               (4) #### Since any single IP packet requires only a single network buffer to receive IP
*                   options (see 'net_ip.c  NetIP_RxPktValidate()  Note #1bC'), then no more than ONE 
*                   network buffer should be linked as an IP options buffer from another buffer.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetBuf_Free (NET_BUF  *pbuf)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN      used;
#endif
    NET_BUF_HDR     *pbuf_hdr;
    NET_BUF        **pbuf_pool;
    NET_BUF         *pbuf_ip_opt;
    NET_STAT_POOL   *pbuf_pool_stat;
    CPU_FNCT_PTR     unlink_fnct;
    NET_ERR          stat_err;


                                                                /* ------------------- VALIDATE PTR ------------------- */
    if (pbuf == (NET_BUF *)0) {
        return;
    }

    pbuf_hdr = &pbuf->Hdr;

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                 /* ---------------- VALIDATE BUF USED ----------------- */
    used = DEF_BIT_IS_SET(pbuf_hdr->Flags, NET_BUF_FLAG_USED);
    if (used != DEF_YES) {                                      /* If buf NOT used, ...                                 */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrNotUsedCtr);
        return;                                                 /* ... rtn but do NOT free (see Note #2).               */
    }
#endif


                                                                /* ------------------- CFG BUF FREE ------------------- */
    switch (pbuf_hdr->Type) {
#if (NET_BUF_CFG_NBR_SMALL > 0)
        case NET_BUF_TYPE_SMALL:
             pbuf_pool      = (NET_BUF      **)&NetBuf_SmallPoolPtr;
             pbuf_pool_stat = (NET_STAT_POOL *)&NetBuf_SmallPoolStat;
             break;
#endif


#if (NET_BUF_CFG_NBR_LARGE > 0)
        case NET_BUF_TYPE_LARGE:
             pbuf_pool      = (NET_BUF      **)&NetBuf_LargePoolPtr;
             pbuf_pool_stat = (NET_STAT_POOL *)&NetBuf_LargePoolStat;
             break;
#endif


        case NET_BUF_TYPE_NONE:
        default:
             NetBuf_Discard(pbuf, NET_BUF_TYPE_NONE);
             NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrInvalidTypeCtr);
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

    pbuf_ip_opt = pbuf_hdr->IP_OptPtr;


                                                                /* -------------------- UNLINK BUF -------------------- */
    unlink_fnct = pbuf_hdr->UnlinkFnctPtr;
    if (unlink_fnct != (CPU_FNCT_PTR)0) {                       /* If unlink fnct avail, ..                             */
        unlink_fnct((void *)pbuf);                              /* .. unlink buf from other layer(s) [see Note #3].     */
    }


                                                                /* ---------------------- CLR BUF --------------------- */
    DEF_BIT_CLR(pbuf_hdr->Flags, NET_BUF_FLAG_USED);            /* Set buf as NOT used.                                 */

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
    NetBuf_ClrHdr(pbuf_hdr);
#endif    

                                                                /* --------------------- FREE BUF --------------------- */
    pbuf_hdr->NextBufPtr = *pbuf_pool;
   *pbuf_pool            =  pbuf;

                                                                /* -------------- UPDATE BUF POOL STATS --------------- */
    NetStat_PoolEntryUsedDec(pbuf_pool_stat, &stat_err);


                                                                /* ----------------- FREE IP OPT BUF ------------------ */
    NetBuf_Free(pbuf_ip_opt);                                   /* See Note #4.                                         */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetBuf_FreeBuf()
*
* Description : Free a network buffer.
*
* Argument(s) : pbuf        Pointer to a network buffer.
*
*               pctr        Pointer to possible error counter.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) #### Buffers are NOT validated for 'Type' or 'USED' before freeing.
*
*                   See also 'NetBuf_Free()  Note #2'.
*
*               (2) Buffers may be referenced by multiple layer(s).  Therefore, the buffer's reference 
*                   counter MUST be checked before freeing the buffer.
*********************************************************************************************************
*/

void  NetBuf_FreeBuf (NET_BUF  *pbuf,
                      NET_CTR  *pctr)
{
    NET_BUF_HDR  *pbuf_hdr;


    if (pbuf != (NET_BUF *)0) {
        pbuf_hdr = &pbuf->Hdr;
        if (pbuf_hdr->RefCtr > 1) {                             /* If     buf ref'd by multiple layers (see Note #2), ..*/
            pbuf_hdr->RefCtr--;                                 /* .. dec buf ref ctr.                                  */
        } else {                                                /* Else free buf.                                       */
            NetBuf_Free(pbuf);
        }
    }

    if (pctr != (NET_CTR *)0) {                                 /* If avail, ...                                        */
        NET_CTR_ERR_INC(*pctr);                                 /* ... inc err ctr.                                     */
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetBuf_FreeBufList()
*
* Description : Free a network buffer list.
*
*               (1) Network buffer lists are implemented as doubly-linked lists :
*
*                   (a) 'pbuf_list' points to the head of the buffer list.
*
*                   (b) Buffer's 'PrevBufPtr' & 'NextBufPtr' doubly-link each buffer in a buffer list.
*
*
*                                     ---        Head of         -------
*                                      ^       Buffer List  ---->|     |
*                                      |                         |     |
*                                      |     (see Note #1a)      |     |
*                                      |                         |     |
*                                      |                         |     |
*                                      |                         -------
*                                      |                           | ^
*                                      |                           | |
*                                      |                           v |
*                                                                -------
*                                 Buffer List                    |     |
*                                (see Note #1)                   |     |
*                                                                |     |
*                                      |                         |     |
*                                      |                         |     |
*                                      |                         -------
*                                      |                           | ^
*                                      |           NextBufPtr ---> | | <--- PrevBufPtr
*                                      |         (see Note #1b)    v |    (see Note #1b)
*                                      |                         -------
*                                      |                         |     |
*                                      v                         |     |
*                                     ---                        -------
*
*
* Argument(s) : pbuf_list   Pointer to a buffer list.
*
*               pctr        Pointer to possible error counter.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) #### Buffers are NOT validated for 'Type' or 'USED' before freeing.
*
*                   See also 'NetBuf_Free()  Note #2'.
*
*               (3) Buffers may be referenced by multiple layer(s).  Therefore, the buffer's reference 
*                   counter MUST be checked before freeing the buffer.
*
*               (4) Buffers NOT freed are unlinked from other buffer fragment lists & compressed within
*                   their own buffer list.  Ideally, buffer fragment lists SHOULD NEVER be compressed 
*                   but should be unlinked in their entirety.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetBuf_FreeBufList (NET_BUF  *pbuf_list,
                          NET_CTR  *pctr)
{
    NET_BUF      *pbuf;
    NET_BUF      *pbuf_prev;
    NET_BUF      *pbuf_next;
    NET_BUF_HDR  *pbuf_hdr;
    NET_BUF_HDR  *pbuf_prev_hdr;


    pbuf      = (NET_BUF *)pbuf_list;
    pbuf_prev = (NET_BUF *)0;

    while (pbuf  != (NET_BUF *)0) {                             /* Free ALL bufs in buf list.                           */
        pbuf_hdr  = &pbuf->Hdr;
        pbuf_next =  pbuf_hdr->NextBufPtr;

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
        pbuf_hdr->PrevPrimListPtr = (NET_BUF *)0;
        pbuf_hdr->NextPrimListPtr = (NET_BUF *)0;
#endif

        if (pbuf_hdr->RefCtr > 1) {                             /* If     buf ref'd by multiple layers (see Note #3), ..*/
            pbuf_hdr->RefCtr--;                                 /* .. dec buf ref ctr.                                  */
            pbuf_hdr->PrevBufPtr = (NET_BUF *)pbuf_prev;
            pbuf_hdr->NextBufPtr = (NET_BUF *)0;
            if (pbuf_prev != (NET_BUF *)0) {                    /* If prev buf non-NULL, ...                            */
                pbuf_prev_hdr             = &pbuf_prev->Hdr;
                pbuf_prev_hdr->NextBufPtr =  pbuf;              /* ... set prev buf's next ptr to cur buf.              */
            }
            pbuf_prev = pbuf;                                   /* Set cur buf as new prev buf (see Note #4).           */

        } else {                                                /* Else free buf.                                       */
            NetBuf_Free(pbuf);
        }

        if (pctr != (NET_CTR *)0) {                             /* If avail, ...                                        */
            NET_CTR_ERR_INC(*pctr);                             /* ... inc err ctr.                                     */
        }

        pbuf = pbuf_next;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetBuf_FreeBufQ_PrimList()
*
* Description : Free a network buffer queue, organized by the buffers' primary buffer lists.
*
*               (1) Network buffer queues are implemented as multiply-linked lists :
*
*                   (a) 'pbuf_q' points to the head of the buffer queue.
*
*                   (b) Buffers are multiply-linked to form a queue of buffer lists.  In the diagram below,
*                       the buffer lists in vertical columns represent buffer fragments & the head fragment
*                       of each buffer list is linked horizontally to form the queue of buffer lists.
*
*                       (1) Buffers' 'PrevPrimListPtr' & 'NextPrimListPtr' doubly-link each buffer list's
*                           head buffer to form the queue of buffer lists.
*
*                       (2) Buffer's 'PrevBufPtr'      & 'NextBufPtr'      doubly-link each buffer in a
*                           buffer list.
*
*
*                                                 |                                               |
*                                                 |<--------------- Buffer Queue ---------------->|
*                                                 |                 (see Note #1)                 |
*
*                                                             NextPrimListPtr
*                                                             (see Note #1b1)
*                                                                        |
*                                                                        |
*                      ---          Head of       -------       -------  v    -------       -------
*                       ^            Buffer  ---->|     |------>|     |------>|     |------>|     |
*                       |            Queue        |     |       |     |       |     |       |     |
*                       |                         |     |<------|     |<------|     |<------|     |
*                       |       (see Note #1a)    |     |       |     |  ^    |     |       |     |
*                       |                         |     |       |     |  |    |     |       |     |
*                       |                         -------       -------  |    -------       -------
*                       |                           | ^                  |      | ^
*                       |                           | |       PrevPrimListPtr   | |
*                       |                           v |       (see Note #1b1)   v |
*                       |                         -------                     -------
*                                                 |     |                     |     |
*                Fragments in the                 |     |                     |     |
*                same Buffer List                 |     |                     |     |
*                 (see Note #1b)                  |     |                     |     |
*                                                 |     |                     |     |
*                       |                         -------                     -------
*                       |                           | ^                         | ^
*                       |           NextBufPtr ---> | | <--- PrevBufPtr         | |
*                       |        (see Note #1b2)    v |   (see Note #1b2)       v |
*                       |                         -------                     -------
*                       |                         |     |                     |     |
*                       |                         |     |                     |     |
*                       |                         |     |                     -------
*                       |                         |     |
*                       v                         |     |
*                      ---                        -------
*
*
* Argument(s) : pbuf_q      Pointer to a buffer queue.
*
*               pctr        Pointer to possible error counter.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) #### Buffers are NOT validated for 'Type' or 'USED' before freeing.
*
*                   See also 'NetBuf_Free()  Note #2'.
*
*               (3) Buffers may be referenced by multiple layer(s).  Therefore, the buffers' reference 
*                   counters MUST be checked before freeing the buffer(s).
*
*               (4) Buffers NOT freed are unlinked from other buffer fragment lists & compressed within
*                   their own buffer list.  Ideally, buffer fragment lists SHOULD NEVER be compressed 
*                   but should be unlinked in their entirety.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetBuf_FreeBufQ_PrimList (NET_BUF  *pbuf_q,
                                NET_CTR  *pctr)
{
    NET_BUF      *pbuf_list;
    NET_BUF      *pbuf_list_next;
    NET_BUF      *pbuf;
    NET_BUF      *pbuf_prev;
    NET_BUF      *pbuf_next;
    NET_BUF_HDR  *pbuf_hdr;


    pbuf_list = pbuf_q;
    while (pbuf_list != (NET_BUF *)0) {                         /* Free ALL buf lists in buf Q.                         */
        pbuf_hdr                  = &pbuf_list->Hdr;
        pbuf_list_next            = (NET_BUF *)pbuf_hdr->NextPrimListPtr;
        pbuf_hdr->PrevPrimListPtr = (NET_BUF *)0;
        pbuf_hdr->NextPrimListPtr = (NET_BUF *)0;

        pbuf                      = (NET_BUF *)pbuf_list;
        pbuf_prev                 = (NET_BUF *)0;

        while (pbuf  != (NET_BUF *)0) {                         /* Free ALL bufs in buf list.                           */
            pbuf_hdr  = &pbuf->Hdr;
            pbuf_next =  pbuf_hdr->NextBufPtr;

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
            pbuf_hdr->PrevPrimListPtr = (NET_BUF *)0;
            pbuf_hdr->NextPrimListPtr = (NET_BUF *)0;
#endif

            if (pbuf_hdr->RefCtr > 1) {                         /* If     buf ref'd by multiple layers (see Note #3), ..*/
                pbuf_hdr->RefCtr--;                             /* .. dec buf ref ctr.                                  */
                pbuf_hdr->PrevBufPtr = (NET_BUF *)pbuf_prev;
                pbuf_hdr->NextBufPtr = (NET_BUF *)0;
                if (pbuf_prev != (NET_BUF *)0) {                /* If prev buf non-NULL, ...                            */
                    pbuf_hdr             = &pbuf_prev->Hdr;
                    pbuf_hdr->NextBufPtr =  pbuf;               /* ... set prev buf's next ptr to cur buf.              */
                }
                pbuf_prev = pbuf;                               /* Set cur buf as new prev buf (see Note #4).           */

            } else {                                            /* Else free buf.                                       */
                NetBuf_Free(pbuf);
            }

            if (pctr != (NET_CTR *)0) {                         /* If avail, ...                                        */
                NET_CTR_ERR_INC(*pctr);                         /* ... inc err ctr.                                     */
            }

            pbuf  = pbuf_next;
        }

        pbuf_list = pbuf_list_next;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetBuf_FreeBufQ_SecList()
*
* Description : Free a network buffer queue, organized by the buffers' secondary buffer lists.
*
*               (1) Network buffer queues are implemented as multiply-linked lists :
*
*                   (a) 'pbuf_q' points to the head of the buffer queue.
*
*                   (b) Buffers are multiply-linked to form a queue of buffer lists.  In the diagram below,
*                       the buffer lists in vertical columns represent buffer fragments & the head fragment
*                       of each buffer list is linked horizontally to form the queue of buffer lists.
*
*                       (1) Buffers' 'PrevSecListPtr' & 'NextSecListPtr' doubly-link each buffer list's
*                           head buffer to form the queue of buffer lists.
*
*                       (2) Buffer's 'PrevBufPtr'     & 'NextBufPtr'     doubly-link each buffer in a
*                           buffer list.
*
*
*                                                 |                                               |
*                                                 |<--------------- Buffer Queue ---------------->|
*                                                 |                 (see Note #1)                 |
*
*                                                              NextSecListPtr
*                                                             (see Note #1b1)
*                                                                        |
*                                                                        |
*                      ---          Head of       -------       -------  v    -------       -------
*                       ^            Buffer  ---->|     |------>|     |------>|     |------>|     |
*                       |            Queue        |     |       |     |       |     |       |     |
*                       |                         |     |<------|     |<------|     |<------|     |
*                       |       (see Note #1a)    |     |       |     |  ^    |     |       |     |
*                       |                         |     |       |     |  |    |     |       |     |
*                       |                         -------       -------  |    -------       -------
*                       |                           | ^                  |      | ^
*                       |                           | |        PrevSecListPtr   | |
*                       |                           v |       (see Note #1b1)   v |
*                       |                         -------                     -------
*                                                 |     |                     |     |
*                Fragments in the                 |     |                     |     |
*                same Buffer List                 |     |                     |     |
*                 (see Note #1b)                  |     |                     |     |
*                                                 |     |                     |     |
*                       |                         -------                     -------
*                       |                           | ^                         | ^
*                       |           NextBufPtr ---> | | <--- PrevBufPtr         | |
*                       |        (see Note #1b2)    v |   (see Note #1b2)       v |
*                       |                         -------                     -------
*                       |                         |     |                     |     |
*                       |                         |     |                     |     |
*                       |                         |     |                     -------
*                       |                         |     |
*                       v                         |     |
*                      ---                        -------
*
*
* Argument(s) : pbuf_q          Pointer to a buffer queue.
*
*               pctr            Pointer to possible error counter.
*
*               pfnct_unlink    Pointer to possible unlink funcion.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) #### Buffers are NOT validated for 'Type' or 'USED' before freeing.
*
*                   See also 'NetBuf_Free()  Note #2'.
*
*               (3) Buffers may be referenced by multiple layer(s).  Therefore, the buffers' reference 
*                   counters MUST be checked before freeing the buffer(s).
*
*               (4) Buffers NOT freed are unlinked from other buffer fragment lists & compressed within
*                   their own buffer list.  Ideally, buffer fragment lists SHOULD NEVER be compressed 
*                   but should be unlinked in their entirety.
*
*               (5) Since buffers' unlink functions are intended to unlink a buffer from a secondary
*                   buffer queue list; the secondary buffer queue list's unlink function MUST be cleared
*                   before freeing the buffer to avoid unlinking the buffer(s) from the secondary buffer
*                   queue list multiple times.
*
*                   See also 'NetBuf_Free()  Note #3'.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetBuf_FreeBufQ_SecList (NET_BUF       *pbuf_q,
                               NET_CTR       *pctr,
                               CPU_FNCT_PTR   pfnct_unlink)
{
    NET_BUF      *pbuf_list;
    NET_BUF      *pbuf_list_next;
    NET_BUF      *pbuf;
    NET_BUF      *pbuf_prev;
    NET_BUF      *pbuf_next;
    NET_BUF_HDR  *pbuf_hdr;


    pbuf_list = pbuf_q;
    while (pbuf_list   != (NET_BUF *)0) {                       /* Free ALL buf lists in buf Q.                         */
        pbuf_hdr        = &pbuf_list->Hdr;
        pbuf_list_next  = (NET_BUF *)pbuf_hdr->NextSecListPtr;

        pbuf            = (NET_BUF *)pbuf_list;
        pbuf_prev       = (NET_BUF *)0;

        while (pbuf    != (NET_BUF *)0) {                       /* Free ALL bufs in buf list.                           */
            pbuf_hdr    = &pbuf->Hdr;
            pbuf_next   =  pbuf_hdr->NextBufPtr;
                                                                /* Clr unlink & sec list ptrs (see Note #5).            */
            if (pbuf_hdr->UnlinkFnctPtr == (CPU_FNCT_PTR)pfnct_unlink) {
                pbuf_hdr->UnlinkFnctPtr  = (CPU_FNCT_PTR)0;
                pbuf_hdr->UnlinkObjPtr   = (void       *)0;
                pbuf_hdr->PrevSecListPtr = (NET_BUF    *)0;
                pbuf_hdr->NextSecListPtr = (NET_BUF    *)0;
            }

            if (pbuf_hdr->RefCtr > 1) {                         /* If     buf ref'd by multiple layers (see Note #3), ..*/
                pbuf_hdr->RefCtr--;                             /* .. dec buf ref ctr.                                  */
                pbuf_hdr->PrevBufPtr = (NET_BUF *)pbuf_prev;
                pbuf_hdr->NextBufPtr = (NET_BUF *)0;
                if (pbuf_prev != (NET_BUF *)0) {                /* If prev buf non-NULL, ...                            */
                    pbuf_hdr             = &pbuf_prev->Hdr;
                    pbuf_hdr->NextBufPtr =  pbuf;               /* ... set prev buf's next ptr to cur buf.              */
                }
                pbuf_prev = pbuf;                               /* Set cur buf as new prev buf (see Note #4).           */

            } else {                                            /* Else free buf.                                       */
                NetBuf_Free(pbuf);
            }

            if (pctr != (NET_CTR *)0) {                         /* If avail, ...                                        */
                NET_CTR_ERR_INC(*pctr);                         /* ... inc err ctr.                                     */
            }

            pbuf  = pbuf_next;
        }

        pbuf_list = pbuf_list_next;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetBuf_DataRd()
*
* Description : (1) Read data from network buffer's DATA area :
*
*                   (a) Validate data read index & size
*                   (b) Read     data from buffer
*
*
* Argument(s) : pbuf        Pointer to a network buffer.
*
*               ix          Index into buffer's DATA area.
*
*               len         Number of octets to read (see Note #2).
*
*               pdest       Pointer to destination to read data into (see Note #3).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_BUF_ERR_NONE                Read from network buffer DATA area successful.
*                               NET_BUF_ERR_NULL_PTR            Argument 'pbuf'/'pdest' passed a NULL pointer.
*                               NET_BUF_ERR_INVALID_TYPE        Argument 'pbuf's TYPE is invalid or unknown.
*                               NET_BUF_ERR_INVALID_IX          Invalid index  (outside buffer's DATA area).
*                               NET_BUF_ERR_INVALID_LEN         Invalid length (outside buffer's DATA area).
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) Data read of 0 octets allowed.
*
*               (3) Destination buffer size NOT validated; buffer overruns MUST be prevented by caller.
*
*               (4) 'ix' & 'len' argument check NOT required unless 'NET_BUF_SIZE's native data type 
*                   'CPU_INT16U' is incorrectly configured as a signed integer in 'cpu.h'.
*
*               (5) Buffer 'Size' is NOT re-validated; validated in NetBuf_Get().
*********************************************************************************************************
*/
/*$PAGE*/
void  NetBuf_DataRd (NET_BUF       *pbuf,
                     NET_BUF_SIZE   ix,
                     NET_BUF_SIZE   len,
                     CPU_INT08U    *pdest,
                     NET_ERR       *perr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NET_BUF_HDR   *pbuf_hdr;
    NET_BUF_SIZE   len_data;
#endif
    CPU_INT08U    *p_data;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ---------------- VALIDATE BUF PTR ------------------ */
    if (pbuf == (NET_BUF *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrNullPtrCtr);
       *perr = NET_BUF_ERR_NULL_PTR;
        return;
    }
                                                                /* ---------------- VALIDATE BUF TYPE ----------------- */
    pbuf_hdr = &pbuf->Hdr;
    switch (pbuf_hdr->Type) {
#if (NET_BUF_CFG_NBR_SMALL > 0)
        case NET_BUF_TYPE_SMALL:
#endif
#if (NET_BUF_CFG_NBR_LARGE > 0)
        case NET_BUF_TYPE_LARGE:
#endif
             break;


        case NET_BUF_TYPE_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrInvalidTypeCtr);
            *perr = NET_BUF_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

                                                                /* ----------------- VALIDATE DEST PTR ---------------- */
    if (pdest == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrNullPtrCtr);
       *perr = NET_BUF_ERR_NULL_PTR;
        return;
    }
#endif

                                                                /* ----------------- VALIDATE IX/SIZE ----------------- */
#if 0                                                           /* See Note #4.                                         */
    if (ix < 0) {                                               /* If req'd ix  < 0,    rtn err.                        */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

    if (len < 0) {                                              /* If req'd len < 0,    rtn err.                        */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrLenCtr);
       *perr = NET_BUF_ERR_INVALID_LEN;
        return;
    }
#endif
    if (len == 0) {                                             /* If req'd len = 0,    rtn null rd (see Note #2).      */
       *perr = NET_BUF_ERR_NONE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (ix >= pbuf_hdr->Size) {                                 /* If req'd ix  > size, rtn err.                        */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

    len_data = ix + len;
    if (len_data > pbuf_hdr->Size) {                            /* If req'd len > size, rtn err.                        */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrLenCtr);
       *perr = NET_BUF_ERR_INVALID_LEN;
        return;
    }
#endif

                                                                /* ------------------- RD BUF DATA -------------------- */
                                                                /* Req'd ix & len within  buf DATA area; ...            */
    p_data = &pbuf->Data[ix];                                   /* ... set ptr to ix into buf DATA area, ...            */
    Mem_Copy((void     *)pdest,                                 /* ... & copy len nbr DATA buf octets to dest.          */
             (void     *)p_data,
             (CPU_SIZE_T)len);

   *perr = NET_BUF_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetBuf_DataWr()
*
* Description : (1) Write data into network buffer's DATA area :
*
*                   (a) Validate data write index & size
*                   (b) Write    data into buffer
*
*
* Argument(s) : pbuf        Pointer to a network buffer.
*
*               ix          Index into buffer's DATA area.
*
*               len         Number of octets to write (see Note #2).
*
*               psrc        Pointer to data to write.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_BUF_ERR_NONE                Write to network buffer DATA area successful.
*                               NET_BUF_ERR_NULL_PTR            Argument 'pbuf'/'psrc' passed a NULL pointer.
*                               NET_BUF_ERR_INVALID_TYPE        Argument 'pbuf's TYPE is invalid or unknown.
*                               NET_BUF_ERR_INVALID_IX          Invalid index  (outside buffer's DATA area).
*                               NET_BUF_ERR_INVALID_LEN         Invalid length (outside buffer's DATA area).
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) Data write of 0 octets allowed.
*
*               (3) 'ix' & 'len' argument check NOT required unless 'NET_BUF_SIZE's native data type 
*                   'CPU_INT16U' is incorrectly configured as a signed integer in 'cpu.h'.
*
*               (4) Buffer 'Size' is NOT re-validated; validated in NetBuf_Get().
*********************************************************************************************************
*/
/*$PAGE*/
void  NetBuf_DataWr (NET_BUF       *pbuf,
                     NET_BUF_SIZE   ix,
                     NET_BUF_SIZE   len,
                     CPU_INT08U    *psrc,
                     NET_ERR       *perr)
{
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    NET_BUF_HDR   *pbuf_hdr;
    NET_BUF_SIZE   len_data;
#endif
    CPU_INT08U    *p_data;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ---------------- VALIDATE BUF PTR ------------------ */
    if (pbuf == (NET_BUF *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrNullPtrCtr);
       *perr = NET_BUF_ERR_NULL_PTR;
        return;
    }
                                                                /* ---------------- VALIDATE BUF TYPE ----------------- */
    pbuf_hdr = &pbuf->Hdr;
    switch (pbuf_hdr->Type) {
#if (NET_BUF_CFG_NBR_SMALL > 0)
        case NET_BUF_TYPE_SMALL:
#endif
#if (NET_BUF_CFG_NBR_LARGE > 0)
        case NET_BUF_TYPE_LARGE:
#endif
             break;


        case NET_BUF_TYPE_NONE:
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrInvalidTypeCtr);
            *perr = NET_BUF_ERR_INVALID_TYPE;
             return;                                            /* Prevent 'break NOT reachable' compiler warning.      */
    }

                                                                /* ----------------- VALIDATE SRC PTR ----------------- */
    if (psrc == (CPU_INT08U *)0) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrNullPtrCtr);
       *perr = NET_BUF_ERR_NULL_PTR;
        return;
    }
#endif

                                                                /* ----------------- VALIDATE IX/SIZE ----------------- */
#if 0                                                           /* See Note #3.                                         */
    if (ix < 0) {                                               /* If req'd ix  < 0,    rtn err.                        */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

    if (len < 0) {                                              /* If req'd len < 0,    rtn err.                        */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrLenCtr);
       *perr = NET_BUF_ERR_INVALID_LEN;
        return;
    }
#endif
    if (len == 0) {                                             /* If req'd len = 0,    rtn null wr (see Note #2).      */
       *perr = NET_BUF_ERR_NONE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    if (ix >= pbuf_hdr->Size) {                                 /* If req'd ix  > size, rtn err.                        */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrIxCtr);
       *perr = NET_BUF_ERR_INVALID_IX;
        return;
    }

    len_data = ix + len;
    if (len_data > pbuf_hdr->Size) {                            /* If req'd len > size, rtn err.                        */
        NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrLenCtr);
       *perr = NET_BUF_ERR_INVALID_LEN;
        return;
    }
#endif

                                                                /* ------------------- WR BUF DATA -------------------- */
                                                                /* Req'd ix & len within  buf DATA area; ...            */
    p_data = &pbuf->Data[ix];                                   /* ... set ptr to ix into buf DATA area, ...            */
    Mem_Copy((void     *)p_data,                                /* ... & copy len nbr src octets into DATA buf.         */
             (void     *)psrc,
             (CPU_SIZE_T)len);            

   *perr = NET_BUF_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetBuf_IsUsed()
*
* Description : Validate buffer in use.
*
* Argument(s) : pbuf        Pointer to object to validate as a network buffer in use.
*
* Return(s)   : DEF_YES, buffer   valid &      in use.
*
*               DEF_NO,  buffer invalid or NOT in use.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) NetBuf_IsUsed() MUST be called with the global network lock already acquired.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetBuf_IsUsed (NET_BUF  *pbuf)
{
    NET_BUF_HDR  *pbuf_hdr;
    CPU_BOOLEAN   used;

                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (pbuf == (NET_BUF *)0) {
        return  (DEF_NO);
    }
                                                                /* ------------------ VALIDATE TYPE ------------------- */
    pbuf_hdr = &pbuf->Hdr;
    switch (pbuf_hdr->Type) {
#if (NET_BUF_CFG_NBR_SMALL > 0)
        case NET_BUF_TYPE_SMALL:
#endif
#if (NET_BUF_CFG_NBR_LARGE > 0)
        case NET_BUF_TYPE_LARGE:
#endif
             break;


        case NET_BUF_TYPE_NONE:
        default:
             return (DEF_NO);                                   /* Prevent 'break NOT reachable' compiler warning.      */
    }

                                                                /* ---------------- VALIDATE BUF USED ----------------- */
    used =  DEF_BIT_IS_SET(pbuf_hdr->Flags, NET_BUF_FLAG_USED);

    return (used);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetBuf_SmallPoolStatGet()
*
* Description : Get SMALL network buffer statistics pool.
*
* Argument(s) : none.
*
* Return(s)   : SMALL network buffer statistics pool, if NO errors.
*
*               NULL                 statistics pool, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) NetBuf_SmallPoolStatGet() blocked until network initialization completes; return NULL
*                   statistics pool.
*
*               (2) 'NetBuf_SmallPoolStat' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if (NET_BUF_CFG_NBR_SMALL > 0)
NET_STAT_POOL  NetBuf_SmallPoolStatGet (void)
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
    stat_pool = NetBuf_SmallPoolStat;
    CPU_CRITICAL_EXIT();

    return (stat_pool);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetBuf_SmallPoolStatResetMaxUsed()
*
* Description : Reset SMALL network buffer statistics pool's maximum number of entries used.
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

#if (NET_BUF_CFG_NBR_SMALL > 0)
void  NetBuf_SmallPoolStatResetMaxUsed (void)
{
    NET_ERR  err;


    NetStat_PoolResetUsedMax(&NetBuf_SmallPoolStat, &err);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetBuf_LargePoolStatGet()
*
* Description : Get LARGE network buffer statistics pool.
*
* Argument(s) : none.
*
* Return(s)   : LARGE network buffer statistics pool, if NO errors.
*
*               NULL                 statistics pool, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) NetBuf_LargePoolStatGet() blocked until network initialization completes; return NULL
*                   statistics pool.
*
*               (2) 'NetBuf_LargePoolStat' MUST ALWAYS be accessed exclusively in critical sections.
*********************************************************************************************************
*/

#if (NET_BUF_CFG_NBR_LARGE > 0)
NET_STAT_POOL  NetBuf_LargePoolStatGet (void)
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
    stat_pool = NetBuf_LargePoolStat;
    CPU_CRITICAL_EXIT();

    return (stat_pool);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetBuf_LargePoolStatResetMaxUsed()
*
* Description : Reset LARGE network buffer statistics pool's maximum number of entries used.
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

#if (NET_BUF_CFG_NBR_LARGE > 0)
void  NetBuf_LargePoolStatResetMaxUsed (void)
{
    NET_ERR  err;


    NetStat_PoolResetUsedMax(&NetBuf_LargePoolStat, &err);
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
*                                         NetBuf_InitSmall()
*
* Description : (1) Initialize SMALL buffers & buffer pool :
*
*                   (a) Initialize SMALL buffer pool
*                   (b) Initialize SMALL buffer table
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetBuf_Init().
*
* Note(s)     : (2) SMALL buffer pool MUST be initialized PRIOR to initializing the pool with pointers to
*                   SMALL buffers.
*********************************************************************************************************
*/

#if (NET_BUF_CFG_NBR_SMALL > 0)
static  void  NetBuf_InitSmall (void)
{
    NET_BUF_SMALL  *pbuf;
    NET_BUF_HDR    *pbuf_hdr;
    NET_BUF_QTY     i;
    NET_ERR         err;

                                                                /* ------------------- INIT BUF POOL ------------------ */
    NetBuf_SmallPoolPtr = (NET_BUF_SMALL *)0;                   /* Init-clr SMALL buf pool (see Note #2).               */

    NetStat_PoolInit((NET_STAT_POOL   *)&NetBuf_SmallPoolStat,
                     (NET_STAT_POOL_QTY) NET_BUF_CFG_NBR_SMALL,
                     (NET_ERR         *)&err);


                                                                /* ------------------- INIT BUF TBL ------------------- */
    pbuf = &NetBuf_SmallTbl[0];
    for (i = 0; i < NET_BUF_CFG_NBR_SMALL; i++) {
        pbuf_hdr        = &pbuf->Hdr;
        pbuf_hdr->Type  =  NET_BUF_TYPE_SMALL;                  /* Init each SMALL buf type/size/id--NEVER modify.      */
        pbuf_hdr->Size  =  NET_BUF_CFG_DATA_SIZE_SMALL;
        pbuf_hdr->ID    = (NET_BUF_QTY)i;

        pbuf_hdr->Flags =  NET_BUF_FLAG_NONE;                   /* Init each buf as NOT used.                           */

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
        NetBuf_ClrHdr(pbuf_hdr);
        Mem_Clr((void     *)&pbuf->Data[0],
                (CPU_SIZE_T) pbuf_hdr->Size);
#endif
                                                                /* Free each buf to SMALL buf pool (see Note #2).       */
        pbuf_hdr->NextBufPtr = (NET_BUF       *)NetBuf_SmallPoolPtr;
        NetBuf_SmallPoolPtr  = (NET_BUF_SMALL *)pbuf;

        pbuf++;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetBuf_InitLarge()
*
* Description : (1) Initialize LARGE buffers & buffer pool :
*
*                   (a) Initialize LARGE buffer pool
*                   (b) Initialize LARGE buffer table
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetBuf_Init().
*
* Note(s)     : (2) LARGE buffer pool MUST be initialized PRIOR to initializing the pool with pointers to
*                   LARGE buffers.
*********************************************************************************************************
*/

#if (NET_BUF_CFG_NBR_LARGE > 0)
static  void  NetBuf_InitLarge (void)
{
    NET_BUF_LARGE  *pbuf;
    NET_BUF_HDR    *pbuf_hdr;
    NET_BUF_QTY     i;
    NET_ERR         err;

                                                                /* ------------------- INIT BUF POOL ------------------ */
    NetBuf_LargePoolPtr = (NET_BUF_LARGE *)0;                   /* Init-clr LARGE buf pool (see Note #2).               */

    NetStat_PoolInit((NET_STAT_POOL   *)&NetBuf_LargePoolStat,
                     (NET_STAT_POOL_QTY) NET_BUF_CFG_NBR_LARGE,
                     (NET_ERR         *)&err);


                                                                /* ------------------- INIT BUF TBL ------------------- */
    pbuf = &NetBuf_LargeTbl[0];
    for (i = 0; i < NET_BUF_CFG_NBR_LARGE; i++) {
        pbuf_hdr        = &pbuf->Hdr;
        pbuf_hdr->Type  =  NET_BUF_TYPE_LARGE;                  /* Init each LARGE buf type/size/id--NEVER modify.      */
        pbuf_hdr->Size  =  NET_BUF_CFG_DATA_SIZE_LARGE;
        pbuf_hdr->ID    = (NET_BUF_QTY)i;

        pbuf_hdr->Flags =  NET_BUF_FLAG_NONE;                   /* Init each buf as NOT used.                           */

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
        NetBuf_ClrHdr(pbuf_hdr);
        Mem_Clr((void     *)&pbuf->Data[0],
                (CPU_SIZE_T) pbuf_hdr->Size);
#endif
                                                                /* Free each buf to LARGE buf pool (see Note #2).       */
        pbuf_hdr->NextBufPtr = (NET_BUF       *)NetBuf_LargePoolPtr;
        NetBuf_LargePoolPtr  = (NET_BUF_LARGE *)pbuf;

        pbuf++;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetBuf_ClrHdr()
*
* Description : Clear network buffer header controls.
*
* Argument(s) : pbuf_hdr    Pointer to network buffer header.
*               --------    Argument validated in NetBuf_Get(),
*                                                 NetBuf_Free(),
*                                                 NetBuf_InitSmall(),
*                                                 NetBuf_InitLarge().
*
* Return(s)   : none.
*
* Caller(s)   : NetBuf_Get(),
*               NetBuf_Free(),
*               NetBuf_InitSmall(),
*               NetBuf_InitLarge().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetBuf_ClrHdr (NET_BUF_HDR  *pbuf_hdr)
{
    pbuf_hdr->PrevPrimListPtr          = (NET_BUF    *)0;
    pbuf_hdr->NextPrimListPtr          = (NET_BUF    *)0;
    pbuf_hdr->PrevSecListPtr           = (NET_BUF    *)0;
    pbuf_hdr->NextSecListPtr           = (NET_BUF    *)0;
    pbuf_hdr->PrevBufPtr               = (NET_BUF    *)0;
    pbuf_hdr->NextBufPtr               = (NET_BUF    *)0;

    pbuf_hdr->TmrPtr                   = (NET_TMR    *)0;

    pbuf_hdr->UnlinkFnctPtr            = (CPU_FNCT_PTR)0;
    pbuf_hdr->UnlinkObjPtr             = (void       *)0;

    pbuf_hdr->ProtocolHdrType          =  NET_PROTOCOL_TYPE_NONE;
    pbuf_hdr->IF_HdrIx                 =  NET_BUF_IX_NONE;
    pbuf_hdr->IF_HdrLen                =  0;
#ifdef  NET_ARP_MODULE_PRESENT
    pbuf_hdr->ARP_MsgIx                =  NET_BUF_IX_NONE;
    pbuf_hdr->ARP_MsgLen               =  0;
#endif
    pbuf_hdr->IP_HdrIx                 =  NET_BUF_IX_NONE;
    pbuf_hdr->IP_HdrLen                =  0;
    pbuf_hdr->ICMP_MsgIx               =  NET_BUF_IX_NONE;
    pbuf_hdr->ICMP_MsgLen              =  0;
    pbuf_hdr->ICMP_HdrLen              =  0;
    pbuf_hdr->TCP_UDP_HdrDataIx        =  NET_BUF_IX_NONE;
    pbuf_hdr->TCP_UDP_HdrLen           =  0;
    pbuf_hdr->DataIx                   =  NET_BUF_IX_NONE;
    pbuf_hdr->DataLen                  =  0;
    pbuf_hdr->TotLen                   =  0;

#ifdef  NET_ARP_MODULE_PRESENT
    pbuf_hdr->ARP_AddrHW_Ptr           = (CPU_INT08U *)0;
    pbuf_hdr->ARP_AddrProtocolPtr      = (CPU_INT08U *)0;
#endif

    pbuf_hdr->IP_TotLen                =  0;
    pbuf_hdr->IP_DataLen               =  0;
    pbuf_hdr->IP_DatagramLen           =  0;
    pbuf_hdr->IP_ID                    =  NET_IP_ID_NONE;
    pbuf_hdr->IP_Flags_FragOffset      =  NET_IP_HDR_FLAG_NONE | NET_IP_HDR_FRAG_OFFSET_NONE;
    pbuf_hdr->IP_FragSizeTot           =  NET_IP_FRAG_SIZE_NONE;
    pbuf_hdr->IP_FragSizeCur           =  0;
    pbuf_hdr->IP_AddrSrc               = (NET_IP_ADDR)NET_IP_ADDR_NONE;
    pbuf_hdr->IP_AddrDest              = (NET_IP_ADDR)NET_IP_ADDR_NONE;
    pbuf_hdr->IP_AddrNextRoute         = (NET_IP_ADDR)NET_IP_ADDR_NONE;
    pbuf_hdr->IP_AddrNextRouteNetOrder = (NET_IP_ADDR)NET_UTIL_HOST_TO_NET_32(NET_IP_ADDR_NONE);
    pbuf_hdr->IP_OptPtr                = (NET_BUF   *)0;
/*$PAGE*/
    pbuf_hdr->TCP_UDP_TotLen           =  0;
    pbuf_hdr->TCP_UDP_DataLen          =  0;
    pbuf_hdr->TCP_UDP_PortSrc          =  NET_PORT_NBR_NONE;
    pbuf_hdr->TCP_UDP_PortDest         =  NET_PORT_NBR_NONE;

#ifdef  NET_TCP_MODULE_PRESENT
    pbuf_hdr->TCP_HdrLen_Flags         =  NET_TCP_HDR_LEN_NONE | NET_TCP_HDR_FLAG_NONE;
    pbuf_hdr->TCP_SegLenInit           =  0;
    pbuf_hdr->TCP_SegLenLast           =  0;
    pbuf_hdr->TCP_SegLen               =  0;
    pbuf_hdr->TCP_SegLenData           =  0;
    pbuf_hdr->TCP_SegReTxCtr           =  0;
    pbuf_hdr->TCP_SegSync              =  DEF_NO;
    pbuf_hdr->TCP_SegClose             =  DEF_NO;
    pbuf_hdr->TCP_SegReset             =  DEF_NO;
    pbuf_hdr->TCP_SegAck               =  DEF_NO;
    pbuf_hdr->TCP_SegAckTxd            =  DEF_NO;
    pbuf_hdr->TCP_SegAckTxReqCode      =  NET_TCP_CONN_TX_ACK_NONE;
    pbuf_hdr->TCP_SeqNbrInit           =  NET_TCP_SEQ_NBR_NONE;
    pbuf_hdr->TCP_SeqNbrLast           =  NET_TCP_SEQ_NBR_NONE;
    pbuf_hdr->TCP_SeqNbr               =  NET_TCP_SEQ_NBR_NONE;
    pbuf_hdr->TCP_AckNbr               =  NET_TCP_ACK_NBR_NONE;
    pbuf_hdr->TCP_AckNbrLast           =  NET_TCP_ACK_NBR_NONE;
    pbuf_hdr->TCP_MaxSegSize           =  NET_TCP_MAX_SEG_SIZE_NONE;
    pbuf_hdr->TCP_WinSize              =  NET_TCP_WIN_SIZE_NONE;
    pbuf_hdr->TCP_WinSizeLast          =  NET_TCP_WIN_SIZE_NONE;
    pbuf_hdr->TCP_RTT_TS_Rxd_ms        =  NET_TCP_TX_RTT_TS_NONE;
    pbuf_hdr->TCP_RTT_TS_Txd_ms        =  NET_TCP_TX_RTT_TS_NONE;
    pbuf_hdr->TCP_Flags                =  NET_TCP_FLAG_NONE;
#endif

#ifdef  NET_CONN_MODULE_PRESENT
    pbuf_hdr->Conn_ID                  =  NET_CONN_ID_NONE;
    pbuf_hdr->Conn_ID_Transport        =  NET_CONN_ID_NONE;
    pbuf_hdr->Conn_ID_App              =  NET_CONN_ID_NONE;
    pbuf_hdr->ConnType                 =  NET_CONN_TYPE_CONN_NONE;
#endif

    pbuf_hdr->RefCtr                   =  0;
    pbuf_hdr->Flags                    =  NET_BUF_FLAG_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           NetBuf_Discard()
*
* Description : (1) Discard an invalid/corrupted network buffer from available buffer pools :
*
*                   (a) Discard buffer from available buffer pool                       See Note #2
*                   (b) Update  buffer pool statistics
*
*               (2) Assumes buffer is invalid/corrupt & MUST be removed.  Buffer removed simply by NOT 
*                   returning the buffer back to any buffer pool.
*
*
* Argument(s) : pbuf        Pointer to an invalid/corrupt network buffer.
*               ----        Argument checked   in NetBuf_Get(),
*                                                 NetBuf_Free().
*                           Argument validated in NetBuf_InitSmall(),
*                                                 NetBuf_InitLarge().
*
*               type        Buffer type :
*
*                               NET_BUF_TYPE_NONE               Buffer TYPE unknown.
*                               NET_BUF_TYPE_SMALL              Buffer TYPE SMALL.
*                               NET_BUF_TYPE_LARGE              Buffer TYPE LARGE.
*
* Return(s)   : none.
*
* Caller(s)   : NetBuf_Get(),
*               NetBuf_Free().
*
* Note(s)     : (3) If buffer 'Type' is unknown, it cannot be determined which buffer pool, SMALL or LARGE, 
*                   lost the buffer.  The unrecoverable buffer will remain unaccounted for in one of the 
*                   buffer pool's 'Used' entries.
*********************************************************************************************************
*/

static  void  NetBuf_Discard (NET_BUF   *pbuf,
                              NET_TYPE   type)
{
    NET_ERR  stat_err;

                                                                /* ------------------- DISCARD BUF -------------------- */
   (void)&pbuf;                                                 /* Prevent compiler warning (see Note #2).              */

                                                                /* --------------- UPDATE DISCARD STATS --------------- */
    switch (type) {
#if (NET_BUF_CFG_NBR_SMALL > 0)
        case NET_BUF_TYPE_SMALL:                                /* Discard SMALL corrupt buf.                           */
             NetStat_PoolEntryLostInc(&NetBuf_SmallPoolStat, &stat_err);
             break;
#endif


#if (NET_BUF_CFG_NBR_LARGE > 0)
        case NET_BUF_TYPE_LARGE:                                /* Discard LARGE corrupt buf.                           */
             NetStat_PoolEntryLostInc(&NetBuf_LargePoolStat, &stat_err);
             break;
#endif


        case NET_BUF_TYPE_NONE:                                 /* If type unknown, NO buf pool updated (see Note #3).  */
        default:
             NET_CTR_ERR_INC(Net_ErrCtrs.NetBuf_ErrLostCtr);
             break;
    }
}

