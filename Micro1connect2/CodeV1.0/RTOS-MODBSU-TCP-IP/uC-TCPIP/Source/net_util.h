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
* Filename      : net_util.h
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
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_UTIL_MODULE
#define  NET_UTIL_EXT
#else
#define  NET_UTIL_EXT  extern
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/


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
*                           NETWORK WORD ORDER - TO - CPU WORD ORDER MACRO'S
*
* Description : Convert data values to & from network word order to host CPU word order.
*
* Argument(s) : val       Data value to convert (see Note #1).
*
* Return(s)   : Converted data value (see Note #1).
*
* Caller(s)   : various.
*
*               These macro's are network protocol suite application interface (API) macro's & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) 'val' data value to convert & any variable to receive the returned conversion MUST 
*                   start on appropriate CPU word-aligned addresses.  This is required because most word-
*                   aligned processors are more efficient & may even REQUIRE that multi-octet words start 
*                   on CPU word-aligned addresses.
*
*                   (a) For 16-bit word-aligned processors, this means that
*
*                           all 16- & 32-bit words MUST start on addresses that are multiples of 2 octets
*
*                   (b) For 32-bit word-aligned processors, this means that
*
*                           all 16-bit       words MUST start on addresses that are multiples of 2 octets
*                           all 32-bit       words MUST start on addresses that are multiples of 4 octets
*
*                   See also 'lib_mem.h  MEMORY DATA VALUE MACRO'S  Note #1a'.
*********************************************************************************************************
*/

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)

#define  NET_UTIL_NET_TO_HOST_32(val)                                   (val)
#define  NET_UTIL_NET_TO_HOST_16(val)                                   (val)


#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)

#define  NET_UTIL_NET_TO_HOST_32(val)                   (((((CPU_INT32U)(val)) & 0xFF000000) >> 24) | \
                                                         ((((CPU_INT32U)(val)) & 0x00FF0000) >>  8) | \
                                                         ((((CPU_INT32U)(val)) & 0x0000FF00) <<  8) | \
                                                         ((((CPU_INT32U)(val)) & 0x000000FF) << 24))

#define  NET_UTIL_NET_TO_HOST_16(val)                   (((((CPU_INT16U)(val)) &     0xFF00) >>  8) | \
                                                         ((((CPU_INT16U)(val)) &     0x00FF) <<  8))

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)

#define  NET_UTIL_NET_TO_HOST_32(val)                   (((((CPU_INT32U)(val)) & 0xFF000000) >>  8) | \
                                                         ((((CPU_INT32U)(val)) & 0x00FF0000) <<  8) | \
                                                         ((((CPU_INT32U)(val)) & 0x0000FF00) >>  8) | \
                                                         ((((CPU_INT32U)(val)) & 0x000000FF) <<  8))

#define  NET_UTIL_NET_TO_HOST_16(val)                   (((((CPU_INT16U)(val)) &     0xFF00) >>  8) | \
                                                         ((((CPU_INT16U)(val)) &     0x00FF) <<  8))

#else

#define  NET_UTIL_NET_TO_HOST_32(val)                                   (val)
#define  NET_UTIL_NET_TO_HOST_16(val)                                   (val)

#endif
#endif



#define  NET_UTIL_HOST_TO_NET_32(val)            NET_UTIL_NET_TO_HOST_32(val)
#define  NET_UTIL_HOST_TO_NET_16(val)            NET_UTIL_NET_TO_HOST_16(val)


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NETWORK DATA VALUE MACRO'S
*
* Description : Encode/decode data values to & from any CPU memory addresses.
*
* Argument(s) : various.
*
* Return(s)   : various.
*
* Caller(s)   : various.
*
*               These macro's are INTERNAL network protocol suite macro's & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network data value macro's appropriately convert data words :
*
*                   (a) (1) From network  word order to host CPU word order
*                       (2) From host CPU word order to network  word order
*
*                   (b) NO network-to-host word-order conversion performed
*
*               (2) (a) Some network data values MUST start on appropriate CPU word-aligned addresses :
*
*                       (1) Data values
*                       (2) Variables to receive data values
*
*                   (b) Some network data values may start on any CPU address, word-aligned or not :
*
*                       (1) Addresses to         data values
*                       (2) Addresses to receive data values
*
*                   See also 'lib_mem.h  MEMORY DATA VALUE MACRO'S  Note #1'.
*********************************************************************************************************
*/
                                                                                        /* See Notes #1a1, #2a2, & #2b1.*/
#define  NET_UTIL_VAL_GET_NET_16(addr)                          MEM_VAL_GET_INT16U_BIG(addr)
#define  NET_UTIL_VAL_GET_NET_32(addr)                          MEM_VAL_GET_INT32U_BIG(addr)
                                                                                        /* See Notes #1a2, #2a1, & #2b2.*/
#define  NET_UTIL_VAL_SET_NET_16(addr, val)                     MEM_VAL_SET_INT16U_BIG(addr, val)
#define  NET_UTIL_VAL_SET_NET_32(addr, val)                     MEM_VAL_SET_INT32U_BIG(addr, val)

                                                                                        /* See Notes #1a1 & #2b.        */
#define  NET_UTIL_VAL_COPY_GET_NET_16(addr_dest, addr_src)      MEM_VAL_COPY_GET_INT16U_BIG(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_GET_NET_32(addr_dest, addr_src)      MEM_VAL_COPY_GET_INT32U_BIG(addr_dest, addr_src)
                                                                                        /* See Notes #1a2 & #2b.        */
#define  NET_UTIL_VAL_COPY_SET_NET_16(addr_dest, addr_src)      MEM_VAL_COPY_SET_INT16U_BIG(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_SET_NET_32(addr_dest, addr_src)      MEM_VAL_COPY_SET_INT32U_BIG(addr_dest, addr_src)


                                                                                        /* See Notes #1b, #2a2, & #2b1. */
#define  NET_UTIL_VAL_GET_HOST_16(addr)                         MEM_VAL_GET_INT16U(addr)
#define  NET_UTIL_VAL_GET_HOST_32(addr)                         MEM_VAL_GET_INT32U(addr)
                                                                                        /* See Notes #1b, #2a1, & #2b2. */
#define  NET_UTIL_VAL_SET_HOST_16(addr, val)                    MEM_VAL_SET_INT16U(addr, val)
#define  NET_UTIL_VAL_SET_HOST_32(addr, val)                    MEM_VAL_SET_INT32U(addr, val)

                                                                                        /* See Notes #1b & #2b.         */
#define  NET_UTIL_VAL_COPY_GET_HOST_16(addr_dest, addr_src)     MEM_VAL_COPY_GET_INT16U(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_GET_HOST_32(addr_dest, addr_src)     MEM_VAL_COPY_GET_INT32U(addr_dest, addr_src)
                                                                                        /* See Notes #1b & #2b.         */
#define  NET_UTIL_VAL_COPY_SET_HOST_16(addr_dest, addr_src)     MEM_VAL_COPY_SET_INT16U(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_SET_HOST_32(addr_dest, addr_src)     MEM_VAL_COPY_SET_INT32U(addr_dest, addr_src)


                                                                                        /* See Notes #1b & #2b.         */
#define  NET_UTIL_VAL_COPY_16(addr_dest, addr_src)              MEM_VAL_COPY_16(addr_dest, addr_src)
#define  NET_UTIL_VAL_COPY_32(addr_dest, addr_src)              MEM_VAL_COPY_32(addr_dest, addr_src)


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                                        /* --------------- CHK SUM FNCTS -------------- */
NET_CHK_SUM  NetUtil_16BitOnesCplChkSumHdrCalc   (void        *phdr,
                                                  CPU_INT16U   hdr_size,
                                                  NET_ERR     *perr);

CPU_BOOLEAN  NetUtil_16BitOnesCplChkSumHdrVerify (void        *phdr,
                                                  CPU_INT16U   hdr_size,
                                                  NET_ERR     *perr);

NET_CHK_SUM  NetUtil_16BitOnesCplChkSumDataCalc  (void        *pdata_buf,
                                                  void        *ppseudo_hdr,
                                                  CPU_INT16U   pseudo_hdr_size,
                                                  NET_ERR     *perr);

CPU_BOOLEAN  NetUtil_16BitOnesCplChkSumDataVerify(void        *pdata_buf,
                                                  void        *ppseudo_hdr,
                                                  CPU_INT16U   pseudo_hdr_size,
                                                  NET_ERR     *perr);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                 DEFINED IN PRODUCT'S  net_util_a.*
*
* Note(s) : (1) The network protocol suite's network-specific library port optimization file(s) are located
*               in the following directories :
*
*               (a) \<Network Protocol Suite>\Ports\<cpu>\<compiler>\net_util_a.*
*
*                       where
*                               <Network Protocol Suite>        directory path for network protocol suite
*                               <cpu>                           directory name for specific processor (CPU)
*                               <compiler>                      directory name for specific compiler
*********************************************************************************************************
*/

#if ((defined(uC_CFG_OPTIMIZE_ASM_EN)) && \
             (uC_CFG_OPTIMIZE_ASM_EN   == DEF_ENABLED))
                                                                        /* Optimize 16-bit sum for 32-bit.              */
CPU_INT32U   NetUtil_16BitSumDataCalcAlign_32    (void        *pdata_32,
                                                  CPU_INT32U   size);
#endif


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                   DEFINED IN PRODUCT'S  net_bsp.c
*********************************************************************************************************
*/

                                                                        /* ----------------- TS FNCTS ----------------- */
NET_TS       NetUtil_TS_Get                      (void);                /* #### Provide fnct description/notes.         */


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

