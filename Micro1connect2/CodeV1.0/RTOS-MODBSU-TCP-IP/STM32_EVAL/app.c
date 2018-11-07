/*
*********************************************************************************************************
*
*                                             EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
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

#include <includes.h>
#include "RTC_Time.h"
//#include "UART5.h"
//#include "USART1.H"
#include "24cxx.h"
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  APP_SCR_SIGN_ON                        0
#define  APP_SCR_VER_TICK_RATE                  1
#define  APP_SCR_CPU                            2
#define  APP_SCR_CTXSW                          3
#define  APP_SCR_TCPIP                          4

#define  APP_SCR_FIRST                    APP_SCR_SIGN_ON
#define  APP_SCR_LAST                     APP_SCR_TCPIP

#define RL	0x0001
#define TL	0x0002
#define VL	0x0004
#define RH	0x0008
#define TH	0x0010
#define VH	0x0020
#define CL	0x0040
#define TE	0x0080
/* ----------------------- Static variables ---------------------------------*/

#if (APP_CFG_TCPIP_EN > 0)
extern  USHORT   usRegCoilsStart;
extern  UCHAR    ucRegCoilsBuf[];

extern  USHORT   usRegDiscreteStart;
extern  UCHAR    ucRegDiscreteBuf[];

extern  USHORT   usRegInputStart;
extern  USHORT   usRegInputBuf[];

extern  USHORT   usRegHoldingStart;
extern  USHORT   usRegHoldingBuf[];
#endif

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
/* ----------------- APPLICATION GLOBALS ------------------ */
static  OS_STK       AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_EVENT    *AppUserIFMbox;
/* --------------- uC/TCPIP RELATED GLOBALS --------------- */
#if (APP_CFG_TCPIP_EN == DEF_ENABLED)
//NET_IP_ADDR  AppNetIP;
//NET_IP_ADDR  AppNetMsk;
//NET_IP_ADDR  AppNetGateway;

static  OS_STK       AppTaskModbusStk[APP_CFG_TASK_MODBUS_STK_SIZE];
static  OS_STK       AppTaskALARMStk[KSD_CFG_TASK_LED_STK_SIZE];
#endif

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskStart                (void        *p_arg);
static  void  App_TaskCreate               (void);
static  void  App_TaskCreate2              (void);

//#if (APP_CFG_TCPIP_EN == DEF_ENABLED)
static  void  App_InitTCPIP                (void);
static  void  App_TaskModbus               (void        *p_arg);
static  void  App_TaskALARM               (void        *p_arg);
//#endif

static  void  App_FormatDec                (CPU_INT08U  *s,
                                            CPU_INT32U   value,
                                            CPU_INT08U   digits);

extern  void  BSP_IntDisAll (void);
extern  void  System_Setup(void);
extern  void  DSP_main(void);
unsigned char get_R_T_flag,get_R_I_flag,get_R_T_flag2,get_R_I_flag2;
unsigned char Reset_Rdata,Get_Rdata,Start_Rdata1,Start_Rdata2;
struct tm time_now;

union float_to_u16 mytest,u32_to_float,u32_buf1,u32_buf2,u32_buf3;
USHORT hour=0,hour2=0;
extern  CPU_CHAR IP[20];
extern  CPU_CHAR NETMASK[20];
extern  CPU_CHAR GW[20];
/*
*********************************************************************************************************
*                                                main()
*
* Description : The standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  main (void)
{
  CPU_INT08U  err;
  
  BSP_IntDisAll();                                            /* Disable all interrupts until we are ready to accept them */
  
  OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"              */
  
  //NVIC_Configuration(); //中断优先级设置
  
  OSTaskCreateExt(App_TaskStart,                              /* Create the start task                                    */
                  (void *)0,
                  (OS_STK *)&AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                  APP_CFG_TASK_START_PRIO,
                  APP_CFG_TASK_START_PRIO,
                  (OS_STK *)&AppTaskStartStk[0],
                  APP_CFG_TASK_START_STK_SIZE,
                  (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  
#if (OS_TASK_NAME_SIZE > 13)
  OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start Task", &err);
#endif
  
  OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
  
}


/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*
*               (2) Interrupts are enabled once the task starts because the I-bit of the CCR register was
*                   set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
  CPU_INT08U  err;
  
  (void)p_arg;
  
  System_Setup();
  OS_CPU_SysTickInit();                                       /* Initialize the SysTick.                                  */
  
  
#if (OS_TASK_STAT_EN > 0)
  OSStatInit();                                               /* Determine CPU capacity                                   */
#endif
  //AT24CXX_WriteOneByte(299,0x00);
  
  
  //#if (APP_CFG_TCPIP_EN > 0)
  //   App_InitTCPIP();                                            /* Initialize uC/TCP-IP and associated appliations          */
  //#endif  
  
  App_TaskCreate();
  App_TaskCreate2();
  
  DSP_main();
  
   //usRegInputBuf[2928] = 1;
  
  //    usRegInputBuf[2421] = 1;
  unsigned char i;
  unsigned char Sensors_len;				//传感器数量
  unsigned char Sensors_len2;				//传感器数量
  unsigned char Monomer_len;						//单体数量
  unsigned char Monomer_len2;						//单体数量
  union float_to_u16 High_voltage_limit;			//电压高限
  union float_to_u16 Low_voltage_limit;				//电压低限
  union float_to_u16 High_temperature_limit;	//温度高限
  union float_to_u16 Low_temperature_limit;		//温度低限
  union float_to_u16 Low_capacity_limit;			//容量低限
  union float_to_u16 High_limit_resistance;		//内阻高限
  union float_to_u16 Low_limit_resistance;		//内阻低限
  union float_to_u16 voltage;				//电压测量值
  union float_to_u16 temperature;				//温度测量值
  union float_to_u16 Resistance;				//内阻测量值
  union float_to_u16 capacity;								//容量测量值
  
  //  if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 1)
  //  {
  //    AT24CXX_WriteOneByte(299,0);
  //  }
  // Sensors_len=3;						//传感器数量
  while(AT24CXX_Check());//首次使用时，将默认设置写到EE
  AT24CXX_Read_Setdata();//读EE	
  
  rerror_flag=0;       
  //AT24CXX_WriteLenByte(24,2,2);
  //AT24CXX_Reset_Alldata();
  //                get_R_I_flag =1;
  float V_Z=0;
  float R_start,R_now,C_low,A_RR_start;
  while (1) {                                          /* Task body, always written as an infinite loop.           */
    
    Sensors_len=usRegInputBuf[2902];								//传感器数量
    //Sensors_len2=usRegInputBuf[2300];	
    
    Monomer_len=usRegInputBuf[2900];								//单体数量
    
    
    High_voltage_limit.data16[1]=usRegInputBuf[2905];	//电压高限
    High_voltage_limit.data16[0]=usRegInputBuf[2906];
    
    Low_voltage_limit.data16[1]=usRegInputBuf[2907];	//电压低限
    Low_voltage_limit.data16[0]=usRegInputBuf[2908];
    
    High_temperature_limit.data16[1]=usRegInputBuf[2909];//温度高限
    High_temperature_limit.data16[0]=usRegInputBuf[2910];
    
    Low_temperature_limit.data16[1]=usRegInputBuf[2911];//温度低限
    Low_temperature_limit.data16[0]=usRegInputBuf[2912];
    
    Low_capacity_limit.data16[1]=usRegInputBuf[2921];	//容量低限
    Low_capacity_limit.data16[0]=usRegInputBuf[2922];
    
    High_limit_resistance.data16[1]=usRegInputBuf[2913]; //内阻高限
    High_limit_resistance.data16[0]=usRegInputBuf[2914];
    
    Low_limit_resistance.data16[1]=usRegInputBuf[2915];	//内阻低限
    Low_limit_resistance.data16[0]=usRegInputBuf[2916];
    
 
    
    if( usRegInputBuf[2928] == 1)
    {
      V_Z = 0;
      for(i=0;i<Sensors_len;i++)
      {	
        rerror_flag = usRegInputBuf[i+2560];
        
        voltage.data32=FUN_Read_V (i+1,CMD_0X60);   	//读取传感器电压
        if(voltage.data32>=High_voltage_limit.data32*Monomer_len)
        {
          if(rerror_flag != TE)
          {
            rerror_flag=rerror_flag|(VH);
          }
        }
        else if(voltage.data32<=Low_voltage_limit.data32*Monomer_len)
        {
          if(rerror_flag != TE)
          {
            rerror_flag=rerror_flag|(VL);	
          }
        }
        else
        {
          rerror_flag=rerror_flag&(~VH)&(~VL);
        }
        usRegInputBuf[2*i]=voltage.data16[1];
        usRegInputBuf[2*i+1]=voltage.data16[0];		
        V_Z+=voltage.data32;
        OSTimeDlyHMSM(0, 0, 0, 50);	
        
        temperature.data32=FUN_Read_T (i+1,CMD_0X61);	//读取温度值
        
        if(temperature.data32>=High_temperature_limit.data32)
        {
          if(rerror_flag != TE)
          {
            rerror_flag=rerror_flag|(TH);
          }
        }
        else if(temperature.data32<=Low_temperature_limit.data32)
        {
          if(rerror_flag != TE)
          {
            rerror_flag=rerror_flag|(TL);
          }
        }
        else
        {
          rerror_flag=rerror_flag&(~TH)&(~TL);
        }
        
        usRegInputBuf[2*i+512]=temperature.data16[1];
        usRegInputBuf[2*i+1+512]=temperature.data16[0];
        if(rerror_flag>=0x80)
        {
          rerror_flag=0x80;
        }
        usRegInputBuf[i+2560]=rerror_flag;
        //usRegInputBuf[2816] = 0;
        usRegInputBuf[2816] |= usRegInputBuf[i+2560];
        OSTimeDlyHMSM(0, 0, 0, 50);	
        //                         第一通道均衡条件
        //-----------------------------------------------------------------*/
        if(usRegInputBuf[2963])//一键允许均衡
        {
          u32_buf1.data16[1] = usRegInputBuf[2955];//当前电流
          u32_buf1.data16[0] = usRegInputBuf[2956];
          
          u32_buf2.data16[1] = usRegInputBuf[2965];//电流上限
          u32_buf2.data16[0] = usRegInputBuf[2966];
          
          u32_buf3.data16[1] = usRegInputBuf[2967];//电流下线
          u32_buf3.data16[0] = usRegInputBuf[2968];
          if(u32_buf1.data32 > u32_buf2.data32)//停止放电
          {
            FUN_SEND_JUNHENG_V (i+1, 2000);	
          }
          else if((u32_buf1.data32 < u32_buf3.data32)&&(u32_buf1.data32 > 0))//放电
          {
            //                             mytest.data16[1] = usRegInputBuf[2404];//组电压
            //                             mytest.data16[0] = usRegInputBuf[2405];
            mytest.data32 = V_Z;
            u32_buf1.data16[1] = usRegInputBuf[2973];//第一通道浮充电压
            u32_buf1.data16[0] = usRegInputBuf[2974];
            if(Sensors_len>0)//只数大于0
            {										
              if((mytest.data32/Sensors_len) > u32_buf1.data32)//组电压均值
              {                             
                FUN_SEND_JUNHENG_V (i+1, (unsigned int)((mytest.data32/Sensors_len)*100));
              }
              else if(u32_buf1.data32 > 13)//设定浮充电压
              {
                FUN_SEND_JUNHENG_V (i+1, (unsigned int)(u32_buf1.data32*100));
              }
              else
              {
                FUN_SEND_JUNHENG_V (i+1, 2000);	//停止放电
              }	
            }
            else
            {
              FUN_SEND_JUNHENG_V (i+1, 2000);	//停止放电
            }
          }
          else 
          {                     
            FUN_SEND_JUNHENG_V (i+1, 2000);	//停止放电
          }						
        }
        else//一键停止均衡
        {
          FUN_SEND_JUNHENG_V (i+1, 2000);//停止放电	
        }
        OSTimeDlyHMSM(0, 0, 0, 50);	
        
      }	 
      mytest.data32 = V_Z;
      usRegInputBuf[2953] = mytest.data16[1];//组电压
      usRegInputBuf[2954] = mytest.data16[0];        
      if(get_R_T_flag)
      {
        for(i=0;i<Sensors_len;i++)
        {       
          if(get_R_I_flag)
          {           
            FUN_SEND_CMD (i+1,CMD_0X42);	//读取内阻值	
            OSTimeDlyHMSM(0, 0, 5, 0);
            FUN_SEND_CMD (i+1,CMD_0X22);
            OSTimeDlyHMSM(0, 0, 0, 500);			
            Resistance.data32=FUN_READ_DATA ();
            
            R_now=Resistance.data32;	
            
            if(R_now<=255.937)
            {
              usRegInputBuf[2*i+1024]=Resistance.data16[1];
              usRegInputBuf[2*i+1+1024]=Resistance.data16[0];												
            }
            if(R_now <=0)
            {
              if(rerror_flag != TE)
              {
                rerror_flag=rerror_flag|(RL);
              }
            }	
            if((!AT24CXX_Check_R(i))&&(R_now>0)&&(R_now<255.937))
            {
              mytest.data16[1]=AT24CXX_ReadLenByte(300+4*i,2);
              OSTimeDlyHMSM(0, 0, 0, 1);
              mytest.data16[0]=AT24CXX_ReadLenByte(302+4*i,2);		
              OSTimeDlyHMSM(0, 0, 0, 1);
              R_start=mytest.data32;
              usRegInputBuf[2*i+2048]=mytest.data16[1];
              usRegInputBuf[2*i+1+2048]=mytest.data16[0];		
              
              
              if(R_now/R_start>High_limit_resistance.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(RH);
                }
              }
              else if(R_now/R_start<Low_limit_resistance.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(RL);
                }
              }
              else
              {
                rerror_flag=rerror_flag&(~RH)&(~RL);
              }
              
              mytest.data16[1]=usRegInputBuf[2913];
              mytest.data16[0]=usRegInputBuf[2914];									
              A_RR_start=mytest.data32;
              C_low=Low_capacity_limit.data32;
              capacity.data32=100-(R_now-R_start)*(100-C_low)/((A_RR_start-1)*R_start);
              if(capacity.data32>100&&capacity.data32<120)
              {
                capacity.data32=100;
              }
              else if(capacity.data32<0)
              {
                capacity.data32=0;
              }
              else if(capacity.data32>=120)
              {
                capacity.data32=9999;
              }
              if(capacity.data32<=Low_capacity_limit.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(CL);
                }
              }
              else
              {
                rerror_flag=rerror_flag&(~CL);
              }		
              
              usRegInputBuf[2*i+1536]=capacity.data16[1];
              usRegInputBuf[2*i+1+1536]=capacity.data16[0];
              
              
            }
            else
            {
              capacity.data32=0;
              if(rerror_flag != TE)
              {
                rerror_flag=rerror_flag|(CL);
              }
            }							
            if(rerror_flag>=0x80)
            {
              rerror_flag=0x80;
            }
          }
          if(rerror_flag>=0x80)
          {
            rerror_flag=0x80;
          }
          usRegInputBuf[i+2560]=rerror_flag;
          usRegInputBuf[2816] |= usRegInputBuf[i+2560];
          
        }
        get_R_T_flag=0;
      }
       if(Start_Rdata1)  
      {
        Start_Rdata1 = 0;
    
          if(get_R_I_flag2)
          {           
            FUN_SEND_CMD (Get_Rdata+1,CMD_0X42);	//读取内阻值	
            OSTimeDlyHMSM(0, 0, 5, 0);
            FUN_SEND_CMD (Get_Rdata+1,CMD_0X22);
            OSTimeDlyHMSM(0, 0, 0, 500);			
            Resistance.data32=FUN_READ_DATA ();
            
            R_now=Resistance.data32;	
            
            if(R_now<=255.937)
            {
              usRegInputBuf[2*Get_Rdata+1024]=Resistance.data16[1];
              usRegInputBuf[2*Get_Rdata+1+1024]=Resistance.data16[0];												
            }
            if(R_now <=0)
            {
              if(rerror_flag != TE)
              {
                rerror_flag=rerror_flag|(RL);
              }
            }	
            if((!AT24CXX_Check_R(Get_Rdata))&&(R_now>0)&&(R_now<255.937))
            {
              mytest.data16[1]=AT24CXX_ReadLenByte(300+4*Get_Rdata,2);
              OSTimeDlyHMSM(0, 0, 0, 1);
              mytest.data16[0]=AT24CXX_ReadLenByte(302+4*Get_Rdata,2);		
              OSTimeDlyHMSM(0, 0, 0, 1);
              R_start=mytest.data32;
              usRegInputBuf[2*Get_Rdata+2048]=mytest.data16[1];
              usRegInputBuf[2*Get_Rdata+1+2048]=mytest.data16[0];		
              
              
              if(R_now/R_start>High_limit_resistance.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(RH);
                }
              }
              else if(R_now/R_start<Low_limit_resistance.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(RL);
                }
              }
              else
              {
                rerror_flag=rerror_flag&(~RH)&(~RL);
              }
              
              mytest.data16[1]=usRegInputBuf[2913];
              mytest.data16[0]=usRegInputBuf[2914];									
              A_RR_start=mytest.data32;
              C_low=Low_capacity_limit.data32;
              capacity.data32=100-(R_now-R_start)*(100-C_low)/((A_RR_start-1)*R_start);
              if(capacity.data32>100&&capacity.data32<120)
              {
                capacity.data32=100;
              }
              else if(capacity.data32<0)
              {
                capacity.data32=0;
              }
              else if(capacity.data32>=120)
              {
                capacity.data32=9999;
              }
              if(capacity.data32<=Low_capacity_limit.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(CL);
                }
              }
              else
              {
                rerror_flag=rerror_flag&(~CL);
              }		
              
              usRegInputBuf[2*Get_Rdata+1536]=capacity.data16[1];
              usRegInputBuf[2*Get_Rdata+1+1536]=capacity.data16[0];
              
              
            }
            else
            {
              capacity.data32=0;
              if(rerror_flag != TE)
              {
                rerror_flag=rerror_flag|(CL);
              }
            }							
            if(rerror_flag>=0x80)
            {
              rerror_flag=0x80;
            }
          }
          if(rerror_flag>=0x80)
          {
            rerror_flag=0x80;
          }
          usRegInputBuf[Get_Rdata+2560]=rerror_flag;
          usRegInputBuf[2816] |= usRegInputBuf[Get_Rdata+2560];
          

      }
    }
    ///////////////////  
    //第二组
    Sensors_len2=usRegInputBuf[2903];				//传感器数量
   // Sensors_len=0;
    Monomer_len2=usRegInputBuf[2901];				//单体数量
    
    High_voltage_limit.data16[1]=usRegInputBuf[2929];	//电压高限
    High_voltage_limit.data16[0]=usRegInputBuf[2930];
    
    Low_voltage_limit.data16[1]=usRegInputBuf[2931];	//电压低限
    Low_voltage_limit.data16[0]=usRegInputBuf[2932];
    
    High_temperature_limit.data16[1]=usRegInputBuf[2933];//温度高限
    High_temperature_limit.data16[0]=usRegInputBuf[2934];
    
    Low_temperature_limit.data16[1]=usRegInputBuf[2935];//温度低限
    Low_temperature_limit.data16[0]=usRegInputBuf[2936];
    
    Low_capacity_limit.data16[1]=usRegInputBuf[2945];		//容量低限
    Low_capacity_limit.data16[0]=usRegInputBuf[2946];
    
    High_limit_resistance.data16[1]=usRegInputBuf[2937]; //内阻高限
    High_limit_resistance.data16[0]=usRegInputBuf[2938];
    
    Low_limit_resistance.data16[1]=usRegInputBuf[2939];		//内阻低限
    Low_limit_resistance.data16[0]=usRegInputBuf[2940];		
    

   // usRegInputBuf[2816] = 0;
    V_Z=0;
    if(usRegInputBuf[2952]==1)
    {
      for(i=0;i<Sensors_len2;i++)
      {	
        rerror_flag = usRegInputBuf[i+2688];
        
        voltage.data32=UART5_Read_V (i+1,CMD_0X60);   	//读取传感器电压
        if(voltage.data32>=High_voltage_limit.data32*Monomer_len2)
        {
          if(rerror_flag != TE)
          {
            rerror_flag=rerror_flag|(VH);
          }
        }
        else if(voltage.data32<=Low_voltage_limit.data32*Monomer_len2)
        {
          if(rerror_flag != TE)
          {
            rerror_flag=rerror_flag|(VL);	
          }
        }
        else
        {
          rerror_flag=rerror_flag&(~VH)&(~VL);
        }
        usRegInputBuf[2*i+256]=voltage.data16[1];
        usRegInputBuf[2*i+1+256]=voltage.data16[0];		
        V_Z+=voltage.data32;
        OSTimeDlyHMSM(0, 0, 0, 50);	
        
        temperature.data32=UART5_Read_T (i+1,CMD_0X61);	//读取温度值
        
        if(temperature.data32>=High_temperature_limit.data32)
        {
          if(rerror_flag != TE)
          {
            rerror_flag=rerror_flag|(TH);
          }
        }
        else if(temperature.data32<=Low_temperature_limit.data32)
        {
          if(rerror_flag != TE)
          {
            rerror_flag=rerror_flag|(TL);
          }
        }
        else
        {
          rerror_flag=rerror_flag&(~TH)&(~TL);
        }
        
        usRegInputBuf[2*i+768]=temperature.data16[1];
        usRegInputBuf[2*i+1+768]=temperature.data16[0];
        if(rerror_flag>=0x80)
        {
          rerror_flag=0x80;
        }
        usRegInputBuf[i+2688]=rerror_flag;
        usRegInputBuf[2816] |= usRegInputBuf[i+2688];
        OSTimeDlyHMSM(0, 0, 0, 50);	
        //                         第一通道均衡条件
        //-----------------------------------------------------------------*/
        if(usRegInputBuf[2964])//一键允许均衡
        {
          u32_buf1.data16[1] = usRegInputBuf[2959];//当前电流
          u32_buf1.data16[0] = usRegInputBuf[2960];
          
          u32_buf2.data16[1] = usRegInputBuf[2969];//电流上限
          u32_buf2.data16[0] = usRegInputBuf[2970];
          
          u32_buf3.data16[1] = usRegInputBuf[2971];//电流下线
          u32_buf3.data16[0] = usRegInputBuf[2972];
          if(u32_buf1.data32 > u32_buf2.data32)//停止放电
          {
            UART5_SEND_JUNHENG_V (i+1, 2000);	
          }
          else if((u32_buf1.data32 < u32_buf3.data32)&&(u32_buf1.data32 > 0))//放电
          {
            //                             mytest.data16[1] = usRegInputBuf[2404];//组电压
            //                             mytest.data16[0] = usRegInputBuf[2405];
            mytest.data32 = V_Z;
            u32_buf1.data16[1] = usRegInputBuf[2975];//第一通道浮充电压
            u32_buf1.data16[0] = usRegInputBuf[2976];
            if(usRegInputBuf[2903]>0)//只数大于0
            {										
              if((mytest.data32/usRegInputBuf[2903] )> u32_buf1.data32)//组电压均值
              {                             
                UART5_SEND_JUNHENG_V (i+1, (unsigned int)((mytest.data32/usRegInputBuf[2903])*100));
              }
              else if(u32_buf1.data32 > 13)//设定浮充电压
              {
                UART5_SEND_JUNHENG_V (i+1, (unsigned int)(u32_buf1.data32*100));
              }
              else
              {
                UART5_SEND_JUNHENG_V (i+1, 2000);	//停止放电
              }	
            }
            else
            {
              UART5_SEND_JUNHENG_V (i+1, 2000);	//停止放电
            }
            
          }
          else 
          {                     
            UART5_SEND_JUNHENG_V (i+1, 2000);	//停止放电
          }						
        }
        else//一键停止均衡
        {
          UART5_SEND_JUNHENG_V (i+1, 2000);//停止放电	
        }
        OSTimeDlyHMSM(0, 0, 0, 50);	
        
      }
      mytest.data32 = V_Z;
      usRegInputBuf[2957] = mytest.data16[1];//组电压
      usRegInputBuf[2958] = mytest.data16[0];    
      if(get_R_T_flag2)     
      {
        for(i=0;i<Sensors_len2;i++)
        {       
          if(get_R_I_flag2)
          {           
            UART5_SEND_CMD (i+1,CMD_0X42);	//读取内阻值	
            OSTimeDlyHMSM(0, 0, 5, 0);
            UART5_SEND_CMD (i+1,CMD_0X22);
            OSTimeDlyHMSM(0, 0, 0, 500);			
            Resistance.data32=UART5_READ_DATA ();
            
            R_now=Resistance.data32;	
            
            if(R_now<=255.937)
            {
              usRegInputBuf[2*i+1280]=Resistance.data16[1];
              usRegInputBuf[2*i+1+1280]=Resistance.data16[0];												
            }
            if(R_now <=0)
            {
              if(rerror_flag != TE)
              {
                rerror_flag=rerror_flag|(RL);
              }
            }	
            if((!AT24CXX_Check_R2(i))&&(R_now>0)&&(R_now<255.937))
            {
              mytest.data16[1]=AT24CXX_ReadLenByte(300+4*(i+128),2);
              OSTimeDlyHMSM(0, 0, 0, 1);
              mytest.data16[0]=AT24CXX_ReadLenByte(302+4*(i+128),2);		
              OSTimeDlyHMSM(0, 0, 0, 1);
              R_start=mytest.data32;
              usRegInputBuf[2*i+2304]=mytest.data16[1];
              usRegInputBuf[2*i+1+2304]=mytest.data16[0];		
              
              
              if(R_now/R_start>High_limit_resistance.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(RH);
                }
              }
              else if(R_now/R_start<Low_limit_resistance.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(RL);
                }
              }
              else
              {
                rerror_flag=rerror_flag&(~RH)&(~RL);
              }
              
              mytest.data16[1]=usRegInputBuf[2937];
              mytest.data16[0]=usRegInputBuf[2938];									
              A_RR_start=mytest.data32;
              C_low=Low_capacity_limit.data32;
              capacity.data32=100-(R_now-R_start)*(100-C_low)/((A_RR_start-1)*R_start);
              if(capacity.data32>100&&capacity.data32<120)
              {
                capacity.data32=100;
              }
              else if(capacity.data32<0)
              {
                capacity.data32=0;
              }
              else if(capacity.data32>=120)
              {
                capacity.data32=9999;
              }
              if(capacity.data32<=Low_capacity_limit.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(CL);
                }
              }
              else
              {
                rerror_flag=rerror_flag&(~CL);
              }		
              
              usRegInputBuf[2*i+1792]=capacity.data16[1];
              usRegInputBuf[2*i+1+1792]=capacity.data16[0];
              
              
            }
            else
            {
              capacity.data32=0;
              if(rerror_flag != TE)
              {
                rerror_flag=rerror_flag|(CL);
              }
            }							
            if(rerror_flag>=0x80)
            {
              rerror_flag=0x80;
            }
          }
          if(rerror_flag>=0x80)
          {
            rerror_flag=0x80;
          }
          usRegInputBuf[i+2688]=rerror_flag;
          usRegInputBuf[2816] |= usRegInputBuf[i+2688];
          
        }
        get_R_T_flag2=0;
      }
      //////////////    
      
     if(Start_Rdata2)  
      {
        Start_Rdata2 = 0;
    
          if(get_R_I_flag2)
          {           
            UART5_SEND_CMD (Get_Rdata+1,CMD_0X42);	//读取内阻值	
            OSTimeDlyHMSM(0, 0, 5, 0);
            UART5_SEND_CMD (Get_Rdata+1,CMD_0X22);
            OSTimeDlyHMSM(0, 0, 0, 500);			
            Resistance.data32=UART5_READ_DATA ();
            
            R_now=Resistance.data32;	
            
            if(R_now<=255.937)
            {
              usRegInputBuf[2*Get_Rdata+1280]=Resistance.data16[1];
              usRegInputBuf[2*Get_Rdata+1+1280]=Resistance.data16[0];												
            }
            if(R_now <=0)
            {
              if(rerror_flag != TE)
              {
                rerror_flag=rerror_flag|(RL);
              }
            }	
            if((!AT24CXX_Check_R2(Get_Rdata))&&(R_now>0)&&(R_now<255.937))
            {
              mytest.data16[1]=AT24CXX_ReadLenByte(300+4*(Get_Rdata+128),2);
              OSTimeDlyHMSM(0, 0, 0, 1);
              mytest.data16[0]=AT24CXX_ReadLenByte(302+4*(Get_Rdata+128),2);		
              OSTimeDlyHMSM(0, 0, 0, 1);
              R_start=mytest.data32;
              usRegInputBuf[2*Get_Rdata+2304]=mytest.data16[1];
              usRegInputBuf[2*Get_Rdata+1+2304]=mytest.data16[0];		
              
              
              if(R_now/R_start>High_limit_resistance.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(RH);
                }
              }
              else if(R_now/R_start<Low_limit_resistance.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(RL);
                }
              }
              else
              {
                rerror_flag=rerror_flag&(~RH)&(~RL);
              }
              
              mytest.data16[1]=usRegInputBuf[2937];
              mytest.data16[0]=usRegInputBuf[2938];									
              A_RR_start=mytest.data32;
              C_low=Low_capacity_limit.data32;
              capacity.data32=100-(R_now-R_start)*(100-C_low)/((A_RR_start-1)*R_start);
              if(capacity.data32>100&&capacity.data32<120)
              {
                capacity.data32=100;
              }
              else if(capacity.data32<0)
              {
                capacity.data32=0;
              }
              else if(capacity.data32>=120)
              {
                capacity.data32=9999;
              }
              if(capacity.data32<=Low_capacity_limit.data32)
              {
                if(rerror_flag != TE)
                {
                  rerror_flag=rerror_flag|(CL);
                }
              }
              else
              {
                rerror_flag=rerror_flag&(~CL);
              }		
              
              usRegInputBuf[2*Get_Rdata+1792]=capacity.data16[1];
              usRegInputBuf[2*Get_Rdata+1+1792]=capacity.data16[0];
              
              
            }
            else
            {
              capacity.data32=0;
              if(rerror_flag != TE)
              {
                rerror_flag=rerror_flag|(CL);
              }
            }							
            if(rerror_flag>=0x80)
            {
              rerror_flag=0x80;
            }
          }
          if(rerror_flag>=0x80)
          {
            rerror_flag=0x80;
          }
          usRegInputBuf[Get_Rdata+2688]=rerror_flag;
          usRegInputBuf[2816] |= usRegInputBuf[Get_Rdata+2688];
          

      }
      
      OSTimeDlyHMSM(0, 0, 0, 10);

    }
       if(usRegInputBuf[2816]!=0)
      {
        usRegInputBuf[2816] = 0;
        relay_on;
      }
      else
      {
        relay_off;
      }		   
    OSTimeDlyHMSM(0, 0, 0, 50); 
  }
  
  
}

/*
*********************************************************************************************************
*                                      App_TaskCreate()
*
* Description :  Create the application tasks.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*********************************************************************************************************
*/

static  void  App_TaskCreate (void)
{
  CPU_INT08U  err;
  
#if (APP_CFG_TCPIP_EN == DEF_ENABLED)
  
  //      OSTaskCreate(App_TaskALARM,
  //                          (void          * ) 0,
  //                          (OS_STK        * )&AppTaskALARMStk[0],
  //                          (INT8U           ) KSD_CFG_TASK_LED_PRIO);
  
  OSTaskCreateExt(App_TaskModbus,
                  (void *)0,
                  (OS_STK *)&AppTaskModbusStk[APP_CFG_TASK_MODBUS_STK_SIZE - 1],
                  APP_CFG_TASK_MODBUS_PRIO,
                  APP_CFG_TASK_MODBUS_PRIO,
                  (OS_STK *)&AppTaskModbusStk[0],
                  APP_CFG_TASK_MODBUS_STK_SIZE,
                  (void *)0,
                  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
  
  
  
  //    OSTaskCreateExt( App_TaskALARM,
  //                             (void          * ) 0,
  //                             (OS_STK        * )&AppTaskALARMStk[APP_CFG_TASK_ALARM_STK_SIZE - 1],
  //                             APP_CFG_TASK_ALARM_PRIO,
  //                             APP_CFG_TASK_ALARM_PRIO,
  //                             (OS_STK        * )&AppTaskALARMStk[0],
  //                             APP_CFG_TASK_ALARM_STK_SIZE,
  //                             (void          * ) 0,
  //                             (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
  
#if (OS_TASK_NAME_SIZE > 11)
  OSTaskNameSet(APP_CFG_TASK_MODBUS_PRIO, "MODBUS Status", &err);
  OSTaskNameSet(KSD_CFG_TASK_LED_PRIO, "ALARM Status", &err);
#endif
  
#endif
}

static  void  App_TaskCreate2 (void)
{
  CPU_INT08U  err;
  
#if (APP_CFG_TCPIP_EN == DEF_ENABLED)
  
  
  
  OSTaskCreateExt( App_TaskALARM,
                  (void          * ) 0,
                  (OS_STK        * )&AppTaskALARMStk[KSD_CFG_TASK_LED_STK_SIZE - 1],
                  KSD_CFG_TASK_LED_PRIO,
                  KSD_CFG_TASK_LED_PRIO,
                  (OS_STK        * )&AppTaskALARMStk[0],
                  KSD_CFG_TASK_LED_STK_SIZE,
                  (void          * ) 0,
                  (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
  
  
#endif
}
/*
*********************************************************************************************************
*                                      App_InitTCPIP()
*
* Description : Initialize uC/TCP-IP and related module.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
unsigned char ReadBuffer[25];
#if (APP_CFG_TCPIP_EN == DEF_ENABLED)
void  App_InitTCPIP (void)
{
  
}
#endif

/*
*********************************************************************************************************
*                                         App_TaskModbus()
*
* Description :
*
* Argument(s) : p_arg.
*
* Return(s)   : none.
*
* Note(s)     : 
*********************************************************************************************************
*/

#if (APP_CFG_TCPIP_EN == DEF_ENABLED)



USHORT adcx;
static  void  App_TaskModbus (void *p_arg)
{
  (void)p_arg;
  float temp;
  union float_to_u16 u32_to_float1;
  unsigned char t2=0,t3=0,tC2=0;  
  
  // BitAction HWD;
  BitAction HWD;
  usRegInputBuf[2925]=0x4328;
  usRegInputBuf[2926]=0x0000;
  init_ok = 1;
  RTC_Config() ;
  while (1) {                                          /* Task body, always written as an infinite loop.           */
    //xStatus = eMBPoll();

    if(init_ok == 1)
    {
      HWD = Bit_RESET;
    }
    else
    {HWD = Bit_SET;}
    GPIO_WriteBit(GPIOB, GPIO_Pin_13, HWD );
    init_ok=init_ok+1;
    if(init_ok == 2)
    {init_ok = 0;}
    
    localAddr = DIPswitch();
    //localAddr = 1;
    checkComm0Modbus();
    checkComm2Modbus();
    checkComm3Modbus();
    adcx=Get_Adc_Average(ADC_Channel_11,100);
    temp=(-4638.7+2.27*adcx)*25/1000;
    //    adcx = Get_Adc_Average(100);
    //    temp=(float)adcx*(3.28/4096);
    //    temp=(2*temp-3.28)*4/3.0;
    //    temp=temp*25;
    u32_to_float1.data16[1]=usRegInputBuf[2925];
    u32_to_float1.data16[0]=usRegInputBuf[2926];
    if((temp>-2)&&(temp<u32_to_float1.data32))
    {
      get_R_I_flag=1;
    }    
    else
    {
      get_R_I_flag=0;
    }
    u32_to_float.data32=temp;
    usRegInputBuf[2955]=u32_to_float.data16[1];
    usRegInputBuf[2956]=u32_to_float.data16[0];	
    adcx=Get_Adc_Average(ADC_Channel_10,100);
    temp=(-4697.4+2.29*adcx)*25/1000;
    u32_to_float1.data16[1]=usRegInputBuf[2949];
    u32_to_float1.data16[0]=usRegInputBuf[2950];
    if((temp>-2)&&(temp<u32_to_float1.data32))
    {
      get_R_I_flag2=1;
    }    
    else
    {
      get_R_I_flag2=0;
    }   

    //IGroup_4 = temp;
    u32_to_float.data32=temp;
    usRegInputBuf[2959]=u32_to_float.data16[1];
    usRegInputBuf[2960]=u32_to_float.data16[0];	
    time_now = Time_GetCalendarTime();
    if(t2!=time_now.tm_sec)
    {
      t2=time_now.tm_sec;
      
      if(get_R_T_flag==0)
      {
        if(t3!=time_now.tm_min)
        {
          t3=time_now.tm_min;
          u32_to_float.data16[1]=usRegInputBuf[2923];
          u32_to_float.data16[0]=usRegInputBuf[2924];
          hour = hour+1;
          if(hour/60.0>=u32_to_float.data32)
          {
            get_R_T_flag=1;
            hour=0;
          }
        }
      }		
      if(get_R_T_flag2==0)
      {
        if(tC2!=time_now.tm_min)
        {
          tC2=time_now.tm_min;
          u32_to_float.data16[1]=usRegInputBuf[2947];
          u32_to_float.data16[0]=usRegInputBuf[2948];
          hour2 = hour2+1;
          if((hour2/60.0f)>=u32_to_float.data32)
          {
            get_R_T_flag2=1;
            hour2=0;
          }
        }
      }
    }
    
    
    OSTimeDlyHMSM(0, 0, 0, 10);
  }
}

static  void  App_TaskALARM (void *p_arg)
{
  (void)p_arg;
  //usRegInputBuf[2962] = 0;
  while(1)
  {
    if(Hardware_reset_read == 0)
   {
     AT24CXX_WriteOneByte(299,0X00);
    }
    if(Reset_Rdata)
    {
      Reset_Rdata = 0;
      AT24CXX_Reset_Rdata();
      
    }
    if(usRegInputBuf[2816]!=0)
    {
      if(usRegInputBuf[2962])
      {
        buzer_on;
        OSTimeDlyHMSM(0, 0, 1, 0);
        buzer_off;
        OSTimeDlyHMSM(0, 0, 2, 0);
      }      
    }
    else
    {
      buzer_off;
    }		
   // usRegInputBuf[2816] = 0;
    OSTimeDlyHMSM(0, 0, 1, 0);
  }
}
#endif



/*
*********************************************************************************************************
*                                      EXTI9_5_IRQHandler()
*
* Description : Convert a decimal value to ASCII (with leading zeros).
*
* Argument(s) : s           Pointer to the destination ASCII string.
*               value       Value to convert (assumes an unsigned value).
*               digits      The desired number of digits.
*
* Return(s)   : none.
*********************************************************************************************************
*/
void  EXTI9_5_IRQHandler (void)
{
  extern uint16_t ETH_ReadPHYRegister(uint16_t PHYAddress, uint16_t PHYReg);
  extern void Ethernet_Configuration(void);
  
  if (EXTI_GetITStatus(EXTI_Line5) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line5);
    
    if (!(ETH_ReadPHYRegister(0x01, 1) & ((u16)0x0004)))
    {
      Ethernet_Configuration();
      
    }
  }
}

/*
*********************************************************************************************************
*                                      App_FormatDec()
*
* Description : Convert a decimal value to ASCII (with leading zeros).
*
* Argument(s) : s           Pointer to the destination ASCII string.
*               value       Value to convert (assumes an unsigned value).
*               digits      The desired number of digits.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static  void  App_FormatDec (CPU_INT08U *s, CPU_INT32U value, CPU_INT08U digits)
{
  CPU_INT08U  i;
  CPU_INT32U  mult;
  
  
  mult = 1;
  for (i = 0; i < (digits - 1); i++) {
    mult *= 10;
  }
  while (mult > 0) {
    *s++   = value / mult + '0';
    value %= mult;
    mult  /= 10;
  }
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
  OSProbe_TaskCreateHook(ptcb);
#endif
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
  (void)ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
  OSProbe_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
  (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                        TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && (OS_PROBE_HOOKS_EN > 0)
  OSProbe_TickHook();
#endif
}
#endif
#endif
