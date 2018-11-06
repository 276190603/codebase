/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */
#include "includes.h"

#include "ethernetif.h"
#include "stm32_eth.h"

#define  ETH_DMARxDesc_FrameLengthShift            16

#define  ETH_ERROR                           ((u32)0)
#define  ETH_SUCCESS                         ((u32)1)

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  int unused;
};


#define ETH_RXBUFNB        5
#define ETH_TXBUFNB        5

u8  macadd[] = {0xad, 0x00, 0x00, 0x00, 0x00, 0x01};

uint8_t MACaddr[6];
ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];/* Ethernet Rx & Tx DMA Descriptors */
uint8_t Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE], Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE];/* Ethernet buffers */

ETH_DMADESCTypeDef  *DMATxDesc = DMATxDscrTab;
extern ETH_DMADESCTypeDef  *DMATxDescToSet;
extern ETH_DMADESCTypeDef  *DMARxDescToGet;

typedef struct{
u32 length;
u32 buffer;
ETH_DMADESCTypeDef *descriptor;
}FrameTypeDef;

FrameTypeDef frame_rx = {0,0,};
FrameTypeDef ETH_RxPkt_ChainMode(void);
u32 ETH_GetCurrentTxBuffer(void);
u32 ETH_TxPkt_ChainMode(u16 FrameLength);


/**
 * Setting the MAC address.
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void Set_MAC_Address(uint8_t* macadd)
{
  MACaddr[0] = macadd[0];
  MACaddr[1] = macadd[1];
  MACaddr[2] = macadd[2];
  MACaddr[3] = macadd[3];
  MACaddr[4] = macadd[4];
  MACaddr[5] = macadd[5];
  
  ETH_MACAddressConfig(ETH_MAC_Address0, macadd);  
}


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void LOW_LEVEL_Init(void)
{
  /* Initialize Tx Descriptors list: Chain Mode */
  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  /* Initialize Rx Descriptors list: Chain Mode  */
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

  /* Enable Ethernet Rx interrrupt */
  { int i;
    for(i=0; i<ETH_RXBUFNB; i++)
    {
      ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
    }
  }

#ifdef CHECKSUM_BY_HARDWARE
  /* Enable the checksum insertion for the Tx frames */
  { int i;
    for(i=0; i<ETH_TXBUFNB; i++)
    {
      ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
    }
  }
#endif
  
  frame_rx.descriptor = (ETH_DMADESCTypeDef*) (ETH->DMARDLAR);

  /* Enable MAC and DMA transmission and reception */
  ETH_Start();

}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

void LOW_LEVEL_Output(u8 *data, u16 len)
{
  u8 *buffer =  (u8 *)ETH_GetCurrentTxBuffer();
  
  Mem_Copy ((u8*)&buffer[0], (u8 *)&data[0], len);

  ETH_TxPkt_ChainMode(len);
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
u16 LOW_LEVEL_Input(u8 *buffer)
{
  u16 len;
  FrameTypeDef frame;
  
  frame = ETH_RxPkt_ChainMode();
  /* Obtain the size of the packet and put it into the "len"
     variable. */
  
  len = frame.length;
  
  Mem_Copy ((u8 *)&buffer[0], (u8 *)frame.buffer, len);

  return len;
}

/*******************************************************************************
* Function Name  : ETH_RxPkt_ChainMode
* Description    : Receives a packet.
* Input          : None
* Output         : None
* Return         : frame: farme size and location
*******************************************************************************/
FrameTypeDef ETH_RxPkt_ChainMode(void)
{ 
  u32 framelength = 0;
  u32 cyc = 0;

  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  for (cyc = 0; cyc < ETH_RXBUFNB; cyc++)
  {
      if((frame_rx.descriptor->Status & ETH_DMARxDesc_OWN) != (u32)RESET)
      {	
	    frame_rx.length = ETH_ERROR;
            
            /* Set Own bit of the Rx descriptor Status: gives the buffer back to ETHERNET DMA */
            frame_rx.descriptor->Status = ETH_DMARxDesc_OWN;
      }
      else
          break;
      
      if((frame_rx.descriptor->ControlBufferSize & ETH_DMARxDesc_RCH) != (uint32_t)RESET)
      {     
          /* Selects the next DMA Rx descriptor list for next buffer to read */ 
          frame_rx.descriptor = (ETH_DMADESCTypeDef*) (frame_rx.descriptor->Buffer2NextDescAddr);    
      }
      else /* Ring Mode */
      {   
          if((frame_rx.descriptor->ControlBufferSize & ETH_DMARxDesc_RER) != (uint32_t)RESET)
          {
             /* Selects the first DMA Rx descriptor for next buffer to read: last Rx descriptor was used */
             frame_rx.descriptor = (ETH_DMADESCTypeDef*) (ETH->DMARDLAR);      
          }
          else
          { 
             /* Selects the next DMA Rx descriptor list for next buffer to read */
             frame_rx.descriptor = (ETH_DMADESCTypeDef*) ((uint32_t)frame_rx.descriptor + 0x10 + ((ETH->DMABMR & ETH_DMABMR_DSL) >> 2));      
          }
      }
      
      if (cyc == ETH_RXBUFNB-1)
      {
          if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  
          {
              /* Clear RBUS ETHERNET DMA flag */
              ETH->DMASR = ETH_DMASR_RBUS;
              /* Resume DMA reception */
              ETH->DMARPDR = 0;
          }

	  /* Return error: OWN bit set */
          return frame_rx;
      }
  }
  
  if(((frame_rx.descriptor->Status & ETH_DMARxDesc_ES) == (u32)RESET) && 
     ((frame_rx.descriptor->Status & ETH_DMARxDesc_LS) != (u32)RESET) &&  
     ((frame_rx.descriptor->Status & ETH_DMARxDesc_FS) != (u32)RESET))  
  {      
        /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
        framelength = ((frame_rx.descriptor->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;
	
	/* Get the addrees of the actual buffer */
	frame_rx.buffer = frame_rx.descriptor->Buffer1Addr;	
  }
  else
  {
      /* Return ERROR */
      framelength = ETH_ERROR;
  }

  frame_rx.length = framelength;

  //frame_rx.descriptor = DMARxDescToGet;
  /* Selects the next DMA Rx descriptor list for next buffer to read */ 
  /* Update the ETHERNET DMA global Rx descriptor with next Rx decriptor */      
  /* Chained Mode */
  if((frame_rx.descriptor->ControlBufferSize & ETH_DMARxDesc_RCH) != (uint32_t)RESET)
  {     
      /* Selects the next DMA Rx descriptor list for next buffer to read */ 
      frame_rx.descriptor = (ETH_DMADESCTypeDef*) (frame_rx.descriptor->Buffer2NextDescAddr);    
  }
  else /* Ring Mode */
  {   
      if((frame_rx.descriptor->ControlBufferSize & ETH_DMARxDesc_RER) != (uint32_t)RESET)
      {
         /* Selects the first DMA Rx descriptor for next buffer to read: last Rx descriptor was used */
         frame_rx.descriptor = (ETH_DMADESCTypeDef*) (ETH->DMARDLAR);      
      }
      else
      { 
         /* Selects the next DMA Rx descriptor list for next buffer to read */
         frame_rx.descriptor = (ETH_DMADESCTypeDef*) ((uint32_t)frame_rx.descriptor + 0x10 + ((ETH->DMABMR & ETH_DMABMR_DSL) >> 2));      
      }
  }
  
  /* Set Own bit of the Rx descriptor Status: gives the buffer back to ETHERNET DMA */
  frame_rx.descriptor->Status = ETH_DMARxDesc_OWN;
 
  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  
  {
    /* Clear RBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_RBUS;
    /* Resume DMA reception */
    ETH->DMARPDR = 0;
  }
  
  /* Return Frame */
  return (frame_rx);
}

/*******************************************************************************
* Function Name  : ETH_TxPkt_ChainMode
* Description    : Transmits a packet, from application buffer, pointed by ppkt.
* Input          : - FrameLength: Tx Packet size.
* Output         : None
* Return         : ETH_ERROR: in case of Tx desc owned by DMA
*                  ETH_SUCCESS: for correct transmission
*******************************************************************************/
u32 ETH_TxPkt_ChainMode(u16 FrameLength)
{   
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
  {  
	/* Return ERROR: OWN bit set */
    return ETH_ERROR;
  }
        
  /* Setting the Frame Length: bits[12:0] */
  DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);

  /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */    
  DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

  /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
  DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if ((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)
  {
    /* Clear TBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    ETH->DMATPDR = 0;
  }
  
  /* Update the ETHERNET DMA global Tx descriptor with next Tx decriptor */  
  /* Chained Mode */
  /* Selects the next DMA Tx descriptor list for next buffer to send */ 
  DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);    


  /* Return SUCCESS */
  return ETH_SUCCESS;   
}


/*******************************************************************************
* Function Name  : ETH_GetCurrentTxBuffer
* Description    : Return the address of the buffer pointed by the current descritor.
* Input          : None
* Output         : None
* Return         : Buffer address
*******************************************************************************/
u32 ETH_GetCurrentTxBuffer(void)
{ 
  /* Return Buffer address */
  return (DMATxDescToSet->Buffer1Addr);   
}

/*******************************************************************************
* Function Name  : ETH_GetDMATxDesc
* Description    : None
* Input          : None
* Output         : None
* Return         : Buffer address
*******************************************************************************/
void ETH_GetDMATxDesc(void)
{ 
    /* Update the ETHERNET DMA global Rx descriptor with next Rx decriptor */      
    /* Chained Mode */
    if((DMARxDescToGet->ControlBufferSize & ETH_DMARxDesc_RCH) != (uint32_t)RESET)
    {     
       /* Selects the next DMA Rx descriptor list for next buffer to read */ 
       DMARxDescToGet = (ETH_DMADESCTypeDef*) (DMARxDescToGet->Buffer2NextDescAddr);    
    }
    else /* Ring Mode */
    {   
       if((DMARxDescToGet->ControlBufferSize & ETH_DMARxDesc_RER) != (uint32_t)RESET)
       {
          /* Selects the first DMA Rx descriptor for next buffer to read: last Rx descriptor was used */
          DMARxDescToGet = (ETH_DMADESCTypeDef*) (ETH->DMARDLAR);      
       }
       else
       { 
          /* Selects the next DMA Rx descriptor list for next buffer to read */
          DMARxDescToGet = (ETH_DMADESCTypeDef*) ((uint32_t)DMARxDescToGet + 0x10 + ((ETH->DMABMR & ETH_DMABMR_DSL) >> 2));      
       }
    }
}

