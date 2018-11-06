#include "main.h"

void main(void)
{
	//step 1.Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the DSP2833x_SysCtrl.c file.
	InitSysCtrl();
	asm(" RPT #8 || NOP");
	DINT;					//绂佹鍏ㄥ眬涓柇
	  MemCopy(&RamfuncsLoadStart,&RamfuncsLoadEnd,&RamfuncsRunStart);
	  InitFlash();
	// Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;
    InitPieCtrl();
    InitPieVectTable();
    myInverter.SOFTSTART = 0;
    EnableInterrupts();		//寮�CUP鍜孭IE涓柇
    ERTM;					//寮�瀹炴椂涓柇

    ResetHardwarePool();	//鍒濆鍖栫‖浠惰祫婧愭睜

    UseInverter();			//璋冪敤璁惧绠＄悊

    myInverter.Init();		//鍒濆鍖栬澶�

    myInverter.ChangeState(Enable);			//浣胯兘璁惧



    myControlSystem.Sample.CalRMSCmd(True);	//浣胯兘鏈夋晥鍊艰绠�
   // myControlSystem.Controller.State = Enable;	//浣胯兘鎺у埗鍣�

   // myCommunication.RS485Com.ChangeState(Enable);
    myCommunication.CANCom.ChangeState(Enable);
   // myCommunication.CANCom.ChangeAutoSendState(Enable);
    StartRun();
	while(1)
	{
	    if(myInverter.SOFTSTART==1)
       {
         myFpgaDriver.SpwmOutEnable(Enable);      //浣胯兘SPWM杈撳嚭
         if(SOFT_START == 1)
         {
           if((myInverter.U_DCRMS >= 200)&(myInverter.U_DCRMS <= 230))//200 230
           {
            SOFT_START_STATE =  myControlSystem.SoftStart.Run();
            if(SOFT_START_STATE == 2)
            {
                 SOFT_START = 0;
                 StartRun2();
            }
           }
         }
         else if(SOFT_START == 0)
         {
           myControlSystem.Sample.Scan();      //閲囨牱鎵弿
           myPhaseLock.Scan();                 //鐩镐綅妫�娴嬫壂鎻�

         }
       }
       else
       {
           myControlSystem.Controller.Output = 0.05;
           myFpgaDriver.SpwmOutEnable(Disable);      //浣胯兘SPWM杈撳嚭
           SOFT_START = 1;
           myControlSystem.Sample.Scan();        //閲囨牱鎵弿
           myPhaseLock.Scan();                   //鐩镐綅妫�娴嬫壂鎻�
       }
	//    myCommunication.CANCom.SendData(8,SendMailBox);
	    myCommunication.CANCom.Scan();

	  //  CanScan();
	//	myCommunication.RS485Com.Scan();	//485鎵弿鍑芥暟
		//mySystemTick.ScanCallback();		//婊寸瓟鏃堕挓鍥炶皟鍑芥暟
		myFpgaDriver.Scan();				//FPGA鎵弿鍑芥暟
		//myPhaseLock.Scan();					//鎹曡幏鎵弿鍑芥暟
		myFpgaDriver.ReadFpgaRegs();
		myFaultDetect.OtherFaultDetScan();
		myFaultDetect.RmsFaultDetectScan();
		myFaultDetect.FaultHandlingScan();    //鏁呴殰澶勭悊鎵弿
	}
}


