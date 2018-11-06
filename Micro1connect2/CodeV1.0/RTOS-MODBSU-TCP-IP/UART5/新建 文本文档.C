  for(i=Sensors_len;i<Sensors_len2;i++)
      {	
        rerror_flag = usRegInputBuf[i+2080];
        
        voltage.data32=UART5_Read_V (i+1,CMD_0X60);   	//读取传感器电压
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
        
        usRegInputBuf[2*i+416]=temperature.data16[1];
        usRegInputBuf[2*i+1+416]=temperature.data16[0];
        if(rerror_flag>=0x80)
        {
          rerror_flag=0x80;
        }
        usRegInputBuf[i+2080]=rerror_flag;
        usRegInputBuf[2403] |= usRegInputBuf[i+2080];
        OSTimeDlyHMSM(0, 0, 0, 50);	
        //                         第一通道均衡条件
        //-----------------------------------------------------------------*/
        if(usRegInputBuf[2421])//一键允许均衡
        {
          u32_buf1.data16[1] = usRegInputBuf[2406];//当前电流
          u32_buf1.data16[0] = usRegInputBuf[2407];
          
          u32_buf2.data16[1] = usRegInputBuf[2425];//电流上限
          u32_buf2.data16[0] = usRegInputBuf[2426];
          
          u32_buf3.data16[1] = usRegInputBuf[2427];//电流下线
          u32_buf3.data16[0] = usRegInputBuf[2428];
          if(u32_buf1.data32 > u32_buf2.data32)//停止放电
          {
            UART5_SEND_JUNHENG_V (i+1, 2000);	
          }
          else if((u32_buf1.data32 < u32_buf3.data32)&&(u32_buf1.data32 > 0))//放电
          {
            //                             mytest.data16[1] = usRegInputBuf[2404];//组电压
            //                             mytest.data16[0] = usRegInputBuf[2405];
            mytest.data32 = V_Z;
            u32_buf1.data16[1] = usRegInputBuf[2441];//第一通道浮充电压
            u32_buf1.data16[0] = usRegInputBuf[2442];
            if(18>0)//只数大于0
            {										
              if((mytest.data32/18) > u32_buf1.data32)//组电压均值
              {                             
                UART5_SEND_JUNHENG_V (i+1, (unsigned int)((mytest.data32/18)*100));
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
       if(get_R_T_flag)     
      {
        for(i=Sensors_len;i<Sensors_len2;i++)
        {       
          if(get_R_I_flag)
          {           
            UART5_SEND_CMD (i+1,CMD_0X42);	//读取内阻值	
            OSTimeDlyHMSM(0, 0, 5, 0);
            UART5_SEND_CMD (i+1,CMD_0X22);
            OSTimeDlyHMSM(0, 0, 0, 500);			
            Resistance.data32=UART5_READ_DATA ();
            
            R_now=Resistance.data32;	
            
            if(R_now<=255.937)
            {
              usRegInputBuf[2*i+832]=Resistance.data16[1];
              usRegInputBuf[2*i+1+832]=Resistance.data16[0];												
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
              usRegInputBuf[2*i+1664]=mytest.data16[1];
              usRegInputBuf[2*i+1+1664]=mytest.data16[0];		
              
              
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
              
              mytest.data16[1]=usRegInputBuf[2315];
              mytest.data16[0]=usRegInputBuf[2316];									
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
              
              usRegInputBuf[2*i+1248]=capacity.data16[1];
              usRegInputBuf[2*i+1+1248]=capacity.data16[0];
              
              
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
          usRegInputBuf[i+2080]=rerror_flag;
          usRegInputBuf[2403] |= usRegInputBuf[i+2080];
          
        }
        get_R_T_flag=0;
      }