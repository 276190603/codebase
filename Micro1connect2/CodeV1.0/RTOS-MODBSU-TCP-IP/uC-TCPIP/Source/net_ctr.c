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
*                                      NETWORK COUNTER MANAGEMENT
*
* Filename      : net_ctr.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_CTR_MODULE
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


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetCtr_Init()
*
* Description : (1) Initialize Network Counter Management Module :
*
*                   (a) Initialize network statistics counters
*                   (b) Initialize network error      counters
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

void  NetCtr_Init (void)
{
#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)                        /* ---------------- INIT NET STAT CTRS ---------------- */
    Mem_Clr((void     *)      &Net_StatCtrs,
            (CPU_SIZE_T)sizeof(Net_StatCtrs));
#endif

#if (NET_CTR_CFG_ERR_EN  == DEF_ENABLED)                        /* ---------------- INIT NET ERR  CTRS ---------------- */
    Mem_Clr((void     *)      &Net_ErrCtrs,
            (CPU_SIZE_T)sizeof(Net_ErrCtrs));
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetCtr_CalcPctLarge()
*
* Description : Calculate percentage equation for large, double-length counters.
*
*               (1) Counter percentage determined by the following equation :
*
*                                        Numerator * 100%
*                       Percentage (%) = ----------------
*                                          Denominator
*
*                   (a) To avoid complicated integer arithmetic on large, double-length counters, percentage
*                       calculation performed by shifting the numerator & denominator until all significant 
*                       digits are located in the numerator's & denominator's low-order counters :
*
*                                            (Numerator * 100%) / N
*                           Percentage (%) = ----------------------
*                                               Denominator / N
*
*                       This does not affect the result of the percentage calculation, only the resolution.
*
*
* Argument(s) : pct_numer_hi    High-order portion of numerator.
*               
*               pct_numer_lo    Low -order portion of numerator.
*               
*               pct_denom_hi    High-order portion of denominator.
*               
*               pct_denom_lo    Low -order portion of denominator.
*
* Return(s)   : Percentage calculation of numerator divided by denominator (see Note #1).
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (2) This percentage calculation is a potentially CPU expensive operation.  Recommended only
*                   from low-priority, background tasks.
*
*               (3) Although counters' native data type is unsigned integer & arithmetic right-shifts of
*                   unsigned integers SHOULD NOT produce a sign-extended '1' bit in the counters' most
*                   significant bit, clearing the most significant bit AFTER the right-shift ensures that
*                   no unexpected sign-extension occurs.
*********************************************************************************************************
*/

CPU_INT08U  NetCtr_CalcPctLarge (NET_CTR  pct_numer_hi,
                                 NET_CTR  pct_numer_lo,
                                 NET_CTR  pct_denom_hi,
                                 NET_CTR  pct_denom_lo)
{
    CPU_INT08U  pct;


    while ((pct_denom_hi >                         0) ||        /* While denom hi ctr > 0  OR ...                       */
           (pct_numer_lo >= NET_CTR_PCT_NUMER_MAX_TH)) {        /* ...   numer lo ctr > lo-ctr pct th (see Note #1a),   */
                                                                /* ...   right-shift-rotate hi ctrs into lo ctrs.       */

        pct_denom_lo >>= 1;                                     /* Right-shift denom lo ctr.                            */
        if (pct_denom_hi &   NET_CTR_BIT_LO) {                  /* If denom hi ctr LSb set, ...                         */
            pct_denom_lo |=  NET_CTR_BIT_HI;                    /* ...  set denom lo ctr MSb.                           */
        } else {
            pct_denom_lo &= ~NET_CTR_BIT_HI;                    /* Else clr denom lo ctr MSb (see Note #3).             */
        }
        pct_denom_hi >>=  1;                                    /* Right-shift denom hi ctr ...                         */
        pct_denom_hi  &= ~NET_CTR_BIT_HI;                       /* ... & clr MSb (see Note #3).                         */


        pct_numer_lo >>= 1;                                     /* Right-shift numer lo ctr.                            */
        if (pct_numer_hi &   NET_CTR_BIT_LO) {                  /* If numer hi ctr LSb set, ...                         */
            pct_numer_lo |=  NET_CTR_BIT_HI;                    /* ...  set numer lo ctr MSb.                           */
        } else {
            pct_numer_lo &= ~NET_CTR_BIT_HI;                    /* Else clr numer lo ctr MSb (see Note #3).             */
        }
        pct_numer_hi >>=  1;                                    /* Right-shift numer hi ctr ...                         */
        pct_numer_hi  &= ~NET_CTR_BIT_HI;                       /* ... & clr MSb (see Note #3).                         */
    }

    pct = (CPU_INT08U)((pct_numer_lo * 100) / pct_denom_lo);    /* Calc pct (see Note #1).                              */

    return (pct);
}

