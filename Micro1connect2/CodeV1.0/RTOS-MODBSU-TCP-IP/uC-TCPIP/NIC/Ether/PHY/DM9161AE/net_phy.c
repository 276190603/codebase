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
* Filename      : net_phy.c
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

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <net.h>
#include  <net_phy.h>
#include  <net_phy_def.h>

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         NetNIC_PhyInit()
*
* Description : Initialize phyter (ethernet link controller)
*               This instance configures the Davicom DM9161AE PHY
*
* Argument(s) : none.
*
* Return(s)   : 1 for OK, 0 for error
*
* Caller(s)   : EMAC_Init()
*
* Note(s)     : Assumes the MDI port as already been enabled for the PHY.
*********************************************************************************************************
*/

void  NetNIC_PhyInit (NET_ERR *perr)
{
    volatile  CPU_INT16U  reg_val;


#if (!defined(EMAC_CFG_RMII)) || (EMAC_CFG_RMII <= 0)
    reg_val     =  NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMCR, perr);  /* Read the Basic Mode Control Register (twice)             */
    reg_val     =  NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMCR, perr);  /* Read the Basic Mode Control Register (twice)             */

    if (*perr  != NET_PHY_ERR_NONE) {
        return;
    }

    reg_val    &= ~BMCR_ISOLATE;                                        /* Mask off 'Disconnect from MII bit (BMCR_ISOLATE)         */
    NetNIC_PhyRegWr(EMAC_CFG_PHY_ADDR, MII_BMCR, reg_val, perr);        /* Put the PHY into MII mode                                */

    if (*perr  != NET_PHY_ERR_NONE) {
        return;
    }
#endif

    NetNIC_PhyAutoNeg();                                                /* Perform auto-negotiation                                 */

    NetNIC_ConnStatus = NetNIC_PhyLinkState();                          /* Set NetNIC_ConnStatus according to link state            */

    if (NetNIC_ConnStatus == DEF_ON) {
        NetNIC_LinkUp();
    } else {
        NetNIC_LinkDown();
    }

#if (EMAC_CFG_PHY_INT == DEF_ON)
    NetNIC_PhyIntInit();

    reg_val     = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, DM9161_MDINTR, perr); /* Clear interrupts                                         */
    reg_val    |= MDINTER_FDX_MSK | MDINTER_SPD_MSK | MDINTER_LINK_MSK | MDINTER_INTR_MSK;
    reg_val    &= ~(MDINTER_SPD_MSK | MDINTER_LINK_MSK | MDINTER_INTR_MSK);

    NetNIC_PhyRegWr(EMAC_CFG_PHY_ADDR, DM9161_MDINTR, reg_val, perr);   /* Enable link change interrupt                             */
#endif
}


/*
*********************************************************************************************************
*                                        NetNIC_PhyAutoNeg()
*
* Description : Do link auto-negotiation
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : NetNIC_PhyInit().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetNIC_PhyAutoNeg (void)
{
    CPU_INT16U   i;
    CPU_INT16U   reg_val;
    CPU_BOOLEAN  link;
    NET_ERR      err;


    i               = DM9161AE_INIT_AUTO_NEG_RETRIES;                   /* Set the # of retries before declaring a timeout  */
    link            = NetNIC_PhyLinkState();                            /* Get the current link state. 1=linked, 0=no link  */

    if (link == DEF_OFF) {
#if (!defined(EMAC_CFG_RMII)) || (EMAC_CFG_RMII <= 0)
        reg_val     = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMCR, &err);   /* Get current control register value           */
        reg_val    |= DEF_BIT_09;                                       /* Set the auto-negotiation start bit               */

        NetNIC_PhyRegWr(EMAC_CFG_PHY_ADDR, MII_BMCR, reg_val, &err);       /* Initiate auto-negotiation                        */
#endif

        do {                                                            /* Do while auto-neg incomplete, or retries expired */
            DM9161AE_DlyAutoNegAck();                                   /* Wait for a while auto-neg to proceed (net_bsp.c) */
            reg_val = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMSR, &err);   /* Read the Basic Mode Status Register          */
            reg_val = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMSR, &err);   /* Read the Basic Mode Status Register          */
            i--;
        } while (((reg_val & BMSR_LSTATUS) == 0) && (i > 0));           /* While link not established and retries remain    */
    }
}


/*
*********************************************************************************************************
*                                    NetNIC_PhyAutoNegState()
*
* Description : Returns state of auto-negotiation
*               This instance probe the Davicom DM9161AE '3.3V Dual-Speed Fast Ethernet Transceiver'
*
* Argument(s) : none.
*
* Return(s)   : State of auto-negociation (DEF_OFF = not completed, DEF_ON = completed).
*
* Caller(s)   : NetNIC_PhyInit().
*
* Note(s)     : If any error is encountered while reading the PHY, this function
*               will return Auto Negotiation State = DEF_OFF (incomplete).
*********************************************************************************************************
*/

CPU_BOOLEAN  NetNIC_PhyAutoNegState (void)
{
    CPU_INT32U  reg_val;
    NET_ERR     err;


    reg_val     = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMSR, &err);
    reg_val     = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMSR, &err);

    if (err   != NET_PHY_ERR_NONE) {
        reg_val = 0;
    }

    if ((reg_val & BMSR_ANEGCOMPLETE) == BMSR_ANEGCOMPLETE) {   /* DM9161AE register 0x01: Basic Status Register #1      */
        return (DEF_ON);                                        /* BIT 5 Signal the result of the auto negotiation       */
    } else {                                                    /* 1 = complete, 0 = incomplete                          */
        return (DEF_OFF);
    }
}


/*
*********************************************************************************************************
*                                     NetNIC_PhyLinkState()
*
* Description : Returns state of ethernet link
*               This instance probe the Davicom DM9161AE '3.3V Dual-Speed Fast Ethernet Transceiver'
*
* Argument(s) : none.
*
* Return(s)   : State of ethernet link (DEF_OFF = link down, DEF_ON = link up).
*
* Caller(s)   : NetNIC_PhyISR_Handler().
*
* Note(s)     : If any error is encountered while reading the PHY, this function
*               will return link state = DEF_OFF.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetNIC_PhyLinkState (void)
{
    NET_ERR     err;
    CPU_INT16U  reg_val;

                                                                /* DM9161AE register 0x01: Basic Status Register #1      */
                                                                /* BIT 2 , Link Status, 1 = linked, 0 = not linked.      */
    reg_val      = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMSR, &err);
    reg_val      = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMSR, &err);

    if (err   != NET_PHY_ERR_NONE) {
        reg_val  = 0;
    }

    reg_val     &= BMSR_LSTATUS;

    if (reg_val == BMSR_LSTATUS) {
        return (DEF_ON);
    } else {
        return (DEF_OFF);
    }
}


/*
*********************************************************************************************************
*                                     NetPHY_GetLinkSpeed()
*
* Description : Returns the speed of the current Ethernet link
*               This probes the Davicom DM9161AE '3.3V Dual-Speed Fast Ethernet Transceiver'
*
* Argument(s) : none.
*
* Return(s)   : 0 = No Link, 10 = 10mbps, 100 = 100mbps
*
* Caller(s)   : EMAC_Init()
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  NetNIC_PhyLinkSpeed (void)
{
	CPU_INT32U  bmsr;
    CPU_INT32U  bmcr;
    CPU_INT32U  lpa;
    NET_ERR     err;


    bmsr    = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMSR, &err);       /* Get Link Status from PHY status reg. Requires 2 reads    */
	bmsr    = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMSR, &err);       /* Get Link Status from PHY status reg. Requires 2 reads    */

    if ((bmsr & BMSR_LSTATUS) == 0) {
        return (NET_PHY_SPD_0);                                         /* No link                                                  */
    }

	bmcr    = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMCR, &err);       /* Read the PHY Control Register                            */
	
    if ((bmcr & BMCR_ANENABLE) == BMCR_ANENABLE) {		                /* If AutoNegotiation is enabled                            */
        if ((bmsr & BMSR_ANEGCOMPLETE) == 0) {                          /* If AutoNegotiation is not complete                       */
			return (NET_PHY_SPD_0);       					            /* AutoNegotitation in progress                             */
        }

		lpa = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_ANLPAR, &err);     /* Read the Link Partner Ability Register                   */

        if (((lpa & ANLPAR_100FULL) == ANLPAR_100FULL) || ((lpa & ANLPAR_100HALF) == ANLPAR_100HALF)) {
			return (NET_PHY_SPD_100);
        } else {
			return (NET_PHY_SPD_10);
        }
	} else {                                                            /* Auto-negotiation not enabled, get speed from BMCR        */
        if ((bmcr & BMCR_SPEED100) == BMCR_SPEED100) {
            return (NET_PHY_SPD_100);
        } else {
            return (NET_PHY_SPD_10);
        }
	}
}


/*
*********************************************************************************************************
*                                     NetPHY_GetDuplex()
*
* Description : Returns the duplex mode of the current Ethernet link
*               This probes the Davicom DM9161AE '3.3V Dual-Speed Fast Ethernet Transceiver'
*
* Argument(s) : none.
*
* Return(s)   : 0 = Unknown (Auto-Neg in progress), 1 = Half Duplex, 2 = Full Duplex
*
* Caller(s)   : EMAC_Init()
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  NetNIC_PhyLinkDuplex (void)
{
    CPU_INT32U  bmcr;
    CPU_INT32U  bmsr;
    CPU_INT32U  lpa;
    NET_ERR     err;


    bmsr    = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMSR, &err);       /* Get Link Status from PHY status reg. Requires 2 reads    */
	bmsr    = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMSR, &err);       /* Get Link Status from PHY status reg. Requires 2 reads    */


    if ((bmsr & BMSR_LSTATUS) == 0) {
        return (NET_PHY_DUPLEX_UNKNOWN);                                /* No link, return 'Duplex Uknown'                          */
    }

	bmcr    = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_BMCR, &err);       /* Read the PHY Control Register                            */
	
    if ((bmcr & BMCR_ANENABLE) == BMCR_ANENABLE) {		                /* If AutoNegotiation is enabled                            */
        if ((bmsr & BMSR_ANEGCOMPLETE) == 0) {                          /* If AutoNegotiation is not complete                       */
			return (NET_PHY_DUPLEX_UNKNOWN);      			            /* AutoNegotitation in progress                             */
        }

		lpa = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, MII_ANLPAR, &err);     /* Read the Link Partner Ability Register                   */

        if (((lpa & ANLPAR_100FULL) == ANLPAR_100FULL) || ((lpa & ANLPAR_10FULL) == ANLPAR_10FULL)) {
			return (NET_PHY_DUPLEX_FULL);
        } else {
            return (NET_PHY_DUPLEX_HALF);
        }
	} else {                                                            /* Auto-negotiation not enabled, get duplex from BMCR       */
        if ((bmcr & BMCR_FULLDPLX) == BMCR_FULLDPLX) {
            return (NET_PHY_DUPLEX_FULL);
        } else {
            return (NET_PHY_DUPLEX_HALF);
        }
	}
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                    GLOBAL FUNCTIONS: PHY INTERRUPTS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        NetNIC_ISR_Handler()
*
* Description : (1) Update NetNIC_ConnStatus according to link state
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

#if (EMAC_CFG_PHY_INT == DEF_ON)
void  NetNIC_PhyISR_Handler (void)
{
    volatile  CPU_INT16U  reg_val;
              NET_ERR     err;


    NetNIC_PhyIntEnter();

    NetNIC_ConnStatus = NetNIC_PhyLinkState();                          /* Set NetNIC_ConnStatus according to link state            */

    if (NetNIC_ConnStatus == DEF_ON) {
        NetNIC_LinkUp();
    } else {
        NetNIC_LinkDown();
    }

    reg_val     = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, DM9161_MDINTR, &err);  /* Clear interrupts                                     */

    NetNIC_PhyIntClr();
}
#endif
