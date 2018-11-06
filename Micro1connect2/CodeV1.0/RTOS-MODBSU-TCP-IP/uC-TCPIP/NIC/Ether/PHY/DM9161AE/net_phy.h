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
*               Network Physical Layer (PHY) port files provided, as is, for FREE and do
*               NOT require any additional licensing or licensing fee.
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
*                                        NETWORK PHYSICAL LAYER
*
*                                               DM9161AE
*
* Filename      : net_phy.h
* Version       : V1.92
* Programmer(s) : EHS
*********************************************************************************************************
* Note(s)       : (1) Supports DM9161AE '3.3V Dual-Speed Fast Ethernet Transceiver' as described in
*
*                         Corporation (INTEL; http://www.intel.com)
*                         (a) DM9161AE       (DM9161AE; Revision 249414-002)
*
*                 (2) The MII interface port is assumed to be part of the host EMAC; consequently,
*                     reads from and writes to the PHY are made through the EMAC.  The functions
*                     NetNIC_PhyRegRd() and NetNIC_PhyRegWr(), which are used to access the PHY, should
*                     be provided in the EMAC driver.
*********************************************************************************************************
*/

#ifndef _NET_PHY_H
#define _NET_PHY_H

/*
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  DM9161AE_INIT_AUTO_NEG_RETRIES        3

#define  DM9161AE_OUI                   0x00606E
#define  DM9161AE_VNDR_MDL                  0x08

/*
*********************************************************************************************************
*                                       DM9161AE REGISTER DEFINES
*********************************************************************************************************
*/
                                                                /* ------- Generic MII registers ---------- */
#define  MII_BMCR                           0x00                /* Basic mode control register              */
#define  MII_BMSR                           0x01                /* Basic mode status register               */
#define  MII_PHYSID1                        0x02                /* PHYS ID 1                                */
#define  MII_PHYSID2                        0x03                /* PHYS ID 2                                */
#define  MII_ANAR                           0x04                /* Advertisement control reg                */
#define  MII_ANLPAR                         0x05                /* Link partner ability reg                 */
#define  MII_ANER                           0x06                /* Expansion register                       */

                                                                /* ---------- Extended registers ---------- */
#define  DM9161_DSCR                        0x10                /* DAVICOM specified configuration          */
#define  DM9161_DSCSR                       0x11                /* DAVICOM specified configuration & status */
#define  DM9161_PWDOR                       0x13                /* Power down control register              */
#define  DM9161_SCR                         0x14                /* Specified configuration                  */
#define  DM9161_MDINTR                      0x15                /* DAVICOM Specified interrupt register     */
#define  DM9161_RECR                        0x16                /* DAVICOM specified RX error counter reg.  */
#define  DM9161_DISCR                       0x17                /* DAVICOM specified disconnect cntr. reg.  */
#define  DM9161_RLSR                        0x18                /* DAVICOM hardware reset latch state reg.  */

/*
*********************************************************************************************************
*                                         DM9161AE REGISTER BITS
*********************************************************************************************************
*/
                                                                /* -------- MII_BMCR Register Bits -------- */
#define  BMCR_RESV                         0x007F               /* Unused...                                */
#define  BMCR_CTST                       DEF_BIT_07             /* Collision test                           */
#define  BMCR_FULLDPLX                   DEF_BIT_08             /* Full duplex                              */
#define  BMCR_ANRESTART                  DEF_BIT_09             /* Auto negotiation restart                 */
#define  BMCR_ISOLATE                    DEF_BIT_10             /* Disconnect DP83840 from MII              */
#define  BMCR_PDOWN                      DEF_BIT_11             /* Powerdown the DP83840                    */
#define  BMCR_ANENABLE                   DEF_BIT_12             /* Enable auto negotiation                  */
#define  BMCR_SPEED100                   DEF_BIT_13             /* Select 100Mbps                           */
#define  BMCR_LOOPBACK                   DEF_BIT_14             /* TXD loopback bits                        */
#define  BMCR_RESET                      DEF_BIT_15             /* Reset the DP83840                        */

                                                                /* -------- MII_BMSR Register Bits -------- */
#define  BMSR_ERCAP                      DEF_BIT_00             /* Ext-reg capability                       */
#define  BMSR_JCD                        DEF_BIT_01             /* Jabber detected                          */
#define  BMSR_LSTATUS                    DEF_BIT_02             /* Link status                              */
#define  BMSR_ANEGCAPABLE                DEF_BIT_03             /* Able to do auto-negotiation              */
#define  BMSR_RFAULT                     DEF_BIT_04             /* Remote fault detected                    */
#define  BMSR_ANEGCOMPLETE               DEF_BIT_05             /* Auto-negotiation complete                */
#define  BMSR_RESV                         0x07C0               /* Unused...                                */
#define  BMSR_10HALF                     DEF_BIT_11             /* Can do 10mbps, half-duplex               */
#define  BMSR_10FULL                     DEF_BIT_12             /* Can do 10mbps, full-duplex               */
#define  BMSR_100HALF                    DEF_BIT_13             /* Can do 100mbps, half-duplex              */
#define  BMSR_100FULL                    DEF_BIT_14             /* Can do 100mbps, full-duplex              */
#define  BMSR_100BASE4                   DEF_BIT_15             /* Can do 100mbps, 4k packets               */

                                                                /* -------- MII_ANAR Register Bits -------- */
#define  ANAR_SLCT                         0x001F               /* Selector bits                            */
#define  ANAR_CSMA                       DEF_BIT_04             /* Only selector supported                  */
#define  ANAR_10HALF                     DEF_BIT_05             /* Try for 10mbps half-duplex               */
#define  ANAR_10FULL                     DEF_BIT_06             /* Try for 10mbps full-duplex               */
#define  ANAR_100HALF                    DEF_BIT_07             /* Try for 100mbps half-duplex              */
#define  ANAR_100FULL                    DEF_BIT_08             /* Try for 100mbps full-duplex              */
#define  ANAR_100BASE4                   DEF_BIT_09             /* Try for 100mbps 4k packets               */
#define  ANAR_RESV                         0x1C00               /* Unused...                                */
#define  ANAR_RFAULT                     DEF_BIT_13             /* Say we can detect faults                 */
#define  ANAR_LPACK                      DEF_BIT_14             /* Ack link partners response               */
#define  ANAR_NPAGE                      DEF_BIT_15             /* Next page bit                            */

#define  ANAR_FULL       (ANAR_100FULL | ANAR_10FULL | ANAR_CSMA)
#define  ANAR_ALL        (ANAR_100FULL | ANAR_10FULL | ANAR_100HALF | ANAR_10HALF)

                                                                /* ------- MII_ANLPAR Register Bits ------- */
#define  ANLPAR_SLCT                       0x001F               /* Same as advertise selector               */
#define  ANLPAR_10HALF                   DEF_BIT_05             /* Can do 10mbps half-duplex                */
#define  ANLPAR_10FULL                   DEF_BIT_06             /* Can do 10mbps full-duplex                */
#define  ANLPAR_100HALF                  DEF_BIT_07             /* Can do 100mbps half-duplex               */
#define  ANLPAR_100FULL                  DEF_BIT_08             /* Can do 100mbps full-duplex               */
#define  ANLPAR_100BASE4                 DEF_BIT_09             /* Can do 100mbps 4k packets                */
#define  ANLPAR_RESV                       0x1C00               /* Unused...                                */
#define  ANLPAR_RFAULT                   DEF_BIT_13             /* Link partner faulted                     */
#define  ANLPAR_LPACK                    DEF_BIT_14             /* Link partner acked us                    */
#define  ANLPAR_NPAGE                    DEF_BIT_15             /* Next page bit                            */

#define  ANLPAR_DUPLEX   (ANLPAR_10FULL  | ANLPAR_100FULL)
#define  ANLPAR_100	     (ANLPAR_100FULL | ANLPAR_100HALF | ANLPAR_100BASE4)

                                                                /* -------- MII_ANER Register Bits -------- */
#define  ANER_NWAY                       DEF_BIT_00             /* Can do N-way auto-nego                   */
#define  ANER_LCWP                       DEF_BIT_01             /* Got new RX page code word                */
#define  ANER_ENABLENPAGE                DEF_BIT_02             /* This enables npage words                 */
#define  ANER_NPCAPABLE                  DEF_BIT_03             /* Link partner supports npage              */
#define  ANER_MFAULTS                    DEF_BIT_04             /* Multiple faults detected                 */
#define  ANER_RESV                         0xFFE0               /* Unused...                                */

                                                                /* ------ DM9161_MDINTR Register Bits ----- */
#define  MDINTR_INTR_PEND                DEF_BIT_15             /* Interrupt pending                        */
#define  MDINTER_FDX_MSK                 DEF_BIT_11             /* Full-duplex interrupt mask               */
#define  MDINTER_SPD_MSK                 DEF_BIT_10             /* Speed       interrupt mask               */
#define  MDINTER_LINK_MSK                DEF_BIT_09             /* Link        interrupt mask               */
#define  MDINTER_INTR_MSK                DEF_BIT_08             /* Master      interrupt mask               */
#define  MDINTER_FDX_CHG                 DEF_BIT_04             /* Full-duplex change                       */
#define  MDINTER_SPD_CHG                 DEF_BIT_03             /* Speed       change                       */
#define  MDINTER_LINK_CHG                DEF_BIT_02             /* Link        change                       */
#define  MDINTER_INTR_STAT               DEF_BIT_00             /* Interrupt status                         */

/*
*********************************************************************************************************
*                                   PHY ERROR CODES 12,000 -> 13,000
*********************************************************************************************************
*/

#define  NET_PHY_ERR_REGRD_TIMEOUT         12010
#define  NET_PHY_ERR_REGWR_TIMEOUT         12020
#define  NET_PHY_ERR_AUTONEG_TIMEOUT       12030


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         NetNIC_PhyInit          (NET_ERR      *perr);

void         NetNIC_PhyAutoNeg       (void);                    /* Do link auto-negotiation                             */

#if (EMAC_CFG_PHY_INT == DEF_ON)
void         NetNIC_PhyISR_Handler   (void);                    /* Decode & handle PHY ISRs.                            */
#endif
                                                                /* -----------------PHY STATUS FNCTS ------------------ */
CPU_BOOLEAN  NetNIC_PhyAutoNegState  (void);                    /* Get PHY auto-negotiation state                       */

CPU_BOOLEAN  NetNIC_PhyLinkState     (void);                    /* Get PHY link state                                   */

CPU_INT32U   NetNIC_PhyLinkSpeed     (void);                    /* Get PHY link speed                                   */

CPU_INT32U   NetNIC_PhyLinkDuplex    (void);                    /* Get PHY duplex mode                                  */


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                    DEFINED IN PRODUCT'S  net_bsp.c
*********************************************************************************************************
*/

void         NetNIC_LinkUp           (void);                    /* Message from NIC that the ethernet link is up.       */
                                                                /* Called in interruption context most of the time.     */

void         NetNIC_LinkDown         (void);                    /* Message from NIC that the ethernet link is down.     */
                                                                /* Called in interruption context most of the time.     */

void         DM9161AE_DlyAutoNegAck  (void);                    /* Implement 1.5 s dly.                                 */
                                                                /*   MUST use OS dly to preempt.                        */

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                    DEFINED IN PRODUCT'S  net_isr.c
*********************************************************************************************************
*/

#if (NET_NIC_CFG_INT_CTRL_EN == DEF_ENABLED)
#if (EMAC_CFG_PHY_INT == DEF_ON)
void         NetNIC_PhyIntInit       (void);                    /* Init int ctrl'r.                                     */

void         NetNIC_PhyIntClr        (void);                    /* Clr  int ctrl'r src(s).                              */

void         NetNIC_PhyIntEnter      (void);                    /* Enter ISR              [see Note #2].                */
#endif
#endif


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_NIC_CFG_INT_CTRL_EN
#error  "NET_NIC_CFG_INT_CTRL_EN           not #define'd in 'net_cfg.h'"
#error  "                            [MUST be  DEF_DISABLED]           "
#error  "                            [     ||  DEF_ENABLED ]           "
#elif  ((NET_NIC_CFG_INT_CTRL_EN != DEF_DISABLED) && \
        (NET_NIC_CFG_INT_CTRL_EN != DEF_ENABLED ))
#error  "NET_NIC_CFG_INT_CTRL_EN     illegally #define'd in 'net_cfg.h'"
#error  "                            [MUST be  DEF_DISABLED]           "
#error  "                            [     ||  DEF_ENABLED ]           "
#endif

#ifndef   EMAC_CFG_PHY_ADDR
#error   "EMAC_CFG_PHY_ADDR           not #define'd in 'net_bsp.h'"
#endif

#ifndef   EMAC_CFG_RMII
#error   "EMAC_CFG_RMII               not #define'd in 'net_bsp.h'"
#error   "                            [MUST be  DEF_YES   ]"
#error   "                            [     ||  DEF_NO    ]"
#elif   ((EMAC_CFG_RMII != DEF_YES) && \
         (EMAC_CFG_RMII != DEF_NO ))
#error   "EMAC_CFG_RMII               illegally #define'd in 'net_bsp.h'"
#error   "                            [MUST be  DEF_YES]"
#error   "                            [     ||  DEF_NO ]"
#endif




#endif
