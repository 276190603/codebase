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
*                                       NETWORK UTILITY LIBRARY
*
* Filename      : net_util.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) NO compiler-supplied standard library functions are used by the network protocol suite.
*                     'net_util.*' implements ALL network-specific library functions.
*
*                     See also 'net.h  NETWORK INCLUDE FILES  Note #3'.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_UTIL_MODULE
#include  <net.h>


/*$PAGE*/
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  NET_UTIL_16_BIT_ONES_CPL_NEG_ZERO            0xFFFF


#define  NET_UTIL_16_BIT_SUM_ERR_NONE             DEF_BIT_NONE
#define  NET_UTIL_16_BIT_SUM_ERR_NULL_SIZE        DEF_BIT_01
#define  NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET       DEF_BIT_02


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

static  CPU_INT32U  NetUtil_16BitSumHdrCalc        (void         *phdr,
                                                    CPU_INT16U    hdr_size);

static  CPU_INT32U  NetUtil_16BitSumDataCalc       (void         *p_data,
                                                    CPU_INT16U    data_size,
                                                    CPU_INT08U   *poctet_prev,
                                                    CPU_INT08U   *poctet_last,
                                                    CPU_BOOLEAN   prev_octet_valid,
                                                    CPU_BOOLEAN   last_pkt_buf,
                                                    CPU_INT08U   *psum_err);

static  CPU_INT16U  NetUtil_16BitOnesCplSumDataCalc(void         *pdata_buf,
                                                    void         *ppseudo_hdr,
                                                    CPU_INT16U    pseudo_hdr_size,
                                                    NET_ERR      *perr);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetUtil_16BitOnesCplChkSumHdrCalc()
*
* Description : Calculate 16-bit one's-complement check-sum on packet header.
*
*               (1) See RFC #1071, Sections 1, 2.(1), & 4.1 for summary of 16-bit one's-complement
*                   check-sum & algorithm.
*
*               (2) To correctly calculate 16-bit one's-complement check-sums on memory buffers of any 
*                   octet-length & word-alignment, the check-sums MUST be calculated in network-order
*                   on headers that are arranged in network-order (see also 'NetUtil_16BitSumHdrCalc()
*                   Note #5b').
*
*
* Argument(s) : phdr        Pointer to packet header.
*
*               hdr_size    Size    of packet header.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UTIL_ERR_NONE           Check-sum calculated; check return value.
*                               NET_UTIL_ERR_NULL_PTR       Argument 'phdr'     passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE      Argument 'hdr_size' passed a zero size.
*
* Return(s)   : 16-bit one's-complement check-sum, if NO errors.
*
*               0,                                 otherwise.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (3) (a) Since the 16-bit sum calculation is returned as a 32-bit network-order value 
*                       (see 'NetUtil_16BitSumHdrCalc()  Note #5c1'), ...
*
*                   (b) ... the final check-sum MUST be converted to host-order but MUST NOT be re-
*                       converted back to network-order (see 'NetUtil_16BitSumHdrCalc()  Note #5c3').
*********************************************************************************************************
*/

NET_CHK_SUM  NetUtil_16BitOnesCplChkSumHdrCalc (void        *phdr,
                                                CPU_INT16U   hdr_size,
                                                NET_ERR     *perr)
{
    CPU_INT32U   sum;
    NET_CHK_SUM  chk_sum;
    NET_CHK_SUM  chk_sum_host;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (phdr == (void *)0) {
       *perr =  NET_UTIL_ERR_NULL_PTR;
        return (0);
    }
                                                                /* ------------------ VALIDATE SIZE ------------------- */
    if (hdr_size < 1) {
       *perr =  NET_UTIL_ERR_NULL_SIZE;
        return (0);
    }
#endif

                                                                /* --------- CALC HDR'S 16-BIT ONE'S-CPL SUM ---------- */
    sum = NetUtil_16BitSumHdrCalc(phdr, hdr_size);              /* Calc  16-bit sum (see Note #3a).                     */

    while (sum >> 16) {                                         /* While 16-bit sum ovf's, ...                          */
        sum = (sum & 0x0000FFFF) + (sum >> 16);                 /* ... sum ovf bits back into 16-bit one's-cpl sum.     */
    }


    chk_sum      = ~((NET_CHK_SUM)sum);                         /* Perform one's cpl on one's-cpl sum.                  */
    chk_sum_host = NET_UTIL_NET_TO_HOST_16(chk_sum);            /* Conv back to host-order (see Note #3b).              */

   *perr         = NET_UTIL_ERR_NONE;

    return (chk_sum_host);                                      /* Rtn 16-bit chk sum (see Note #3).                    */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetUtil_16BitOnesCplChkSumHdrVerify()
*
* Description : (1) Verify 16-bit one's-complement check-sum on packet header :
*
*                   (a) Calculate one's-complement sum on packet header
*                   (b) Verify check-sum by comparison of one's-complement sum to one's-complement 
*                         '-0' value (negative zero)
*
*               (2) See RFC #1071, Sections 1, 2.(1), & 4.1 for summary of 16-bit one's-complement
*                   check-sum & algorithm.
*
*               (3) To correctly calculate 16-bit one's-complement check-sums on memory buffers of any 
*                   octet-length & word-alignment, the check-sums MUST be calculated in network-order
*                   on headers that are arranged in network-order (see also 'NetUtil_16BitSumHdrCalc()
*                   Note #5b').
*
*
* Argument(s) : phdr        Pointer to packet header.
*
*               hdr_size    Size    of packet header.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UTIL_ERR_NONE           Check-sum verified; check return value.
*                               NET_UTIL_ERR_NULL_PTR       Argument 'phdr'     passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE      Argument 'hdr_size' passed a zero size.
*
* Return(s)   : DEF_OK,   if valid check-sum.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (4) (a) Since the 16-bit sum calculation is returned as a 32-bit network-order value 
*                       (see 'NetUtil_16BitSumHdrCalc()  Note #5c1'), ...
*
*                   (b) ... the check-sum MUST be converted to host-order but MUST NOT be re-converted
*                       back to network-order for the final check-sum comparison
*                       (see 'NetUtil_16BitSumHdrCalc()  Note #5c3').
*********************************************************************************************************
*/

CPU_BOOLEAN  NetUtil_16BitOnesCplChkSumHdrVerify (void        *phdr,
                                                  CPU_INT16U   hdr_size,
                                                  NET_ERR     *perr)
{
    CPU_INT32U   sum;
    NET_CHK_SUM  chk_sum;
    NET_CHK_SUM  chk_sum_host;
    CPU_BOOLEAN  valid;


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
                                                                /* ------------------ VALIDATE PTR -------------------- */
    if (phdr == (void *)0) {
       *perr =  NET_UTIL_ERR_NULL_PTR;
        return (DEF_FAIL);
    }
                                                                /* ------------------ VALIDATE SIZE ------------------- */
    if (hdr_size < 1) {
       *perr =  NET_UTIL_ERR_NULL_SIZE;
        return (DEF_FAIL);
    }
#endif

                                                                /* -------- VERIFY HDR'S 16-BIT ONE'S-CPL SUM --------- */
    sum = NetUtil_16BitSumHdrCalc(phdr, hdr_size);              /* Calc 16-bit sum (see Note #4a).                      */

    while (sum >> 16) {                                         /* While 16-bit sum ovf's, ...                          */
        sum = (sum & 0x0000FFFF) + (sum >> 16);                 /* ... sum ovf bits back into 16-bit one's-cpl sum.     */
    }

    chk_sum      = (NET_CHK_SUM)sum;
    chk_sum_host =  NET_UTIL_NET_TO_HOST_16(chk_sum);           /* Conv back to host-order (see Note #4b).              */

                                                                /* Verify chk sum (see Note #1b).                       */
    valid        = (chk_sum_host == NET_UTIL_16_BIT_ONES_CPL_NEG_ZERO) ? DEF_OK : DEF_FAIL;

   *perr         =  NET_UTIL_ERR_NONE;

    return (valid);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                NetUtil_16BitOnesCplChkSumDataCalc()
*
* Description : Calculate 16-bit one's-complement check-sum on packet data.
*
*               (1) See RFC #1071, Sections 1, 2.(1), & 4.1 for summary of 16-bit one's-complement
*                   check-sum & algorithm.
*
*               (2) Check-sum calculated on packet data encapsulated in :
*
*                   (a) One or more network buffers             Support non-fragmented & fragmented packets
*                   (b) Transport layer pseudo-header           See RFC #768, Section 'Fields : Checksum' &
*                                                                   RFC #793, Section 3.1 'Header Format :
*                                                                       Checksum'.
*
*               (3) To correctly calculate 16-bit one's-complement check-sums on memory buffers of any 
*                   octet-length & word-alignment, the check-sums MUST be calculated in network-order on
*                   data & headers that are arranged in network-order (see also 'NetUtil_16BitSumDataCalc()
*                   Note #5b').
*
*
* Argument(s) : pdata_buf           Pointer to packet data network buffer(s) (see Note #2a).
*
*               ppseudo_hdr         Pointer to transport layer pseudo-header (see Note #2b).
*
*               pseudo_hdr_size     Size    of transport layer pseudo-header.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UTIL_ERR_NONE               Check-sum calculated; check return value.
*
*                                                               - RETURNED BY NetUtil_16BitOnesCplSumDataCalc() : -
*                               NET_UTIL_ERR_NULL_PTR           Argument 'pdata_buf' passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Packet data is a zero size.
*                               NET_UTIL_ERR_INVALID_PROTOCOL   Invalid data packet protocol.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
* Return(s)   : 16-bit one's-complement check-sum, if NO errors.
*
*               0,                                 otherwise.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (4) (a) Since the 16-bit one's-complement check-sum calculations are returned in host-
*                       order, ...
*
*                   (b) ... the returned check-sum MUST NOT be re-converted back to network-order.
*
*                   See also 'NetUtil_16BitSumDataCalc()         Note #5c3' &
*                            'NetUtil_16BitOnesCplSumDataCalc()  Note #4'.
*********************************************************************************************************
*/

NET_CHK_SUM  NetUtil_16BitOnesCplChkSumDataCalc (void        *pdata_buf,
                                                 void        *ppseudo_hdr,
                                                 CPU_INT16U   pseudo_hdr_size,
                                                 NET_ERR     *perr)
{
    CPU_INT16U   sum;
    NET_CHK_SUM  chk_sum;

                                                                /* Calc 16-bit one's-cpl sum (see Note #4a).            */
    sum = NetUtil_16BitOnesCplSumDataCalc(pdata_buf, ppseudo_hdr, pseudo_hdr_size, perr);
    if (*perr != NET_UTIL_ERR_NONE) {
         return (0);
    }

    chk_sum = ~((NET_CHK_SUM)sum);                              /* Perform one's cpl on one's-cpl sum.                  */

   *perr    = NET_UTIL_ERR_NONE;

    return (chk_sum);                                           /* Rtn 16-bit chk sum (see Note #4).                    */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                               NetUtil_16BitOnesCplChkSumDataVerify()
*
* Description : (1) Verify 16-bit one's-complement check-sum on packet data :
*
*                   (a) Calculate one's-complement sum on packet data & packet pseudo-header
*                   (b) Verify check-sum by comparison of one's-complement sum to one's-complement 
*                         '-0' value (negative zero)
*
*               (2) See RFC #1071, Sections 1, 2.(1), & 4.1 for summary of 16-bit one's-complement
*                   check-sum & algorithm.
*
*               (3) Check-sum calculated on packet data encapsulated in :
*
*                   (a) One or more network buffers             Support non-fragmented & fragmented packets
*                   (b) Transport layer pseudo-header           See RFC #768, Section 'Fields : Checksum' &
*                                                                   RFC #793, Section 3.1 'Header Format :
*                                                                       Checksum'.
*
*               (4) To correctly calculate 16-bit one's-complement check-sums on memory buffers of any 
*                   octet-length & word-alignment, the check-sums MUST be calculated in network-order on
*                   data & headers that are arranged in network-order (see also 'NetUtil_16BitSumDataCalc()
*                   Note #5b').
*
*
* Argument(s) : pdata_buf           Pointer to packet data network buffer(s) (see Note #3a).
*
*               ppseudo_hdr         Pointer to transport layer pseudo-header (see Note #3b).
*
*               pseudo_hdr_size     Size    of transport layer pseudo-header.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UTIL_ERR_NONE               Check-sum calculated; check return value.
*
*                                                               - RETURNED BY NetUtil_16BitOnesCplSumDataCalc() : -
*                               NET_UTIL_ERR_NULL_PTR           Argument 'pdata_buf' passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Packet data is a zero size.
*                               NET_UTIL_ERR_INVALID_PROTOCOL   Invalid data packet protocol.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
* Return(s)   : DEF_OK,   if valid check-sum.
*
*               DEF_FAIL, otherwise.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (5) (a) Since the 16-bit one's-complement check-sum calculations are returned in host-
*                       order, ...
*
*                   (b) ... the returned check-sum MUST NOT be re-converted back to network-order for
*                       the final check-sum comparison.
*
*                   See also 'NetUtil_16BitSumDataCalc()         Note #5c3' &
*                            'NetUtil_16BitOnesCplSumDataCalc()  Note #4'.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetUtil_16BitOnesCplChkSumDataVerify (void        *pdata_buf,
                                                   void        *ppseudo_hdr,
                                                   CPU_INT16U   pseudo_hdr_size,
                                                   NET_ERR     *perr)
{
    CPU_INT16U   sum;
    NET_CHK_SUM  chk_sum;
    CPU_BOOLEAN  valid;

                                                                /* Calc 16-bit one's-cpl sum (see Note #5a).            */
    sum = NetUtil_16BitOnesCplSumDataCalc(pdata_buf, ppseudo_hdr, pseudo_hdr_size, perr);
    if (*perr != NET_UTIL_ERR_NONE) {
         return (DEF_FAIL);
    }
                                                                /* Verify chk sum (see Notes #1b & #5b).                */
    chk_sum = (NET_CHK_SUM)sum;
    valid   = (chk_sum == NET_UTIL_16_BIT_ONES_CPL_NEG_ZERO) ? DEF_OK : DEF_FAIL;

   *perr    =  NET_UTIL_ERR_NONE;

    return (valid);
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
*                                      NetUtil_16BitSumHdrCalc()
*
* Description : Calculate 16-bit sum on packet header memory buffer.
*
*               (1) Calculates the sum of consecutive 16-bit values.
*
*               (2) 16-bit sum is returned as a 32-bit value to preserve possible 16-bit summation overflow 
*                   in the upper 16-bits.
*
*
* Argument(s) : phdr        Pointer to packet header.
*               ----        Argument checked in NetUtil_16BitOnesCplChkSumHdrCalc(),
*                                               NetUtil_16BitOnesCplChkSumHdrVerify(),
*                                               NetUtil_16BitOnesCplSumDataCalc().
*
*               hdr_size    Size    of packet header.
*
* Return(s)   : 16-bit sum (see Note #2), if NO errors.
*
*               0,                        otherwise.
*
* Caller(s)   : NetUtil_16BitOnesCplChkSumHdrCalc(),
*               NetUtil_16BitOnesCplChkSumHdrVerify(),
*               NetUtil_16BitOnesCplSumDataCalc().
*
* Note(s)     : (3) Since many word-aligned processors REQUIRE that multi-octet words be located on word-
*                   aligned addresses, 16-bit sum calculation MUST ensure that 16-bit words are accessed
*                   on addresses that are multiples of 2 octets.
*
*                   If packet header memory buffer does NOT start on a 16-bit word address boundary, then 
*                   16-bit sum calculation MUST be performed by concatenating two consecutive 8-bit values.
*
*               (4) Modulo arithmetic is used to determine whether a memory buffer starts on the desired 
*                   word-aligned address boundary.
*
*                   Modulo arithmetic in ANSI-C REQUIREs operations performed on integer values.  Thus, 
*                   address values MUST be cast to an appropriately-sized integer value PRIOR to any
*                   modulo arithmetic operation.
*
*               (5) (a) Although "the sum of 16-bit integers can be computed in either byte order" 
*                       [RFC #1071, Section 2.(B)], the handling of odd-length &/or off-word-boundary 
*                       memory buffers is performed assuming network-order.
*
*                   (b) However, to correctly & consistently calculate 16-bit integer sums for even-/
*                       odd-length & word-/off-word-boundary memory buffers, the sums MUST be calculated 
*                       in network-order.
*
*                   (c) (1) To preserve possible 16-bit summation overflow (see Note #2) during all check-
*                           sum calculations, the 16-bit sum is returned as a 32-bit network-order value.
*
*                       (2) To encapsulate the network-order transform to the 16-bit check-sum calculations
*                           ONLY, the final check-sum MUST be converted to host-order.
*
*                           See 'NetUtil_16BitOnesCplChkSumHdrCalc()    Note #3b' &
*                               'NetUtil_16BitOnesCplChkSumHdrVerify()  Note #4b').
*
*                       (3) However, since network-to-host-order conversion & host-order memory access are
*                           inverse operations, the final host-order check-sum value MUST NOT be converted
*                           back to network-order for calculation or comparison.
*
*               (6) RFC #1071, Section 4.1 explicitly casts & sums the last odd-length octet in a check-sum 
*                   calculation as a 16-bit value.
*
*                   ???? However, this contradicts the following sections which state that "if the total
*                   length is odd, ... the last octet is padded on the right with ... one octet of zeros
*                   ... to form a 16 bit word for ... purposes ... [of] computing the checksum" :
*
*                   (a) RFC #768, Section     'Fields                     : Checksum'
*                   (b) RFC #792, Section     'Echo or Echo Reply Message : Checksum'
*                   (c) RFC #793, Section 3.1 'Header Format              : Checksum'
*
*                   See also 'NetUtil_16BitSumDataCalc()  Note #8'.
*********************************************************************************************************
*/
/*$PAGE*/
static  CPU_INT32U  NetUtil_16BitSumHdrCalc (void        *phdr,
                                             CPU_INT16U   hdr_size)
{
    CPU_INT32U   sum_32;
    CPU_INT32U   sum_val_32;
    CPU_INT16U   hdr_val_16;
    CPU_INT16U   size_rem;
    CPU_INT16U  *phdr_16;
    CPU_INT08U  *phdr_08;
    CPU_INT08U   modulo_16;

                                                                    /* ---------------- VALIDATE SIZE ----------------- */
    if (hdr_size < 1) {
        return (0);
    }


    size_rem  =  hdr_size;
    sum_32    =  0;

    modulo_16 = (CPU_INT08U)((CPU_ADDR)phdr % sizeof(CPU_INT16U));  /* See Note #4.                                     */
    if (modulo_16 == 0) {                                           /* If pkt hdr on 16-bit word boundary (see Note #3),*/
        phdr_16 = (CPU_INT16U *)phdr;
        while (size_rem >=  sizeof(CPU_INT16U)) {
            hdr_val_16   = (CPU_INT16U)*phdr_16++;
            sum_val_32   = (CPU_INT32U) NET_UTIL_HOST_TO_NET_16(hdr_val_16);    /* Conv to net-order (see Note #5b).    */
            sum_32      += (CPU_INT32U) sum_val_32;                 /* ... calc sum with 16-bit data words.             */
            size_rem    -=  sizeof(CPU_INT16U);
        }
        phdr_08 = (CPU_INT08U *)phdr_16;

    } else {                                                        /* Else if pkt hdr NOT on 16-bit word boundary, ... */
        phdr_08 = (CPU_INT08U *)phdr;
        while (size_rem >=  sizeof(CPU_INT16U)) {
            sum_val_32   = (CPU_INT32U)*phdr_08++;
            sum_val_32 <<=  DEF_OCTET_NBR_BITS;
            sum_val_32  += (CPU_INT32U)*phdr_08++;
            sum_32      += (CPU_INT32U) sum_val_32;                 /* ... calc sum with  8-bit data vals.              */
            size_rem    -=  sizeof(CPU_INT16U);
        }
    }

    if (size_rem > 0) {                                             /* Sum last octet, if any (see Note #6).            */
        sum_32 += (CPU_INT32U)*phdr_08;
    }


    return (sum_32);                                                /* Rtn 16-bit sum (see Note #5c1).                  */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetUtil_16BitSumDataCalc()
*
* Description : Calculate 16-bit sum on packet data memory buffer.
*
*               (1) Calculates the sum of consecutive 16-bit values.
*
*               (2) 16-bit sum is returned as a 32-bit value to preserve possible 16-bit summation overflow 
*                   in the upper 16-bits.
*
*
* Argument(s) : p_data              Pointer to packet data.
*               ------              Argument validated in NetUtil_16BitOnesCplSumDataCalc().
*
*               data_size           Size    of packet data (in this network buffer only
*                                                           [see 'NetUtil_16BitOnesCplSumDataCalc()  Note #1a']).
*
*               poctet_prev         Pointer to last octet from a fragmented packet's previous buffer.
*               -----------         Argument validated in NetUtil_16BitOnesCplSumDataCalc().
*
*               poctet_last         Pointer to variable that will receive the value of the last octet from a
*               -----------             fragmented packet's current buffer.
*
*                                   Argument validated in NetUtil_16BitOnesCplSumDataCalc().
*
*               prev_octet_valid    Indicate whether pointer to the last octet of the packet's previous
*               ----------------        buffer is valid.
*
*                                   Argument validated in NetUtil_16BitOnesCplSumDataCalc().
*
*               last_pkt_buf        Indicate whether the current packet buffer is the last packet buffer.
*               ------------        Argument validated in NetUtil_16BitOnesCplSumDataCalc().
*
*               psum_err            Pointer to variable that will receive the error return code(s) from this function :
*
*                                       NET_UTIL_16_BIT_SUM_ERR_NONE            No error return codes.
*
*                                   The following error return codes are bit-field codes logically OR'd & MUST 
*                                       be individually tested by bit-wise tests :
*
*                                       NET_UTIL_16_BIT_SUM_ERR_NULL_SIZE       Packet buffer's data size is
*                                                                                   a zero size.
*                                       NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET      Last odd-length octet in packet 
*                                                                                   buffer is available; check
*                                                                                   'poctet_last' return value.
*
* Return(s)   : 16-bit sum (see Note #2), if NO errors.
*
*               0,                        otherwise.
*
* Caller(s)   : NetUtil_16BitOnesCplSumDataCalc().
*$PAGE*
* Note(s)     : (3) Since many word-aligned processors REQUIRE that multi-octet words be located on word-
*                   aligned addresses, 16-bit sum calculation MUST ensure that 16-bit words are accessed
*                   on addresses that are multiples of 2 octets.
*
*                   If packet data memory buffer does NOT start on a 16-bit word address boundary, then 
*                   16-bit sum calculation MUST be performed by concatenating two consecutive 8-bit values.
*
*               (4) Modulo arithmetic is used to determine whether a memory buffer starts on the desired 
*                   word-aligned address boundary.
*
*                   Modulo arithmetic in ANSI-C REQUIREs operations performed on integer values.  Thus, 
*                   address values MUST be cast to an appropriately-sized integer value PRIOR to any
*                   modulo arithmetic operation.
*
*               (5) (a) Although "the sum of 16-bit integers can be computed in either byte order" 
*                       [RFC #1071, Section 2.(B)], the handling of odd-length &/or off-word-boundary 
*                       memory buffers is performed assuming network-order.
*
*                   (b) However, to correctly & consistently calculate 16-bit integer sums for even-/
*                       odd-length & word-/off-word-boundary memory buffers, the sums MUST be calculated 
*                       in network-order so that the last octet of any packet buffer is correctly pre-
*                       pended to the first octet of the next packet buffer.
*
*                   (c) (1) To preserve possible 16-bit summation overflow (see Note #2) during all check-
*                           sum calculations, the 16-bit sum is returned as a 32-bit network-order value.
*
*                       (2) To encapsulate the network-order transform to the 16-bit check-sum calculations
*                           ONLY, the final check-sum MUST be converted to host-order
*                           (see 'NetUtil_16BitOnesCplSumDataCalc()  Note #4').
*
*                       (3) However, since network-to-host-order conversion & host-order memory access are
*                           inverse operations, the final host-order check-sum value MUST NOT be converted
*                           back to network-order for calculation or comparison.
*
*               (6) Optimized 32-bit sum calculations implemented in the network protocol suite's network-
*                   specific library port optimization file(s).
*
*                   See also 'net_util.h  FUNCTION PROTOTYPES  DEFINED IN PRODUCT'S  net_util_a.*  Note #1'.
*
*               (7) Since pointer arithmetic is based on the specific pointer data type & inherent pointer
*                   data type size, pointer arithmetic operands :
*
*                   (a) MUST be in terms of the specific pointer data type & data type size; ...
*                   (b) SHOULD NOT & in some cases MUST NOT be cast to other data types or data type sizes.
*
*               (8) The following sections state that "if the total length is odd, ... the last octet
*                   is padded on the right with ... one octet of zeros ... to form a 16 bit word for
*                   ... purposes ... [of] computing the checksum" :
*
*                   (a) RFC #768, Section     'Fields                     : Checksum'
*                   (b) RFC #792, Section     'Echo or Echo Reply Message : Checksum'
*                   (c) RFC #793, Section 3.1 'Header Format              : Checksum'
*
*                   See also 'NetUtil_16BitSumHdrCalc()  Note #6'.
*********************************************************************************************************
*/
/*$PAGE*/
static  CPU_INT32U  NetUtil_16BitSumDataCalc (void         *p_data,
                                              CPU_INT16U    data_size,
                                              CPU_INT08U   *poctet_prev,
                                              CPU_INT08U   *poctet_last,
                                              CPU_BOOLEAN   prev_octet_valid,
                                              CPU_BOOLEAN   last_pkt_buf,
                                              CPU_INT08U   *psum_err)
{
#if (NET_CFG_OPTIMIZE == NET_OPTIMIZE_SPD)
    CPU_INT08U    modulo_32;
#if ((defined(uC_CFG_OPTIMIZE_ASM_EN)) && \
             (uC_CFG_OPTIMIZE_ASM_EN   == DEF_ENABLED))
    CPU_INT16U    size_rem_32_offset;
    CPU_INT16U    size_rem_32;
#else
    CPU_INT32U    data_val_32;
    CPU_INT32U   *pdata_32;
#endif
#endif
    CPU_INT32U    sum_32;
    CPU_INT32U    sum_val_32;
    CPU_INT16U    data_val_16;
    CPU_INT16U    size_rem;
    CPU_INT16U   *pdata_16;
    CPU_INT08U   *pdata_08;
    CPU_INT08U    modulo_16;
    CPU_BOOLEAN   pkt_aligned_16;


    sum_32 = 0;


    if (data_size < 1) {                                        /* ------------ HANDLE NULL-SIZE DATA PKT ------------- */
       *psum_err = NET_UTIL_16_BIT_SUM_ERR_NULL_SIZE;

        if (prev_octet_valid != DEF_NO) {                       /* If null size & last octet from prev pkt buf avail .. */

            if (last_pkt_buf != DEF_NO) {                       /* ...   & on last pkt buf,              ...            */
                sum_val_32   = (CPU_INT32U)*poctet_prev;        /* ...   cast prev pkt buf's last octet, ...            */
                sum_val_32 <<=  DEF_OCTET_NBR_BITS;             /* ... pad odd-len pkt len (see Note #5) ...            */
                sum_32       =  sum_val_32;                     /* ...  & rtn prev pkt buf's last octet as last sum.    */

            } else {                                            /* ... & NOT on last pkt buf, ...                       */
               *poctet_last = *poctet_prev;                     /* ... rtn last octet from prev pkt buf as last octet.  */
                DEF_BIT_SET(*psum_err, NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET);

            }

        } else {
            ;                                                   /* If null size & NO prev octet, NO action(s) req'd.    */
        }

        return (sum_32);                                        /* Rtn 16-bit sum (see Note #5c1).                      */
    }


/*$PAGE*/
                                                                    /* ----------- HANDLE NON-NULL DATA PKT ----------- */
    size_rem       =  data_size;
   *psum_err       =  NET_UTIL_16_BIT_SUM_ERR_NONE;

                                                                    /* See Notes #3 & #4.                               */
    modulo_16      = (CPU_INT08U)((CPU_ADDR)p_data % sizeof(CPU_INT16U));
    pkt_aligned_16 = (((modulo_16 == 0) && (prev_octet_valid == DEF_NO )) ||
                      ((modulo_16 != 0) && (prev_octet_valid == DEF_YES))) ? DEF_YES : DEF_NO;


    pdata_08 = (CPU_INT08U *)p_data;
    if (prev_octet_valid == DEF_YES) {                              /* If last octet from prev pkt buf avail,   ...     */
        sum_val_32   = (CPU_INT32U)*poctet_prev;
        sum_val_32 <<=  DEF_OCTET_NBR_BITS;                         /* ... prepend last octet from prev pkt buf ...     */

        sum_val_32  += (CPU_INT32U)*pdata_08++;
        sum_32      += (CPU_INT32U) sum_val_32;                     /* ... to first octet in cur pkt buf.               */

        size_rem    -=  sizeof(CPU_INT08U);
    }

    if (pkt_aligned_16 == DEF_YES) {                                /* If pkt data aligned on 16-bit boundary, ..       */
                                                                    /* .. calc sum with 16- & 32-bit data words.        */
        pdata_16  = (CPU_INT16U *)pdata_08;
#if (NET_CFG_OPTIMIZE == NET_OPTIMIZE_SPD)
        modulo_32 = (CPU_INT08U  )((CPU_ADDR)pdata_16 % sizeof(CPU_INT32U));    /* See Note #4.                         */
        if ((modulo_32    !=  0) &&                                 /* If leading 16-bit pkt data avail, ..             */
            (size_rem     >=  sizeof(CPU_INT16U))) {
             data_val_16   = (CPU_INT16U)*pdata_16++;
             sum_val_32    = (CPU_INT32U) NET_UTIL_HOST_TO_NET_16(data_val_16); /* Conv to net-order (see Note #5b).    */
             sum_32       += (CPU_INT32U) sum_val_32;               /* .. start calc sum with leading 16-bit data word. */
             size_rem     -=  sizeof(CPU_INT16U);
        }

#if ((defined(uC_CFG_OPTIMIZE_ASM_EN)) && \
             (uC_CFG_OPTIMIZE_ASM_EN   == DEF_ENABLED))
                                                                    /* Calc optimized 32-bit size rem.                  */
        size_rem_32_offset = (CPU_INT16U)(size_rem % sizeof(CPU_INT32U));
        size_rem_32        = (CPU_INT16U)(size_rem - size_rem_32_offset);
                                                                    /* Calc optimized 32-bit sum (see Note #6).         */
        sum_val_32         = (CPU_INT32U)NetUtil_16BitSumDataCalcAlign_32((void     *)pdata_16,
                                                                          (CPU_INT32U)size_rem_32);
        sum_32            += (CPU_INT32U)sum_val_32;
        size_rem          -= (CPU_INT32U)size_rem_32;

        pdata_08  = (CPU_INT08U *)pdata_16;
        pdata_08 +=               size_rem_32;                      /* MUST NOT cast ptr operand (see Note #7b).        */
        pdata_16  = (CPU_INT16U *)pdata_08;

#else
        pdata_32 = (CPU_INT32U *)pdata_16;
        while (size_rem >=  sizeof(CPU_INT32U)) {                   /* While pkt data aligned on 32-bit boundary; ...   */
            data_val_32  = (CPU_INT32U) *pdata_32++;                /* ... get sum data with 32-bit data words,   ...   */

            data_val_16  = (CPU_INT16U)((data_val_32 >> 16) & 0x0000FFFF);
            sum_val_32   = (CPU_INT32U)  NET_UTIL_HOST_TO_NET_16(data_val_16);  /* Conv to net-order (see Note #5b).    */
            sum_32      += (CPU_INT32U)  sum_val_32;                /* ... & calc sum with upper 16-bit data word ...   */

            data_val_16  = (CPU_INT16U) (data_val_32        & 0x0000FFFF);
            sum_val_32   = (CPU_INT32U)  NET_UTIL_HOST_TO_NET_16(data_val_16);  /* Conv to net-order (see Note #5b).    */
            sum_32      += (CPU_INT32U)  sum_val_32;                /* ...               & lower 16-bit data word.      */

            size_rem    -=  sizeof(CPU_INT32U);
        }
        pdata_16 = (CPU_INT16U *)pdata_32;
#endif
#endif

        while (size_rem >=  sizeof(CPU_INT16U)) {                   /* While pkt data aligned on 16-bit boundary; ..    */
            data_val_16  = (CPU_INT16U)*pdata_16++;
            sum_val_32   = (CPU_INT32U) NET_UTIL_HOST_TO_NET_16(data_val_16);   /* Conv to net-order (see Note #5b).    */
            sum_32      += (CPU_INT32U) sum_val_32;                 /* .. calc sum with 16-bit data words.              */
            size_rem    -=  sizeof(CPU_INT16U);
        }
        if (size_rem > 0) {
            sum_val_32   = (CPU_INT32U)(*((CPU_INT08U *)pdata_16));
        }

    } else {                                                        /* Else pkt data NOT aligned on 16-bit boundary, .. */
        while (size_rem >=  sizeof(CPU_INT16U)) {
            sum_val_32   = (CPU_INT32U)*pdata_08++;
            sum_val_32 <<=  DEF_OCTET_NBR_BITS;
            sum_val_32  += (CPU_INT32U)*pdata_08++;
            sum_32      += (CPU_INT32U) sum_val_32;                 /* .. calc sum with  8-bit data vals.               */
            size_rem    -=  sizeof(CPU_INT16U);
        }
        if (size_rem > 0) {
            sum_val_32   = (CPU_INT32U)*pdata_08;
        }
    }


/*$PAGE*/
    if (size_rem > 0) {
        if (last_pkt_buf !=  DEF_NO) {                              /* If last pkt buf, ...                             */
            sum_val_32  <<=  DEF_OCTET_NBR_BITS;                    /* ... pad odd-len pkt len (see Note #8).           */
            sum_32       += (CPU_INT32U)sum_val_32;
        } else {
           *poctet_last   = (CPU_INT08U)sum_val_32;                 /* Else rtn last octet.                             */
            DEF_BIT_SET(*psum_err, NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET);
        }
    }


    return (sum_32);                                                /* Rtn 16-bit sum (see Note #5c1).                  */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetUtil_16BitOnesCplSumDataCalc()
*
* Description : Calculate 16-bit one's-complement sum on packet data.
*
*               (1) Calculates the 16-bit one's-complement sum of packet data encapsulated in :
*
*                   (a) One or more network buffers             Support non-fragmented & fragmented packets
*                   (b) Transport layer pseudo-header           See RFC #768, Section 'Fields : Checksum' &
*                                                                   RFC #793, Section 3.1 'Header Format :
*                                                                       Checksum'.
*
* Argument(s) : pdata_buf           Pointer to packet data network buffer(s) (see Notes #1a & #2).
*
*               ppseudo_hdr         Pointer to transport layer pseudo-header (see Note #1b).
*
*               pseudo_hdr_size     Size    of transport layer pseudo-header.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_UTIL_ERR_NONE               16-bit one's-complement sum calculated;
*                                                                   check return value.
*                               NET_UTIL_ERR_NULL_PTR           Argument 'pdata_buf' passed a NULL pointer.
*                               NET_UTIL_ERR_NULL_SIZE          Packet data is a zero size.
*                               NET_UTIL_ERR_INVALID_PROTOCOL   Invalid data packet protocol.
*                               NET_BUF_ERR_INVALID_IX          Invalid buffer index.
*
* Return(s)   : 16-bit one's-complement sum, if NO errors.
*
*               0,                           otherwise.
*
* Caller(s)   : NetUtil_16BitOnesCplChkSumDataCalc(),
*               NetUtil_16BitOnesCplChkSumDataVerify().
*
* Note(s)     : (2) Pointer to network buffer packet NOT validated as a network buffer.  However, no memory
*                   corruption occurs since no write operations are performed.
*
*               (3) Default case already invalidated in earlier functions.  However, the default case is
*                   included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
*
*               (4) (a) Since the 16-bit sum calculations are returned as 32-bit network-order values 
*                       (see 'NetUtil_16BitSumDataCalc()  Note #5c1'), ...
*
*                   (b) ... the one's-complement sum MUST be converted to host-order but MUST NOT be re-
*                       converted back to network-order (see 'NetUtil_16BitSumDataCalc()  Note #5c3').
*********************************************************************************************************
*/
/*$PAGE*/
static  CPU_INT16U  NetUtil_16BitOnesCplSumDataCalc (void        *pdata_buf,
                                                     void        *ppseudo_hdr,
                                                     CPU_INT16U   pseudo_hdr_size,
                                                     NET_ERR     *perr)
{
    NET_BUF      *pbuf;
    NET_BUF      *pbuf_next;
    NET_BUF_HDR  *pbuf_hdr;
    void         *p_data;
    CPU_INT32U    sum;
    CPU_INT32U    sum_val;
    CPU_INT16U    sum_ones_cpl;
    CPU_INT16U    sum_ones_cpl_host;
    CPU_INT16U    data_ix;
    CPU_INT16U    data_len;
    CPU_INT08U    sum_err;
    CPU_INT08U    octet_prev;
    CPU_INT08U    octet_last;
    CPU_BOOLEAN   octet_prev_valid;
    CPU_BOOLEAN   octet_last_valid;
    CPU_BOOLEAN   mem_buf_last;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    CPU_BOOLEAN   mem_buf_first;
    CPU_BOOLEAN   mem_buf_null_size;
#endif


#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                     /* ----------------- VALIDATE PTR ----------------- */
    if (pdata_buf == (void *)0) {
       *perr =  NET_UTIL_ERR_NULL_PTR;
        return (0);
    }
#endif


                                                                    /* ------ CALC PKT DATA 16-BIT ONE'S-CPL SUM ------ */
    pbuf             = (NET_BUF *)pdata_buf;
    sum              =  0;
    octet_prev       =  0;
    octet_last       =  0;
    octet_prev_valid =  DEF_NO;
    octet_last_valid =  DEF_NO;
    mem_buf_last     =  DEF_NO;
#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
    mem_buf_first    =  DEF_YES;
#endif

    if (ppseudo_hdr != (void *)0) {                                 /* Calc pkt's pseudo-hdr 16-bit sum (see Note #1b). */
        sum_val  = NetUtil_16BitSumDataCalc((void       *) ppseudo_hdr,
                                            (CPU_INT16U  ) pseudo_hdr_size, 
                                            (CPU_INT08U *)&octet_prev,
                                            (CPU_INT08U *)&octet_last,
                                            (CPU_BOOLEAN ) octet_prev_valid,
                                            (CPU_BOOLEAN ) mem_buf_last,
                                            (CPU_INT08U *)&sum_err);
        sum     += sum_val;

        octet_last_valid = DEF_BIT_IS_SET(sum_err, NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET);
        if (octet_last_valid == DEF_YES) {                          /* If last octet from pseudo-hdr avail, ...         */
            octet_prev        = octet_last;                         /* ... prepend last octet to first pkt buf.         */
            octet_prev_valid  = DEF_YES;
        } else {
            octet_prev        = 0;
            octet_prev_valid  = DEF_NO;
        }
    }

/*$PAGE*/
    while (pbuf  != (NET_BUF *)0) {                                 /* Calc ALL data pkts' 16-bit sum (see Note #1a).   */
        pbuf_hdr  = &pbuf->Hdr;
        switch (pbuf_hdr->ProtocolHdrType) {                        /* Demux pkt buf's protocol ix & len.               */
            case NET_PROTOCOL_TYPE_ICMP:
                 data_ix  = pbuf_hdr->ICMP_MsgIx;
                 data_len = pbuf_hdr->ICMP_HdrLen    + (CPU_INT16U)pbuf_hdr->DataLen;
                 break;


            case NET_PROTOCOL_TYPE_UDP:
#ifdef  NET_TCP_MODULE_PRESENT
            case NET_PROTOCOL_TYPE_TCP:
#endif
                 data_ix  = pbuf_hdr->TCP_UDP_HdrDataIx;
                 data_len = pbuf_hdr->TCP_UDP_HdrLen + (CPU_INT16U)pbuf_hdr->DataLen;
                 break;


            default:                                                /* See Note #3.                                     */
                *perr =  NET_UTIL_ERR_INVALID_PROTOCOL;
                 return (0);                                        /* Prevent 'break NOT reachable' compiler warning.  */
        }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        if (data_ix == NET_BUF_IX_NONE) {
           *perr =  NET_BUF_ERR_INVALID_IX;
            return (0);
        }
#endif

        p_data       = (void    *)&pbuf->Data[data_ix];
        pbuf_next    = (NET_BUF *) pbuf_hdr->NextBufPtr;
        mem_buf_last = (pbuf_next == (NET_BUF *)0) ? DEF_YES : DEF_NO;
                                                                    /* Calc pkt buf's 16-bit sum.                       */
        sum_val      =  NetUtil_16BitSumDataCalc((void       *) p_data,
                                                 (CPU_INT16U  ) data_len, 
                                                 (CPU_INT08U *)&octet_prev,
                                                 (CPU_INT08U *)&octet_last,
                                                 (CPU_BOOLEAN ) octet_prev_valid,
                                                 (CPU_BOOLEAN ) mem_buf_last,
                                                 (CPU_INT08U *)&sum_err);

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)
        if (mem_buf_first == DEF_YES) {
            mem_buf_first  = DEF_NO;
            if (mem_buf_last == DEF_YES) {
                mem_buf_null_size = DEF_BIT_IS_SET(sum_err, NET_UTIL_16_BIT_SUM_ERR_NULL_SIZE);
                if (mem_buf_null_size != DEF_NO) {                  /* If ONLY mem buf & null size, rtn err.            */
                   *perr =  NET_UTIL_ERR_NULL_SIZE;
                    return (0);
                }
            }
        }
#endif

        if (mem_buf_last != DEF_YES) {                              /* If NOT on last pkt buf &                     ... */
            octet_last_valid = DEF_BIT_IS_SET(sum_err, NET_UTIL_16_BIT_SUM_ERR_LAST_OCTET);
            if (octet_last_valid == DEF_YES) {                      /* ...         last octet from cur  pkt buf avail,  */
                octet_prev        = octet_last;                     /* ... prepend last octet to   next pkt buf.        */
                octet_prev_valid  = DEF_YES;
            } else {
                octet_prev        = 0;
                octet_prev_valid  = DEF_NO;
            }
        }

        sum  += sum_val;
        pbuf  = pbuf_next;
    }


    while (sum >> 16) {                                             /* While 16-bit sum ovf's, ...                      */
        sum = (sum & 0x0000FFFF) + (sum >> 16);                     /* ... sum ovf bits back into 16-bit one's-cpl sum. */
    }

    sum_ones_cpl      = (CPU_INT16U)sum;
    sum_ones_cpl_host =  NET_UTIL_NET_TO_HOST_16(sum_ones_cpl);     /* Conv back to host-order  (see Note #4b).         */
   *perr              =  NET_UTIL_ERR_NONE;


    return (sum_ones_cpl_host);                                     /* Rtn 16-bit one's-cpl sum (see Note #1).          */
}

