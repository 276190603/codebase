#include "main.h"

void main(void)
{
	//step 1.Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the DSP2833x_SysCtrl.c file.
	InitSysCtrl();
	asm(" RPT #8 || NOP");
	DINT;					//缂佸倷鐒﹂娑㈠礂閵娿儳婀板☉鎿冨幗閺岋拷
//#ifdef FLASH
  MemCopy(&RamfuncsLoadStart,&RamfuncsLoadEnd,&RamfuncsRunStart);
   InitFlash();
//#endif
	// Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;
    InitPieCtrl();
    InitPieVectTable();

    EnableInterrupts();		//鐎殿噯鎷稢UP闁告粌顒E濞戞搩鍘介弻锟�
    ERTM;					//鐎殿噯鎷烽悗鍦仦濡炲倹绋夐鐔哥劷

    ResetHardwarePool();	//闁告帗绻傞～鎰板礌閺嶎偀锟芥牗绂掗幆鎵偒婵犙勫姈閻拷

    UseScr();			//閻犲鍟伴弫銈囨媼閹屾У缂佺媴绱曢幃锟�

    myScr.Init();		//闁告帗绻傞～鎰板礌閺嶎剦鍟庡璁规嫹

    myScr.ChangeState(Enable);			//濞达綀鍎婚崗妯兼媼閹屾У

    myFpgaDriver.SpwmOutEnable(Enable);		//濞达綀鍎婚崗妤絇WM閺夊牊鎸搁崵锟�

    myControlSystem.Sample.CalRMSCmd(True);	//濞达綀鍎婚崗姗�寮垫径瀣珡闁稿﹨澹堥鍝ョ不閿燂拷
   // myControlSystem.Controller.State = Enable;	//濞达綀鍎婚崗姗�骞掕閸╂宕抽敓锟�

  //  myCommunication.RS485Com.ChangeState(Enable);
    scr_number_switch(scr_1,0);//
    scr_number_switch(scr_4,1);//
    myScr.SOFTSTART=1;
    myCommunication.CANCom.ChangeState(Enable);
    CAN.SendDataWithPtc(2,AutoSendMailBox);   //发送数据
	while(1)
	{

		//myControlSystem.Sample.Scan();		//闂佹彃娲﹂悧閬嶅箥椤愶絽浼�
		//myPhaseLock.Scan();					//闁烩晠鏅茬紞鍛涢敓钘壝圭�ｎ偄顥囬柟浼欐嫹
      //  myFpgaDriver.Scan();                //FPGA闁规鍋呭鍧楀礄閼恒儲娈�
      //  myFpgaDriver.ReadFpgaRegs();

        if(myScr.SOFTSTART==1)
        {
            //scr_number_switch(scr_1,1);
            //myControlSystem.Sample.Scan();      //闂佹彃娲﹂悧閬嶅箥椤愶絽浼�
           // myPhaseLock.Scan();                 //闁烩晠鏅茬紞鍛涢敓钘壝圭�ｎ偄顥囬柟浼欐嫹
            Uint16 SOFT_START_STATE =  myControlSystem.SoftStart.Run();
            if(SOFT_START_STATE == 2)
            {myScr.SOFTSTART=0;}
        }
        else
        {

            myPhaseLock.Scan();                 //闁烩晠鏅茬紞鍛涢敓钘壝圭�ｎ偄顥囬柟浼欐嫹
            //scr_number_switch(scr_1,0);
            myControlSystem.Sample.Scan();      //闂佹彃娲﹂悧閬嶅箥椤愶絽浼�
           // SOFT_START=1;
        }
         myFpgaDriver.Scan();                //FPGA闁规鍋呭鍧楀礄閼恒儲娈�
         myFpgaDriver.ReadFpgaRegs();
		//myFaultDetect.FaultHandlingScan();	//闁轰礁鎳樺▓鐗堝緞閸曨厽鍊為柟娈垮亝瀵拷
        myCommunication.CANCom.Scan();
	//	myCommunication.RS485Com.Scan();	//485闁规鍋呭鍧楀礄閼恒儲娈�
	//	mySystemTick.ScanCallback();		//婵犲﹤顕悺鐔煎籍閸洘瀵柛銉у仩閻ㄧ喖宕欓懞銉︽
        myFaultDetect.OtherFaultDetScan();
        myFaultDetect.RmsFaultDetectScan();
        myFaultDetect.FaultHandlingScan();    //鏁呴殰澶勭悊鎵弿

	}
}


