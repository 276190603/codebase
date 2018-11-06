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
*               Network Interface Card (NIC) port files provided, as is, for FREE and do
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
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   NET_NIC_MODULE
#include  <net.h>
#include  "stm32_eth.h"

extern    void ETH_GetDMATxDesc(void);
extern    void LOW_LEVEL_Init(void);
extern    void Ethernet_Configuration(void);

extern    u16  LOW_LEVEL_Input(u8 *buffer);
extern    void LOW_LEVEL_Output(u8 *data, u16 len);

/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

typedef struct {
    CPU_INT32U  addr;                                                   /* Address of RX buffer.                                */
    CPU_INT32U  status;                                                 /* Status of RX buffer.                                 */
}  NIC_BUF_DESC;


/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void        NetNIC_TxPktDiscard (NET_ERR     *perr);

static  void        MACB_TxPktPrepare   (void        *ppkt,
                                         CPU_INT16U   size,
                                         NET_ERR     *perr);

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             NetNIC_Init()
*
* Description : (1) Initialize Network Interface Card :
*                   (a) Perform NIC Layer OS initialization
*                   (b) Initialize NIC status
*                   (c) Initialize NIC statistics & error counters
*                   (d) Initialize MACB
*
* Argument(s) : perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                    Network interface card successfully initialized.
*
*                                                                   -------- RETURNED BY NetOS_NIC_Init() : --------
*                               NET_OS_ERR_INIT_NIC_TX_RDY          NIC transmit ready signal NOT successfully
*                                                                       initialized.
*                               NET_OS_ERR_INIT_NIC_TX_RDY_NAME     NIC transmit ready name   NOT successfully
*                                                                       configured.
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*********************************************************************************************************
*/

void  NetNIC_Init (NET_ERR  *perr)
{
                                                                        /* --------------- PERFORM NIC/OS INIT ---------------  */
    NetOS_NIC_Init(perr);                                               /* Create NIC objs.                                     */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

                                                                        /* ----------------- INIT NIC STATUS -----------------  */
    NetNIC_ConnStatus           =  DEF_OFF;

                                                                        /* ------------- INIT NIC STAT & ERR CTRS ------------  */
#if (NET_CTR_CFG_STAT_EN        == DEF_ENABLED)
    NetNIC_StatRxPktCtr         =  0;
    NetNIC_StatTxPktCtr         =  0;
#endif

#if (NET_CTR_CFG_ERR_EN         == DEF_ENABLED)
    NetNIC_ErrRxPktDiscardedCtr =  0;
    NetNIC_ErrTxPktDiscardedCtr =  0;
#endif

    *perr = NET_NIC_ERR_NONE;
}


/*
*********************************************************************************************************
*                                            NetNIC_IntEn()
*
* Description : Enable NIC interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*********************************************************************************************************
*/

void  NetNIC_IntEn (NET_ERR  *perr)
{  
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif

    CPU_CRITICAL_ENTER();
    
    CPU_CRITICAL_EXIT();

    *perr = NET_NIC_ERR_NONE;
}

/*
*********************************************************************************************************
*                                        NetNIC_ConnStatusGet()
*
* Description : Get NIC's network connection status.
*
*               (1) Obtaining the NIC's network connection status is encapsulated in this function for the
*                   possibility that obtaining a NIC's connection status requires a non-trivial procedure.
*
*
* Argument(s) : none.
*
* Return(s)   : NIC network connection status :
*
*                   DEF_OFF     Network connection DOWN.
*                   DEF_ON      Network connection UP.
*
* Caller(s)   : NetIF_Pkt_Tx().
*********************************************************************************************************
*/

CPU_BOOLEAN  NetNIC_ConnStatusGet (void)
{
    NetNIC_ConnStatus = DEF_ON;
  
    return (NetNIC_ConnStatus);
}

/*
*********************************************************************************************************
*                                         NetNIC_ISR_Handler()
*
* Description : (1) Decode ISR & call appropriate ISR handler :
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR
*
* Note(s)     : (3) The functions NetNIC_IntEnter() and NetNIC_IntClr() inform the external interrupt
*                   controller that the ISR is being entered and clear the interrupt source(s) on the
*                   external interrupt controller, respectively. These MUST be developer-implemented in
*
*                       \<Your Product Application>\net_isr.c
*
*                           where
*                                   <Your Product Application>    directory path for Your Product's Application.
*********************************************************************************************************
*/

void  NetNIC_ISR_Handler (void)
{
    NET_ERR  err;
    
    NetOS_IF_RxTaskSignal(&err);
    switch (err) {
            case NET_IF_ERR_NONE:
                 /* Clear the Eth DMA Rx IT pending bits */
                 ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
                 ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
                 break;

            case NET_IF_ERR_RX_Q_FULL:
            case NET_IF_ERR_RX_Q_POST_FAIL:
            default:
                 NetNIC_RxPktDiscard(0, &err);
                 break;
    }
    
    ETH_GetDMATxDesc();
}

/*
*********************************************************************************************************
*                                         NetNIC_RxPktGetSize()
*
* Description : Get network packet size from NIC.
*
* Argument(s) : none.
*
* Return(s)   : Size, in octets, of next frame to be read from the EMAC buffers
*
* Caller(s)   : NetIF_RxTaskHandler()
*
* Note(s)     : (1) On occassion, it is possible to have frame fragments in the descriptor list. As mentioned
*                   above in Note 1) of NIC_GetNRdy(), should 2 start of frame descriptors be found without
*                   corresponding EOF descriptors , then softwares must free the rogue descriptors. This
*                   function will free rogue descriptors if they are found while searching for the next
*                   frame and its corresponding size
*
*               (2) This routine causes the TCP/IP stack to service received frames in a random order.
*                   In other words, frames that arrive before other frames are not guarrenteed to be
*                   serviced first. This is a result of always searching through the descriptor list from
*                   top to bottom, and not from an expected start point and wrapping around. The benefit to
*                   this approach however is that you dont have to keep state information about where the
*                   next expected SOF flag will be. Nor do you have check if the expected SOF pointer is
*                   correct and make changes should Note 1) effect the correctness of that pointer.
*
*               (3) There MUST be at least 1 SOF / EOF combination in the descriptor list, otherwise
*                   interrupt generation would not have occured since interrupt generation takes place
*                   after the complete frame has been trasferred via DMA to RAM.
*********************************************************************************************************
*/

CPU_INT16U  NetNIC_RxPktGetSize (void)
{
    CPU_INT16U size;

    size = (CPU_INT16U)ETH_GetRxPktSize() - 4;
    return (size);
}

/*
*********************************************************************************************************
*                                            NetNIC_RxPkt()
*
* Description : Read network packet from NIC into buffer.
*
* Argument(s) : ppkt        Pointer to memory buffer to receive NIC packet.
*
*               size        Number of packet frame octets to read into buffer.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE            Packet successfully read.
*                               NET_ERR_INIT_INCOMPLETE     Network initialization NOT complete.
*                               NET_NIC_ERR_NULL_PTR        Argument 'ppkt' passed a NULL pointer.
*                               NET_NIC_ERR_INVALID_SIZE    Invalid size.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
* Note(s)     : (1) NetNIC_RxPkt() blocked until network initialization completes; perform NO action.
*********************************************************************************************************
*/

void  NetNIC_RxPkt (void        *ppkt,
                    CPU_INT16U   size,
                    NET_ERR     *perr)
{
    *perr = NET_NIC_ERR_NONE;
  
    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, exit rx (see Note #1).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                         /* ------------------- VALIDATE PTR ------------------  */
    if (ppkt == (void *)0) {
       *perr = NET_NIC_ERR_NULL_PTR;
        return;
    }

                                                                        /* ------------------- VALIDATE SIZE -----------------  */
    if (size < NET_IF_FRAME_MIN_SIZE) {
       *perr = NET_NIC_ERR_INVALID_SIZE;
        return;
    }
#endif

    //ETH_HandleRxPkt(ppkt);
    LOW_LEVEL_Input(ppkt);
    
    if (*perr != NET_NIC_ERR_NONE) {
        return;
    }

    NET_CTR_STAT_INC(NetNIC_StatRxPktCtr);
}

/*
*********************************************************************************************************
*                                         NetNIC_RxPktDiscard()
*
* Description : Discard network packet from NIC to free NIC packet frames for new receive packets.
*
* Argument(s) : size        Number of packet frame octets.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE            Packet successfully discarded.
*                               NET_ERR_INIT_INCOMPLETE     Network initialization NOT complete.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
* Note(s)     : (1) NetNIC_RxPktDiscard() blocked until network initialization completes; perform NO action.
*********************************************************************************************************
*/

void  NetNIC_RxPktDiscard (CPU_INT16U   size,
                           NET_ERR     *perr)
{  
#if ((NET_CTR_CFG_ERR_EN      == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr = 0;
#endif

    CPU_CRITICAL_ENTER();
    
    LOW_LEVEL_Init();
    
    CPU_CRITICAL_EXIT();

    NET_CTR_ERR_INC(NetNIC_ErrRxPktDiscardedCtr);

    *perr = NET_NIC_ERR_NONE;
}

/*
*********************************************************************************************************
*                                         NetNIC_TxPktPrepare()
*
* Description : Prepare to transmit data packets from network driver layer to network interface card.
*
* Argument(s) : ppkt        Pointer to memory buffer to transmit NIC packet.
*
*               size        Number of packet frame octets to write to frame.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE            Packet successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE     Network initialization NOT complete.
*
*                                                           - RETURNED BY NetNIC_TxPktDiscard() : -
*                               NET_ERR_TX                  Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx().
*
* Note(s)     : (1) NetNIC_TxPktPrepare() blocked until network initialization completes; perform NO action.
*********************************************************************************************************
*/

void  NetNIC_TxPktPrepare (void        *ppkt,
                           CPU_INT16U   size,
                           NET_ERR     *perr)
{
    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, exit tx (see Note #1).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                         /* ------------------- VALIDATE PTR ------------------  */
    if (ppkt == (void *)0) {
        NetNIC_TxPktDiscard(perr);
        return;
    }
#endif

    MACB_TxPktPrepare(ppkt, size, perr);                                /* Prepare to tx pkt to the EMAC.                       */

    if (*perr != NET_NIC_ERR_NONE) {
        NetNIC_TxPktDiscard(perr);
        return;
    }

    *perr = NET_NIC_ERR_NONE;
}

/*
*********************************************************************************************************
*                                            NetNIC_TxPkt()
*
* Description : Transmit data packets from network driver layer to network interface card.
*
* Argument(s) : ppkt        Pointer to memory buffer to transmit NIC packet.
*
*               size        Number of packet frame octets to write to frame.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE            Packet successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE     Network initialization NOT complete.
*
*                                                           - RETURNED BY NetNIC_TxPktDiscard() : -
*                               NET_ERR_TX                  Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx().
*
* Note(s)     : (1) NetNIC_TxPkt() blocked until network initialization completes; perform NO action.
*********************************************************************************************************
*/

void  NetNIC_TxPkt (void        *ppkt,
                    CPU_INT16U   size,
                    NET_ERR     *perr)
{
    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, exit tx (see Note #1).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                         /* ------------------- VALIDATE PTR ------------------  */
    if (ppkt == (void *)0) {
        NetNIC_TxPktDiscard(perr);
        return;
    }
#endif

    //MACB_TxPkt(ppkt, size, perr);                                     /* Tx pkt to processor.                                 */
    LOW_LEVEL_Output(ppkt, size);
    *perr = NET_NIC_ERR_NONE;

    if (*perr != NET_NIC_ERR_NONE) {
        NetNIC_TxPktDiscard(perr);
        return;
    }

    NET_CTR_STAT_INC(NetNIC_StatTxPktCtr);

    *perr = NET_NIC_ERR_NONE;
}

/*
*********************************************************************************************************
*                                         NetNIC_TxPktDiscard()
*
* Description : On any Transmit errors, set error.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_ERR_TX                  Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_TxPkt().
*********************************************************************************************************
*/

static  void  NetNIC_TxPktDiscard (NET_ERR  *perr)
{
    NET_CTR_ERR_INC(NetNIC_ErrTxPktDiscardedCtr);

   *perr = NET_ERR_TX;
}

/*
*********************************************************************************************************
*                                          MACB_TxPktPrepare()
*
* Description : (1) This funciton is empty.
*
* Argument(s) : ppkt        Pointer to memory buffer to transmit NIC packet.
*               ----        Argument checked in NetNIC_TxPkt().
*
*               size        Number of packet frame octets to write to frame.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Packet successfully transmitted.
* Return(s)   : none.
*
* Caller(s)   : NetNIC_TxPktPrepare().
*********************************************************************************************************
*/

static void  MACB_TxPktPrepare (void        *ppkt,
                                CPU_INT16U   size,
                                NET_ERR     *perr)
{
   *perr = NET_NIC_ERR_NONE;
}
