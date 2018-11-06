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
*                                       NETWORK INTERFACE LAYER
*
*                                              ETHERNET
*
* Filename      : net_if.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports Ethernet as described in RFC #894; supports IEEE 802 as described in RFC #1042.
*
*                 (2) Ethernet implementation conforms to RFC #1122, Section 2.3.3, bullets (a) & (b), but
*                     does NOT implement bullet (c) :
*
*                     RFC #1122                  LINK LAYER                  October 1989
*
*                     2.3.3  ETHERNET (RFC-894) and IEEE 802 (RFC-1042) ENCAPSULATION
*
*                            Every Internet host connected to a 10Mbps Ethernet cable :
*
*                            (a) MUST be able to send and receive packets using RFC-894 encapsulation;
*
*                            (b) SHOULD be able to receive RFC-1042 packets, intermixed with RFC-894 packets; and
*
*                            (c) MAY be able to send packets using RFC-1042 encapsulation.
*
*                 (3) REQUIREs the following network protocol files in network directories :
*
*                     (a) Packet-based Network Interface Layer located in the following network directory :
*
*                             \<Network Protocol Suite>\IF\
*
*                     (b) Address Resolution Protocol    Layer located in the following network directory :
*
*                             \<Network Protocol Suite>\
*
*                         See also 'net_arp.h  Note #1'.
*
*                              where
*                                      <Network Protocol Suite>    directory path for network protocol suite
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <net_if_pkt.h>                                        /* See 'net_if.h  Note #3a'.                            */
#include  <net_arp.h>                                           /* See 'net_if.h  Note #3b'.                            */


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) The following Ethernet-IF-module-present configuration value MUST be pre-#define'd in 
*               'net_cfg_net.h' PRIOR to all other network modules that require ARP Layer Configuration 
*               (see  'net_cfg_net.h  NETWORK INTERFACE LAYER CONFIGURATION  Note #4') :
*
*                   NET_IF_ETHER_MODULE_PRESENT
*********************************************************************************************************
*/

#ifdef   NET_IF_ETHER_MODULE_PRESENT                            /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_IF_MODULE
#define  NET_IF_EXT
#else
#define  NET_IF_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                NETWORK INTERFACE / ETHERNET DEFINES
*********************************************************************************************************
*/

#define  NET_IF_ADDR_MAC_PRESENT

#define  NET_IF_ADDR_SIZE                                  6    /* 48-bit MAC/net addr size.                            */
#define  NET_IF_ADDR_SIZE_MAC                            NET_IF_ADDR_SIZE


#define  NET_IF_ADDR_NULL                     0x000000000000
#define  NET_IF_ADDR_BROADCAST                0xFFFFFFFFFFFF


/*
*********************************************************************************************************
*                           NETWORK INTERFACE LAYER / ETHERNET ERROR CODES
*
* Note(s) : (1) ALL IF-independent    error codes #define'd in      'net_err.h';
*               ALL Ethernet-specific error codes #define'd in this 'net_if.h'.
*
*           (2) Network error code '11,000' series reserved for network interfaces.
*********************************************************************************************************
*/

#define  NET_IF_ERR_INVALID_MAC                        11400    /* Invalid MAC addr.                                    */
#define  NET_IF_ERR_INVALID_PROTOCOL                   11410    /* Invalid protocol.                                    */
#define  NET_IF_ERR_INVALID_ETHER_TYPE                 11411    /* Invalid Ethernet type.                               */
#define  NET_IF_ERR_INVALID_LLC_DSAP                   11420    /* Invalid IEEE 802.2 LLC  DSAP val.                    */
#define  NET_IF_ERR_INVALID_LLC_SSAP                   11421    /* Invalid IEEE 802.2 LLC  SSAP val.                    */
#define  NET_IF_ERR_INVALID_LLC_CTRL                   11422    /* Invalid IEEE 802.2 LLC  Ctrl val.                    */
#define  NET_IF_ERR_INVALID_SNAP_CODE                  11430    /* Invalid IEEE 802.2 SNAP OUI  val.                    */
#define  NET_IF_ERR_INVALID_SNAP_TYPE                  11431    /* Invalid IEEE 802.2 SNAP Type val.                    */


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NETWORK INTERFACE HEADER DEFINES
*
* Note(s) : (1) NET_IF_HDR_SIZE_MAX's ideal #define'tion :
*
*                   (A) max( Ether Hdr, IEEE 802 Hdr)
*
*               (a) However, since NET_IF_HDR_SIZE_MAX is used ONLY for network transmit & IEEE 802 is
*                   NEVER transmitted (see 'net_if.h  Note #2'), NET_IF_HDR_SIZE_MAX  MUST be #define'd
*                   with hard-coded knowledge that Ethernet is the only supported frame encapsulation 
*                   for network transmit.
*
*               (b) The following network interface values MUST be pre-#define'd in 'net_def.h' PRIOR to
*                   'net_cfg.h' so that the developer can configure the network interface for the correct
*                   network interface link layer values (see 'net_def.h  NETWORK INTERFACE LAYER DEFINES'
*                   & 'net_cfg_net.h  NETWORK INTERFACE LAYER CONFIGURATION') :
*
*                   (1) NET_IF_HDR_SIZE_ETHER                     14
*                   (2) NET_IF_HDR_SIZE_MAX                      NET_IF_HDR_SIZE_ETHER
*********************************************************************************************************
*/

#define  NET_IF_HDR_SIZE_BASE_ETHER                       14    /* Ethernet base hdr size.                              */
#define  NET_IF_HDR_SIZE_BASE_IEEE_802                     8    /* IEEE 802 base hdr size.                              */

#if 0                                                           /* See Note #1b1.                                       */
#define  NET_IF_HDR_SIZE_ETHER                           NET_IF_HDR_SIZE_BASE_ETHER
#endif
#define  NET_IF_HDR_SIZE_IEEE_802                       (NET_IF_HDR_SIZE_BASE_ETHER + NET_IF_HDR_SIZE_BASE_IEEE_802)

#define  NET_IF_HDR_SIZE_MIN                    (DEF_MIN(NET_IF_HDR_SIZE_ETHER, NET_IF_HDR_SIZE_IEEE_802))
#if 0                                                           /* See Notes #1a & #1b2.                                */
#define  NET_IF_HDR_SIZE_MAX                             NET_IF_HDR_SIZE_ETHER
#endif

                                                                /* See Note #1a.                                        */
#define  NET_IF_HDR_SIZE_TOT_MIN                         NET_IF_HDR_SIZE_ETHER
#define  NET_IF_HDR_SIZE_TOT_MAX                         NET_IF_HDR_SIZE_ETHER


#define  NET_IF_HDR_SIZE_ADDR                            NET_IF_ADDR_SIZE


/*$PAGE*/
/*
*********************************************************************************************************
*               NETWORK INTERFACE TOTAL LENGTH & MAXIMUM TRANSMISSION UNIT (MTU) DEFINES
*
* Note(s) : (1) NET_IF_MTU's ideal #define'tion :
*
*                   (A) min( Ether MTU, IEEE 802 MTU)
*
*               (a) However, since NET_IF_MTU is used by higher network protocol layers ONLY for network 
*                   transmit & IEEE 802 is NEVER transmitted (see 'net_if.h  Note #2'), NET_IF_MTU  MUST
*                   be #define'd with hard-coded knowledge that Ethernet is the only supported encapsulation 
*                   for network interface transmit.
*
*           (2) Ethernet & IEEE 802 minimum frame size is 64 octets (see RFC #894, Section 'Frame Format'
*               & RFC #1042, Section 'Frame Format and MAC Level Issues : For IEEE 802.3').  However, the
*               4-octet CRC trailer is included in this 64 octet minimum frame size.  Therefore, the minimum
*               size for receive & transmit packets is adjusted by the CRC size.
*
*           (3) Since network data value macro's appropriately convert data values from any CPU addresses,
*               word-aligned or not; network receive & transmit packets are NOT required to ensure that 
*               network packet headers (ARP/IP/UDP/TCP/etc.) & header members will locate on CPU word-
*               aligned addresses.  Therefore, network interface frame packets are NOT required to start
*               on any specific buffer indices.
*
*               See also 'net_util.h  NETWORK DATA VALUE MACRO'S           Note #2b'
*                      & 'net_buf.h   NETWORK BUFFER INDEX & SIZE DEFINES  Note #2'.
*********************************************************************************************************
*/

#define  NET_IF_MTU_ETHER                               1500
#define  NET_IF_MTU_IEEE_802                            1492
                                                                /* See Note #1.                                         */
#define  NET_IF_MTU_MIN                         (DEF_MIN(NET_IF_MTU_ETHER, NET_IF_MTU_IEEE_802))
#define  NET_IF_MTU_MAX                         (DEF_MAX(NET_IF_MTU_ETHER, NET_IF_MTU_IEEE_802))
#define  NET_IF_MTU                                      NET_IF_MTU_ETHER


#define  NET_IF_FRAME_HDR_SIZE                           NET_IF_HDR_SIZE_ETHER
#define  NET_IF_FRAME_HDR_SIZE_ETHER                     NET_IF_HDR_SIZE_ETHER
#define  NET_IF_FRAME_HDR_SIZE_IEEE_802                  NET_IF_HDR_SIZE_IEEE_802

#define  NET_IF_FRAME_CRC_SIZE                             4


#define  NET_IF_FRAME_MIN_CRC_SIZE                        64    /* See Note #2.                                         */
#define  NET_IF_FRAME_MIN_SIZE                          (NET_IF_FRAME_MIN_CRC_SIZE - NET_IF_FRAME_CRC_SIZE)

#define  NET_IF_FRAME_MAX_SIZE                          (NET_IF_MTU                + NET_IF_HDR_SIZE_MAX  )
#define  NET_IF_FRAME_MAX_CRC_SIZE                      (NET_IF_FRAME_MAX_SIZE     + NET_IF_FRAME_CRC_SIZE)


#define  NET_IF_PKT_SIZE_MIN                             NET_IF_FRAME_MIN_SIZE
#define  NET_IF_PKT_SIZE_MAX                             NET_IF_FRAME_MAX_SIZE


                                                                /* See Note #3.                                         */
#define  NET_IF_RX_IX                                    NET_BUF_DATA_RX_IX


/*$PAGE*/
/*
*********************************************************************************************************
*                                          ETHERNET DEFINES
*********************************************************************************************************
*/

                                                                /* ---------------- ETHER FRAME TYPES ----------------- */
#define  NET_IF_ETHER_FRAME_TYPE_IP                   0x0800
#define  NET_IF_ETHER_FRAME_TYPE_ARP                  0x0806
#define  NET_IF_ETHER_FRAME_TYPE_RARP                 0x8035    /* See 'net_def.h  NETWORK PROTOCOL TYPES  Note #1'.    */


/*
*********************************************************************************************************
*                                          IEEE 802 DEFINES
*
* Note(s) : (1) SNAP 'Organizational Unique Identifier' (OUI) abbreviated to 'SNAP' for some SNAP OUI
*               codes to enforce ANSI-compliance of 31-character symbol length uniqueness.
*
*           (2) Default SNAP 'Organizational Unique Identifier' (OUI) IEEE 802.2 frame type is ALWAYS
*               Ethernet frame type (see 'IEEE 802 HEADER / FRAME  Note #1').
*********************************************************************************************************
*/

#define  NET_IF_IEEE_802_FRAME_LEN_MAX                  (NET_IF_MTU_IEEE_802 + NET_IF_HDR_SIZE_BASE_IEEE_802)

                                                                /* ------- IEEE 802.2 LOGICAL LINK CONTROL (LLC) ------ */
#define  NET_IF_IEEE_802_LLC_DSAP                       0xAA
#define  NET_IF_IEEE_802_LLC_SSAP                       0xAA
#define  NET_IF_IEEE_802_LLC_CTRL                       0x03

                                                                /* --- IEEE 802.2 SUB-NETWORK ACCESS PROTOCOL (SNAP) -- */
#define  NET_IF_IEEE_802_SNAP_CODE_SIZE                    3    /*  3-octet SNAP org code         (see Note #1).        */
#define  NET_IF_IEEE_802_SNAP_CODE_ETHER            0x000000    /*    Dflt  SNAP org code (Ether) [see Note #2].        */
#define  NET_IF_IEEE_802_SNAP_CODE_00                   0x00    /*    Dflt  SNAP org code, Octet #00.                   */
#define  NET_IF_IEEE_802_SNAP_CODE_01                   0x00    /*    Dflt  SNAP org code, Octet #01.                   */
#define  NET_IF_IEEE_802_SNAP_CODE_02                   0x00    /*    Dflt  SNAP org code, Octet #02.                   */

#define  NET_IF_IEEE_802_SNAP_TYPE_IP                    NET_IF_ETHER_FRAME_TYPE_IP
#define  NET_IF_IEEE_802_SNAP_TYPE_ARP                   NET_IF_ETHER_FRAME_TYPE_ARP


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                             NETWORK INTERFACE HEADER / FRAME DATA TYPES
*********************************************************************************************************
*/

                                                                /* -------------------- NET IF HDR -------------------- */
typedef  struct  net_if_hdr {
    CPU_INT08U              AddrDest[NET_IF_HDR_SIZE_ADDR];     /* Ethernet dest  addr.                                 */
    CPU_INT08U              AddrSrc[NET_IF_HDR_SIZE_ADDR];      /* Ethernet src   addr.                                 */
    CPU_INT16U              FrameType_Len;                      /* Demux Ethernet frame type vs. IEEE 802.3 frame len.  */
} NET_IF_HDR;


/*
*********************************************************************************************************
*                                 ETHERNET HEADER / FRAME DATA TYPES
*
* Note(s) : (1) Frame 'Data' buffer CANNOT be declared to force word-alignment.  'Data' buffer MUST immediately
*               follow frame 'Hdr' since Ethernet frames are contiguous, non-aligned data packets.
*
*           (2) 'Data' declared with 1 entry; prevents removal by compiler optimization.
*
*           (3) Frame CRC's are computed/validated by NIC.  NO software CRC is handled for receive or transmit.
*********************************************************************************************************
*/

                                                                /* ----------------- NET IF ETHER HDR ----------------- */
typedef  struct  net_if_hdr_ether {
    CPU_INT08U              AddrDest[NET_IF_HDR_SIZE_ADDR];     /* Ethernet dest  addr.                                 */
    CPU_INT08U              AddrSrc[NET_IF_HDR_SIZE_ADDR];      /* Ethernet src   addr.                                 */
    CPU_INT16U              FrameType;                          /* Ethernet frame type.                                 */
} NET_IF_HDR_ETHER;


                                                                /* ---------------- NET IF ETHER FRAME ---------------- */
typedef  struct  net_if_frame_ether {
    NET_IF_HDR_ETHER        Hdr;
    CPU_INT08U              Data[1];                            /* See Note #2.                                         */
} NET_IF_FRAME_ETHER;


/*$PAGE*/
/*
*********************************************************************************************************
*                                 IEEE 802 HEADER / FRAME DATA TYPES
*
* Note(s) : (1) Header 'SNAP_OrgCode' defines the SNAP 'Organizational Unique Identifier' (OUI).  The OUI
*               indicates the various organization/vendor/manufacturer with each organization then defining
*               their respective frame types.
*
*               However, the default SNAP OUI indicates Ethernet frame types & is ALWAYS used.  ALL other 
*               OUI's are discarded as invalid.
*
*               See also 'IEEE 802 DEFINES  Notes #1 & #2'.
*
*           (2) Frame 'Data' buffer CANNOT be declared to force word-alignment.  'Data' buffer MUST immediately
*               follow frame 'Hdr' since Ethernet frames are contiguous, non-aligned data packets.
*
*           (3) 'Data' declared with 1 entry; prevents removal by compiler optimization.
*
*           (4) Frame CRC's are computed/validated by NIC.  NO software CRC is handled for receive or transmit.
*********************************************************************************************************
*/

                                                                /* --------------- NET IF IEEE 802 HDR ---------------- */
typedef  struct  net_if_hdr_ieee_802 {
    CPU_INT08U              AddrDest[NET_IF_HDR_SIZE_ADDR];     /* IEEE 802.3 dest  addr.                               */
    CPU_INT08U              AddrSrc[NET_IF_HDR_SIZE_ADDR];      /* IEEE 802.3 src   addr.                               */
    CPU_INT16U              FrameLen;                           /* IEEE 802.3 frame len.                                */

                                                                /* ------ IEEE 802.2 LOGICAL LINK CONTROL (LLC) ------- */
    CPU_INT08U              LLC_DSAP;                           /* Dest Serv Access Pt.                                 */
    CPU_INT08U              LLC_SSAP;                           /* Src  Serv Access Pt.                                 */
    CPU_INT08U              LLC_Ctrl;                           /* Ctrl Field.                                          */

                                                                /* -- IEEE 802.2 SUB-NETWORK ACCESS PROTOCOL (SNAP) --- */
    CPU_INT08U              SNAP_OrgCode[NET_IF_IEEE_802_SNAP_CODE_SIZE];   /* Org code (see Note #1).                  */
    CPU_INT16U              SNAP_FrameType;                                 /* IEEE 802.2 frame type.                   */
} NET_IF_HDR_IEEE_802;


                                                                /* -------------- NET IF IEEE 802 FRAME --------------- */
typedef  struct  net_if_frame_ieee_802 {
    NET_IF_HDR_IEEE_802     Hdr;
    CPU_INT08U              Data[1];                            /* See Note #3.                                         */
} NET_IF_FRAME_IEEE_802;


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

NET_IF_EXT  CPU_INT08U   NetIF_MAC_Addr[NET_IF_ADDR_SIZE];      /* NIC's MAC addr.                                      */
NET_IF_EXT  CPU_BOOLEAN  NetIF_MAC_AddrValid;                   /* Indicates if valid NIC MAC addr avail.               */


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         NetIF_Init          (NET_ERR     *perr);



                                                                /* ------------------- STATUS FNCTS ------------------- */
void         NetIF_MAC_AddrGet   (CPU_INT08U  *paddr,
                                  NET_ERR     *perr);

void         NetIF_MAC_AddrSet   (CPU_INT08U  *paddr,
                                  NET_ERR     *perr);


CPU_BOOLEAN  NetIF_IsValidPktSize(CPU_INT16U   size);           /* Validate an IF pkt size.                             */

CPU_BOOLEAN  NetIF_IsValidAddrSrc(CPU_INT08U  *paddr_src);      /* Validate an IF src addr.                             */



                                                                /* --------------------- RX FNCTS --------------------- */
void         NetIF_Rx            (NET_BUF     *pbuf,
                                  NET_ERR     *perr);


                                                                /* --------------------- TX FNCTS --------------------- */
void         NetIF_Tx            (NET_BUF     *pbuf,
                                  NET_ERR     *perr);

void         NetIF_TxPkt         (NET_BUF     *pbuf,
                                  NET_ERR     *perr);


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_IF_CFG_TYPE
#error  "NET_IF_CFG_TYPE                  not #define'd in 'net_cfg.h'"
#error  "                           [MUST be  NET_IF_TYPE_ETHER]      "
#elif   (NET_IF_CFG_TYPE != NET_IF_TYPE_ETHER)
#error  "NET_IF_CFG_TYPE            illegally #define'd in 'net_cfg.h'"
#error  "                           [MUST be  NET_IF_TYPE_ETHER]      "

#else
                                                            /* Correctly configured in 'net_cfg_net.h'; DO NOT MODIFY.  */
#ifndef  NET_IF_HDR_SIZE_MAX
#error  "NET_IF_HDR_SIZE_MAX        not #define'd in 'net_cfg_net.h'  "
#error  "                           [NET_IF_HDR_SIZE_MAX  MUST be > 0]"
#elif   (NET_IF_HDR_SIZE_MAX < 1)
#error  "NET_IF_HDR_SIZE_MAX  illegally #define'd in 'net_cfg_net.h'  "
#error  "                           [NET_IF_HDR_SIZE_MAX  MUST be > 0]"
#endif

#endif


                                                            /* Correctly configured in this 'net_if.h'; DO NOT MODIFY.  */
#ifndef  NET_IF_FRAME_MIN_SIZE
#error  "NET_IF_FRAME_MIN_SIZE            not #define'd in 'net_if.h'  "
#error  "                           [NET_IF_FRAME_MIN_SIZE  MUST be > ]"
#error  "                           [NET_IF_HDR_SIZE_MAX              ]"
#elif   (NET_IF_FRAME_MIN_SIZE < NET_IF_HDR_SIZE_MAX)
#error  "NET_IF_FRAME_MIN_SIZE      illegally #define'd in 'net_if.h'  "
#error  "                           [NET_IF_FRAME_MIN_SIZE  MUST be > ]"
#error  "                           [NET_IF_HDR_SIZE_MAX              ]"
#endif




#ifndef  NET_IF_CFG_ADDR_FLTR_EN
#error  "NET_IF_CFG_ADDR_FLTR_EN          not #define'd in 'net_cfg.h'"
#error  "                           [MUST be  DEF_DISABLED]           "
#error  "                           [     ||  DEF_ENABLED ]           "
#elif  ((NET_IF_CFG_ADDR_FLTR_EN != DEF_DISABLED) && \
        (NET_IF_CFG_ADDR_FLTR_EN != DEF_ENABLED ))
#error  "NET_IF_CFG_ADDR_FLTR_EN    illegally #define'd in 'net_cfg.h'"
#error  "                           [MUST be  DEF_DISABLED]           "
#error  "                           [     ||  DEF_ENABLED ]           "
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See this 'net_if.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of Ether IF module include (see Note #1).            */

