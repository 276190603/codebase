/**
******************************************************************************
* @file    stm32f107.c
* @author  MCD Application Team
* @version V1.0.0
* @date    11/20/2009
* @brief   STM32F107 hardware configuration
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32_eth.h"
#include "stm32f107.h"
#include "RS485.h"
#include "USART1.H"
#include "RTC_Time.h"
#include "myiic.h"
#include "DIPswitch.h"
#include "gpio_control.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define CHECKSUM_BY_HARDWARE 
#define DP83848_PHY                /* Ethernet pins mapped on STM3210C-EVAL Board */
#define PHY_ADDRESS           0x01 /* Relative to STM3210C-EVAL Board */

//#define MII_MODE                   /* MII mode for STM3210C-EVAL Board (MB784) (check jumpers setting) */
#define RMII_MODE                /* RMII mode for STM3210C-EVAL Board (MB784) (check jumpers setting) */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void GPIO_Configuration(void);
void NVIC_Configuration(void);
void ADC_Configuration(void);
void EXTI_Configuration(void);
void Ethernet_Configuration(void);
uint16_t Get_Adc(uint8_t ch);
uint16_t Get_Adc_Average(uint8_t ch,uint8_t times);

extern u8   macadd[];
extern void Set_MAC_Address(uint8_t* macadd);
extern void LOW_LEVEL_Init(void);

/**
* @brief  Setup STM32 system (clocks, Ethernet, GPIO, NVIC) and STM3210C-EVAL resources.
* @param  None
* @retval None
*/
void System_Setup(void)
{
  /* Setup STM32 clock, PLL and Flash configuration) */
  SystemInit();
  
  /* Enable USART2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
  /* Enable ETHERNET clock  */

  
  /* Enable GPIOs and ADC1 clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO |
                           RCC_APB2Periph_ADC1, ENABLE);
  
  /* NVIC configuration */
  NVIC_Configuration();
  
  /* Configure the GPIO ports */
  //GPIO_Configuration();
  
  /* ADC configuration */
  ADC_Configuration();
  //I2C_Configuration();
  /* EXTI configuration */
  //EXTI_Configuration();
  
  USART_Configuration(); //串口配置  
  //USART1_Configuration();
  /* Configure the Ethernet peripheral */
  // Ethernet_Configuration();
  //RTC_Config() ;
  /* SystTick configuration  */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  IIC_Init();
  DIPswitch_Init();
  Hardware_Watchdog_init() ;
  buzzer_Init();
  relay_Init();
  Hardware_reset();
}

/**
* @brief  Configures the Ethernet Interface
* @param  None
* @retval None
*/
void Ethernet_Configuration(void)
{
 
}

/**
* @brief  Configures the different GPIO ports.
* @param  None
* @retval None
*/
void GPIO_Configuration(void)
{
  
}

/**
* @brief  Configures the ADC.
* @param  None
* @retval None
*/
void ADC_Configuration(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //使能ADC1通道时钟
  //PA1 作为模拟通道输入引脚                         
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
  
  
  /* ADC1 Configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  //ADC_TempSensorVrefintCmd(ENABLE);
  
  /* ADC1 regular channels configuration */
  /* TempSensor time > 2.2us,(239cycles) */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10|ADC_Channel_11,1,ADC_SampleTime_55Cycles5);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));
  
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
  
  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
uint16_t Get_Adc(uint8_t ch)  
{
  //设置指定ADC的规则组通道，一个序列，采样时间
  ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
  
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
  
  return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

uint16_t Get_Adc_Average(uint8_t ch,uint8_t times)
{
  u32 temp_val=0;
  u8 t;
  for(t=0;t<times;t++)
  {
    temp_val+=Get_Adc(ch);
  }
  return temp_val/times;
} 	 
/**
* @brief  Configures the nested vectored interrupt controller.
* @param  None
* @retval None
*/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  
  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  
  /* Enable the Ethernet global Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the EXTI9_5 Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //使能串口2中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //先占优先级2级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //从优先级2级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
  
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //使能串口2中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //先占优先级2级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //从优先级2级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
  
  
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //使能串口2中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级2级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //从优先级2级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
  
   NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; //使能串口2中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级2级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //从优先级2级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
  
     NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //使能串口2中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级2级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级2级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
  
  /* Enable the RTC Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
  
}

/**
* @brief  Configures the EXTI Interface
* @param  None
* @retval None
*/
void EXTI_Configuration(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  
  /* Connect  EXTI Line to GPIO Pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
  
  /* Configure  EXTI Line to generate an interrupt */  
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

