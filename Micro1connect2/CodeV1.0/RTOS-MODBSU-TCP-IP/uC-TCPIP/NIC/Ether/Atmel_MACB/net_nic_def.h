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
*                                          MACB EMAC DEFINES
*
* Note(s) : (1) These global #define's are required PRIOR to network configuration.
*********************************************************************************************************
*/

#define  EMAC_MAC_ADDR_SEL_NONE                     0       /* Do not explicity select a MAC address src   */
#define  EMAC_MAC_ADDR_SEL_EEPROM                   1       /* EMAC MAC addr cfg'd from NIC's ext EEPROM.  */
#define  EMAC_MAC_ADDR_SEL_CFG                      2       /* EMAC MAC addr cfg'd by prod app @ init.     */
