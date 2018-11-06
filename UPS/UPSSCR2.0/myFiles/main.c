#include "main.h"

void main(void)
{
	//step 1.Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the DSP2833x_SysCtrl.c file.
	InitSysCtrl();
	asm(" RPT #8 || NOP");
	DINT;					//缁備焦顒涢崗銊ョ湰娑擃厽鏌�
//#ifdef FLASH
   MemCopy(&RamfuncsLoadStart,&RamfuncsLoadEnd,&RamfuncsRunStart);
   InitFlash();
//#endif
	// Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;
    InitPieCtrl();
    InitPieVectTable();

    EnableInterrupts();		//瀵拷CUP閸滃IE娑擃厽鏌�
    ERTM;					//瀵拷鐎圭偞妞傛稉顓熸焽

    ResetHardwarePool();	//閸掓繂顫愰崠鏍�栨禒鎯扮カ濠ф劖鐫�

    UseScr();			//鐠嬪啰鏁ょ拋鎯ь槵缁狅紕鎮�

    myScr.Init();		//閸掓繂顫愰崠鏍啎婢讹拷

    myScr.ChangeState(Enable);			//娴ｈ儻鍏樼拋鎯ь槵

    myFpgaDriver.SpwmOutEnable(Enable);		//娴ｈ儻鍏楽PWM鏉堟挸鍤�

    myControlSystem.Sample.CalRMSCmd(True);	//娴ｈ儻鍏橀張澶嬫櫏閸婅壈顓哥粻锟�
   // myControlSystem.Controller.State = Enable;	//娴ｈ儻鍏橀幒褍鍩楅崳锟�

  //  myCommunication.RS485Com.ChangeState(Enable);
  //  scr_number_switch(scr_1,1);
  //  scr_number_switch(scr_2,1);
    scr_number_switch(scr_3,0);//
    scr_number_switch(scr_4,1);//
    myScr.SOFTSTART=0;
  //  scr_number_switch(scr_4,1);
  //  scr_number_switch(scr_5,1);
   // scr_number_switch(scr_6,1);
    myCommunication.CANCom.ChangeState(Enable);
	while(1)
	{

		//myControlSystem.Sample.Scan();		//闁插洦鐗遍幍顐ｅ伎
		//myPhaseLock.Scan();					//閻╅晲缍呭Λ锟藉ù瀣閹伙拷
      //  myFpgaDriver.Scan();                //FPGA閹殿偅寮块崙鑺ユ殶
      //  myFpgaDriver.ReadFpgaRegs();

        if(myScr.SOFTSTART==1)
        {
            myControlSystem.Sample.Scan();      //闁插洦鐗遍幍顐ｅ伎
            myPhaseLock.Scan();                 //閻╅晲缍呭Λ锟藉ù瀣閹伙拷
        }
        else
        {

            myPhaseLock.Scan();                 //閻╅晲缍呭Λ锟藉ù瀣閹伙拷
            TIMER1_num=0;
            scr_number_switch(scr_3,0);
            myControlSystem.Sample.Scan();      //闁插洦鐗遍幍顐ｅ伎
            SOFT_START=1;
            PID=190;
        }
        /*
        if(myScr.U_DC<170)
            SOFT_START=1;
        */
		//myFaultDetect.FaultHandlingScan();	//閺佸懘娈版径鍕倞閹殿偅寮�
        myCommunication.CANCom.Scan();
	//	myCommunication.RS485Com.Scan();	//485閹殿偅寮块崙鑺ユ殶
	//	mySystemTick.ScanCallback();		//濠婂鐡熼弮鍫曟寭閸ョ偠鐨熼崙鑺ユ殶

		//myPhaseLock.Scan();					//閹规洝骞忛幍顐ｅ伎閸戣姤鏆�

	}
}


